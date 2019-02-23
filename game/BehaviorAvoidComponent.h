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

#ifndef _BEHAVIORAVOIDCOMPONENT_H_
#define _BEHAVIORAVOIDCOMPONENT_H_

#include "BehaviorComponent.h"
#include "SceneObjectManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SceneObject;

    class BehaviorAvoidComponent : public boost::enable_shared_from_this<BehaviorAvoidComponent>,
                                   public BehaviorComponent
    {
    public:
        BehaviorAvoidComponent();
        ~BehaviorAvoidComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void reset();

        virtual void start();

        virtual bool finished() const { return finished_; }

        inline void setTypes(SceneObjectTypes value) { types_ = value; }

        inline float radius() const { return radius_; }
        inline void setRadius(float value) { radius_ = value; }

        inline float linearVelocity() const { return linearVelocity_; }
        inline void setLinearVelocity(float value) { linearVelocity_ = value; }

        inline float timeout() const { return timeout_; }
        inline void setTimeout(float value) { timeout_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        bool queryAABBCb(b2Fixture* fixture, b2Vec2& accum);

        void apply();

        static std::set<SceneObject*> tmpObjs_;

        bool started_;
        bool finished_;

        SceneObjectTypes types_;
        float radius_;
        float linearVelocity_;
        float timeout_;

        float t_;
        bool haveDir_;
        b2Vec2 dir_;
    };

    typedef boost::shared_ptr<BehaviorAvoidComponent> BehaviorAvoidComponentPtr;
}

#endif
