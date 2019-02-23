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

#include "StainedGlassComponent.h"
#include "Renderer.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    StainedGlassComponent::StainedGlassComponent(int zOrder)
    : UIComponent(zOrder),
      color_(0.0f, 0.0f, 0.0f, 0.0f),
      tweenTime_(0.0f)
    {
    }

    StainedGlassComponent::~StainedGlassComponent()
    {
    }

    void StainedGlassComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void StainedGlassComponent::update(float dt)
    {
        if (!tweening_) {
            return;
        }

        float v = tweening_->getValue(tweenTime_);

        color_.rgba[0] = v * endColor_.rgba[0] + (1.0f - v) * startColor_.rgba[0];
        color_.rgba[1] = v * endColor_.rgba[1] + (1.0f - v) * startColor_.rgba[1];
        color_.rgba[2] = v * endColor_.rgba[2] + (1.0f - v) * startColor_.rgba[2];
        color_.rgba[3] = v * endColor_.rgba[3] + (1.0f - v) * startColor_.rgba[3];

        tweenTime_ += dt;
        if (tweening_->finished(tweenTime_)) {
            tweening_.reset();
            color_ = endColor_;
        }
    }

    void StainedGlassComponent::render()
    {
        renderer.setProgramColor();

        RenderTriangleFan rop = renderer.renderTriangleFan();

        rop.addVertex(0.0f, 0.0f);
        rop.addVertex(scene()->gameWidth(), 0.0f);
        rop.addVertex(scene()->gameWidth(), scene()->gameHeight());
        rop.addVertex(0.0f, scene()->gameHeight());

        rop.addColors(color_);
    }

    void StainedGlassComponent::setColor(const Color& value, Easing easing, float timeout)
    {
        tweening_.reset();

        if (timeout <= 0.0f) {
            color_ = value;
        } else {
            startColor_ = color_;
            endColor_ = value;

            tweening_ = boost::make_shared<SingleTweening>(timeout, easing);
            tweenTime_ = 0.0f;
        }
    }

    void StainedGlassComponent::onRegister()
    {
    }

    void StainedGlassComponent::onUnregister()
    {
    }
}
