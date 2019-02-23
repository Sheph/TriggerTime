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

#include "RenderTrailComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderTrailComponent::RenderTrailComponent(const b2Vec2& pos, float angle,
        const DrawablePtr& drawable, int zOrder)
    : RenderComponent(zOrder),
      t_(pos, b2Rot(angle)),
      width1_(1.0f),
      width2_(1.0f),
      drawable_(drawable),
      color2_(color()),
      prevAngle_(0.0f),
      length_(1.0f),
      cookie_(0),
      dirty_(false),
      texWidth1_(1.0f),
      texWidth2_(1.0f)
    {
    }

    RenderTrailComponent::~RenderTrailComponent()
    {
    }

    void RenderTrailComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderTrailComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_) &&
            !dirty_) {
            return;
        }

        b2AABB aabb = updateVertices();

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = aabb;
        dirty_ = false;
    }

    void RenderTrailComponent::render(void* const* parts, size_t numParts)
    {
        if (length_ == 0.0f) {
            return;
        }

        float width = b2Max(texWidth1_, texWidth2_);
        float len = length_ / (drawable_->image().aspect() * width);

        renderer.setProgramProjTex(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], 6);

        rop.addTexCoord4(0.0f, drawable_->image().texCoords()[5] * texWidth1_, 0.0f, texWidth1_);
        rop.addTexCoord4(0.0f, drawable_->image().texCoords()[1] * texWidth1_, 0.0f, texWidth1_);
        rop.addTexCoord4(len * texWidth2_, drawable_->image().texCoords()[1] * texWidth2_, 0.0f, texWidth2_);

        rop.addTexCoord4(0.0f, drawable_->image().texCoords()[5] * texWidth1_, 0.0f, texWidth1_);
        rop.addTexCoord4(len * texWidth2_, drawable_->image().texCoords()[1] * texWidth2_, 0.0f, texWidth2_);
        rop.addTexCoord4(len * texWidth2_, drawable_->image().texCoords()[5] * texWidth2_, 0.0f, texWidth2_);

        rop.addColor(color());
        rop.addColor(color());
        rop.addColor(color2_);

        rop.addColor(color());
        rop.addColor(color2_);
        rop.addColor(color2_);
    }

    void RenderTrailComponent::setLength(float length)
    {
        if (length_ != length) {
            length_ = length;
            updatePoints();
            dirty_ = true;
        }
    }

    void RenderTrailComponent::setAngle(float value)
    {
        t_ = b2Transform(t_.p, b2Rot(value));
        updatePoints();
        dirty_ = true;
    }

    void RenderTrailComponent::setWidth1(float value)
    {
        if (width1_ != value) {
            width1_ = value;
            texWidth1_ = value;
            updatePoints();
            dirty_ = true;
        }
    }

    void RenderTrailComponent::setWidth1NoTex(float value)
    {
        if (width1_ != value) {
            width1_ = value;
            updatePoints();
            dirty_ = true;
        }
    }

    void RenderTrailComponent::setWidth2(float value)
    {
        if (width2_ != value) {
            width2_ = value;
            texWidth2_ = value;
            updatePoints();
            dirty_ = true;
        }
    }

    void RenderTrailComponent::setWidth2NoTex(float value)
    {
        if (width2_ != value) {
            width2_ = value;
            updatePoints();
            dirty_ = true;
        }
    }

    void RenderTrailComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        updatePoints();
        prevAABB_ = updateVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = false;
    }

    void RenderTrailComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    void RenderTrailComponent::updatePoints()
    {
        points_[0] = b2Mul(t_, b2Vec2(0.0f, -width1_ / 2.0f));
        points_[1] = b2Mul(t_, b2Vec2(0.0f, width1_ / 2.0f));
        points_[2] = b2Mul(t_, b2Vec2(length_, width2_ / 2.0f));
        points_[3] = b2Mul(t_, b2Vec2(length_, -width2_ / 2.0f));
    }

    b2AABB RenderTrailComponent::updateVertices()
    {
        const b2Transform& xf = parent()->getSmoothTransform();
        b2Vec2 tmp[4];

        vertices_.clear();

        for (size_t i = 0; i < sizeof(points_)/sizeof(points_[0]); ++i) {
            tmp[i] = b2Mul(xf, points_[i]);
            appendTriangleFan(vertices_, tmp[i].x, tmp[i].y);
        }

        return computeAABB(&tmp[0], sizeof(tmp)/sizeof(tmp[0]));
    }
}
