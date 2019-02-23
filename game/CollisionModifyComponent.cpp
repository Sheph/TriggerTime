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

#include "CollisionModifyComponent.h"
#include "SceneObject.h"

namespace af
{
    CollisionModifyComponent::CollisionModifyComponent()
    : modifyFriction_(false),
      friction_(0.0f),
      modifyRestitution_(false),
      restitution_(0.0f)
    {
    }

    CollisionModifyComponent::~CollisionModifyComponent()
    {
    }

    void CollisionModifyComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionModifyComponent::preSolve(SInt32 cookie, b2Contact* contact)
    {
        if (contact->GetFixtureA()->IsSensor() ||
            contact->GetFixtureB()->IsSensor()) {
            return;
        }

        SceneObject* obj = SceneObject::fromFixture(contact->GetFixtureA());

        if (obj == parent()) {
            obj = SceneObject::fromFixture(contact->GetFixtureB());
        }

        bool passed = typeFilter_.empty() && cookieFilter_.empty();

        if (!typeFilter_.empty() &&
            typeFilter_[obj->type()]) {
            passed = true;
        }

        if (!passed && !cookieFilter_.empty() &&
            (cookieFilter_.count(obj->cookie()) != 0)) {
            passed = true;
        }

        if (passed) {
            if (modifyFriction_) {
                contact->SetFriction(friction_);
            }
            if (modifyRestitution_) {
                contact->SetRestitution(restitution_);
            }
        }
    }

    void CollisionModifyComponent::beginCollision(const Collision& collision)
    {
    }

    void CollisionModifyComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionModifyComponent::endCollision(SInt32 cookie)
    {
    }

    void CollisionModifyComponent::update(float dt)
    {
    }

    void CollisionModifyComponent::onRegister()
    {
    }

    void CollisionModifyComponent::onUnregister()
    {
    }

    void CollisionModifyComponent::addCookieFilter(SInt32 cookie)
    {
        cookieFilter_.insert(cookie);
    }

    void CollisionModifyComponent::script_addObjectFilter(const SceneObjectPtr& obj)
    {
        addCookieFilter(obj->cookie());
    }
}
