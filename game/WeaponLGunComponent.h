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

#ifndef _WEAPONLGUNCOMPONENT_H_
#define _WEAPONLGUNCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include "LightComponent.h"
#include "LineLight.h"
#include "PointLight.h"
#include "RenderLBoltComponent.h"
#include "ParticleEffectComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponLGunComponent : public boost::enable_shared_from_this<WeaponLGunComponent>,
                                public WeaponComponent
    {
    public:
        explicit WeaponLGunComponent(SceneObjectType objectType);
        ~WeaponLGunComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void onFreeze();

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float length() const { return length_; }
        inline void setLength(float value) { length_ = value; }

        inline float impulse() const { return impulse_; }
        inline void setImpulse(float value) { impulse_ = value; }

        inline int I() const { return i_; }
        inline void setI(int value) { i_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
            SceneObject*& obj);

        SceneObjectPtr fakeMissile_;
        RenderLBoltComponentPtr rc_[5];
        LightComponentPtr lightC_;
        LineLightPtr light_;
        PointLightPtr sparkLight_;
        PointLightPtr flashLight_;
        ParticleEffectComponentPtr pec_;
        ParticleEffectComponentPtr flashPec_;
        float damage_;
        float length_;
        float impulse_;
        float t_;
        int i_;

        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<WeaponLGunComponent> WeaponLGunComponentPtr;
}

#endif
