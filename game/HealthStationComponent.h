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

#ifndef _HEALTHSTATIONCOMPONENT_H_
#define _HEALTHSTATIONCOMPONENT_H_

#include "PhasedComponent.h"
#include "SensorListener.h"
#include "WeaponComponent.h"
#include "AudioManager.h"
#include "Tweening.h"
#include "Light.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class HealthStationComponent : public boost::enable_shared_from_this<HealthStationComponent>,
                                   public PhasedComponent,
                                   public SensorListener
    {
    public:
        HealthStationComponent();
        ~HealthStationComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        LightPtr light_;
        Color startColor_;

        TweeningPtr tweening_;
        float tweenTime_;
        float t_;

        SceneObjectPtr target_;

        AudioSourcePtr sndWork_;
    };

    typedef boost::shared_ptr<HealthStationComponent> HealthStationComponentPtr;
}

#endif
