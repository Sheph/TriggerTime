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

#include "WeaponHeaterComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "AssetManager.h"
#include "Const.h"
#include "Renderer.h"
#include "Utils.h"
#include "CameraComponent.h"
#include "SceneObjectFactory.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include "MeasurementComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponHeaterComponent::WeaponHeaterComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeHeater),
      damage_(0.0f),
      length_(1.0f),
      impulse_(0.0f),
      duration_(0.0f),
      interval_(0.0f),
      t_(0.0f),
      minWidth_(0.18f),
      maxWidth_(1.8f),
      measure_(false),
      snd_(audio.createSound("tetrobot_attack.ogg"))
    {

    }

    WeaponHeaterComponent::~WeaponHeaterComponent()
    {
    }

    void WeaponHeaterComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponHeaterComponent::update(float dt)
    {
        if (tweening_) {
            t_ += dt;

            if (tweening_->finished(t_)) {
                if ((interval_ <= 0.0f) && triggerHeld()) {
                    t_ = 0.0f;
                    if (haveSound()) {
                        snd_->play();
                    }
                } else {
                    tweening_.reset();
                    t_ = interval_;

                    if (rc_) {
                        rc_->removeFromParent();
                        rc_.reset();
                    }

                    if (light_) {
                        lightC_->removeFromParent();
                        lightC_.reset();
                        light_.reset();
                    }

                    return;
                }
            }

            float v = tweening_->getValue(t_);

            b2Transform xf = b2Mul(parent()->getTransform(),
                b2Transform(pos(), b2Rot(angle())));

            b2Vec2 p1 = b2Mul(xf, b2Vec2_zero);
            b2Vec2 p2 = b2Mul(xf, b2Vec2(length_, 0.0f));

            b2Vec2 pt = p2;
            SceneObject* obj = NULL;

            scene()->rayCast(p1, p2, boost::bind(&WeaponHeaterComponent::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(pt), boost::ref(obj)));

            if (obj) {
                obj->changeLife2(fakeMissile_.get(), -damage_ * dt);
                b2Vec2 tmp = p2 - p1;
                tmp.Normalize();
                obj->applyLinearImpulse(impulse_ * dt * tmp, pt, true);
                if (measure_) {
                    MeasurementComponentPtr c = obj->findComponent<MeasurementComponent>();
                    if (c) {
                        c->changeHeaterDamageReceived(impulse_ * dt);
                    }
                }
            }

            float len = (pt - p1).Length() - flashDistance();

            if (len < 0.0f) {
                len = 0.0f;
            }

            rc_->setLength(len);
            rc_->setWidth1NoTex(v);
            rc_->setWidth2NoTex(v);

            light_->setPos(pos() + angle2vec(angle(), flashDistance() + len / 2.0f));
            light_->setLength(len / 2.0f);
            light_->setDistance(0.2f + v * 0.5f);
        } else {
            t_ -= dt;

            if ((t_ > 0.0f) || !triggerHeld()) {
                return;
            }

            SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

            tweening->addTweening(boost::make_shared<SingleTweening>(duration_ / 2.0f, EaseInOutQuad, minWidth_, maxWidth_));
            tweening->addTweening(boost::make_shared<SingleTweening>(duration_ / 2.0f, EaseInOutQuad, maxWidth_, minWidth_));

            tweening_ = tweening;
            t_ = 0.0f;

            b2Vec2 p = pos() + angle2vec(angle(), flashDistance());

            rc_ = boost::make_shared<RenderTrailComponent>(p, angle(),
                assetManager.getDrawable("lbolt3.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
                15);
            rc_->setLength(0.0f);
            rc_->setWidth1(maxWidth_);
            rc_->setWidth2(maxWidth_);

            parent()->addComponent(rc_);

            light_ = boost::make_shared<LineLight>();
            light_->setAngle(angle() + b2_pi / 2.0f);
            light_->setColor(Color(1.0f, 0.0f, 0.0f, 0.5f));
            light_->setDiffuse(false);
            light_->setXray(true);
            light_->setBothWays(true);
            light_->setDistance(0.0f);
            light_->setIntensity(1.5f);

            lightC_ = boost::make_shared<LightComponent>();

            lightC_->attachLight(light_);

            parent()->addComponent(lightC_);

            if (haveSound()) {
                snd_->play();
            }
        }
    }

    void WeaponHeaterComponent::reload()
    {
    }

    void WeaponHeaterComponent::onRegister()
    {
        fakeMissile_ = sceneObjectFactory.createFakeMissile(objectType());
        scene()->addObject(fakeMissile_);
    }

    void WeaponHeaterComponent::onUnregister()
    {
        fakeMissile_->removeFromParent();
        fakeMissile_.reset();

        if (rc_) {
            rc_->removeFromParent();
            rc_.reset();
        }

        if (light_) {
            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();
        }
    }

    float WeaponHeaterComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
        SceneObject*& obj)
    {
        if (fixture->IsSensor() || !fakeMissile_->collidesWith(fixture)) {
            return -1;
        }

        finalPoint = point;
        obj = SceneObject::fromFixture(fixture);

        return fraction;
    }
}
