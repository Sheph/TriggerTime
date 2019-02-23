/*
 * Copyright (c) 2014, Stanislav Vorobiov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "af/AudioOggFile.h"
#include "Logger.h"
#include <boost/detail/endian.hpp>
#include <log4cplus/ndc.h>

namespace af
{
    AudioOggFile::AudioOggFile(std::istream* is, const std::string& name)
    : is_(is),
      name_(name),
      info_(NULL),
      currentSection_(0),
      eof_(false)
    {
        log4cplus::NDCContextCreator ndc(name);

        LOG4CPLUS_DEBUG(afutil::logger(), "Processing...");

        ov_callbacks callbacks;

        callbacks.read_func = &readFunc;
        callbacks.seek_func = NULL;
        callbacks.close_func = NULL;
        callbacks.tell_func = NULL;

        int res = ov_open_callbacks(this, &file_, NULL, 0, callbacks);

        if (res != 0) {
            LOG4CPLUS_ERROR(afutil::logger(), "Error reading \"" << name_ << "\" headers: " << res);

            delete is_;
            is_ = NULL;

            eof_ = true;

            return;
        }

        info_ = ov_info(&file_, -1);
    }

    AudioOggFile::~AudioOggFile()
    {
        close();
    }

    float AudioOggFile::duration() const
    {
        if (!is_) {
            return 0.0f;
        }

        return ov_time_total(&file_, -1);
    }

    void AudioOggFile::close()
    {
        if (!is_) {
            return;
        }

        ov_clear(&file_);

        delete is_;
        is_ = NULL;

        eof_ = true;
    }

    bool AudioOggFile::eof() const
    {
        if (!is_) {
            return true;
        }

        return eof_;
    }

    void AudioOggFile::readIntoBuffer(ALuint buffer, UInt32 maxSize)
    {
        if (!is_) {
            return;
        }

        if (maxSize) {
            maxSize = (maxSize / 16) * 16;
        }

        size_t totalRead = 0;

        buff_.resize(maxSize ? maxSize : chunkSize_);

        int bigendian = 0;

#ifdef BOOST_BIG_ENDIAN
        bigendian = 1;
#endif

        while ((maxSize == 0) || (static_cast<UInt32>(totalRead) < maxSize)) {
            if (totalRead == buff_.size()) {
                buff_.resize(buff_.size() + chunkSize_);
            }

            long chunkSize = ov_read(&file_,
                &buff_[0] + totalRead,
                buff_.size() - totalRead,
                bigendian,
                2, 1, &currentSection_);

            if (chunkSize == 0) {
                break;
            } else if (chunkSize == OV_HOLE) {
                LOG4CPLUS_ERROR(afutil::logger(), "Hole met while reading \"" << name_ << "\"");
            } else if (chunkSize < 0) {
                LOG4CPLUS_ERROR(afutil::logger(), "Error while reading \"" << name_ << "\": " << chunkSize);
                eof_ = true;
                return;
            } else {
                totalRead += chunkSize;
            }
        }

        if (totalRead == 0) {
            eof_ = true;
            return;
        }

        ALenum format;

        if (info_->channels == 1) {
            format = AL_FORMAT_MONO16;
        } else {
            format = AL_FORMAT_STEREO16;
        }

        oal.BufferData(buffer, format, &buff_[0], totalRead, info_->rate);
    }

    size_t AudioOggFile::readFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
    {
        AudioOggFile* oggFile = static_cast<AudioOggFile*>(datasource);

        if (!oggFile->is_->read(reinterpret_cast<char*>(ptr), size * nmemb)) {
            return oggFile->is_->gcount();
        }

        return oggFile->is_->gcount();
    }
}
