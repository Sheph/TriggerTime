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

#include "FloatComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SequentialTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    FloatComponent::FloatComponent(float t, float distance, float force)
    : TargetableComponent(phaseThink),
      force_(force)
    {
        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(t / 2.0f, EaseInQuad, distance / 2.0f, -distance / 2.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(t / 2.0f, EaseOutQuad, -distance / 2.0f, distance / 2.0f));

        tweening_= tweening;
        tweenTime_ = getRandom(0.0f, tweening_->duration());
    }

    FloatComponent::~FloatComponent()
    {
    }

    void FloatComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void FloatComponent::update(float dt)
    {
        float value = 0.0f;

        if (tweening_) {
            value = tweening_->getValue(tweenTime_);
        }

        b2Vec2 tmp = pos_ + value * dir_ - target()->pos();
        tmp.Normalize();

        target()->applyForceToCenter(force_ * tmp, true);

        tweenTime_ += dt;
    }

    void FloatComponent::stop()
    {
        tweening_.reset();
    }

    void FloatComponent::onRegister()
    {
        pos_ = target()->pos();
        dir_ = target()->getDirection(1.0f);
    }

    void FloatComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
