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

#include "CrosshairComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "SingleTweening.h"
#include "InputManager.h"
#include "Settings.h"
#include "Scene.h"
#include "PlayerComponent.h"
#include "CameraComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    CrosshairComponent::CrosshairComponent(int zOrder)
    : UIComponent(zOrder),
      dot_(assetManager.getImage("common1/crosshair_dot.png")),
      tweenTime_(0.0f)
    {
        tweening_ = boost::make_shared<SingleTweening>(0.5f, EaseOutQuad, 1.0f, 0.0f);
        tweenTime_ = tweening_->duration();
    }

    CrosshairComponent::~CrosshairComponent()
    {
    }

    void CrosshairComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void CrosshairComponent::update(float dt)
    {
        tweenTime_ += dt;
    }

    void CrosshairComponent::render()
    {
        if (!settings.crosshair.enabled || settings.touchScreen.enabled ||
            scene()->cutscene() || scene()->paused() || !scene()->playable()) {
            return;
        }

        bool relative = false;

        b2Vec2 pos = inputManager.lookPos(relative);

        if (relative) {
            if (!scene()->player() || scene()->player()->dead() ||
                scene()->inputRoboArm()->active() ||
                scene()->inputTurret()->active()) {
                return;
            }
            pos.Normalize();
            pos *= 7.0f;
            if (relativeObj_) {
                pos += scene()->camera()->findComponent<CameraComponent>()->getGamePoint(relativeObj_->smoothPos());
            } else {
                pos += scene()->camera()->findComponent<CameraComponent>()->getGamePoint(scene()->player()->smoothPos());
            }
        }

        float height = 0.7f;
        float width = dot_.aspect() * height;

        renderColorQuad(dot_, pos - b2Vec2(width / 2.0f, height / 2.0f), width, height, Color(0.0f, 1.0f, 1.0f, 1.0f));

        if (!tweening_->finished(tweenTime_)) {
            float v = tweening_->getValue(tweenTime_);
            float a = 0.5f * (1.0f - v) + 1.0f * v;

            height = 0.7f * (1.0f - v) + 2.5f * v;
            width = dot_.aspect() * height;

            renderColorQuad(dot_, pos - b2Vec2(width / 2.0f, height / 2.0f), width, height, Color(0.6f, 1.0f, 1.0f, a));
        }
    }

    void CrosshairComponent::trigger()
    {
        tweenTime_ = 0.0f;

        if (scene()->inputPlayer()->active() && scene()->player()) {
            PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();
            if (pc) {
                pc->triggerRecoil();
            }
        }
    }

    void CrosshairComponent::onRegister()
    {
    }

    void CrosshairComponent::onUnregister()
    {
        relativeObj_.reset();
    }

    void CrosshairComponent::renderColorQuad(const Image& image,
        const b2Vec2& pos,
        float width,
        float height,
        const Color& color)
    {
        renderer.setProgramDef(image.texture());

        RenderTriangleFan rop = renderer.renderTriangleFan();

        b2Vec2 tmp = pos; rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, 0.0f); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(width, height); rop.addVertex(tmp.x, tmp.y);
        tmp = pos + b2Vec2(0.0f, height); rop.addVertex(tmp.x, tmp.y);

        rop.addTexCoords(image.texCoords(), 6);

        rop.addColors(color);
    }
}
