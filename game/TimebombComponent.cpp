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

#include "TimebombComponent.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Const.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    TimebombComponent::TimebombComponent(const SceneObjectPtr& obj, float height)
    : PhasedComponent(phaseThink),
      obj_(obj),
      height_(height),
      blast_(false),
      t_(0.0f),
      beepT_(0.0f),
      beepHoldT_(0.0f),
      snd_(audio.createSound("timebomb.ogg"))
    {
        xf_.SetIdentity();
    }

    TimebombComponent::~TimebombComponent()
    {
    }

    void TimebombComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TimebombComponent::update(float dt)
    {
        b2Transform xf = b2Mul(parent()->getTransform(), xf_);

        if (t_ <= 0.0f) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(xf);

            if (blast_) {
                explosion->findComponent<ExplosionComponent>()->setBlast(SceneObjectPtr(),
                    impulse_, damage_, types_);
            }

            scene()->addObject(explosion);

            TimeoutFn fn = fn_;

            parent()->removeFromParent();

            if (fn) {
                fn();
            }

            return;
        }

        if (beepT_ <= 0.0f) {
            light_->setDistance(height_ * 4.0f);
            beepT_ = 0.3f;
            beepHoldT_ = 0.1f;
        }

        if (beepHoldT_ <= 0.0f) {
            light_->setDistance(height_);
        }

        t_ -= dt;
        beepT_ -= dt;
        beepHoldT_ -= dt;

        obj_->setTransform(xf);
    }

    void TimebombComponent::setBlast(float impulse, float damage, const SceneObjectTypes& types)
    {
        blast_ = true;
        impulse_ = impulse;
        damage_ = damage;
        types_ = types;
    }

    void TimebombComponent::onRegister()
    {
        obj_->setTransform(b2Mul(parent()->getTransform(), xf_));

        light_ = boost::make_shared<PointLight>();
        light_->setColor(settings.timebomb.color);
        light_->setDistance(height_);
        light_->setXray(true);

        lightC_ = boost::make_shared<LightComponent>();
        lightC_->attachLight(light_);

        obj_->addComponent(lightC_);

        scene()->addObject(obj_);

        t_ = 4.2f;
        beepT_ = 1.0f;

        snd_->play();
    }

    void TimebombComponent::onUnregister()
    {
        obj_->removeFromParent();
        obj_.reset();
    }
}
