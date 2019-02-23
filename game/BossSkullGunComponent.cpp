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

#include "BossSkullGunComponent.h"
#include "PhysicsJointComponent.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BossSkullGunComponent::BossSkullGunComponent(const Weapons& weapons, float turnSpeed,
        float shootAngle, float shootRadius, const std::string& jointName)
    : PhasedComponent(phaseThink),
      alwaysFire_(false),
      weapons_(weapons),
      turnSpeed_(turnSpeed),
      shootAngle_(shootAngle),
      shootRadius_(shootRadius),
      jointName_(jointName),
      hadTarget_(false)
    {
    }

    BossSkullGunComponent::~BossSkullGunComponent()
    {
    }

    void BossSkullGunComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossSkullGunComponent::update(float dt)
    {
        if (!joint_) {
            joint_ = parent()->parentObject()->findComponent<PhysicsJointComponent>()->joint<RevoluteJointProxy>(jointName_);
        }

        if (!target_) {
            seekBehavior_->reset();
            hadTarget_ = false;
            return;
        }

        if (!hadTarget_) {
            seekBehavior_->setAngularVelocity(turnSpeed_);
            seekBehavior_->setUseJoint(joint_);
            seekBehavior_->setLoop(true);
            seekBehavior_->start();
            hadTarget_ = true;
        }

        seekBehavior_->setTarget(target_);

        float angle = fabs(angleBetween(parent()->getDirection(1.0f), target_->pos() - parent()->pos()));

        bool hold = alwaysFire_ || ((angle <= shootAngle_) && (target_->pos() - parent()->pos()).Length() <= shootRadius_);

        for (Weapons::const_iterator it = weapons_.begin();
             it != weapons_.end(); ++it) {
            (*it)->trigger(hold);
        }
    }

    void BossSkullGunComponent::onRegister()
    {
        for (Weapons::const_iterator it = weapons_.begin();
             it != weapons_.end(); ++it) {
            parent()->addComponent(*it);
        }

        seekBehavior_ = parent()->seekBehavior();
    }

    void BossSkullGunComponent::onUnregister()
    {
        target_.reset();
    }
}
