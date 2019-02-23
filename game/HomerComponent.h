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

#ifndef _HOMERCOMPONENT_H_
#define _HOMERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponSeekerComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class HomerComponent : public boost::enable_shared_from_this<HomerComponent>,
                           public TargetableComponent
    {
    public:
        enum
        {
            AnimationMelee = 1,
            AnimationPreGun,
            AnimationGun,
            AnimationPostGun,
            AnimationPreMissile,
            AnimationMissile,
            AnimationPostMissile
        };

        explicit HomerComponent(const AnimationComponentPtr& ac,
            const b2Vec2& meleePos, float meleeRadius);
        ~HomerComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline void setWeaponGun(const WeaponComponentPtr& value) { weaponGun_ = value; }

        inline void setWeaponMissile(const WeaponSeekerComponentPtr& value) { weaponMissile_ = value; }

    private:
        enum State
        {
            StateIdle = 0,
            StateMelee,
            StatePreGun,
            StateGun,
            StatePostGun,
            StatePreMissile,
            StateMissile,
            StatePostMissile
        };

        virtual void onRegister();

        virtual void onUnregister();

        AnimationComponentPtr ac_;
        b2Vec2 meleePos_;
        float meleeRadius_;
        WeaponComponentPtr weaponGun_;
        WeaponSeekerComponentPtr weaponMissile_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float t_;
        float rotateT_;
        bool hadTarget_;
        bool rotateDir_;
        bool missileDone_;
        bool meleeDone_;

        AudioSourcePtr sndMissile_[1];
        AudioSourcePtr sndGun_[2];
        AudioSourcePtr sndMelee_[2];
    };

    typedef boost::shared_ptr<HomerComponent> HomerComponentPtr;
}

#endif
