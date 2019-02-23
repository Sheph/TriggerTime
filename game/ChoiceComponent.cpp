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

#include "ChoiceComponent.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneObject.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Utils.h"
#include "Settings.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    ChoiceComponent::ChoiceComponent(int zOrder)
    : UIComponent(zOrder),
      currentItem_(-1),
      tweenTime_(0.0f),
      selectionTimeout_(0.0f),
      finger_(-1),
      fingerItem_(-1)
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 0.0f, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 1.0f, 0.0f));

        tweening_ = tweening;
    }

    ChoiceComponent::~ChoiceComponent()
    {
    }

    void ChoiceComponent::update(float dt)
    {
        if (!scene()->inputGameUI()->active()) {
            finger_ = -1;
            return;
        }

        tweenTime_ += dt;

        if (scene()->inputGameUI()->havePointer()) {
            b2Vec2 point;
            bool pressed = scene()->inputGameUI()->pressed(&point);
            bool triggered = scene()->inputGameUI()->triggered();

            if (finger_ == 0) {
                if (pressed) {
                    b2Transform xf = b2Mul(parent()->getTransform(), items_[fingerItem_].xf);
                    if (pointInRectFromCenter(b2MulT(xf, point),
                                              b2Vec2_zero,
                                              items_[fingerItem_].height * items_[fingerItem_].image.aspect(),
                                              items_[fingerItem_].height)) {
                        if (currentItem_ < 0) {
                            tweenTime_ = 0.0f;
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
                for (int j = 0; j < static_cast<int>(items_.size()); ++j) {
                    b2Transform xf = b2Mul(parent()->getTransform(), items_[j].xf);
                    if (pointInRectFromCenter(b2MulT(xf, point),
                                              b2Vec2_zero,
                                              items_[j].height * items_[j].image.aspect(),
                                              items_[j].height)) {
                        finger_ = 0;
                        fingerItem_ = currentItem_ = j;
                        tweenTime_ = 0.0f;
                    }
                }
            }
        } else {
            if (currentItem_ == -1) {
                currentItem_ = 0;
            }

            selectionTimeout_ -= dt;
            if (scene()->inputGameUI()->leftPressed() || scene()->inputGameUI()->downPressed()) {
                if (selectionTimeout_ <= 0.0f) {
                    if (currentItem_-- == 0) {
                        currentItem_ = static_cast<int>(items_.size() - 1);
                    }
                    selectionTimeout_ = 0.5f;
                    if (items_.size() > 1) {
                        tweenTime_ = 0.0f;
                    }
                }
            } else if (scene()->inputGameUI()->rightPressed() || scene()->inputGameUI()->upPressed()) {
                if (selectionTimeout_ <= 0.0f) {
                    if (++currentItem_ == static_cast<int>(items_.size())) {
                        currentItem_ = 0;
                    }
                    selectionTimeout_ = 0.5f;
                    if (items_.size() > 1) {
                        tweenTime_ = 0.0f;
                    }
                }
            } else if (scene()->inputGameUI()->okPressed()) {
                onPress(currentItem_);
            } else {
                selectionTimeout_ = 0.0f;
            }
        }
    }

    void ChoiceComponent::render()
    {
        if (!scene()->inputGameUI()->active()) {
            return;
        }

        for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
            float extra = 0.0f;

            if (i == currentItem_) {
                extra += tweening_->getValue(tweenTime_);
            }

            renderQuad(items_[i].image, items_[i].xf,
                       (items_[i].height + extra) * items_[i].image.aspect(),
                       items_[i].height + extra);
        }
    }

    void ChoiceComponent::addItem(const b2Vec2& pos, float angle, float height,
        const std::string& image)
    {
        Item item;

        item.xf = b2Transform(pos, b2Rot(angle));
        item.height = height;
        item.image = assetManager.getImage(image);

        items_.push_back(item);
    }

    void ChoiceComponent::renderQuad(const Image& image, b2Transform xf,
        float width, float height)
    {
        xf = b2Mul(parent()->getTransform(), xf);

        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = b2Mul(xf, b2Vec2(-width / 2, -height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(width / 2, -height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(width / 2, height / 2)); rop.addVertex(tmp.x, tmp.y);
        tmp = b2Mul(xf, b2Vec2(-width / 2, height / 2)); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors(Color(1.0f, 1.0f, 1.0f, settings.touchScreen.alpha));
    }
}
