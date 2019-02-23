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

#include "RenderChainComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"

namespace af
{
    Points RenderChainComponent::tmp_;

    RenderChainComponent::RenderChainComponent(const PathPtr& path,
                                               float step,
                                               const DrawablePtr& drawable,
                                               const std::pair<float, float>& heights,
                                               const std::pair<float, float>& angles,
                                               int zOrder)
    : RenderComponent(zOrder),
      drawable_(drawable),
      prevAngle_(0.0f)
    {
        PathIteratorPtr it = path->first();
        PathIteratorPtr lastIt = path->last();

        b2Vec2 prev = it->current();

        it->step(step);

        while (it->less(lastIt)) {
            b2Vec2 cur = it->current();

            float angle = vec2angle(cur - prev);

            b2Rot rot(getRandom(angle + angles.first, angle + angles.second));

            quads_.push_back(Quad());

            float height = getRandom(heights.first, heights.second);
            float width = drawable->image().aspect() * height;

            quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(-height / 2.0f, width / 2.0f)));
            quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(-height / 2.0f, -width / 2.0f)));
            quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(height / 2.0f, -width / 2.0f)));
            quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(height / 2.0f, width / 2.0f)));

            quads_.back().texCoords.insert(quads_.back().texCoords.end(),
                drawable_->image().texCoords(),
                drawable_->image().texCoords() + 12);

            it->step(step);
            prev = cur;
        }

        b2Vec2 cur = lastIt->current();

        float angle = vec2angle(cur - prev);

        b2Rot rot(getRandom(angle + angles.first, angle + angles.second));

        quads_.push_back(Quad());

        float height = getRandom(heights.first, heights.second);
        float width = drawable->image().aspect() * height;

        quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(-height / 2.0f, width / 2.0f)));
        quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(-height / 2.0f, -width / 2.0f)));
        quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(height / 2.0f, -width / 2.0f)));
        quads_.back().points.push_back(prev + b2Mul(rot, b2Vec2(height / 2.0f, width / 2.0f)));

        quads_.back().texCoords.insert(quads_.back().texCoords.end(),
            drawable_->image().texCoords(),
            drawable_->image().texCoords() + 12);
    }

    RenderChainComponent::~RenderChainComponent()
    {
    }

    void RenderChainComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderChainComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_)) {
            return;
        }

        updateVertices(true);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
    }

    void RenderChainComponent::render(void* const* parts, size_t numParts)
    {
        static std::multimap<void*, int> zOrderMap;

        renderer.setProgramDef(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        zOrderMap.clear();

        for (size_t i = 0; i < numParts; ++i) {
            size_t j = reinterpret_cast<size_t>(parts[i]);
            zOrderMap.insert(std::make_pair(&quads_[j].points[0], j));
        }

        for (std::multimap<void*, int>::const_iterator it = zOrderMap.begin(); it != zOrderMap.end(); ++it) {
            rop.addVertices(&quads_[it->second].vertices[0], quads_[it->second].vertices.size() / 2);
            rop.addTexCoords(&quads_[it->second].texCoords[0], quads_[it->second].texCoords.size() / 2);
        }

        rop.addColors(color());
    }

    void RenderChainComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        updateVertices(false);
    }

    void RenderChainComponent::onUnregister()
    {
        for (size_t i = 0; i < cookies_.size(); ++i) {
            manager()->removeAABB(cookies_[i]);
        }

        cookies_.clear();
    }

    void RenderChainComponent::updateVertices(bool updateAABBs)
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        for (size_t i = 0; i < quads_.size(); ++i) {
            tmp_.clear();
            quads_[i].vertices.clear();

            for (size_t j = 0; j < quads_[i].points.size(); ++j) {
                tmp_.push_back(b2Mul(xf, quads_[i].points[j]));
                appendTriangleFan(quads_[i].vertices, tmp_.back().x, tmp_.back().y);
            }

            b2AABB aabb = computeAABB(&tmp_[0], tmp_.size());

            if (updateAABBs) {
                b2Vec2 displacement = parent()->smoothPos() - prevPos_;

                manager()->moveAABB(cookies_[i], quads_[i].prevAABB, aabb, displacement);
            } else {
                cookies_.push_back(manager()->addAABB(this,
                    aabb,
                    reinterpret_cast<void*>(i)));
            }

            quads_[i].prevAABB = aabb;
        }
    }
}
