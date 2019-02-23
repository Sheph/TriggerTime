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

#ifndef _INPUTHINTCOMPONENT_H_
#define _INPUTHINTCOMPONENT_H_

#include "UIComponent.h"
#include "Drawable.h"
#include "Tweening.h"
#include "TextArea.h"
#include "InputGamepad.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class InputHintComponent : public boost::enable_shared_from_this<InputHintComponent>,
                               public UIComponent
    {
    public:
        explicit InputHintComponent(int zOrder = 0);
        ~InputHintComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        void addKbNormal(const std::string& value);

        void addKbLong(const std::string& value);

        void addText(const std::string& value);

        void addMb(bool left);

        void addGamepadStick(bool left);

        void addGamepadButton(GamepadButton button);

        void setDescription(const std::string& value);

        void setFade(float duration);

    private:
        struct Entry
        {
            Entry()
            : flip(false),
              gamepad(false)
            {
            }

            Image image;
            TextAreaPtr text;
            bool flip;
            bool gamepad;
        };

        virtual void onRegister();

        virtual void onUnregister();

        void renderColorQuad(const Image& image, const b2Vec2& pos,
             float width, float height,
             const Color& color, bool flip = false);

        float charSize_;
        float mouseSize_;
        float gamepadSize_;
        b2Vec2 padding_;
        float spacing_;
        std::vector<Entry> entries_;
        TextAreaPtr text_;

        TweeningPtr tweening_;
        float tweenTime_;
    };

    typedef boost::shared_ptr<InputHintComponent> InputHintComponentPtr;
}

#endif
