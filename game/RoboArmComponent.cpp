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

#include "RoboArmComponent.h"
#include "PhysicsJointComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "Utils.h"

namespace af
{
    RoboArmComponent::RoboArmComponent(const SceneObjectPtr& tip,
        const SceneObjectPtr& clampHigh,
        const SceneObjectPtr& clampLow,
        const std::string& clampJointHighName,
        const std::string& clampJointLowName,
        float minLength)
    : PhasedComponent(phaseThink),
      tip_(tip),
      clampHigh_(clampHigh),
      clampLow_(clampLow),
      clampJointHighName_(clampJointHighName),
      clampJointLowName_(clampJointLowName),
      minLength_(minLength),
      maxLength_(0.0f),
      velocity_(10.0f),
      sndMove_(audio.createSound("roboarm_move.ogg")),
      active_(false)
    {
        sndMove_->setLoop(true);
    }

    RoboArmComponent::~RoboArmComponent()
    {
    }

    void RoboArmComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void RoboArmComponent::update(float dt)
    {
        if (!scene()->inputRoboArm()->active() || !active_) {
            sndMove_->stop();
            tip_->setLinearVelocity(b2Vec2_zero);
            return;
        }

        bool done = true;

        if (scene()->inputRoboArm()->lockPressed()) {
            clampJoint_->setMotorSpeed(fabs(clampJoint_->motorSpeed()));
        } else {
            clampJoint_->setMotorSpeed(-fabs(clampJoint_->motorSpeed()));
        }

        if (fabs(clampJoint_->getJointSpeed()) > 1.0f) {
            if (sndMove_->status() != AudioSource::Playing) {
                sndMove_->play();
            }
            done = false;
        }

        b2Vec2 pos = b2Vec2_zero;
        float velScale = 1.0f;

        if (scene()->inputRoboArm()->moveToPressed(pos)) {
            pos = scene()->camera()->findComponent<CameraComponent>()->getWorldPoint(pos);
        } else if (scene()->inputRoboArm()->movePressed(pos)) {
            velScale = pos.Normalize();
            pos = tip_->pos() + 100.0f * pos;
        } else {
            tip_->setLinearVelocity(b2Vec2_zero);
            if (done) {
                sndMove_->stop();
            }
            return;
        }

        if ((pos - parent()->pos()).LengthSquared() > maxLength_ * maxLength_) {
            pos -= parent()->pos();
            pos.Normalize();
            pos *= maxLength_;
            pos += parent()->pos();
        } else if ((pos - parent()->pos()).LengthSquared() < minLength_ * minLength_) {
            pos -= parent()->pos();
            pos.Normalize();
            pos *= minLength_;
            pos += parent()->pos();
        }

        if (angleBetween(pos - parent()->pos(), parent()->getDirection(1.0f)) > deg2rad(85.0f)) {
            pos = parent()->pos() + angle2vec(parent()->angle() - deg2rad(85.0f), (pos - parent()->pos()).Length());
        } else if (angleBetween(pos - parent()->pos(), parent()->getDirection(1.0f)) < -deg2rad(85.0f)) {
            pos = parent()->pos() + angle2vec(parent()->angle() + deg2rad(85.0f), (pos - parent()->pos()).Length());
        }

        b2Vec2 dir = pos - tip_->pos();
        float len = dir.Normalize();

        if (velocity_ * dt * velScale > len) {
            dir *= len;
            if (done) {
                sndMove_->stop();
            }
        } else {
            dir *= velocity_ * velScale;
            if (sndMove_->status() != AudioSource::Playing) {
                sndMove_->play();
            }
        }

        tip_->setLinearVelocity(dir);
    }

    void RoboArmComponent::onRegister()
    {
        clampJoint_ = parent()->findComponent<PhysicsJointComponent>()->joint<PrismaticJointProxy>(clampJointHighName_);
        PrismaticJointProxyPtr low = parent()->findComponent<PhysicsJointComponent>()->joint<PrismaticJointProxy>(clampJointLowName_);

        scene()->addGearJoint(clampHigh_, clampLow_, clampJoint_, low, -1.0f);

        maxLength_ = (parent()->pos() - tip_->pos()).Length();
    }

    void RoboArmComponent::onUnregister()
    {
        tip_.reset();
        clampHigh_.reset();
        clampLow_.reset();
        sndMove_->stop();
    }
}
