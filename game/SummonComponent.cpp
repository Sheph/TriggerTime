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

#include "SummonComponent.h"
#include "Scene.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "PointLight.h"
#include <boost/make_shared.hpp>

namespace af
{
    SummonComponent::SummonComponent(const ParticleEffectComponentPtr& pec,
        const Color& lightColor, float lightDistance,
        float beforeTime, float afterTime, bool haveSound)
    : PhasedComponent(phaseThink),
      pec_(pec),
      lightColor_(lightColor),
      lightDistance_(lightDistance),
      tweenTime_(0.0f),
      snd_(haveSound ? audio.createSound("summon1.ogg") : AudioSourcePtr())
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(beforeTime, EaseInQuad, 0.0f, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(afterTime, EaseOutQuad, 1.0f, 0.0f));

        tweening_ = tweening;
    }

    SummonComponent::~SummonComponent()
    {
    }

    void SummonComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SummonComponent::update(float dt)
    {
        if (tweening_->finished(tweenTime_)) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        tweenTime_ += dt;

        float val = tweening_->getValue(tweenTime_);

        Color c = light_->color();
        c.rgba[3] = val;
        light_->setColor(c);
    }

    void SummonComponent::onRegister()
    {
        Color color = lightColor_;
        color.rgba[3] = 0.0f;

        PointLightPtr light = boost::make_shared<PointLight>();
        light->setColor(color);
        light->setDiffuse(false);
        light->setXray(true);
        light->setDistance(lightDistance_);
        light->setIntensity(1.5f);
        light->setNumRays(8);
        light_ = light;

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);

        if (snd_) {
            snd_->play();
        }
    }

    void SummonComponent::onUnregister()
    {
        lightC_->removeFromParent();
    }
}
