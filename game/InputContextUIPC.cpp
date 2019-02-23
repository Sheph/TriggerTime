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

#include "InputContextUIPC.h"
#include "InputManager.h"
#include "LRManager.h"
#include "Settings.h"
#include "Scene.h"

namespace af
{
    InputContextUIPC::InputContextUIPC(Scene* scene, Rocket::Core::Context* rc)
    : InputContextUI(scene, rc)
    {
    }

    InputContextUIPC::~InputContextUIPC()
    {
        if (rc()) {
            rc()->ShowMouseCursor(false);
        }
        lrManager.setActiveContext(NULL);
    }

    void InputContextUIPC::update(float dt)
    {
    }

    bool InputContextUIPC::leftPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveLeft)->pressed() || inputManager.gamepad().moveLeft();
    }

    bool InputContextUIPC::rightPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveRight)->pressed() || inputManager.gamepad().moveRight();
    }

    bool InputContextUIPC::upPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveUp)->pressed() || inputManager.gamepad().moveUp();
    }

    bool InputContextUIPC::downPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveDown)->pressed() || inputManager.gamepad().moveDown();
    }

    bool InputContextUIPC::okPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdInteract)->triggered() ||
            inputManager.gamepadBinding(ActionGamepadIdInteract)->triggered();
    }

    bool InputContextUIPC::havePointer() const
    {
        return false;
    }

    bool InputContextUIPC::pressed(b2Vec2* point) const
    {
        return false;
    }

    bool InputContextUIPC::triggered() const
    {
        return false;
    }

    void InputContextUIPC::doActivate()
    {
        if (rc()) {
            rc()->ShowMouseCursor(true);
            rc()->ProcessMouseMove(
                inputManager.mouse().pos().x * settings.layoutWidth / scene()->gameWidth(),
                (scene()->gameHeight() - inputManager.mouse().pos().y) * settings.layoutHeight / scene()->gameHeight(),
                0);
        }
        lrManager.setActiveContext(rc());
    }

    void InputContextUIPC::doDeactivate()
    {
        if (rc()) {
            rc()->ShowMouseCursor(false);
        }
        lrManager.setActiveContext(NULL);
    }
}
