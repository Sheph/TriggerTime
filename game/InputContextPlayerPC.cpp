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

#include "InputContextPlayerPC.h"
#include "InputManager.h"
#include "Settings.h"
#include "Scene.h"
#include "PlayerComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    InputContextPlayerPC::InputContextPlayerPC(Scene* scene)
    : InputContextPlayer(scene)
    {
        primaryRadius_ = settings.pc.primaryRadius;
        primaryPos_ = settings.pc.primaryPadding + b2Vec2(primaryRadius_, primaryRadius_);

        secondaryRadius_ = settings.pc.secondaryRadius;
        secondaryPos_ = settings.pc.secondaryPadding + b2Vec2(secondaryRadius_, secondaryRadius_);

        doDeactivate();
    }

    InputContextPlayerPC::~InputContextPlayerPC()
    {
        doDeactivate();
    }

    void InputContextPlayerPC::update(float dt)
    {
        PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();

        if (pc->weapon() && !primaryKnob_) {
            SceneObjectPtr obj = boost::make_shared<SceneObject>();
            obj->setPos(primaryPos_);
            primaryKnob_ = boost::make_shared<KnobComponent>(-1);
            primaryKnob_->setHandleRadius(primaryRadius_);
            primaryKnob_->setDrawRing(false);
            primaryKnob_->setAlpha(settings.pc.alpha);
            primaryKnob_->setTextSize(settings.pc.primaryTextSize);
            primaryKnob_->setTextPos(settings.pc.primaryTextPos);
            primaryKnob_->setVisible(settings.hud.enabled);
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
            secondaryKnob_->setHandleRadius(secondaryRadius_);
            secondaryKnob_->setDrawRing(false);
            secondaryKnob_->setAlpha(settings.pc.alpha);
            secondaryKnob_->setTextSize(settings.pc.secondaryTextSize);
            secondaryKnob_->setTextPos(settings.pc.secondaryTextPos);
            secondaryKnob_->setVisible(settings.hud.enabled);
            obj->addComponent(secondaryKnob_);
            scene()->addObject(obj);
        } else if (!pc->altWeapon() && secondaryKnob_) {
            secondaryKnob_->parent()->removeFromParent();
            secondaryKnob_.reset();
        }

        if (inputManager.binding(ActionIdPrimarySwitch)->triggered() ||
            inputManager.gamepadBinding(ActionGamepadIdPrimarySwitch)->triggered()) {
            for (int i = 0; i < WeaponSlotSecondary0 - 1; ++i) {
                WeaponSlot slot = static_cast<WeaponSlot>((pc->primarySlot() + i + 1) % WeaponSlotSecondary0);
                if (pc->weapons()[slot]) {
                    pc->switchWeapon(slot);
                    if (primarySwitchShowOff()) {
                        primaryKnob_->setShowOff(false);
                    }
                    setPrimarySwitchShowOff(false);
                    break;
                }
            }
        }

        if (inputManager.binding(ActionIdSecondarySwitch)->triggered() ||
            inputManager.gamepadBinding(ActionGamepadIdSecondarySwitch)->triggered()) {
            for (int i = 0; i < WeaponSlotMax - WeaponSlotSecondary0; ++i) {
                WeaponSlot slot = static_cast<WeaponSlot>(WeaponSlotSecondary0 +
                    (pc->secondarySlot() - WeaponSlotSecondary0 + i + 1) % (WeaponSlotMax - WeaponSlotSecondary0 + 1));
                if (pc->weapons()[slot]) {
                    pc->switchWeapon(slot);
                    if (secondarySwitchShowOff()) {
                        secondaryKnob_->setShowOff(false);
                    }
                    setSecondarySwitchShowOff(false);
                    break;
                }
            }
        }

        if (primaryKnob_) {
            primaryKnob_->setText(pc->weapon()->ammoStr());
            primaryKnob_->setImage(
                WeaponComponent::getImage(pc->weapon()->weaponType()));

            if (!primaryKnob_->showOff() && primarySwitchShowOff()) {
                primaryKnob_->setShowOff(true);
            }
        }

        if (secondaryKnob_) {
            b2Vec2 dir;

            if (secondaryShowOff() && secondaryPressed(dir)) {
                secondaryKnob_->setShowOff(false);
                setSecondaryShowOff(false);
            }

            if (!secondaryKnob_->showOff() && (secondaryShowOff() || secondarySwitchShowOff())) {
                secondaryKnob_->setShowOff(true);
            }

            secondaryKnob_->setText(pc->altWeapon()->ammoStr());
            secondaryKnob_->setImage(
                WeaponComponent::getImage(pc->altWeapon()->weaponType()));
        }
    }

    bool InputContextPlayerPC::movePressed(b2Vec2& direction) const
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

    bool InputContextPlayerPC::primaryPressed(b2Vec2& direction) const
    {
        if (!primaryKnob_) {
            return false;
        }

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

    bool InputContextPlayerPC::secondaryPressed(b2Vec2& direction) const
    {
        if (!secondaryKnob_) {
            return false;
        }

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

    bool InputContextPlayerPC::lookPressed(b2Vec2& pos, bool& relative) const
    {
        if (!active()) {
            return false;
        }

        pos = inputManager.lookPos(relative);

        return true;
    }

    bool InputContextPlayerPC::runPressed() const
    {
        if (!active()) {
            return false;
        }

        if (inputManager.binding(ActionIdRun)->pressed() ||
            inputManager.gamepadBinding(ActionGamepadIdRun)->pressed()) {
            return true;
        }

        return false;
    }

    bool InputContextPlayerPC::suicidePressed() const
    {
        if (settings.debugKeys) {
            return active() && inputManager.keyboard().triggered(KI_K);
        } else {
            return false;
        }
    }

    bool InputContextPlayerPC::ghostTriggered() const
    {
        if (settings.debugKeys) {
            return active() && inputManager.keyboard().triggered(KI_G);
        } else {
            return false;
        }
    }

    void InputContextPlayerPC::doActivate()
    {
    }

    void InputContextPlayerPC::doDeactivate()
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
