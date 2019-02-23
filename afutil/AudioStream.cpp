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

#include "af/AudioStream.h"
#include "Logger.h"

namespace af
{
    AudioStream::AudioStream(AudioDevice* device, const AudioStreamDataPtr& data,
        UInt32 numBuffers, UInt32 bufferSize)
    : AudioSourceImpl(true, device),
      data_(data),
      numBuffers_(numBuffers),
      bufferSize_(bufferSize)
    {
    }

    AudioStream::~AudioStream()
    {
    }

    void AudioStream::update()
    {
        boost::mutex::scoped_lock lock(m_);

        if (buffers_.empty()) {
            return;
        }

        ALint numProcessed = 0;

        oal.GetSourceiv(source(), AL_BUFFERS_PROCESSED, &numProcessed);

        bool enqueued = false;

        while (numProcessed > 0) {
            ALuint buffer;

            oal.SourceUnqueueBuffers(source(), 1, &buffer);

            audioFile_->readIntoBuffer(buffer, bufferSize_);

            if (loop() && audioFile_->eof()) {
                audioFile_->close();
                audioFile_ = data_->openFile();
                audioFile_->readIntoBuffer(buffer, bufferSize_);
            }

            if (audioFile_->eof()) {
                ALint bufferCount = 0;

                oal.GetSourceiv(source(), AL_BUFFERS_QUEUED, &bufferCount);

                if (!enqueued && (bufferCount == 0)) {
                    setStopped();
                }
                break;
            } else {
                oal.SourceQueueBuffers(source(), 1, &buffer);
                enqueued = true;
            }

            --numProcessed;
        }

        if (enqueued) {
            ALint state = AL_INITIAL;

            oal.GetSourceiv(source(), AL_SOURCE_STATE, &state);

            if (state == AL_STOPPED) {
                LOG4CPLUS_WARN(afutil::logger(), "Audio stream underflow, force playing...");
                oal.SourcePlay(source());
            }
        }
    }

    void AudioStream::doSetLoop(bool value)
    {
    }

    void AudioStream::doPlay(bool needPrepare)
    {
        boost::mutex::scoped_lock lock(m_);

        if (audioFile_) {
            audioFile_->close();
        }

        audioFile_ = data_->openFile();

        if (buffers_.empty()) {
            for (UInt32 i = 0; i < numBuffers_; ++i) {
                ALuint tmp;
                oal.GenBuffers(1, &tmp);
                buffers_.push_back(tmp);
            }
        } else {
            oal.SourceStop(source());

            ALint bufferCount = 0;

            oal.GetSourceiv(source(), AL_BUFFERS_QUEUED, &bufferCount);

            ALuint tmp = 0;

            for (ALint i = 0; i < bufferCount; ++i) {
                oal.SourceUnqueueBuffers(source(), 1, &tmp);
            }
        }

        bool enqueued = false;

        for (std::vector<ALuint>::const_iterator it = buffers_.begin();
             it != buffers_.end(); ++it) {
            audioFile_->readIntoBuffer(*it, bufferSize_);
            if (audioFile_->eof()) {
                break;
            }
            oal.SourceQueueBuffers(source(), 1, &(*it));
            enqueued = true;
        }

        if (enqueued) {
            oal.SourcePlay(source());
        } else {
            for (std::vector<ALuint>::const_iterator it = buffers_.begin();
                 it != buffers_.end(); ++it) {
                oal.DeleteBuffers(1, &(*it));
            }
            buffers_.clear();
            audioFile_->close();
            audioFile_.reset();
        }
    }

    void AudioStream::doPause()
    {
        boost::mutex::scoped_lock lock(m_);

        oal.SourcePause(source());
    }

    void AudioStream::doStop()
    {
        boost::mutex::scoped_lock lock(m_);

        oal.SourceStop(source());

        setStopped();
    }

    void AudioStream::doDetach()
    {
        if (audioFile_) {
            audioFile_->close();
            audioFile_.reset();
        }
    }

    AudioSource::Status AudioStream::doStatus() const
    {
        boost::mutex::scoped_lock lock(m_);

        if (buffers_.empty()) {
            return AudioSource::Stopped;
        }

        ALint state = AL_INITIAL;

        oal.GetSourceiv(source(), AL_SOURCE_STATE, &state);

        return (state == AL_PAUSED) ? AudioSource::Paused
            : AudioSource::Playing;
    }

    void AudioStream::setStopped()
    {
        ALint bufferCount = 0;

        oal.GetSourceiv(source(), AL_BUFFERS_QUEUED, &bufferCount);

        ALuint tmp = 0;

        for (ALint i = 0; i < bufferCount; ++i) {
            oal.SourceUnqueueBuffers(source(), 1, &tmp);
        }

        for (std::vector<ALuint>::const_iterator it = buffers_.begin();
             it != buffers_.end(); ++it) {
            oal.DeleteBuffers(1, &(*it));
        }
        buffers_.clear();
        audioFile_->close();
        audioFile_.reset();
    }
}
