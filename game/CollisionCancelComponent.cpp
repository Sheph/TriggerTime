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

#include "CollisionCancelComponent.h"
#include "CameraComponent.h"
#include "PlayerComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Const.h"

namespace af
{
    CollisionCancelComponent::CollisionCancelComponent()
    : damage_(0.0f),
      roll_(0.0f),
      immediate_(false),
      noStatic_(false),
      coreProtectorMelee_(false)
    {
    }

    CollisionCancelComponent::~CollisionCancelComponent()
    {
    }

    void CollisionCancelComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionCancelComponent::preSolve(SInt32 cookie, b2Contact* contact)
    {
        if ((cookies_.count(cookie) > 0)) {
            contact->SetEnabled(false);
        }

        if (immediate_) {
            if (contact->GetFixtureA()->IsSensor() ||
                contact->GetFixtureB()->IsSensor()) {
                return;
            }

            SceneObject* otherObj = SceneObject::fromFixture(contact->GetFixtureA());

            if (parent() == otherObj) {
                otherObj = SceneObject::fromFixture(contact->GetFixtureB());
            }

            if (check(otherObj)) {
                contact->SetEnabled(false);
            }
        }
    }

    void CollisionCancelComponent::beginCollision(const Collision& collision)
    {
        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        SceneObject* otherObj = collision.getOther(parent());

        /*
         * FIXME: dirty hack for "enemy rock".
         */
        if ((otherObj->body()->GetType() == b2_staticBody) &&
            ((collision.getOtherFixture(parent())->GetFilterData().categoryBits & collisionBitEnemyRock) != 0)) {
            return;
        }

        if (noStatic_ && (otherObj->body()->GetType() == b2_staticBody)) {
            return;
        }

        if (!check(otherObj)) {
            return;
        }

        cookies_.insert(collision.cookie);

        if (damage_ != 0.0f) {
            otherObj->changeLife(-damage_);

            if (coreProtectorMelee_) {
                PlayerComponentPtr pc = otherObj->findComponent<PlayerComponent>();
                if (pc) {
                    pc->setDamagedByCoreProtectorMelee();
                }
            }
        }

        if (sndDamage_) {
            sndDamage_->play();
        }

        if (roll_ != 0.0f) {
            scene()->camera()->findComponent<CameraComponent>()->roll(roll_);
        }
    }

    void CollisionCancelComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionCancelComponent::endCollision(SInt32 cookie)
    {
        cookies_.erase(cookie);
    }

    void CollisionCancelComponent::update(float dt)
    {
    }

    void CollisionCancelComponent::onRegister()
    {
    }

    void CollisionCancelComponent::onUnregister()
    {
    }

    bool CollisionCancelComponent::check(SceneObject* obj)
    {
        bool passed = typeFilter_.empty() && cookieFilter_.empty();

        if (!typeFilter_.empty() &&
            typeFilter_[obj->type()]) {
            passed = true;
        }

        if (!passed && !cookieFilter_.empty() &&
            (cookieFilter_.count(obj->cookie()) != 0)) {
            passed = true;
        }

        return passed;
    }

    void CollisionCancelComponent::addCookieFilter(SInt32 cookie)
    {
        cookieFilter_.insert(cookie);
    }

    void CollisionCancelComponent::script_addObjectFilter(const SceneObjectPtr& obj)
    {
        addCookieFilter(obj->cookie());
    }
}
