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

#include "RenderTextComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    RenderTextComponent::RenderTextComponent(const b2Vec2& pos, float angle,
        float width, const std::string& text, int zOrder)
    : RenderComponent(zOrder),
      pos_(pos),
      angle_(angle),
      numLetters_(-1),
      prevAngle_(0.0f),
      cookie_(0),
      dirty_(false)
    {
        text_ = boost::make_shared<TextArea>();
        text_->setWidth(width);
        text_->setText(text);
    }

    RenderTextComponent::~RenderTextComponent()
    {
    }

    void RenderTextComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderTextComponent::update(float dt)
    {
        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_) &&
            !dirty_) {
            return;
        }

        text_->setTransform(b2Mul(parent()->getSmoothTransform(), b2Transform(pos_, b2Rot(angle_))));

        b2AABB aabb = text_->aabb();

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        prevAABB_ = aabb;
        dirty_ = false;
    }

    void RenderTextComponent::render(void* const* parts, size_t numParts)
    {
        text_->setColor(color());
        text_->render(numLetters_);
    }

    void RenderTextComponent::setPos(const b2Vec2& value)
    {
        if (pos_ != value) {
            pos_ = value;
            dirty_ = true;
        }
    }

    void RenderTextComponent::setAngle(float value)
    {
        if (angle_ != value) {
            angle_ = value;
            dirty_ = true;
        }
    }

    void RenderTextComponent::setWidth(float value)
    {
        if (text_->width() != value) {
            text_->setWidth(value);
            dirty_ = true;
        }
    }

    void RenderTextComponent::setCharSize(float value)
    {
        if (text_->charSize() != value) {
            text_->setCharSize(value);
            dirty_ = true;
        }
    }

    void RenderTextComponent::setText(const std::string& value)
    {
        if (text_->text() != value) {
            text_->setText(value);
            dirty_ = true;
        }
    }

    void RenderTextComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        text_->setTransform(b2Mul(parent()->getSmoothTransform(), b2Transform(pos_, b2Rot(angle_))));
        prevAABB_ = text_->aabb();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
        dirty_ = false;
    }

    void RenderTextComponent::onUnregister()
    {
        manager()->removeAABB(cookie_);
    }
}
