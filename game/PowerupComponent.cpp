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

#include "PowerupComponent.h"
#include "SingleTweening.h"
#include "RenderComponent.h"
#include "PlayerComponent.h"
#include "AssetManager.h"
#include "Const.h"
#include "AudioManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    static const int PowerupTypeIntValues[] =
    {
        PowerupTypeHealth,
        PowerupTypeGem,
        PowerupTypeInventory,
        PowerupTypeHelmet,
        PowerupTypeAmmo,
        PowerupTypeBackpack,
        PowerupTypeEarpiece,
    };

    static const char* PowerupTypeStrValues[] =
    {
        "health",
        "gem",
        "inventory",
        "helmet",
        "ammo",
        "backpack",
        "earpiece",
    };

    AF_ENUMTRAITS_IMPL(PowerupType, PowerupType);

    PowerupComponent::PowerupComponent(const CollisionDieComponentPtr& cdc,
        float timeout, float radius, const Color& color)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      haveTimeout_(timeout > 0.0f),
      timeout_(timeout),
      radius_(radius),
      color_(color),
      tweenTime_(0.0f)
    {
    }

    PowerupComponent::~PowerupComponent()
    {
    }

    void PowerupComponent::update(float dt)
    {
        if (pec_) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        timeout_ -= dt;

        if (tweening_) {
            float value = tweening_->getValue(tweenTime_);

            Color color = color_;
            color.rgba[3] = value * color_.rgba[3];

            light_->setColor(color);

            color = rcColor_;
            color.rgba[3] = value * rcColor_.rgba[3];

            rc_->setColor(color);

            tweenTime_ += dt;

            if (tweening_->finished(tweenTime_)) {
                parent()->removeFromParent();
                return;
            }
        }

        if (haveTimeout_ && (timeout_ <= 0.0f) && !tweening_) {
            SingleTweeningPtr tweening =
                boost::make_shared<SingleTweening>(1.0f, EaseInQuad, 1.0f, 0.0f);
            tweening_ = tweening;
            tweenTime_ = 0.0f;
        }

        if (cdc_->collidedWith()) {
            onPowerup(cdc_->collidedWith());

            pec_ = assetManager.getParticleEffect("powerup.p", b2Vec2_zero, 0.0f);

            std::vector<float> tmp;

            for (ParticleEffectComponent::Emmiters::iterator it = pec_->emmiters().begin();
                 it != pec_->emmiters().end(); ++it) {
                tmp.push_back(0.0f);

                (*it)->entry().tint.setTimeline(tmp);

                tmp.clear();

                tmp.push_back(color_.rgba[0]);
                tmp.push_back(color_.rgba[1]);
                tmp.push_back(color_.rgba[2]);

                (*it)->entry().tint.setColors(tmp);

                tmp.clear();
            }

            pec_->setZOrder(zOrderLight + 1);

            pec_->resetEmit();

            parent()->addComponent(pec_);

            lightC_->removeFromParent();
            rc_->removeFromParent();
        }
    }

    void PowerupComponent::onRegister()
    {
        lightC_ = boost::make_shared<LightComponent>();

        light_ = boost::make_shared<PointLight>();
        light_->setXray(true);
        light_->setDiffuse(false);
        light_->setColor(color_);
        light_->setDistance(radius_);

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);

        rc_ = parent()->findComponent<RenderComponent>();
        rcColor_ = rc_->color();
    }

    void PowerupComponent::onUnregister()
    {
    }

    void PowerupHealthComponent::onPowerup(const SceneObjectPtr& obj)
    {
        /*
         * was 'obj->changeLife', but changed because obj can also be a vehicle.
         */
        scene()->player()->changeLife(amount_);
        audio.playSound("powerup_health.ogg");
    }

    void PowerupGemComponent::onPowerup(const SceneObjectPtr& obj)
    {
        /*
         * was 'obj->changeLife', but changed because obj can also be a vehicle.
         */
        PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();
        if (pc) {
            pc->changeNumGems(1);
        }

        audio.playSound("powerup_gem.ogg");
    }

    void PowerupInventoryComponent::onPowerup(const SceneObjectPtr& obj)
    {
        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (pc) {
            pc->inventory()->give(item_);
        }
        audio.playSound("powerup_inventory.ogg");
    }

    void PowerupHelmetComponent::onPowerup(const SceneObjectPtr& obj)
    {
        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (pc) {
            pc->setFlashlight(true);
        }

        audio.playSound("powerup_inventory.ogg");
    }

    void PowerupAmmoComponent::onPowerup(const SceneObjectPtr& obj)
    {
        audio.playSound("powerup_inventory.ogg");

        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (!pc) {
            return;
        }

        if ((type_ == WeaponTypeProxMine) || (type_ == WeaponTypeEShield)) {
            pc->giveWeapon(type_);
        }

        pc->changeAmmo(type_, scene()->fixupAmmoAmount(amount_));
    }

    void PowerupWeaponComponent::onPowerup(const SceneObjectPtr& obj)
    {
        audio.playSound("weapon_pickup.ogg");

        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (!pc) {
            return;
        }

        pc->giveWeapon(type_);
        pc->changeAmmo(type_, scene()->fixupAmmoAmount(amount_));
    }

    void PowerupBackpackComponent::onPowerup(const SceneObjectPtr& obj)
    {
        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (pc) {
            pc->setHaveBackpack(true);
            pc->giveWeapon(WeaponTypeBlaster);
            pc->giveWeapon(WeaponTypeGG);
        }

        audio.playSound("powerup_inventory.ogg");
    }

    void PowerupEarpieceComponent::onPowerup(const SceneObjectPtr& obj)
    {
        PlayerComponentPtr pc = obj->findComponent<PlayerComponent>();
        if (pc) {
            pc->setEarpiece(true);
        }

        audio.playSound("powerup_inventory.ogg");
    }
}
