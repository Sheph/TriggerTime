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

#include "CollisionComponentManager.h"
#include "CollisionComponent.h"
#include "SceneObject.h"

namespace af
{
    Collision::Collision()
    : cookie(0),
      fixtureA(NULL),
      fixtureB(NULL),
      pointCount(0)
    {
    }

    Collision::Collision(SInt32 cookie)
    : cookie(cookie),
      fixtureA(NULL),
      fixtureB(NULL),
      pointCount(0)
    {
    }

    Collision::Collision(SInt32 cookie, b2Contact* contact)
    : cookie(cookie)
    {
        fixtureA = contact->GetFixtureA();
        fixtureB = contact->GetFixtureB();

        pointCount = contact->GetManifold()->pointCount;
        contact->GetWorldManifold(&worldManifold);

        for (int i = 0; i < b2_maxManifoldPoints; i++) {
            normalImpulses[i] = 0;
            tangentImpulses[i] = 0;
        }
    }

    SceneObject* Collision::getOther(SceneObject* obj) const
    {
        SceneObject* objA = SceneObject::fromFixture(fixtureA);

        return (obj == objA) ? SceneObject::fromFixture(fixtureB) : objA;
    }

    b2Fixture* Collision::getOtherFixture(SceneObject* obj) const
    {
        return (obj == SceneObject::fromFixture(fixtureA)) ? fixtureB : fixtureA;
    }

    b2Fixture* Collision::getThisFixture(SceneObject* obj) const
    {
        return (obj == SceneObject::fromFixture(fixtureA)) ? fixtureA : fixtureB;
    }

    CollisionUpdate::CollisionUpdate()
    : cookie(0),
      pointCount(0)
    {
    }

    CollisionUpdate::CollisionUpdate(SInt32 cookie, const b2ContactImpulse* impulse)
    : cookie(cookie)
    {
        assert(impulse->count > 0);

        for (int i = 0; i < b2_maxManifoldPoints; i++) {
            normalImpulses[i] = 0;
            tangentImpulses[i] = 0;
        }

        pointCount = impulse->count;

        for (int i = 0; i < pointCount; i++) {
            normalImpulses[i] += impulse->normalImpulses[i];
            tangentImpulses[i] += impulse->tangentImpulses[i];
        }
    }

    CollisionComponentManager::CollisionComponentManager()
    : nextCookie_(1)
    {
    }

