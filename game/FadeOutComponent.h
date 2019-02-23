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

#ifndef _FADEOUTCOMPONENT_H_
#define _FADEOUTCOMPONENT_H_

#include "PhasedComponent.h"
#include "Tweening.h"
#include "Light.h"
#include "RenderComponent.h"
#include <boost/enable_shared_from_this.hpp>
#include <map>

namespace af
{
    class FadeOutComponent : public boost::enable_shared_from_this<FadeOutComponent>,
                             public PhasedComponent
    {
    public:
        explicit FadeOutComponent(float duration, float delay = 0.0f);
        ~FadeOutComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        inline bool lightsOnly() const { return lightsOnly_; }
        inline void setLightsOnly(bool value) { lightsOnly_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        TweeningPtr tweening_;
        float tweenTime_;

        std::map<RenderComponentPtr, float> rc_;
        std::map<LightPtr, float> lights_;
        bool lightsOnly_;
    };

    typedef boost::shared_ptr<FadeOutComponent> FadeOutComponentPtr;
}

#endif
