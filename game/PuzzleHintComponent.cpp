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

#include "PuzzleHintComponent.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    PuzzleHintComponent::PuzzleHintComponent(const RenderQuadComponentPtr& rc)
    : PhasedComponent(phasePreRender),
      rc_(rc),
      tweenTime_(0.0f)
    {
    }

    PuzzleHintComponent::~PuzzleHintComponent()
    {
    }

    void PuzzleHintComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PuzzleHintComponent::preRender(float dt)
    {
        b2Vec2 p = rc_->pos();

        p.y = tweening_->getValue(tweenTime_);

        rc_->setPos(p);

        tweenTime_ += dt;
    }

    void PuzzleHintComponent::onRegister()
    {
        float y = rc_->pos().y;

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, y, y + settings.puzzleHint.displacement));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, y + settings.puzzleHint.displacement, y));

        tweening_ = tweening;
        tweenTime_ = 0.0f;
    }

    void PuzzleHintComponent::onUnregister()
    {
    }
}
