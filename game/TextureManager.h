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

#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

#include "af/Single.h"
#include "Texture.h"
#include <boost/thread.hpp>
#include <map>

namespace af
{
    class TextureManager : public Single<TextureManager>
    {
    public:
        TextureManager();
        virtual ~TextureManager();

        bool init();

        void shutdown();

        void reload();

        void renderReload();

        TexturePtr loadTexture(const std::string& path,
                               Texture::WrapMode wrapX,
                               Texture::WrapMode wrapY);

        TexturePtr genTexture(const Byte* data, UInt32 w, UInt32 h,
                              Texture::WrapMode wrapX,
                              Texture::WrapMode wrapY);

    private:
        struct Key
        {
            Key(const std::string& path,
                Texture::WrapMode wrapX,
                Texture::WrapMode wrapY)
            : path(path),
              wrapX(wrapX),
              wrapY(wrapY)
            {
            }

            inline bool operator<(const Key& other) const
            {
                int res = path.compare(other.path);

                if (res != 0) {
                    return (res < 0);
                }

                if (wrapX != other.wrapX) {
                    return wrapX < other.wrapX;
                }

                return wrapY < other.wrapY;
            }

            std::string path;
            Texture::WrapMode wrapX;
            Texture::WrapMode wrapY;
        };

        typedef std::map<Key, TexturePtr> TextureMap;

        boost::mutex m_;
        TextureMap textures_;
    };

    extern TextureManager textureManager;
}

#endif
