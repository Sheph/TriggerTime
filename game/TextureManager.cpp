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

#include "TextureManager.h"
#include "Renderer.h"
#include "Platform.h"
#include "af/PNGDecoder.h"
#include <boost/make_shared.hpp>

namespace af
{
    TextureManager textureManager;

    template <>
    Single<TextureManager>* Single<TextureManager>::single = NULL;

    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
    }

    bool TextureManager::init()
    {
        return true;
    }

    void TextureManager::shutdown()
    {
        textures_.clear();
    }

    void TextureManager::reload()
    {
        for (TextureMap::iterator it = textures_.begin();
             it != textures_.end();) {
            if (it->second.use_count() == 1) {
                textures_.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void TextureManager::renderReload()
    {
        boost::mutex::scoped_lock lock(m_);

        for (TextureMap::const_iterator it = textures_.begin();
             it != textures_.end();
             ++it) {
            renderer.uploadTexture(it->second, it->first.path, false);
        }
    }

    TexturePtr TextureManager::loadTexture(const std::string& path,
                                           Texture::WrapMode wrapX,
                                           Texture::WrapMode wrapY)
    {
        Key key(path, wrapX, wrapY);

        TextureMap::iterator it = textures_.find(key);

        if (it != textures_.end()) {
            return it->second;
        }

        PlatformIFStream is(path);

        PNGDecoder decoder(path, is);

        TexturePtr texture;

        if (decoder.init()) {
            texture = boost::make_shared<Texture>(decoder.width(),
                                                  decoder.height(),
                                                  wrapX,
                                                  wrapY);

            renderer.uploadTexture(texture, path, true);
        }

        boost::mutex::scoped_lock lock(m_);

        textures_[key] = texture;

        return texture;
    }

    TexturePtr TextureManager::genTexture(const Byte* data, UInt32 w, UInt32 h,
        Texture::WrapMode wrapX,
        Texture::WrapMode wrapY)
    {
        TexturePtr texture = boost::make_shared<Texture>(w, h, wrapX, wrapY);

        renderer.genTexture(texture, data);

        return texture;
    }
}
