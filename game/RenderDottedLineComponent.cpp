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

#include "RenderDottedLineComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderDottedLineComponent::RenderDottedLineComponent(const b2Vec2& pos, float angle,
        float width, const DrawablePtr& drawable, int zOrder)
    : RenderComponent(zOrder, true),
      xf_(pos, b2Rot(angle)),
      width_(width),
      drawable_(drawable),
      length_(0.0f),
      dotVelocity_(0.0f),
      dotDistance_(0.0f),
      t_(0.0f)
    {
    }

    RenderDottedLineComponent::~RenderDottedLineComponent()
    {
    }

    void RenderDottedLineComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderDottedLineComponent::update(float dt)
    {
        vertices_.clear();
        texCoords_.clear();

        t_ += dt;

        b2Transform finalXf = b2Mul(parent()->getSmoothTransform(), xf_);

        float dotHeight = width_ / drawable_->image().aspect();
        float segmentHeight = dotHeight + dotDistance_;
        float off = std::fmod(dotVelocity_ * t_, segmentHeight);

        for (float i = 0.0f; i < length_; i += segmentHeight) {
            float iEnd = i + segmentHeight;
            if (iEnd > length_) {
                iEnd = length_;
            }

            float x1 = i + off;
            float x2 = i + off + dotHeight;

            if (x1 < iEnd) {
                if (x2 > iEnd) {
                    x2 = iEnd;
                }
                addQuad(finalXf, b2Vec2(x1, 0.0f), x2 - x1, 0.0f, (x2 - x1) / dotHeight);
            }

            if ((off + dotHeight) > segmentHeight) {
                x1 = i;
                x2 = i + (off + dotHeight) - segmentHeight;
                if (x2 > length_) {
                    x2 = length_;
                }
                addQuad(finalXf, b2Vec2(x1, 0.0f), x2 - x1, 1.0f - ((x2 - x1) / dotHeight), 1.0f);
            }
        }
    }

    void RenderDottedLineComponent::render(void* const* parts, size_t numParts)
    {
        if (!vertices_.empty()) {
            renderer.setProgramDef(drawable_->image().texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&vertices_[0], vertices_.size() / 2);
            rop.addTexCoords(&texCoords_[0], texCoords_.size() / 2);
            rop.addColors(color());
        }
    }

    void RenderDottedLineComponent::onRegister()
    {
    }

    void RenderDottedLineComponent::onUnregister()
    {
    }

    void RenderDottedLineComponent::addQuad(const b2Transform& xf, const b2Vec2& pos, float len,
        float u1, float u2)
    {
        size_t bv = vertices_.size();

        b2Vec2 tmp = b2Mul(xf, pos + b2Vec2(0.0f, width_ / 2.0f));
        appendTriangleFan(vertices_, tmp.x, tmp.y, bv);

        tmp = b2Mul(xf, pos + b2Vec2(0.0f, -width_ / 2.0f));
        appendTriangleFan(vertices_, tmp.x, tmp.y, bv);

        tmp = b2Mul(xf, pos + b2Vec2(len, -width_ / 2.0f));
        appendTriangleFan(vertices_, tmp.x, tmp.y ,bv);

        tmp = b2Mul(xf, pos + b2Vec2(len, width_ / 2.0f));
        appendTriangleFan(vertices_, tmp.x, tmp.y ,bv);

        size_t bt = texCoords_.size();

        texCoords_.insert(texCoords_.end(),
                          drawable_->image().texCoords(),
                          drawable_->image().texCoords() + 12);

        float w = texCoords_[bt + 5] - texCoords_[bt + 1];
        u1 = texCoords_[bt + 1] + (w * u1);
        u2 = texCoords_[bt + 1] + (w * u2);

        texCoords_[bt + 1] = u1;
        texCoords_[bt + 3] = u1;
        texCoords_[bt + 7] = u1;

        texCoords_[bt + 5] = u2;
        texCoords_[bt + 9] = u2;
        texCoords_[bt + 11] = u2;
    }
}
