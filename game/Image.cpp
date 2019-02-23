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

#include "Image.h"

namespace af
{
    Image::Image()
    {
    }

    Image::Image(const TexturePtr& texture,
                 UInt32 x, UInt32 y,
                 UInt32 width, UInt32 height)
    : texture_(texture),
      x_(x),
      y_(y),
      width_(width),
      height_(height)
    {
        if (!texture) {
            aspect_ = 0.0f;
            return;
        }

        float xf = static_cast<float>(x);
        float yf = static_cast<float>(y);

        b2AABB aabb;

        aabb.lowerBound.x = (xf - 0.5f) / texture->width();
        aabb.lowerBound.y = 1.0f - (yf + height + 0.5f) / texture->height();
        aabb.upperBound.x = (xf + width + 0.5f) / texture->width();
        aabb.upperBound.y = 1.0f - (yf - 0.5f) / texture->height();

        texCoords_[0] = texCoords_[6] = aabb.lowerBound.x;
        texCoords_[1] = texCoords_[7] = aabb.lowerBound.y;
        texCoords_[2] = aabb.upperBound.x;
        texCoords_[3] = aabb.lowerBound.y;
        texCoords_[4] = texCoords_[8] = aabb.upperBound.x;
        texCoords_[5] = texCoords_[9] = aabb.upperBound.y;
        texCoords_[10] = aabb.lowerBound.x;
        texCoords_[11] = aabb.upperBound.y;

        aspect_ = static_cast<float>(width) / height;
    }

    Image::~Image()
    {
    }
}
