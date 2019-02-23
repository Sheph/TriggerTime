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

#include "PointLight.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "RenderLightComponent.h"
#include <boost/bind.hpp>

namespace af
{
    PointLight::PointLight(const std::string& name)
    : Light(name), distance_(1.0f)
    {
    }

    PointLight::~PointLight()
    {
    }

    void PointLight::setDistance(float value)
    {
        if (distance_ != value) {
            distance_ = value;
            setDirty();
        }
    }

    void PointLight::update()
    {
        vertices_.clear();
        colors_.clear();
        s_.clear();

        softVertices_.clear();
        softColors_.clear();
        softS_.clear();

        b2Vec2 absPos = b2Mul(finalTransform(), b2Vec2_zero);

        appendTriangleFan(vertices_, absPos.x, absPos.y);
        appendColorFan(colors_, color());
        appendGenericFan(s_, nearS(1.0f));

        float dist = parent()->gammaCorrected(distance_);

        b2Rot rot(2.0f * b2_pi / (numRays() - 1));
        b2Vec2 dir(1.0f, 0.0f);

        b2Vec2 p = b2Mul(finalTransform(), b2Vec2_zero);

        for (UInt32 i = 0; i < numRays(); ++i) {
            if (i == (numRays() - 1)) {
                dir = b2Vec2(1.0f, 0.0f);
            }

            b2Vec2 pt = p + dist * dir;

            float fraction = 1.0f;
            b2Vec2 endPt = pt;

            if (!xray()) {
                parent()->scene()->rayCast(absPos, pt,
                    boost::bind(&Light::rayCastCb, this,
                    _1, _2, _3, _4, boost::ref(endPt), boost::ref(fraction)));
            }

            appendTriangleFan(vertices_, endPt.x, endPt.y);
            appendColorFan(colors_, color());
            appendGenericFan(s_, farS(1.0f - fraction));

            if (soft() && !xray()) {
                appendTriangleStrip(softVertices_, endPt.x, endPt.y);
                appendColorStrip(softColors_, color());
                appendGenericStrip(softS_, farS(1.0f - fraction));

                endPt = p + (dist * fraction +
                    (1.0f - fraction) * softLength()) * dir;

                appendTriangleStrip(softVertices_, endPt.x, endPt.y);
                appendColorStrip(softColors_, Color(0.0f, 0.0f, 0.0f, 0.0f));
                appendGenericStrip(softS_, farS(0.0f));
            }

            dir = b2Mul(rot, dir);
        }
    }

    void PointLight::render()
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

    b2AABB PointLight::calculateAABB()
    {
        b2AABB aabb;

        b2Vec2 absPos = b2Mul(finalTransform(), b2Vec2_zero);

        float dist = parent()->gammaCorrected(distance_);

        aabb.lowerBound = absPos - b2Vec2(dist, dist);
        aabb.upperBound = absPos + b2Vec2(dist, dist);

        return aabb;
    }
}
