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

#include "Game.h"
#include "Settings.h"
#include "Logger.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "AudioManager.h"
#include "LRManager.h"
#include "SceneObjectFactory.h"
#include "Platform.h"
#include "UserData.h"
#include "UpgradeManager.h"
#include "GameShell.h"
#include "af/Utils.h"
#include "af/StreamAppConfig.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    template <>
    Single<Game>* Single<Game>::single = NULL;

    static const char* actionIdName[ActionIdMax + 1] =
    {
        "up",
        "down",
        "left",
        "right",
        "primaryFire",
        "secondaryFire",
        "primarySwitch",
        "secondarySwitch",
        "interact",
        "run"
    };

    static const char* actionGamepadIdName[ActionGamepadIdMax + 1] =
    {
        "primaryFire",
        "secondaryFire",
        "primarySwitch",
        "secondarySwitch",
        "interact",
        "run"
    };

    Game::Game()
    : gameWidth_(0.0f),
      gameHeight_(0.0f),
      lastTimeUs_(0),
      numFrames_(0),
      accumRenderTimeUs_(0),
      accumTimeUs_(0),
      lastProfileReportTimeUs_(0),
      gamepadUIPos_(b2Vec2_zero),
      gamepadUIDir_(b2Vec2_zero)
    {
    }

    Game::~Game()
    {
    }

    bool Game::init(const std::string& startScript,
        const std::string& startAsset)
    {
        setObjectPoolMaxBytes(settings.objectPool.maxBytes);
        setObjectPoolWarnThreshold(settings.objectPool.warnThreshold);
        setObjectPoolReportTimeoutMs(settings.objectPool.reportTimeoutMs);

        gameWidth_ = settings.gameHeight * settings.viewAspect;
        gameHeight_ = settings.gameHeight;

        gamepadUIPos_ = b2Vec2(settings.layoutWidth / 2.0f, settings.layoutHeight / 2.0f);

        if (!assetManager.init()) {
            return false;
        }

        if (!renderer.init()) {
            return false;
        }

        if (!textureManager.init()) {
            return false;
        }

        if (!inputManager.init()) {
            return false;
        }

        LOG4CPLUS_DEBUG(logger(), "Supported desktop video modes:");

        int i = 0;

        for (std::vector<VideoMode>::const_iterator it = platform->desktopVideoModes().begin();
            it != platform->desktopVideoModes().end(); ++it) {
            LOG4CPLUS_DEBUG(logger(), i << " = " << it->width << "x" << it->height);
            ++i;
        }

        LOG4CPLUS_DEBUG(logger(), "Supported windowed video modes:");

        i = 0;

        for (std::vector<VideoMode>::const_iterator it = platform->winVideoModes().begin();
            it != platform->winVideoModes().end(); ++it) {
            LOG4CPLUS_DEBUG(logger(), i << " = " << it->width << "x" << it->height);
            ++i;
        }

        LOG4CPLUS_DEBUG(logger(), "Supported MSAA modes:");

        i = 0;

        for (std::vector<UInt32>::const_iterator it = platform->msaaModes().begin();
            it != platform->msaaModes().end(); ++it) {
            LOG4CPLUS_DEBUG(logger(), i << " = x" << *it);
            ++i;
        }

        LOG4CPLUS_DEBUG(logger(), "Default windowed video mode: " << platform->winVideoModes()[platform->defaultVideoMode()].width << "x" << platform->winVideoModes()[platform->defaultVideoMode()].height);

        if (platform->desktopVideoMode() < 0) {
            LOG4CPLUS_WARN(logger(), "Fullscreen mode not supported!");
        } else {
            LOG4CPLUS_DEBUG(logger(), "Desktop video mode: " << platform->desktopVideoModes()[platform->desktopVideoMode()].width << "x" << platform->desktopVideoModes()[platform->desktopVideoMode()].height);
        }

        if (!platform->vsyncSupported()) {
            LOG4CPLUS_WARN(logger(), "vsync not supported!");
        }

        boost::shared_ptr<af::StreamAppConfig> userConfig;

        std::string userConfigData = platform->readUserConfig();

        if (!userConfigData.empty()) {
            userConfig = boost::make_shared<af::StreamAppConfig>();

            std::istringstream is(userConfigData);

            if (!userConfig->load(is)) {
                LOG4CPLUS_WARN(logger(), "Error parsing user config file!");
                userConfig.reset();
            }
        }

        if (!userConfig || !setupControls(*userConfig)) {
            inputManager.binding(ActionIdMoveUp)->ib(0)->setKey(KI_W);
            inputManager.binding(ActionIdMoveUp)->ib(1)->setKey(KI_UP);
            inputManager.binding(ActionIdMoveDown)->ib(0)->setKey(KI_S);
            inputManager.binding(ActionIdMoveDown)->ib(1)->setKey(KI_DOWN);
            inputManager.binding(ActionIdMoveLeft)->ib(0)->setKey(KI_A);
            inputManager.binding(ActionIdMoveLeft)->ib(1)->setKey(KI_LEFT);
            inputManager.binding(ActionIdMoveRight)->ib(0)->setKey(KI_D);
            inputManager.binding(ActionIdMoveRight)->ib(1)->setKey(KI_RIGHT);
            inputManager.binding(ActionIdPrimaryFire)->ib(0)->setMb(true);
            inputManager.binding(ActionIdSecondaryFire)->ib(0)->setMb(false);
            inputManager.binding(ActionIdPrimarySwitch)->ib(0)->setKey(KI_1);
            inputManager.binding(ActionIdSecondarySwitch)->ib(0)->setKey(KI_2);
            inputManager.binding(ActionIdInteract)->ib(0)->setKey(KI_SPACE);
            inputManager.binding(ActionIdInteract)->ib(1)->setKey(KI_RETURN);
            inputManager.binding(ActionIdRun)->ib(0)->setKey(KI_LSHIFT);
        }

        if (!userConfig || !setupGamepad(*userConfig)) {
            inputManager.gamepadBinding(ActionGamepadIdPrimaryFire)->ib(0)->setButton(GamepadRightTrigger);
            inputManager.gamepadBinding(ActionGamepadIdSecondaryFire)->ib(0)->setButton(GamepadLeftTrigger);
            inputManager.gamepadBinding(ActionGamepadIdPrimarySwitch)->ib(0)->setButton(GamepadRightBumper);
            inputManager.gamepadBinding(ActionGamepadIdSecondarySwitch)->ib(0)->setButton(GamepadLeftBumper);
            inputManager.gamepadBinding(ActionGamepadIdInteract)->ib(0)->setButton(GamepadA);
            inputManager.gamepadBinding(ActionGamepadIdInteract)->ib(1)->setButton(GamepadB);
            inputManager.gamepadBinding(ActionGamepadIdRun)->ib(0)->setButton(GamepadLeftStick);
        }

        if (userConfig) {
            if (!setupVideo(*userConfig)) {
                writeUserConfig(true);
                return false;
            }
        } else {
            if (!platform->changeVideoMode(false, platform->defaultVideoMode(), 0, platform->vsyncSupported(), false)) {
                return false;
            }
        }

        if (!audio.init()) {
            return false;
        }

        if (userConfig) {
            setupAudio(*userConfig);
        }

        if (!sceneObjectFactory.init()) {
            return false;
        }

        if (!lrManager.init()) {
            return false;
        }

        if (!userData.init()) {
            return false;
        }

        if (!upgradeManager.init()) {
            return false;
        }

        return loadLevel(startScript, startAsset);
    }

    void Game::suspend()
    {
        LOG4CPLUS_DEBUG(logger(), "suspending...");
        audio.suspend();
    }

    void Game::reload()
    {
        renderer.advanceGeneration();
        textureManager.reload();
        gameShell->reload();
        assetManager.reload();
        if (lrManager.initialized()) {
            lrManager.reload();
            mouseMove(b2Vec2(0, 0));
        }
    }

    void Game::renderReload()
    {
        LOG4CPLUS_DEBUG(logger(), "reloading textures...");
        textureManager.renderReload();
        LOG4CPLUS_DEBUG(logger(), "reloading renderer...");
        renderer.reload();
    }

    bool Game::update()
    {
        static std::string scriptPath;
        static std::string assetPath;

        UInt64 timeUs = getTimeUs();
        UInt32 deltaUs;

        if (lastTimeUs_ == 0) {
            lastProfileReportTimeUs_ = timeUs;
            deltaUs = 16000; // pretend that very first frame lasted 16ms
        } else {
            deltaUs = static_cast<UInt32>(timeUs - lastTimeUs_);
        }

        lastTimeUs_ = timeUs;

        float dt = static_cast<float>(deltaUs) / 1000000.0f;

        if (gamepadUIDir_ != b2Vec2_zero) {
            if (lrManager.activeContext()) {
                gamepadUIPos_.x += gamepadUIDir_.x * dt * 400.0f;
                gamepadUIPos_.y -= gamepadUIDir_.y * dt * 400.0f;

                if (gamepadUIPos_.x < 0) {
                    gamepadUIPos_.x = 0;
                } else if (gamepadUIPos_.x > (settings.layoutWidth - 32.0f)) {
                    gamepadUIPos_.x = settings.layoutWidth - 32.0f;
                }

                if (gamepadUIPos_.y < 0) {
                    gamepadUIPos_.y = 0;
                } else if (gamepadUIPos_.y > (settings.layoutHeight - 32.0f)) {
                    gamepadUIPos_.y = settings.layoutHeight - 32.0f;
                }

                lrManager.activeContext()->ProcessMouseMove(
                    gamepadUIPos_.x,
                    gamepadUIPos_.y,
                    0);
            }
        }

        level_->scene()->update(dt);

        UInt64 timeUs2 = getTimeUs();

        accumRenderTimeUs_ += static_cast<UInt32>(timeUs2 - timeUs);
        accumTimeUs_ += deltaUs;
        ++numFrames_;

        if ((timeUs2 - lastProfileReportTimeUs_) > settings.profileReportTimeoutMs * 1000) {
            lastProfileReportTimeUs_ = timeUs2;

            LOG4CPLUS_TRACE(logger(),
                            "FPS: " << (numFrames_ * 1000000) / accumTimeUs_
                            << " Time: " << accumRenderTimeUs_ / (numFrames_ * 1000));

            accumRenderTimeUs_ = 0;
            accumTimeUs_ = 0;
            numFrames_ = 0;
        }

        if (level_->scene()->quit()) {
            level_->scene()->setQuit(false);
            return false;
        }

        if (level_->scene()->getNextLevel(scriptPath, assetPath)) {
            if (!loadLevel(scriptPath, assetPath)) {
                return false;
            }
        }

        return true;
    }

    bool Game::render()
    {
        return renderer.update();
    }

    void Game::cancelUpdate()
    {
        renderer.cancelSwap();
    }

    void Game::cancelRender()
    {
        renderer.cancelUpdate();
    }

    void Game::shutdown()
    {
        upgradeManager.shutdown();

        userData.shutdown();

        writeUserConfig(false);

        level_.reset();

        lrManager.shutdown();

        sceneObjectFactory.shutdown();

        inputManager.shutdown();

        textureManager.shutdown();

        audio.shutdown();

        renderer.shutdown();

        assetManager.shutdown();
    }

    void Game::keyPress(KeyIdentifier ki)
    {
        inputManager.keyboard().press(ki);

        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessKeyDown(ki, 0);
        }
    }

    void Game::keyRelease(KeyIdentifier ki)
    {
        inputManager.keyboard().release(ki);

        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessKeyUp(ki, 0);
        }
    }

    void Game::touchDown(int finger, const b2Vec2& point)
    {
        b2Vec2 pt = point - b2Vec2(settings.viewX, settings.viewY);

        b2Vec2 tmp((pt.x * gameWidth_) / settings.viewWidth,
            gameHeight_ - (pt.y * gameHeight_) / settings.viewHeight);

        inputManager.touchScreen().press(finger, tmp);
    }

    void Game::touchUp(int finger)
    {
        inputManager.touchScreen().release(finger);
    }

    void Game::mouseDown(bool left)
    {
        inputManager.mouse().press(left);

        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessMouseButtonDown(left ? 0 : 1, 0);
        }
    }

    void Game::mouseUp(bool left)
    {
        inputManager.mouse().release(left);

        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessMouseButtonUp(left ? 0 : 1, 0);
        }
    }

    void Game::mouseWheel(int delta)
    {
        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessMouseWheel(delta, 0);
        }
    }

    void Game::mouseMove(const b2Vec2& point)
    {
        b2Vec2 pt = point - b2Vec2(settings.viewX, settings.viewY);

        b2Vec2 tmp((pt.x * gameWidth_) / settings.viewWidth,
            gameHeight_ - (pt.y * gameHeight_) / settings.viewHeight);

        inputManager.mouse().move(tmp);

        if (lrManager.activeContext()) {
            lrManager.activeContext()->ProcessMouseMove(
                pt.x * settings.layoutWidth / settings.viewWidth,
                pt.y * settings.layoutHeight / settings.viewHeight,
                0);
        }
    }

    void Game::gamepadMoveStick(bool left, const b2Vec2& value)
    {
        inputManager.gamepad().moveStick(left, value);

        if (left) {
            if (lrManager.activeContext()) {
                gamepadUIDir_ = inputManager.gamepad().pos(true);
            } else {
                gamepadUIDir_ = b2Vec2_zero;
            }
        }
    }

    void Game::gamepadMoveTrigger(bool left, float value)
    {
        inputManager.gamepad().moveTrigger(left, value);
    }

    void Game::gamepadPress(GamepadButton button)
    {
        inputManager.gamepad().press(button);

        if (lrManager.activeContext()) {
            if (button == GamepadA) {
                lrManager.activeContext()->ProcessMouseButtonDown(0, 0);
            } else if ((button == GamepadBack) || (button == GamepadB)) {
                lrManager.activeContext()->ProcessKeyDown(KI_ESCAPE, 0);
            }
        }
    }

    void Game::gamepadRelease(GamepadButton button)
    {
        inputManager.gamepad().release(button);

        if (lrManager.activeContext()) {
            if (button == GamepadA) {
                lrManager.activeContext()->ProcessMouseButtonUp(0, 0);
            } else if ((button == GamepadBack) || (button == GamepadB)) {
                lrManager.activeContext()->ProcessKeyUp(KI_ESCAPE, 0);
            }
        }
    }

    bool Game::loadLevel(const std::string& scriptPath,
                         const std::string& assetPath)
    {
        level_.reset();

        audio.stopAll();

        LOG4CPLUS_INFO(logger(), "loading level (\"" << scriptPath << "\", \"" << assetPath << "\")...");

        LevelPtr level = boost::make_shared<Level>(scriptPath, assetPath,
                                                   gameWidth_, gameHeight_);

        if (!level->init()) {
            return false;
        }

        level_ = level;

        LOG4CPLUS_INFO(logger(), "level loaded");

        return true;
    }

    bool Game::setupVideo(const AppConfig& userConfig)
    {
        bool fullscreen = false;
        int videoMode = -1;
        int msaaMode = -1;
        bool vsync = false;
        bool trilinearFilter = false;
        bool failed = false;

        if (userConfig.haveKey("video.gamma")) {
            float gamma = userConfig.getFloat("video.gamma");
            if ((gamma < 0.5f) || (gamma > 1.5f)) {
                gamma = 1.0f;
            }
            settings.gamma = gamma;
        }

        if (userConfig.haveKey("video.fullscreen")) {
            fullscreen = userConfig.getBool("video.fullscreen");
        } else {
            failed = true;
        }

        if (userConfig.haveKey("video.mode")) {
            b2Vec2 res = userConfig.getVec2("video.mode");

            std::vector<VideoMode> modes;

            if (fullscreen) {
                modes = platform->desktopVideoModes();
            } else {
                modes = platform->winVideoModes();
            }

            VideoMode m(res.x, res.y);

            for (size_t i = 0; i < modes.size(); ++i) {
                if (m == modes[i]) {
                    videoMode = i;
                    break;
                }
            }

            if (videoMode == -1) {
                failed = true;
            }
        } else {
            failed = true;
        }

        if (userConfig.haveKey("video.msaa")) {
            UInt32 msaa = userConfig.getInt("video.msaa");

            for (size_t i = 0; i < platform->msaaModes().size(); ++i) {
                if (msaa == platform->msaaModes()[i]) {
                    msaaMode = i;
                    break;
                }
            }

            if (msaaMode == -1) {
                failed = true;
            }
        } else {
            failed = true;
        }

        if (userConfig.haveKey("video.vsync")) {
            vsync = userConfig.getBool("video.vsync");
            if (vsync && !platform->vsyncSupported()) {
                failed = true;
            }
        } else {
            failed = true;
        }

        if (userConfig.haveKey("video.trilinear")) {
            trilinearFilter = userConfig.getBool("video.trilinear");
        } else {
            failed = true;
        }

        if (failed) {
            return platform->changeVideoMode(false, platform->defaultVideoMode(), 0, platform->vsyncSupported(), false);
        } else {
            return platform->changeVideoMode(fullscreen, videoMode, msaaMode, vsync, trilinearFilter);
        }
    }

    void Game::setupAudio(const AppConfig& userConfig)
    {
        if (userConfig.haveKey("audio.sound")) {
            float v = userConfig.getFloat("audio.sound");
            if ((v >= 0.0f) && (v <= 1.0f)) {
                audio.setSoundVolume(v);
            }
        }

        if (userConfig.haveKey("audio.music")) {
            float v = userConfig.getFloat("audio.music");
            if ((v >= 0.0f) && (v <= 1.0f)) {
                audio.setMusicVolume(v);
            }
        }
    }

    bool Game::setupControls(const AppConfig& userConfig)
    {
        for (int i = 0; i <= ActionIdMax; ++i) {
            if (!userConfig.haveKey(std::string("controls.") + actionIdName[i] + ".0")) {
                return false;
            }
            if (!userConfig.haveKey(std::string("controls.") + actionIdName[i] + ".1")) {
                return false;
            }

            std::string v = userConfig.getString(std::string("controls.") + actionIdName[i] + ".0");

            if (!v.empty()) {
                KeyIdentifier ki = KI_UNKNOWN;
                bool left = false;

                if (InputBinding::stringToKey(v, ki)) {
                    inputManager.binding(static_cast<ActionId>(i))->ib(0)->setKey(ki);
                } else if (InputBinding::stringToMb(v, left)) {
                    inputManager.binding(static_cast<ActionId>(i))->ib(0)->setMb(left);
                } else {
                    return false;
                }
            }

            v = userConfig.getString(std::string("controls.") + actionIdName[i] + ".1");

            if (!v.empty()) {
                KeyIdentifier ki = KI_UNKNOWN;
                bool left = false;

                if (InputBinding::stringToKey(v, ki)) {
                    inputManager.binding(static_cast<ActionId>(i))->ib(1)->setKey(ki);
                } else if (InputBinding::stringToMb(v, left)) {
                    inputManager.binding(static_cast<ActionId>(i))->ib(1)->setMb(left);
                } else {
                    return false;
                }
            }
        }

        return true;
    }

    bool Game::setupGamepad(const AppConfig& userConfig)
    {
        for (int i = 0; i <= ActionGamepadIdMax; ++i) {
            if (!userConfig.haveKey(std::string("gamepad.") + actionGamepadIdName[i] + ".0")) {
                return false;
            }
            if (!userConfig.haveKey(std::string("gamepad.") + actionGamepadIdName[i] + ".1")) {
                return false;
            }

            std::string v = userConfig.getString(std::string("gamepad.") + actionGamepadIdName[i] + ".0");

            if (!v.empty()) {
                GamepadButton button = GamepadUnknown;

                if (InputGamepadBinding::stringToButton(v, button)) {
                    inputManager.gamepadBinding(static_cast<ActionGamepadId>(i))->ib(0)->setButton(button);
                } else {
                    return false;
                }
            }

            v = userConfig.getString(std::string("gamepad.") + actionGamepadIdName[i] + ".1");

            if (!v.empty()) {
                GamepadButton button = GamepadUnknown;

                if (InputGamepadBinding::stringToButton(v, button)) {
                    inputManager.gamepadBinding(static_cast<ActionGamepadId>(i))->ib(1)->setButton(button);
                } else {
                    return false;
                }
            }
        }

        if (userConfig.haveKey("gamepad.stickDeadzone")) {
            float v = userConfig.getFloat("gamepad.stickDeadzone");
            if ((v >= 0.01f) && (v <= 0.99f)) {
                inputManager.gamepad().setStickDeadzone(v);
            }
        }

        if (userConfig.haveKey("gamepad.triggerDeadzone")) {
            float v = userConfig.getFloat("gamepad.triggerDeadzone");
            if ((v >= 0.01f) && (v <= 0.99f)) {
                inputManager.gamepad().setTriggerDeadzone(v);
            }
        }

        return true;
    }

    void Game::writeUserConfig(bool controlsOnly)
    {
        std::ostringstream os;

        os << "[controls]" << std::endl;

        for (int i = 0; i <= ActionIdMax; ++i) {
            os << actionIdName[i] << ".0=" << inputManager.binding(static_cast<ActionId>(i))->ib(0)->str() << std::endl;
            os << actionIdName[i] << ".1=" << inputManager.binding(static_cast<ActionId>(i))->ib(1)->str() << std::endl;
        }

        os << std::endl << "[gamepad]" << std::endl;

        for (int i = 0; i <= ActionGamepadIdMax; ++i) {
            os << actionGamepadIdName[i] << ".0=" << inputManager.gamepadBinding(static_cast<ActionGamepadId>(i))->ib(0)->str() << std::endl;
            os << actionGamepadIdName[i] << ".1=" << inputManager.gamepadBinding(static_cast<ActionGamepadId>(i))->ib(1)->str() << std::endl;
        }
        os << "stickDeadzone=" << inputManager.gamepad().stickDeadzone() << std::endl;
        os << "triggerDeadzone=" << inputManager.gamepad().triggerDeadzone() << std::endl;

        if (!controlsOnly) {
            os << std::endl;
            os << "[video]" << std::endl;
            os << "fullscreen=" << (settings.fullscreen ? "true" : "false") << std::endl;

            VideoMode vm;

            if (settings.fullscreen) {
                vm = platform->desktopVideoModes()[settings.videoMode];
            } else {
                vm = platform->winVideoModes()[settings.videoMode];
            }

            os << "mode=" << vm.width << "," << vm.height << std::endl;
            os << "msaa=" << platform->msaaModes()[settings.msaaMode] << std::endl;
            os << "vsync=" << (settings.vsync ? "true" : "false") << std::endl;
            os << "trilinear=" << (settings.trilinearFilter ? "true" : "false") << std::endl;
            os << "gamma=" << settings.gamma << std::endl;

            os << std::endl;
            os << "[audio]" << std::endl;

            os << "sound=" << audio.soundVolume() << std::endl;
            os << "music=" << audio.musicVolume() << std::endl;
        }

        platform->writeUserConfig(os.str());
    }
}
