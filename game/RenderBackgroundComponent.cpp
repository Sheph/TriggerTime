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

#include "RenderBackgroundComponent.h"
#include "Scene.h"
#include "Renderer.h"

namespace af
{
    RenderBackgroundComponent::RenderBackgroundComponent(const DrawablePtr& drawable,
                                                         float imageWidth, float imageHeight,
                                                         int zOrder)
    : RenderComponent(zOrder, true),
      drawable_(drawable),
      imageWidth_(imageWidth),
      imageHeight_(imageHeight),
      offset_(0.0f, 0.0f),
      unbound_(false),
      unboundT_(0.0f)
    {
    }

    RenderBackgroundComponent::~RenderBackgroundComponent()
    {
    }

    void RenderBackgroundComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderBackgroundComponent::update(float dt)
    {
        unboundT_ += dt;
    }

    void RenderBackgroundComponent::render(void* const* parts, size_t numParts)
    {
        renderer.setProgramDef(drawable_->image().texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2AABB aabb = camera_->getAABB();

        rop.addVertex(aabb.lowerBound.x, aabb.lowerBound.y);
        rop.addVertex(aabb.upperBound.x, aabb.lowerBound.y);
        rop.addVertex(aabb.upperBound.x, aabb.upperBound.y);
        rop.addVertex(aabb.lowerBound.x, aabb.upperBound.y);

        aabb.lowerBound += offset_;
        aabb.upperBound += offset_;

        b2Vec2 center;

        if (unbound_) {
            center = b2Vec2(unboundT_, aabb.GetCenter().y);
        } else {
            center = aabb.GetCenter();
        }

        center.x *= parent()->linearVelocity().x;
        center.y *= parent()->linearVelocity().y;

        if (unbound_) {
            center.x += aabb.GetCenter().x;
        }

        b2Vec2 aabbSizeD2 = aabb.GetExtents();

        aabb.lowerBound = center - aabbSizeD2;
        aabb.upperBound = center + aabbSizeD2;

        rop.addTexCoord(aabb.lowerBound.x / imageWidth_, aabb.lowerBound.y / imageHeight_);
        rop.addTexCoord(aabb.upperBound.x / imageWidth_, aabb.lowerBound.y / imageHeight_);
        rop.addTexCoord(aabb.upperBound.x / imageWidth_, aabb.upperBound.y / imageHeight_);
        rop.addTexCoord(aabb.lowerBound.x / imageWidth_, aabb.upperBound.y / imageHeight_);

        rop.addColors(color());
    }

    void RenderBackgroundComponent::onRegister()
    {
        camera_ = scene()->camera()->findComponent<CameraComponent>();
    }

    void RenderBackgroundComponent::onUnregister()
    {
        camera_.reset();
    }
}
