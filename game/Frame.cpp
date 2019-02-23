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

#include "Frame.h"
#include "AssetManager.h"
#include "Renderer.h"

namespace af
{
    Frame::Frame()
    : t_(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)),
      width_(1.0f),
      height_(1.0f),
      borderSize_(0.3f),
      borderColor_(1.0f, 1.0f, 1.0f, 1.0f),
      bg_(assetManager.getImage("dialog_bg.png", Texture::WrapModeRepeat, Texture::WrapModeRepeat)),
      topLeft_(assetManager.getImage("dialog_topleft.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      top_(assetManager.getImage("dialog_top.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      topRight_(assetManager.getImage("dialog_topright.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      right_(assetManager.getImage("dialog_right.png", Texture::WrapModeClamp, Texture::WrapModeRepeat)),
      bottomRight_(assetManager.getImage("dialog_bottomright.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      bottom_(assetManager.getImage("dialog_bottom.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      bottomLeft_(assetManager.getImage("dialog_bottomleft.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      left_(assetManager.getImage("dialog_left.png", Texture::WrapModeClamp, Texture::WrapModeRepeat))
    {
    }

    Frame::~Frame()
    {
    }

    void Frame::render()
    {
        renderColorQuad(bg_,
                        b2Vec2(borderSize_ * bottomLeft_.aspect() / 2, borderSize_ / 2),
                        width_ - borderSize_ * (bottomLeft_.aspect() + bottomRight_.aspect()) / 2,
                        height_ - borderSize_,
                        bgColor_);
        renderColorQuad(topLeft_,
                   b2Vec2(0.0f, height_ - borderSize_),
                   borderSize_ * topLeft_.aspect(),
                   borderSize_,
                   borderColor_);
        renderColorQuad(top_,
                   b2Vec2(borderSize_ * topLeft_.aspect(), height_ - borderSize_),
                   width_ - borderSize_ * (topLeft_.aspect() + topRight_.aspect()),
                   borderSize_,
                   borderColor_);
        renderColorQuad(topRight_,
                   b2Vec2(width_ - borderSize_ * topRight_.aspect(), height_ - borderSize_),
                   borderSize_ * topRight_.aspect(),
                   borderSize_,
                   borderColor_);
        renderColorQuad(right_,
                   b2Vec2(width_ - borderSize_ * topRight_.aspect(), borderSize_),
                   borderSize_ * bottomRight_.aspect(),
                   height_ - borderSize_ * 2,
                   borderColor_);
        renderColorQuad(bottomRight_,
                   b2Vec2(width_ - borderSize_ * bottomRight_.aspect(), 0.0f),
                   borderSize_ * bottomRight_.aspect(),
                   borderSize_,
                   borderColor_);
        renderColorQuad(bottom_,
                   b2Vec2(borderSize_ * bottomLeft_.aspect(), 0.0f),
                   width_ - borderSize_ * (bottomLeft_.aspect() + bottomRight_.aspect()),
                   borderSize_,
                   borderColor_);
        renderColorQuad(bottomLeft_,
                   b2Vec2(0.0f, 0.0f),
                   borderSize_ * bottomLeft_.aspect(),
                   borderSize_,
                   borderColor_);
        renderColorQuad(left_,
                   b2Vec2(0.0f, borderSize_),
                   borderSize_ * bottomLeft_.aspect(),
                   height_ - borderSize_ * 2,
                   borderColor_);
    }

    void Frame::renderColorQuad(const Image& image,
                                const b2Vec2& pos,
                                float width,
                                float height,
                                const Color& color)
    {
        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = b2Mul(t_, pos); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(t_, pos + b2Vec2(width, 0.0f)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(t_, pos + b2Vec2(width, height)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(t_, pos + b2Vec2(0.0f, height)); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors(color);
    }
}
