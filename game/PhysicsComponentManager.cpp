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

#include "PhysicsComponentManager.h"
#include "Settings.h"
#include "PhysicsComponent.h"
#include "CollisionComponentManager.h"
#include "SceneObject.h"

namespace af
{
    PhysicsComponentManager::ContactFilter::ContactFilter()
    {
    }

    PhysicsComponentManager::ContactFilter::~ContactFilter()
    {
    }

    bool PhysicsComponentManager::ContactFilter::ShouldCollide(b2Fixture* fixtureA,
                                                               b2Fixture* fixtureB)
    {
        const b2Filter& filterA = fixtureA->GetFilterData();
        const b2Filter& filterB = fixtureB->GetFilterData();

        if ((filterA.groupIndex == filterB.groupIndex) &&
            (filterA.groupIndex != 0)) {
            return filterA.groupIndex > 0;
        }

        SceneObject* objectA = SceneObject::fromFixture(fixtureA);
        SceneObject* objectB = SceneObject::fromFixture(fixtureB);

        return ((filterA.maskBits & filterB.categoryBits) != 0) &&
               ((filterA.categoryBits & filterB.maskBits) != 0) &&
               (!objectA->collisionFilter() || objectA->collisionFilter()->shouldCollideWith(fixtureB, fixtureA)) &&
               (!objectB->collisionFilter() || objectB->collisionFilter()->shouldCollideWith(fixtureA, fixtureB));
    }

    PhysicsComponentManager::PhysicsComponentManager(const boost::shared_ptr<CollisionComponentManager>& collisionComponentManager,
                                                     b2DestructionListener* destructionListener,
                                                     b2Draw* debugDraw)
    : world_(b2Vec2(0.0f, 0.0f)),
      collisionComponentManager_(collisionComponentManager)
    {
        world_.SetContactListener(collisionComponentManager_.get());

        world_.SetContactFilter(&contactFilter_);

        world_.SetDestructionListener(destructionListener);

        world_.SetDebugDraw(debugDraw);

        world_.SetAutoClearForces(true);
    }

    PhysicsComponentManager::~PhysicsComponentManager()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());
    }

    void PhysicsComponentManager::cleanup()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());
    }

    void PhysicsComponentManager::addComponent(const ComponentPtr& component)
    {
        PhysicsComponentPtr physicsComponent = boost::dynamic_pointer_cast<PhysicsComponent>(component);
        assert(physicsComponent);

        assert(!component->manager());

        components_.insert(physicsComponent);
        physicsComponent->setManager(this);
    }

    void PhysicsComponentManager::removeComponent(const ComponentPtr& component)
    {
        PhysicsComponentPtr physicsComponent = boost::dynamic_pointer_cast<PhysicsComponent>(component);
        assert(physicsComponent);

        if (components_.erase(physicsComponent) ||
            frozenComponents_.erase(physicsComponent)) {
            physicsComponent->setManager(NULL);
        }
    }

    void PhysicsComponentManager::freezeComponent(const ComponentPtr& component)
    {
        PhysicsComponentPtr physicsComponent = boost::static_pointer_cast<PhysicsComponent>(component);

        components_.erase(physicsComponent);
        frozenComponents_.insert(physicsComponent);
        component->onFreeze();
    }

    void PhysicsComponentManager::thawComponent(const ComponentPtr& component)
    {
        PhysicsComponentPtr physicsComponent = boost::static_pointer_cast<PhysicsComponent>(component);

        frozenComponents_.erase(physicsComponent);
        components_.insert(physicsComponent);
        component->onThaw();
    }

    void PhysicsComponentManager::update(float dt)
    {
        world_.Step(dt, 6, 2);
    }

    void PhysicsComponentManager::debugDraw()
    {
        for (std::set<PhysicsComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            (*it)->debugDraw();
        }
    }
}
