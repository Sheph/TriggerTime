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

#ifndef _WEAPONMACHINEGUNCOMPONENT_H_
#define _WEAPONMACHINEGUNCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include "RenderQuadComponent.h"
#include "AnimationComponent.h"
#include "PointLight.h"
#include "LightComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponMachineGunComponent : public boost::enable_shared_from_this<WeaponMachineGunComponent>,
                                      public WeaponComponent
    {
    public:
        explicit WeaponMachineGunComponent(SceneObjectType objectType);
        ~WeaponMachineGunComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float velocity() const { return velocity_; }
        inline void setVelocity(float value) { velocity_ = value; }

        inline float spreadAngle() const { return spreadAngle_; }
        inline void setSpreadAngle(float value) { spreadAngle_ = value; }

        inline float turnInterval() const { return turnInterval_; }
        inline void setTurnInterval(float value) { turnInterval_ = value; }

        inline float turnDuration() const { return turnDuration_; }
        inline void setTurnDuration(float value) { turnDuration_ = value; }

        inline float loopDelay() const { return loopDelay_; }
        inline void setLoopDelay(float value) { loopDelay_ = value; }

        inline bool tweakPos() const { return tweakPos_; }
        inline void setTweakPos(bool value) { tweakPos_ = value; }

        inline const b2Vec2& tweakOffset() const { return tweakOffset_; }
        inline void setTweakOffset(const b2Vec2& value) { tweakOffset_ = value; }

        inline float bulletHeight() const { return bulletHeight_; }
        inline void setBulletHeight(float value) { bulletHeight_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float damage_;
        float velocity_;
        float spreadAngle_;
        float turnInterval_;
        float turnDuration_;
        float loopDelay_;
        float t_;
        float turnT_;
        float flashT_;
        AudioSourcePtr snd_[4];
        RenderQuadComponentPtr flash_;
        AnimationComponentPtr flashAc_;
        PointLightPtr light_;
        LightComponentPtr lightC_;
        bool tweakPos_;
        b2Vec2 tweakOffset_;
        float bulletHeight_;
    };

    typedef boost::shared_ptr<WeaponMachineGunComponent> WeaponMachineGunComponentPtr;
}

#endif
