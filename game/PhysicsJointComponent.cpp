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

#include "PhysicsJointComponent.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    PhysicsJointComponent::PhysicsJointComponent(const RUBEWorldPtr& rubeWorld,
                                                 const Objects& objects)
    : rubeWorld_(rubeWorld),
      objects_(objects)
    {
    }

    PhysicsJointComponent::~PhysicsJointComponent()
    {
    }

    void PhysicsJointComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhysicsComponent(shared_from_this());
    }

    void PhysicsJointComponent::onRegister()
    {
        joints_.resize(rubeWorld_->numJoints());

        for (int i = 0; i < rubeWorld_->numJoints(); ++i) {
            RUBEGearJointPtr gearJoint =
               boost::dynamic_pointer_cast<RUBEGearJoint>(rubeWorld_->joint(i));

            if (!gearJoint) {
                b2JointDef& jointDef = rubeWorld_->joint(i)->jointDef();

                jointDef.bodyA = objects_[rubeWorld_->joint(i)->bodyAIndex()]->body();
                jointDef.bodyB = objects_[rubeWorld_->joint(i)->bodyBIndex()]->body();

                joints_[i] = scene()->addJoint(&jointDef);
            }
        }

        for (int i = 0; i < rubeWorld_->numJoints(); ++i) {
            RUBEGearJointPtr gearJoint =
               boost::dynamic_pointer_cast<RUBEGearJoint>(rubeWorld_->joint(i));

            if (gearJoint) {
                b2GearJointDef& jointDef = gearJoint->jointDef();

                jointDef.bodyA = objects_[gearJoint->bodyAIndex()]->body();
                jointDef.bodyB = objects_[gearJoint->bodyBIndex()]->body();
                jointDef.joint1 = joints_[gearJoint->joint1Index()]->joint();
                jointDef.joint2 = joints_[gearJoint->joint2Index()]->joint();

                joints_[i] = scene()->addJoint(&jointDef);
            }
        }
    }

    void PhysicsJointComponent::onUnregister()
    {
    }
}
