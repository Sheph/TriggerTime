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

#include "SeekerComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    SeekerComponent::SeekerComponent(const CollisionDieComponentPtr& cdc,
        const ParticleEffectComponentPtr& pec,
        float explosionImpulse, float explosionDamage, const SceneObjectTypes& damageTypes)
    : TargetableComponent(phaseThink),
      cdc_(cdc),
      pec_(pec),
      explosionImpulse_(explosionImpulse),
      explosionDamage_(explosionDamage),
      damageTypes_(damageTypes),
      haveStartPos_(false),
      wasFreezable_(false),
      linearVelocityLen_(0.0f),
      seekVelocity_(0.0f)
    {
    }

    SeekerComponent::~SeekerComponent()
    {
    }

    void SeekerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SeekerComponent::update(float dt)
    {
        if (pec_->isAllowCompletion()) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        if (cdc_->collidedWith()) {
            pec_->setPaused(false);

            pec_->allowCompletion();

            parent()->findComponent<RenderQuadComponent>()->removeFromParent();
            parent()->findComponent<LightComponent>()->removeFromParent();

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion6(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                explosionImpulse_, explosionDamage_, damageTypes_);

            scene()->addObject(explosion);

            return;
        }

        if (haveStartPos_) {
            if (b2DistanceSquared(parent()->pos(), startPos_) > 0.000001f) {
                parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
                return;
            } else {
                parent()->setVisible(true);
                parent()->setLinearVelocity(actualLinearVelocity_);
                parent()->resetSmooth();
                parent()->setFreezable(wasFreezable_);
                haveStartPos_ = false;
                pec_->setPaused(false);
            }
        }

        if (!target() || target()->dead()) {
            return;
        }

        float angle = angleBetween(parent()->getDirection(1.0f),
            target()->pos() - parent()->pos());

        float angularVelocity = 0.0f;

        if (angle >= 0.0f) {
            angularVelocity = seekVelocity_;
        } else {
            angularVelocity = -seekVelocity_;
        }

        if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
            angularVelocity = angle / settings.physics.fixedTimestep;
        }

        parent()->setAngleSmoothed(parent()->angle() + angularVelocity * dt);
        parent()->setLinearVelocity(parent()->getDirection(linearVelocityLen_));
    }

    void SeekerComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void SeekerComponent::onRegister()
    {
        linearVelocityLen_ = parent()->linearVelocity().Length();
        if (haveStartPos_) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
            pec_->setPaused(true);
        }
    }

    void SeekerComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
