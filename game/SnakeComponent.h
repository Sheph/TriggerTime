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

#ifndef _SNAKECOMPONENT_H_
#define _SNAKECOMPONENT_H_

#include "TargetableComponent.h"
#include "SensorListener.h"
#include "AudioManager.h"
#include "Tweening.h"
#include "WeaponComponent.h"
#include "RenderTentacleComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SnakeComponent : public boost::enable_shared_from_this<SnakeComponent>,
                           public TargetableComponent,
                           public SensorListener
    {
    public:
        SnakeComponent(const SceneObjectPtr& eye,
            const RenderTentacleComponentPtr& tentacle);
        ~SnakeComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

        void setIdle() { desiredState_ = StateIdle; }

        void setWalk() { desiredState_ = StateWalk; }

        void setMegaShoot(float force, float torque, float delay)
        {
            desiredState_ = StateMegaShoot;
            megaShootForce_ = force;
            megaShootTorque_ = torque;
            megaShootDelay_ = delay;
        }

        void setAngry(float force, float torque, float delay)
        {
            desiredState_ = StateAngry;
            megaShootForce_ = force;
            megaShootTorque_ = torque;
            megaShootDelay_ = delay;
        }

        void setDie(float headForce, float headTorque, float force, float torque);

        inline void setWeaponGun(const WeaponComponentPtr& value) { weaponGun_ = value; }

        inline void setWeaponMissile(const WeaponComponentPtr& value) { weaponMissile_ = value; }

        inline bool deathFinished() const { return state_ == StateDie2; }

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateBite1,
            StateBite2,
            StateShoot,
            StateMegaShoot,
            StateAngry,
            StateDie1,
            StateDie2
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startMegaShoot();

        void startWalk();

        void startAngry();

        void startDie();

        SceneObjectPtr eye_;

        State state_;
        State desiredState_;
        int numInside_;
        float t_;

        std::vector<RevoluteJointProxyPtr> fangJoints_;

        TweeningPtr tweening_;
        float tweenTime_;

        float megaShootForce_;
        float megaShootTorque_;
        float megaShootDelay_;

        bool inSight_;

        WeaponComponentPtr weaponGun_;
        WeaponComponentPtr weaponMissile_;

        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndMegaShoot_;
        AudioSourcePtr sndBite_;
        AudioSourcePtr sndSight_;
        AudioSourcePtr sndDie_;

        std::vector<SceneObjectPtr> dieObjs_;

        float dieForce_;
        float dieTorque_;
    };

    typedef boost::shared_ptr<SnakeComponent> SnakeComponentPtr;
}

#endif
