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

#include "RenderProjComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderProjComponent::RenderProjComponent(const b2Vec2 points[4],
                                             const DrawablePtr& drawable,
                                             int zOrder)
    : RenderComponent(zOrder),
      drawable_(drawable),
      prevAngle_(0.0f),
      cookie_(0),
      fixedPos_(false),
      flip_(false),
      dirty_(false)
    {
        for (size_t i = 0; i < sizeof(points_)/sizeof(points_[0]); ++i) {
            points_[i] = points[i];
        }
    }

    RenderProjComponent::~RenderProjComponent()
    {
    }

    void RenderProjComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderProjComponent::update(float dt)
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

    void RenderProjComponent::render(void* const* parts, size_t numParts)
    {
        applyStencil(true);

        renderer.setProgramProjTex(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], 6);

        rop.addTexCoord4(drawable_->image().texCoords()[0] * width1_, drawable_->image().texCoords()[1] * width1_, 0.0f, width1_);
        rop.addTexCoord4(drawable_->image().texCoords()[2] * width1_, drawable_->image().texCoords()[3] * width1_, 0.0f, width1_);
        rop.addTexCoord4(drawable_->image().texCoords()[4] * width2_, drawable_->image().texCoords()[5] * width2_, 0.0f, width2_);

        rop.addTexCoord4(drawable_->image().texCoords()[6] * width1_, drawable_->image().texCoords()[7] * width1_, 0.0f, width1_);
        rop.addTexCoord4(drawable_->image().texCoords()[8] * width2_, drawable_->image().texCoords()[9] * width2_, 0.0f, width2_);
        rop.addTexCoord4(drawable_->image().texCoords()[10] * width2_, drawable_->image().texCoords()[11] * width2_, 0.0f, width2_);

        rop.addColor(color());
        rop.addColor(color());
        rop.addColor(color());

        rop.addColor(color());
        rop.addColor(color());
        rop.addColor(color());

        applyStencil(false);
    }

    void RenderProjComponent::setFixedPos(bool value)
    {
        if (fixedPos_ != value) {
            fixedPos_ = value;
            dirty_ = true;
        }
    }

    void RenderProjComponent::setFlip(bool value)
    {
        if (flip_ != value) {
            flip_ = value;
            dirty_ = true;
        }
    }

    void RenderProjComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = updateVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = false;
    }

    void RenderProjComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    b2AABB RenderProjComponent::updateVertices()
    {
        b2Transform xf = parent()->getSmoothTransform();
        b2Vec2 tmp[4];

        width1_ = (points_[0] - points_[1]).Length();
        width2_ = (points_[2] - points_[3]).Length();

        if (fixedPos_) {
            xf.q.SetIdentity();
        }

        vertices_.clear();

        if (flip_) {
            tmp[0] = b2Mul(xf, points_[1]);
            appendTriangleFan(vertices_, tmp[0].x, tmp[0].y);
            tmp[1] = b2Mul(xf, points_[0]);
            appendTriangleFan(vertices_, tmp[1].x, tmp[1].y);
            tmp[2] = b2Mul(xf, points_[3]);
            appendTriangleFan(vertices_, tmp[2].x, tmp[2].y);
            tmp[3] = b2Mul(xf, points_[2]);
            appendTriangleFan(vertices_, tmp[3].x, tmp[3].y);
        } else {
            tmp[0] = b2Mul(xf, points_[0]);
            appendTriangleFan(vertices_, tmp[0].x, tmp[0].y);
            tmp[1] = b2Mul(xf, points_[1]);
            appendTriangleFan(vertices_, tmp[1].x, tmp[1].y);
            tmp[2] = b2Mul(xf, points_[2]);
            appendTriangleFan(vertices_, tmp[2].x, tmp[2].y);
            tmp[3] = b2Mul(xf, points_[3]);
            appendTriangleFan(vertices_, tmp[3].x, tmp[3].y);
        }

        return computeAABB(&tmp[0], sizeof(tmp)/sizeof(tmp[0]));
    }
}
