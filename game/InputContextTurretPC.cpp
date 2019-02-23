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

#include "InputContextTurretPC.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextTurretPC::InputContextTurretPC(Scene* scene)
    : InputContextTurret(scene)
    {
        shootRadius_ = settings.pc.primaryRadius;
        shootPos_ = settings.pc.primaryPadding + b2Vec2(shootRadius_, shootRadius_);

        doDeactivate();
    }

    InputContextTurretPC::~InputContextTurretPC()
    {
        doDeactivate();
    }

    void InputContextTurretPC::update(float dt)
    {
        if (!shootKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(shootPos_);
            shootKnob_ = boost::make_shared<KnobComponent>(-1);
            shootKnob_->setHandleRadius(shootRadius_);
            shootKnob_->setDrawRing(false);
            shootKnob_->setAlpha(settings.pc.alpha);
            shootKnob_->setImage(shootImage());
            obj->addComponent(shootKnob_);
            scene()->addObject(obj);
        }
    }

    bool InputContextTurretPC::movePressed(b2Vec2& direction) const
    {
        if (!active()) {
            return false;
        }

        bool relative = false;

        b2Vec2 tmp = inputManager.lookPos(relative);

        if (relative && (inputManager.gamepad().pos(false) != b2Vec2_zero)) {
            direction = tmp;
            return true;
        } else {
            return false;
        }
    }

    bool InputContextTurretPC::moveToPressed(b2Vec2& pos) const
    {
        if (!active()) {
            return false;
        }

        bool relative = false;

        b2Vec2 tmp = inputManager.lookPos(relative);

        if (relative) {
            return false;
        } else {
            pos = tmp;
            return true;
        }
    }

    bool InputContextTurretPC::shootPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdPrimaryFire)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdPrimaryFire)->pressed();
    }

    void InputContextTurretPC::doActivate()
    {
    }

    void InputContextTurretPC::doDeactivate()
    {
        if (shootKnob_) {
            shootKnob_->parent()->removeFromParent();
            shootKnob_.reset();
        }
    }
}
