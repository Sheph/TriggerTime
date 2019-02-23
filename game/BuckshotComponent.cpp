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

#include "BuckshotComponent.h"
#include "MeasurementComponent.h"
#include "Scene.h"
#include "Const.h"
#include "SceneObjectFactory.h"
#include "SingleTweening.h"
#include "Settings.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    BuckshotComponent::BuckshotComponent(const CollisionDieComponentPtr& cdc,
                                         const RenderQuadComponentPtr& rc,
                                         const RenderTrailComponentPtr& rtc,
                                         const ConeLightPtr& light,
                                         float damage,
                                         float width1, float width2, float maxLength)
    : PhasedComponent(phaseThink | phasePreRender),
      cdc_(cdc),
      rc_(rc),
      rtc_(rtc),
      light_(light),
      damage_(damage),
      width1_(width1),
      width2_(width2),
      maxLength_(maxLength),
      haveStartPos_(false),
      wasFreezable_(false)
    {
    }

    BuckshotComponent::~BuckshotComponent()
    {
    }

    void BuckshotComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BuckshotComponent::update(float dt)
    {
        if (cdc_->collidedWith()) {
            return;
        }

        if (haveStartPos_) {
            if (b2DistanceSquared(parent()->pos(), startPos_) > 0.000001f) {
                parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
            } else {
                parent()->setVisible(true);
                parent()->setLinearVelocity(actualLinearVelocity_);
                parent()->resetSmooth();
                parent()->setFreezable(wasFreezable_);
                haveStartPos_ = false;
            }
        }
    }

    void BuckshotComponent::preRender(float dt)
    {
        if (tweening_) {
            tweenTime_ += dt;

            float v = tweening_->getValue(tweenTime_);

            Color c = rc_->color();
            c.rgba[3] = rcColor_.rgba[3] * v;
            rc_->setColor(c);

            c = rtc_->color();
            c.rgba[3] = rtcColor_.rgba[3] * v;
            rtc_->setColor(c);

            c = light_->color();
            c.rgba[3] = lightColor_.rgba[3] * v;
            light_->setColor(c);

            if (tweening_->finished(tweenTime_)) {
                parent()->removeFromParent();
            }

            return;
        }

        float len = 0.0f;

        if (!haveStartPos_) {
            len = (parent()->smoothPos() - pos_).Length();

            if (len > 0.0f) {
                if (len <= maxLength_) {
                    rtc_->setLength(len);
                    light_->setPos(lightPos_ + b2Vec2(-len, 0.0f));
                    light_->setFarDistance(len);
                }

                if (len * (width2_ / maxLength_) <= width2_) {
                    float w = len * (width2_ / maxLength_);
                    if (w < rtc_->width2()) {
                        w = rtc_->width2();
                    }
                    rtc_->setWidth1(w);
                    rc_->setHeight(w);
                    light_->setConeAngle(std::atan2(w / 2.0f, len));
                }
            }
        }

        if (cdc_->collidedWith()) {
            float maxDamageLength = maxLength_ * 1.0f;

            float damage = damage_;

            if (len < maxDamageLength) {
                damage += ((maxDamageLength - len) * damage_ * 0.5f) / maxDamageLength;
            }

            if (cdc_->collidedWith()->material() == MaterialBulletProof) {
                MeasurementComponentPtr mc =
                    cdc_->collidedWith()->findComponent<MeasurementComponent>();
                if (mc) {
                    mc->changeBulletDamageMissed(damage);
                }
            } else {
                cdc_->collidedWith()->changeLife2(parent(), -damage);
            }

            scene()->addObject(sceneObjectFactory.createMissileHit1(cdc_->collidedWith(), cdc_->collisionPoint()));

            scene()->stats()->incShotsHit(parent(), cdc_->collidedWith().get());

            parent()->setPos(pos_ + b2Dot(cdc_->collisionPoint() - pos_, dir_) * dir_);
            parent()->resetSmooth();
            light_->setWorldTransform(parent()->getSmoothTransform());

            tweening_ = boost::make_shared<SingleTweening>(0.5f, EaseOutQuad, 1.0f, 0.0f);
            tweenTime_ = 0.0f;

            return;
        }
    }

    void BuckshotComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void BuckshotComponent::onRegister()
    {
        pos_ = parent()->smoothPos();
        lightPos_ = light_->pos();
        dir_ = parent()->linearVelocity();
        dir_.Normalize();

        rcColor_ = rc_->color();
        rtcColor_ = rtc_->color();
        lightColor_ = light_->color();

        rtc_->setLength(0.0f);

        rc_->setHeight(width1_);
        rtc_->setWidth1(width1_);
        rtc_->setWidth2(width1_);

        light_->setFarDistance(0.0f);

        if (haveStartPos_) {
            pos_ = startPos_;
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
        }
    }

    void BuckshotComponent::onUnregister()
    {
    }
}
