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

#include "Level.h"
#include "AssetManager.h"
#include "LRManager.h"

namespace af
{
    Level::Level(const std::string& scriptPath,
                 const std::string& assetPath,
                 float gameWidth, float gameHeight)
    : scene_(new Scene(gameWidth, gameHeight, scriptPath, assetPath)),
      script_(new Script(scriptPath, scene_)),
      assetPath_(assetPath)
    {
    }

    Level::~Level()
    {
        if (scene_) {
            scene_->cleanup();
        }

        lrManager.setLuaState(NULL);

        delete script_;
        delete scene_;
    }

    bool Level::init()
    {
        if (!assetPath_.empty()) {
            SceneAssetPtr asset = assetManager.getSceneAsset(assetPath_);

            if (!asset) {
                return false;
            }

            asset->process(scene_);

            scene_->addInstances(asset->instances());
        }

        scene_->prepare();

        if (!script_->init()) {
            return false;
        }

        lrManager.setLuaState(script_->state());

        if (!script_->run()) {
            return false;
        }

        return true;
    }
}
