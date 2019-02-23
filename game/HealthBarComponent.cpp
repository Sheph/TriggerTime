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

#include "HealthBarComponent.h"
#include "AssetManager.h"
#include "Renderer.h"

namespace af
{
    HealthBarComponent::HealthBarComponent(const b2Vec2& pos, float width, float height)
    : pos_(pos), width_(width), height_(height),
      percent_(0.0f),
      heart_(assetManager.getImage("common1/healthbar_heart.png")),
      bg_(assetManager.getImage("healthbar_bg.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      end_(assetManager.getImage("healthbar_end.png", Texture::WrapModeClamp, Texture::WrapModeClamp)),
      life_(assetManager.getImage("healthbar_life.png", Texture::WrapModeRepeat, Texture::WrapModeClamp))
    {
    }

    HealthBarComponent::~HealthBarComponent()
    {
    }

    void HealthBarComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void HealthBarComponent::update(float dt)
    {
    }

    void HealthBarComponent::render()
    {
        b2Vec2 pos = pos_ + b2Vec2(height_ * heart_.aspect() * 3 / 5, 0.0f);

        renderQuad(bg_, pos, width_, height_);
        renderQuad(end_, pos + b2Vec2(width_, 0.0f),
                   height_ * end_.aspect(), height_);
        renderQuad(life_, pos,
                   (width_ + (height_ * end_.aspect()) / 4.2f) * percent_,
                   height_);
        renderQuad(heart_, pos_, height_ * heart_.aspect(), height_);
    }

    void HealthBarComponent::onRegister()
    {
    }

    void HealthBarComponent::onUnregister()
    {
    }

    void HealthBarComponent::renderQuad(const Image& image,
                                        const b2Vec2& pos,
                                        float width,
                                        float height)
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = b2Mul(xf, pos); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(width, 0.0f)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(width, height)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(0.0f, height)); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors();
    }
}
