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

#ifndef _AF_TYPES_H_
#define _AF_TYPES_H_

#include "af/FastVector.h"
#include <Box2D/Box2D.h>
#include <string>
#include <vector>

inline bool operator!=(const b2Vec2& a, const b2Vec2& b)
{
    return !(a == b);
}

namespace af
{
    typedef unsigned char UInt8;
    typedef signed char SInt8;
    typedef unsigned short UInt16;
    typedef signed short SInt16;
    typedef unsigned int UInt32;
    typedef signed int SInt32;
    typedef unsigned long long UInt64;
    typedef signed long long SInt64;

    typedef UInt8 Byte;
    typedef std::vector<b2Vec2> Points;

    struct Color
    {
        Color()
        {
            rgba[0] = 0.0f;
            rgba[1] = 0.0f;
            rgba[2] = 0.0f;
            rgba[3] = 1.0f;
        }

        Color(float r, float g, float b, float a = 1.0f)
        {
            rgba[0] = r;
            rgba[1] = g;
            rgba[2] = b;
            rgba[3] = a;
        }

        explicit Color(const b2Color& color)
        {
            rgba[0] = color.r;
            rgba[1] = color.g;
            rgba[2] = color.b;
            rgba[3] = 1.0f;
        }

        inline bool operator==(const Color& rhs) const
        {
            return (::memcmp(rgba, rhs.rgba, sizeof(rhs.rgba)) == 0);
        }

        inline bool operator!=(const Color& rhs) const
        {
            return !(*this == rhs);
        }

        float rgba[4];
    };
}

#endif
