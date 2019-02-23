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

#include "PauseMenuComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "InputManager.h"
#include "Utils.h"
#include "Logger.h"
#include <boost/make_shared.hpp>

namespace af
{
    PauseMenuComponent::PauseMenuComponent(float width, float height, int zOrder)
    : UIComponent(zOrder),
      frame_(boost::make_shared<Frame>()),
      logo_(assetManager.getImage("common1/pause.png")),
      currentItem_(-1),
      time_(0.0f),
      selectionTimeout_(0.0f),
      finger_(-1),
      fingerItem_(-1)
    {
        frame_->setWidth(width);
        frame_->setHeight(height);
        frame_->setBgColor(Color(0.0f, 0.0f, 0.2f));

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 0.0f, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 1.0f, 0.0f));

        tweening_ = tweening;

        items_.push_back(assetManager.getImage("common1/resume.png"));
        items_.push_back(assetManager.getImage("common1/main_menu.png"));
    }

    PauseMenuComponent::~PauseMenuComponent()
    {
    }

    void PauseMenuComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void PauseMenuComponent::update(float dt)
    {
        time_ += dt;

        if (scene()->inputMenuUI()->havePointer()) {
            b2Vec2 pos(frame_->width() / 2,
                       (frame_->height() - 2.0f) / 2 + (6.5f * (items_.size() - 1) / 2.0f));

            pos = b2Mul(parent()->getSmoothTransform(), pos);

            b2Vec2 point;
            bool pressed = scene()->inputMenuUI()->pressed(&point);
            bool triggered = scene()->inputMenuUI()->triggered();

            if (finger_ == 0) {
                if (pressed) {
                    if (pointInRectFromCenter(point,
                                              pos - b2Vec2(0.0f, 6.5f * fingerItem_), 3.5f * items_[fingerItem_].aspect(),
                                              3.5f)) {
                        if (currentItem_ < 0) {
                            time_ = 0.0f;
                        }
                        currentItem_ = fingerItem_;
                    } else {
                        currentItem_ = -1;
                    }
                } else {
                    int ci = currentItem_;
                    currentItem_ = -1;
                    finger_ = -1;
                    fingerItem_ = -1;
                    onPress(ci);
                }
            } else if (triggered) {
                for (size_t j = 0; j < items_.size(); ++j) {
                    if (pointInRectFromCenter(point,
                                              pos, 3.5f * items_[j].aspect(),
                                              3.5f)) {
                        finger_ = 0;
                        fingerItem_ = currentItem_ = j;
                        time_ = 0.0f;
                        break;
                    }

                    pos -= b2Vec2(0.0f, 6.5f);
                }
            }
        } else {
            if (currentItem_ == -1) {
                currentItem_ = 0;
            }

            selectionTimeout_ -= dt;
            if (scene()->inputMenuUI()->upPressed()) {
                if (selectionTimeout_ <= 0.0f) {
                    if (currentItem_-- == 0) {
                        currentItem_ = items_.size() - 1;
                    }
                    selectionTimeout_ = 0.5f;
                    time_ = 0.0f;
                }
            } else if (scene()->inputMenuUI()->downPressed()) {
                if (selectionTimeout_ <= 0.0f) {
                    if (++currentItem_ == static_cast<int>(items_.size())) {
                        currentItem_ = 0;
                    }
                    selectionTimeout_ = 0.5f;
                    time_ = 0.0f;
                }
            } else if (scene()->inputMenuUI()->okPressed()) {
                onPress(currentItem_);
            } else {
                selectionTimeout_ = 0.0f;
            }
        }
    }

    void PauseMenuComponent::render()
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        frame_->setTransform(xf);

        frame_->render();

        renderQuad(logo_, b2Vec2(frame_->width() / 2.0f, frame_->height() - 4.0f / 2),
                   4.0f * logo_.aspect(),
                   4.0f);

        b2Vec2 pos = b2Vec2(frame_->width() / 2,
                            (frame_->height() - 2.0f) / 2 + (6.5f * (items_.size() - 1) / 2.0f));

        for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
            float extra = 0.0f;

            if (i == currentItem_) {
                extra += tweening_->getValue(time_);
            }

            renderQuad(items_[i], pos,
                       (3.5f + extra) * items_[i].aspect(),
                       3.5f + extra);

            pos -= b2Vec2(0.0f, 6.5f);
        }
    }

    void PauseMenuComponent::onRegister()
    {
    }

    void PauseMenuComponent::onUnregister()
    {
    }

    void PauseMenuComponent::renderQuad(const Image& image, const b2Vec2& pos,
                                        float width, float height)
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

    void PauseMenuComponent::onPress(int i)
    {
        if (i == 0) {
            LOG4CPLUS_INFO(logger(), "game resumed");
            parent()->removeFromParent();
        } else if (i == 1) {
            scene()->setNextLevel("main_menu.lua", "");
        }
    }
}
