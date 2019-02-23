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

#include "QuadPulseComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SequentialTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    QuadPulseComponent::QuadPulseComponent(const RenderQuadComponentPtr& rc,
        const b2Vec2& pivot,
        float t1, Easing easing1, float factor1,
        float t2, Easing easing2, float factor2)
    : PhasedComponent(phasePreRender),
      rc_(rc),
      pivot_(pivot)
    {
        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(t1, easing1, factor2, factor1));
        tweening->addTweening(boost::make_shared<SingleTweening>(t2, easing2, factor1, factor2));

        tweening_= tweening;
        t_ = getRandom(0.0f, tweening_->duration());
        origHeight_ = rc_->height();
        origPos_ = rc_->pos();
    }

    QuadPulseComponent::~QuadPulseComponent()
    {
    }

    void QuadPulseComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void QuadPulseComponent::preRender(float dt)
    {
        if (!tweening_) {
            return;
        }

        float value = tweening_->getValue(t_);

        rc_->setHeight(origHeight_ * value);
        rc_->setPos(pivot_ + value * (origPos_ - pivot_));

        t_ += dt;
    }

    void QuadPulseComponent::onRegister()
    {
    }

    void QuadPulseComponent::onUnregister()
    {
    }
}
