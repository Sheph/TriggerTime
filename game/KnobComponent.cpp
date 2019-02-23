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

#include "KnobComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    KnobComponent::KnobComponent(int zOrder)
    : UIComponent(zOrder),
      ring_(assetManager.getImage("common1/knob_ring.png")),
      handle_(assetManager.getImage("common1/knob_handle.png")),
      radius_(0.0f),
      handleRadius_(0.0f),
      drawRing_(false),
      handlePos_(b2Vec2_zero),
      tweenTime_(0.0f),
      actualHandleRadius_(0.0f),
      alpha_(1.0f),
      text_(boost::make_shared<TextArea>()),
      textSize_(1.0f),
      textPos_(b2Vec2_zero)
    {
        text_->setWidth(100.0f);
    }

    KnobComponent::~KnobComponent()
    {
    }

    void KnobComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void KnobComponent::update(float dt)
    {
        if (!tweening_) {
            actualHandleRadius_ = handleRadius_;
        } else {
            actualHandleRadius_ = tweening_->getValue(tweenTime_);
            tweenTime_ += dt;
        }
    }

    void KnobComponent::render()
    {
        if (drawRing_) {
            renderQuad(ring_, -b2Vec2(radius_, radius_), radius_ * 2.0f, radius_ * 2.0f);
        }

        renderQuad(handle_, handlePos_ - b2Vec2(actualHandleRadius_, actualHandleRadius_), actualHandleRadius_ * 2.0f, actualHandleRadius_ * 2.0f);

        if (image_) {
            renderQuad(image_,
                handlePos_ - b2Vec2(actualHandleRadius_ - 0.1f, (actualHandleRadius_ - 0.1f) / image_.aspect()),
                (actualHandleRadius_ - 0.1f) * 2.0f, (actualHandleRadius_ - 0.1f) * 2.0f / image_.aspect());
        }

        if (!text_->text().empty()) {
            float factor = actualHandleRadius_ / handleRadius_;
            text_->setCharSize(textSize_ * factor);
            text_->setTransform(b2Mul(parent()->getSmoothTransform(),
                b2Transform(factor * (handlePos_ + textPos_), b2Rot(0.0f))));
            text_->render();
        }
    }

    bool KnobComponent::showOff() const
    {
        return !!tweening_;
    }

    void KnobComponent::setShowOff(bool value)
    {
        if (!value) {
            tweening_.reset();
            return;
        }

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseOutQuad, 0.0f, handleRadius_ * (4.0f / 3.0f)));

        SequentialTweeningPtr tweening2 = boost::make_shared<SequentialTweening>(true);

        tweening2->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, handleRadius_ * (4.0f / 3.0f), handleRadius_ * (3.0f / 4.0f)));
        tweening2->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, handleRadius_ * (3.0f / 4.0f), handleRadius_ * (4.0f / 3.0f)));

        tweening->addTweening(tweening2);

        tweening_ = tweening;
        tweenTime_ = 0.0f;
    }

    void KnobComponent::onRegister()
    {
    }

    void KnobComponent::onUnregister()
    {
    }

    void KnobComponent::renderQuad(const Image& image,
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

        rop.addColors(Color(1.0f, 1.0f, 1.0f, alpha_));
    }
}
