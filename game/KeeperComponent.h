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

#ifndef _KEEPERCOMPONENT_H_
#define _KEEPERCOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "AnimationComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class KeeperComponent : public boost::enable_shared_from_this<KeeperComponent>,
                            public TargetableComponent
    {
    public:
        enum
        {
            AnimationAngry = 1,
            AnimationWalk,
            AnimationMelee,
            AnimationGun,
            AnimationPrePlasma,
            AnimationPlasma,
            AnimationPostPlasma,
            AnimationMissile,
            AnimationCrawlOut,
            AnimationDeath,
        };

        KeeperComponent(const AnimationComponentPtr& ac,
            const b2Vec2& meleePos, float meleeRadius, bool folded);
        ~KeeperComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void onFreeze();

        inline void setWeaponGun(const WeaponComponentPtr& value) { weaponGun_ = value; }

        inline void setWeaponPlasma(const WeaponComponentPtr& value) { weaponPlasma_ = value; }

        inline void setWeaponMissile(const WeaponComponentPtr& value) { weaponMissile_ = value; }

        inline void crawlOut() { unfold_ = true; }

    private:
        enum State
        {
            StateFolded = 0,
            StateIdle,
            StateAngry,
            StateWalk,
            StateCloseup,
            StateMelee,
            StateGun,
            StatePrePlasma,
            StatePlasma,
            StatePostPlasma,
            StateMissile,
            StateCrawlOut,
        };

        virtual void onRegister();

        virtual void onUnregister();

        void startGun();

        void startPlasma();

        void startMissile();

        void startMelee();

        AnimationComponentPtr ac_;
        b2Vec2 meleePos_;
        float meleeRadius_;

        WeaponComponentPtr weaponGun_;
        WeaponComponentPtr weaponPlasma_;
        WeaponComponentPtr weaponMissile_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float attackTime_;
        bool weaponStarted_;
        bool weaponDone_;
        bool firstAngry_;
        bool unfold_;
        int origZOrder_;

        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndMelee_;
        AudioSourcePtr sndShoot_[2];
        AudioSourcePtr sndDeath_;
        AudioSourcePtr sndCrawlOut_;
    };

    typedef boost::shared_ptr<KeeperComponent> KeeperComponentPtr;
}

#endif
