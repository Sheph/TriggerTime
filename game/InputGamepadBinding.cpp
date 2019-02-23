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

#include "InputGamepadBinding.h"
#include "InputManager.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    struct ButtonInfo
    {
        ButtonInfo()
        : isLong(false)
        {
        }

        explicit ButtonInfo(const std::string& str)
        : str(str), isLong(str.length() > 1)
        {
        }

        std::string str;
        bool isLong;
    };

    static ButtonInfo buttonInfo[GamepadButtonMax + 1];
    static bool buttonInfoInitialized = false;

    static const std::string buttonImages[GamepadButtonMax + 1] = {
        "",
        "common1/gamepad_dp_up.png",
        "common1/gamepad_dp_down.png",
        "common1/gamepad_dp_left.png",
        "common1/gamepad_dp_right.png",
        "common1/gamepad_start.png",
        "common1/gamepad_back.png",
        "common1/gamepad_lsb.png",
        "common1/gamepad_rsb.png",
        "common1/gamepad_lb.png",
        "common1/gamepad_rb.png",
        "common1/gamepad_lt.png",
        "common1/gamepad_rt.png",
        "common1/gamepad_a.png",
        "common1/gamepad_b.png",
        "common1/gamepad_x.png",
        "common1/gamepad_y.png"
    };

    static const std::string stickImages[2] = {
        "common1/gamepad_rs.png",
        "common1/gamepad_ls.png"
    };

    static void buttonInfoInitialize()
    {
        buttonInfo[GamepadDPADUp] = ButtonInfo("DP_UP");
        buttonInfo[GamepadDPADDown] = ButtonInfo("DP_DOWN");
        buttonInfo[GamepadDPADLeft] = ButtonInfo("DP_LEFT");
        buttonInfo[GamepadDPADRight] = ButtonInfo("DP_RIGHT");
        buttonInfo[GamepadStart] = ButtonInfo("START");
        buttonInfo[GamepadBack] = ButtonInfo("BACK");
        buttonInfo[GamepadLeftStick] = ButtonInfo("LTHUMB");
        buttonInfo[GamepadRightStick] = ButtonInfo("RTHUMB");
        buttonInfo[GamepadLeftBumper] = ButtonInfo("LB");
        buttonInfo[GamepadRightBumper] = ButtonInfo("RB");
        buttonInfo[GamepadLeftTrigger] = ButtonInfo("LT");
        buttonInfo[GamepadRightTrigger] = ButtonInfo("RT");
        buttonInfo[GamepadA] = ButtonInfo("A");
        buttonInfo[GamepadB] = ButtonInfo("B");
        buttonInfo[GamepadX] = ButtonInfo("X");
        buttonInfo[GamepadY] = ButtonInfo("Y");
    }

    InputGamepadBinding::InputGamepadBinding()
    : button_(GamepadUnknown)
    {
    }

    InputGamepadBinding::~InputGamepadBinding()
    {
    }

    const std::string& InputGamepadBinding::getButtonImageName(GamepadButton button)
    {
        return buttonImages[button];
    }

    const std::string& InputGamepadBinding::getStickImageName(bool left)
    {
        return stickImages[left];
    }

    Image InputGamepadBinding::getButtonImage(GamepadButton button)
    {
        return assetManager.getImage(getButtonImageName(button));
    }

    Image InputGamepadBinding::getStickImage(bool left)
    {
        return assetManager.getImage(getStickImageName(left));
    }

    bool InputGamepadBinding::isLongButton(GamepadButton value)
    {
        if (!buttonInfoInitialized) {
            buttonInfoInitialize();
            buttonInfoInitialized = true;
        }
        return buttonInfo[value].isLong;
    }

    const std::string& InputGamepadBinding::buttonToString(GamepadButton value)
    {
        if (!buttonInfoInitialized) {
            buttonInfoInitialize();
            buttonInfoInitialized = true;
        }
        return buttonInfo[value].str;
    }

    bool InputGamepadBinding::stringToButton(const std::string& str, GamepadButton& value)
    {
        if (!buttonInfoInitialized) {
            buttonInfoInitialize();
            buttonInfoInitialized = true;
        }

        for (size_t i = 0; i < sizeof(buttonInfo)/sizeof(buttonInfo[0]); ++i) {
            if (str == buttonInfo[i].str) {
                value = static_cast<GamepadButton>(i);
                return true;
            }
        }
        return false;
    }

    bool InputGamepadBinding::isLongButton() const
    {
        return isLongButton(button_);
    }

    const std::string& InputGamepadBinding::str() const
    {
        return buttonToString(button_);
    }

    void InputGamepadBinding::setButton(GamepadButton value)
    {
        button_ = value;
    }

    bool InputGamepadBinding::equal(const InputGamepadBindingPtr& other) const
    {
        return (button_ == other->button_);
    }

    void InputGamepadBinding::assign(const InputGamepadBindingPtr& other)
    {
        button_ = other->button_;
    }

    void InputGamepadBinding::clear()
    {
        button_ = GamepadUnknown;
    }

    bool InputGamepadBinding::empty() const
    {
        return (button_ == GamepadUnknown);
    }

    bool InputGamepadBinding::pressed() const
    {
        return !empty() && inputManager.gamepad().pressed(button_);
    }

    bool InputGamepadBinding::triggered() const
    {
        return !empty() && inputManager.gamepad().triggered(button_);
    }

    ActionGamepadBinding::ActionGamepadBinding()
    {
        ib_[0] = boost::make_shared<InputGamepadBinding>();
        ib_[1] = boost::make_shared<InputGamepadBinding>();
    }

    ActionGamepadBinding::~ActionGamepadBinding()
    {
    }

    bool ActionGamepadBinding::pressed() const
    {
        return ib_[0]->pressed() || ib_[1]->pressed();
    }

    bool ActionGamepadBinding::triggered() const
    {
        return ib_[0]->triggered() || ib_[1]->triggered();
    }
}
