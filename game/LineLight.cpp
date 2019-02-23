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

#include "LineLight.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "RenderLightComponent.h"
#include "Settings.h"
#include <boost/bind.hpp>

namespace af
{
    LineLight::LineLight(const std::string& name)
    : Light(name),
      length_(1.0f),
      distance_(1.0f),
      bothWays_(false)
    {
    }

    LineLight::~LineLight()
    {
    }

    void LineLight::setLength(float value)
    {
        if (length_ != value) {
            length_ = value;
            setDirty();
        }
    }

    void LineLight::setDistance(float value)
    {
        if (distance_ != value) {
            distance_ = value;
            setDirty();
        }
    }

    void LineLight::setBothWays(bool value)
    {
        if (bothWays_ != value) {
            bothWays_ = value;
            setDirty();
        }
    }

    void LineLight::update()
    {
        vertices_.clear();
        colors_.clear();
        s_.clear();

        softVertices_.clear();
        softColors_.clear();
        softS_.clear();

        b2Vec2 p1 = b2Mul(finalTransform(), b2Vec2(0.0f, length_));
        b2Vec2 p2 = b2Mul(finalTransform(), b2Vec2(0.0f, -length_));

        if (!xray()) {
            b2Vec2 pt = 0.5f * (p1 + p2);
            float fraction = 1.0f;

            b2Vec2 n = p2 - p1;
            n.Normalize();

            parent()->scene()->rayCast(pt, p1,
                boost::bind(&Light::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(p1), boost::ref(fraction)));

            p1 += b2_linearSlop * n;

            parent()->scene()->rayCast(pt, p2,
                boost::bind(&Light::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(p2), boost::ref(fraction)));

            p2 -= b2_linearSlop * n;
        }

        b2Vec2 endP1, endP2;
        float s1, s2;

        addOneDir(p1, p2, endP1, endP2, s1, s2);

        if (bothWays_) {
            b2Vec2 endP1r, endP2r;
            float s1r, s2r;

            addOneDir(p2, p1, endP2r, endP1r, s2r, s1r);

            addEdge(p1, endP1, nearS(1.0f), s1);
            addEdge(endP2, p2, s2, nearS(1.0f));
            addEdge(endP1r, p1, s1r, nearS(1.0f));
            addEdge(p2, endP2r, nearS(1.0f), s2r);
        } else {
            addEdge(p1, endP1, nearS(1.0f), s1);
            addEdge(endP2, p2, s2, nearS(1.0f));
            addEdge(p2, p1, nearS(1.0f), nearS(1.0f));
        }
    }

    void LineLight::render()
    {
        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], vertices_.size() / 2);
        rop.addGeneric1(&s_[0], s_.size());
        rop.addColors(&colors_[0], colors_.size() / 4);

        if (!softVertices_.empty()) {
            rop.addVertices(&softVertices_[0], softVertices_.size() / 2);
            rop.addGeneric1(&softS_[0], softS_.size());
            rop.addColors(&softColors_[0], softColors_.size() / 4);
        }
    }

    b2AABB LineLight::calculateAABB()
    {
        float dist = parent()->gammaCorrected(distance_);

        float d = 0.0f;

        if (bothWays_) {
            d = -dist;
        }

        b2Vec2 v[] = {
            b2Mul(finalTransform(), b2Vec2(d, -length_)),
            b2Mul(finalTransform(), b2Vec2(d, length_)),
            b2Mul(finalTransform(), b2Vec2(dist, -length_)),
            b2Mul(finalTransform(), b2Vec2(dist, length_))
        };

        return computeAABB(v, sizeof(v)/sizeof(v[0]));
    }

    void LineLight::addOneDir(const b2Vec2& p1, const b2Vec2& p2,
        b2Vec2& endP1, b2Vec2& endP2,
        float& s1, float& s2)
    {
        b2Vec2 v = (p2 - p1).Skew();
        v.Normalize();

        size_t bv = vertices_.size();
        size_t bc = colors_.size();
        size_t bs = s_.size();

        float dist = parent()->gammaCorrected(distance_);

        for (UInt32 i = 0; i < numRays(); ++i) {
            float a = static_cast<float>(i) / (numRays() - 1);

            b2Vec2 pt = a * p2 + (1.0f - a) * p1;

            appendTriangleStrip(vertices_, pt.x, pt.y, bv);
            appendColorStrip(colors_, color(), bc);
            appendGenericStrip(s_, nearS(1.0f), bs);

            b2Vec2 endPt = pt + dist * v;
            float fraction = 1.0f;

            if (!xray()) {
                parent()->scene()->rayCast(pt, endPt,
                    boost::bind(&LineLight::rayCastCb, this,
                    _1, _2, _3, _4, boost::ref(endPt), boost::ref(fraction)));
            }

            if (i == 0) {
                endP1 = endPt;
                s1 = farS(1.0f - fraction);
            } else if (i == (numRays() - 1)) {
                endP2 = endPt;
                s2 = farS(1.0f - fraction);
            }

            appendTriangleStrip(vertices_, endPt.x, endPt.y, bv);
            appendColorStrip(colors_, color(), bc);
            appendGenericStrip(s_, farS(1.0f - fraction), bs);

            if (soft() && !xray()) {
                appendTriangleStrip(softVertices_, endPt.x, endPt.y, bv);
                appendColorStrip(softColors_, color(), bc);
                appendGenericStrip(softS_, farS(1.0f - fraction), bs);

                endPt = pt + (dist * fraction +
                    (1.0f - fraction) * softLength()) * v;

                appendTriangleStrip(softVertices_, endPt.x, endPt.y, bv);
                appendColorStrip(softColors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
                appendGenericStrip(softS_, farS(0.0f), bs);
            }
        }
    }

    void LineLight::addEdge(const b2Vec2& p1, const b2Vec2& p2,
         float s1, float s2)
    {
        b2Vec2 v = (p2 - p1).Skew();
        v.Normalize();

        size_t bv = vertices_.size();
        size_t bc = colors_.size();
        size_t bs = s_.size();

        appendTriangleStrip(vertices_, p1.x, p1.y, bv);
        appendColorStrip(colors_, color(), bc);
        appendGenericStrip(s_, s1, bs);

        b2Vec2 tmp = p1 + settings.light.edgeSmooth * v;

        appendTriangleStrip(vertices_, tmp.x, tmp.y, bv);
        appendColorStrip(colors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(s_, s1, bs);

        appendTriangleStrip(vertices_, p2.x, p2.y, bv);
        appendColorStrip(colors_, color(), bc);
        appendGenericStrip(s_, s2, bs);

        tmp = p2 + settings.light.edgeSmooth * v;

        appendTriangleStrip(vertices_, tmp.x, tmp.y, bv);
        appendColorStrip(colors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(s_, s2, bs);
    }
}
