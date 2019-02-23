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

#ifndef _CENTIPEDECOMPONENT_H_
#define _CENTIPEDECOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class CentipedeComponent : public boost::enable_shared_from_this<CentipedeComponent>,
                               public TargetableComponent
    {
    public:
        enum
        {
            AnimationAttack = 1
        };

        CentipedeComponent(const std::vector<AnimationComponentPtr>& acs,
            const AnimationComponentPtr& headAc);
        ~CentipedeComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

        inline void setTrapped() { trapped_ = true; }

        inline bool spawnPowerup() const { return spawnPowerup_; }
        inline void setSpawnPowerup(bool value) { spawnPowerup_ = value; }

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateBite,
            StateShoot,
        };

        virtual void onRegister();

        virtual void onUnregister();

        float actualVelocity(float value);

        void resetShootTime();

        std::vector<AnimationComponentPtr> acs_;
        AnimationComponentPtr headAc_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float t_;
        float biteTime_;
        bool trapped_;
        bool spawnPowerup_;

        WeaponComponentPtr weapon_;

        AudioSourcePtr sndBite_;
        AudioSourcePtr sndAttack_[2];
        AudioSourcePtr sndDie_;
    };

    typedef boost::shared_ptr<CentipedeComponent> CentipedeComponentPtr;
}

#endif
