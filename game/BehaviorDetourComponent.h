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

#ifndef _BEHAVIORDETOURCOMPONENT_H_
#define _BEHAVIORDETOURCOMPONENT_H_

#include "BehaviorComponent.h"
#include "SceneObjectManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BehaviorDetourComponent : public boost::enable_shared_from_this<BehaviorDetourComponent>,
                                    public BehaviorComponent
    {
    public:
        BehaviorDetourComponent();
        ~BehaviorDetourComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void reset();

        virtual void start();

        virtual bool finished() const { return finished_; }

        inline float aheadDistance() const { return aheadDistance_; }
        inline void setAheadDistance(float value) { aheadDistance_ = value; }

        inline float seekAngle() const { return seekAngle_; }
        inline void setSeekAngle(float value) { seekAngle_ = value; }

        inline float angularVelocity() const { return angularVelocity_; }
        inline void setAngularVelocity(float value) { angularVelocity_ = value; }

        inline float timeout() const { return timeout_; }
        inline void setTimeout(float value) { timeout_ = value; }

        inline float rayDistance() const { return rayDistance_; }
        inline void setRayDistance(float value) { rayDistance_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        bool rayCast(const b2Vec2& pos, const b2Vec2& dir);

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, bool& found);

        void apply();

        bool started_;
        bool finished_;

        float aheadDistance_;
        float seekAngle_;
        float angularVelocity_;
        float timeout_;
        float rayDistance_;

        float t_;
        bool haveDir_;
        float dirAngle_;
        float dirFactor_;
    };

    typedef boost::shared_ptr<BehaviorDetourComponent> BehaviorDetourComponentPtr;
}

#endif
