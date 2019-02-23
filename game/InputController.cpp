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

#include "InputController.h"
#include "InputManager.h"
#include "Settings.h"

namespace af
{
    InputController::InputController()
    : gameDebugPressed_(false),
      physicsDebugPressed_(false),
      slowmoPressed_(false),
      cullPressed_(false)
    {
    }

    InputController::~InputController()
    {
    }

    void InputController::update(float dt)
    {
        if (settings.debugKeys) {
            if (inputManager.keyboard().triggered(KI_R)) {
                gameDebugPressed_ = !gameDebugPressed_;
            }
            if (inputManager.keyboard().triggered(KI_P)) {
                physicsDebugPressed_ = !physicsDebugPressed_;
            }
            if (inputManager.keyboard().triggered(KI_M)) {
                slowmoPressed_ = !slowmoPressed_;
            }
            if (inputManager.keyboard().triggered(KI_C)) {
                cullPressed_ = !cullPressed_;
            }
        }

        if (updateContext(menuUI(), dt)) {
            return;
        }

        updateContext(cutscene(), dt);
        updateContext(gameUI(), dt);
        updateContext(player(), dt);
        updateContext(roboArm(), dt);
        updateContext(boat(), dt);
        updateContext(mech(), dt);
        updateContext(turret(), dt);
    }

    bool InputController::pausePressed() const
    {
        return !menuUI()->active() && (inputManager.keyboard().triggered(KI_ESCAPE) || inputManager.gamepad().triggered(GamepadStart));
    }

    bool InputController::zoomInPressed() const
    {
        if (settings.debugKeys) {
            return inputManager.keyboard().pressed(KI_ADD);
        } else {
            return false;
        }
    }

    bool InputController::zoomOutPressed() const
    {
        if (settings.debugKeys) {
            return inputManager.keyboard().pressed(KI_SUBTRACT);
        } else {
            return false;
        }
    }

    bool InputController::zoomResetPressed() const
    {
        if (settings.debugKeys) {
            return inputManager.keyboard().pressed(KI_MULTIPLY);
        } else {
            return false;
        }
    }

    bool InputController::updateContext(InputContext* c, float dt)
    {
        if (c->active()) {
            c->update(dt);
            return true;
        }
        return false;
    }
}
