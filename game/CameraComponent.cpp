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

#include "CameraComponent.h"
#include "SceneObject.h"
#include "InputManager.h"
#include "Utils.h"
#include "Scene.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    CameraComponent::CameraComponent(float width, float height)
    : TargetableComponent(phasePreRender),
      sizeD2_(width / 2, height / 2),
      debugZoom_(0.0f),
      linearOffset_(b2Vec2_zero),
      angularOffset_(0.0f),
      zoom_(0.0f),
      shakeTweenTime_(0.0f),
      rollTweenTime_(0.0f),
      moveTweenTime_(0.0f),
      zoomTweenTime_(0.0f),
      tremor_(false),
      tremorOffset_(b2Vec2_zero),
      tremorTime_(0.0f),
      useTargetAngle_(false)
    {
        constraint_.lowerBound = constraint_.upperBound = b2Vec2_zero;
    }

    CameraComponent::~CameraComponent()
    {
    }

    void CameraComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void CameraComponent::preRender(float dt)
    {
        if (scene()->input()->zoomOutPressed()) {
            debugZoom_ += 40.0f * dt;
        }
        if (scene()->input()->zoomInPressed()) {
            debugZoom_ -= 40.0f * dt;
        }
        if (scene()->input()->zoomResetPressed()) {
            debugZoom_ = 0.0f;
        }

        updatePos(dt);
    }

    b2AABB CameraComponent::getAABB() const
    {
        return getAABBInternal(true);
    }

    b2AABB CameraComponent::getOOBB() const
    {
        b2Vec2 pos(0.0f, 0.0f);

        if (parent()) {
            pos = parent()->pos() + linearOffset_+ tremorOffset_;
        }

        b2Vec2 tmp = sizeD2_;

        tmp.y += (zoom_ + debugZoom_) * 0.5f;
        tmp.x += (zoom_ + debugZoom_) * 0.5f * (sizeD2_.x / sizeD2_.y);

        b2AABB aabb;

        aabb.lowerBound = pos - tmp;
        aabb.upperBound = pos + tmp;

        return aabb;
    }

    b2AABB CameraComponent::getTrueAABB() const
    {
        return getAABBInternal(false);
    }

    float CameraComponent::getActualAngle() const
    {
        return parent()->angle() + angularOffset_;
    }

    void CameraComponent::setTarget(const SceneObjectPtr& target)
    {
        TargetableComponent::setTarget(target);
        if (target) {
            parent()->setPos(constrainedPos(target->smoothPos()));
            updateAngle();
            moveTweening_.reset();
            moveToTarget_.reset();
        }
    }

    void CameraComponent::shake(float amount)
    {
        if (shakeTweeningX_ && (fabs(shakeTweeningX_->end()) > amount)) {
            return;
        }

        shakeTweeningX_ = boost::make_shared<SingleTweening>(1.0f, EaseOutVibrate,
            0.0f, (((rand() % 2) == 0) ? amount : -amount));
        shakeTweeningY_ = boost::make_shared<SingleTweening>(1.0f, EaseOutVibrate,
            0.0f, (((rand() % 2) == 0) ? amount : -amount));
        shakeTweenTime_ = 0.0f;
    }

    void CameraComponent::roll(float amount)
    {
        rollTweening_ = boost::make_shared<SingleTweening>(2.0f,
            VibrateFactor / 2.0f, EaseOutVibrate,
            0.0f, (((rand() % 2) == 0) ? amount : -amount));
        rollTweenTime_ = 0.0f;
    }

    void CameraComponent::moveTo(const b2Vec2& pos, Easing easing, float timeout)
    {
        moveFromPos_ = parent()->pos();
        moveToPos_ = pos;
        moveToTarget_.reset();
        moveTweening_ = boost::make_shared<SingleTweening>(timeout, easing);
        moveTweenTime_ = 0.0f;
        TargetableComponent::setTarget(SceneObjectPtr());
    }

    void CameraComponent::follow(const SceneObjectPtr& target, Easing easing, float timeout)
    {
        moveFromPos_ = parent()->pos();
        moveToTarget_ = target;
        moveTweening_ = boost::make_shared<SingleTweening>(timeout, easing);
        moveTweenTime_ = 0.0f;
        TargetableComponent::setTarget(SceneObjectPtr());
    }

    void CameraComponent::zoomTo(float value, Easing easing, float timeout)
    {
        if (timeout <= 0.0f) {
            zoom_ = value - sizeD2_.y * 2.0f;
            return;
        }

        zoomTweening_ = boost::make_shared<SingleTweening>(timeout,
            easing, zoom_, value - sizeD2_.y * 2.0f);
        zoomTweenTime_ = 0.0f;
    }

    void CameraComponent::tremorStart(float amount)
    {
        tremor_ = true;
        tremorTime_ = 0.0f;
        tremorNext_ = b2Vec2_zero;
        tremorAmount_ = amount;
    }

    void CameraComponent::tremor(bool value)
    {
        if (value) {
            tremorStart(0.2f);
        } else {
            tremor_ = value;
            tremorTime_ = 0.0f;
            tremorNext_ = b2Vec2_zero;
            tremorOffset_ = b2Vec2_zero;
        }
    }

    bool CameraComponent::pointVisible(const b2Vec2& pos) const
    {
        return pointInRect(pos, getTrueAABB());
    }

    bool CameraComponent::rectVisible(const b2Vec2& center, float width, float height) const
    {
        b2AABB aabb;

        aabb.lowerBound = center - b2Vec2(width / 2.0f, height / 2.0f);
        aabb.upperBound = center + b2Vec2(width / 2.0f, height / 2.0f);

        return b2TestOverlap(getTrueAABB(), aabb);
    }

    bool CameraComponent::entireRectVisible(const b2Vec2& center, float width, float height) const
    {
        b2AABB aabb;

        aabb.lowerBound = center - b2Vec2(width / 2.0f, height / 2.0f);
        aabb.upperBound = center + b2Vec2(width / 2.0f, height / 2.0f);

        return getTrueAABB().Contains(aabb);
    }

    b2Vec2 CameraComponent::getGamePoint(const b2Vec2& worldPoint) const
    {
        b2Vec2 res = worldPoint - getOOBB().lowerBound;

        res.x *= sizeD2_.x / ((zoom_ + debugZoom_) * 0.5f * (sizeD2_.x / sizeD2_.y) + sizeD2_.x);
        res.y *= sizeD2_.y / ((zoom_ + debugZoom_) * 0.5f + sizeD2_.y);

        return res;
    }

    b2Vec2 CameraComponent::getWorldPoint(const b2Vec2& gamePoint) const
    {
        b2Vec2 res = gamePoint;

        res.x *= ((zoom_ + debugZoom_) * 0.5f * (sizeD2_.x / sizeD2_.y) + sizeD2_.x) / sizeD2_.x;
        res.y *= ((zoom_ + debugZoom_) * 0.5f + sizeD2_.y) / sizeD2_.y;

        return res + getOOBB().lowerBound;
    }

    void CameraComponent::onRegister()
    {
    }

    void CameraComponent::onUnregister()
    {
        TargetableComponent::setTarget(SceneObjectPtr());
        moveToTarget_.reset();
    }

    void CameraComponent::updatePos(float dt)
    {
        if (target()) {
            parent()->setPos(constrainedPos(target()->smoothPos()));
        }

        updateAngle();

        if (moveTweening_) {
            float value = moveTweening_->getValue(moveTweenTime_);

            if (moveToTarget_) {
                parent()->setPos((1.0f - value) * moveFromPos_ + value * constrainedPos(moveToTarget_->smoothPos()));
            } else {
                parent()->setPos((1.0f - value) * moveFromPos_ + value * constrainedPos(moveToPos_));
            }

            moveTweenTime_ += dt;
            if (moveTweening_->finished(moveTweenTime_)) {
                moveTweening_.reset();
                if (moveToTarget_) {
                    setTarget(moveToTarget_);
                }
            }
        }

        if (rollTweening_) {
            angularOffset_ = rollTweening_->getValue(rollTweenTime_);

            rollTweenTime_ += dt;
            if (rollTweening_->finished(rollTweenTime_)) {
                rollTweening_.reset();
            }
        }

        if (shakeTweeningX_) {
            linearOffset_ = b2Vec2(shakeTweeningX_->getValue(shakeTweenTime_),
                shakeTweeningY_->getValue(shakeTweenTime_));

            shakeTweenTime_ += dt;
            if (shakeTweeningX_->finished(shakeTweenTime_)) {
                shakeTweeningX_.reset();
                shakeTweeningY_.reset();
            }
        }

        if (zoomTweening_) {
            zoomTweenTime_ += dt;

            zoom_ = zoomTweening_->getValue(zoomTweenTime_);

            if (zoomTweening_->finished(zoomTweenTime_)) {
                zoomTweening_.reset();
            }
        }

        if (tremor_) {
            tremorTime_ -= dt;

            if (tremorTime_ <= 0.0f) {
                tremorOffset_ = tremorNext_;
                tremorLast_ = tremorOffset_;
                tremorNext_ = b2Vec2(getRandom(-tremorAmount_, tremorAmount_),
                    getRandom(-tremorAmount_, tremorAmount_));
                tremorTime_ = 0.02f;
            } else {
                float a = (0.02f - tremorTime_) / 0.02f;
                tremorOffset_ = a * tremorNext_ + (1.0f - a) * tremorLast_;
            }
        }
    }

    b2AABB CameraComponent::getAABBInternal(bool withDebug) const
    {
        b2Transform xf;

        b2Vec2 tmp = sizeD2_;

        tmp.y += zoom_ * 0.5f;
        tmp.x += zoom_ * 0.5f * (sizeD2_.x / sizeD2_.y);

        if (withDebug) {
            tmp.y += debugZoom_ * 0.5f;
            tmp.x += debugZoom_ * 0.5f * (sizeD2_.x / sizeD2_.y);
        }

        if (parent()) {
            xf = b2Transform(parent()->pos() + linearOffset_ + tremorOffset_,
                b2Rot(parent()->angle() + angularOffset_));
        } else {
            xf.SetIdentity();
        }

        b2Vec2 v[] = {
            b2Mul(xf, b2Vec2(-tmp.x, -tmp.y)),
            b2Mul(xf, b2Vec2(-tmp.x, tmp.y)),
            b2Mul(xf, b2Vec2(tmp.x, tmp.y)),
            b2Mul(xf, b2Vec2(tmp.x, -tmp.y))
        };

        return computeAABB(v, sizeof(v)/sizeof(v[0]));
    }

    b2Vec2 CameraComponent::constrainedPos(b2Vec2 pos)
    {
        if (constraint_.lowerBound == constraint_.upperBound) {
            return pos;
        }

        b2Vec2 tmp = sizeD2_;

        tmp.y += zoom_ * 0.5f;
        tmp.x += zoom_ * 0.5f * (sizeD2_.x / sizeD2_.y);

        if ((pos.x + tmp.x) > constraint_.upperBound.x) {
            pos.x = constraint_.upperBound.x - tmp.x;
        }
        if ((pos.x - tmp.x) < constraint_.lowerBound.x) {
            pos.x = constraint_.lowerBound.x + tmp.x;
        }
        if ((pos.y + tmp.y) > constraint_.upperBound.y) {
            pos.y = constraint_.upperBound.y - tmp.y;
        }
        if ((pos.y - tmp.y) < constraint_.lowerBound.y) {
            pos.y = constraint_.lowerBound.y + tmp.y;
        }

        return pos;
    }

    void CameraComponent::updateAngle()
    {
        if (useTargetAngle_ && target()) {
            parent()->setAngle(-target()->smoothAngle());
        }
    }
}
