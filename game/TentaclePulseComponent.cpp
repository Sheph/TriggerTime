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

#include "TentaclePulseComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TentaclePulseComponent::TentaclePulseComponent(int numPoints, float timeFactor,
        float phaseFactor, float amplitudeFactor1, float amplitudeFactor2)
    : PhasedComponent(phasePreRender),
      numPoints_(numPoints),
      timeFactor_(timeFactor),
      phaseFactor_(phaseFactor),
      t_(0.0f)
    {
        amplitudeFactor_[0] = amplitudeFactor1;
        amplitudeFactor_[1] = amplitudeFactor2;
    }

    TentaclePulseComponent::~TentaclePulseComponent()
    {
    }

    void TentaclePulseComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TentaclePulseComponent::preRender(float dt)
    {
        t_ += dt * timeFactor_;
        updateTimeline();
    }

    void TentaclePulseComponent::onRegister()
    {
        tentacle_ = parent()->findComponent<RenderTentacleComponent>();

        width1_[0] = tentacle_->width1At(0);
        width1_[1] = tentacle_->width1At(tentacle_->timeline1Size() - 1);

        width2_[0] = tentacle_->width2At(0);
        width2_[1] = tentacle_->width2At(tentacle_->timeline2Size() - 1);

        tentacle_->resetTimeline1(numPoints_);
        tentacle_->resetTimeline2(numPoints_);

        t_ = getRandom(0.0f, 1.0f);

        updateTimeline();
    }

    void TentaclePulseComponent::onUnregister()
    {
    }

    void TentaclePulseComponent::updateTimeline()
    {
        for (int i = 0; i < tentacle_->timeline1Size(); ++i) {
            float t = 1.0f * i / (tentacle_->timeline1Size() - 1);
            float w1 = width1_[0] * (1.0f - t) + width1_[1] * t;
            float w2 = width2_[0] * (1.0f - t) + width2_[1] * t;
            float val = sinf((t + t_) * b2_pi * phaseFactor_);
            tentacle_->set1At(i, t, w1 + val * amplitudeFactor_[0]);
            tentacle_->set2At(i, t, w2 + val * amplitudeFactor_[1]);
        }
    }
}
