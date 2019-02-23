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

#ifndef _GOALINDICATORCOMPONENT_H_
#define _GOALINDICATORCOMPONENT_H_

#include "UIComponent.h"
#include "AnimationComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GoalIndicatorComponent : public boost::enable_shared_from_this<GoalIndicatorComponent>,
                                   public UIComponent
    {
    public:
        explicit GoalIndicatorComponent(float length, int zOrder = 0);
        ~GoalIndicatorComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        inline float padding() const { return padding_; }
        void setPadding(float value);

        inline const Color& color() const { return color_; }
        inline void setColor(const Color& value) { color_ = value; }

        void setDirection(const b2Vec2& value);
        inline const b2Vec2& direction() const { return dir_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void renderQuad(const Image& image, const b2Vec2& pos, float angle,
                        float width, float height, const Color& color);

        float length_;

        AnimationComponentPtr ac_;

        float padding_;
        b2PolygonShape boundary_;
        Color color_;
        b2Vec2 dir_;
        b2Vec2 p1_;

        b2Vec2 pos_;
    };

    typedef boost::shared_ptr<GoalIndicatorComponent> GoalIndicatorComponentPtr;
}

#endif
