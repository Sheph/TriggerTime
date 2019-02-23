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

#ifndef _FRAME_H_
#define _FRAME_H_

#include "af/Types.h"
#include "Image.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace af
{
    class Frame : boost::noncopyable
    {
    public:
        Frame();
        ~Frame();

        inline const b2Transform& transform() const { return t_; }
        inline void setTransform(const b2Transform& value) { t_ = value; }

        inline float width() const { return width_; }
        inline void setWidth(float value) { width_ = value; }

        inline float height() const { return height_; }
        inline void setHeight(float value) { height_ = value; }

        inline float borderSize() const { return borderSize_; }
        inline void setBorderSize(float value) { borderSize_ = value; }

        inline const Color& bgColor() const { return bgColor_; }
        inline void setBgColor(const Color& value) { bgColor_ = value; }

        inline const Image& bgImage() const { return bg_; }
        inline void setBgImage(const Image& value) { bg_ = value; }

        inline const Color& borderColor() const { return borderColor_; }
        inline void setBorderColor(const Color& value) { borderColor_ = value; }

        void render();

    private:
        void renderColorQuad(const Image& image, const b2Vec2& pos,
                             float width, float height,
                             const Color& color);

        b2Transform t_;
        float width_;
        float height_;
        float borderSize_;
        Color borderColor_;
        Color bgColor_;
        Image bg_;
        Image topLeft_;
        Image top_;
        Image topRight_;
        Image right_;
        Image bottomRight_;
        Image bottom_;
        Image bottomLeft_;
        Image left_;
    };

    typedef boost::shared_ptr<Frame> FramePtr;
}

#endif
