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

#include "CollisionSparksComponent.h"
#include "PhysicsComponent.h"
#include "AssetManager.h"
#include "Const.h"

namespace af
{
    CollisionSparksComponent::CollisionSparksComponent()
    {
    }

    CollisionSparksComponent::~CollisionSparksComponent()
    {
    }

    void CollisionSparksComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionSparksComponent::beginCollision(const Collision& collision)
    {
        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        if (collision.getOther(parent())->type() == SceneObjectTypeBlocker) {
            /*
             * Don't spark against blockers.
             */
            return;
        }

        /*
         * We want to get local pos, but without taking
         * parent rotation into account, this is because
         * we're going to use fixedPos in particle effect.
         */

        b2Vec2 pos = collision.worldManifold.points[0] - parent()->smoothPos();

        ParticleEffectComponentPtr component =
            assetManager.getParticleEffect("sparks1.p",
                                           pos,
                                           0.0f,
                                           true);

        component->setZOrder(zOrderEffects);

        component->resetEmit();

        parent()->addComponent(component);

        sparkMap_[collision.cookie] = component;
    }

    void CollisionSparksComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionSparksComponent::endCollision(SInt32 cookie)
    {
        SparkMap::iterator it = sparkMap_.find(cookie);

        if (it == sparkMap_.end()) {
            return;
        }

        it->second->allowCompletion();

        if (it->second->isComplete()) {
            /*
             * Set duration to 100ms for instant collisions.
             */
            it->second->setDuration(0.1f);
        }
    }

    void CollisionSparksComponent::update(float dt)
    {
        for (SparkMap::iterator it = sparkMap_.begin();
             it != sparkMap_.end();) {
            if (it->second->isComplete()) {
                it->second->removeFromParent();
                sparkMap_.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void CollisionSparksComponent::onRegister()
    {
    }

    void CollisionSparksComponent::onUnregister()
    {
    }
}
