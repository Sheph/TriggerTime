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

#include "PlatformAndroid.h"
#include "af/Utils.h"

namespace af
{
    class AndroidAssetStreamBuf : public std::streambuf
    {
    public:
        explicit AndroidAssetStreamBuf(AAsset* asset)
        : asset_(asset),
          buff_(4096)
        {
            setg(&buff_[0], &buff_[0], &buff_[0]);
            setp(&buff_[0], &buff_[0] + buff_.size());
        }

        ~AndroidAssetStreamBuf()
        {
            try {
                overflow(EOF);
            } catch (...) {
            }

            AAsset_close(asset_);
        }

    protected:
        virtual std::streambuf* setbuf(char_type* buf, std::streamsize len)
        {
            /*
             * We don't allow changing the buffer.
             */

            return 0;
        }

        virtual int sync()
        {
            if (overflow(EOF)) {
                return EOF;
            }

            return 0;
        }

        virtual int underflow()
        {
            if (in_avail() == 0) {
                int glen = AAsset_read(asset_, eback(), buff_.size());

                if (glen == 0) {
                    return EOF;
                } else if (glen < 0) {
                    throw std::ios_base::failure("Asset read error");
                }

                setg(eback(), eback(), eback() + glen);
            }

            return static_cast<unsigned char>(*gptr());
        }

        virtual int uflow()
        {
            int c = underflow();

            gbump(1);

            return c;
        }

        virtual int overflow(int c)
        {
            return EOF;
        }

    private:
        AAsset* asset_;
        std::vector<char> buff_;
    };

    PlatformPtr platform(new PlatformAndroid());

    PlatformAndroid::PlatformAndroid()
    : mgr_(NULL)
    {
    }

    PlatformAndroid::~PlatformAndroid()
    {
    }

    bool PlatformAndroid::init(AAssetManager* mgr)
    {
        initTimeUs();
        mgr_ = mgr;
        return true;
    }

    void PlatformAndroid::shutdown()
    {
        mgr_ = NULL;
    }

    std::streambuf* PlatformAndroid::openFile(const std::string& fileName)
    {
        AAsset* asset = AAssetManager_open(mgr_, fileName.c_str(), AASSET_MODE_STREAMING);

        if (!asset) {
            return NULL;
        }

        AndroidAssetStreamBuf* buf = new AndroidAssetStreamBuf(asset);

        return buf;
    }
}
