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

#ifndef _BOSSCOREPROTECTORCOMPONENT_H_
#define _BOSSCOREPROTECTORCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossCoreProtectorComponent : public boost::enable_shared_from_this<BossCoreProtectorComponent>,
                                       public TargetableComponent
    {
    public:
        BossCoreProtectorComponent(const SceneObjectPtr& head,
            const SceneObjectPtr arm[2],
            const b2Vec2 armTip[2],
            const b2Vec2 armElbow[2],
            const b2Vec2 armInitPos[2],
            const b2Vec2 armAimPos[2],
            const b2Vec2 armWeaponPos[2],
            const b2Vec2 armAngryPos[2][2],
            const b2Vec2 armDiePos[2][2]);
        ~BossCoreProtectorComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void setArena(const b2Vec2& arenaPos, const b2Vec2& startPos, const b2Vec2& diePos)
        {
            arenaPos_ = arenaPos;
            startPos_ = startPos;
            diePos_ = diePos;
            arenaRadius_ = (startPos_ - arenaPos_).Length();
        }

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

        void startAngry();

        inline void setDie() { die_ = true; }

    private:
        enum State
        {
            StateInit = 0,
            StateAttack1Aim,
            StateAttack1Strike,
            StateAttack2Aim,
            StateAttack2Strike,
            StateAttack3Aim,
            StateAttack3Strike,
            StateAngry1,
            StateAngry2,
            StateDie1,
            StateDie2,
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool updateArm(int index, float dt);

        void moveArm(int index, const b2Vec2& pos, float velocity, Easing easing, bool tip, bool haveSound = false);

        bool checkClose();

        bool checkFar();

        void moveToInit();

        void attackFinished();

        void createNormalMoveJoint();

        void createFastMoveJoint();

        void startAttack2();

        void attackSound();

        bool checkDie();

        SceneObjectPtr head_;
        SceneObjectPtr arm_[2];
        b2Vec2 armTip_[2];
        b2Vec2 armElbow_[2];
        b2Vec2 armInitPos_[2];
        b2Vec2 armAimPos_[2];
        b2Vec2 armWeaponPos_[2];
        b2Vec2 armAngryPos_[2][2];
        b2Vec2 armDiePos_[2][2];
        DistanceJointProxyPtr armJoint_[2];
        bool armHaveSound_[2];
        DistanceJointProxyPtr normalMoveJoint_;
        DistanceJointProxyPtr fastMoveJoint_;

        TweeningPtr armTweening_[8];
        float armTweenTime_[8];

        State state_;
        float t_;

        SceneObjectPtr dummyObj_;
        SceneObjectPtr moveObj_;
        b2Vec2 startPos_;
        b2Vec2 arenaPos_;
        b2Vec2 diePos_;
        float arenaRadius_;

        WeaponComponentPtr weapon_;
        int dir_;
        bool first_;
        int count_;
        bool started_;
        bool die_;

        AudioSourcePtr sndWhoosh_;
        AudioSourcePtr sndMove_;
        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndAngryHit_;
        AudioSourcePtr sndAttack_[4];
        AudioSourcePtr sndDie_;
        AudioSourcePtr sndDieCalmDown_;
    };

    typedef boost::shared_ptr<BossCoreProtectorComponent> BossCoreProtectorComponentPtr;
}

#endif
