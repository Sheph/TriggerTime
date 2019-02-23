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

#ifndef _KNOBCOMPONENT_H_
#define _KNOBCOMPONENT_H_

#include "UIComponent.h"
#include "Drawable.h"
#include "Tweening.h"
#include "TextArea.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class KnobComponent : public boost::enable_shared_from_this<KnobComponent>,
                          public UIComponent
    {
    public:
        KnobComponent(int zOrder = 0);
        ~KnobComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        inline float radius() const { return radius_; }
        inline void setRadius(float value) { radius_ = value; }

        inline float handleRadius() const { return handleRadius_; }
        inline void setHandleRadius(float value) { handleRadius_ = value; }

        inline bool drawRing() const { return drawRing_; }
        inline void setDrawRing(bool value) { drawRing_ = value; }

        inline const b2Vec2& handlePos() const { return handlePos_; }
        inline void setHandlePos(const b2Vec2& value) { handlePos_ = value; }

        inline const Image& image() const { return image_; }
        inline void setImage(const Image& value) { image_ = value; }

        bool showOff() const;
        void setShowOff(bool value);

        inline float alpha() const { return alpha_; }
        inline void setAlpha(float value) { alpha_ = value; }

        inline const std::string& text() const { return text_->text(); }
        inline void setText(const char* value)
        {
            if (text_->text() != value) {
                text_->setText(value);
            }
        }

        inline float textSize() const { return textSize_; }
        inline void setTextSize(float value) { textSize_ = value; }

        inline const b2Vec2& textPos() const { return textPos_; }
        inline void setTextPos(const b2Vec2& value) { textPos_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void renderQuad(const Image& image, const b2Vec2& pos,
                        float width, float height);

        Image ring_;
        Image handle_;

        float radius_;
        float handleRadius_;
        bool drawRing_;
        b2Vec2 handlePos_;
        Image image_;

        TweeningPtr tweening_;
        float tweenTime_;
        float actualHandleRadius_;

        float alpha_;

        TextAreaPtr text_;
        float textSize_;
        b2Vec2 textPos_;
    };

    typedef boost::shared_ptr<KnobComponent> KnobComponentPtr;
}

#endif
