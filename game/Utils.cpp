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

#include "Utils.h"
#include <cmath>

namespace af
{
    b2AABB computeAABB(const b2Vec2* vertices, size_t numVertices)
    {
        b2Vec2 lower = vertices[0];
        b2Vec2 upper = lower;

        for (size_t i = 1; i < numVertices; ++i) {
            b2Vec2 v = vertices[i];
            lower = b2Min(lower, v);
            upper = b2Max(upper, v);
        }

        b2AABB aabb;

        aabb.lowerBound = lower;
        aabb.upperBound = upper;

        return aabb;
    }

    b2AABB computeAABB(const std::vector<Points>& polygons)
    {
        b2Vec2 lower = polygons[0][0];
        b2Vec2 upper = lower;

        for (size_t i = 1; i < polygons[0].size(); ++i) {
            b2Vec2 v = polygons[0][i];
            lower = b2Min(lower, v);
            upper = b2Max(upper, v);
        }

        for (size_t i = 1; i < polygons.size(); ++i) {
            for (size_t j = 0; j < polygons[i].size(); ++j) {
                b2Vec2 v = polygons[i][j];
                lower = b2Min(lower, v);
                upper = b2Max(upper, v);
            }
        }

        b2AABB aabb;

        aabb.lowerBound = lower;
        aabb.upperBound = upper;

        return aabb;
    }

    b2Vec2 angle2vec(float angle, float length)
    {
        return b2Mul(b2Rot(angle), b2Vec2(length, 0.0f));
    }

    float vec2angle(const b2Vec2& vec)
    {
        return std::atan2(vec.y, vec.x);
    }

    float angleBetween(const b2Vec2& v1, const b2Vec2& v2)
    {
        return std::atan2(b2Cross(v1, v2), b2Dot(v1, v2));
    }

    float normalizeAngle(float angle)
    {
        angle = std::fmod(angle + b2_pi, 2.0f * b2_pi);

        if (angle < 0.0f) {
            angle += 2.0f * b2_pi;
        }

        return angle - b2_pi;
    }

    bool pointInRect(const b2Vec2& p, const b2Vec2& lower, const b2Vec2& upper)
    {
        return ((p.x >= b2Min(lower.x,upper.x)) && (p.x <= b2Max(lower.x,upper.x)) &&
                (p.y >= b2Min(lower.y,upper.y)) && (p.y <= b2Max(lower.y,upper.y)));
    }

    bool pointInRect(const b2Vec2& p, const b2AABB& aabb)
    {
        return pointInRect(p, aabb.lowerBound, aabb.upperBound);
    }

    bool pointInRectFromLower(const b2Vec2& p, const b2Vec2& lower,
                              float width, float height)
    {
        return pointInRect(p, lower, lower + b2Vec2(width, height));
    }

    bool pointInRectFromCenter(const b2Vec2& p, const b2Vec2& center,
                               float width, float height)
    {
        return pointInRect(p, center - b2Vec2(width / 2.0f, height / 2.0f),
                           center + b2Vec2(width / 2.0f, height / 2.0f));
    }

    b2Vec2 linePointDistance(const b2Vec2& p,
            const b2Vec2& p1, const b2Vec2& p2)
    {
        b2Vec2 tmp = (p2 - p1).Skew();

        tmp.Normalize();

        return b2Dot(p1 - p, tmp) * tmp;
    }

    b2Vec2 solveLinearIntercept(const b2Vec2& p, const b2Vec2& v,
            const b2Vec2& interceptP, float interceptV)
    {
        /*
         * See http://zulko.github.io/blog/2013/11/11/interception-of-a-linear-trajectory-with-constant-speed
         * This is exactly the sample implementation, only it doesn't care about 'H' position.
         * Also, if the 'p' cannot be caught it can still return
         * some "best effort" value, this case can be handled though by checking
         * where 'interceptP' will be after time 't', we don't do this because
         * we don't need it for now.
         */

        b2Vec2 tmp = interceptP - p;
        float tmpLen = tmp.Length();
        float vLen = v.Length();
        float sinB = (tmp.x * v.y - tmp.y * v.x) / (tmpLen * vLen);
        float sinA = (vLen / interceptV) * sinB;

        if (fabs(sinA) > 1.0f) {
            return p;
        }

        float sinC = sinA * b2Sqrt(1.0f - sinB * sinB) + sinB * b2Sqrt(1.0f - sinA * sinA);

        float bc = (sinA / sinC) * tmpLen;

        float t = (bc / vLen);

        b2Vec2 res = (p + t * v);

        if (!res.IsValid()) {
            /*
             * I've tried everything, check for 0 before div, checking for near 0, etc.
             * But the fucking NaN keeps showing up, so cut the crap, just check for bad
             * values and return 'p'.
             */
            return p;
        }

        return res;
    }

    void appendColorFan(std::vector<float>& v, const Color& color, size_t base)
    {
        size_t i = v.size();

        if ((i - base) >= 12) {
            v.push_back(v[base + 0]);
            v.push_back(v[base + 1]);
            v.push_back(v[base + 2]);
            v.push_back(v[base + 3]);
            v.push_back(v[i - 4]);
            v.push_back(v[i - 3]);
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(color.rgba[0]);
        v.push_back(color.rgba[1]);
        v.push_back(color.rgba[2]);
        v.push_back(color.rgba[3]);
    }

    void appendGenericFan(std::vector<float>& v, float s, size_t base)
    {
        size_t i = v.size();

        if ((i - base) >= 3) {
            v.push_back(v[base + 0]);
            v.push_back(v[i - 1]);
        }

        v.push_back(s);
    }

    void appendTriangleStrip(std::vector<float>& v,
                             float x, float y, size_t base)
    {
        size_t i = v.size();

        if ((i - base) >= 6) {
            v.push_back(v[i - 4]);
            v.push_back(v[i - 3]);
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(x);
        v.push_back(y);
    }

    void appendColorStrip(std::vector<float>& v, const Color& color, size_t base)
    {
        size_t i = v.size();

        if ((i - base) >= 12) {
            v.push_back(v[i - 8]);
            v.push_back(v[i - 7]);
            v.push_back(v[i - 6]);
            v.push_back(v[i - 5]);
            v.push_back(v[i - 4]);
            v.push_back(v[i - 3]);
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(color.rgba[0]);
        v.push_back(color.rgba[1]);
        v.push_back(color.rgba[2]);
        v.push_back(color.rgba[3]);
    }

    void appendGenericStrip(std::vector<float>& v, float s, size_t base)
    {
        size_t i = v.size();

        if ((i - base) >= 3) {
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(s);
    }
}
