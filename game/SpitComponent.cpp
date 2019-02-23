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

#include "SpitComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    SpitComponent::SpitComponent(const CollisionDieComponentPtr& cdc,
        float damage)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      damage_(damage),
      haveStartPos_(false),
      wasFreezable_(false)
    {
    }

    SpitComponent::~SpitComponent()
    {
    }

    void SpitComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SpitComponent::update(float dt)
    {
        if (cdc_->collidedWith()) {
            cdc_->collidedWith()->changeLife2(parent(), -damage_);

            scene()->addObject(sceneObjectFactory.createMissileHit1(cdc_->collidedWith(), cdc_->collisionPoint()));

            parent()->removeFromParent();

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

    void SpitComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void SpitComponent::onRegister()
    {
        if (haveStartPos_) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
        }
    }

    void SpitComponent::onUnregister()
    {
    }
}
