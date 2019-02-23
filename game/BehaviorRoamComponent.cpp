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

#include "BehaviorRoamComponent.h"
#include "HermitePath.h"
#include "SceneObject.h"
#include "Settings.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BehaviorRoamComponent::BehaviorRoamComponent()
    : BehaviorComponent(),
      started_(false),
      paused_(false)
    {
        reset();
    }

    BehaviorRoamComponent::~BehaviorRoamComponent()
    {
    }

    void BehaviorRoamComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BehaviorRoamComponent::update(float dt)
    {
        if (!started_ || finished() || parent()->stunned() || paused_) {
            return;
        }

        if (linearDamping_ == 0.0f) {
            currentLinearVelocity_ = linearVelocity_;
        } else {
            float tmp = linearVelocity_;

            if (currentLinearVelocity_ >= 0.0f) {
                if (!tmpPath_ && ((prevPos_ + dampDistance_) >= path_->length())) {
                    tmp = 0.0f;
                }
            } else {
                if (!tmpPath_ && ((prevPos_ - dampDistance_) <= 0.0f)) {
                    tmp = 0.0f;
                }
            }

            currentLinearVelocity_ +=
                ((settings.physics.fixedTimestep * linearDamping_) / (1.0f + settings.physics.fixedTimestep * linearDamping_)) *
                (tmp - currentLinearVelocity_);
        }

        if (fabs(currentLinearVelocity_) <= b2_linearSleepTolerance) {
            parent()->setLinearVelocity(b2Vec2_zero);
            updateAngularVelocity(0.0f);
            return;
        }

        it_->step(currentLinearVelocity_ * dt);

        if (it_->gtEq(endIt_)) {
            if (!tmpPath_ && !loop()) {
                if (dampDistance_ != 0.0f) {
                    parent()->setLinearVelocity(b2Vec2_zero);
                }
                updateAngularVelocity(0.0f);
                return;
            }

            float rem = tmpPath_ ? tmpPath_->length() : path_->length();
            rem -= prevPos_;
            prevPos_ = (currentLinearVelocity_ * dt) - rem;
            it_ = path_->find(prevPos_);
            endIt_ = path_->end();
            tmpPath_.reset();
        } else {
            prevPos_ += currentLinearVelocity_ * dt;
        }

        b2Vec2 cur = b2Mul(pathXf_, it_->current());

        parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (cur - parent()->pos()));

        if ((cur - prev_).LengthSquared() <= b2_linearSleepTolerance * b2_linearSleepTolerance) {
            updateAngularVelocity(0.0f);
        } else {
            float a;

            if (currentLinearVelocity_ < 0.0f) {
                a = vec2angle(prev_ - cur);
            } else {
                a = vec2angle(cur - prev_);
            }

            a += angleOffset_;

            updateAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(a - parent()->angle()));
        }

        prev_ = cur;
    }

    void BehaviorRoamComponent::changePath(const PathPtr& p, const b2Transform& xf)
    {
        pathXf_ = xf;
        path_= p;

        if (started_) {
            start();
        }
    }

    bool BehaviorRoamComponent::finished() const
    {
        return !started_ || (it_->gtEq(endIt_) && !tmpPath_ && !loop());
    }

    void BehaviorRoamComponent::damp()
    {
        setDampDistance(path_->length());
    }

    void BehaviorRoamComponent::reset()
    {
        if (started_ && !finished()) {
            parent()->setLinearVelocity(b2Vec2_zero);
            updateAngularVelocity(0.0f);
        }

        started_ = false;
        pathXf_.SetIdentity();
        path_.reset();
        linearVelocity_ = 0.0f;
        linearDamping_ = 0.0f;
        dampDistance_ = 0.0f;
        setLoop(false);
        changeAngle_ = true;
        angleOffset_ = 0.0f;
        tmpPath_.reset();
        prevPos_ = 0.0f;
        currentLinearVelocity_ = 0.0f;
    }

    void BehaviorRoamComponent::start()
    {
        if (!scene() || !path_) {
            return;
        }

        float c = 0.0f;

        HermitePathPtr hp = boost::dynamic_pointer_cast<HermitePath>(path_);

        if (hp) {
            c = hp->tension();
        }

        HermitePathPtr tmp = boost::make_shared<HermitePath>(c);

        tmp->add(b2MulT(pathXf_, parent()->pos() + parent()->getDirection(-1.0f)));
        tmp->add(b2MulT(pathXf_, parent()->pos()));
        tmp->add(path_->points()[0]);
        tmp->add(path_->points()[1]);
        tmp->add(path_->points()[2]);

        tmpPath_ = tmp;
        it_ = tmpPath_->first();
        endIt_ = tmpPath_->end();
        prevPos_ = 0.0f;
        prev_ = b2Mul(pathXf_, it_->current());
        currentLinearVelocity_ = parent()->linearVelocity().Length();

        if (tmp->points()[1] == tmp->points()[3]) {
            it_ = endIt_;
            prevPos_ = tmpPath_->length();
        }

        started_ = true;
    }

    void BehaviorRoamComponent::updateAngularVelocity(float value)
    {
        if (changeAngle_) {
            parent()->setAngularVelocity(value);
        }
    }

    void BehaviorRoamComponent::onRegister()
    {
    }

    void BehaviorRoamComponent::onUnregister()
    {
    }
}
