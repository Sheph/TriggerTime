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

#ifndef _BOSSBEHOLDERCOMPONENT_H_
#define _BOSSBEHOLDERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include "WeaponComponent.h"
#include "Light.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossBeholderComponent : public boost::enable_shared_from_this<BossBeholderComponent>,
                                  public TargetableComponent
    {
    public:
        BossBeholderComponent(const SceneObjectPtr& legs,
            const SceneObjectPtr& guns,
            const LightPtr& light,
            const LightPtr fireLight[6]);
        ~BossBeholderComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& shieldWeapon(int i) const { return shield_[i]; }
        inline void setShieldWeapon(int i, const WeaponComponentPtr& value) { shield_[i] = value; }

        inline const WeaponComponentPtr& gunWeapon(int i) const { return gun_[i]; }
        inline void setGunWeapon(int i, const WeaponComponentPtr& value) { gun_[i] = value; }

        inline const WeaponComponentPtr& laserWeapon(int i) const { return laser_[i]; }
        inline void setLaserWeapon(int i, const WeaponComponentPtr& value) { laser_[i] = value; }

        inline const WeaponComponentPtr& aimWeapon(int i) const { return aim_[i]; }
        inline void setAimWeapon(int i, const WeaponComponentPtr& value) { aim_[i] = value; }

        inline bool deathFinished() const { return state_ == StateDie2; }

    private:
        enum State
        {
            StateInit = 0,
            StatePatrol,
            StateAngry1,
            StateAngry2,
            StateAngry3,
            StateAngry4,
            StateWalk,
            StateDie1,
            StateDie2
        };

        enum ShootState
        {
            ShootStateIdle = 0,
            ShootStateGunExtend,
            ShootStateGun,
            ShootStateGunRetract,
            ShootStateLaserStart,
            ShootStateLaserExtend,
            ShootStateLaserAim,
            ShootStateLaser,
            ShootStateLaserPost,
            ShootStateLaserRetract,
            ShootStateNapalmPre,
            ShootStateNapalmPre2,
            ShootStateNapalm,
            ShootStateNapalmPost,
            ShootStateMegaPre1,
            ShootStateMegaPre2,
            ShootStateMegaPre3,
            ShootStateMega,
            ShootStateMegaPost1,
            ShootStateMegaPost2,
            ShootStateMegaPost3,
            ShootStateDie
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startPatrol();

        void startWalkVerySlow();

        void startWalkSlow(bool move);

        void startWalkFast(bool move, bool sound = true);

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, bool& hit);

        static const int numCheckRays = 15;

        SceneObjectPtr legs_;
        SceneObjectPtr guns_;
        LightPtr light_;

        WeaponComponentPtr shield_[3];
        WeaponComponentPtr gun_[8];
        WeaponComponentPtr laser_[8];
        WeaponComponentPtr aim_[8];

        State state_;
        float t_;

        ShootState shootState_;
        float shootT_;
        int shootNum_;
        int gunNum_;
        float megaTime_;

        int legsAccelerate_;

        TweeningPtr lightTweening_;
        float lightTweenTime_;
        bool firstPatrol_;

        PrismaticJointProxyPtr legJoint_[8];
        PrismaticJointProxyPtr gunJoint_[8];

        b2Vec2 walkDir_;
        b2Rot walkRot_;
        b2Rot checkRot_;

        float walkDuration_;
        float walkLinearVelocity_;
        float walkAngularVelocity_;
        float walkCheckDistance_;

        b2Vec2 patrolPoint_;

        LightPtr fireLight_[6];
        float fireTime_[6];

        AudioSourcePtr sndShake_;
        AudioSourcePtr sndCharge_;
        AudioSourcePtr sndLaser_;
    };

    typedef boost::shared_ptr<BossBeholderComponent> BossBeholderComponentPtr;
}

#endif
