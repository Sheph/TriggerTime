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

#include "RenderPolygonsComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    Points RenderPolygonsComponent::tmp_;

    RenderPolygonsComponent::RenderPolygonsComponent(const std::vector<Points>& polygons,
                                                     const Image& image,
                                                     float imageWidth, float imageHeight,
                                                     int zOrder)
    : RenderComponent(zOrder),
      polygons_(polygons),
      image_(image),
      prevAngle_(0.0f),
      cookie_(0)
    {
        b2AABB aabb = computeAABB(polygons_);

        aabb.lowerBound.y = aabb.upperBound.y;

        for (size_t i = 0; i < polygons_.size(); ++i) {
            size_t base = texCoords_.size();
            for (size_t j = 0; j < polygons_[i].size(); ++j) {
                b2Vec2 p = polygons_[i][j] - aabb.lowerBound;

                /*
                 * Texture coordinates need to be rotated by
                 * -90 degrees since we're texturing 0 degree
                 * rotated object.
                 */

                appendTriangleFan(texCoords_, p.y / imageWidth, p.x / imageHeight, base);
            }
        }
    }

    RenderPolygonsComponent::~RenderPolygonsComponent()
    {
    }

    void RenderPolygonsComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderPolygonsComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_)) {
            return;
        }

        b2AABB aabb = updateVertices();

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = aabb;
    }

    void RenderPolygonsComponent::render(void* const* parts, size_t numParts)
    {
        renderer.setProgramDef(image_.texture());

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], vertices_.size() / 2);
        rop.addTexCoords(&texCoords_[0], texCoords_.size() / 2);
        rop.addColors(color());
    }

    void RenderPolygonsComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = updateVertices();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
    }

    void RenderPolygonsComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }

    b2AABB RenderPolygonsComponent::updateVertices()
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        tmp_.clear();
        vertices_.clear();

        for (size_t i = 0; i < polygons_.size(); ++i) {
            size_t base = vertices_.size();
            for (size_t j = 0; j < polygons_[i].size(); ++j) {
                tmp_.push_back(b2Mul(xf, polygons_[i][j]));
                appendTriangleFan(vertices_, tmp_.back().x, tmp_.back().y, base);
            }
        }

        return computeAABB(&tmp_[0], tmp_.size());
    }
}
