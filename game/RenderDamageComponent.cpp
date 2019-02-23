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

#include "RenderDamageComponent.h"
#include "CameraComponent.h"
#include "SingleTweening.h"
#include "Scene.h"
#include "Renderer.h"
#include <boost/make_shared.hpp>

namespace af
{
    RenderDamageComponent::RenderDamageComponent(int zOrder)
    : RenderComponent(zOrder, true),
      tweenTime_(0.0f)
    {
    }

    RenderDamageComponent::~RenderDamageComponent()
    {
    }

    void RenderDamageComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderDamageComponent::update(float dt)
    {
        if (!tweening_) {
            return;
        }

        tweenTime_ += dt;

        if (tweening_->finished(tweenTime_)) {
            tweening_.reset();
        }
    }

    void RenderDamageComponent::render(void* const* parts, size_t numParts)
    {
        if (!tweening_) {
            return;
        }

        float alpha = tweening_->getValue(tweenTime_);

        CameraComponentPtr camera =
            scene()->camera()->findComponent<CameraComponent>();

        renderer.setProgramDamage();

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2AABB aabb = camera->getAABB();

        rop.addVertex(aabb.lowerBound.x, aabb.lowerBound.y);
        rop.addVertex(aabb.upperBound.x, aabb.lowerBound.y);
        rop.addVertex(aabb.upperBound.x, aabb.upperBound.y);
        rop.addVertex(aabb.lowerBound.x, aabb.upperBound.y);

        rop.addTexCoord(0.0f, 0.0f);
        rop.addTexCoord(1.0f, 0.0f);
        rop.addTexCoord(1.0f, 1.0f);
        rop.addTexCoord(0.0f, 1.0f);

        rop.addColors(Color(1.0f, 0.0f, 0.0f, alpha));
    }

    void RenderDamageComponent::onRegister()
    {
    }

    void RenderDamageComponent::onUnregister()
    {
    }

    void RenderDamageComponent::setDamaged(float value)
    {
        if (tweening_) {
            value += tweening_->getValue(tweenTime_);
        }

        if (value > 1.0f) {
            value = 1.0f;
        }

        tweening_ = boost::make_shared<SingleTweening>(0.5f, EaseLinear,
            value, 0.0f);
        tweenTime_ = 0.0f;
    }
}
