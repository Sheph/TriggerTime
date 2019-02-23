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

#ifndef _BOSSCHOPPERCOMPONENT_H_
#define _BOSSCHOPPERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include "WeaponSeekerComponent.h"
#include "ParticleEffectComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossChopperComponent : public boost::enable_shared_from_this<BossChopperComponent>,
                                 public TargetableComponent
    {
    public:
        typedef std::vector<WeaponComponentPtr> Weapons;

        explicit BossChopperComponent(const Points& explosionPos);
        ~BossChopperComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        inline void setWeaponGuns(const Weapons& value) { guns_ = value; }

        inline void setWeaponMissile(const WeaponSeekerComponentPtr& value) { weaponMissile_ = value; }

        inline bool canDie() const { return canDie_; }
        inline void setCanDie(bool value) { canDie_ = value; }

    private:
        enum State
        {
            StateInit = 0,
            StateStrafe,
            StateCharge,
            StateChargePost,
            StatePatrol,
            StateDie1,
            StateDie2,
            StateDie3
        };

        enum ShootState
        {
            ShootStateIdle = 0,
            ShootStateDelay,
            ShootStateGun
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startShoot();

        void stopShoot();

        void chargeDamage(float dt);

        Points explosionPos_;
        State state_;
        float t_;
        bool dir_;
        bool close_;
        Weapons guns_;
        WeaponSeekerComponentPtr weaponMissile_;
        bool canDie_;

        float origLinearDamping_;
        float origAngularDamping_;

        ShootState shootState_;
        float shootT_;
        bool shootStop_;
        b2Vec2 chargeDir_;
        b2Vec2 chargePos_;
        bool charged_;
        bool nextCharge_;
        int chargeProb_;

        ParticleEffectComponentPtr oldPec_;
        ParticleEffectComponentPtr pec_;

        AudioSourcePtr sndChopper_;
        AudioSourcePtr sndNapalm_;

        int explosionIndex_;
    };

    typedef boost::shared_ptr<BossChopperComponent> BossChopperComponentPtr;
}

#endif
