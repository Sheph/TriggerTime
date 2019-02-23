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

#include "FollowPathComponent.h"
#include "SceneObject.h"
#include "Utils.h"
#include "Settings.h"

namespace af
{
    FollowPathComponent::FollowPathComponent()
    : PhasedComponent(phaseThink),
      t_(0.0f),
      lastPos_(0.0f)
    {
    }

    FollowPathComponent::~FollowPathComponent()
    {
    }

    void FollowPathComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void FollowPathComponent::update(float dt)
    {
        if (!path_ || !tweening_) {
            return;
        }

        if (tweening_->finished(t_)) {
            return;
        }

        t_ += dt;

        float pos = tweening_->getValue(t_);

        if (pos == lastPos_) {
            b2Vec2 p0 = b2Mul(pathXf_, lastIt_->current());

            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (p0 - parent()->pos()));

            parent()->setAngularVelocity(0.0f);

            return;
        }

        if (it_) {
            it_->step(pos - lastPos_);
        } else {
            it_ = path_->find(pos);
            it_->setLoop(tweening_->loop());
        }

        b2Vec2 p0 = b2Mul(pathXf_, lastIt_->current());
        b2Vec2 p1 = b2Mul(pathXf_, it_->current());

        parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (p0 - parent()->pos()));
        parent()->setAngularVelocity(0.0f);
        parent()->setAngle(vec2angle(p1 - p0));

        lastIt_->step(pos - lastPos_);
        lastPos_ = pos;
    }

    bool FollowPathComponent::finished() const
    {
        return !tweening_ || tweening_->finished(t_);
    }

    void FollowPathComponent::setPath(const PathPtr& value)
    {
        path_ = value;
        restart();
    }

    void FollowPathComponent::setTweening(const TweeningPtr& value)
    {
        tweening_ = value;
        restart();
    }

    void FollowPathComponent::setTime(float value)
    {
        restart(value);
    }

    void FollowPathComponent::onRegister()
    {
    }

    void FollowPathComponent::onUnregister()
    {
    }

    void FollowPathComponent::restart(float t)
    {
        t_ = t;
        lastPos_ = 0.0f;
        it_.reset();
        if (path_ && tweening_) {
            lastPos_ = tweening_->getValue(t_);
            lastIt_ = path_->find(lastPos_);
            lastIt_->setLoop(tweening_->loop());
        }
    }
}
