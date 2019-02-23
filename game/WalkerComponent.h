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

#ifndef _WALKERCOMPONENT_H_
#define _WALKERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include "WeaponSeekerComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WalkerComponent : public boost::enable_shared_from_this<WalkerComponent>,
                            public TargetableComponent
    {
    public:
        WalkerComponent(const SceneObjectPtr& heart,
            const SceneObjectPtr legs[4],
            const b2Vec2 legTips[4]);
        ~WalkerComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        inline const WeaponSeekerComponentPtr& weapon(int i) const { return weapon_[i]; }
        inline void setWeapon(int i, const WeaponSeekerComponentPtr& value) { weapon_[i] = value; }

        inline const WeaponComponentPtr& exposedWeapon(int i) const { return exposedWeapon_[i]; }
        inline void setExposedWeapon(int i, const WeaponComponentPtr& value) { exposedWeapon_[i] = value; }

        inline void expose() { expose_ = true; }

        inline void angry() { angry_ = true; }

        inline bool deathFinished() const { return state_ == StateDie3; }

    private:
        enum State
        {
            StateInit = 0,
            StateDown,
            StateUp,
            StateExposing,
            StateExposed,
            StateHiding,
            StateAngry,
            StateDie1,
            StateDie2,
            StateDie3
        };

        virtual void onRegister();

        virtual void onUnregister();

        void move(int index, float distance);

        bool updateLeg(int index, float dt);

        void moveLegUp(int index, float distance, float velocity, Easing easing);

        void moveLegDown(int index, float velocity, Easing easing);

        bool updateBody(float dt);

        void moveBody(float distance, float velocity, Easing easing);

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint);

        void startDie2();

        SceneObjectPtr heart_;

        SceneObjectPtr legs_[4];
        b2Vec2 legTips_[4];
        DistanceJointProxyPtr bodyJoint_;
        DistanceJointProxyPtr legJoint_[4];
        State state_;
        int legIndex_;

        TweeningPtr legTweening_[4];
        float legTweenTime_[4];

        TweeningPtr bodyTweening_;
        float bodyTweenTime_;

        SceneObjectPtr dummyObj_;

        b2Vec2 legPos_[4];
        b2Vec2 legDir_[2];

        std::vector<RevoluteJointProxyPtr> motorJoints_;

        WeaponSeekerComponentPtr weapon_[4];
        float shootT_;
        bool expose_;

        RevoluteJointProxyPtr doorJoint_[2];
        float t_;

        TweeningPtr exposedTweening_;
        float exposedTweenTime_;
        WeaponComponentPtr exposedWeapon_[4];
        int doorIndex_;
        float healthBefore_;

        bool angry_;
        TweeningPtr angryTweening_;
        float angryTweenTime_;

        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndStep_[2];
        AudioSourcePtr sndExposed_;
        AudioSourcePtr sndDie_;
        bool stepSounded_;
        std::vector<SceneObjectPtr> dieObjs_;
        b2Vec2 dieVelDir_;
        b2Rot dieRot_;
    };

    typedef boost::shared_ptr<WalkerComponent> WalkerComponentPtr;
}

#endif
