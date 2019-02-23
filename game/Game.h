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

#ifndef _GAME_H_
#define _GAME_H_

#include "af/Types.h"
#include "af/Single.h"
#include "af/AppConfig.h"
#include "Level.h"
#include "InputManager.h"

namespace af
{
    class Game : public Single<Game>
    {
    public:
        Game();
        ~Game();

        bool init(const std::string& startScript = "logo.lua",
            const std::string& startAsset = "");

        void suspend();

        void reload();

        void renderReload();

        bool update();

        bool render();

        void cancelUpdate();

        void cancelRender();

        void shutdown();

        void keyPress(KeyIdentifier ki);

        void keyRelease(KeyIdentifier ki);

        void touchDown(int finger, const b2Vec2& point);

        void touchUp(int finger);

        void mouseDown(bool left);

        void mouseUp(bool left);

        void mouseWheel(int delta);

        void mouseMove(const b2Vec2& point);

        void gamepadMoveStick(bool left, const b2Vec2& value);

        void gamepadMoveTrigger(bool left, float value);

        void gamepadPress(GamepadButton button);

        void gamepadRelease(GamepadButton button);

    private:
        bool loadLevel(const std::string& scriptPath,
                       const std::string& assetPath);

        bool setupVideo(const AppConfig& userConfig);

        void setupAudio(const AppConfig& userConfig);

        bool setupControls(const AppConfig& userConfig);

        bool setupGamepad(const AppConfig& userConfig);

        void writeUserConfig(bool controlsOnly);

        float gameWidth_;
        float gameHeight_;

        LevelPtr level_;

        UInt64 lastTimeUs_;
        UInt32 numFrames_;
        UInt32 accumRenderTimeUs_;
        UInt32 accumTimeUs_;
        UInt64 lastProfileReportTimeUs_;

        b2Vec2 gamepadUIPos_;
        b2Vec2 gamepadUIDir_;
    };
}

#endif
