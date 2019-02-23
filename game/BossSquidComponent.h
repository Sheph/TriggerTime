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

#ifndef _BOSSSQUIDCOMPONENT_H_
#define _BOSSSQUIDCOMPONENT_H_

#include "TargetableComponent.h"
#include "AudioManager.h"
#include "SingleTweening.h"
#include "TentacleSquidComponent.h"
#include "TentacleAttractComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossSquidComponent : public boost::enable_shared_from_this<BossSquidComponent>,
                               public TargetableComponent
    {
    public:
        BossSquidComponent(const SceneObjectPtr tentacle[8],
            const SceneObjectPtr legControl[4],
            const SceneObjectPtr eyeControl[4],
            const SceneObjectPtr mouthControl[4],
            const SceneObjectPtr leg[4],
            const SceneObjectPtr mouth[4],
            const Points& explosionPos);
        ~BossSquidComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline bool deathFinished() const { return state_ == StateDie2; }

    private:
        enum State
        {
            StateInit = 0,
            StateFirstSpawnEyes,
            StateFirstWait,
            StateFirstAngry,
            StateMouthOpen,
            StateMouthOpened,
            StateMouthClose,
            StateRetractLegs,
            StateDie1,
            StateDie2,
            StateIdle,
            StateLegAim,
            StateLegDown,
            StateSpawnEyes,
            StateSpin1,
            StateSpin2,
            StateSpin3,
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool updateLeg(int index, float dt);

        void moveLeg(int index, float distance, float duration, Easing easing);

        bool updateEye(int index, float dt);

        void moveEye(int index, float distance, float duration, Easing easing);

        bool updateMouth(int index, float dt);

        void moveMouth(int index, float distance, float duration, Easing easing);

        bool updateBody(float dt);

        void moveBodyPain(float duration, float angle);

        void moveBodyRotate(float velocity);

        void moveBodyInit();

        void spawnEyes();

        void showHealthbar();

        State state_;

        TentacleSquidComponentPtr tentacle_[8];
        SceneObjectPtr legControl_[4];
        SceneObjectPtr eyeControl_[4];
        SceneObjectPtr eye_[4];
        SceneObjectPtr mouthControl_[4];
        SceneObjectPtr leg_[4];
        SceneObjectPtr mouth_[4];
        Points explosionPos_;

        TweeningPtr legTweening_[4];
        float legTweenTime_[4];

        int legIndex_;

        TweeningPtr eyeTweening_[4];
        float eyeTweenTime_[4];
        bool eyeDying_[4];

        TweeningPtr mouthTweening_[4];
        float mouthTweenTime_[4];

        b2Vec2 eyeControlInit_[4];
        b2Vec2 mouthControlInit_[4];
        float eyeControlInitAngle_[4];
        float mouthControlInitAngle_[4];
        TweeningPtr bodyTweening_;
        float bodyTweenTime_;

        std::vector<TentacleAttractComponentPtr> welds_;

        float t_;

        bool upDown_;

        int explosionIndex_;

        AudioSourcePtr sndSpawnEyes_;
        AudioSourcePtr sndAngry_;
        AudioSourcePtr sndLegHit_;
        AudioSourcePtr sndPain_;
        AudioSourcePtr sndMouthMove_;
        AudioSourcePtr sndDie_;
    };

    typedef boost::shared_ptr<BossSquidComponent> BossSquidComponentPtr;
}

#endif