    CollisionComponentManager::~CollisionComponentManager()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());
    }

    void CollisionComponentManager::cleanup()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());

        contactEvents_.clear();
        updateCollisions_.clear();
        contacts_.clear();
    }

    void CollisionComponentManager::addComponent(const ComponentPtr& component)
    {
        CollisionComponentPtr collisionComponent = boost::dynamic_pointer_cast<CollisionComponent>(component);
        assert(collisionComponent);

        assert(!component->manager());

        components_.insert(collisionComponent);
        collisionComponent->setManager(this);

        component->parent()->changeNumCollisionComponents(1);
    }

    void CollisionComponentManager::removeComponent(const ComponentPtr& component)
    {
        CollisionComponentPtr collisionComponent = boost::dynamic_pointer_cast<CollisionComponent>(component);
        assert(collisionComponent);

        if (components_.erase(collisionComponent) ||
            frozenComponents_.erase(collisionComponent)) {
            collisionComponent->setManager(NULL);
            component->parent()->changeNumCollisionComponents(-1);
        }
    }

    void CollisionComponentManager::freezeComponent(const ComponentPtr& component)
    {
        CollisionComponentPtr collisionComponent = boost::static_pointer_cast<CollisionComponent>(component);

        components_.erase(collisionComponent);
        frozenComponents_.insert(collisionComponent);
        component->onFreeze();
    }

    void CollisionComponentManager::thawComponent(const ComponentPtr& component)
    {
        CollisionComponentPtr collisionComponent = boost::static_pointer_cast<CollisionComponent>(component);

        frozenComponents_.erase(collisionComponent);
        components_.insert(collisionComponent);
        component->onThaw();
    }

    void CollisionComponentManager::update(float dt)
    {
        static std::vector<CollisionComponentPtr> tmp;

        tmp.reserve(components_.size());

        for (std::set<CollisionComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            tmp.push_back(*it);
        }

        for (std::vector<CollisionComponentPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if ((*it)->manager()) {
                (*it)->update(dt);
            }
        }

        tmp.resize(0);
    }

    void CollisionComponentManager::debugDraw()
    {
        for (std::set<CollisionComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            (*it)->debugDraw();
        }
    }

    void CollisionComponentManager::step()
    {
        while (!contactEvents_.empty()) {
            updateCollisions_.clear();

            ContactEvents tmp = contactEvents_;

            contactEvents_.clear();

            /*
             * Hold on to the objects, they might get removed
             * in component handlers.
             */

            std::set<SceneObjectPtr> objs;

            for (ContactEvents::const_iterator it = tmp.begin();
                 it != tmp.end(); ++it) {
                if (!it->component->manager()) {
                    continue;
                }

                if (it->collision.fixtureA) {
                    objs.insert(SceneObject::fromFixture(
                        it->collision.fixtureA)->shared_from_this());
                    objs.insert(SceneObject::fromFixture(
                        it->collision.fixtureB)->shared_from_this());
                }
            }

            for (ContactEvents::const_iterator it = tmp.begin();
                 it != tmp.end(); ++it) {
                if (!it->component->manager()) {
                    continue;
                }

                if (it->collision.fixtureA) {
                    it->component->beginCollision(it->collision);
                } else if (it->collisionUpdate.pointCount > 0) {
                    it->component->updateCollision(it->collisionUpdate);
                } else {
                    it->component->endCollision(it->collision.cookie);
                }
            }
        }
    }

    void CollisionComponentManager::BeginContact(b2Contact* contact)
    {
        if (!contact->IsTouching()) {
            return;
        }

        SceneObject* objectA = SceneObject::fromFixture(contact->GetFixtureA());
        SceneObject* objectB = SceneObject::fromFixture(contact->GetFixtureB());

        if (!objectA->haveCollisionComponents() &&
            !objectB->haveCollisionComponents()) {
            return;
        }

        Collision collision(nextCookie_++, contact);

        contacts_.insert(std::make_pair(contact, collision.cookie));

        if (objectA->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectA->components().begin();
                 jt != objectA->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    updateCollisions_.insert(std::make_pair(contact, contactEvents_.size()));
                    contactEvents_.push_back(ContactEvent(component, collision));
                }
            }
        }

        if (objectB->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectB->components().begin();
                 jt != objectB->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    updateCollisions_.insert(std::make_pair(contact, contactEvents_.size()));
                    contactEvents_.push_back(ContactEvent(component, collision));
                }
            }
        }
    }

    void CollisionComponentManager::EndContact(b2Contact* contact)
    {
        ContactMap::iterator it = contacts_.find(contact);

        if (it == contacts_.end()) {
            return;
        }

        SceneObject* objectA = SceneObject::fromFixture(contact->GetFixtureA());
        SceneObject* objectB = SceneObject::fromFixture(contact->GetFixtureB());

        if (objectA->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectA->components().begin();
                 jt != objectA->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    contactEvents_.push_back(ContactEvent(component, it->second));
                }
            }
        }

        if (objectB->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectB->components().begin();
                 jt != objectB->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    contactEvents_.push_back(ContactEvent(component, it->second));
                }
            }
        }

        contacts_.erase(it);
        updateCollisions_.erase(contact);
    }

    void CollisionComponentManager::PreSolve(b2Contact* contact,
                                             const b2Manifold* oldManifold)
    {
        ContactMap::iterator it = contacts_.find(contact);

        if (it == contacts_.end()) {
            return;
        }

        SceneObject* objectA = SceneObject::fromFixture(contact->GetFixtureA());
        SceneObject* objectB = SceneObject::fromFixture(contact->GetFixtureB());

        if (objectA->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectA->components().begin();
                 jt != objectA->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    component->preSolve(it->second, contact);
                }
            }
        }

        if (objectB->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectB->components().begin();
                 jt != objectB->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    component->preSolve(it->second, contact);
                }
            }
        }
    }

    void CollisionComponentManager::PostSolve(b2Contact* contact,
                                              const b2ContactImpulse* impulse)
    {
        bool alreadyUpdating = false;

        for (UpdateCollisions::const_iterator it = updateCollisions_.lower_bound(contact);
             it != updateCollisions_.upper_bound(contact); ++it) {
            for (int i = 0; i < impulse->count; ++i) {
                if (contactEvents_[it->second].collision.fixtureA) {
                    contactEvents_[it->second].collision.normalImpulses[i] += impulse->normalImpulses[i];
                    contactEvents_[it->second].collision.tangentImpulses[i] += impulse->tangentImpulses[i];
                } else {
                    contactEvents_[it->second].collisionUpdate.normalImpulses[i] += impulse->normalImpulses[i];
                    contactEvents_[it->second].collisionUpdate.tangentImpulses[i] += impulse->tangentImpulses[i];
                }
            }
            alreadyUpdating = true;
        }

        if (alreadyUpdating || (impulse->count <= 0)) {
            return;
        }

        ContactMap::iterator it = contacts_.find(contact);

        if (it == contacts_.end()) {
            return;
        }

        CollisionUpdate collisionUpdate(it->second, impulse);

        SceneObject* objectA = SceneObject::fromFixture(contact->GetFixtureA());
        SceneObject* objectB = SceneObject::fromFixture(contact->GetFixtureB());

        if (objectA->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectA->components().begin();
                 jt != objectA->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    updateCollisions_.insert(std::make_pair(contact, contactEvents_.size()));
                    contactEvents_.push_back(ContactEvent(component, collisionUpdate));
                }
            }
        }

        if (objectB->haveCollisionComponents()) {
            for (std::vector<ComponentPtr>::const_iterator jt = objectB->components().begin();
                 jt != objectB->components().end();
                 ++jt ) {
                if ((*jt)->manager() == this) {
                    const CollisionComponentPtr& component =
                        boost::static_pointer_cast<CollisionComponent>(*jt);
                    updateCollisions_.insert(std::make_pair(contact, contactEvents_.size()));
                    contactEvents_.push_back(ContactEvent(component, collisionUpdate));
                }
            }
        }
    }
}
