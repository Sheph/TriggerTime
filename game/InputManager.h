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

#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

#include "af/Single.h"
#include "InputKeyboard.h"
#include "InputTouchScreen.h"
#include "InputMouse.h"
#include "InputGamepad.h"
#include "InputBinding.h"
#include "InputGamepadBinding.h"

namespace af
{
    enum ActionId
    {
        ActionIdMoveUp = 0,
        ActionIdMoveDown,
        ActionIdMoveLeft,
        ActionIdMoveRight,
        ActionIdPrimaryFire,
        ActionIdSecondaryFire,
        ActionIdPrimarySwitch,
        ActionIdSecondarySwitch,
        ActionIdInteract,
        ActionIdRun
    };

    static const int ActionIdMax = ActionIdRun;

    enum ActionGamepadId
    {
        ActionGamepadIdPrimaryFire = 0,
        ActionGamepadIdSecondaryFire,
        ActionGamepadIdPrimarySwitch,
        ActionGamepadIdSecondarySwitch,
        ActionGamepadIdInteract,
        ActionGamepadIdRun
    };

    static const int ActionGamepadIdMax = ActionGamepadIdRun;

    class InputManager : public Single<InputManager>
    {
    public:
        InputManager();
        ~InputManager();

        bool init();

        void shutdown();

        void processed();

        inline InputKeyboard& keyboard() { return keyboard_; }

        inline InputTouchScreen& touchScreen() { return touchScreen_; }

        inline InputMouse& mouse() { return mouse_; }

        inline InputGamepad& gamepad() { return gamepad_; }

        inline const ActionBindingPtr& binding(ActionId action) const { return bindings_[action]; }

        inline const ActionGamepadBindingPtr& gamepadBinding(ActionGamepadId action) const { return gamepadBindings_[action]; }

        b2Vec2 lookPos(bool& relative);

        inline bool usingGamepad() const { return usingGamepad_; }
        inline void setUsingGamepad(bool value) { usingGamepad_ = value; }

    private:
        InputKeyboard keyboard_;
        InputTouchScreen touchScreen_;
        InputMouse mouse_;
        InputGamepad gamepad_;

        ActionBindingPtr bindings_[ActionIdMax + 1];
        ActionGamepadBindingPtr gamepadBindings_[ActionGamepadIdMax + 1];

        bool lookRelative_;
        b2Vec2 lookMousePos_;
        b2Vec2 lookGamepadPos_;

        bool usingGamepad_;
    };

    extern InputManager inputManager;
}

#endif
