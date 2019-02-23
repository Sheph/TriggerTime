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

#ifndef _WARDERCOMPONENT_H_
#define _WARDERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WarderComponent : public boost::enable_shared_from_this<WarderComponent>,
                            public TargetableComponent
    {
    public:
        enum
        {
            AnimationWalk = 1,
            AnimationMelee1,
            AnimationMelee2,
            AnimationMelee3,
            AnimationShoot
        };

        WarderComponent(const AnimationComponentPtr& ac,
            const b2Vec2 meleePos[3], const float meleeRadius[3]);
        ~WarderComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateMelee1,
            StateMelee2,
            StateMelee3,
            StateShoot,
        };

        virtual void onRegister();

        virtual void onUnregister();

        void resetShootTime();

        AnimationComponentPtr ac_;
        WeaponComponentPtr weapon_;
        b2Vec2 meleePos_[3];
        float meleeRadius_[3];

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float t_;
        bool actionDone_;

        AudioSourcePtr sndMelee_[3];
        AudioSourcePtr sndShoot_[2];
        AudioSourcePtr sndWalk_[3];
    };

    typedef boost::shared_ptr<WarderComponent> WarderComponentPtr;
}

#endif
