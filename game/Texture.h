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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class Texture : boost::noncopyable
    {
    public:
        enum WrapMode
        {
            WrapModeRepeat = 0,
            WrapModeClamp
        };

        Texture(UInt32 width, UInt32 height,
                WrapMode wrapX, WrapMode wrapY)
        : id_(0),
          width_(width),
          height_(height),
          wrapX_(wrapX),
          wrapY_(wrapY)
        {
        }

        ~Texture();

        inline UInt32 id() const { return id_; }
        inline void setId(UInt32 generation, UInt32 value) { generation_ = generation; id_ = value; }

        inline UInt32 width() const { return width_; }

        inline UInt32 height() const { return height_; }

        inline WrapMode wrapX() const { return wrapX_; }

        inline WrapMode wrapY() const { return wrapY_; }

    private:
        UInt32 generation_;
        UInt32 id_;
        UInt32 width_;
        UInt32 height_;
        WrapMode wrapX_;
        WrapMode wrapY_;
    };

    typedef boost::shared_ptr<Texture> TexturePtr;
}

#endif
