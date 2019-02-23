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

#include "GasCloudComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "SceneObjectFactory.h"
#include "ExplosionComponent.h"
#include "PlayerComponent.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    float GasCloudComponent::damageT_ = 0.0f;
    int GasCloudComponent::numInside_ = 0;

    GasCloudComponent::GasCloudComponent(const ParticleEffectComponentPtr& pec, float delay, float offset)
    : PhasedComponent(phaseThink),
      pec_(pec),
      delay_(delay),
      tweenTime_(0.0f),
      propagateT_(offset),
      snd_(audio.createSound("toxic2.ogg"))
    {
        light_ = boost::make_shared<PointLight>();
        light_->setColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
        light_->setDynamic(false);
        light_->setDiffuse(true);
        light_->setDistance(10.0f);
        light_->setXray(true);
        light_->setIntensity(1.5f);
        light_->setNumRays(10);

        tweening_ = boost::make_shared<SingleTweening>(10.0f,
            EaseLinear, 0.0f, 0.4f);
    }

    GasCloudComponent::~GasCloudComponent()
    {
    }

    void GasCloudComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GasCloudComponent::update(float dt)
    {
        if (tweening_) {
            float value = tweening_->getValue(tweenTime_);

            light_->setColor(Color(1.0f, 1.0f, 0.0f, value));

            if (tweening_->finished(tweenTime_)) {
                tweening_.reset();
            }

            tweenTime_ += dt;
        }

        propagateT_ += dt;

        if (propagateT_ >= delay_) {
            propagateT_ = 0.0f;

            b2Rot rot(2.0f * b2_pi / 10);
            b2Vec2 dir(9.0f, 0.0f);

            for (UInt32 i = 0; i < 10; ++i) {
                bool found = false;

                scene()->rayCast(parent()->pos(), parent()->pos() + dir,
                    boost::bind(&GasCloudComponent::rayCastCb, this,
                    _1, _2, _3, _4, boost::ref(found)));

                if (!found) {
                    float newDelay = delay_ * settings.gasCloud1.propagationFactor;
                    if (newDelay > settings.gasCloud1.propagationMaxDelay) {
                        newDelay = settings.gasCloud1.propagationMaxDelay;
                    }
                    SceneObjectPtr obj = sceneObjectFactory.createGasCloud1Inner(newDelay, 0.0f, pec_->zOrder());
                    obj->setPos(parent()->pos() + dir);
                    if (parent()->parentObject()) {
                        parent()->parentObject()->addObject(obj);
                    } else {
                        parent()->addObject(obj);
                    }
                }

                dir = b2Mul(rot, dir);
            }
        }

        if (target_) {
            damageT_ += dt / numInside_;

            if (damageT_ < settings.gasCloud1.damageTimeout) {
                return;
            }

            damageT_ = 0.0f;

            target_->changeLife(-settings.gasCloud1.damage);
            for (int i = 0; i < 3; ++i) {
                b2Vec2 offset = angle2vec(getRandom(0.0f, 2.0f * b2_pi), getRandom(0.1f, 0.5f));
                scene()->addObject(sceneObjectFactory.createMissileHit1(MaterialFlesh, target_->pos() + offset));
            }
            snd_->play();

            if (target_->type() == SceneObjectTypePlayer) {
                PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();
                if (pc) {
                    pc->setDamagedByGas();
                }
            }
        }
    }

    void GasCloudComponent::sensorEnter(const SceneObjectPtr& other)
    {
        target_ = other;
        ++numInside_;
    }

    void GasCloudComponent::sensorExit(const SceneObjectPtr& other)
    {
        target_.reset();
        --numInside_;
        if (numInside_ == 0) {
            damageT_ = 0.0f;
        }
    }

    void GasCloudComponent::onRegister()
    {
        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);
    }

    void GasCloudComponent::onUnregister()
    {
        if (light_) {
            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();
        }
        target_.reset();
    }

    float GasCloudComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, bool& found)
    {
        SceneObject* obj = SceneObject::fromFixture(fixture);

        if (fixture->IsSensor()) {
            if (obj->findComponent<GasCloudComponent>()) {
                found = true;
                return 0.0f;
            }
            return -1;
        }

        if ((obj->type() == SceneObjectTypeRock) ||
            (obj->type() == SceneObjectTypeTerrain) ||
            (obj->type() == SceneObjectTypeBlocker)) {
            found = true;
            return 0.0f;
        }

        return -1;
    }
}
