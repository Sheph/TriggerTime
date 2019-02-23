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

#include "RopePullComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Utils.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    RopePullComponent::RopePullComponent(const SceneObjectPtr& target,
        const b2Vec2& pos, float velocity, float maxMotorForce)
    : PhasedComponent(phaseThink),
      ropeC_(target->findComponent<PhysicsRopeComponent>()),
      pos_(pos),
      axis_(angle2vec(b2_pi, 1.0f)),
      velocity_(velocity),
      maxMotorForce_(maxMotorForce),
      visibilityRadius_(ropeC_->segmentWidth() / 2),
      paused_(false)
    {
    }

    RopePullComponent::~RopePullComponent()
    {
    }

    void RopePullComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void RopePullComponent::update(float dt)
    {
        if (!joint_ || paused_) {
            return;
        }

        while ((joint_->localAnchorA().x + joint_->getJointTranslation() - pos_.x >= visibilityRadius_ * 2) &&
            (ropeC_->objects().size() > 2)) {
            float tr = joint_->localAnchorA().x + joint_->getJointTranslation() - pos_.x;

            ropeC_->cutTail(1);

            float angle = ropeC_->tail()->angle() - parent()->angle();

            if (angle >= 0.0f) {
                std::modf(angle / (b2_pi * 2), &angle);
                angle *= b2_pi * 2;
                angle += 0.0f;
            } else {
                std::modf(angle / (b2_pi * 2), &angle);
                angle *= b2_pi * 2;
                angle -= 0.0f;
            }

            JointProxyPtr joint =
               scene()->addPrismaticJoint(parent()->shared_from_this(),
                   ropeC_->tail()->shared_from_this(),
                   pos_ + b2Vec2(tr - visibilityRadius_ * 2, 0.0f),
                   b2Vec2_zero, 0.0f, visibilityRadius_ * 2.0f, axis_,
                   angle);

            joint_ = boost::dynamic_pointer_cast<PrismaticJointProxy>(joint);

            if (ropeC_->objects().size() > 2) {
                joint_->enableMotor(true);
                joint_->setMotorSpeed(velocity_);
                joint_->setMaxMotorForce(maxMotorForce_);
            }
        }
    }

    void RopePullComponent::onRegister()
    {
        JointProxyPtr joint =
            scene()->addPrismaticJoint(parent()->shared_from_this(),
                ropeC_->tail()->shared_from_this(),
                pos_, b2Vec2_zero, 0.0f, visibilityRadius_ * 2.0f, axis_);

        joint_ = boost::dynamic_pointer_cast<PrismaticJointProxy>(joint);

        joint_->enableMotor(true);
        joint_->setMotorSpeed(velocity_);
        joint_->setMaxMotorForce(maxMotorForce_);
    }

    void RopePullComponent::onUnregister()
    {
        ropeC_.reset();
        if (joint_) {
            joint_->remove();
        }
    }
}
