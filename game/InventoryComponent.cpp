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

#include "InventoryComponent.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "Scene.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    InventoryComponent::InventoryComponent(const b2Vec2& pos, float height)
    : pos_(pos),
      height_(height),
      frame_(assetManager.getImage("common2/inventory_frame.png")),
      spacing_(0.2f),
      padding_(0.8f)
    {
    }

    InventoryComponent::~InventoryComponent()
    {
    }

    void InventoryComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void InventoryComponent::update(float dt)
    {
    }

    void InventoryComponent::render()
    {
        if (!inventory_) {
            return;
        }

        if (lastState_ != inventory_->state()) {
            lastState_ = inventory_->state();
            items_.clear();

            for (int i = 0; i <= InventoryItemMax; ++i) {
                InventoryItem invItem = static_cast<InventoryItem>(i);

                if (!inventory_->have(invItem)) {
                    continue;
                }

                Item item;

                item.image = Inventory::getImage(invItem);

                if (Inventory::allowMultiple(invItem)) {
                    item.text = boost::make_shared<TextArea>();

                    std::ostringstream os;

                    os << inventory_->count(invItem);

                    item.text->setText(os.str());
                    item.text->setCharSize(height_ / 3.0f);
                    item.text->setWidth(10.0f);
                }

                items_.push_back(item);
            }
        }

        b2Vec2 pos = pos_ - b2Vec2(height_ * frame_.aspect() / 2.0f, height_ / 2.0f);

        float frameWidth = height_ * frame_.aspect();

        for (Items::const_iterator it = items_.begin();
             it != items_.end();
             ++it) {
            renderQuad(frame_, pos, frameWidth, height_);

            float itemWidth = height_ * it->image.aspect();

            if (itemWidth <= frameWidth) {
                renderQuad(it->image, pos, itemWidth - (padding_ * it->image.aspect()), height_ - padding_);
            } else {
                renderQuad(it->image, pos, frameWidth - padding_, (frameWidth - padding_) / it->image.aspect());
            }

            if (it->text) {
                it->text->setTransform(b2Mul(parent()->getSmoothTransform(),
                    b2Transform(pos + b2Vec2(frameWidth / 2.0f - it->text->charSize(),
                        -height_ / 2.0f + it->text->charSize() + 0.2f), b2Rot(0.0f))));
                it->text->render();
            }

            pos -= b2Vec2(frameWidth + spacing_, 0.0f);
        }
    }

    void InventoryComponent::onRegister()
    {
    }

    void InventoryComponent::onUnregister()
    {
    }

    void InventoryComponent::renderQuad(const Image& image,
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
