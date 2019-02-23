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

#include "WeaponComponent.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>
#include <stdio.h>

namespace af
{
    static char ammoStrBuff[100];

    static const int WeaponTypeIntValues[] =
    {
        WeaponTypeGG,
        WeaponTypeBlaster,
        WeaponTypeShotgun,
        WeaponTypeRLauncher,
        WeaponTypeFireball,
        WeaponTypeFlamethrower,
        WeaponTypePistol,
        WeaponTypeMachineGun,
        WeaponTypeSuperBlaster,
        WeaponTypeProxMine,
        WeaponTypeChainsaw,
        WeaponTypeLGun,
        WeaponTypePlasmaGun,
        WeaponTypeAcidball,
        WeaponTypeSpitter,
        WeaponTypeSuperShotgun,
        WeaponTypeSeeker,
        WeaponTypeEShield,
        WeaponTypeRope,
        WeaponTypeHeater,
        WeaponTypeAim,
    };

    static const char* WeaponTypeStrValues[] =
    {
        "gravityGun",
        "blaster",
        "shotgun",
        "rlauncher",
        "fireball",
        "flamethrower",
        "pistol",
        "machineGun",
        "superBlaster",
        "proxmine",
        "chainsaw",
        "lgun",
        "plasmagun",
        "acidball",
        "spitter",
        "superShotgun",
        "seeker",
        "eshield",
        "rope",
        "heater",
        "aim"
    };

    AF_ENUMTRAITS_IMPL(WeaponType, WeaponType);

    static const std::string imageNames[WeaponTypeMax + 1] = {
        "common2/weapon_gg.png",
        "common2/weapon_blaster.png",
        "common2/weapon_shotgun.png",
        "common2/weapon_rlauncher.png",
        "common2/weapon_blaster.png",
        "common2/weapon_blaster.png",
        "common2/weapon_blaster.png",
        "common2/weapon_mgun.png",
        "common2/weapon_superblaster.png",
        "common2/weapon_proxmine.png",
        "common2/weapon_chainsaw.png",
        "common2/weapon_lgun.png",
        "common2/weapon_plasmagun.png",
        "common2/weapon_blaster.png",
        "common2/weapon_blaster.png",
        "common2/weapon_shotgun.png",
        "common2/weapon_blaster.png",
        "common2/weapon_eshield.png",
        "common2/weapon_blaster.png",
        "common2/weapon_blaster.png",
        "common2/weapon_blaster.png",
    };

    static const std::string ammoAssetPaths[WeaponTypeMax + 1] = {
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_shotgun.json",
        "powerup_ammo_rlauncher.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_mgun.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_proxmine.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_plasmagun.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_eshield.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
        "powerup_ammo_unknown.json",
    };

    static WeaponSlot slots[WeaponTypeMax + 1] = {
        WeaponSlotSecondary0,
        WeaponSlotPrimary1,
        WeaponSlotPrimary1,
        WeaponSlotPrimary3,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary1,
        WeaponSlotSecondary1,
        WeaponSlotPrimary0,
        WeaponSlotPrimary3,
        WeaponSlotPrimary3,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotSecondary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
        WeaponSlotPrimary2,
    };

    static bool directional[WeaponTypeMax + 1] = {
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        false,
        true,
        true,
        true,
        true,
        true,
        true,
        false,
        true,
        true,
        true
    };

    WeaponComponent::WeaponComponent(SceneObjectType objectType, WeaponType weaponType)
    : PhasedComponent(phaseThink),
      objectType_(objectType),
      weaponType_(weaponType),
      triggerHeld_(false),
      once_(false),
      cancel_(false),
      pos_(b2Vec2_zero),
      angle_(0.0f),
      haveSound_(true),
      flashDistance_(0.0f),
      flashQuad_(true),
      ammo_(NULL),
      emptyClicked_(false),
      useFilter_(false),
      useFreeze_(true),
      triggerCrosshair_(false),
      sndEmptyClick_(audio.createSound("weapon_empty.ogg"))
    {
    }

    WeaponComponent::~WeaponComponent()
    {
    }

    bool WeaponComponent::aiming() const
    {
        return false;
    }

    bool WeaponComponent::charged() const
    {
        return false;
    }

    void WeaponComponent::trigger(bool hold)
    {
        triggerHeld_ = hold;
        if (!hold) {
            emptyClicked_ = false;
        }
    }

    void WeaponComponent::triggerOnce()
    {
        triggerHeld_ = true;
        once_ = true;
    }

    void WeaponComponent::cancel()
    {
        cancel_ = true;
    }

    bool WeaponComponent::triggerHeld() const
    {
        bool held = triggerHeld_;
        if (once_) {
            once_ = false;
            triggerHeld_ = false;
            emptyClicked_ = false;
        }
        return held;
    }

    bool WeaponComponent::canceled() const
    {
        bool c = cancel_;
        cancel_ = false;
        return c;
    }

    void WeaponComponent::addToFilter(SInt32 cookie)
    {
        if (!collisionFilter_) {
            CollisionCookieFilterPtr tmp = boost::make_shared<CollisionCookieFilter>();
            tmp->add(parent()->cookie());
            collisionFilter_ = tmp;
        }

        CollisionCookieFilterPtr tmp =
            boost::static_pointer_cast<CollisionCookieFilter>(collisionFilter_);
        tmp->add(cookie);
    }

    void WeaponComponent::setTransform(const b2Transform& value)
    {
        pos_ = value.p;
        angle_ = value.q.GetAngle();
    }

    const char* WeaponComponent::ammoStr() const
    {
        if (ammo_ != NULL) {
            sprintf(&ammoStrBuff[0], "%d", static_cast<int>(*ammo_));
        } else {
            strcpy(&ammoStrBuff[0], "∞");
        }
        return &ammoStrBuff[0];
    }

    const std::string& WeaponComponent::getImageName(WeaponType type)
    {
        return imageNames[type];
    }

    Image WeaponComponent::getImage(WeaponType type)
    {
        return assetManager.getImage(getImageName(type));
    }

    const std::string& WeaponComponent::getAmmoAssetPath(WeaponType type)
    {
        return ammoAssetPaths[type];
    }

    WeaponSlot WeaponComponent::getSlot(WeaponType type)
    {
        return slots[type];
    }

    bool WeaponComponent::isDirectional(WeaponType type)
    {
        return directional[type];
    }

    bool WeaponComponent::changeAmmo(float amount, bool quiet)
    {
        if (!ammo_) {
            return true;
        }

        if (*ammo_ < amount) {
            if (!emptyClicked_) {
                emptyClicked_ = true;
                if (!quiet) {
                    sndEmptyClick_->play();
                }
            }

            return false;
        }

        *ammo_ -= amount;

        return true;
    }

    void WeaponComponent::setupShot(const SceneObjectPtr& obj)
    {
        if (!useFreeze_) {
            obj->setFreezable(false);
        }

        if (!useFilter_) {
            return;
        }

        if (!collisionFilter_) {
            CollisionCookieFilterPtr tmp = boost::make_shared<CollisionCookieFilter>();
            tmp->add(parent()->cookie());
            collisionFilter_ = tmp;
        }

        obj->setCollisionFilter(collisionFilter_);
    }

    void WeaponComponent::updateCrosshair()
    {
        if (triggerCrosshair_) {
            scene()->crosshair()->trigger();
        }
    }
}
