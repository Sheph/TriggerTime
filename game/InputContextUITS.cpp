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

#include "InputContextUITS.h"
#include "InputManager.h"

namespace af
{
    InputContextUITS::InputContextUITS(Scene* scene, Rocket::Core::Context* rc)
    : InputContextUI(scene, rc)
    {
    }

    InputContextUITS::~InputContextUITS()
    {
    }

    void InputContextUITS::update(float dt)
    {
    }

    bool InputContextUITS::leftPressed() const
    {
        return false;
    }

    bool InputContextUITS::rightPressed() const
    {
        return false;
    }

    bool InputContextUITS::upPressed() const
    {
        return false;
    }

    bool InputContextUITS::downPressed() const
    {
        return false;
    }

    bool InputContextUITS::okPressed() const
    {
        return false;
    }

    bool InputContextUITS::havePointer() const
    {
        return true;
    }

    bool InputContextUITS::pressed(b2Vec2* point) const
    {
        if (!active()) {
            return false;
        }

        return inputManager.touchScreen().pressed(0, point);
    }

    bool InputContextUITS::triggered() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.touchScreen().triggered(0);
    }

    void InputContextUITS::doActivate()
    {
    }

    void InputContextUITS::doDeactivate()
    {
    }
}
