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

#ifndef _RENDERTEXTCOMPONENT_H_
#define _RENDERTEXTCOMPONENT_H_

#include "RenderComponent.h"
#include "TextArea.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderTextComponent : public boost::enable_shared_from_this<RenderTextComponent>,
                                public RenderComponent
    {
    public:
        RenderTextComponent(const b2Vec2& pos, float angle, float width, const std::string& text,
            int zOrder = 0);
        ~RenderTextComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline const b2Vec2& pos() const { return pos_; }
        void setPos(const b2Vec2& value);

        inline float angle() const { return angle_; }
        void setAngle(float value);

        inline float width() const { return text_->width(); }
        void setWidth(float value);

        inline float charSize() const { return text_->charSize(); }
        void setCharSize(float value);

        inline const std::string& text() const { return text_->text(); }
        void setText(const std::string& value);

        inline int maxLetters() const { return text_->textNumLetters(); }

        inline int numLetters() const { return numLetters_; }
        inline void setNumLetters(int value) { numLetters_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        TextAreaPtr text_;

        b2Vec2 pos_;
        float angle_;
        int numLetters_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        bool dirty_;
    };

    typedef boost::shared_ptr<RenderTextComponent> RenderTextComponentPtr;
}

#endif
