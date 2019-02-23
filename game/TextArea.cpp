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

#include "TextArea.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Utils.h"
#include <sstream>

namespace af
{
    TextArea::TextArea()
    : t_(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)),
      width_(1.0f),
      charSize_(1.0f),
      color_(1.0f, 1.0f, 1.0f),
      dirty_(true)
    {
    }

    TextArea::~TextArea()
    {
    }

    void TextArea::setTransform(const b2Transform& value)
    {
        if ((t_.p == value.p) &&
            (t_.q.s == value.q.s) &&
            (t_.q.c == value.q.c)) {
            return;
        }

        t_ = value;
        dirty_ = true;
    }

    void TextArea::setWidth(float value)
    {
        if (width_ == value) {
            return;
        }

        width_ = value;
        dirty_ = true;
    }

    void TextArea::setCharSize(float value)
    {
        if (charSize_ == value) {
            return;
        }

        charSize_ = value;
        dirty_ = true;
    }

    void TextArea::setText(const std::string& value)
    {
        text_ = value;
        dirty_ = true;
        ucs2Text_.clear();
    }

    void TextArea::setColor(const Color& value)
    {
        color_ = value;
    }

    int TextArea::textNumLetters() const
    {
        if (dirty_) {
            update();
            dirty_ = false;
        }

        return texCoords_.size() / 12;
    }

    b2AABB TextArea::aabb() const
    {
        if (dirty_) {
            update();
            dirty_ = false;
        }

        if (vertices_.empty()) {
            b2Vec2 tmp[2] = { b2Mul(t_, b2Vec2_zero), b2Mul(t_, b2Vec2(1.0f, 1.0f)) };

            return computeAABB(&tmp[0], sizeof(tmp)/sizeof(tmp[0]));
        }

        b2Vec2 lower(vertices_[0], vertices_[1]);
        b2Vec2 upper = lower;

        for (size_t i = 2; i < vertices_.size(); i += 2) {
            b2Vec2 v(vertices_[i], vertices_[i + 1]);
            lower = b2Min(lower, v);
            upper = b2Max(upper, v);
        }

        b2AABB aabb;

        aabb.lowerBound = lower;
        aabb.upperBound = upper;

        return aabb;
    }

    void TextArea::render(int numLetters)
    {
        if (dirty_) {
            update();
            dirty_ = false;
        }

        if (texCoords_.empty() || (numLetters == 0)) {
            return;
        }

        size_t numVertices;

        if ((numLetters > 0) && (numLetters <= textNumLetters())) {
            numVertices = numLetters * 6;
        } else {
            numVertices = texCoords_.size() / 2;
        }

        renderer.setProgramDef(assetManager.charMap().begin()->second.texture());

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&vertices_[0], numVertices);
        rop.addTexCoords(&texCoords_[0], numVertices);

        rop.addColors(color_);
    }

    void TextArea::update() const
    {
        const CharMap& charMap = assetManager.charMap();

        if (ucs2Text_.empty()) {
            UTF8toUCS2(text_, ucs2Text_);
        }

        vertices_.clear();
        texCoords_.clear();

        b2Vec2 pos(0.0f, -charSize_);

        size_t i = 0;

        while (i < ucs2Text_.size()) {
            if (ucs2Text_[i] == L' ') {
                pos.x += charSize_ / 3;

                if (pos.x > width_) {
                    pos.x = 0;
                    pos.y -= charSize_;
                }

                ++i;

                continue;
            } else if (ucs2Text_[i] == L'\r') {
                ++i;

                continue;
            } else if (ucs2Text_[i] == L'\n') {
                pos.x = 0;
                pos.y -= charSize_;

                ++i;

                continue;
            }

            size_t specialPos;

            for (specialPos = i; specialPos < ucs2Text_.size(); ++specialPos) {
                if ((ucs2Text_[specialPos] == L'\r') ||
                    (ucs2Text_[specialPos] == L'\n') ||
                    (ucs2Text_[specialPos] == L' ')) {
                    break;
                }
            }

            float curWidth = pos.x;

            for (size_t j = i; j < specialPos; ++j) {
                CharMap::const_iterator it = charMap.find(ucs2Text_[j]);
                if (it == charMap.end()) {
                    it = charMap.find(L'?');
                }

                const Image& image = it->second;

                curWidth += image.aspect() * charSize_;
            }

            if (curWidth > width_) {
                pos.x = 0;
                pos.y -= charSize_;
            }

            for (size_t j = i; j < specialPos; ++j) {
                CharMap::const_iterator it = charMap.find(ucs2Text_[j]);
                if (it == charMap.end()) {
                    it = charMap.find(L'?');
                }

                const Image& image = it->second;

                addQuad(image, pos, image.aspect() * charSize_, charSize_);

                pos.x += image.aspect() * charSize_;
            }

            i = specialPos;
        }
    }

    void TextArea::addQuad(const Image& image, const b2Vec2& pos,
                           float width, float height) const
    {
        size_t i = vertices_.size();

        vertices_.resize(i + 12);

        b2Vec2 tmp = b2Mul(t_, pos);
        vertices_[i + 0] = vertices_[i + 6] = tmp.x;
        vertices_[i + 1] = vertices_[i + 7] = tmp.y;

        tmp = b2Mul(t_, pos + b2Vec2(width, 0.0f));
        vertices_[i + 2] = tmp.x;
        vertices_[i + 3] = tmp.y;

        tmp = b2Mul(t_, pos + b2Vec2(width, height));
        vertices_[i + 4] = vertices_[i + 8] = tmp.x;
        vertices_[i + 5] = vertices_[i + 9] = tmp.y;

        tmp = b2Mul(t_, pos + b2Vec2(0.0f, height));
        vertices_[i + 10] = tmp.x;
        vertices_[i + 11] = tmp.y;

        texCoords_.insert(texCoords_.end(),
                          image.texCoords(),
                          image.texCoords() + 12);
    }
}
