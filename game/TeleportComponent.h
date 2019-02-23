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

#ifndef _TELEPORTCOMPONENT_H_
#define _TELEPORTCOMPONENT_H_

#include "PhasedComponent.h"
#include "RenderBeamComponent.h"
#include "AnimationComponent.h"
#include "LightComponent.h"
#include "LineLight.h"
#include "PointLight.h"
#include "Tweening.h"
#include "AudioManager.h"
#include "JointProxy.h"
#include "SceneObject.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class TeleportComponent : public boost::enable_shared_from_this<TeleportComponent>,
                              public PhasedComponent
    {
    public:
        explicit TeleportComponent(const SceneObjectPtr& ring);
        ~TeleportComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void start(float beamRadius);

        inline bool finished() const { return finished_; }

    private:
        struct Beam
        {
            RenderBeamComponentPtr render;
            AnimationComponentPtr animation;
            LineLightPtr light;
        };

        virtual void onRegister();

        virtual void onUnregister();

        void cleanupSpawn();

        SceneObjectPtr ring_;

        LightComponentPtr lightC_;

        float beamRadius_;
        RevoluteJointProxyPtr joint_;

        Beam beams_[3];
        PointLightPtr light_;
        size_t i_;
        float beamTime_;

        TweeningPtr tweening_;
        float tweenTime_;

        TweeningPtr tweening2_;
        float tweenTime2_;

        bool finished_;

        AudioSourcePtr strmActive_;
        AudioSourcePtr sndBeam_;
        AudioSourcePtr strmReady_;
    };

    typedef boost::shared_ptr<TeleportComponent> TeleportComponentPtr;
}

#endif
