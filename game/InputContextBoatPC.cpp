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

#include "InputContextBoatPC.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextBoatPC::InputContextBoatPC(Scene* scene)
    : InputContextBoat(scene),
      shootImage_(assetManager.getImage("prison1/lightning_icon.png"))
    {
        shootRadius_ = settings.pc.primaryRadius;
        shootPos_ = settings.pc.primaryPadding + b2Vec2(shootRadius_, shootRadius_);

        doDeactivate();
    }

    InputContextBoatPC::~InputContextBoatPC()
    {
        doDeactivate();
    }

    void InputContextBoatPC::update(float dt)
    {
        if (!shootKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(shootPos_);
            shootKnob_ = boost::make_shared<KnobComponent>(-1);
            shootKnob_->setHandleRadius(shootRadius_);
            shootKnob_->setDrawRing(false);
            shootKnob_->setAlpha(settings.pc.alpha);
            shootKnob_->setImage(shootImage_);
            obj->addComponent(shootKnob_);
            scene()->addObject(obj);
        }
    }

    bool InputContextBoatPC::movePressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        b2Vec2 tmp = inputManager.gamepad().pos(true);

        if (tmp != b2Vec2_zero) {
            direction = tmp;
            return true;
        } else {
            return false;
        }
    }

    bool InputContextBoatPC::turnLeftPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveLeft)->pressed();
    }

    bool InputContextBoatPC::turnRightPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveRight)->pressed();
    }

    bool InputContextBoatPC::forwardPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveUp)->pressed();
    }

    bool InputContextBoatPC::backPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdMoveDown)->pressed();
    }

    bool InputContextBoatPC::lookPressed(b2Vec2& pos, bool& relative) const
    {
        if (!active()) {
            return false;
        }

        pos = inputManager.lookPos(relative);

        return true;
    }

    bool InputContextBoatPC::shootPressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        direction = b2Vec2_zero;

        return inputManager.binding(ActionIdPrimaryFire)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdPrimaryFire)->pressed();
    }

    void InputContextBoatPC::doActivate()
    {
    }

    void InputContextBoatPC::doDeactivate()
    {
        if (shootKnob_) {
            shootKnob_->parent()->removeFromParent();
            shootKnob_.reset();
        }
    }
}
