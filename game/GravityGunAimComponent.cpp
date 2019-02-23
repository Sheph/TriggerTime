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

#include "GravityGunAimComponent.h"
#include "PhysicsBodyComponent.h"
#include "Scene.h"
#include "Const.h"
#include "AssetManager.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

namespace af
{
    GravityGunAimComponent::GravityGunAimComponent(const b2Vec2& pos)
    : PhasedComponent(phaseThink),
      p1_(pos),
      p2_(pos + b2Vec2(settings.player.ggAimLength, 0.0f)),
      sndAim_(audio.createSound("gg_aim.ogg")),
      sndLocked_(audio.createSound("gg_locked.ogg"))
    {
    }

    GravityGunAimComponent::~GravityGunAimComponent()
    {
    }

    void GravityGunAimComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GravityGunAimComponent::update(float dt)
    {
        b2Vec2 p1 = b2Mul(parent()->getTransform(), p1_);
        b2Vec2 p2 = b2Mul(parent()->getTransform(), p2_);

        b2Vec2 pt = p2;
        SceneObject* obj = NULL;

        scene()->rayCast(p1, p2, boost::bind(&GravityGunAimComponent::rayCastCb, this,
            _1, _2, _3, _4, boost::ref(pt), boost::ref(obj)));

        aimLine_->setLength((pt - p1).Length());

        if (target_ && !target_->scene()) {
            aimCircle_.reset();
        }

        if (obj != target_.get()) {
            if (aimCircle_) {
                aimCircle_->removeFromParent();
            }

            target_.reset();

            if (obj) {
                sndLocked_->play();

                target_ = obj->shared_from_this();
                hitPoint_ = pt;

                aimCircle_ = boost::make_shared<RenderDottedCircleComponent>(
                    b2Vec2_zero, settings.player.ggAimWidth,
                    assetManager.getDrawable("common2/dot.png"), zOrderMarker);

                PhysicsBodyComponentPtr c = target_->findComponent<PhysicsBodyComponent>();

                b2Vec2 ext = c->computeAABB(b2Transform(b2Vec2_zero, b2Rot(0))).GetExtents();

                float radius = b2Max(ext.x, ext.y) + settings.player.ggAimRadiusPadding;

                aimCircle_->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
                aimCircle_->setRadius(radius);
                aimCircle_->setDotVelocity(settings.player.ggAimDotVelocity, radius);
                aimCircle_->setDotDistance(settings.player.ggAimDotDistance, radius);

                target_->addComponent(aimCircle_);
            }
        }
    }

    void GravityGunAimComponent::onRegister()
    {
        aimLine_ = boost::make_shared<RenderDottedLineComponent>(p1_, 0.0f,
            settings.player.ggAimWidth,
            assetManager.getDrawable("common2/dot.png"), zOrderMarker);

        aimLine_->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
        aimLine_->setDotVelocity(settings.player.ggAimDotVelocity);
        aimLine_->setDotDistance(settings.player.ggAimDotDistance);

        parent()->addComponent(aimLine_);

        sndAim_->setLoop(true);
        sndAim_->play();
    }

    void GravityGunAimComponent::onUnregister()
    {
        aimLine_->removeFromParent();
        aimLine_.reset();

        if (aimCircle_) {
            aimCircle_->removeFromParent();
            aimCircle_.reset();
        }

        target_.reset();

        sndAim_->stop();
    }

    float GravityGunAimComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
        SceneObject*& obj)
    {
        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeBlocker) |
            SceneObjectTypePlayerMissile | SceneObjectTypeEnemyMissile |
            SceneObjectTypeAllyMissile | SceneObjectTypeNeutralMissile | SceneObjectTypeGarbage |
            SceneObjectTypeVehicle | SceneObjectTypeDeadbody | SceneObjectTypeShield;

        if (fixture->IsSensor() ||
            (SceneObject::fromFixture(fixture) == parent())) {
            return -1;
        }

        if (SceneObject::fromFixture(fixture)->gravityGunAware()) {
            obj = SceneObject::fromFixture(fixture);
            finalPoint = point;

            return fraction;
        }

        if (hitTypes[SceneObject::fromFixture(fixture)->type()]) {
            return -1;
        }

        obj = NULL;
        finalPoint = point;

        return fraction;
    }
}
