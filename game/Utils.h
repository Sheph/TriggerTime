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

#ifndef _UTILS_H_
#define _UTILS_H_

#include "af/Types.h"
#include <Box2D/Box2D.h>

namespace af
{
    #define AF_SCRIPT_CALL(func, ...) \
        try \
        { \
            call<void>(func,##__VA_ARGS__); \
        } \
        catch (const luabind::error& e) \
        { \
            ::lua_pop(e.state(), 1); \
        } \
        catch (const std::exception& e) \
        { \
            LOG4CPLUS_ERROR(logger(), e.what()); \
        }

    inline float deg2rad(float value)
    {
       return float((value * b2_pi) / 180.0f);
    }

    inline float rad2deg(float value)
    {
       return float((value * 180.0f) / b2_pi);
    }

    b2AABB computeAABB(const b2Vec2* vertices, size_t numVertices);

    b2AABB computeAABB(const std::vector<Points>& polygons);

    b2Vec2 angle2vec(float angle, float length);

    /*
     * angle is -M_PI..M_PI.
     */
    float vec2angle(const b2Vec2& vec);

    /*
     * angle is -M_PI..M_PI.
     */
    float angleBetween(const b2Vec2& v1, const b2Vec2& v2);

    /*
     * result is -M_PI..M_PI.
     */
    float normalizeAngle(float angle);

    bool pointInRect(const b2Vec2& p, const b2Vec2& lower, const b2Vec2& upper);

    bool pointInRect(const b2Vec2& p, const b2AABB& aabb);

    bool pointInRectFromLower(const b2Vec2& p, const b2Vec2& lower,
                              float width, float height);

    bool pointInRectFromCenter(const b2Vec2& p, const b2Vec2& center,
                               float width, float height);

    b2Vec2 linePointDistance(const b2Vec2& p,
        const b2Vec2& p1, const b2Vec2& p2);

    b2Vec2 solveLinearIntercept(const b2Vec2& p, const b2Vec2& v,
        const b2Vec2& interceptP, float interceptV);

    template <class T>
    void appendColors(T& v,
                      const Color& color,
                      int count)
    {
        size_t i = v.size();

        v.resize(i + count * 4);

        while (count-- > 0) {
            v[i++] = color.rgba[0];
            v[i++] = color.rgba[1];
            v[i++] = color.rgba[2];
            v[i++] = color.rgba[3];
        }
    }

    void appendColorFan(std::vector<float>& v, const Color& color, size_t base = 0);

    template <class T>
    void appendTriangleFan(T& v,
                           float x, float y, size_t base = 0)
    {
        size_t i = v.size();

        if ((i - base) >= 6) {
            v.push_back(v[base]);
            v.push_back(v[base + 1]);
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(x);
        v.push_back(y);
    }

    void appendGenericFan(std::vector<float>& v, float s, size_t base = 0);

    void appendTriangleStrip(std::vector<float>& v,
                             float x, float y, size_t base = 0);

    void appendColorStrip(std::vector<float>& v, const Color& color, size_t base = 0);

    void appendGenericStrip(std::vector<float>& v, float s, size_t base = 0);
}

#endif
