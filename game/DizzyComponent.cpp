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

#include "DizzyComponent.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include "Utils.h"
#include "SceneObject.h"
#include <boost/make_shared.hpp>

namespace af
{
    DizzyComponent::DizzyComponent(const SceneObjectPtr& target, float duration, float wearoutDuration)
    : PhasedComponent(phasePreRender),
      target_(target),
      duration_(duration),
      wearoutDuration_(wearoutDuration),
      t_(0.0f),
      done_(false)
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.8f, EaseInOutQuad, -1.0f, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.8f, EaseInOutQuad, 1.0f, -1.0f));

        tweening_ = tweening;
        tweenTime_ = 0.4f;
    }

    DizzyComponent::~DizzyComponent()
    {
    }

    void DizzyComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void DizzyComponent::preRender(float dt)
    {
        float value = tweening_->getValue(tweenTime_);

        float angle, a = 0.0f;

        if (t_ > duration_) {
            if (!done_) {
                done_ = true;
                c_->setColor(Color(0.0f, 0.0f, 0.0f, 0.0f), EaseLinear, wearoutDuration_);
            }

            a = (t_ - duration_) / wearoutDuration_;
            if (a > 1.0f) {
                a = 1.0f;
            }

            angle = value * deg2rad(18.0f * (1.0f - a) + 0.0f * a);
        } else {
            angle = value * deg2rad(18.0f);
            if (target_->dead()) {
                t_ = duration_;
            }
        }

        parent()->setAngle(angle);

        if (a == 1.0f) {
            removeFromParent();
            return;
        }

        tweenTime_ += dt;
        t_ += dt;
    }

    void DizzyComponent::onRegister()
    {
        c_ = boost::make_shared<StainedGlassComponent>(-100);
        c_->setColor(Color(0.0f, 1.0f, 0.0f, 0.7f), EaseLinear, 0.0f);
        c_->setColor(Color(0.0f, 1.0f / 1.5f, 0.0f, 0.7f / 1.5f), EaseLinear, duration_);

        parent()->addComponent(c_);
    }

    void DizzyComponent::onUnregister()
    {
        target_.reset();
        c_->removeFromParent();
    }
}
