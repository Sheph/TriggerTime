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

#include "InputContextMechPC.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextMechPC::InputContextMechPC(Scene* scene)
    : InputContextMech(scene),
      primaryImage_(assetManager.getImage("subway1/mechfist_icon.png")),
      secondaryImage_(assetManager.getImage("subway1/mechgun_icon.png"))
    {
        primaryRadius_ = settings.pc.primaryRadius;
        primaryPos_ = settings.pc.primaryPadding + b2Vec2(primaryRadius_, primaryRadius_);

        secondaryRadius_ = settings.pc.secondaryRadius;
        secondaryPos_ = settings.pc.secondaryPadding + b2Vec2(secondaryRadius_, secondaryRadius_);

        doDeactivate();
    }

    InputContextMechPC::~InputContextMechPC()
    {
        doDeactivate();
    }

    void InputContextMechPC::update(float dt)
    {
        if (!primaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(primaryPos_);
            primaryKnob_ = boost::make_shared<KnobComponent>(-1);
            primaryKnob_->setHandleRadius(primaryRadius_);
            primaryKnob_->setDrawRing(false);
            primaryKnob_->setAlpha(settings.pc.alpha);
            primaryKnob_->setImage(primaryImage_);
            obj->addComponent(primaryKnob_);
            scene()->addObject(obj);
        }

        if (!secondaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(secondaryPos_);
            secondaryKnob_ = boost::make_shared<KnobComponent>();
            secondaryKnob_->setHandleRadius(secondaryRadius_);
            secondaryKnob_->setDrawRing(false);
            secondaryKnob_->setAlpha(settings.pc.alpha);
            secondaryKnob_->setImage(secondaryImage_);
            obj->addComponent(secondaryKnob_);
            scene()->addObject(obj);
        }
    }

    bool InputContextMechPC::movePressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        b2Vec2 tmp = b2Vec2_zero;
        bool pressed = false;

        if (inputManager.binding(ActionIdMoveUp)->pressed()) {
            tmp += b2Vec2(0.0f, 1.0f);
            pressed = true;
        }

        if (inputManager.binding(ActionIdMoveDown)->pressed()) {
            tmp += b2Vec2(0.0f, -1.0f);
            pressed = true;
        }

        if (inputManager.binding(ActionIdMoveLeft)->pressed()) {
            tmp += b2Vec2(-1.0f, 0.0f);
            pressed = true;
        }

        if (inputManager.binding(ActionIdMoveRight)->pressed()) {
            tmp += b2Vec2(1.0f, 0.0f);
            pressed = true;
        }

        if (pressed) {
            direction = tmp;
            direction.Normalize();
        } else {
            tmp = inputManager.gamepad().pos(true);
            if (tmp != b2Vec2_zero) {
                direction = tmp;
                direction.Normalize();
                pressed = true;
            }
        }

        return pressed;
    }

    bool InputContextMechPC::primaryPressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        if (inputManager.binding(ActionIdPrimaryFire)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdPrimaryFire)->pressed()) {
            direction = b2Vec2_zero;
            return true;
        }

        return false;
    }

    bool InputContextMechPC::secondaryPressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        if (inputManager.binding(ActionIdSecondaryFire)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdSecondaryFire)->pressed()) {
            direction = b2Vec2_zero;
            return true;
        }

        return false;
    }

    bool InputContextMechPC::lookPressed(b2Vec2& pos, bool& relative) const
    {
        if (!active()) {
            return false;
        }

        pos = inputManager.lookPos(relative);

        return true;
    }

    void InputContextMechPC::doActivate()
    {
    }

    void InputContextMechPC::doDeactivate()
    {
        if (primaryKnob_) {
            primaryKnob_->parent()->removeFromParent();
            primaryKnob_.reset();
        }
        if (secondaryKnob_) {
            secondaryKnob_->parent()->removeFromParent();
            secondaryKnob_.reset();
        }
    }
}
