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

#include "RocketComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    RocketComponent::RocketComponent(const CollisionDieComponentPtr& cdc,
        const ParticleEffectComponentPtr& pec,
        float explosionImpulse, float explosionDamage, const SceneObjectTypes& damageTypes)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      pec_(pec),
      explosionImpulse_(explosionImpulse),
      explosionDamage_(explosionDamage),
      damageTypes_(damageTypes),
      haveStartPos_(false),
      wasFreezable_(false)
    {
    }

    RocketComponent::~RocketComponent()
    {
    }

    void RocketComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void RocketComponent::update(float dt)
    {
        if (pec_->isAllowCompletion()) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        if (cdc_->collidedWith()) {
            pec_->allowCompletion();

            parent()->findComponent<RenderQuadComponent>()->removeFromParent();
            parent()->findComponent<LightComponent>()->removeFromParent();

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                explosionImpulse_, explosionDamage_, damageTypes_);

            scene()->addObject(explosion);

            scene()->stats()->incShotsHit(parent(), cdc_->collidedWith().get());

            return;
        }

        if (haveStartPos_) {
            if (b2DistanceSquared(parent()->pos(), startPos_) > 0.000001f) {
                parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
            } else {
                parent()->setVisible(true);
                parent()->setLinearVelocity(actualLinearVelocity_);
                parent()->resetSmooth();
                parent()->setFreezable(wasFreezable_);
                haveStartPos_ = false;
            }
        }
    }

    void RocketComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void RocketComponent::onRegister()
    {
        if (haveStartPos_) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
        }
    }

    void RocketComponent::onUnregister()
    {
    }
}
