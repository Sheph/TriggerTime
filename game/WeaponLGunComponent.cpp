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

#include "WeaponLGunComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "AssetManager.h"
#include "Const.h"
#include "Renderer.h"
#include "Utils.h"
#include "CameraComponent.h"
#include "SceneObjectFactory.h"
#include <boost/make_shared.hpp>

namespace af
{
    static const struct {
        const char* lbolt1Path;
        const char* lbolt2Path;
        Color color;
    } lboltData[] = {
        {"lbolt1.png", "lbolt2.png", Color(0.0f, 0.0f, 1.0f)},
        {"lbolt3.png", "lbolt4.png", Color(1.0f, 0.0f, 0.0f)}
    };

    WeaponLGunComponent::WeaponLGunComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeLGun),
      damage_(0.0f),
      length_(1.0f),
      impulse_(0.0f),
      t_(0.0f),
      i_(0),
      snd_(audio.createSound("lfire.ogg"))
    {
        snd_->setLoop(true);
    }

    WeaponLGunComponent::~WeaponLGunComponent()
    {
    }

    void WeaponLGunComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponLGunComponent::update(float dt)
    {
        if (!triggerHeld() || !changeAmmo(8.0f * dt)) {
            snd_->stop();

            for (size_t i = 0; i < sizeof(rc_)/sizeof(rc_[0]); ++i) {
                if (rc_[i]) {
                    rc_[i]->removeFromParent();
                    rc_[i].reset();
                }
            }

            if (light_) {
                lightC_->removeFromParent();
                lightC_.reset();
                light_.reset();
                sparkLight_.reset();
                flashLight_.reset();
            }

            if (pec_) {
                pec_->removeFromParent();
                pec_.reset();
                flashPec_->removeFromParent();
                flashPec_.reset();
            }

            return;
        }

        if (haveSound() && (snd_->status() != AudioSource::Playing)) {
            snd_->play();
        }

        if (!rc_[0]) {
            b2Vec2 p = pos() + angle2vec(angle(), flashDistance());

            rc_[0] = boost::make_shared<RenderLBoltComponent>(p, angle(),
                assetManager.getImage(lboltData[i_].lbolt1Path, Texture::WrapModeRepeat, Texture::WrapModeClamp),
                zOrderEffects + 1);
            rc_[0]->setLength(0.0f);
            rc_[0]->setSubLength(5.0f);
            rc_[0]->setWidth(2.5f);
            rc_[0]->setMaxOffset(1.5f);
            rc_[0]->setSegmentLength(0.8f);
            parent()->addComponent(rc_[0]);

            for (size_t i = 1; i < sizeof(rc_)/sizeof(rc_[0]); ++i) {
                rc_[i] = boost::make_shared<RenderLBoltComponent>(p, angle(),
                    assetManager.getImage(lboltData[i_].lbolt2Path, Texture::WrapModeRepeat, Texture::WrapModeClamp),
                    zOrderEffects);
                rc_[i]->setLength(0.0f);
                rc_[i]->setSubLength(5.0f);
                rc_[i]->setWidth(0.2f);
                rc_[i]->setMaxOffset(2.0f);
                rc_[i]->setSegmentLength(0.5f);
                parent()->addComponent(rc_[i]);
            }

            Color tmp = lboltData[i_].color;
            tmp.rgba[3] = 0.5f;

            light_ = boost::make_shared<LineLight>();
            light_->setAngle(angle() + b2_pi / 2.0f);
            light_->setColor(tmp);
            light_->setDiffuse(false);
            light_->setXray(true);
            light_->setBothWays(true);
            light_->setDistance(3.5f);
            light_->setIntensity(1.5f);

            tmp.rgba[3] = 1.0f;

            sparkLight_ = boost::make_shared<PointLight>();
            sparkLight_->setColor(tmp);
            sparkLight_->setDiffuse(false);
            sparkLight_->setXray(true);
            sparkLight_->setDistance(4.5f);

            flashLight_ = boost::make_shared<PointLight>();
            flashLight_->setPos(pos() + angle2vec(angle(), flashDistance()));
            flashLight_->setColor(tmp);
            flashLight_->setDiffuse(false);
            flashLight_->setXray(true);
            flashLight_->setDistance(2.5f);

            lightC_ = boost::make_shared<LightComponent>();

            lightC_->attachLight(light_);
            lightC_->attachLight(sparkLight_);
            lightC_->attachLight(flashLight_);

            parent()->addComponent(lightC_);

            pec_ = assetManager.getParticleEffect("sparks3.p", b2Vec2_zero, 0.0f);
            pec_->setZOrder(zOrderEffects + 2);
            pec_->resetEmit();
            parent()->addComponent(pec_);

            flashPec_ = assetManager.getParticleEffect("sparks4.p", pos() + angle2vec(angle(), flashDistance()), angle());
            flashPec_->setZOrder(zOrderEffects + 2);
            flashPec_->resetEmit();
            parent()->addComponent(flashPec_);

            t_ = 0.0f;
        }

        t_ -= dt;

        b2Transform xf = b2Mul(parent()->getTransform(),
            b2Transform(pos(), b2Rot(angle())));

        b2Vec2 p1 = b2Mul(xf, b2Vec2_zero);
        b2Vec2 p2 = b2Mul(xf, b2Vec2(length_, 0.0f));

        b2Vec2 pt = p2;
        SceneObject* obj = NULL;

        scene()->stats()->incTimeFired(this, dt);

        scene()->rayCast(p1, p2, boost::bind(&WeaponLGunComponent::rayCastCb, this,
            _1, _2, _3, _4, boost::ref(pt), boost::ref(obj)));

        if (obj && ((objectType() != SceneObjectTypePlayerMissile) ||
            scene()->camera()->findComponent<CameraComponent>()->rectVisible(obj->pos(), 4.0f, 4.0f))) {
            obj->changeLife2(fakeMissile_.get(), -damage_ * dt);
            b2Vec2 tmp = p2 - p1;
            tmp.Normalize();
            obj->applyLinearImpulse(impulse_ * dt * tmp, pt, true);
            scene()->stats()->incTimeHit(this, obj, dt);
        }

        if (t_ <= 0.0f) {
            float len = (pt - p1).Length() - flashDistance();

            if (len < 0.0f) {
                len = 0.0f;
            }

            for (size_t i = 0; i < sizeof(rc_)/sizeof(rc_[0]); ++i) {
                rc_[i]->setLength(len);
                rc_[i]->setDirty();
            }

            light_->setPos(pos() + angle2vec(angle(), flashDistance() + len / 2.0f));
            light_->setLength(len / 2.0f);

            sparkLight_->setPos(pos() + angle2vec(angle(), flashDistance() + len));

            pec_->setTransform(b2Transform(pos() + angle2vec(angle(), flashDistance() + len), b2Rot(angle())));

            t_ = 0.05f;
        }
    }

    void WeaponLGunComponent::onFreeze()
    {
        snd_->stop();
    }

    void WeaponLGunComponent::reload()
    {
    }

    void WeaponLGunComponent::onRegister()
    {
        fakeMissile_ = sceneObjectFactory.createFakeMissile(objectType());
        scene()->addObject(fakeMissile_);
    }

    void WeaponLGunComponent::onUnregister()
    {
        fakeMissile_->removeFromParent();
        fakeMissile_.reset();

        snd_->stop();

        for (size_t i = 0; i < sizeof(rc_)/sizeof(rc_[0]); ++i) {
            if (rc_[i]) {
                rc_[i]->removeFromParent();
                rc_[i].reset();
            }
        }

        if (light_) {
            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();
            sparkLight_.reset();
            flashLight_.reset();
        }

        if (pec_) {
            pec_->removeFromParent();
            pec_.reset();
            flashPec_->removeFromParent();
            flashPec_.reset();
        }
    }

    float WeaponLGunComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
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
