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

#ifndef _DEBUGDRAW_H_
#define _DEBUGDRAW_H_

#include "af/Types.h"

namespace af
{
    class DebugDraw : public b2Draw
    {
    public:
        DebugDraw();
        ~DebugDraw();

        virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount,
                                 const b2Color& color);

        virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount,
                                      const b2Color& color);

        virtual void DrawCircle(const b2Vec2& center, float32 radius,
                                const b2Color& color);

        virtual void DrawSolidCircle(const b2Vec2& center, float32 radius,
                                     const b2Vec2& axis, const b2Color& color);

        virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2,
                                 const b2Color& color);

        virtual void DrawTransform(const b2Transform& xf);
    };
}

#endif
