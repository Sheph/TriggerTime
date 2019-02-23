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

#include "CollisionDamageComponent.h"
#include "GameShell.h"

namespace af
{
    CollisionDamageComponent::CollisionDamageComponent()
    : enabled_(true),
      impulseThreshold_(0.0f),
      multiplier_(1.0f)
    {
    }

    CollisionDamageComponent::~CollisionDamageComponent()
    {
    }

    void CollisionDamageComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionDamageComponent::beginCollision(const Collision& collision)
    {
        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        SceneObject* otherObj = collision.getOther(parent());

        bool passed = typeFilter_.empty() && cookieFilter_.empty();

        if (!typeFilter_.empty() &&
            typeFilter_[otherObj->type()]) {
            passed = true;
        }

        if (!passed && !cookieFilter_.empty() &&
            (cookieFilter_.count(otherObj->cookie()) != 0)) {
            passed = true;
        }

        if (!passed) {
            return;
        }

        cookies_.insert(std::make_pair(collision.cookie, Info(otherObj->collisionImpulseMultiplier(), otherObj->gravityGunAware())));

        float avg = 0.0f;

        for (int i = 0; i < collision.pointCount; ++i) {
            float tmp =
                b2Sqrt(collision.normalImpulses[i] * collision.normalImpulses[i] +
                       collision.tangentImpulses[i] * collision.tangentImpulses[i]);
            if (tmp > avg) {
                avg = tmp;
            }
        }

        avg *= otherObj->collisionImpulseMultiplier();

        if (avg <= impulseThreshold_) {
            return;
        }

        if (enabled_) {
            if (sndDamage_) {
                sndDamage_->play();
            }

            damageSelf(avg, otherObj->gravityGunAware());
        }
    }

    void CollisionDamageComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
        std::map<SInt32, Info>::const_iterator it = cookies_.find(collisionUpdate.cookie);

        if (it == cookies_.end()) {
            return;
        }

        float avg = 0.0f;

        for (int i = 0; i < collisionUpdate.pointCount; ++i) {
            float tmp =
                b2Sqrt(collisionUpdate.normalImpulses[i] * collisionUpdate.normalImpulses[i] +
                       collisionUpdate.tangentImpulses[i] * collisionUpdate.tangentImpulses[i]);
            if (tmp > avg) {
                avg = tmp;
            }
        }

        avg *= it->second.multiplier;

        if (avg <= impulseThreshold_) {
            return;
        }

        if (enabled_) {
            damageSelf(avg, it->second.eatThat);
        }
    }

    void CollisionDamageComponent::endCollision(SInt32 cookie)
    {
        cookies_.erase(cookie);
    }

    void CollisionDamageComponent::update(float dt)
    {
    }

    void CollisionDamageComponent::onRegister()
    {
    }

    void CollisionDamageComponent::onUnregister()
    {
    }

    void CollisionDamageComponent::damageSelf(float avg, bool eatThat)
    {
        static SceneObjectTypes tmpTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
            SceneObjectTypeEnemyBuilding;

        SceneObject* dr = NULL;
        bool alive = false;

        if (!parent()->stunned() && eatThat) {
            dr = parent()->damageReceiver();
            alive = dr->alive();
        }

        parent()->changeLife((impulseThreshold_ - avg) * multiplier_);

        if (alive && !dr->alive() && tmpTypes[dr->type()]) {
            gameShell->incAchievementProgress(AchievementEatThat, 1);
        }
    }

    void CollisionDamageComponent::addCookieFilter(SInt32 cookie)
    {
        cookieFilter_.insert(cookie);
    }

    void CollisionDamageComponent::script_addObjectFilter(const SceneObjectPtr& obj)
    {
        addCookieFilter(obj->cookie());
    }
}
