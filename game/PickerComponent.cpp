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

#include "PickerComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "InputManager.h"
#include "Utils.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    static const float itemPadding = 1.0f;

    PickerComponent::PickerComponent()
    : back_(assetManager.getImage("common1/back.png")),
      numVisibleItems_(3),
      firstVisibleItem_(0),
      nextVisibleItem_(0),
      currentItem_(-2),
      pickTime_(0.0f),
      scrollTime_(0.0f),
      selectionTimeout_(0.0f),
      offset_(0.0f),
      finger_(-1),
      backFinger_(false)
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 0.0f, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInOutQuad, 1.0f, 0.0f));

        pickTweening_ = tweening;
    }

    PickerComponent::~PickerComponent()
    {
    }

    void PickerComponent::update(float dt)
    {
        scrollTime_ += dt;
        pickTime_ += dt;

        if (scene()->inputMenuUI()->havePointer()) {
            b2Vec2 point;
            bool pressed = scene()->inputMenuUI()->pressed(&point);
            bool triggered = scene()->inputMenuUI()->triggered();

            if ((finger_ == 0) && !backFinger_) {
                if (pressed) {
                    offset_ = point.x - fingerPos_.x;
                } else {
                    int currentItem = currentItem_;

                    currentItem_ = -2;
                    finger_ = -1;

                    if ((fabs(offset_) <= 0.5f) && (currentItem >= -1)) {
                        offset_ = 0.0f;
                        onPressed(currentItem);
                        return;
                    }

                    int numItems = std::ceil(fabs(offset_) / (items_[0].frame->width() + itemPadding * 2));

                    if (offset_ < 0) {
                        numItems = -numItems;
                    }

                    nextVisibleItem_ = firstVisibleItem_ - numItems;

                    if (nextVisibleItem_ < 0) {
                        nextVisibleItem_ = 0;
                    } else if (nextVisibleItem_ >= static_cast<int>(items_.size())) {
                        nextVisibleItem_ = items_.size() - 1;
                    }

                    numItems = firstVisibleItem_ - nextVisibleItem_;

                    SingleTweeningPtr tweening =
                        boost::make_shared<SingleTweening>(
                            0.2f, EaseOutQuad, offset_,
                            (items_[firstVisibleItem_].frame->width() + itemPadding * 2) * numItems);

                    scrollTweening_ = tweening;
                    scrollTime_ = 0.0f;
                }
            } else if ((finger_ == 0) && backFinger_) {
                if (pressed) {
                    if (pointInRectFromLower(point,
                                             b2Vec2(itemPadding, itemPadding),
                                             3.5f * back_.aspect(),
                                             3.5f)) {
                        if (currentItem_ < -1) {
                            pickTime_ = 0.0f;
                        }
                        currentItem_ = -1;
                    } else {
                        currentItem_ = -2;
                    }
                } else {
                    int currentItem = currentItem_;
                    currentItem_ = -2;
                    finger_ = -1;

                    if (currentItem == -1) {
                        onPressed(currentItem);
                        return;
                    }
                }
            } else if ((finger_ == -1) && triggered && !scrollTweening_) {
                if ((point.y >= (itemPadding + 4.5f)) &&
                    (point.y <= (scene()->gameHeight() - 5.0f))) {
                    finger_ = 0;
                    fingerPos_ = point;
                    backFinger_ = false;

                    int selectedItem = firstVisibleItem_ +
                        point.x / (items_[0].frame->width() + itemPadding * 2);

                    if ((selectedItem >= 0) &&
                        (selectedItem < static_cast<int>(items_.size()))) {
                        currentItem_ = selectedItem;
                        pickTime_ = 0.0f;
                    }
                } else if (pointInRectFromLower(point,
                                                b2Vec2(itemPadding, itemPadding),
                                                3.5f * back_.aspect(),
                                                3.5f)) {
                    finger_ = 0;
                    backFinger_ = true;
                    currentItem_ = -1;
                    pickTime_ = 0.0f;
                }
            }
        } else {
            if (currentItem_ == -2) {
                currentItem_ = 0;
            }

            selectionTimeout_ -= dt;

            if (scene()->inputMenuUI()->leftPressed()) {
                if (!scrollTweening_ &&
                    (selectionTimeout_ <= 0.0f) &&
                    (currentItem_ > 0)) {
                    --currentItem_;
                    selectionTimeout_ = 0.5f;
                    pickTime_ = 0.0f;

                    if (currentItem_ < firstVisibleItem_) {
                        nextVisibleItem_ = firstVisibleItem_ - 1;

                        SingleTweeningPtr tweening =
                            boost::make_shared<SingleTweening>(
                                0.2f, EaseOutQuad, 0.0f,
                                items_[firstVisibleItem_].frame->width() + itemPadding * 2);

                        scrollTweening_ = tweening;
                        scrollTime_ = 0.0f;
                    }
                }
            } else if (scene()->inputMenuUI()->rightPressed()) {
                if (!scrollTweening_ &&
                    (selectionTimeout_ <= 0.0f) &&
                    (currentItem_ >= 0) &&
                    (currentItem_ != static_cast<int>(items_.size() - 1))) {
                    ++currentItem_;
                    selectionTimeout_ = 0.5f;
                    pickTime_ = 0.0f;

                    if (currentItem_ >= (firstVisibleItem_ + numVisibleItems_)) {
                        nextVisibleItem_ = firstVisibleItem_ + 1;

                        SingleTweeningPtr tweening =
                            boost::make_shared<SingleTweening>(
                                0.2f, EaseOutQuad, 0.0f,
                                -(items_[firstVisibleItem_].frame->width() + itemPadding * 2));

                        scrollTweening_ = tweening;
                        scrollTime_ = 0.0f;
                    }
                }
            } else if (scene()->inputMenuUI()->upPressed()) {
                if (!scrollTweening_ && (selectionTimeout_ <= 0.0f) && (currentItem_ < 0)) {
                    currentItem_ = firstVisibleItem_;
                    selectionTimeout_ = 0.5f;
                    pickTime_ = 0.0f;
                }
            } else if (scene()->inputMenuUI()->downPressed()) {
                if (!scrollTweening_ && (selectionTimeout_ <= 0.0f) && (currentItem_ >= 0)) {
                    currentItem_ = -1;
                    selectionTimeout_ = 0.5f;
                    pickTime_ = 0.0f;
                }
            } else if (scene()->inputMenuUI()->okPressed()) {
                if (!scrollTweening_) {
                    onPressed(currentItem_);
                    if (!scene()) {
                        return;
                    }
                }
            } else {
                selectionTimeout_ = 0.0f;
            }
        }

        if (scrollTweening_) {
            offset_ = scrollTweening_->getValue(scrollTime_);

            if (scrollTweening_->finished(scrollTime_)) {
                scrollTweening_.reset();
                firstVisibleItem_ = nextVisibleItem_;
                offset_ = 0.0f;
            }
        }
    }

    void PickerComponent::render()
    {
        float extra = offset_ - (items_[0].frame->width() + itemPadding * 2) * firstVisibleItem_;

        b2Vec2 pos((scene()->gameWidth() - 4.0f * header_.aspect()) / 2, scene()->gameHeight() - 5.0f);

        renderQuad(header_, pos,
                   4.0f * header_.aspect(),
                   4.0f);

        b2Transform t;

        t.p = b2Vec2(extra, itemPadding + 4.5f);
        t.q = b2Rot(0.0f);

        for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
            float contentPadding = 0.8f;

            t.p += b2Vec2(itemPadding, 0.0f);

            float frameWidth = items_[i].frame->width();
            float frameHeight = items_[i].frame->height();
            float titleCharSize = items_[i].title->charSize();
            float detailsCharSize = items_[i].details->charSize();
            b2Vec2 off = b2Vec2_zero;

            if (i == currentItem_) {
                float xVal = pickTweening_->getValue(pickTime_);
                float yVal = xVal * (frameHeight / frameWidth);

                items_[i].frame->setWidth(frameWidth + xVal);
                items_[i].frame->setHeight(frameHeight + yVal);
                items_[i].title->setCharSize(titleCharSize * items_[i].frame->height() / frameHeight);
                items_[i].title->setWidth(items_[i].frame->width());
                items_[i].details->setCharSize(detailsCharSize * items_[i].frame->height() / frameHeight);
                items_[i].details->setWidth(items_[i].frame->width());

                off.x = -xVal / 2;
                off.y = -yVal / 2;

                contentPadding *= items_[i].frame->height() / frameHeight;
            }

            b2Transform t2 = t;

            t2.p += off;

            items_[i].frame->setTransform(t2);
            items_[i].frame->render();

            t2.p = t.p + off + b2Vec2(contentPadding,
                items_[i].frame->height() - contentPadding);

            items_[i].title->setTransform(t2);
            items_[i].title->render();

            float imageHeight =
                (items_[i].frame->width() - contentPadding * 2) / items_[i].image.aspect();

            t2.p = t.p + off + b2Vec2(contentPadding,
                items_[i].frame->height() - contentPadding * 2 - items_[i].title->charSize() * 2 - imageHeight);

            renderQuad(items_[i].image, t2.p,
                       items_[i].frame->width() - contentPadding * 2,
                       imageHeight);

            t2.p = t.p + off + b2Vec2(contentPadding,
                contentPadding + items_[i].details->charSize());

            items_[i].details->setTransform(t2);
            items_[i].details->render();

            if (i == currentItem_) {
                items_[i].frame->setWidth(frameWidth);
                items_[i].frame->setHeight(frameHeight);
                items_[i].title->setCharSize(titleCharSize);
                items_[i].title->setWidth(items_[i].frame->width());
                items_[i].details->setCharSize(detailsCharSize);
                items_[i].details->setWidth(items_[i].frame->width());
            }

            t.p += b2Vec2(items_[i].frame->width() + itemPadding, 0.0f);
        }

        extra = 0.0f;

        if (currentItem_ == -1) {
            extra = pickTweening_->getValue(pickTime_);
        }

        renderQuad(back_, b2Vec2(itemPadding - (extra * back_.aspect() / 2), itemPadding - (extra / 2)),
                   (3.5f + extra) * back_.aspect(),
                   3.5f + extra);
    }

    void PickerComponent::setHeader(const std::string& image)
    {
        header_ = assetManager.getImage(image);
    }

    void PickerComponent::addItem(const std::string& title,
                                  const std::string& image,
                                  const std::string& details)
    {
        Item item;

        item.frame = boost::make_shared<Frame>();
        item.frame->setWidth(13.0f);
        item.frame->setHeight(17.0f);
        item.frame->setBgColor(Color(0.0f, 0.0f, 0.2f));

        item.title = boost::make_shared<TextArea>();
        item.title->setWidth(13.0f);
        item.title->setText(title);
        item.title->setCharSize(1.5f);

        item.image = assetManager.getImage(image);

        item.details = boost::make_shared<TextArea>();
        item.details->setWidth(13.0f);
        item.details->setText(details);
        item.details->setCharSize(1.0f);

        items_.push_back(item);
    }

    void PickerComponent::renderQuad(const Image& image, const b2Vec2& pos,
                                     float width, float height)
    {
        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = pos; rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, 0.0f); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, height); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(0.0f, height); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors();
    }
}
