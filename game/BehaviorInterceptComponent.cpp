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

#include "BehaviorInterceptComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Utils.h"

namespace af
{
    BehaviorInterceptComponent::BehaviorInterceptComponent()
    : started_(false)
    {
        reset();
    }

    BehaviorInterceptComponent::~BehaviorInterceptComponent()
    {
    }

    void BehaviorInterceptComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BehaviorInterceptComponent::update(float dt)
    {
        if (!started_ || !target_ || parent()->stunned()) {
            return;
        }

        float vel;

        if (parentLinearVelocity_ != 0.0f) {
            vel = parentLinearVelocity_;
        } else {
            vel = parent()->linearVelocity().Length();
        }

        b2Vec2 targetDir = target_->pos() - parent()->pos();

        if (vel > 0.1f) {
            b2Vec2 targetPos;

            if (target_->linearVelocity().LengthSquared() > 0.1f) {
                targetPos = solveLinearIntercept(target_->pos(),
                    target_->linearVelocity(), parent()->pos(), vel);

                if (targetPos == target_->pos()) {
                    if (b2Dot(target_->linearVelocity(), targetDir) < 0.0f) {
                        b2Vec2 dist = linePointDistance(parent()->pos(),
                            target_->pos(), target_->pos() + target_->linearVelocity());
                        targetPos = parent()->pos() + dist;
                    } else {
                        targetPos = target_->pos();
                    }
                }
            } else {
                targetPos = target_->pos();
            }

            targetDir = targetPos - parent()->pos();
        }

        if (angularVelocity_ > 0.0f) {
            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                parent()->applyTorque(parent()->inertia() * (angularVelocity_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            } else {
                parent()->applyTorque(parent()->inertia() * (-angularVelocity_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            }
        }

        if ((linearVelocity_ > 0.0f) && (parent()->linearVelocity().Length() < linearVelocity_)) {
            targetDir.Normalize();

            parent()->applyForceToCenter(parent()->mass() *
                (linearVelocity_ + parent()->linearVelocityDamped()) * targetDir, true);
        }
    }

    void BehaviorInterceptComponent::reset()
    {
        started_ = false;
        target_.reset();
        angularVelocity_ = 0.0f;
        linearVelocity_ = 0.0f;
        parentLinearVelocity_ = 0.0f;
        setLoop(false);
    }

    void BehaviorInterceptComponent::start()
    {
        if (!scene()) {
            return;
        }

        started_ = true;
    }

    void BehaviorInterceptComponent::onRegister()
    {
    }

    void BehaviorInterceptComponent::onUnregister()
    {
        target_.reset();
    }
}
