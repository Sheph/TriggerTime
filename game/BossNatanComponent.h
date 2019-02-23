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

#ifndef _BOSSNATANCOMPONENT_H_
#define _BOSSNATANCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "ParticleEffectComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossNatanComponent : public boost::enable_shared_from_this<BossNatanComponent>,
                               public TargetableComponent
    {
    public:
        enum
        {
            AnimationMelee = 1,
            AnimationPreShoot,
            AnimationPreShoot2,
            AnimationPostShoot,
            AnimationShoot,
            AnimationSyringe,
            AnimationRam,
            AnimationPowerup,
            AnimationDie,
            AnimationDead,
        };

        BossNatanComponent(const AnimationComponentPtr& ac,
            const ParticleEffectComponentPtr pec[2],
            const b2Vec2& meleePos, float meleeRadius);
        ~BossNatanComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        inline const WeaponComponentPtr& weapon(int i) const { return weapon_[i]; }
        inline void setWeapon(int i, const WeaponComponentPtr& value) { weapon_[i] = value; }

        inline const Points& teleportDests() const { return teleportDests_; }
        inline void setTeleportDests(const Points& value) { teleportDests_ = value; }

        inline const SceneObjectPtr& powerupObj() const { return powerupObj_; }
        inline void setPowerupObj(const SceneObjectPtr& value) { powerupObj_ = value; }

        inline bool needPowerup() const { return state_ == StatePowerup2; }

        inline bool deathFinished() const { return state_ == StateDead; }

        void setupInitial();

        void startNapalm();

        void finishNapalm();

        void finishPowerup();

        void startPowerupAnimation();

        void finishPowerupAnimation();

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateMelee,
            StateSyringe,
            StatePreTeleport,
            StateTeleport,
            StatePostTeleport,
            StatePreShoot,
            StateShoot,
            StatePostShoot,
            StatePreRam,
            StateRam,
            StatePostRam,
            StatePowerup,
            StatePowerup2,
            StatePowerup3,
            StateDie,
            StateDead
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startWalk();

        void startTeleport();

        void setupHealth(bool vulnerable);

        bool checkPowerup();

        AnimationComponentPtr ac_;
        ParticleEffectComponentPtr pec_[2];
        WeaponComponentPtr weapon_[3];
        b2Vec2 meleePos_;
        float meleeRadius_;
        Points teleportDests_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float t_;
        float rotateT_;
        bool rotateDir_;
        bool attackDone_[2];
        float syringeT_;
        UInt32 filterMaskBits_;
        int shootNum_;
        int weaponIndex_;

        ParticleEffectComponentPtr ramPec_;

        SceneObjectPtr powerupObj_;

        b2Rot dieRot_;
        float dieT_;

        AudioSourcePtr sndTeleOut_;
        AudioSourcePtr sndTeleIn_;
        AudioSourcePtr sndShoot_[2];
        AudioSourcePtr sndRam_;
        AudioSourcePtr sndPowerup_;
        AudioSourcePtr sndMelee_[2];
        AudioSourcePtr sndSyringePain_;
        AudioSourcePtr sndFinishPowerup_;
    };

    typedef boost::shared_ptr<BossNatanComponent> BossNatanComponentPtr;
}

#endif
