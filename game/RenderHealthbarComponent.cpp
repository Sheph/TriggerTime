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

#include "RenderHealthbarComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"
#include "AssetManager.h"

namespace af
{
    RenderHealthbarComponent::RenderHealthbarComponent(const b2Vec2& pos,
        float angle, float width, float height, int zOrder)
    : RenderComponent(zOrder),
      barPos_(pos),
      barAngle_(angle),
      width_(width),
      height_(height),
      bg_(assetManager.getImage("healthbar_bg.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      end_(assetManager.getImage("healthbar_end.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      life_(assetManager.getImage("healthbar_life.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      prevCameraAngle_(0.0f),
      prevLifePercent_(0.0f),
      cookie_(0)
    {
    }

    RenderHealthbarComponent::~RenderHealthbarComponent()
    {
    }

    void RenderHealthbarComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderHealthbarComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (scene()->camera()->angle() == prevCameraAngle_)) {
            if (realTarget()->lifePercent() != prevLifePercent_) {
                updateLifeVertices();
                prevLifePercent_ = realTarget()->lifePercent();
            }

            return;
        }

        b2AABB aabb = updateVertices();

        updateLifeVertices();

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevCameraAngle_ = scene()->camera()->angle();
        prevLifePercent_ = realTarget()->lifePercent();
        prevAABB_ = aabb;
    }

    void RenderHealthbarComponent::render(void* const* parts, size_t numParts)
    {
        {
            renderer.setProgramDef(bg_.texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&bgVertices_[0], 6);
            rop.addTexCoords(bg_.texCoords(), 6);
            rop.addColors(color());
        }
        {
            renderer.setProgramDef(end_.texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&endVertices_[0], endVertices_.size() / 2);
            rop.addTexCoords(&endTexCoords_[0], endTexCoords_.size() / 2);
            rop.addColors(color());
        }
        {
            renderer.setProgramDef(life_.texture());

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&lifeVertices_[0], 6);
            rop.addTexCoords(life_.texCoords(), 6);
            rop.addColors(color());
        }
    }

    void RenderHealthbarComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevCameraAngle_ = scene()->camera()->angle();
        prevAABB_ = updateVertices();
        prevLifePercent_ = realTarget()->lifePercent();
        updateLifeVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
    }

    void RenderHealthbarComponent::onUnregister()
    {
        target_.reset();
        manager()->removeAABB(cookie_);
    }

    b2AABB RenderHealthbarComponent::updateVertices()
    {
        const b2Transform& sxf = parent()->getSmoothTransform();

        b2Transform xf(sxf.p + barPos_, b2Rot(barAngle_ - scene()->camera()->angle()));

        bgVertices_.clear();
        endVertices_.clear();
        endTexCoords_.clear();
        lifeVertices_.clear();

        b2Vec2 v[4];

        /*
         * bg.
         */
        b2Vec2 tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, -height_ / 2.0f));
        appendTriangleFan(bgVertices_, tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, -height_ / 2.0f));
        appendTriangleFan(bgVertices_, tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, height_ / 2.0f));
        appendTriangleFan(bgVertices_, tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, height_ / 2.0f));
        appendTriangleFan(bgVertices_, tmp.x, tmp.y);

        /*
         * right end.
         */
        tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, -height_ / 2.0f));
        appendTriangleFan(endVertices_, tmp.x, tmp.y);
        v[0] = b2Mul(xf, b2Vec2((width_ / 2.0f) + end_.aspect() * height_, -height_ / 2.0f));
        appendTriangleFan(endVertices_, v[0].x, v[0].y);
        v[1] = b2Mul(xf, b2Vec2((width_ / 2.0f) + end_.aspect() * height_, height_ / 2.0f));
        appendTriangleFan(endVertices_, v[1].x, v[1].y);
        tmp = b2Mul(xf, b2Vec2(width_ / 2.0f, height_ / 2.0f));
        appendTriangleFan(endVertices_, tmp.x, tmp.y);

        endTexCoords_.insert(endTexCoords_.end(), end_.texCoords(),
            end_.texCoords() + 12);

        /*
         * left end.
         */
        size_t bv = endVertices_.size();

        tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, -height_ / 2.0f));
        appendTriangleFan(endVertices_, tmp.x, tmp.y, bv);
        v[2] = b2Mul(xf, b2Vec2((-width_ / 2.0f) - end_.aspect() * height_, -height_ / 2.0f));
        appendTriangleFan(endVertices_, v[2].x, v[2].y, bv);
        v[3] = b2Mul(xf, b2Vec2((-width_ / 2.0f) - end_.aspect() * height_, height_ / 2.0f));
        appendTriangleFan(endVertices_, v[3].x, v[3].y, bv);
        tmp = b2Mul(xf, b2Vec2(-width_ / 2.0f, height_ / 2.0f));
        appendTriangleFan(endVertices_, tmp.x, tmp.y, bv);

        endTexCoords_.insert(endTexCoords_.end(), end_.texCoords(),
            end_.texCoords() + 12);

        /*
         * life.
         */

        float extra = (height_ * end_.aspect()) / 4.2f;

        tmp = b2Mul(xf, b2Vec2((-width_ / 2.0f) - extra, -height_ / 2.0f));
        appendTriangleFan(lifeVertices_, tmp.x, tmp.y);
        appendTriangleFan(lifeVertices_, 0.0f, 0.0f);
        appendTriangleFan(lifeVertices_, 0.0f, 0.0f);
        tmp = b2Mul(xf, b2Vec2((-width_ / 2.0f) - extra, height_ / 2.0f));
        appendTriangleFan(lifeVertices_, tmp.x, tmp.y);

        return computeAABB(&v[0], sizeof(v)/sizeof(v[0]));
    }

    void RenderHealthbarComponent::updateLifeVertices()
    {
        const b2Transform& sxf = parent()->getSmoothTransform();

        b2Transform xf(sxf.p + barPos_, b2Rot(barAngle_ - scene()->camera()->angle()));

        float extra = (height_ * end_.aspect()) / 4.2f;

        b2Vec2 tmp = b2Mul(xf, b2Vec2((-width_ / 2.0f) - extra + (width_ + extra * 2) * realTarget()->lifePercent(), -height_ / 2.0f));
        lifeVertices_[2] = tmp.x;
        lifeVertices_[3] = tmp.y;
        tmp = b2Mul(xf, b2Vec2((-width_ / 2.0f) - extra + (width_ + extra * 2) * realTarget()->lifePercent(), height_ / 2.0f));
        lifeVertices_[4] = lifeVertices_[8] = tmp.x;
        lifeVertices_[5] = lifeVertices_[9] = tmp.y;
    }
}
