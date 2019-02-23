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

#include "LetterBoxComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    LetterBoxComponent::LetterBoxComponent(float distance, float duration, int zOrder)
    : UIComponent(zOrder),
      borderWidth_(0.5f),
      tweenTime_(0.0f),
      active_(false),
      bgColor_(0.0f, 0.0f, 0.0f, 1.0f),
      borderColor_(0.0f, 0.0f, 0.0f, 1.0f),
      bg_(assetManager.getImage("dialog_bg.png", Texture::WrapModeRepeat, Texture::WrapModeRepeat)),
      top_(assetManager.getImage("dialog_top.png", Texture::WrapModeRepeat, Texture::WrapModeClamp)),
      bottom_(assetManager.getImage("dialog_bottom.png", Texture::WrapModeRepeat, Texture::WrapModeClamp))
    {
        tweening_ = boost::make_shared<SingleTweening>(duration, EaseOutQuad, 0.0f, distance);
    }

    LetterBoxComponent::~LetterBoxComponent()
    {
    }

    void LetterBoxComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void LetterBoxComponent::update(float dt)
    {
        if (active_) {
            tweenTime_ += dt;
            if (tweenTime_ > tweening_->duration()) {
                tweenTime_ = tweening_->duration();
            }
        } else {
            tweenTime_ -= dt;
            if (tweenTime_ < 0.0f) {
                tweenTime_ = 0.0f;
            }
        }
    }

    void LetterBoxComponent::render()
    {
        if (tweenTime_ <= 0.0f) {
            return;
        }

        float dist = tweening_->getValue(tweenTime_);

        if (dist > (borderWidth_ / 2.0f)) {
            renderColorQuad(bg_, b2Vec2(0.0f, 0.0f),
                scene()->gameWidth(), dist - (borderWidth_ / 2.0f), bgColor_);
            renderColorQuad(bg_, b2Vec2(0.0f, scene()->gameHeight() - (dist - (borderWidth_ / 2.0f))),
                scene()->gameWidth(), dist - (borderWidth_ / 2.0f), bgColor_);
        }
        renderColorQuad(bottom_, b2Vec2(0.0f, dist - borderWidth_),
            scene()->gameWidth(), borderWidth_, borderColor_);
        renderColorQuad(top_, b2Vec2(0.0f, scene()->gameHeight() - dist),
            scene()->gameWidth(), borderWidth_, borderColor_);
    }

    void LetterBoxComponent::setActive(bool value, bool immediate)
    {
        active_ = value;
        if (immediate) {
            if (value) {
                tweenTime_ = tweening_->duration();
            } else {
                tweenTime_ = 0.0f;
            }
        }
    }

    void LetterBoxComponent::onRegister()
    {
    }

    void LetterBoxComponent::onUnregister()
    {
    }

    void LetterBoxComponent::renderColorQuad(const Image& image,
        const b2Vec2& pos,
        float width,
        float height,
        const Color& color)
    {
        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = pos; rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, 0.0f); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, height); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(0.0f, height); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors(color);
    }
}
