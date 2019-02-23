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

#ifndef _STAMINABARCOMPONENT_H_
#define _STAMINABARCOMPONENT_H_

#include "UIComponent.h"
#include "Image.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class StaminaBarComponent : public boost::enable_shared_from_this<StaminaBarComponent>,
                                public UIComponent
    {
    public:
        StaminaBarComponent(const b2Vec2& pos, float width, float height);
        ~StaminaBarComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        void setPercent(float value) { percent_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void renderQuad(const Image& image, const b2Vec2& pos,
            float width, float height, bool flip = false);

        b2Vec2 pos_;
        float width_;
        float height_;
        float percent_;

        Image bg_;
        Image end_;
        Image stamina_;
    };

    typedef boost::shared_ptr<StaminaBarComponent> StaminaBarComponentPtr;
}

#endif
