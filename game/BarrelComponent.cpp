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

#include "BarrelComponent.h"
#include "ExplosionComponent.h"
#include "FadeOutComponent.h"
#include "SceneObjectFactory.h"
#include "Scene.h"
#include "Const.h"
#include <boost/make_shared.hpp>

namespace af
{
    BarrelComponent::BarrelComponent()
    : PhasedComponent(phaseThink),
      explosive_(false),
      toxic_(false)
    {
        snd_[0] = audio.createSound("barrel_break1.ogg");
        snd_[1] = audio.createSound("barrel_break2.ogg");
    }

    BarrelComponent::~BarrelComponent()
    {
    }

    void BarrelComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BarrelComponent::update(float dt)
    {
        if (parent()->life() > 0) {
            return;
        }

        snd_[rand() % 2]->play();

        SceneObjectPtr gibbed;

        if (explosive_) {
            gibbed = sceneObjectFactory.createBarrel1Gibbed();
        } else if (toxic_) {
            gibbed = sceneObjectFactory.createBarrel3Gibbed();
        } else {
            gibbed = sceneObjectFactory.createBarrel2Gibbed();
        }

        gibbed->setTransformRecursive(parent()->getTransform());

        b2Vec2 velDir = parent()->getDirection(10.0f);
        b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

        if (gibbed->type() != SceneObjectTypeGarbage) {
            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

            gibbed->addComponent(component);
        }

        gibbed->setLinearVelocity(parent()->linearVelocity());
        gibbed->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

        for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
             it != gibbed->objects().end(); ++it) {
            velDir = b2Mul(rot, velDir);

            (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);
            (*it)->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

            if ((*it)->type() != SceneObjectTypeGarbage) {
                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

                (*it)->addComponent(component);
            }
        }

        scene()->addObjectUnparent(gibbed);

        if (explosive_) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                impulse_, damage_, types);

            scene()->addObject(explosion);

            parent()->removeFromParent();

            return;
        }

        if (toxic_) {
            SceneObjectPtr cloud = sceneObjectFactory.createToxicCloud1(damage_, damageTimeout_, zOrderExplosion - 1);

            cloud->setTransform(parent()->getTransform());

            scene()->addObject(cloud);
        }

        parent()->removeFromParent();
    }

    void BarrelComponent::setExplosive(float impulse, float damage)
    {
        explosive_ = true;
        impulse_ = impulse;
        damage_ = damage;
    }

    void BarrelComponent::setToxic(float damage, float damageTimeout)
    {
        toxic_ = true;
        damage_ = damage;
        damageTimeout_ = damageTimeout;
    }

    void BarrelComponent::onRegister()
    {
    }

    void BarrelComponent::onUnregister()
    {
    }
}
