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

#include "GoalComponent.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "SceneObject.h"
#include <boost/make_shared.hpp>

namespace af
{
    GoalComponent::GoalComponent(const RenderDottedCircleComponentPtr& dcc)
    : PhasedComponent(phasePreRender),
      dcc_(dcc),
      tweenTime_(0.0f)
    {
    }

    GoalComponent::~GoalComponent()
    {
    }

    void GoalComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GoalComponent::preRender(float dt)
    {
        dcc_->setRadius(tweening_->getValue(tweenTime_));
        tweenTime_ += dt;
    }

    void GoalComponent::onRegister()
    {
        float radius = dcc_->radius();

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, radius * 1.0f, radius * 1.25f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, radius * 1.25f, radius * 1.0f));

        tweening_ = tweening;
        tweenTime_ = 0.0f;

        parent()->addComponent(dcc_);
    }

    void GoalComponent::onUnregister()
    {
        dcc_->removeFromParent();
    }
}
