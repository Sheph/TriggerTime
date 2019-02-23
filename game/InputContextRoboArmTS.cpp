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

#include "InputContextRoboArmTS.h"
#include "InputManager.h"
#include "Settings.h"
#include "Scene.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextRoboArmTS::InputContextRoboArmTS(Scene* scene)
    : InputContextRoboArm(scene),
      armImage_(assetManager.getImage("factory4/roboarm_icon.png"))
    {
        moveRadius_ = settings.touchScreen.moveRadius;
        movePos_ = settings.touchScreen.movePadding + b2Vec2(moveRadius_, moveRadius_);
        moveHandleRadius_ = settings.touchScreen.moveHandleRadius;

        lockRadius_ = settings.touchScreen.primaryHandleRadius;
        lockPos_ = settings.touchScreen.primaryPadding + b2Vec2(lockRadius_, lockRadius_);
        lockPos_.x = scene->gameWidth() - lockPos_.x;

        doDeactivate();
    }

    InputContextRoboArmTS::~InputContextRoboArmTS()
    {
        doDeactivate();
    }

    void InputContextRoboArmTS::update(float dt)
    {
        if (!moveKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(movePos_);
            moveKnob_ = boost::make_shared<KnobComponent>();
            moveKnob_->setRadius(moveRadius_);
            moveKnob_->setHandleRadius(moveHandleRadius_);
            moveKnob_->setDrawRing(true);
            moveKnob_->setAlpha(settings.touchScreen.alpha);
            obj->addComponent(moveKnob_);
            scene()->addObject(obj);
        }

        if (!lockKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(lockPos_);
            lockKnob_ = boost::make_shared<KnobComponent>(-1);
            lockKnob_->setHandleRadius(lockRadius_);
            lockKnob_->setDrawRing(false);
            lockKnob_->setAlpha(settings.touchScreen.alpha);
            lockKnob_->setImage(armImage_);
            obj->addComponent(lockKnob_);
            scene()->addObject(obj);
        }

        for (int i = 0; i < InputMaxFingers; ++i) {
            b2Vec2 point;
            bool pressed = inputManager.touchScreen().pressed(i, &point);
            bool triggered = inputManager.touchScreen().triggered(i);

            if (i == moveFinger_) {
                if (pressed) {
                    moveTouchPoint_ = point;
                } else {
                    moveFinger_ = -1;
                }
            } else if (i == lockFinger_) {
                if (!pressed) {
                    lockFinger_ = -1;
                }
            } else if (triggered &&
                ((point - movePos_).Length() <= moveRadius_)) {
                moveFinger_ = i;
                moveDownPoint_ = moveTouchPoint_ = point;
            } else if (triggered &&
                ((point - lockPos_).Length() <= lockRadius_)) {
                lockFinger_ = i;
            }
        }

        b2Vec2 dir = b2Vec2_zero;

        movePressed(dir);

        moveKnob_->setHandlePos((moveRadius_ - moveHandleRadius_) * dir);
    }

    bool InputContextRoboArmTS::movePressed(b2Vec2& direction) const
    {
        if ((moveFinger_ >= 0) && (moveTouchPoint_ != movePos_)) {
            b2Vec2 tmp = moveTouchPoint_ - movePos_;
            if (tmp.LengthSquared() >= (1.0f * 1.0f)) {
                float maxRadius = moveRadius_ - moveHandleRadius_;
                float len = tmp.Normalize();

                if (len > maxRadius) {
                    len = maxRadius;
                }

                direction = (len / maxRadius) * tmp;
                return true;
            }
        }

        return false;
    }

    bool InputContextRoboArmTS::moveToPressed(b2Vec2& pos) const
    {
        return false;
    }

    bool InputContextRoboArmTS::lockPressed() const
    {
        return (lockFinger_ >= 0);
    }

    void InputContextRoboArmTS::doActivate()
    {
        moveFinger_ = -1;
        lockFinger_ = -1;
    }

    void InputContextRoboArmTS::doDeactivate()
    {
        moveFinger_ = -1;
        lockFinger_ = -1;

        if (moveKnob_) {
            moveKnob_->parent()->removeFromParent();
            moveKnob_.reset();
        }
        if (lockKnob_) {
            lockKnob_->parent()->removeFromParent();
            lockKnob_.reset();
        }
    }
}
