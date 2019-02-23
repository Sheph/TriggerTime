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

#include "RenderQuadComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderQuadComponent::RenderQuadComponent(const b2Vec2& pos, float angle,
                                             float width, float height,
                                             const DrawablePtr& drawable,
                                             int zOrder)
    : RenderComponent(zOrder),
      pos_(pos),
      angle_(angle),
      width_(width),
      height_(height),
      aspect_(width / height),
      drawable_(drawable),
      prevAngle_(0.0f),
      cookie_(0),
      mask_(false),
      fixedPos_(false),
      flip_(false),
      dirty_(false)
    {
        updatePoints();
    }

    RenderQuadComponent::~RenderQuadComponent()
    {
    }

    void RenderQuadComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderQuadComponent::update(float dt)
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

    void RenderQuadComponent::render(void* const* parts, size_t numParts)
    {
        applyStencil(true);

        if (mask_) {
            renderer.setProgramMask(drawable_->image().texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&vertices_[0], 6);
            rop.addTexCoords(drawable_->image().texCoords(), 6);
        } else if (flashColor().rgba[3] != 0.0f) {
            renderer.setProgramFlash(drawable_->image().texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&vertices_[0], 6);
            rop.addTexCoords(drawable_->image().texCoords(), 6);
            rop.addColors(color());
            rop.addColors2(flashColor());
        } else {
            renderer.setProgramDef(drawable_->image().texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&vertices_[0], 6);
            rop.addTexCoords(drawable_->image().texCoords(), 6);
            rop.addColors(color());
        }

        applyStencil(false);
    }

    void RenderQuadComponent::setPos(const b2Vec2& value)
    {
        if (pos_ != value) {
            pos_ = value;
            dirty_ = true;
            updatePoints();
        }
    }

    void RenderQuadComponent::setAngle(float value)
    {
        if (angle_ != value) {
            angle_ = value;
            dirty_ = true;
            updatePoints();
        }
    }

    void RenderQuadComponent::setSizes(float w, float h)
    {
        if ((width_ != w) || (height_ != h)) {
            width_ = w;
            height_ = h;
            dirty_ = true;
            updatePoints();
        }
    }

    void RenderQuadComponent::setWidth(float value, bool keepAspect)
    {
        if (keepAspect) {
            setSizes(value, value / aspect_);
        } else {
            setSizes(value, height());
        }
    }

    void RenderQuadComponent::setHeight(float value, bool keepAspect)
    {
        if (keepAspect) {
            setSizes(value * aspect_, value);
        } else {
            setSizes(width(), value);
        }
    }

    void RenderQuadComponent::setFixedPos(bool value)
    {
        if (fixedPos_ != value) {
            fixedPos_ = value;
            dirty_ = true;
        }
    }

    void RenderQuadComponent::setFlip(bool value)
    {
        if (flip_ != value) {
            flip_ = value;
            dirty_ = true;
            updatePoints();
        }
    }

    void RenderQuadComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = updateVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = false;
    }

    void RenderQuadComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    void RenderQuadComponent::updatePoints()
    {
        b2Transform xf(pos_, b2Rot(angle_));

        if (flip_) {
            points_[0] = b2Mul(xf, b2Vec2(width_ / 2.0f, -height_ / 2.0f));
            points_[1] = b2Mul(xf, b2Vec2(-width_ / 2.0f, -height_ / 2.0f));
            points_[2] = b2Mul(xf, b2Vec2(-width_ / 2.0f, height_ / 2.0f));
            points_[3] = b2Mul(xf, b2Vec2(width_ / 2.0f, height_ / 2.0f));
        } else {
            points_[0] = b2Mul(xf, b2Vec2(-width_ / 2.0f, -height_ / 2.0f));
            points_[1] = b2Mul(xf, b2Vec2(width_ / 2.0f, -height_ / 2.0f));
            points_[2] = b2Mul(xf, b2Vec2(width_ / 2.0f, height_ / 2.0f));
            points_[3] = b2Mul(xf, b2Vec2(-width_ / 2.0f, height_ / 2.0f));
        }
    }

    b2AABB RenderQuadComponent::updateVertices()
    {
        b2Transform xf = parent()->getSmoothTransform();
        b2Vec2 tmp[4];

        if (fixedPos_) {
            xf.q.SetIdentity();
        }

        vertices_.clear();

        for (size_t i = 0; i < sizeof(points_)/sizeof(points_[0]); ++i) {
            tmp[i] = b2Mul(xf, points_[i]);
            appendTriangleFan(vertices_, tmp[i].x, tmp[i].y);
        }

        return computeAABB(&tmp[0], sizeof(tmp)/sizeof(tmp[0]));
    }
}
