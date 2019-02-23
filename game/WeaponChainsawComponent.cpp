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

#include "WeaponChainsawComponent.h"
#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "Scene.h"
#include "Const.h"
#include "Renderer.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    static const float chainsawRadius = 0.6f;

    WeaponChainsawComponent::WeaponChainsawComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeChainsaw),
      damage_(0.0f),
      snd_(audio.createSound("chainsaw.ogg")),
      t_(0.0f)
    {
        snd_->setLoop(true);
    }

    WeaponChainsawComponent::~WeaponChainsawComponent()
    {
    }

    void WeaponChainsawComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponChainsawComponent::update(float dt)
    {
        t_ -= dt;

        if (!triggerHeld()) {
            snd_->stop();
            return;
        }

        b2Vec2 p = b2Mul(parent()->getTransform(), pos());

        scene()->stats()->incTimeFired(this, 0.1f);

        bool hit = false;

        b2AABB aabb;

        aabb.lowerBound = p - b2Vec2(chainsawRadius, chainsawRadius);
        aabb.upperBound = p + b2Vec2(chainsawRadius, chainsawRadius);

        scene()->queryAABB(aabb, boost::bind(&WeaponChainsawComponent::queryAABBCb,
            this, _1, boost::cref(p), dt, boost::ref(hit)));

        if (t_ <= 0.0f) {
            t_ = 0.1f;
        }

        if (haveSound() && (snd_->status() != AudioSource::Playing)) {
            snd_->play();
        }
    }

    void WeaponChainsawComponent::debugDraw()
    {
        static const int32 circleSegments = 10;
        static const float32 circleIncrement = 2.0f * b2_pi / circleSegments;

        if (!triggerHeld()) {
            return;
        }

        b2Vec2 center = b2Mul(parent()->getTransform(), pos());

        float32 theta = 0.0f;
        b2Vec2 p0;

        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        for (int32 i = 0; i < circleSegments; ++i) {
            b2Vec2 v = center + chainsawRadius * b2Vec2(cosf(theta), sinf(theta));

            rop.addVertex(v.x, v.y);

            if (i == 0) {
                p0 = v;
            }

            theta += circleIncrement;
        }

        rop.addVertex(p0.x, p0.y);

        rop.addColors(Color(1.0f, 0.0f, 0.0f));
    }

    void WeaponChainsawComponent::reload()
    {
    }

    void WeaponChainsawComponent::onRegister()
    {
    }

    void WeaponChainsawComponent::onUnregister()
    {
        snd_->stop();
    }

    bool WeaponChainsawComponent::queryAABBCb(b2Fixture* fixture, const b2Vec2& p, float dt, bool& hit)
    {
        SceneObject* obj = SceneObject::fromFixture(fixture);

        if (fixture->IsSensor() || (obj == parent())) {
            return true;
        }

        if (objectType() == SceneObjectTypeEnemyMissile) {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypePlayer) | SceneObjectTypeAlly | SceneObjectTypeGizmo;
            if (!hitTypes[obj->type()]) {
                return true;
            }
        } else if (objectType() == SceneObjectTypeNeutralMissile) {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypePlayer) | SceneObjectTypeAlly |
                SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding | SceneObjectTypeGizmo;
            if (!hitTypes[obj->type()]) {
                return true;
            }
        } else {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeEnemy) | SceneObjectTypeEnemyBuilding | SceneObjectTypeGizmo;
            if (!hitTypes[obj->type()]) {
                return true;
            }
        }

        b2Rot rot(2.0f * b2_pi / 5);
        b2Vec2 dir(chainsawRadius, 0.0f);

        bool found = false;

        for (int i = 0; i < 5; ++i) {
            if (fixture->TestPoint(p + dir)) {
                found = true;
                break;
            }

            dir = b2Mul(rot, dir);
        }

        if (!found) {
            return true;
        }

        obj->changeLife(-damage_ * dt);

        if (!hit) {
            hit = scene()->stats()->incTimeHit(this, obj, 0.1f);
        }

        if (t_ <= 0.0f) {
            b2Vec2 offset = angle2vec(getRandom(0.0f, 2.0f * b2_pi), getRandom(0.0f, chainsawRadius));

            scene()->addObject(sceneObjectFactory.createMissileHit1(obj->shared_from_this(), p + offset));
        }

        return true;
    }
}
