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

#ifndef _GASCLOUDCOMPONENT_H_
#define _GASCLOUDCOMPONENT_H_

#include "PhasedComponent.h"
#include "SensorListener.h"
#include "AudioManager.h"
#include "PointLight.h"
#include "LightComponent.h"
#include "ParticleEffectComponent.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GasCloudComponent : public boost::enable_shared_from_this<GasCloudComponent>,
                              public PhasedComponent,
                              public SensorListener
    {
    public:
        GasCloudComponent(const ParticleEffectComponentPtr& pec, float delay, float offset);
        ~GasCloudComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, bool& found);

        static float damageT_;
        static int numInside_;

        ParticleEffectComponentPtr pec_;
        float delay_;
        LightComponentPtr lightC_;
        PointLightPtr light_;
        TweeningPtr tweening_;
        float tweenTime_;
        float propagateT_;

        SceneObjectPtr target_;
        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<GasCloudComponent> GasCloudComponentPtr;
}

#endif
