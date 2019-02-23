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

#include "WeaponStationComponent.h"
#include "PlayerComponent.h"
#include "Scene.h"

namespace af
{
    WeaponStationComponent::WeaponStationComponent(WeaponType weaponType, float minAmmo)
    : PhasedComponent(phaseThink),
      weaponType_(weaponType),
      minAmmo_(minAmmo),
      snd_(audio.createSound("weapon_pickup.ogg")),
      used_(false)
    {
    }

    WeaponStationComponent::~WeaponStationComponent()
    {
    }

    void WeaponStationComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponStationComponent::update(float dt)
    {
    }

    void WeaponStationComponent::sensorEnter(const SceneObjectPtr& other)
    {
        PlayerComponentPtr player = other->findComponent<PlayerComponent>();
        if (!player) {
            return;
        }

        bool playSound = false;

        if (player->giveWeapon(weaponType_)) {
            playSound = true;
        }

        const WeaponComponentPtr& weapon = player->weapons()[WeaponComponent::getSlot(weaponType_)];

        if (!used_) {
            used_ = true;
            if (weapon->haveAmmo()) {
                player->changeAmmo(weaponType_, scene()->fixupAmmoAmount(minAmmo_));
                playSound = true;
            }
        }

        if (weapon->haveAmmo() && (weapon->ammo() < scene()->fixupAmmoAmount(minAmmo_))) {
            player->changeAmmo(weaponType_, scene()->fixupAmmoAmount(minAmmo_) - weapon->ammo());
            playSound = true;
        }

        if (playSound) {
            snd_->play();
        }
    }

    void WeaponStationComponent::sensorExit(const SceneObjectPtr& other)
    {
    }

    void WeaponStationComponent::onRegister()
    {
    }

    void WeaponStationComponent::onUnregister()
    {
    }
}
