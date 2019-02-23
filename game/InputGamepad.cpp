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

#include "InputGamepad.h"
#include "InputManager.h"
#include "Settings.h"
#include "af/Utils.h"

namespace af
{
    InputGamepad::InputGamepad()
    : stickDeadzone_(0.2f),
      triggerDeadzone_(0.1f)
    {
        pos_[0] = b2Vec2_zero;
        pos_[1] = b2Vec2_zero;
    }

    InputGamepad::~InputGamepad()
    {
    }

    void InputGamepad::moveStick(bool left, const b2Vec2& value)
    {
        b2Vec2 tmp = value;

        float len = tmp.Normalize();

        if (len <= stickDeadzone_) {
            pos_[left] = b2Vec2_zero;
        } else {
            if (len > 1.0f) {
                len = 1.0f;
            }
            pos_[left] = ((len - stickDeadzone_) / (1.0f - stickDeadzone_)) * tmp;
        }

        inputManager.setUsingGamepad(true);
    }

    void InputGamepad::moveTrigger(bool left, float value)
    {
        if (value <= triggerDeadzone_) {
            release(left ? GamepadLeftTrigger : GamepadRightTrigger);
        } else {
            press(left ? GamepadLeftTrigger : GamepadRightTrigger);
        }
    }

    void InputGamepad::press(GamepadButton button)
    {
        if (!buttonMap_[button].pressed) {
            buttonMap_[button].triggered = true;
        }
        buttonMap_[button].pressed = true;

        inputManager.setUsingGamepad(true);
    }

    void InputGamepad::release(GamepadButton button)
    {
        buttonMap_[button].pressed = false;
        buttonMap_[button].triggered = false;

        inputManager.setUsingGamepad(true);
    }

    bool InputGamepad::pressed(GamepadButton button) const
    {
        return buttonMap_[button].pressed;
    }

    bool InputGamepad::triggered(GamepadButton button) const
    {
        return buttonMap_[button].triggered;
    }

    bool InputGamepad::moveLeft() const
    {
        b2Vec2 tmp = pos(true);

        if (tmp == b2Vec2_zero) {
            return false;
        }

        return (b2Cross(tmp, b2Vec2(1.0f, 1.0f)) < 0.0f) &&
            (b2Cross(tmp, b2Vec2(-1.0f, 1.0f)) < 0.0f);
    }

    bool InputGamepad::moveRight() const
    {
        b2Vec2 tmp = pos(true);

        if (tmp == b2Vec2_zero) {
            return false;
        }

        return (b2Cross(tmp, b2Vec2(1.0f, 1.0f)) >= 0.0f) &&
            (b2Cross(tmp, b2Vec2(-1.0f, 1.0f)) >= 0.0f);
    }

    bool InputGamepad::moveUp() const
    {
        b2Vec2 tmp = pos(true);

        if (tmp == b2Vec2_zero) {
            return false;
        }

        return (b2Cross(tmp, b2Vec2(1.0f, 1.0f)) < 0.0f) &&
            (b2Cross(tmp, b2Vec2(-1.0f, 1.0f)) >= 0.0f);
    }

    bool InputGamepad::moveDown() const
    {
        b2Vec2 tmp = pos(true);

        if (tmp == b2Vec2_zero) {
            return false;
        }

        return (b2Cross(tmp, b2Vec2(1.0f, 1.0f)) >= 0.0f) &&
            (b2Cross(tmp, b2Vec2(-1.0f, 1.0f)) < 0.0f);
    }

    void InputGamepad::processed()
    {
        for (ButtonMap::iterator it = buttonMap_.begin(); it != buttonMap_.end(); ++it) {
            it->second.triggered = false;
        }
    }
}
