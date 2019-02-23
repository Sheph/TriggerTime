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

#include "DialogComponent.h"
#include "Settings.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    DialogComponent::DialogComponent(const b2Vec2& pos, float width, float height)
    : pos_(pos), width_(width), height_(height),
      frame_(boost::make_shared<Frame>()),
      title_(boost::make_shared<TextArea>()),
      message_(boost::make_shared<TextArea>()),
      blinkTime_(0.0f),
      blinkState_(false),
      fastComplete_(true),
      okPressed_(false)
    {
        frame_->setWidth(width_);
        frame_->setHeight(height_);
        frame_->setBorderSize(settings.dialog.borderSize);
        frame_->setBgColor(settings.dialog.bgColor);

        title_->setCharSize(settings.dialog.titleCharSize);
        title_->setWidth(width_ - settings.dialog.borderSize * 2 - settings.dialog.padding * 3 - settings.dialog.portraitSize);

        message_->setCharSize(settings.dialog.messageCharSize);
        message_->setWidth(width_ - settings.dialog.borderSize * 2 - settings.dialog.padding * 3 - settings.dialog.portraitSize);
        message_->setColor(settings.dialog.messageColor);

        setTitlePlayer("");
        setMessage("");
    }

    DialogComponent::~DialogComponent()
    {
    }

    void DialogComponent::update(float dt)
    {
        if (!okPressed_) {
            okPressed_ = scene()->inputCutscene()->okPressed();
        }

        letterTime_ -= dt;

        if (letterCur_ <= -1) {
            blinkTime_ -= dt;
            if (blinkTime_ <= 0.0f) {
                blinkState_ = !blinkState_;
                blinkTime_ = settings.dialog.blinkTimeThreshold;
            }
        }

        if (letterCur_ == -1) {
            if (okPressed_) {
                okPressed_ = false;
                letterCur_ = -2;
                okPressed();
            }
        } else if ((letterCur_ >= 0) && (letterTime_ <= 0.0f)) {
            ++letterCur_;
            if ((letterCur_ == (message_->textNumLetters() - 2)) || (fastComplete_ && okPressed_)) {
                letterCur_ = -1;
            } else {
                letterTime_ = settings.dialog.letterTimeThreshold;
            }
            okPressed_ = false;
        }
    }

    void DialogComponent::render()
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        frame_->setTransform(b2Mul(xf, b2Transform(pos_, b2Rot(0.0f))));

        frame_->render();

        renderQuad(portrait_,
                   pos_ + b2Vec2(settings.dialog.borderSize + settings.dialog.padding,
                                 height_ - settings.dialog.borderSize - settings.dialog.portraitSize - settings.dialog.padding),
                   settings.dialog.portraitSize,
                   settings.dialog.portraitSize);

        title_->setTransform(
            b2Mul(xf, b2Transform(pos_ + b2Vec2(settings.dialog.borderSize + settings.dialog.padding * 2 + settings.dialog.portraitSize,
                                                height_ - settings.dialog.borderSize - settings.dialog.padding),
                                  b2Rot(0.0f))));

        title_->render();

        message_->setTransform(
            b2Mul(xf, b2Transform(pos_ + b2Vec2(settings.dialog.borderSize + settings.dialog.padding * 2 + settings.dialog.portraitSize,
                                                height_ - settings.dialog.borderSize - settings.dialog.padding * 2 - settings.dialog.titleCharSize),
                                  b2Rot(0.0f))));

        if ((letterCur_ <= -1) && (!blinkState_ || !fastComplete_)) {
            message_->render(message_->textNumLetters() - 2);
        } else {
            message_->render(letterCur_);
        }
    }

    void DialogComponent::setTitlePlayer(const std::string& name)
    {
        title_->setColor(settings.dialog.titlePlayerColor);
        title_->setText(name);
        portrait_ = assetManager.getImage("common1/portrait_player.png");
    }

    void DialogComponent::setTitleAlly(const std::string& name,
                                       const std::string& image)
    {
        title_->setColor(settings.dialog.titleAllyColor);
        title_->setText(name);
        portrait_ = assetManager.getImage(image);
    }

    void DialogComponent::setTitleEnemy(const std::string& name,
                                        const std::string& image)
    {
        title_->setColor(settings.dialog.titleEnemyColor);
        title_->setText(name);
        portrait_ = assetManager.getImage(image);
    }

    void DialogComponent::setMessage(const std::string& message)
    {
        message_->setText(message + " >>");
        letterCur_ = 0;
        letterTime_ = settings.dialog.letterTimeThreshold;
    }

    void DialogComponent::endDialog()
    {
        if (parent()) {
            parent()->removeFromParent();
        }
    }

    void DialogComponent::renderQuad(const Image& image,
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
