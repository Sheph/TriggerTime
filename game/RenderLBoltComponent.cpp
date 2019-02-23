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

#include "RenderLBoltComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    RenderLBoltComponent::RenderLBoltComponent(const b2Vec2& pos, float angle,
        const Image& image, int zOrder)
    : RenderComponent(zOrder),
      t_(pos, b2Rot(angle)),
      image_(image),
      length_(1.0f),
      subLength_(1.0f),
      width_(1.0f),
      maxOffset_(1.0f),
      segmentLength_(0.3f),
      prevAngle_(0.0f),
      cookie_(0),
      dirty_(false),
      needsUpdatePoints_(false),
      needsUpdateVertices_(false)
    {
    }

    RenderLBoltComponent::~RenderLBoltComponent()
    {
    }

    void RenderLBoltComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderLBoltComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_) &&
            !dirty_) {
            return;
        }

        if (dirty_) {
            needsUpdatePoints_ = true;
        }
        needsUpdateVertices_ = true;

        b2AABB aabb = calculateAABB();

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = aabb;
        dirty_ = false;
    }

    void RenderLBoltComponent::render(void* const* parts, size_t numParts)
    {
        if (length_ == 0.0f) {
            return;
        }

        if (needsUpdatePoints_) {
            needsUpdatePoints_ = false;
            updatePoints();
        }

        if (needsUpdateVertices_) {
            needsUpdateVertices_ = false;
            updateVertices();
        }

        renderer.setProgramDef(image_.texture());

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], vertices_.size() / 2);
        rop.addTexCoords(&texCoords_[0], texCoords_.size() / 2);
        rop.addColors(color());
    }

    void RenderLBoltComponent::setLength(float value)
    {
        if (length_ != value) {
            length_ = value;
            dirty_ = true;
        }
    }

    void RenderLBoltComponent::setSubLength(float value)
    {
        if (subLength_ != value) {
            subLength_ = value;
            dirty_ = true;
        }
    }

    void RenderLBoltComponent::setWidth(float value)
    {
        if (width_ != value) {
            width_ = value;
            dirty_ = true;
        }
    }

    void RenderLBoltComponent::setMaxOffset(float value)
    {
        if (maxOffset_ != value) {
            maxOffset_ = value;
            dirty_ = true;
        }
    }

    void RenderLBoltComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = calculateAABB();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = true;
    }

    void RenderLBoltComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    b2AABB RenderLBoltComponent::calculateAABB()
    {
        b2Transform xf = b2Mul(parent()->getSmoothTransform(), t_);

        b2Vec2 tmp[4];

        tmp[0] = b2Mul(xf, b2Vec2(-2.0f * width_, 0.0f));
        tmp[1] = b2Mul(xf, b2Vec2(length_ + 2.0f * width_, 0.0f));
        tmp[2] = b2Mul(xf, b2Vec2(length_ / 2.0f, (maxOffset_ + width_) * 2.0f));
        tmp[3] = b2Mul(xf, b2Vec2(length_ / 2.0f, -(maxOffset_ + width_) * 2.0f));

        return computeAABB(&tmp[0], sizeof(tmp)/sizeof(tmp[0]));
    }

    void RenderLBoltComponent::updatePoints()
    {
        static Points tmp1, tmp2;

        tmp1.clear();

        float curOffset = maxOffset_;

        float curLen;

        if (length_ < subLength_) {
            curLen = length_;
        } else {
            curLen = length_ / std::floor(length_ / subLength_);
        }

        float ln = 0.0f;

        while (ln <= length_) {
            if ((ln == 0.0f) || (ln > length_ - curLen)) {
                tmp1.push_back(b2Vec2(ln, 0.0f));
            } else {
                tmp1.push_back(b2Vec2(ln, getRandom(-curOffset, curOffset)));
            }
            ln += curLen;
        }

        curLen /= 2.0f;
        curOffset /= 2.0f;

        while (curLen >= segmentLength_) {
            tmp2.clear();
            for (size_t i = 0; i < tmp1.size() - 1; ++i) {
                tmp2.push_back(tmp1[i]);
                b2Vec2 n = (tmp1[i + 1] - tmp1[i]).Skew();
                n.Normalize();
                tmp2.push_back(0.5f * (tmp1[i] + tmp1[i + 1]) + getRandom(-curOffset, curOffset) * n);
            }
            tmp2.push_back(tmp1.back());
            tmp1.swap(tmp2);
            curLen /= 2.0f;
            curOffset /= 2.0f;
        }

        points_.clear();

        b2Vec2 dir = (tmp1[1] - tmp1[0]).Skew();
        dir.Normalize();

        points_.push_back((-width_ / 2.0f) * dir);
        points_.push_back((width_ / 2.0f) * dir);

        texCoords_.clear();

        appendTriangleStrip(texCoords_, 0.0f, 0.0f);
        appendTriangleStrip(texCoords_, 0.0f, 1.0f);

        float totalLength = 0.0f;

        float imageWidth = width_ * 2.0f * image_.aspect();

        for (size_t i = 1; i < tmp1.size() - 1; ++i) {
            b2Vec2 dir1 = (tmp1[i] - tmp1[i - 1]).Skew();
            b2Vec2 dir2 = (tmp1[i + 1] - tmp1[i]).Skew();

            dir1.Normalize();
            dir2.Normalize();

            b2Vec2 n = dir1 + dir2;
            n.Normalize();

            b2Vec2 p1 = tmp1[i] - ((width_ / 2.0f) / b2Dot(n, dir1)) * n;
            b2Vec2 p2 = tmp1[i] + ((width_ / 2.0f) / b2Dot(n, dir1)) * n;

            points_.push_back(p1);
            points_.push_back(p2);

            totalLength += b2Distance(tmp1[i], tmp1[i - 1]);

            appendTriangleStrip(texCoords_, totalLength / imageWidth, 0.0f);
            appendTriangleStrip(texCoords_, totalLength / imageWidth, 1.0f);
        }

        dir = (tmp1[tmp1.size() - 1] - tmp1[tmp1.size() - 2]).Skew();
        dir.Normalize();

        points_.push_back(b2Vec2(length_, 0.0f) + (-width_ / 2.0f) * dir);
        points_.push_back(b2Vec2(length_, 0.0f) + (width_ / 2.0f) * dir);

        totalLength += b2Distance(tmp1[tmp1.size() - 1], tmp1[tmp1.size() - 2]);

        appendTriangleStrip(texCoords_, totalLength / imageWidth, 0.0f);
        appendTriangleStrip(texCoords_, totalLength / imageWidth, 1.0f);
    }

    void RenderLBoltComponent::updateVertices()
    {
        b2Transform xf = b2Mul(parent()->getSmoothTransform(), t_);

        vertices_.clear();
        for (size_t i = 0; i < points_.size(); ++i) {
            b2Vec2 tmp = b2Mul(xf, points_[i]);
            appendTriangleStrip(vertices_, tmp.x, tmp.y);
        }
    }
}
