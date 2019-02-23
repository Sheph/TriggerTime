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

#ifndef _BEHAVIORROAMCOMPONENT_H_
#define _BEHAVIORROAMCOMPONENT_H_

#include "BehaviorComponent.h"
#include "Path.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BehaviorRoamComponent : public boost::enable_shared_from_this<BehaviorRoamComponent>,
                                  public BehaviorComponent
    {
    public:
        BehaviorRoamComponent();
        ~BehaviorRoamComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void changePath(const PathPtr& p, const b2Transform& xf);

        virtual void reset();

        virtual void start();

        virtual bool finished() const;

        inline const PathPtr& path() const { return path_; }
        inline const b2Transform& pathTransform() const { return pathXf_; }

        inline float linearVelocity() const { return linearVelocity_; }
        inline void setLinearVelocity(float value) { linearVelocity_ = value; }

        inline float linearDamping() const { return linearDamping_; }
        inline void setLinearDamping(float value) { linearDamping_ = value; }

        inline float dampDistance() const { return dampDistance_; }
        inline void setDampDistance(float value) { dampDistance_ = value; }

        inline bool changeAngle() const { return changeAngle_; }
        inline void setChangeAngle(bool value) { changeAngle_ = value; }

        inline float angleOffset() const { return angleOffset_; }
        inline void setAngleOffset(float value) { angleOffset_ = value; }

        inline bool paused() const { return paused_; }
        inline void setPaused(bool value) { paused_ = value; }

        void damp();

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void setupPath();

        void updateAngularVelocity(float value);

        bool started_;

        b2Transform pathXf_;
        PathPtr path_;
        float linearVelocity_;
        float linearDamping_;
        float dampDistance_;
        bool changeAngle_;
        float angleOffset_;

        PathPtr tmpPath_;
        PathIteratorPtr it_;
        PathIteratorPtr endIt_;
        float prevPos_;
        b2Vec2 prev_;
        float currentLinearVelocity_;

        bool paused_;
    };

    typedef boost::shared_ptr<BehaviorRoamComponent> BehaviorRoamComponentPtr;
}

#endif
