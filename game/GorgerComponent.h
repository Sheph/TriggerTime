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

#ifndef _GORGERCOMPONENT_H_
#define _GORGERCOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "AnimationComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GorgerComponent : public boost::enable_shared_from_this<GorgerComponent>,
                            public TargetableComponent
    {
    public:
        enum
        {
            AnimationAngry = 1,
            AnimationWalk = 2,
            AnimationMelee = 3,
            AnimationPreShoot = 4,
            AnimationShoot = 5,
            AnimationPostShoot = 6,
        };

        GorgerComponent(const AnimationComponentPtr& ac,
            const b2Vec2& meleePos, float meleeRadius);
        ~GorgerComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void onFreeze();

        inline const WeaponComponentPtr& weapon(int i) const { return weapon_[i]; }
        inline void setWeapon(int i, const WeaponComponentPtr& value) { weapon_[i] = value; }

        void setWalk();

    private:
        enum State
        {
            StateIdle = 0,
            StateAngry,
            StateWalk,
            StateAim,
            StateMelee,
            StatePreShoot,
            StateShoot,
            StatePostShoot,
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startMelee();

        void startShoot();

        AnimationComponentPtr ac_;
        b2Vec2 meleePos_;
        float meleeRadius_;

        WeaponComponentPtr weapon_[2];

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        bool meleeDone_;
        bool shootDone_;
        bool weaponDone_[2];
        float attackTime_;
        bool firstAngry_;

        AudioSourcePtr sndMelee_;
        AudioSourcePtr sndHit_;
        AudioSourcePtr sndShoot_[2];
        AudioSourcePtr sndAngry_;
    };

    typedef boost::shared_ptr<GorgerComponent> GorgerComponentPtr;
}

#endif
