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

#include "NumericIndicatorComponent.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    NumericIndicatorComponent::NumericIndicatorComponent(const b2Vec2& pos, const DrawablePtr& drawable)
    : pos_(pos),
      drawable_(drawable),
      height_(1.0f),
      text_(boost::make_shared<TextArea>()),
      amount_(0)
    {
        text_->setText("x0");
    }

    NumericIndicatorComponent::~NumericIndicatorComponent()
    {
    }

    void NumericIndicatorComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void NumericIndicatorComponent::update(float dt)
    {
    }

    void NumericIndicatorComponent::render()
    {
        renderQuad(drawable_->image(), pos_,
            height_ * drawable_->image().aspect(), height_);
        text_->render();
    }

    float NumericIndicatorComponent::charSize() const
    {
        return text_->charSize();
    }

    void NumericIndicatorComponent::setCharSize(float value)
    {
        text_->setCharSize(value);
    }

    const Color& NumericIndicatorComponent::color() const
    {
        return text_->color();
    }

    void NumericIndicatorComponent::setColor(const Color& value)
    {
        text_->setColor(value);
    }

    void NumericIndicatorComponent::setAmount(UInt32 value)
    {
        if (amount_ != value) {
            amount_ = value;
            std::ostringstream os;
            os << "x" << value;
            text_->setText(os.str());
        }
    }

    void NumericIndicatorComponent::onRegister()
    {
        text_->setWidth(scene()->gameWidth());

        const b2Transform& xf = parent()->getSmoothTransform();

        text_->setTransform(b2Mul(xf,
            b2Transform(pos_ +
                b2Vec2(height_ * drawable_->image().aspect() / 2.0f,
                    (text_->charSize()) / 2.0f), b2Rot(0.0f))));
    }

    void NumericIndicatorComponent::onUnregister()
    {
    }

    void NumericIndicatorComponent::renderQuad(const Image& image,
                                               const b2Vec2& pos,
                                               float width,
                                               float height)
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = b2Mul(xf, pos - b2Vec2(width / 2, height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(width / 2, -height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(width / 2, height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, pos + b2Vec2(-width / 2, height / 2)); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors();
    }
}
