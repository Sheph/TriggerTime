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

#ifndef _BOSSQUEENCOMPONENT_H_
#define _BOSSQUEENCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossQueenComponent : public boost::enable_shared_from_this<BossQueenComponent>,
                               public TargetableComponent
    {
    public:
        enum
        {
            AnimationAttack = 1
        };

        BossQueenComponent(const SceneObjectPtr legs[8],
            const b2Vec2 legTips[8],
            const b2Vec2& bodyTip,
            const b2Vec2 cover[4],
            const Points& spawn,
            const b2Vec2 dead[4],
            const AnimationComponentPtr& ac);
        ~BossQueenComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline bool deathStarted() const { return state_ == StateDie; }

        inline bool deathFinished() const { return deathFinished_; }

        inline void trapped() { state_ = StateTrapped1; stateTime_ = 0.0f; trapped_ = true; }

        inline void angry() { state_ = StateAttack4; stateTime_ = 0.0f; }

    private:
        enum State
        {
            StateAttack1 = 0,
            StateAttack2,
            StateAttack3,
            StateAttack4,
            StateDie,
            StateTrapped1,
            StateTrapped2,
        };

        static const int StateMax = StateAttack4;

        enum SubState
        {
            SubStateInitial = 0,
            SubStateAim,
            SubStateMelee,
        };

        enum LegState
        {
            LegStateDown = 0,
            LegStateMeleeAim,
            LegStateMeleeStrike,
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool updateAttack1(float dt, bool calmDown);

        bool updateAttack2(float dt, bool calmDown);

        bool updateAttack3(float dt, bool calmDown);

        bool updateAttack4(float dt, bool calmDown);

        void updateDie(float dt);

        bool updateLeg(int index, float dt);

        bool updateBody(float dt);

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, bool& found, b2Vec2& finalPoint);

        void moveLeg(int index, const b2Vec2& pos, float velocity, Easing easing);

        void moveBody(const b2Vec2& pos, float velocity, Easing easing);

        SceneObjectPtr leg_[8];
        b2Vec2 legTip_[8];
        b2Vec2 legDefaultPos_[8];
        b2Vec2 bodyTip_;
        b2Vec2 bodyDefaultPos_;
        b2Vec2 bodyDefaultDir_;
        DistanceJointProxyPtr legJoint_[8];
        DistanceJointProxyPtr bodyJoint_;
        b2Vec2 cover_[4];
        b2Vec2 dead_[4];
        Points spawn_;
        AnimationComponentPtr ac_;

        LegState legState_[8];
        TweeningPtr legTweening_[8];
        float legTweenTime_[8];

        TweeningPtr bodyTweening_;
        float bodyTweenTime_;

        bool biteDone_;
        float biteTime_;

        State state_;
        float stateTime_;
        int newState_;
        SubState subState_;

        SceneObjectPtr dummyObj_;

        std::vector<SceneObjectPtr> spawned_;
        bool shaked_;

        float goalTime_;
        ComponentPtr goal_;

        bool deathFinished_;
        bool trapped_;

        AudioSourcePtr sndAim_;
        AudioSourcePtr sndHit_[8];
        AudioSourcePtr sndShake_;
        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndBite_;
        AudioSourcePtr sndSpawn_;
        AudioSourcePtr sndStrike_;
        AudioSourcePtr sndDie_;
    };

    typedef boost::shared_ptr<BossQueenComponent> BossQueenComponentPtr;
}

#endif
