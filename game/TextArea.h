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

#ifndef _TEXTAREA_H_
#define _TEXTAREA_H_

#include "af/Types.h"
#include "Image.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace af
{
    class TextArea : boost::noncopyable
    {
    public:
        TextArea();
        ~TextArea();

        inline const b2Transform& transform() const { return t_; }
        void setTransform(const b2Transform& value);

        inline float width() const { return width_; }
        void setWidth(float value);

        inline float charSize() const { return charSize_; }
        void setCharSize(float value);

        inline const std::string& text() const { return text_; }
        void setText(const std::string& value);

        inline const Color& color() const { return color_; }
        void setColor(const Color& value);

        int textNumLetters() const;

        b2AABB aabb() const;

        void render(int numLetters = -1);

    private:
        void update() const;

        void addQuad(const Image& image, const b2Vec2& pos,
                     float width, float height) const;

        b2Transform t_;
        float width_;
        float charSize_;
        std::string text_;
        mutable std::vector<UInt16> ucs2Text_;
        Color color_;

        mutable bool dirty_;
        mutable std::vector<float> vertices_;
        mutable std::vector<float> texCoords_;
    };

    typedef boost::shared_ptr<TextArea> TextAreaPtr;
}

#endif
