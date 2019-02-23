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

#include "CreatureComponent.h"
#include "PhysicsJointComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "Scene.h"
#include "Const.h"
#include "SceneObjectFactory.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    CreatureComponent::CreatureComponent(int i)
    : PhasedComponent(phaseThink),
      i_(i)
    {
    }

    CreatureComponent::~CreatureComponent()
    {
    }

    void CreatureComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void CreatureComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            if (jc) {
                PhysicsJointComponent::Joints joints = jc->joints<JointProxy>("br_joint");

                for (PhysicsJointComponent::Joints::const_iterator it = joints.begin();
                     it != joints.end(); ++it) {
                    (*it)->remove();
                }
            }

            parent()->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(2.0f);
            parent()->addComponent(component);

            b2Vec2 velDir = parent()->getDirection(2.0f);
            b2Rot rot(2.0f * b2_pi / (parent()->objects().size() + 1));

            parent()->setAngularVelocity(b2_pi * 8.0f);

            parent()->setLinearVelocity(0.5f * parent()->linearVelocity());

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                 it != parent()->objects().end(); ++it) {
                SceneObjectPtr obj = sceneObjectFactory.createBlood1(3.0f, zOrderExplosion - 1);

                obj->setPos((*it)->pos());
                obj->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(obj);

                (*it)->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(2.0f);
                (*it)->addComponent(component);

                velDir = b2Mul(rot, velDir);

                if ((rand() % 2) == 0) {
                    velDir = -velDir;
                }

                (*it)->setLinearVelocity(0.5f * (*it)->linearVelocity() + velDir);
            }

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(4.0f, zOrderBack);

            bs->setPos(parent()->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            audio.playSound("tetrocrab_crack.ogg");

            scene()->stats()->incNumKilledAnimals();

            removeFromParent();
        }
    }

    void CreatureComponent::onRegister()
    {
        scene()->stats()->incNumAnimals();
    }

    void CreatureComponent::onUnregister()
    {
    }
}
