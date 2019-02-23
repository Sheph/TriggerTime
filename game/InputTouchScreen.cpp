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

#include "InputTouchScreen.h"

namespace af
{
    InputTouchScreen::InputTouchScreen()
    {
        for (int i = 0; i < InputMaxFingers; ++i) {
            fingers_[i].pressed = false;
            fingers_[i].point = b2Vec2_zero;
            fingers_[i].triggered = false;
        }
    }

    InputTouchScreen::~InputTouchScreen()
    {
    }

    void InputTouchScreen::press(int finger, const b2Vec2& point)
    {
        if ((finger < 0) || (finger >= InputMaxFingers)) {
            return;
        }

        if (!fingers_[finger].pressed) {
            fingers_[finger].triggered = true;
        }

        fingers_[finger].pressed = true;
        fingers_[finger].point = point;
    }

    void InputTouchScreen::release(int finger)
    {
        if ((finger < 0) || (finger >= InputMaxFingers)) {
            return;
        }

        fingers_[finger].pressed = false;
        fingers_[finger].triggered = false;
    }

    bool InputTouchScreen::pressed(int finger, b2Vec2* point) const
    {
        if ((finger < 0) || (finger >= InputMaxFingers) || !fingers_[finger].pressed) {
            return false;
        }

        if (point) {
            *point = fingers_[finger].point;
        }

        return true;
    }

    bool InputTouchScreen::triggered(int finger, b2Vec2* point) const
    {
        if ((finger < 0) || (finger >= InputMaxFingers) || !fingers_[finger].triggered) {
            return false;
        }

        if (point) {
            *point = fingers_[finger].point;
        }

        return true;
    }

    void InputTouchScreen::processed()
    {
        for (int i = 0; i < InputMaxFingers; ++i) {
            fingers_[i].triggered = false;
        }
    }
}
