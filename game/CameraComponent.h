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

#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "SingleTweening.h"
#include <Box2D/Box2D.h>
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class CameraComponent : public boost::enable_shared_from_this<CameraComponent>,
                            public TargetableComponent
    {
    public:
        CameraComponent(float width, float height);
        ~CameraComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        b2AABB getAABB() const;

        b2AABB getOOBB() const;

        b2AABB getTrueAABB() const;

        float getActualAngle() const;

        virtual void setTarget(const SceneObjectPtr& target);

        void shake(float amount);

        void roll(float amount);

        void moveTo(const b2Vec2& pos, Easing easing, float timeout);
        inline bool isMoving() const { return !!moveTweening_; }

        void follow(const SceneObjectPtr& target, Easing easing, float timeout);

        void zoomTo(float value, Easing easing, float timeout);

        void tremorStart(float amount);

        void tremor(bool value);

        bool pointVisible(const b2Vec2& pos) const;

        bool rectVisible(const b2Vec2& center, float width, float height) const;

        bool entireRectVisible(const b2Vec2& center, float width, float height) const;

        b2Vec2 getGamePoint(const b2Vec2& worldPoint) const;

        b2Vec2 getWorldPoint(const b2Vec2& gamePoint) const;

        inline const b2AABB& constraint() const { return constraint_; }
        inline void setConstraint(const b2Vec2& lowerBound, const b2Vec2& upperBound)
        {
            constraint_.lowerBound = lowerBound;
            constraint_.upperBound = upperBound;
        }

        inline const SceneObjectPtr& moveToTarget() const { return moveToTarget_; }

        inline void setUseTargetAngle(bool value) { useTargetAngle_ = value; }
        inline bool useTargetAngle() const { return useTargetAngle_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void updatePos(float dt);

        b2AABB getAABBInternal(bool withDebug) const;

        b2Vec2 constrainedPos(b2Vec2 pos);

        void updateAngle();

        b2Vec2 sizeD2_;
        float debugZoom_;

        b2Vec2 linearOffset_;
        float angularOffset_;
        float zoom_;

        SingleTweeningPtr shakeTweeningX_;
        SingleTweeningPtr shakeTweeningY_;
        float shakeTweenTime_;

        TweeningPtr rollTweening_;
        float rollTweenTime_;

        b2Vec2 moveFromPos_;
        b2Vec2 moveToPos_;
        SceneObjectPtr moveToTarget_;
        TweeningPtr moveTweening_;
        float moveTweenTime_;

        TweeningPtr zoomTweening_;
        float zoomTweenTime_;

        bool tremor_;
        b2Vec2 tremorNext_;
        b2Vec2 tremorLast_;
        b2Vec2 tremorOffset_;
        float tremorTime_;
        float tremorAmount_;

        b2AABB constraint_;

        bool useTargetAngle_;
    };

    typedef boost::shared_ptr<CameraComponent> CameraComponentPtr;
}

#endif
