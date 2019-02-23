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

#include "BuddyTongueComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "RenderQuadComponent.h"
#include "RenderHealthbarComponent.h"
#include "SingleTweening.h"
#include "PhysicsJointComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "RenderTentacleComponent.h"
#include "PhysicsRopeComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BuddyTongueComponent::BuddyTongueComponent(const b2Transform& eyeletXf)
    : TargetableComponent(phaseThink),
      eyeletXf_(eyeletXf),
      dead_(false),
      t_(1.0f),
      idleRot_(0.0f),
      idleRotDir_(1)
    {
    }

    BuddyTongueComponent::~BuddyTongueComponent()
    {
    }

    void BuddyTongueComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BuddyTongueComponent::update(float dt)
    {
        if (!dead_ && parent()->dead()) {
            weapon_->trigger(false);

            dead_ = true;

            RenderHealthbarComponentPtr hc =
                parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            assert(parent()->body());

            for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
                RUBEFixture* rf = static_cast<RUBEFixture*>(f->GetUserData());
                if (rf->name() == "root") {
                    b2Filter filter = f->GetFilterData();

                    filter.categoryBits = 0;
                    filter.maskBits = 0;

                    f->SetFilterData(filter);
                }
            }

            parent()->setType(SceneObjectTypeDeadbody);

            parent()->findComponentByName<RenderQuadComponent>("root")->setVisible(false);
            parent()->findComponent<LightComponent>()->lights()[0]->setVisible(true);
            parent()->findComponentByName<RenderQuadComponent>("chain")->setVisible(true);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            scene()->addObject(explosion);
        }

        if (dead_) {
            return;
        }

        updateAutoTarget(dt);

        if (target()) {
            t_ -= dt;

            if (t_ <= 0.0f) {
                t_ = 2.0f;
                idleRotDir_ = -idleRotDir_;
            }

            b2Vec2 v = b2Mul(idleRot_, last_->getDirection(-800.0f));

            parent()->applyForceToCenter(v, true);

            idleRot_ = b2Mul(idleRot_, b2Rot(deg2rad(100.0f) * dt * idleRotDir_));

            b2Vec2 targetDir = last_->getDirection(-1.0f);

            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                parent()->applyTorque(800.0f, true);
            } else {
                parent()->applyTorque(-800.0f, true);
            }
        }

        weapon_->trigger(!!target());
    }

    void BuddyTongueComponent::sensorEnter(const SceneObjectPtr& other)
    {
        if (!dead_ || parent()->invulnerable()) {
            return;
        }

        PhysicsRopeComponentPtr rc = other->findComponent<PhysicsRopeComponent>();

        if (!rc) {
            return;
        }

        if (rope_) {
            return;
        }

        other->setTransform(b2Mul(parent()->getTransform(), eyeletXf_));
        scene()->addWeldJoint(other, parent()->shared_from_this(),
            b2Mul(parent()->getTransform(), eyeletXf_.p), false);

        rope_ = other;

        rc->setHit();
    }

    void BuddyTongueComponent::sensorExit(const SceneObjectPtr& other)
    {
    }

    void BuddyTongueComponent::onRegister()
    {
        for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
            it != parent()->objects().end(); ++it) {
            RenderTentacleComponentPtr tc = (*it)->findComponent<RenderTentacleComponent>();
            if (tc) {
                last_ = tc->objects().back();
                break;
            }
        }
    }

    void BuddyTongueComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        rope_.reset();
        last_.reset();
    }
}
