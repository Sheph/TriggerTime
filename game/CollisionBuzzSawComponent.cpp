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

#include "CollisionBuzzSawComponent.h"
#include "CameraComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Const.h"
#include "Utils.h"
#include "Settings.h"
#include "af/Utils.h"
#include "SceneObjectFactory.h"

namespace af
{
    CollisionBuzzSawComponent::CollisionBuzzSawComponent()
    : t_(0.0f),
      sndHit_(audio.createSound("buzzsaw.ogg"))
    {
    }

    CollisionBuzzSawComponent::~CollisionBuzzSawComponent()
    {
    }

    void CollisionBuzzSawComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionBuzzSawComponent::preSolve(SInt32 cookie, b2Contact* contact)
    {
        HitMap::iterator it = hitMap_.find(cookie);

        if (it == hitMap_.end()) {
            return;
        }

        if (contact->GetManifold()->pointCount > 0) {
            b2WorldManifold worldManifold;

            contact->GetWorldManifold(&worldManifold);

            it->second.second = worldManifold.points[0];
        }
    }

    void CollisionBuzzSawComponent::beginCollision(const Collision& collision)
    {
        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
            SceneObjectTypePlayer | SceneObjectTypeAlly | SceneObjectTypeRock | SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding;

        if (collision.fixtureA->IsSensor() ||
            collision.fixtureB->IsSensor()) {
            return;
        }

        SceneObject* other = collision.getOther(parent());

        if (!hitTypes[other->type()]) {
            return;
        }

        hitMap_[collision.cookie] = std::make_pair(other->shared_from_this(),
            collision.worldManifold.points[0]);
    }

    void CollisionBuzzSawComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionBuzzSawComponent::endCollision(SInt32 cookie)
    {
        hitMap_.erase(cookie);
    }

    void CollisionBuzzSawComponent::update(float dt)
    {
        t_ -= dt;

        if (t_ > 0.0f) {
            return;
        }

        bool wasHit = false;

        for (HitMap::const_iterator it = hitMap_.begin(); it != hitMap_.end(); ++it) {
            if (it->second.first->scene()) {
                b2Vec2 tmp = it->second.first->pos() - parent()->pos();
                tmp.Normalize();

                it->second.first->applyLinearImpulse(settings.sawer.fixedSawImpulse * tmp, it->second.second, true);

                for (int i = 0; i < 3; ++i) {
                    b2Vec2 offset = angle2vec(getRandom(0.0f, 2.0f * b2_pi), getRandom(0.1f, 0.5f));
                    if (it->second.first->type() == SceneObjectTypePlayer) {
                        scene()->addObject(sceneObjectFactory.createMissileHit1(MaterialFlesh, it->second.second + offset));
                    } else {
                        scene()->addObject(sceneObjectFactory.createMissileHit1(it->second.first->shared_from_this(), it->second.second + offset));
                    }
                }

                if ((it->second.first->type() != SceneObjectTypeEnemy) &&
                    (it->second.first->type() != SceneObjectTypeEnemyBuilding)) {
                    it->second.first->changeLife2(parent()->stunned() ? SceneObjectTypeNeutralMissile : SceneObjectTypeEnemyMissile, -settings.sawer.fixedSawDamage);
                } else {
                    it->second.first->changeLife2(parent()->stunned() ? SceneObjectTypeNeutralMissile : SceneObjectTypeEnemyMissile, -settings.sawer.fixedSawEnemyDamage);
                }

                wasHit = true;
            }
        }

        if (wasHit) {
            sndHit_->play();
        }

        t_ = settings.sawer.fixedSawDamageTimeout;
    }

    void CollisionBuzzSawComponent::onRegister()
    {
    }

    void CollisionBuzzSawComponent::onUnregister()
    {
        hitMap_.clear();
    }
}
