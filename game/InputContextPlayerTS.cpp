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

#include "InputContextPlayerTS.h"
#include "InputManager.h"
#include "Settings.h"
#include "Scene.h"
#include "AssetManager.h"
#include "PlayerComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextPlayerTS::InputContextPlayerTS(Scene* scene)
    : InputContextPlayer(scene),
      switchImage_(assetManager.getImage("common2/weapon_switch.png"))
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

        switchRadius_ = settings.touchScreen.switchRadius;
        switchPos_ = settings.touchScreen.switchPadding + b2Vec2(switchRadius_, switchRadius_);
        switchPos_.x = scene->gameWidth() - switchPos_.x;

        slotRadius_ = settings.touchScreen.slotRadius;
        slotDir_ = settings.touchScreen.slotDir;
        primarySlotPos_ = settings.touchScreen.slotPrimaryPadding + b2Vec2(slotRadius_, slotRadius_);
        primarySlotPos_.x = scene->gameWidth() - primarySlotPos_.x;
        secondarySlotPos_ = settings.touchScreen.slotSecondaryPadding + b2Vec2(slotRadius_, slotRadius_);
        secondarySlotPos_.x = scene->gameWidth() - secondarySlotPos_.x;

        doDeactivate();
    }

    InputContextPlayerTS::~InputContextPlayerTS()
    {
        doDeactivate();
    }

    void InputContextPlayerTS::update(float dt)
    {
        if (switch_) {
            updateSwitch(dt);
        } else {
            updateGame(dt);
        }
    }

    bool InputContextPlayerTS::movePressed(b2Vec2& direction) const
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

    bool InputContextPlayerTS::primaryPressed(b2Vec2& direction) const
    {
        if (!primaryKnob_) {
            return false;
        }

        if (primaryFinger_ >= 0) {
            direction = primaryTouchPoint_ - primaryDownPoint_;
            if (direction.LengthSquared() < (0.5f * 0.5f)) {
                direction = b2Vec2_zero;
            }
            return (primaryTimeout_ <= 0.0f);
        }

        return false;
    }

    bool InputContextPlayerTS::secondaryPressed(b2Vec2& direction) const
    {
        if (!secondaryKnob_) {
            return false;
        }

        if (secondaryFinger_ >= 0) {
            direction = secondaryTouchPoint_ - secondaryDownPoint_;
            if (direction.LengthSquared() < (0.5f * 0.5f)) {
                direction = b2Vec2_zero;
            }
            return (secondaryTimeout_ <= 0.0f);
        }

        return false;
    }

    bool InputContextPlayerTS::lookPressed(b2Vec2& pos, bool& relative) const
    {
        return false;
    }

    bool InputContextPlayerTS::runPressed() const
    {
        return false;
    }

    bool InputContextPlayerTS::suicidePressed() const
    {
        return active() && inputManager.keyboard().triggered(KI_K);
    }

    bool InputContextPlayerTS::ghostTriggered() const
    {
        return false;
    }

    void InputContextPlayerTS::doActivate()
    {
        moveFinger_ = -1;
        primaryFinger_ = -1;
        secondaryFinger_ = -1;
        primaryTimeout_ = 0.0f;
        secondaryTimeout_ = 0.0f;
    }

    void InputContextPlayerTS::doDeactivate()
    {
        if (scene()->player()) {
            PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();

            if (pc->weapon()) {
                pc->weapon()->trigger(false);
                pc->weapon()->cancel();
            }

            if (pc->altWeapon()) {
                pc->altWeapon()->trigger(false);
                pc->altWeapon()->cancel();
            }
        }

        moveFinger_ = -1;
        primaryFinger_ = -1;
        secondaryFinger_ = -1;
        primaryTimeout_ = 0.0f;
        secondaryTimeout_ = 0.0f;
        switch_ = false;

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
        if (switchKnob_) {
            switchKnob_->parent()->removeFromParent();
            switchKnob_.reset();
        }
        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            WeaponSlot slot = static_cast<WeaponSlot>(i);
            if (slots_[slot]) {
                slots_[slot]->parent()->removeFromParent();
                slots_[slot].reset();
            }
        }
    }

    void InputContextPlayerTS::updateGame(float dt)
    {
        PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();

        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            WeaponSlot slot = static_cast<WeaponSlot>(i);
            if (slots_[slot]) {
                slots_[slot]->parent()->removeFromParent();
                slots_[slot].reset();
            }
        }

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

        if (pc->weapon() && !primaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(primaryPos_);
            primaryKnob_ = boost::make_shared<KnobComponent>(-1);
            primaryKnob_->setRadius(primaryRadius_);
            primaryKnob_->setHandleRadius(primaryHandleRadius_);
            primaryKnob_->setAlpha(settings.touchScreen.alpha);
            primaryKnob_->setTextSize(settings.touchScreen.primaryTextSize);
            primaryKnob_->setTextPos(settings.touchScreen.primaryTextPos);
            obj->addComponent(primaryKnob_);
            scene()->addObject(obj);
        } else if (!pc->weapon() && primaryKnob_) {
            primaryKnob_->parent()->removeFromParent();
            primaryKnob_.reset();
        }

        if (pc->altWeapon() && !secondaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(secondaryPos_);
            secondaryKnob_ = boost::make_shared<KnobComponent>();
            secondaryKnob_->setRadius(secondaryRadius_);
            secondaryKnob_->setHandleRadius(secondaryHandleRadius_);
            secondaryKnob_->setAlpha(settings.touchScreen.alpha);
            secondaryKnob_->setTextSize(settings.touchScreen.secondaryTextSize);
            secondaryKnob_->setTextPos(settings.touchScreen.secondaryTextPos);
            obj->addComponent(secondaryKnob_);
            scene()->addObject(obj);
        } else if (!pc->altWeapon() && secondaryKnob_) {
            secondaryKnob_->parent()->removeFromParent();
            secondaryKnob_.reset();
        }

        int numPrimaryWeapons = 0;
        int numSecondaryWeapons = 0;
        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            if (pc->weapons()[i]) {
                if (weaponSlotPrimary(static_cast<WeaponSlot>(i))) {
                    ++numPrimaryWeapons;
                } else {
                    ++numSecondaryWeapons;
                }
            }
        }

        bool haveSwitch = (numPrimaryWeapons >= 2) || (numSecondaryWeapons >= 2);

        if (haveSwitch && !switchKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(switchPos_);
            switchKnob_ = boost::make_shared<KnobComponent>();
            switchKnob_->setHandleRadius(switchRadius_);
            switchKnob_->setDrawRing(false);
            switchKnob_->setAlpha(settings.touchScreen.switchAlpha);
            switchKnob_->setImage(switchImage_);
            obj->addComponent(switchKnob_);
            scene()->addObject(obj);
        } else if (!haveSwitch && switchKnob_) {
            switchKnob_->parent()->removeFromParent();
            switchKnob_.reset();
        }

        primaryTimeout_ -= dt;
        secondaryTimeout_ -= dt;

        bool primaryDirectional = false;
        bool secondaryDirectional = false;

        if (pc->weapon()) {
            primaryDirectional = WeaponComponent::isDirectional(pc->weapon()->weaponType());
        }
        if (pc->altWeapon()) {
            secondaryDirectional = WeaponComponent::isDirectional(pc->altWeapon()->weaponType()) && !pc->altWeapon()->charged();
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
                    if (primaryDirectional) {
                        primaryTouchPoint_ = point;
                    }
                } else {
                    primaryFinger_ = -1;
                }
            } else if (i == secondaryFinger_) {
                if (pressed) {
                    if (secondaryDirectional) {
                        secondaryTouchPoint_ = point;
                    }
                } else {
                    secondaryFinger_ = -1;
                }
            } else if (triggered &&
                ((point - movePos_).Length() <= moveRadius_)) {
                moveFinger_ = i;
                moveDownPoint_ = moveTouchPoint_ = point;
            } else if (triggered &&
                ((point - primaryPos_).Length() <= primaryHandleRadius_)) {
                primaryFinger_ = i;
                primaryDownPoint_ = primaryTouchPoint_ = point;
                if (primaryDirectional) {
                    if (pc->weapon() && (pc->weapon()->weaponType() == WeaponTypeRLauncher)) {
                        primaryTimeout_ = 0.6f;
                    } else {
                        primaryTimeout_ = 0.1f;
                    }
                } else {
                    primaryTimeout_ = 0.0f;
                }
            } else if (triggered &&
                ((point - secondaryPos_).Length() <= secondaryHandleRadius_)) {
                secondaryFinger_ = i;
                secondaryDownPoint_ = secondaryTouchPoint_ = point;
                if (secondaryDirectional) {
                    secondaryTimeout_ = 0.1f;
                } else {
                    secondaryTimeout_ = 0.0f;
                }
            } else if (triggered && switchKnob_ &&
                ((point - switchPos_).Length() <= switchRadius_) &&
                scene()->player()->alive()) {
                switch_ = true;
                switchKnob_->setShowOff(false);
                setPrimarySwitchShowOff(false);
                setSecondarySwitchShowOff(false);
            }
        }

        if (switchKnob_ && !switchKnob_->showOff() && (primarySwitchShowOff() || secondarySwitchShowOff())) {
            switchKnob_->setShowOff(true);
        }

        b2Vec2 dir = b2Vec2_zero;

        movePressed(dir);

        moveKnob_->setHandlePos((moveRadius_ - moveHandleRadius_) * dir);

        if (primaryKnob_) {
            dir = b2Vec2_zero;

            if (primaryDirectional) {
                primaryPressed(dir);

                if (dir.Length() <= (primaryRadius_ - primaryHandleRadius_)) {
                    primaryKnob_->setHandlePos(dir);
                } else {
                    dir.Normalize();
                    primaryKnob_->setHandlePos((primaryRadius_ - primaryHandleRadius_) * dir);
                }
                primaryKnob_->setDrawRing(true);
            } else {
                primaryKnob_->setHandlePos(dir);
                primaryKnob_->setDrawRing(false);
            }

            primaryKnob_->setText(pc->weapon()->ammoStr());
            primaryKnob_->setImage(
                WeaponComponent::getImage(pc->weapon()->weaponType()));
        }

        if (secondaryKnob_) {
            if (secondaryFinger_ != -1) {
                secondaryKnob_->setShowOff(false);
                setSecondaryShowOff(false);
            } else if (!secondaryKnob_->showOff() && secondaryShowOff()) {
                secondaryKnob_->setShowOff(true);
            }

            dir = b2Vec2_zero;

            if (secondaryDirectional) {
                secondaryPressed(dir);

                if (dir.Length() <= (secondaryRadius_ - secondaryHandleRadius_)) {
                    secondaryKnob_->setHandlePos(dir);
                } else {
                    dir.Normalize();
                    secondaryKnob_->setHandlePos((secondaryRadius_ - secondaryHandleRadius_) * dir);
                }
                secondaryKnob_->setDrawRing(true);
            } else {
                secondaryKnob_->setHandlePos(dir);
                secondaryKnob_->setDrawRing(false);
            }

            secondaryKnob_->setText(pc->altWeapon()->ammoStr());
            secondaryKnob_->setImage(
                WeaponComponent::getImage(pc->altWeapon()->weaponType()));
        }
    }

    void InputContextPlayerTS::updateSwitch(float dt)
    {
        PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();

        if (scene()->player()->dead()) {
            switch_ = false;
            return;
        }

        if (switchKnob_) {
            if (pc->weapon()) {
                pc->weapon()->trigger(false);
                pc->weapon()->cancel();
            }

            if (pc->altWeapon()) {
                pc->altWeapon()->trigger(false);
                pc->altWeapon()->cancel();
            }

            primaryFinger_ = -1;
            secondaryFinger_ = -1;
            primaryTimeout_ = 0.0f;
            secondaryTimeout_ = 0.0f;

            if (primaryKnob_) {
                primaryKnob_->parent()->removeFromParent();
                primaryKnob_.reset();
            }
            if (secondaryKnob_) {
                secondaryKnob_->parent()->removeFromParent();
                secondaryKnob_.reset();
            }
            if (switchKnob_) {
                switchKnob_->parent()->removeFromParent();
                switchKnob_.reset();
            }
        }

        b2Vec2 dir = b2Vec2_zero;

        for (int i = 0; i < WeaponSlotSecondary0; ++i) {
            WeaponSlot slot = static_cast<WeaponSlot>((pc->primarySlot() + i) % WeaponSlotSecondary0);

            if (pc->weapons()[slot] && !slots_[slot]) {
                SceneObjectPtr obj = boost::make_shared<SceneObject>();
                slots_[slot] = boost::make_shared<KnobComponent>();

                if (i == 0) {
                    obj->setPos(primaryPos_);
                    slots_[slot]->setHandleRadius(primaryHandleRadius_);
                    slots_[slot]->setTextSize(settings.touchScreen.primaryTextSize);
                    slots_[slot]->setTextPos(settings.touchScreen.primaryTextPos);
                } else {
                    obj->setPos(primarySlotPos_ + dir);
                    slots_[slot]->setHandleRadius(slotRadius_);
                    slots_[slot]->setTextSize(settings.touchScreen.slotTextSize);
                    slots_[slot]->setTextPos(settings.touchScreen.slotTextPos);
                    dir += slotDir_;
                }

                slots_[slot]->setDrawRing(false);
                slots_[slot]->setAlpha(settings.touchScreen.alpha);
                obj->addComponent(slots_[slot]);
                scene()->addObject(obj);
            } else if (!pc->weapons()[slot] && slots_[slot]) {
                slots_[slot]->parent()->removeFromParent();
                slots_[slot].reset();
            }
        }

        dir = b2Vec2_zero;

        for (int i = 0; i < WeaponSlotMax - WeaponSlotSecondary0 + 1; ++i) {
            WeaponSlot slot = static_cast<WeaponSlot>(WeaponSlotSecondary0 +
                (pc->secondarySlot() - WeaponSlotSecondary0 + i) % (WeaponSlotMax - WeaponSlotSecondary0 + 1));

            if (pc->weapons()[slot] && !slots_[slot]) {
                SceneObjectPtr obj = boost::make_shared<SceneObject>();
                slots_[slot] = boost::make_shared<KnobComponent>();

                if (i == 0) {
                    obj->setPos(secondaryPos_);
                    slots_[slot]->setHandleRadius(secondaryHandleRadius_);
                    slots_[slot]->setTextSize(settings.touchScreen.secondaryTextSize);
                    slots_[slot]->setTextPos(settings.touchScreen.secondaryTextPos);
                } else {
                    obj->setPos(secondarySlotPos_ + dir);
                    slots_[slot]->setHandleRadius(slotRadius_);
                    slots_[slot]->setTextSize(settings.touchScreen.slotTextSize);
                    slots_[slot]->setTextPos(settings.touchScreen.slotTextPos);
                    dir += slotDir_;
                }

                slots_[slot]->setDrawRing(false);
                slots_[slot]->setAlpha(settings.touchScreen.alpha);
                obj->addComponent(slots_[slot]);
                scene()->addObject(obj);
            } else if (!pc->weapons()[slot] && slots_[slot]) {
                slots_[slot]->parent()->removeFromParent();
                slots_[slot].reset();
            }
        }

        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            WeaponSlot slot = static_cast<WeaponSlot>(i);
            if (slots_[slot]) {
                slots_[slot]->setText(pc->weapons()[slot]->ammoStr());
                slots_[slot]->setImage(
                    WeaponComponent::getImage(pc->weapons()[slot]->weaponType()));
            }
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
            } else if (triggered &&
                ((point - movePos_).Length() <= moveRadius_)) {
                moveFinger_ = i;
                moveDownPoint_ = moveTouchPoint_ = point;
            } else if (triggered) {
                for (int i = 0; i < WeaponSlotMax + 1; ++i) {
                    WeaponSlot slot = static_cast<WeaponSlot>(i);
                    if (slots_[slot] && ((point - slots_[slot]->parent()->pos()).Length() <= slots_[slot]->handleRadius())) {
                        pc->switchWeapon(slot);
                        switch_ = false;
                        break;
                    }
                }
            }
        }
    }
}
