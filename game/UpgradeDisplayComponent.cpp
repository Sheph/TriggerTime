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

#include "UpgradeDisplayComponent.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "Scene.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    UpgradeDisplayComponent::UpgradeDisplayComponent(const b2Vec2& pos, float height)
    : pos_(pos),
      height_(height),
      spacing_(0.2f)
    {
    }

    UpgradeDisplayComponent::~UpgradeDisplayComponent()
    {
    }

    void UpgradeDisplayComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void UpgradeDisplayComponent::update(float dt)
    {
    }

    void UpgradeDisplayComponent::render()
    {
        b2Vec2 pos = pos_ + b2Vec2(0.0f, height_ / 2.0f);

        for (int i = UpgradeIdArmor; i >= UpgradeIdMoreAmmo; --i) {
            if (scene()->upgradeCount(static_cast<UpgradeId>(i)) > 0.0f) {
                Image image = UpgradeManager::getImage(static_cast<UpgradeId>(i));

                pos -= b2Vec2(height_ * image.aspect() / 2.0f, 0.0f);

                renderQuad(image, pos, height_ * image.aspect(), height_);

                pos -= b2Vec2(height_ * image.aspect() / 2.0f + spacing_, 0.0f);
            }
        }
    }

    void UpgradeDisplayComponent::onRegister()
    {
    }

    void UpgradeDisplayComponent::onUnregister()
    {
    }

    void UpgradeDisplayComponent::renderQuad(const Image& image,
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
