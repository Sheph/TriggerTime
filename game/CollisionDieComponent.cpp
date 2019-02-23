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

#include "CollisionDieComponent.h"
#include "PhysicsComponent.h"
#include "SceneObject.h"

namespace af
{
    CollisionDieComponent::CollisionDieComponent()
    : allowSensor_(false)
    {
    }

    CollisionDieComponent::~CollisionDieComponent()
    {
    }

    void CollisionDieComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionDieComponent::beginCollision(const Collision& collision)
    {
        if (collidedWith_) {
            return;
        }

        if (allowSensor_) {
            if (collision.getOtherFixture(parent())->IsSensor()) {
                return;
            }
        } else if (collision.fixtureA->IsSensor() ||
                   collision.fixtureB->IsSensor()) {
            return;
        }

        collidedWith_ = collision.getOther(parent())->shared_from_this();

        collisionPoint_ = collision.worldManifold.points[0];

        collisionNormal_ = collision.worldManifold.normal;

        parent()->findComponent<PhysicsComponent>()->removeFromParent();
    }

    void CollisionDieComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionDieComponent::endCollision(SInt32 cookie)
    {
    }

    void CollisionDieComponent::update(float dt)
    {
    }

    void CollisionDieComponent::onRegister()
    {
    }

    void CollisionDieComponent::onUnregister()
    {
        collidedWith_.reset();
    }
}
