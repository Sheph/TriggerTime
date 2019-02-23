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

#include "BehaviorSeekComponent.h"
#include "SceneObject.h"
#include "Settings.h"
#include "Utils.h"

namespace af
{
    BehaviorSeekComponent::BehaviorSeekComponent()
    : started_(false)
    {
        reset();
    }

    BehaviorSeekComponent::~BehaviorSeekComponent()
    {
    }

    void BehaviorSeekComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BehaviorSeekComponent::update(float dt)
    {
        if (!started_ || finished() || !target_ || parent()->stunned()) {
            return;
        }

        b2Vec2 dir = parent()->getDirection(1.0f);

        b2Vec2 targetDir = target_->pos() - parent()->pos();

        if (useTorque_) {
            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                parent()->applyTorque(parent()->inertia() * (angularVelocity_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            } else {
                parent()->applyTorque(parent()->inertia() * (-angularVelocity_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            }
        } else {
            float angle = angleBetween(dir, targetDir);

            float angVelocity = 0.0f;

            if (angle >= 0.0f) {
                angVelocity = angularVelocity_;
            } else {
                angVelocity = -angularVelocity_;
            }

            if (fabs(angVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angVelocity = angle / settings.physics.fixedTimestep;
            }

            if ((fabs(angVelocity) <= b2_epsilon) && !loop()) {
                angVelocity = 0.0f;
                finished_ = true;
            }

            if (joint_) {
                joint_->setMotorSpeed(angVelocity);
            } else {
                parent()->setAngularVelocity(angVelocity);
            }
        }
    }

    void BehaviorSeekComponent::reset()
    {
        if (started_ && !finished()) {
            if (joint_) {
                joint_->setMotorSpeed(0);
            } else {
                parent()->setAngularVelocity(0);
            }
        }

        started_ = false;
        finished_ = false;
        angularVelocity_ = 0.0f;
        target_.reset();
        useTorque_ = false;
        joint_.reset();
        setLoop(false);
    }

    void BehaviorSeekComponent::start()
    {
        if (!scene()) {
            return;
        }

        started_ = true;
        finished_ = false;
    }

    void BehaviorSeekComponent::onRegister()
    {
    }

    void BehaviorSeekComponent::onUnregister()
    {
        target_.reset();
    }
}
