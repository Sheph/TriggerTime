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

#include "DebugDraw.h"
#include "Renderer.h"

namespace af
{
    static const float32 circleSegments = 30.0f;
    static const float32 circleIncrement = 2.0f * b2_pi / circleSegments;
    static const float32 transformAxisScale = 0.4f;

    DebugDraw::DebugDraw()
    {
    }

    DebugDraw::~DebugDraw()
    {
    }

    void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount,
                                const b2Color& color)
    {
        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        for (int32 i = 0; i < vertexCount; ++i) {
            rop.addVertex(vertices[i].x, vertices[i].y);
        }

        if (vertexCount > 0) {
            rop.addVertex(vertices[0].x, vertices[0].y);
        }

        rop.addColors(Color(color));
    }

    void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount,
                                     const b2Color& color)
    {
        renderer.setProgramColor();

        RenderTriangleFan rop = renderer.renderTriangleFan();

        for (int32 i = 0; i < vertexCount; ++i) {
            rop.addVertex(vertices[i].x, vertices[i].y);
        }

        rop.addColors(Color(color.r, color.g, color.b, 0.35f));

        DrawPolygon(vertices, vertexCount, color);
    }

    void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius,
                               const b2Color& color)
    {
        float32 theta = 0.0f;
        b2Vec2 p0;

        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        for (int32 i = 0; i < circleSegments; ++i) {
            b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

            rop.addVertex(v.x, v.y);

            if (i == 0) {
                p0 = v;
            }

            theta += circleIncrement;
        }

        rop.addVertex(p0.x, p0.y);

        rop.addColors(Color(color));
    }

    void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius,
                                    const b2Vec2& axis, const b2Color& color)
    {
        float32 theta = 0.0f;
        b2Vec2 p0;

        renderer.setProgramColor();

        RenderTriangleFan rop = renderer.renderTriangleFan();

        for (int32 i = 0; i < circleSegments; ++i) {
            b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

            rop.addVertex(v.x, v.y);

            if (i == 0) {
                p0 = v;
            }

            theta += circleIncrement;
        }

        rop.addVertex(p0.x, p0.y);

        rop.addColors(Color(color.r, color.g, color.b, 0.35f));

        DrawCircle(center, radius, color);

        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop2 = renderer.renderLineStrip();

        rop2.addVertex(center.x, center.y);

        b2Vec2 p = center + radius * axis;

        rop2.addVertex(p.x, p.y);

        rop2.addColors(Color(color));
    }

    void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2,
                                const b2Color& color)
    {
        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        rop.addVertex(p1.x, p1.y);
        rop.addVertex(p2.x, p2.y);

        rop.addColors(Color(color));
    }

    void DebugDraw::DrawTransform(const b2Transform& xf)
    {
        renderer.setProgramColorLines(2.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        b2Vec2 p1 = xf.p;

        rop.addVertex(p1.x, p1.y);

        b2Vec2 p2 = p1 + transformAxisScale * xf.q.GetXAxis();

        rop.addVertex(p2.x, p2.y);

        rop.addColors(Color(1.0f, 0.0f, 0.0f));

        rop = renderer.renderLineStrip();

        rop.addVertex(p1.x, p1.y);

        p2 = p1 + transformAxisScale * xf.q.GetYAxis();

        rop.addVertex(p2.x, p2.y);

        rop.addColors(Color(0.0f, 1.0f, 0.0f));
    }
}
