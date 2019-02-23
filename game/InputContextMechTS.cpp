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

#include "InputContextMechTS.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextMechTS::InputContextMechTS(Scene* scene)
    : InputContextMech(scene),
      primaryImage_(assetManager.getImage("subway1/mechfist_icon.png")),
      secondaryImage_(assetManager.getImage("subway1/mechgun_icon.png"))
    {
        moveRadius_ = settings.touchScreen.moveRadius;
        movePos_ = settings.touchScreen.movePadding + b2Vec2(moveRadius_, moveRadius_);
        moveHandleRadius_ = settings.touchScreen.moveHandleRadius;

        primaryRadius_ = settings.touchScreen.primaryRadius;
        primaryPos_ = settings.touchScreen.primaryPadding + b2Vec2(primaryRadius_, primaryRadius_);
        primaryPos_.x = scene->gameWidth() - primaryPos_.x;
        primaryHandleRadius_ = settings.touchScreen.primaryHandleRadius;

        secondaryRadius_ = settings.touchScreen.secondaryRadius;
        secondaryPos_ = settings.touchScreen.secondaryPadding + b2Vec2(secondaryRadius_, secondaryRadius_);
        secondaryPos_.x = scene->gameWidth() - secondaryPos_.x;
        secondaryHandleRadius_ = settings.touchScreen.secondaryHandleRadius;

        doDeactivate();
    }

    InputContextMechTS::~InputContextMechTS()
    {
        doDeactivate();
    }

    void InputContextMechTS::update(float dt)
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

        if (!primaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(primaryPos_);
            primaryKnob_ = boost::make_shared<KnobComponent>(-1);
            primaryKnob_->setRadius(primaryRadius_);
            primaryKnob_->setHandleRadius(primaryHandleRadius_);
            primaryKnob_->setDrawRing(true);
            primaryKnob_->setAlpha(settings.touchScreen.alpha);
            primaryKnob_->setImage(primaryImage_);
            obj->addComponent(primaryKnob_);
            scene()->addObject(obj);
        }

        if (!secondaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(secondaryPos_);
            secondaryKnob_ = boost::make_shared<KnobComponent>();
            secondaryKnob_->setRadius(secondaryRadius_);
            secondaryKnob_->setHandleRadius(secondaryHandleRadius_);
            secondaryKnob_->setDrawRing(true);
            secondaryKnob_->setAlpha(settings.touchScreen.alpha);
            secondaryKnob_->setImage(secondaryImage_);
            obj->addComponent(secondaryKnob_);
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
            } else if (i == primaryFinger_) {
                if (pressed) {
                    primaryTouchPoint_ = point;
                } else {
                    primaryFinger_ = -1;
                }
            } else if (i == secondaryFinger_) {
                if (pressed) {
                    secondaryTouchPoint_ = point;
                } else {
                    secondaryFinger_ = -1;
                }
            } else if (triggered &&
                ((point - movePos_).Length() <= moveRadius_)) {
                moveFinger_ = i;
                moveDownPoint_ = moveTouchPoint_ = point;
            } else if (triggered &&
                ((point - primaryPos_).Length() <= primaryRadius_)) {
                primaryFinger_ = i;
                primaryDownPoint_ = primaryTouchPoint_ = point;
            } else if (triggered &&
                ((point - secondaryPos_).Length() <= secondaryRadius_)) {
                secondaryFinger_ = i;
                secondaryDownPoint_ = secondaryTouchPoint_ = point;
            }
        }

        b2Vec2 dir = b2Vec2_zero;

        movePressed(dir);

        moveKnob_->setHandlePos((moveRadius_ - moveHandleRadius_) * dir);

        dir = b2Vec2_zero;

        primaryPressed(dir);

        if (dir.Length() <= (primaryRadius_ - primaryHandleRadius_)) {
            primaryKnob_->setHandlePos(dir);
        } else {
            dir.Normalize();
            primaryKnob_->setHandlePos((primaryRadius_ - primaryHandleRadius_) * dir);
        }

        dir = b2Vec2_zero;

        secondaryPressed(dir);

        if (dir.Length() <= (secondaryRadius_ - secondaryHandleRadius_)) {
            secondaryKnob_->setHandlePos(dir);
        } else {
            dir.Normalize();
            secondaryKnob_->setHandlePos((secondaryRadius_ - secondaryHandleRadius_) * dir);
        }
    }

    bool InputContextMechTS::movePressed(b2Vec2& direction) const
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

    bool InputContextMechTS::primaryPressed(b2Vec2& direction) const
    {
        if (primaryFinger_ >= 0) {
            direction = primaryTouchPoint_ - primaryDownPoint_;
            if (direction.LengthSquared() < (0.5f * 0.5f)) {
                direction = b2Vec2_zero;
            }
            return true;
        }

        return false;
    }

    bool InputContextMechTS::secondaryPressed(b2Vec2& direction) const
    {
        if (secondaryFinger_ >= 0) {
            direction = secondaryTouchPoint_ - secondaryDownPoint_;
            if (direction.LengthSquared() < (0.5f * 0.5f)) {
                direction = b2Vec2_zero;
            }
            return true;
        }

        return false;
    }

    bool InputContextMechTS::lookPressed(b2Vec2& pos, bool& relative) const
    {
        return false;
    }

    void InputContextMechTS::doActivate()
    {
        moveFinger_ = -1;
        primaryFinger_ = -1;
        secondaryFinger_ = -1;
    }

    void InputContextMechTS::doDeactivate()
    {
        moveFinger_ = -1;
        primaryFinger_ = -1;
        secondaryFinger_ = -1;

        if (moveKnob_) {
            moveKnob_->parent()->removeFromParent();
            moveKnob_.reset();
        }
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
