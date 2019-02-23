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

#include "ConeLight.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "RenderLightComponent.h"
#include "Settings.h"
#include <boost/bind.hpp>

namespace af
{
    ConeLight::ConeLight(const std::string& name)
    : Light(name),
      nearDistance_(0.0f),
      farDistance_(1.0f),
      coneAngle_(b2_pi / 4.0f)
    {
    }

    ConeLight::~ConeLight()
    {
    }

    void ConeLight::setNearDistance(float value)
    {
        if (nearDistance_ != value) {
            nearDistance_ = value;
            setDirty();
        }
    }

    void ConeLight::setFarDistance(float value)
    {
        if (farDistance_ != value) {
            farDistance_ = value;
            setDirty();
        }
    }

    void ConeLight::setConeAngle(float value)
    {
        if (coneAngle_ != value) {
            coneAngle_ = value;
            setDirty();
        }
    }

    void ConeLight::update()
    {
        vertices_.clear();
        colors_.clear();
        s_.clear();

        softVertices_.clear();
        softColors_.clear();
        softS_.clear();

        edgeVertices_.clear();
        edgeColors_.clear();
        edgeS_.clear();

        b2Vec2 absPos = b2Mul(finalTransform(), b2Vec2_zero);

        float farDist = parent()->gammaCorrected(farDistance_);

        if (nearDistance_ != 0.0f) {
            addNearEdge();
        }

        for (UInt32 i = 0; i < numRays(); ++i) {
            float a = coneAngle_ - 2.0f * coneAngle_ * i / (numRays() - 1);

            b2Vec2 pt = b2Mul(finalTransform(),
                b2Vec2(nearDistance_, nearDistance_ * tanf(a)));

            appendTriangleStrip(vertices_, pt.x, pt.y);
            appendColorStrip(colors_, color());
            appendGenericStrip(s_, nearS(1.0f));

            b2Vec2 endPt = b2Mul(finalTransform(), farDist * b2Vec2(cosf(a), sinf(a)));
            float fraction = 1.0f;

            if (!xray()) {
                parent()->scene()->rayCast(absPos, endPt,
                    boost::bind(&Light::rayCastCb, this,
                    _1, _2, _3, _4, boost::ref(endPt), boost::ref(fraction)));
            }

            appendTriangleStrip(vertices_, endPt.x, endPt.y);
            appendColorStrip(colors_, color());
            appendGenericStrip(s_, farS(1.0f - fraction));

            if (i == 0) {
                addEdge(pt, endPt, fraction, false);
            } else if (i == (numRays() - 1)) {
                addEdge(pt, endPt, fraction, true);
            }

            if (soft() && !xray()) {
                appendTriangleStrip(softVertices_, endPt.x, endPt.y);
                appendColorStrip(softColors_, color());
                appendGenericStrip(softS_, farS(1.0f - fraction));

                endPt = b2Mul(finalTransform(),
                    (farDist * fraction +
                    (1.0f - fraction) * softLength()) * b2Vec2(cosf(a), sinf(a)));

                appendTriangleStrip(softVertices_, endPt.x, endPt.y);
                appendColorStrip(softColors_, Color(0.0f, 0.0f, 0.0f, 0.0f));
                appendGenericStrip(softS_, farS(0.0f));
            }
        }
    }

    void ConeLight::render()
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

        rop.addVertices(&edgeVertices_[0], edgeVertices_.size() / 2);
        rop.addGeneric1(&edgeS_[0], edgeS_.size());
        rop.addColors(&edgeColors_[0], edgeColors_.size() / 4);
    }

    b2AABB ConeLight::calculateAABB()
    {
        float farDist = parent()->gammaCorrected(farDistance_);

        float p = farDist * sinf(coneAngle_);

        b2Vec2 v[] = {
            b2Mul(finalTransform(), b2Vec2(nearDistance_, -p)),
            b2Mul(finalTransform(), b2Vec2(nearDistance_, p)),
            b2Mul(finalTransform(), b2Vec2(farDist, -p)),
            b2Mul(finalTransform(), b2Vec2(farDist, p))
        };

        return computeAABB(v, sizeof(v)/sizeof(v[0]));
    }

    /*
     * This is needed in order to smooth that light cone edge crap that happens
     * even after 3 gaussian blur passes.
     * @{
     */

    void ConeLight::addNearEdge()
    {
        float p = nearDistance_ * tanf(coneAngle_);

        b2Vec2 pt = b2Mul(finalTransform(), b2Vec2(nearDistance_, -p));
        b2Vec2 endPt = b2Mul(finalTransform(), b2Vec2(nearDistance_, p));

        b2Vec2 v = (endPt - pt).Skew();
        v.Normalize();

        size_t bv = edgeVertices_.size();
        size_t bc = edgeColors_.size();
        size_t bs = edgeS_.size();

        appendTriangleStrip(edgeVertices_, pt.x, pt.y, bv);
        appendColorStrip(edgeColors_, color(), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);

        b2Vec2 tmp = pt + settings.light.edgeSmooth * v;

        appendTriangleStrip(edgeVertices_, tmp.x, tmp.y, bv);
        appendColorStrip(edgeColors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);

        appendTriangleStrip(edgeVertices_, endPt.x, endPt.y, bv);
        appendColorStrip(edgeColors_, color(), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);

        tmp = endPt + settings.light.edgeSmooth * v;

        appendTriangleStrip(edgeVertices_, tmp.x, tmp.y, bv);
        appendColorStrip(edgeColors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);
    }

    void ConeLight::addEdge(const b2Vec2& pt, const b2Vec2& endPt, float fraction, bool dir)
    {
        b2Vec2 v = (endPt - pt).Skew();
        if (dir) {
            v = -v;
        }
        v.Normalize();

        size_t bv = edgeVertices_.size();
        size_t bc = edgeColors_.size();
        size_t bs = edgeS_.size();

        appendTriangleStrip(edgeVertices_, pt.x, pt.y, bv);
        appendColorStrip(edgeColors_, color(), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);

        b2Vec2 tmp = pt + settings.light.edgeSmooth * v;

        appendTriangleStrip(edgeVertices_, tmp.x, tmp.y, bv);
        appendColorStrip(edgeColors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(edgeS_, nearS(1.0f), bs);

        appendTriangleStrip(edgeVertices_, endPt.x, endPt.y, bv);
        appendColorStrip(edgeColors_, color(), bc);
        appendGenericStrip(edgeS_, farS(1.0f - fraction), bs);

        tmp = endPt + settings.light.edgeSmooth * v;

        appendTriangleStrip(edgeVertices_, tmp.x, tmp.y, bv);
        appendColorStrip(edgeColors_, Color(0.0f, 0.0f, 0.0f, 0.0f), bc);
        appendGenericStrip(edgeS_, farS(1.0f - fraction), bs);
    }

    /*
     * @}
     */
}
