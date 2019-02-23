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

#include "InputBinding.h"
#include "InputManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    struct KeyInfo
    {
        KeyInfo()
        : isLong(false)
        {
        }

        explicit KeyInfo(const std::string& str)
        : str(str), isLong(str.length() > 1)
        {
        }

        std::string str;
        bool isLong;
    };

    static KeyInfo keyInfo[256];
    static bool keyInfoInitialized = false;

    static std::string mbStr[2] = { "MOUSE1", "MOUSE2" };

    static void keyInfoInitialize()
    {
        keyInfo[KI_BACK] = KeyInfo("BACKSPACE");
        keyInfo[KI_TAB] = KeyInfo("TAB");
        keyInfo[KI_RETURN] = KeyInfo("ENTER");
        keyInfo[KI_PAUSE] = KeyInfo("PAUSE");
        keyInfo[KI_SCROLL] = KeyInfo("SCROLL");
        keyInfo[KI_ESCAPE] = KeyInfo("ESC");
        keyInfo[KI_DELETE] = KeyInfo("DELETE");
        keyInfo[KI_HOME] = KeyInfo("HOME");
        keyInfo[KI_LEFT] = KeyInfo("LEFT");
        keyInfo[KI_UP] = KeyInfo("UP");
        keyInfo[KI_RIGHT] = KeyInfo("RIGHT");
        keyInfo[KI_DOWN] = KeyInfo("DOWN");
        keyInfo[KI_PRIOR] = KeyInfo("PAGEUP");
        keyInfo[KI_NEXT] = KeyInfo("PAGEDOWN");
        keyInfo[KI_END] = KeyInfo("END");
        keyInfo[KI_SNAPSHOT] = KeyInfo("PRNTSCRN");
        keyInfo[KI_INSERT] = KeyInfo("INSERT");
        keyInfo[KI_NUMLOCK] = KeyInfo("NUMLOCK");
        keyInfo[KI_SPACE] = KeyInfo("SPACE");
        keyInfo[KI_NUMPADENTER] = KeyInfo("KP_ENTER");
        keyInfo[KI_F1] = KeyInfo("F1");
        keyInfo[KI_F2] = KeyInfo("F2");
        keyInfo[KI_F3] = KeyInfo("F3");
        keyInfo[KI_F4] = KeyInfo("F4");
        keyInfo[KI_NUMPAD7] = KeyInfo("KP_7");
        keyInfo[KI_NUMPAD4] = KeyInfo("KP_4");
        keyInfo[KI_NUMPAD8] = KeyInfo("KP_8");
        keyInfo[KI_NUMPAD6] = KeyInfo("KP_6");
        keyInfo[KI_NUMPAD2] = KeyInfo("KP_2");
        keyInfo[KI_NUMPAD9] = KeyInfo("KP_9");
        keyInfo[KI_NUMPAD3] = KeyInfo("KP_3");
        keyInfo[KI_NUMPAD1] = KeyInfo("KP_1");
        keyInfo[KI_NUMPAD5] = KeyInfo("KP_5");
        keyInfo[KI_NUMPAD0] = KeyInfo("KP_0");
        keyInfo[KI_DECIMAL] = KeyInfo("KP_PERIOD");
        keyInfo[KI_OEM_NEC_EQUAL] = KeyInfo("KP_EQUAL");
        keyInfo[KI_MULTIPLY] = KeyInfo("KP_MUL");
        keyInfo[KI_ADD] = KeyInfo("KP_ADD");
        keyInfo[KI_SEPARATOR] = KeyInfo("KP_COMMA");
        keyInfo[KI_SUBTRACT] = KeyInfo("KP_SUB");
        keyInfo[KI_DIVIDE] = KeyInfo("KP_DIV");
        keyInfo[KI_F5] = KeyInfo("F5");
        keyInfo[KI_F6] = KeyInfo("F6");
        keyInfo[KI_F7] = KeyInfo("F7");
        keyInfo[KI_F8] = KeyInfo("F8");
        keyInfo[KI_F9] = KeyInfo("F9");
        keyInfo[KI_F10] = KeyInfo("F10");
        keyInfo[KI_F11] = KeyInfo("F11");
        keyInfo[KI_F12] = KeyInfo("F12");
        keyInfo[KI_F13] = KeyInfo("F13");
        keyInfo[KI_F14] = KeyInfo("F14");
        keyInfo[KI_F15] = KeyInfo("F15");
        keyInfo[KI_F16] = KeyInfo("F16");
        keyInfo[KI_F17] = KeyInfo("F17");
        keyInfo[KI_F18] = KeyInfo("F18");
        keyInfo[KI_F19] = KeyInfo("F19");
        keyInfo[KI_F20] = KeyInfo("F20");
        keyInfo[KI_F21] = KeyInfo("F21");
        keyInfo[KI_F22] = KeyInfo("F22");
        keyInfo[KI_F23] = KeyInfo("F23");
        keyInfo[KI_F24] = KeyInfo("F24");
        keyInfo[KI_LSHIFT] = KeyInfo("LSHIFT");
        keyInfo[KI_RSHIFT] = KeyInfo("RSHIFT");
        keyInfo[KI_LCONTROL] = KeyInfo("LCTRL");
        keyInfo[KI_RCONTROL] = KeyInfo("RCTRL");
        keyInfo[KI_CAPITAL] = KeyInfo("CAPSLOCK");
        keyInfo[KI_LMENU] = KeyInfo("LALT");
        keyInfo[KI_RMENU] = KeyInfo("RALT");
        keyInfo[KI_OEM_7] = KeyInfo("\"");
        keyInfo[KI_OEM_COMMA] = KeyInfo(",");
        keyInfo[KI_OEM_MINUS] = KeyInfo("-");
        keyInfo[KI_OEM_PERIOD] = KeyInfo(".");
        keyInfo[KI_OEM_2] = KeyInfo("/");
        keyInfo[KI_0] = KeyInfo("0");
        keyInfo[KI_1] = KeyInfo("1");
        keyInfo[KI_2] = KeyInfo("2");
        keyInfo[KI_3] = KeyInfo("3");
        keyInfo[KI_4] = KeyInfo("4");
        keyInfo[KI_5] = KeyInfo("5");
        keyInfo[KI_6] = KeyInfo("6");
        keyInfo[KI_7] = KeyInfo("7");
        keyInfo[KI_8] = KeyInfo("8");
        keyInfo[KI_9] = KeyInfo("9");
        keyInfo[KI_OEM_1] = KeyInfo(":");
        keyInfo[KI_OEM_PLUS] = KeyInfo("+");
        keyInfo[KI_OEM_4] = KeyInfo("[");
        keyInfo[KI_OEM_5] = KeyInfo("\\");
        keyInfo[KI_OEM_6] = KeyInfo("]");
        keyInfo[KI_OEM_3] = KeyInfo("~");
        keyInfo[KI_A] = KeyInfo("A");
        keyInfo[KI_B] = KeyInfo("B");
        keyInfo[KI_C] = KeyInfo("C");
        keyInfo[KI_D] = KeyInfo("D");
        keyInfo[KI_E] = KeyInfo("E");
        keyInfo[KI_F] = KeyInfo("F");
        keyInfo[KI_G] = KeyInfo("G");
        keyInfo[KI_H] = KeyInfo("H");
        keyInfo[KI_I] = KeyInfo("I");
        keyInfo[KI_J] = KeyInfo("J");
        keyInfo[KI_K] = KeyInfo("K");
        keyInfo[KI_L] = KeyInfo("L");
        keyInfo[KI_M] = KeyInfo("M");
        keyInfo[KI_N] = KeyInfo("N");
        keyInfo[KI_O] = KeyInfo("O");
        keyInfo[KI_P] = KeyInfo("P");
        keyInfo[KI_Q] = KeyInfo("Q");
        keyInfo[KI_R] = KeyInfo("R");
        keyInfo[KI_S] = KeyInfo("S");
        keyInfo[KI_T] = KeyInfo("T");
        keyInfo[KI_U] = KeyInfo("U");
        keyInfo[KI_V] = KeyInfo("V");
        keyInfo[KI_W] = KeyInfo("W");
        keyInfo[KI_X] = KeyInfo("X");
        keyInfo[KI_Y] = KeyInfo("Y");
        keyInfo[KI_Z] = KeyInfo("Z");
    }

    InputBinding::InputBinding()
    : ki_(KI_UNKNOWN)
    {
        mb_[0] = false;
        mb_[1] = false;
    }

    InputBinding::~InputBinding()
    {
    }

    bool InputBinding::isLongKey(KeyIdentifier ki)
    {
        if (!keyInfoInitialized) {
            keyInfoInitialize();
            keyInfoInitialized = true;
        }
        return keyInfo[ki].isLong;
    }

    const std::string& InputBinding::keyToString(KeyIdentifier ki)
    {
        if (!keyInfoInitialized) {
            keyInfoInitialize();
            keyInfoInitialized = true;
        }
        return keyInfo[ki].str;
    }

    const std::string& InputBinding::mbToString(bool left)
    {
        return mbStr[left ? 0 : 1];
    }

    bool InputBinding::stringToKey(const std::string& str, KeyIdentifier& ki)
    {
        if (!keyInfoInitialized) {
            keyInfoInitialize();
            keyInfoInitialized = true;
        }

        for (size_t i = 0; i < sizeof(keyInfo)/sizeof(keyInfo[0]); ++i) {
            if (str == keyInfo[i].str) {
                ki = static_cast<KeyIdentifier>(i);
                return true;
            }
        }
        return false;
    }

    bool InputBinding::stringToMb(const std::string& str, bool& left)
    {
        for (size_t i = 0; i < sizeof(mbStr)/sizeof(mbStr[0]); ++i) {
            if (str == mbStr[i]) {
                left = (i == 0);
                return true;
            }
        }
        return false;
    }

    bool InputBinding::isKey() const
    {
        return ki_ != KI_UNKNOWN;
    }

    bool InputBinding::isLongKey() const
    {
        return isLongKey(ki_);
    }

    bool InputBinding::isMb(bool left) const
    {
        if (isKey()) {
            return false;
        }
        return mb_[left ? 0 : 1];
    }

    const std::string& InputBinding::str() const
    {
        static const std::string emptyStr;

        if (isKey()) {
            return keyToString(ki_);
        } else if (mb_[0]) {
            return mbToString(true);
        } else if (mb_[1]) {
            return mbToString(false);
        } else {
            return emptyStr;
        }
    }

    void InputBinding::setKey(KeyIdentifier value)
    {
        ki_ = value;
        mb_[0] = false;
        mb_[1] = false;
    }

    void InputBinding::setMb(bool left)
    {
        ki_ = KI_UNKNOWN;
        mb_[0] = false;
        mb_[1] = false;
        mb_[left ? 0 : 1] = true;
    }

    bool InputBinding::equal(const InputBindingPtr& other) const
    {
        return (ki_ == other->ki_) && (mb_[0] == other->mb_[0]) && (mb_[1] == other->mb_[1]);
    }

    void InputBinding::assign(const InputBindingPtr& other)
    {
        ki_ = other->ki_;
        mb_[0] = other->mb_[0];
        mb_[1] = other->mb_[1];
    }

    void InputBinding::clear()
    {
        ki_ = KI_UNKNOWN;
        mb_[0] = false;
        mb_[1] = false;
    }

    bool InputBinding::empty() const
    {
        return (ki_ == KI_UNKNOWN) && !mb_[0] && !mb_[1];
    }

    bool InputBinding::pressed() const
    {
        if (isKey()) {
            if (inputManager.keyboard().pressed(ki_)) {
                return true;
            }
        } else if (isMb(true)) {
            if (inputManager.mouse().pressed(true)) {
                return true;
            }
        } else if (isMb(false)) {
            if (inputManager.mouse().pressed(false)) {
                return true;
            }
        }
        return false;
    }

    bool InputBinding::triggered() const
    {
        if (isKey()) {
            if (inputManager.keyboard().triggered(ki_)) {
                return true;
            }
        } else if (isMb(true)) {
            if (inputManager.mouse().triggered(true)) {
                return true;
            }
        } else if (isMb(false)) {
            if (inputManager.mouse().triggered(false)) {
                return true;
            }
        }
        return false;
    }

    ActionBinding::ActionBinding()
    {
        ib_[0] = boost::make_shared<InputBinding>();
        ib_[1] = boost::make_shared<InputBinding>();
    }

    ActionBinding::~ActionBinding()
    {
    }

    bool ActionBinding::pressed() const
    {
        return ib_[0]->pressed() || ib_[1]->pressed();
    }

    bool ActionBinding::triggered() const
    {
        return ib_[0]->triggered() || ib_[1]->triggered();
    }
}
