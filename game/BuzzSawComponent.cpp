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

#include "BuzzSawComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "AssetManager.h"
#include "FadeOutComponent.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BuzzSawComponent::BuzzSawComponent(const CollisionBuzzSawMissileComponentPtr& cbc, float damage2)
    : PhasedComponent(phaseThink),
      cbc_(cbc),
      damage2_(damage2),
      sndHit_(audio.createSound("buzzsaw.ogg")),
      stunned_(false)
    {
    }

    BuzzSawComponent::~BuzzSawComponent()
    {
    }

    void BuzzSawComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BuzzSawComponent::update(float dt)
    {
        parent()->setAngularVelocity(origAngularVel_);
        if (!stunned_) {
            parent()->setLinearVelocity(origLinearVel_);
        }

        if (!stunned_ && parent()->stunned()) {
            stunned_ = true;
            parent()->setLinearDamping(3.0f);

            parent()->setType(SceneObjectTypePlayerMissile);

            PhysicsBodyComponentPtr pc = parent()->findComponent<PhysicsBodyComponent>();
            if (pc) {
                b2Filter filter = pc->getFilter();

                filter.maskBits |= (collisionBitEnemy | collisionBitEnemyBuilding);
                filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);

                pc->setFilterOverride(filter);
                pc->enableAllFixtures();
            }

            cbc_->setDamage(damage2_);
        } else if (stunned_ && !parent()->stunned()) {
            if (parent()->gravityGunDropped()) {
                parent()->changeLife(-parent()->maxLife());
            } else {
                stunned_ = false;
                parent()->setGravityGunAware(false);
                parent()->setLinearDamping(0.0f);
                b2Vec2 tmp = parent()->linearVelocity();
                tmp.Normalize();
                origLinearVel_ = settings.sawer.sawFastWalkSpeed * tmp;
                parent()->setLinearVelocity(origLinearVel_);
            }
        }

        if (parent()->dead()) {
            SceneObjectPtr gibbed = sceneObjectFactory.createSawerSawGibbed();

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(10.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f);

            gibbed->addComponent(component);

            gibbed->setLinearVelocity(parent()->linearVelocity());
            gibbed->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);
                (*it)->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f);

                (*it)->addComponent(component);
            }

            scene()->addObject(gibbed);

            if (parent()->findComponent<PhysicsBodyComponent>()) {
                sndHit_->play();
            }

            parent()->removeFromParent();

            return;
        }
    }

    void BuzzSawComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void BuzzSawComponent::onRegister()
    {
        stunned_ = parent()->stunned();

        origLinearVel_ = parent()->linearVelocity();
        origAngularVel_ = parent()->angularVelocity();
    }

    void BuzzSawComponent::onUnregister()
    {
    }
}
