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

#ifndef _TIMEBOMBCOMPONENT_H_
#define _TIMEBOMBCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"
#include "LightComponent.h"
#include "PointLight.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class TimebombComponent : public boost::enable_shared_from_this<TimebombComponent>,
                              public PhasedComponent
    {
    public:
        typedef boost::function<void()> TimeoutFn;

        TimebombComponent(const SceneObjectPtr& obj, float height);
        ~TimebombComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const b2Transform& transform() const { return xf_; }
        inline void setTransform(const b2Transform& value) { xf_ = value; }

        void setBlast(float impulse, float damage, const SceneObjectTypes& types);

        inline const TimeoutFn& timeoutFn() const { return fn_; }
        inline void setTimeoutFn(const TimeoutFn& value) { fn_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr obj_;
        float height_;
        b2Transform xf_;
        bool blast_;
        float impulse_;
        float damage_;
        SceneObjectTypes types_;
        float t_;
        float beepT_;
        float beepHoldT_;

        PointLightPtr light_;
        LightComponentPtr lightC_;

        AudioSourcePtr snd_;

        TimeoutFn fn_;
    };

    typedef boost::shared_ptr<TimebombComponent> TimebombComponentPtr;
}

#endif
