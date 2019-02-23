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

#include "TentacleInflateComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TentacleInflateComponent::TentacleInflateComponent(int numPoints, float radius, float velocity,
        const RenderQuadComponentPtr& rc)
    : PhasedComponent(phasePreRender),
      numPoints_(numPoints),
      radius_(radius),
      velocity_(velocity),
      rc_(rc),
      done_(false)
    {
    }

    TentacleInflateComponent::~TentacleInflateComponent()
    {
    }

    void TentacleInflateComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TentacleInflateComponent::preRender(float dt)
    {
        float dx = (dt * velocity_) / tentacle_->path().length();

        for (int i = 0; i < tentacle_->timeline1Size(); ++i) {
            tentacle_->set1At(i, tentacle_->time1At(i) - dx, tentacle_->width1At(i));
            tentacle_->set2At(i, tentacle_->time2At(i) - dx, tentacle_->width2At(i));
        }

        int i = 1;

        float scale = tentacle_->calc1At(i, 0.0f) / width_;

        if (i >= (tentacle_->timeline1Size() / 2)) {
            done_ = true;
        }

        rc_->setHeight(rcOrigHeight_ * scale);

        float w = rcOrigHeight_ * (scale - 1.0f) * rc_->drawable()->image().aspect();

        rc_->setPos(rcOrigPos_ + b2Vec2(w / 2.0f, 0.0f));

        if (i >= tentacle_->timeline1Size()) {
            removeFromParent();
        }
    }

    void TentacleInflateComponent::onRegister()
    {
        tentacle_ = parent()->findComponent<RenderTentacleComponent>();

        width_ = tentacle_->width1At(0);

        tentacle_->resetTimeline1(numPoints_);
        tentacle_->resetTimeline2(numPoints_);

        for (int i = 0; i < tentacle_->timeline1Size(); ++i) {
            float a = 1.0f * i / (tentacle_->timeline1Size() - 1);
            float t = 1.0f + ((radius_ * 2.0f) / tentacle_->path().length()) * a;
            float v = sqrtf(radius_ * radius_ - ((2.0f * a - 1.0f) * radius_) * ((2.0f * a - 1.0f) * radius_));
            if (v < width_) {
                v = width_;
            }

            tentacle_->set1At(i, t, v);
            tentacle_->set2At(i, t, v);
        }

        tentacle_->set1At(0, 0.0f, width_);
        tentacle_->set2At(0, 0.0f, width_);

        tentacle_->set1At(1, tentacle_->time1At(1), width_);
        tentacle_->set2At(1, tentacle_->time2At(1), width_);

        rcOrigHeight_ = rc_->height();
        rcOrigPos_ = rc_->pos();
    }

    void TentacleInflateComponent::onUnregister()
    {
    }
}
