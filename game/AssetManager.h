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

#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include "af/TPS.h"
#include "af/RUBEWorld.h"
#include "af/GdxP.h"
#include "af/Single.h"
#include "Image.h"
#include "Drawable.h"
#include "SceneObjectAsset.h"
#include "SceneAsset.h"
#include "ParticleEffectComponent.h"
#include "Animation.h"
#include <map>

namespace af
{
    typedef std::map<UInt16, Image> CharMap;

    class AssetManager : public Single<AssetManager>
    {
    public:
        AssetManager();
        ~AssetManager();

        bool init();

        void shutdown();

        void reload();

        Image getBadImage();

        Image getImage(const std::string& name,
                       Texture::WrapMode wrapX = Texture::WrapModeRepeat,
                       Texture::WrapMode wrapY = Texture::WrapModeRepeat);

        DrawablePtr getBadDrawable();

        DrawablePtr getDrawable(const std::string& name,
                                Texture::WrapMode wrapX = Texture::WrapModeRepeat,
                                Texture::WrapMode wrapY = Texture::WrapModeRepeat);

        SceneObjectAssetPtr getSceneObjectAsset(const std::string& name);

        SceneAssetPtr getSceneAsset(const std::string& name);

        ParticleEffectComponentPtr getParticleEffect(const std::string& name,
                                                     const b2Vec2& pos,
                                                     float angle,
                                                     bool fixedPos = false);

        Animation getAnimation(const std::string& name, float factor = 1.0f);

        const CharMap& charMap();

        void assetTouched(const std::string& name);

    private:
        typedef std::map<std::string, TPSPtr> TPSMap;
        typedef std::map<std::string, RUBEWorldPtr> WorldMap;
        typedef std::map<std::string, GdxPPtr> GdxPMap;
        typedef std::map<std::string, Animation> AnimationMap;

        TPSMap tpsMap_;
        WorldMap worldMap_;
        GdxPMap gdxpMap_;
        CharMap charMap_;
        AnimationMap animationMap_;
    };

    extern AssetManager assetManager;
}

#endif
