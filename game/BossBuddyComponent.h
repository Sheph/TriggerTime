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

#ifndef _BOSSBUDDYCOMPONENT_H_
#define _BOSSBUDDYCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "WeaponSeekerComponent.h"
#include "Tweening.h"
#include "BuddySideComponent.h"
#include "SensorListener.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossBuddyComponent : public boost::enable_shared_from_this<BossBuddyComponent>,
                               public TargetableComponent,
                               public SensorListener
    {
    public:
        BossBuddyComponent(const BuddySideComponentPtr sides[3],
            const SceneObjectPtr& head, const SceneObjectPtr& headS,
            const b2Transform& tongueXf, const SceneObjectPtr pipes[2],
            const b2Transform eyeletXf[2], const SceneObjectPtr& brain,
            const SceneObjectPtr& root3, const SceneObjectPtr& root3S,
            const b2Transform& napalmXf, const SceneObjectPtr jaws[2],
            const SceneObjectPtr skull[2], const SceneObjectPtr& frontGun);
        ~BossBuddyComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

        void setDead();

        inline const WeaponComponentPtr& sideWeapon(int i) const { return sideWeapon_[i]; }
        inline void setSideWeapon(int i, const WeaponComponentPtr& value) { sideWeapon_[i] = value; }

        inline const WeaponSeekerComponentPtr& frontWeapon() const { return frontWeapon_; }
        inline void setFrontWeapon(const WeaponSeekerComponentPtr& value) { frontWeapon_ = value; }

        inline const WeaponComponentPtr& sideAltWeapon(int i) const { return sideAltWeapon_[i]; }
        inline void setSideAltWeapon(int i, const WeaponComponentPtr& value) { sideAltWeapon_[i] = value; }

        inline bool brainDead() const { return brain_->dead(); }

        inline bool canSwallow() const { return canSwallow_; }
        inline void setCanSwallow(bool value) { canSwallow_ = value; }

        inline bool deathFinished() const { return (state_ == StateDead) || (state_ == StateDead2); }

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateTongueOpenJaws,
            StateTongueExtend,
            StateTongueAttack,
            StateTongueRetract,
            StateTongueCloseJaws,
            StateBrainRetract,
            StateCloseSkull,
            StateSideGunExtend,
            StateSideGun,
            StateSideAltGun,
            StateSideGunPreRetract,
            StateSideGunRetract,
            StateFrontGunOpenJaws,
            StateFrontGunExtend,
            StateFrontGun,
            StateFrontGunRetract,
            StateFrontGunCloseJaws,
            StateNapalmOpenJaws,
            StateNapalmLeanBack,
            StateNapalmStrike,
            StateNapalmPost,
            StateNapalmCloseJaws,
            StateSwallowAim,
            StateSwallowLeanBack,
            StateSwallowStrike,
            StateDead,
            StateDead2,
            StateCaptured,
            StatePreOpenSkull,
            StateOpenSkull,
            StateBrainExtend,
            StateBrainExposed
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool tongueRoped() const;

        void tongueStartRetract();

        void tongueStopCaptured();

        void startWalk();

        void openJaw();

        void closeJaw();

        BuddySideComponentPtr sides_[3];
        SceneObjectPtr head_;
        SceneObjectPtr headS_;
        b2Transform tongueXf_;
        SceneObjectPtr pipes_[2];
        b2Transform eyeletXf_[2];
        SceneObjectPtr brain_;
        SceneObjectPtr root3_;
        SceneObjectPtr root3S_;
        b2Transform napalmXf_;
        SceneObjectPtr jaws_[2];
        SceneObjectPtr skull_[2];
        SceneObjectPtr frontGun_;
        SceneObjectPtr dummy_;

        RevoluteJointProxyPtr jawJoints_[2];
        RevoluteJointProxyPtr skullJoints_[2];
        PrismaticJointProxyPtr brainJoint_;
        PrismaticJointProxyPtr sideGunJoints_[2];
        PrismaticJointProxyPtr frontGunJoint_;

        State state_;

        b2Rot idleRot_;
        int idleRotDir_;
        float idleRotT_;
        float t_;

        SceneObjectPtr tongue_;
        PrismaticJointProxyPtr tongueJoint_;
        WeldJointProxyPtr tongueWeldJoint_;
        float tongueJointLen_;
        bool tongueDead_;

        SceneObjectPtr rope_[2];

        b2Vec2 origPos_;

        WeaponComponentPtr sideWeapon_[2];
        WeaponSeekerComponentPtr frontWeapon_;
        WeaponComponentPtr sideAltWeapon_[2];

        int attackNum_;
        int numMissiles_;
        bool canSwallow_;

        float prevHealth_;
        float painT_;

        AudioSourcePtr sndJawOpen_;
        AudioSourcePtr sndJawClose_;
        AudioSourcePtr sndSidegunExtend_;
        AudioSourcePtr sndSidegunRetract_;
        AudioSourcePtr sndFrontgunExtend_;
        AudioSourcePtr sndFrontgunRetract_;
        AudioSourcePtr sndTongueCaptured_;
        AudioSourcePtr sndBrainPain_[2];
    };

    typedef boost::shared_ptr<BossBuddyComponent> BossBuddyComponentPtr;
}

#endif
