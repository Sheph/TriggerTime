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

#include "InputContextRoboArmPC.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextRoboArmPC::InputContextRoboArmPC(Scene* scene)
    : InputContextRoboArm(scene),
      armImage_(assetManager.getImage("factory4/roboarm_icon.png"))
    {
        lockRadius_ = settings.pc.primaryRadius;
        lockPos_ = settings.pc.primaryPadding + b2Vec2(lockRadius_, lockRadius_);

        doDeactivate();
    }

    InputContextRoboArmPC::~InputContextRoboArmPC()
    {
        doDeactivate();
    }

    void InputContextRoboArmPC::update(float dt)
    {
        if (!lockKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(lockPos_);
            lockKnob_ = boost::make_shared<KnobComponent>(-1);
            lockKnob_->setHandleRadius(lockRadius_);
            lockKnob_->setDrawRing(false);
            lockKnob_->setAlpha(settings.pc.alpha);
            lockKnob_->setImage(armImage_);
            obj->addComponent(lockKnob_);
            scene()->addObject(obj);
        }
    }

    bool InputContextRoboArmPC::movePressed(b2Vec2& direction) const
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

    bool InputContextRoboArmPC::moveToPressed(b2Vec2& pos) const
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

    bool InputContextRoboArmPC::lockPressed() const
    {
        if (!active()) {
            return false;
        }

        return inputManager.binding(ActionIdPrimaryFire)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdPrimaryFire)->pressed();
    }

    void InputContextRoboArmPC::doActivate()
    {
    }

    void InputContextRoboArmPC::doDeactivate()
    {
        if (lockKnob_) {
            lockKnob_->parent()->removeFromParent();
            lockKnob_.reset();
        }
    }
}
