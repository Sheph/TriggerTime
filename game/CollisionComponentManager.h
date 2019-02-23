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

#ifndef _COLLISIONCOMPONENTMANAGER_H_
#define _COLLISIONCOMPONENTMANAGER_H_

#include "ComponentManager.h"
#include <Box2D/Box2D.h>
#include <set>
#include <map>
#include <vector>

namespace af
{
    class SceneObject;

    struct Collision
    {
        Collision();
        explicit Collision(SInt32 cookie);
        Collision(SInt32 cookie, b2Contact* contact);

        SceneObject* getOther(SceneObject* obj) const;

        b2Fixture* getOtherFixture(SceneObject* obj) const;

        b2Fixture* getThisFixture(SceneObject* obj) const;

        SInt32 cookie;

        b2Fixture* fixtureA;
        b2Fixture* fixtureB;

        int pointCount;
        b2WorldManifold worldManifold;

        float normalImpulses[b2_maxManifoldPoints];
        float tangentImpulses[b2_maxManifoldPoints];
    };

    struct CollisionUpdate
    {
        CollisionUpdate();
        CollisionUpdate(SInt32 cookie, const b2ContactImpulse* impulse);

        SInt32 cookie;

        int pointCount;

        float normalImpulses[b2_maxManifoldPoints];
        float tangentImpulses[b2_maxManifoldPoints];
    };

    class CollisionComponentManager : public ComponentManager,
                                      public b2ContactListener
    {
    public:
        CollisionComponentManager();
        ~CollisionComponentManager();

        virtual void cleanup();

        virtual void addComponent(const ComponentPtr& component);

        virtual void removeComponent(const ComponentPtr& component);

        virtual void freezeComponent(const ComponentPtr& component);

        virtual void thawComponent(const ComponentPtr& component);

        virtual void update(float dt);

        virtual void debugDraw();

        void step();

        /*
         * For internal use only.
         * @{
         */

        virtual void BeginContact(b2Contact* contact);

        virtual void EndContact(b2Contact* contact);

        virtual void PreSolve(b2Contact* contact,
                              const b2Manifold* oldManifold);

        virtual void PostSolve(b2Contact* contact,
                               const b2ContactImpulse* impulse);

        /*
         * @}
         */

    private:
        struct ContactEvent
        {
            ContactEvent(const CollisionComponentPtr& component,
                         const Collision& collision)
            : component(component),
              collision(collision)
            {
            }

            ContactEvent(const CollisionComponentPtr& component,
                         SInt32 cookie)
            : component(component),
              collision(cookie)
            {
            }

            ContactEvent(const CollisionComponentPtr& component,
                         const CollisionUpdate& collisionUpdate)
            : component(component),
              collisionUpdate(collisionUpdate)
            {
            }

            CollisionComponentPtr component;
            Collision collision;
            CollisionUpdate collisionUpdate;
        };

        typedef std::map<b2Contact*, SInt32> ContactMap;

        typedef std::multimap<b2Contact*, size_t> UpdateCollisions;

        typedef std::vector<ContactEvent> ContactEvents;

        std::set<CollisionComponentPtr> components_;
        std::set<CollisionComponentPtr> frozenComponents_;

        ContactMap contacts_;
        UpdateCollisions updateCollisions_;
        ContactEvents contactEvents_;

        SInt32 nextCookie_;
    };
}

#endif
