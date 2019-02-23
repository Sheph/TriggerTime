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

#ifndef _LETTERBOXCOMPONENT_H_
#define _LETTERBOXCOMPONENT_H_

#include "UIComponent.h"
#include "Drawable.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class LetterBoxComponent : public boost::enable_shared_from_this<LetterBoxComponent>,
                               public UIComponent
    {
    public:
        LetterBoxComponent(float distance, float duration, int zOrder = 0);
        ~LetterBoxComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        void setActive(bool value, bool immediate);

        inline float borderWidth() const { return borderWidth_; }
        inline void setBorderWidth(float value) { borderWidth_ = value; }

        inline const Color& bgColor() const { return bgColor_; }
        inline void setBgColor(const Color& value) { bgColor_ = value; }

        inline const Color& borderColor() const { return borderColor_; }
        inline void setBorderColor(const Color& value) { borderColor_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void renderColorQuad(const Image& image, const b2Vec2& pos,
             float width, float height,
             const Color& color);

        float borderWidth_;

        TweeningPtr tweening_;
        float tweenTime_;
        bool active_;

        Color bgColor_;
        Color borderColor_;
        Image bg_;
        Image top_;
        Image bottom_;
    };

    typedef boost::shared_ptr<LetterBoxComponent> LetterBoxComponentPtr;
}

#endif
