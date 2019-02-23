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

#include "LaserComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    LaserComponent::LaserComponent(const b2Vec2& pos, float angle, float length,
        float damage, const RenderBeamComponentPtr& rbc)
    : PhasedComponent(phaseThink),
      damage_(damage),
      rbc_(rbc)
    {
        xf_ = b2Transform(pos, b2Rot(angle));
        p1_ = b2Mul(xf_, b2Vec2(0.0f, 0.0f));
        p2_ = b2Mul(xf_, b2Vec2(length, 0.0f));

        light_ = boost::make_shared<LineLight>();
        light_->setAngle(b2_pi / 2);
        light_->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
        light_->setXray(true);
        light_->setDistance(0.5f);
        light_->setBothWays(true);
    }

    LaserComponent::~LaserComponent()
    {
    }

    void LaserComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void LaserComponent::update(float dt)
    {
        b2Vec2 p1 = b2Mul(parent()->getTransform(), p1_);
        b2Vec2 p2 = b2Mul(parent()->getTransform(), p2_);

        b2Vec2 pt = p2;
        SceneObject* obj = NULL;

        scene()->rayCast(p1, p2, boost::bind(&LaserComponent::rayCastCb, this,
            _1, _2, _3, _4, boost::ref(pt), boost::ref(obj)));

        float len = (pt - p1).Length();

        if (rbc_) {
            rbc_->setLength(len);
        }

        light_->setWorldTransform(b2Mul(parent()->getSmoothTransform(), xf_));
        light_->setPos(b2Vec2(len / 2, 0.0f));
        light_->setLength(len / 2);

        if (obj) {
            if (fakeMissile_) {
                obj->changeLife2(fakeMissile_.get(), -damage_ * dt);
            } else {
                obj->changeLife(-damage_ * dt);
            }
        }
    }

    void LaserComponent::onRegister()
    {
        if (parent()->type() == SceneObjectTypeEnemy) {
            fakeMissile_ = sceneObjectFactory.createFakeMissile(SceneObjectTypeEnemyMissile);
        } else if (parent()->type() == SceneObjectTypePlayer) {
            fakeMissile_ = sceneObjectFactory.createFakeMissile(SceneObjectTypePlayerMissile);
        } else if (parent()->type() == SceneObjectTypeAlly) {
            fakeMissile_ = sceneObjectFactory.createFakeMissile(SceneObjectTypeAllyMissile);
        }
        if (fakeMissile_) {
            scene()->addObject(fakeMissile_);
        }

        parent()->addComponent(rbc_);

        light_->setWorldTransform(b2Mul(parent()->getSmoothTransform(), xf_));

        scene()->lighting()->addLight(light_);
    }

    void LaserComponent::onUnregister()
    {
        if (fakeMissile_) {
            fakeMissile_->removeFromParent();
            fakeMissile_.reset();
        }

        if (rbc_) {
            rbc_->removeFromParent();
        }
        rbc_.reset();
        light_->remove();
    }

    float LaserComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
        SceneObject*& obj)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        if (fakeMissile_) {
            if (!fakeMissile_->collidesWith(fixture)) {
                return -1;
            }
        } else {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeBlocker) |
                SceneObjectTypePlayerMissile | SceneObjectTypeEnemyMissile |
                SceneObjectTypeAllyMissile | SceneObjectTypeNeutralMissile | SceneObjectTypeGarbage |
                SceneObjectTypeVehicle | SceneObjectTypeDeadbody | SceneObjectTypeShield;

            if (hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        }

        finalPoint = point;
        obj = SceneObject::fromFixture(fixture);

        return fraction;
    }
}
