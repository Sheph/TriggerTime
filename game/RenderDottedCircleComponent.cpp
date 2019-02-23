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

#include "RenderDottedCircleComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderDottedCircleComponent::RenderDottedCircleComponent(const b2Vec2& pos,
        float width, const DrawablePtr& drawable, int zOrder)
    : RenderComponent(zOrder, true),
      xf_(pos, b2Rot(0)),
      width_(width),
      drawable_(drawable),
      radius_(0.0f),
      dotVelocity_(0.0f),
      dotVelocityRadius_(0.0f),
      t_(0.0f)
    {
        setDotDistance(0.0f, radius_);
    }

    RenderDottedCircleComponent::~RenderDottedCircleComponent()
    {
    }

    void RenderDottedCircleComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderDottedCircleComponent::update(float dt)
    {
        vertices_.clear();
        texCoords_.clear();

        t_ += dt;

        if (radius_ <= 0.0f) {
            return;
        }

        float dotHeight = width_ / drawable_->image().aspect();

        b2Transform sxf = parent()->getSmoothTransform();
        b2Transform finalXf(sxf.p + xf_.p, b2Mul(sxf.q, xf_.q));

        for (int i = 0; i < numSectors_; ++i) {
            float a = 2.0f * b2_pi * i / numSectors_;

            a += t_ * (dotVelocity_ / dotVelocityRadius_);

            b2Transform xf = b2Mul(b2Transform(finalXf.p, b2Rot(0)),
                b2Transform(radius_ * b2Vec2(cosf(a), sinf(a)), b2Rot(a)));

            size_t bv = vertices_.size();

            b2Vec2 tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, -dotHeight / 2.0f));
            appendTriangleFan(vertices_, tmp.x, tmp.y, bv);

            tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, -dotHeight / 2.0f));
            appendTriangleFan(vertices_, tmp.x, tmp.y, bv);

            tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, dotHeight / 2.0f));
            appendTriangleFan(vertices_, tmp.x, tmp.y ,bv);

            tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, dotHeight / 2.0f));
            appendTriangleFan(vertices_, tmp.x, tmp.y ,bv);

            texCoords_.insert(texCoords_.end(),
                              drawable_->image().texCoords(),
                              drawable_->image().texCoords() + 12);
        }
    }

    void RenderDottedCircleComponent::render(void* const* parts, size_t numParts)
    {
        if (!vertices_.empty()) {
            renderer.setProgramDef(drawable_->image().texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&vertices_[0], vertices_.size() / 2);
            rop.addTexCoords(&texCoords_[0], texCoords_.size() / 2);
            rop.addColors(color());
        }
    }

    void RenderDottedCircleComponent::setDotDistance(float value, float forRadius)
    {
        float dotHeight = width_ / drawable_->image().aspect();

        float segmentHeight = dotHeight + value;

        numSectors_ = std::floor(b2_pi / std::atan2(segmentHeight, 2.0f * forRadius));
    }

    void RenderDottedCircleComponent::onRegister()
    {
    }

    void RenderDottedCircleComponent::onUnregister()
    {
    }
}
