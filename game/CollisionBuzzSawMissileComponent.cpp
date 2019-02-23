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

#include "CollisionBuzzSawMissileComponent.h"
#include "CollisionBuzzSawComponent.h"
#include "CameraComponent.h"
#include "PhysicsComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Const.h"
#include "Utils.h"
#include "Settings.h"
#include "GameShell.h"
#include "af/Utils.h"
#include "SceneObjectFactory.h"

namespace af
{
    CollisionBuzzSawMissileComponent::CollisionBuzzSawMissileComponent()
    : sndHit_(audio.createSound("buzzsaw.ogg")),
      damage_(0.0f),
      numEnemiesKilled_(0)
    {
    }

    CollisionBuzzSawMissileComponent::~CollisionBuzzSawMissileComponent()
    {
    }

    void CollisionBuzzSawMissileComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionBuzzSawMissileComponent::preSolve(SInt32 cookie, b2Contact* contact)
    {
        SceneObject* otherObj = SceneObject::fromFixture(contact->GetFixtureA());

        if (parent() == otherObj) {
            otherObj = SceneObject::fromFixture(contact->GetFixtureB());
        }

        if (hitSet_.count(otherObj->shared_from_this()) > 0) {
            contact->SetEnabled(false);
        }
    }

    void CollisionBuzzSawMissileComponent::beginCollision(const Collision& collision)
    {
        static SceneObjectTypes missileTypes = SceneObjectTypes(SceneObjectTypePlayerMissile) |
            SceneObjectTypeAllyMissile | SceneObjectTypeEnemyMissile | SceneObjectTypeNeutralMissile;

        static SceneObjectTypes penetrateTypes = SceneObjectTypes(SceneObjectTypeEnemy);

        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        if (parent()->dead()) {
            return;
        }

        SceneObject* otherObj = collision.getOther(parent());

        if (missileTypes[otherObj->type()]) {
            if (otherObj->findComponent<CollisionBuzzSawMissileComponent>()) {
                hit(otherObj, collision.worldManifold.points[0]);
                parent()->changeLife(-parent()->maxLife());
                parent()->findComponent<PhysicsComponent>()->removeFromParent();
            }
            return;
        }

        if (hitSet_.count(otherObj->shared_from_this()) > 0) {
            return;
        }

        if (parent()->stunned() || !penetrateTypes[otherObj->type()] || otherObj->findComponent<CollisionBuzzSawComponent>()) {
            hit(otherObj, collision.worldManifold.points[0]);
            parent()->changeLife(-parent()->maxLife());
            parent()->findComponent<PhysicsComponent>()->removeFromParent();
            return;
        }

        hitSet_.insert(otherObj->shared_from_this());

        hit(otherObj, collision.worldManifold.points[0]);
        parent()->changeLife(-parent()->maxLife() / settings.sawer.sawPenetrationFactor);
        if (parent()->dead()) {
            parent()->findComponent<PhysicsComponent>()->removeFromParent();
        }
    }

    void CollisionBuzzSawMissileComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionBuzzSawMissileComponent::endCollision(SInt32 cookie)
    {
    }

    void CollisionBuzzSawMissileComponent::update(float dt)
    {
    }

    void CollisionBuzzSawMissileComponent::onRegister()
    {
    }

    void CollisionBuzzSawMissileComponent::onUnregister()
    {
        hitSet_.clear();

        if (parent()->type() != SceneObjectTypePlayerMissile) {
            return;
        }

        if (numEnemiesKilled_ >= 3) {
            scene()->stats()->incImpresses();
        } else {
            scene()->stats()->resetImpresses();
        }

        if (scene()->stats()->impresses() >= 2) {
            gameShell->setAchieved(AchievementImpresses);
        }
    }

    void CollisionBuzzSawMissileComponent::hit(SceneObject* otherObj, const b2Vec2& pt)
    {
        static SceneObjectTypes tmpTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
            SceneObjectTypeEnemyBuilding;

        SceneObject* dr = otherObj->damageReceiver();

        bool alive = dr->alive();

        otherObj->changeLife2(parent(), -damage_);

        if (alive && !dr->alive() && tmpTypes[dr->type()]) {
            ++numEnemiesKilled_;
        }

        for (int i = 0; i < 3; ++i) {
            b2Vec2 offset = angle2vec(getRandom(0.0f, 2.0f * b2_pi), getRandom(0.1f, 0.5f));
            if (otherObj->type() == SceneObjectTypePlayer) {
                scene()->addObject(sceneObjectFactory.createMissileHit1(MaterialFlesh, pt + offset));
            } else {
                scene()->addObject(sceneObjectFactory.createMissileHit1(otherObj->shared_from_this(), pt + offset));
            }
        }

        sndHit_->play();
    }
}
