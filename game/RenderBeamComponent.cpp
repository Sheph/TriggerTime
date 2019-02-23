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

#include "RenderBeamComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderBeamComponent::RenderBeamComponent(const b2Vec2& pos, float angle,
        float width, const DrawablePtr& drawable, int zOrder)
    : RenderComponent(zOrder),
      t_(pos, b2Rot(angle)),
      width_(width),
      drawable_(drawable),
      prevAngle_(0.0f),
      length_(1.0f),
      cookie_(0),
      dirty_(false)
    {
    }

    RenderBeamComponent::~RenderBeamComponent()
    {
    }

    void RenderBeamComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderBeamComponent::update(float dt)
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

    void RenderBeamComponent::render(void* const* parts, size_t numParts)
    {
        renderer.setProgramDef(drawable_->image().texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        rop.addVertices(&vertices_[0], 6);

        rop.addTexCoord(0.0f, drawable_->image().texCoords()[5]);
        rop.addTexCoord(0.0f, drawable_->image().texCoords()[1]);
        rop.addTexCoord(length_ / (drawable_->image().aspect() * width_), drawable_->image().texCoords()[1]);
        rop.addTexCoord(length_ / (drawable_->image().aspect() * width_), drawable_->image().texCoords()[5]);

        rop.addColors(color());
    }

    void RenderBeamComponent::setLength(float length)
    {
        if (length_ != length) {
            length_ = length;
            updatePoints(length_);
            dirty_ = true;
        }
    }

    void RenderBeamComponent::setAngle(float value)
    {
        t_ = b2Transform(t_.p, b2Rot(value));
        updatePoints(length_);
        dirty_ = true;
    }

    void RenderBeamComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        updatePoints(length_);
        prevAABB_ = updateVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = false;
    }

    void RenderBeamComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    void RenderBeamComponent::updatePoints(float length)
    {
        points_[0] = b2Mul(t_, b2Vec2(0.0f, -width_ / 2.0f));
        points_[1] = b2Mul(t_, b2Vec2(0.0f, width_ / 2.0f));
        points_[2] = b2Mul(t_, b2Vec2(length, width_ / 2.0f));
        points_[3] = b2Mul(t_, b2Vec2(length, -width_ / 2.0f));
    }

    b2AABB RenderBeamComponent::updateVertices()
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
