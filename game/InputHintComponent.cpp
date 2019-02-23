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

#include "InputHintComponent.h"
#include "InputGamepadBinding.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputHintComponent::InputHintComponent(int zOrder)
    : UIComponent(zOrder),
      charSize_(2.0f),
      mouseSize_(4.0f),
      gamepadSize_(3.0f),
      padding_(0.35f, 0.25f),
      spacing_(0.25f),
      tweenTime_(0.0f)
    {
        text_ = boost::make_shared<TextArea>();
        text_->setCharSize(charSize_);
        text_->setWidth(100.0f);
    }

    InputHintComponent::~InputHintComponent()
    {
    }

    void InputHintComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void InputHintComponent::update(float dt)
    {
        tweenTime_ += dt;
        if (tweening_ && tweening_->finished(tweenTime_)) {
            parent()->removeFromParent();
        }
    }

    void InputHintComponent::render()
    {
        float a = 0.8f;

        if (tweening_) {
            a = tweening_->getValue(tweenTime_);
        }

        const b2Transform& xf = parent()->getSmoothTransform();

        b2Vec2 pos = b2Vec2_zero;

        for (std::vector<Entry>::const_iterator it = entries_.begin();
            it != entries_.end(); ++it) {
            if (it->image) {
                float height, width;

                if (it->text) {
                    height = charSize_ + padding_.y * 2.0f;
                    width = it->image.aspect() * height;

                    renderColorQuad(it->image, pos, width, height, Color(1.0f, 1.0f, 1.0f, a), it->flip);

                    it->text->setColor(Color(0.1f, 0.1f, 0.1f, a));
                    it->text->setTransform(
                        b2Mul(xf, b2Transform(pos + b2Vec2((width - (2.0f * it->text->aabb().GetExtents()).x) / 2.0f,
                            (height + it->text->charSize()) / 2.0f), b2Rot(0.0f))));
                    it->text->render();
                } else {
                    height = it->gamepad ? gamepadSize_ : mouseSize_;
                    width = it->image.aspect() * height;

                    float buttonHeight = charSize_ + padding_.y * 2.0f;

                    renderColorQuad(it->image, pos + b2Vec2(0.0f, (buttonHeight - height) / 2.0f),
                        width, height, Color(1.0f, 1.0f, 1.0f, a), it->flip);
                }

                pos += b2Vec2(width + spacing_, 0.0f);
            } else {
                it->text->setColor(Color(1.0f, 1.0f, 1.0f, a));
                it->text->setTransform(
                    b2Mul(xf, b2Transform(pos + b2Vec2(0.0f, padding_.y + charSize_), b2Rot(0.0f))));
                it->text->render();
                pos += b2Vec2((2.0f * it->text->aabb().GetExtents()).x + spacing_, 0.0f);
            }
        }

        text_->setColor(Color(1.0f, 1.0f, 1.0f, a));
        text_->setTransform(
            b2Mul(xf, b2Transform(pos + b2Vec2(0.0f, padding_.y + charSize_), b2Rot(0.0f))));
        text_->render();
    }

    void InputHintComponent::addKbNormal(const std::string& value)
    {
        Entry entry;

        entry.image = assetManager.getImage("common1/kb_normal.png");
        entry.text = boost::make_shared<TextArea>();
        entry.text->setCharSize(charSize_);
        entry.text->setWidth(100.0f);
        entry.text->setText(value);

        entries_.push_back(entry);
    }

    void InputHintComponent::addKbLong(const std::string& value)
    {
        Entry entry;

        entry.image = assetManager.getImage("common1/kb_long.png");
        entry.text = boost::make_shared<TextArea>();
        entry.text->setCharSize(charSize_ / 2.0f);
        entry.text->setWidth(100.0f);
        entry.text->setText(value);

        entries_.push_back(entry);
    }

    void InputHintComponent::addText(const std::string& value)
    {
        Entry entry;

        entry.text = boost::make_shared<TextArea>();
        entry.text->setCharSize(charSize_);
        entry.text->setWidth(100.0f);
        entry.text->setText(value);

        entries_.push_back(entry);
    }

    void InputHintComponent::addMb(bool left)
    {
        Entry entry;

        entry.image = assetManager.getImage("common1/mb.png");
        entry.flip = !left;

        entries_.push_back(entry);
    }

    void InputHintComponent::addGamepadStick(bool left)
    {
        Entry entry;

        entry.image = InputGamepadBinding::getStickImage(left);
        entry.gamepad = true;

        entries_.push_back(entry);
    }

    void InputHintComponent::addGamepadButton(GamepadButton button)
    {
        Entry entry;

        entry.image = InputGamepadBinding::getButtonImage(button);
        entry.gamepad = true;

        entries_.push_back(entry);
    }

    void InputHintComponent::setDescription(const std::string& value)
    {
        text_->setText(value);
    }

    void InputHintComponent::setFade(float duration)
    {
        tweening_ = boost::make_shared<SingleTweening>(duration, EaseOutQuad, 0.8f, 0.0f);
        tweenTime_ = 0.0f;
    }

    void InputHintComponent::onRegister()
    {
    }

    void InputHintComponent::onUnregister()
    {
    }

    void InputHintComponent::renderColorQuad(const Image& image,
        const b2Vec2& pos,
        float width,
        float height,
        const Color& color, bool flip)
    {
        const b2Transform& t = parent()->getSmoothTransform();

        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        if (flip) {
            b2Vec2 tmp = b2Mul(t, pos + b2Vec2(width, 0.0f)); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(0.0f, 0.0f)); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(0.0f, height)); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(width, height)); rop.addVertex(tmp.x, tmp.y);
        } else {
            b2Vec2 tmp = b2Mul(t, pos); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(width, 0.0f)); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(width, height)); rop.addVertex(tmp.x, tmp.y);
            tmp = b2Mul(t, pos + b2Vec2(0.0f, height)); rop.addVertex(tmp.x, tmp.y);
        }

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors(color);
    }
}
