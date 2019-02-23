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

#include "ExplosionComponent.h"
#include "CameraComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    ExplosionComponent::ExplosionComponent(const ParticleEffectComponentPtr& pec,
        float blastRadius, UInt32 blastNumRays)
    : PhasedComponent(phaseThink),
      pec_(pec),
      shakeAmount_(0.0f),
      tweenTime_(0.0f),
      blast_(false),
      blastRadius_(blastRadius),
      blastNumRays_(blastNumRays),
      color_(1.0f, 1.0f, 1.0f)
    {
    }

    ExplosionComponent::~ExplosionComponent()
    {
    }

    void ExplosionComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ExplosionComponent::update(float dt)
    {
        if (tweening_) {
            float value = tweening_->getValue(tweenTime_);

            Color color = color_;
            color.rgba[3] = value;

            light_->setColor(color);

            if (tweening_->finished(tweenTime_)) {
                tweening_.reset();
                light_->remove();
                light_.reset();
            }

            tweenTime_ += dt;
        }

        if (pec_->isComplete()) {
            parent()->removeFromParent();
        }
    }

    void ExplosionComponent::setLight(float distance, float duration, bool xray, float maxAlpha)
    {
        Color color = color_;
        color.rgba[3] = 0.0f;

        light_ = boost::make_shared<PointLight>();
        light_->setColor(color);
        light_->setDynamic(false);
        light_->setSoftLength(distance / 8);
        light_->setDistance(distance);
        light_->setXray(xray);

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(duration / 2,
            EaseOutQuad, 0.0f, maxAlpha));
        tweening->addTweening(boost::make_shared<SingleTweening>(duration / 2,
            EaseInQuad, maxAlpha, 0.0f));

        tweening_ = tweening;
    }

    void ExplosionComponent::setShake(float amount)
    {
        shakeAmount_ = amount;
    }

    void ExplosionComponent::setSound(const AudioSourcePtr& value)
    {
        sound_ = value;
    }

    void ExplosionComponent::setBlast(const SceneObjectPtr& sourceObj, float impulse, float damage, const SceneObjectTypes& types)
    {
        blast_ = true;
        blastImpulse_ = impulse;
        blastDamage_ = damage;
        blastTypes_ = types;
        blastSourceObj_ = sourceObj;
    }

    void ExplosionComponent::onRegister()
    {
        if (shakeAmount_ > 0.0f) {
            scene()->camera()->findComponent<CameraComponent>()->shake(shakeAmount_);
        }

        if (light_) {
            light_->setWorldTransform(parent()->getSmoothTransform());

            scene()->lighting()->addLight(light_);
        }

        if (sound_) {
            sound_->play();
        }

        if (blast_) {
            scene()->blast(parent()->pos(),
                blastRadius_, blastImpulse_, blastDamage_, blastTypes_,
                blastNumRays_, blastSourceObj_.get());
            blastSourceObj_.reset();
        }
    }

    void ExplosionComponent::onUnregister()
    {
        if (light_) {
            light_->remove();
        }

        blastSourceObj_.reset();
    }
}
