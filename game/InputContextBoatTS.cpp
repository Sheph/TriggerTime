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

#include "InputContextBoatTS.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextBoatTS::InputContextBoatTS(Scene* scene)
    : InputContextBoat(scene),
      shootImage_(assetManager.getImage("prison1/lightning_icon.png"))
    {
        moveRadius_ = settings.touchScreen.moveRadius;
        movePos_ = settings.touchScreen.movePadding + b2Vec2(moveRadius_, moveRadius_);
        moveHandleRadius_ = settings.touchScreen.moveHandleRadius;

        shootRadius_ = settings.touchScreen.primaryRadius;
        shootPos_ = settings.touchScreen.primaryPadding + b2Vec2(shootRadius_, shootRadius_);
        shootPos_.x = scene->gameWidth() - shootPos_.x;
        shootHandleRadius_ = settings.touchScreen.primaryHandleRadius;

        doDeactivate();
    }

    InputContextBoatTS::~InputContextBoatTS()
    {
        doDeactivate();
    }

    void InputContextBoatTS::update(float dt)
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

        if (!shootKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(shootPos_);
            shootKnob_ = boost::make_shared<KnobComponent>(-1);
            shootKnob_->setRadius(shootRadius_);
            shootKnob_->setHandleRadius(shootHandleRadius_);
            shootKnob_->setDrawRing(true);
            shootKnob_->setAlpha(settings.touchScreen.alpha);
            shootKnob_->setImage(shootImage_);
            obj->addComponent(shootKnob_);
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
            } else if (i == shootFinger_) {
                if (pressed) {
                    shootTouchPoint_ = point;
                } else {
                    shootFinger_ = -1;
                }
            } else if (triggered &&
                ((point - movePos_).Length() <= moveRadius_)) {
                moveFinger_ = i;
                moveDownPoint_ = moveTouchPoint_ = point;
            } else if (triggered &&
                ((point - shootPos_).Length() <= shootRadius_)) {
                shootFinger_ = i;
                shootDownPoint_ = shootTouchPoint_ = point;
            }
        }

        b2Vec2 dir = b2Vec2_zero;

        movePressed(dir);

        moveKnob_->setHandlePos((moveRadius_ - moveHandleRadius_) * dir);

        dir = b2Vec2_zero;

        shootPressed(dir);

        if (dir.Length() <= (shootRadius_ - shootHandleRadius_)) {
            shootKnob_->setHandlePos(dir);
        } else {
            dir.Normalize();
            shootKnob_->setHandlePos((shootRadius_ - shootHandleRadius_) * dir);
        }
    }

    bool InputContextBoatTS::movePressed(b2Vec2& direction) const
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

    bool InputContextBoatTS::turnLeftPressed() const
    {
        return false;
    }

    bool InputContextBoatTS::turnRightPressed() const
    {
        return false;
    }

    bool InputContextBoatTS::forwardPressed() const
    {
        return false;
    }

    bool InputContextBoatTS::backPressed() const
    {
        return false;
    }

    bool InputContextBoatTS::lookPressed(b2Vec2& pos, bool& relative) const
    {
        return false;
    }

    bool InputContextBoatTS::shootPressed(b2Vec2& direction) const
    {
        if (shootFinger_ >= 0) {
            direction = shootTouchPoint_ - shootDownPoint_;
            if (direction.LengthSquared() < (0.5f * 0.5f)) {
                direction = b2Vec2_zero;
            }
            return true;
        }

        return false;
    }

    void InputContextBoatTS::doActivate()
    {
        moveFinger_ = -1;
        shootFinger_ = -1;
    }

    void InputContextBoatTS::doDeactivate()
    {
        moveFinger_ = -1;
        shootFinger_ = -1;

        if (moveKnob_) {
            moveKnob_->parent()->removeFromParent();
            moveKnob_.reset();
        }
        if (shootKnob_) {
            shootKnob_->parent()->removeFromParent();
            shootKnob_.reset();
        }
    }
}
