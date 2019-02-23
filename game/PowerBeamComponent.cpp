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

#include "PowerBeamComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    PowerBeamComponent::PowerBeamComponent(float radius, float length)
    : PhasedComponent(phasePreRender),
      radius_(radius),
      length_(length),
      tweenTime_(0.0f)
    {
    }

    PowerBeamComponent::~PowerBeamComponent()
    {
    }

    void PowerBeamComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PowerBeamComponent::preRender(float dt)
    {
        float value = tweening_->getValue(tweenTime_);

        lineLight_->setDistance(value * radius_ * (3.0f / 4.0f));

        if (tweening_->finished(tweenTime_)) {
            parent()->removeFromParent();
            return;
        }

        tweenTime_ += dt;
    }

    void PowerBeamComponent::onRegister()
    {
        beamAnimation_ = sceneObjectFactory.createLightningAnimation();

        beamAnimation_->startAnimation(AnimationDefault);

        beam_ = boost::make_shared<RenderBeamComponent>(b2Vec2_zero,
            0.0f, radius_ * 2.0f,
            beamAnimation_->drawable(), zOrderEffects - 1);

        beam_->setLength(length_);

        lineLight_ = boost::make_shared<LineLight>();
        lineLight_->setDiffuse(false);
        lineLight_->setColor(Color(0.0f, 0.33f, 1.0f, 1.0f));
        lineLight_->setXray(true);
        lineLight_->setDistance(0.0f);
        lineLight_->setBothWays(true);
        lineLight_->setAngle(b2_pi / 2.0f);
        lineLight_->setPos(b2Vec2(length_ / 2, 0.0f));
        lineLight_->setLength(length_ / 2);

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(lineLight_);

        parent()->addComponent(beam_);
        parent()->addComponent(beamAnimation_);
        parent()->addComponent(lightC_);

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseOutQuad, 0.0f, 4.0f / 3.0f));

        SequentialTweeningPtr tweening2 = boost::make_shared<SequentialTweening>(true);

        tweening2->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (4.0f / 3.0f), (3.0f / 4.0f)));
        tweening2->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));

        tweening->addTweening(tweening2);

        tweening_ = tweening;
        tweenTime_ = 0.0f;
    }

    void PowerBeamComponent::onUnregister()
    {
    }

    void PowerBeamComponent::finish()
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        float t = std::fmod(tweenTime_, 0.6f);

        if (t >= 0.3f) {
            tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (4.0f / 3.0f), (3.0f / 4.0f)));
            tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));
            tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInQuad, (4.0f / 3.0f), 0.0f));
            tweenTime_ = t - 0.3f;
        } else {
            tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));
            tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInQuad, (4.0f / 3.0f), 0.0f));
            tweenTime_ = t;
        }

        tweening_ = tweening;
    }
}
