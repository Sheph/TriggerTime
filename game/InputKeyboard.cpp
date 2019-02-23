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

#include "InputKeyboard.h"
#include "InputManager.h"
#include "Settings.h"
#include "af/Utils.h"

namespace af
{
    InputKeyboard::InputKeyboard()
    {
    }

    InputKeyboard::~InputKeyboard()
    {
    }

    void InputKeyboard::press(KeyIdentifier ki)
    {
        if (!keyMap_[ki].pressed) {
            keyMap_[ki].triggered = true;
        }
        keyMap_[ki].pressed = true;

        inputManager.setUsingGamepad(false);
    }

    void InputKeyboard::release(KeyIdentifier ki)
    {
        keyMap_[ki].pressed = false;
        keyMap_[ki].triggered = false;

        inputManager.setUsingGamepad(false);
    }

    bool InputKeyboard::pressed(KeyIdentifier ki) const
    {
        return keyMap_[ki].pressed;
    }

    bool InputKeyboard::triggered(KeyIdentifier ki) const
    {
        return keyMap_[ki].triggered;
    }

    void InputKeyboard::processed()
    {
        for (KeyMap::iterator it = keyMap_.begin(); it != keyMap_.end(); ++it) {
            it->second.triggered = false;
        }
    }
}
