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

#ifndef _EXPLOSIONCOMPONENT_H_
#define _EXPLOSIONCOMPONENT_H_

#include "PhasedComponent.h"
#include "ParticleEffectComponent.h"
#include "PointLight.h"
#include "Tweening.h"
#include "AudioManager.h"
#include "SceneObjectManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class ExplosionComponent : public boost::enable_shared_from_this<ExplosionComponent>,
                               public PhasedComponent
    {
    public:
        explicit ExplosionComponent(const ParticleEffectComponentPtr& pec,
            float blastRadius = 0.0f, UInt32 blastNumRays = 0);
        ~ExplosionComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void setLight(float distance, float duration, bool xray, float maxAlpha);

        void setShake(float amount);

        void setSound(const AudioSourcePtr& value);

        void setBlast(const SceneObjectPtr& sourceObj, float impulse, float damage, const SceneObjectTypes& types);

        inline void setColor(const Color& value) { color_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        ParticleEffectComponentPtr pec_;
        float shakeAmount_;
        AudioSourcePtr sound_;

        PointLightPtr light_;
        TweeningPtr tweening_;
        float tweenTime_;

        bool blast_;
        float blastRadius_;
        UInt32 blastNumRays_;
        float blastImpulse_;
        float blastDamage_;
        SceneObjectTypes blastTypes_;
        SceneObjectPtr blastSourceObj_;

        Color color_;
    };

    typedef boost::shared_ptr<ExplosionComponent> ExplosionComponentPtr;
}

#endif
