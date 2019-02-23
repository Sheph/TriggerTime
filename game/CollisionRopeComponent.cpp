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

#include "CollisionRopeComponent.h"
#include "PhysicsComponent.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    CollisionRopeComponent::CollisionRopeComponent()
    : missed_(false)
    {
    }

    CollisionRopeComponent::~CollisionRopeComponent()
    {
    }

    void CollisionRopeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionRopeComponent::beginCollision(const Collision& collision)
    {
        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        if (joint_ || missed_) {
            return;
        }

        SceneObject* other = collision.getOther(parent());

        if (other->bodyDef().type != b2_dynamicBody) {
            missed_ = true;
            return;
        }

        joint_ = scene()->addRevoluteJoint(other->shared_from_this(),
            parent()->shared_from_this(),
            other->getLocalPoint(collision.worldManifold.points[0]),
            b2Vec2_zero);

        other->setStunned(true);
    }

    void CollisionRopeComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionRopeComponent::endCollision(SInt32 cookie)
    {
    }

    void CollisionRopeComponent::update(float dt)
    {
    }

    void CollisionRopeComponent::onRegister()
    {
    }

    void CollisionRopeComponent::onUnregister()
    {
    }
}
