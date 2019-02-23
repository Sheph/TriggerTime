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

#include "BlasterShotComponent.h"
#include "MeasurementComponent.h"
#include "SceneObjectFactory.h"
#include "Scene.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "Const.h"
#include "Utils.h"
#include "Settings.h"
#include "RenderQuadComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BlasterShotComponent::BlasterShotComponent(const CollisionDieComponentPtr& cdc,
                                               const ParticleEffectComponentPtr& pec,
                                               float damage, bool twisted)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      pec_(pec),
      damage_(damage),
      tweenTime_(0.0f),
      twisted_(twisted),
      haveStartPos_(false),
      wasFreezable_(false)
    {
    }

    BlasterShotComponent::~BlasterShotComponent()
    {
    }

    void BlasterShotComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BlasterShotComponent::update(float dt)
    {
        if (pec_->isAllowCompletion()) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        if (twisted_ && !haveStartPos_) {
            tweenTime_ += dt;

            b2Vec2 vel = dir_ + tweening_->getValue(tweenTime_) * skewedDir_;

            parent()->setLinearVelocity(vel);

            parent()->setAngle(vec2angle(vel));
        }

        if (cdc_->collidedWith()) {
            pec_->setPaused(false);
            if (cdc_->collidedWith()->material() == MaterialBulletProof) {
                MeasurementComponentPtr mc =
                    cdc_->collidedWith()->findComponent<MeasurementComponent>();
                if (mc) {
                    mc->changeBulletDamageMissed(damage_);
                }
            } else {
                cdc_->collidedWith()->changeLife2(parent(), -damage_);
            }

            scene()->addObject(sceneObjectFactory.createMissileHit1(cdc_->collidedWith(), cdc_->collisionPoint()));

            scene()->stats()->incShotsHit(parent(), cdc_->collidedWith().get());

            pec_->update(settings.physics.fixedTimestep);

            pec_->allowCompletion();

            parent()->findComponent<RenderQuadComponent>()->removeFromParent();
            parent()->findComponent<LightComponent>()->removeFromParent();

            return;
        }

        if (haveStartPos_) {
            if (b2DistanceSquared(parent()->pos(), startPos_) > 0.000001f) {
                parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
                return;
            } else {
                parent()->setVisible(true);
                if (twisted_) {
                    b2Vec2 vel = dir_ + tweening_->getValue(tweenTime_) * skewedDir_;

                    parent()->setLinearVelocity(vel);

                    parent()->setAngle(vec2angle(vel));
                } else {
                    parent()->setLinearVelocity(actualLinearVelocity_);
                }
                parent()->resetSmooth();
                parent()->setFreezable(wasFreezable_);
                haveStartPos_ = false;
                pec_->setPaused(false);
            }
        }
    }

    void BlasterShotComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void BlasterShotComponent::onRegister()
    {
        if (twisted_) {
            parent()->setLinearVelocity((1.0f + 1.0f / ((rand() % 5) + 5.0f)) *
                parent()->linearVelocity());

            dir_ = parent()->linearVelocity();
            skewedDir_ = dir_.Skew();
            skewedDir_.Normalize();

            float len = dir_.Length() / ((rand() % 3) + 1.0f);
            float t = 0.2f;

            if ((rand() % 2) == 0) {
                len = -len;
            }

            SequentialTweeningPtr tweening =
                boost::make_shared<SequentialTweening>(true);

            tweening->addTweening(boost::make_shared<SingleTweening>(t, EaseLinear, len, -len));
            tweening->addTweening(boost::make_shared<SingleTweening>(t, EaseLinear, -len, len));

            tweening_= tweening;
            tweenTime_ = (t * 2.0f) / ((rand() % 3) + 1.0f);
        }

        if (haveStartPos_) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
            pec_->setPaused(true);
        }
    }

    void BlasterShotComponent::onUnregister()
    {
    }
}
