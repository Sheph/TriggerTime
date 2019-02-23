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

#ifndef _SPIDERCOMPONENT_H_
#define _SPIDERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "AudioManager.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SpiderComponent : public boost::enable_shared_from_this<SpiderComponent>,
                            public TargetableComponent
    {
    public:
        enum
        {
            AnimationAttack = 1
        };

        SpiderComponent(const SceneObjectPtr legs[8],
            const b2Vec2 legTips[8],
            const b2Vec2& bodyTip,
            const AnimationComponentPtr& ac);
        ~SpiderComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        inline void setLegVelocity(float value) { legVelocity_ = value; }
        inline float legVelocity() const { return legVelocity_; }

        inline bool canRotate() const { return canRotate_; }
        inline void setCanRotate(bool value) { canRotate_ = value; }

        void rotate();

        void angry();

        void trapped();

    private:
        enum State
        {
            StateStart = 0,
            StateDown,
            StateUp,
            StateRotate,
            StateAttack,
            StateAngry,
            StateTrapped1,
            StateTrapped2
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool legMove(int index, float distance);

        bool legDown(int index, float dt);

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, bool& found, b2Vec2& finalPoint);

        b2Vec2 findBestDir(const b2Vec2& p1, const b2Vec2& origDir,
            b2Vec2& bp1, b2Vec2& bp2);

        float findLegPos(const b2Vec2& p1, const b2Vec2& dir,
            b2Vec2& bp1, b2Vec2& bp2);

        bool queryAABBCb(b2Fixture* fixture, bool& found);

        SceneObjectPtr leg_[8];
        b2Vec2 legTip_[8];
        b2Vec2 bodyTip_;
        AnimationComponentPtr ac_;
        DistanceJointProxyPtr legJoint_[8];
        DistanceJointProxyPtr bodyJoint_;
        State state_;
        std::pair<int, int> legPair_[4];
        int legPairIndex_;
        float attackTime_;
        bool cornered_;

        b2Vec2 legPos_[8];
        SceneObjectPtr dummyObj_;
        b2Vec2 legDir_[4];
        bool downSlow_;

        float legVelocity_;
        bool canRotate_;

        TweeningPtr tweening_;
        float tweenTime_;
        float angryStartAngle_;
        bool trapped_;

        AudioSourcePtr sndBite_;
        AudioSourcePtr sndAngry_;
    };

    typedef boost::shared_ptr<SpiderComponent> SpiderComponentPtr;
}

#endif
