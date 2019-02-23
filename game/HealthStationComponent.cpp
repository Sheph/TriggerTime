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

#include "HealthStationComponent.h"
#include "PlayerComponent.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "LightComponent.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    HealthStationComponent::HealthStationComponent()
    : PhasedComponent(phaseThink),
      tweenTime_(0.0f),
      t_(0.0f),
      sndWork_(audio.createSound("healthstation_work.ogg"))
    {
    }

    HealthStationComponent::~HealthStationComponent()
    {
    }

    void HealthStationComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void HealthStationComponent::update(float dt)
    {
        if (!tweening_) {
            return;
        }

        if (parent()->life() <= 0.0f) {
            tweening_.reset();
            light_->setVisible(false);
            return;
        }

        if (target_->life() == target_->maxLife()) {
            tweening_.reset();
            light_->setColor(startColor_);
            return;
        }

        float amount = 20.0f * (settings.player.maxLife / settings.healthStation.life) * dt;

        if ((target_->life() + amount) > target_->maxLife()) {
            amount = target_->maxLife() - target_->life();
        }

        if (parent()->life() < amount) {
            amount = parent()->life();
        }

        parent()->changeLife(-amount);
        target_->changeLife(amount);

        if (t_ <= 0.0f) {
            sndWork_->play();
            t_ = 0.4f;
        }

        tweenTime_ += dt;
        t_ -= dt;

        float val = tweening_->getValue(tweenTime_);

        Color c = light_->color();
        c.rgba[3] = val;
        light_->setColor(c);
    }

    void HealthStationComponent::sensorEnter(const SceneObjectPtr& other)
    {
        if (parent()->life() <= 0.0f) {
            return;
        }

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.4f, EaseInOutQuad, startColor_.rgba[3], 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.4f, EaseInOutQuad, 1.0f, startColor_.rgba[3]));

        tweening_ = tweening;
        tweenTime_ = 0.0f;
        t_ = 0.0f;

        target_ = other;
    }

    void HealthStationComponent::sensorExit(const SceneObjectPtr& other)
    {
        tweening_.reset();
        light_->setColor(startColor_);
        target_.reset();
    }

    void HealthStationComponent::onRegister()
    {
        light_ = parent()->findComponent<LightComponent>()->lights()[0];
        startColor_ = light_->color();
    }

    void HealthStationComponent::onUnregister()
    {
        target_.reset();
    }
}
