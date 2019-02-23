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

#ifndef _INPUTGAMEPADBINDING_H_
#define _INPUTGAMEPADBINDING_H_

#include "InputGamepad.h"
#include "Image.h"
#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class InputGamepadBinding;
    typedef boost::shared_ptr<InputGamepadBinding> InputGamepadBindingPtr;

    class InputGamepadBinding : boost::noncopyable
    {
    public:
        InputGamepadBinding();
        ~InputGamepadBinding();

        static const std::string& getButtonImageName(GamepadButton button);
        static const std::string& getStickImageName(bool left);

        static Image getButtonImage(GamepadButton button);
        static Image getStickImage(bool left);

        static bool isLongButton(GamepadButton value);
        static const std::string& buttonToString(GamepadButton value);

        static bool stringToButton(const std::string& str, GamepadButton& value);

        bool isLongButton() const;

        inline GamepadButton button() const { return button_; }

        const std::string& str() const;

        void setButton(GamepadButton value);

        bool equal(const InputGamepadBindingPtr& other) const;

        void assign(const InputGamepadBindingPtr& other);

        void clear();

        bool empty() const;

        bool pressed() const;

        bool triggered() const;

    private:
        GamepadButton button_;
    };

    class ActionGamepadBinding : boost::noncopyable
    {
    public:
        ActionGamepadBinding();
        ~ActionGamepadBinding();

        inline const InputGamepadBindingPtr& ib(int i) const { assert((i == 0) || (i == 1)); return ib_[i]; }

        bool pressed() const;

        bool triggered() const;

    private:
        InputGamepadBindingPtr ib_[2];
    };

    typedef boost::shared_ptr<ActionGamepadBinding> ActionGamepadBindingPtr;
}

#endif
