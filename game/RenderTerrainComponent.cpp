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

#include "RenderTerrainComponent.h"
#include "SceneObject.h"
#include "Utils.h"
#include "Renderer.h"
#include "poly2tri.h"

namespace af
{
    RenderTerrainComponent::RenderTerrainComponent(const Points& points,
                                                   const std::vector<Points>& holes,
                                                   const DrawablePtr& drawable,
                                                   float imageWidth, float imageHeight,
                                                   const b2Vec2& imageOffset,
                                                   int zOrder)
    : RenderComponent(zOrder),
      drawable_(drawable)
    {
        size_t totalSize = points.size();

        for (size_t i = 0; i < holes.size(); ++i) {
            totalSize += holes[i].size();
        }

        std::vector<p2t::Point> tessPoints;
        std::vector<p2t::Point*> tessPointPtrs;

        tessPoints.resize(totalSize);
        tessPointPtrs.resize(points.size());

        size_t cur = 0;

        for (size_t i = 0; i < points.size(); ++i, ++cur) {
            tessPoints[cur] = p2t::Point(points[i].x, points[i].y);
            tessPointPtrs[i] = &tessPoints[cur];
        }

        p2t::CDT cdt(tessPointPtrs);

        for (size_t i = 0; i < holes.size(); ++i) {
            const Points& hole = holes[i];

            if (hole.size() == 1) {
                /*
                 * Steiner point.
                 */
                tessPoints[cur] = p2t::Point(hole[0].x, hole[0].y);

                cdt.AddPoint(&tessPoints[cur++]);
            } else {
                /*
                 * Hole.
                 */
                tessPointPtrs.resize(hole.size());

                for (size_t j = 0; j < hole.size(); ++j, ++cur) {
                    tessPoints[cur] = p2t::Point(hole[j].x, hole[j].y);
                    tessPointPtrs[j] = &tessPoints[cur];
                }

                cdt.AddHole(tessPointPtrs);
            }
        }

        cdt.Triangulate();

        std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();

        vertices_.resize(triangles.size() * 6);
        texCoords_.resize(triangles.size() * 6);

        for (size_t i = 0; i < triangles.size(); ++i) {
            vertices_[i * 6 + 0] = triangles[i]->GetPoint(0)->x;
            vertices_[i * 6 + 1] = triangles[i]->GetPoint(0)->y;
            vertices_[i * 6 + 2] = triangles[i]->GetPoint(1)->x;
            vertices_[i * 6 + 3] = triangles[i]->GetPoint(1)->y;
            vertices_[i * 6 + 4] = triangles[i]->GetPoint(2)->x;
            vertices_[i * 6 + 5] = triangles[i]->GetPoint(2)->y;

            b2Vec2 tp0 = b2Vec2(triangles[i]->GetPoint(0)->x,
                            triangles[i]->GetPoint(0)->y) + imageOffset;
            b2Vec2 tp1 = b2Vec2(triangles[i]->GetPoint(1)->x,
                            triangles[i]->GetPoint(1)->y) + imageOffset;
            b2Vec2 tp2 = b2Vec2(triangles[i]->GetPoint(2)->x,
                            triangles[i]->GetPoint(2)->y) + imageOffset;

            texCoords_[i * 6 + 0] = tp0.y / imageWidth;
            texCoords_[i * 6 + 1] = tp0.x / imageHeight;
            texCoords_[i * 6 + 2] = tp1.y / imageWidth;
            texCoords_[i * 6 + 3] = tp1.x / imageHeight;
            texCoords_[i * 6 + 4] = tp2.y / imageWidth;
            texCoords_[i * 6 + 5] = tp2.x / imageHeight;
        }
    }

    RenderTerrainComponent::~RenderTerrainComponent()
    {
    }

    void RenderTerrainComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderTerrainComponent::update(float dt)
    {
    }

    void RenderTerrainComponent::render(void* const* parts, size_t numParts)
    {
        applyStencil(true);

        renderer.setProgramDef(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        for (size_t i = 0; i < numParts; ++i) {
            size_t j = reinterpret_cast<size_t>(parts[i]);

            rop.addVertices(&vertices_[j], 3);
            rop.addTexCoords(&texCoords_[j], 3);
        }

        rop.addColors(color());

        applyStencil(false);
    }

    void RenderTerrainComponent::onRegister()
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        cookies_.reserve(vertices_.size() / 6);

        b2Vec2 p[3];

        for (size_t i = 0; i < vertices_.size(); i += 6) {
            p[0].x = vertices_[i + 0];
            p[0].y = vertices_[i + 1];
            p[1].x = vertices_[i + 2];
            p[1].y = vertices_[i + 3];
            p[2].x = vertices_[i + 4];
            p[2].y = vertices_[i + 5];

            p[0] = b2Mul(xf, p[0]);
            p[1] = b2Mul(xf, p[1]);
            p[2] = b2Mul(xf, p[2]);

            vertices_[i + 0] = p[0].x;
            vertices_[i + 1] = p[0].y;
            vertices_[i + 2] = p[1].x;
            vertices_[i + 3] = p[1].y;
            vertices_[i + 4] = p[2].x;
            vertices_[i + 5] = p[2].y;

            b2AABB aabb = computeAABB(&p[0], 3);

            cookies_.push_back(manager()->addAABB(this, aabb, reinterpret_cast<void*>(i)));
        }
    }

    void RenderTerrainComponent::onUnregister()
    {
        for (size_t i = 0; i < cookies_.size(); ++i) {
            manager()->removeAABB(cookies_[i]);
        }

        cookies_.resize(0);
    }
}
