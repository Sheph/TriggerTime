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

#ifndef _PLAYERCOMPONENT_H_
#define _PLAYERCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"
#include "JointProxy.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "RenderQuadComponent.h"
#include "AnimationComponent.h"
#include "ParticleEffectComponent.h"
#include "Inventory.h"
#include "Light.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class PlayerComponent;
    typedef boost::shared_ptr<PlayerComponent> PlayerComponentPtr;

    class PlayerComponent : public boost::enable_shared_from_this<PlayerComponent>,
                            public PhasedComponent
    {
    public:
        enum
        {
            AnimationIdle = 1,
            AnimationWalk = 2,
            AnimationHelmetDefault = 3,
            AnimationHelmetIdle = 4,
            AnimationHelmetWalk = 5,
            AnimationNogunDefault = 6,
            AnimationNogunIdle = 7,
            AnimationNogunWalk = 8,
            AnimationNobackpackDefault = 9,
            AnimationNobackpackIdle = 10,
            AnimationNobackpackWalk = 11,
            AnimationNobackpackChainsawDefault = 12,
            AnimationNobackpackChainsawIdle = 13,
            AnimationNobackpackChainsawWalk = 14,
            AnimationNobackpackChainsawAttack = 15,
            AnimationChainsawDefault = 16,
            AnimationChainsawIdle = 17,
            AnimationChainsawWalk = 18,
            AnimationChainsawAttack = 19,
        };

        PlayerComponent(const b2Vec2& weaponPos,
            float weaponAngle,
            float weaponFlashDistance,
            const b2Vec2& exhaustPos1,
            float exhaustAngle1,
            const b2Vec2& exhaustPos2,
            float exhaustAngle2,
            const RenderQuadComponentPtr& torsoRc,
            const AnimationComponentPtr& torsoAc,
            const RenderQuadComponentPtr& legsRc,
            const AnimationComponentPtr& legsAc,
            const LightPtr& flashlight);
        ~PlayerComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void setAngVelocity(float angVelocity);

        inline WeaponSlot primarySlot() const { return primarySlot_; }
        inline WeaponSlot secondarySlot() const { return secondarySlot_; }

        inline const WeaponComponentPtr& weapon() const { return weapons_[primarySlot_]; }
        inline const WeaponComponentPtr& altWeapon() const { return weapons_[secondarySlot_]; }

        inline const WeaponComponentPtr* weapons() const { return weapons_; }

        inline UInt32 numHearts() const { return numHearts_; }
        inline void setNumHearts(UInt32 value) { numHearts_ = value; }

        inline UInt32 numGems() const { return numGems_; }
        inline void setNumGems(UInt32 value) { numGems_ = value; }

        void changeNumGems(UInt32 value) { numGems_ += value; }

        bool giveWeapon(WeaponType weaponType);

        void switchWeapon(WeaponSlot slot);

        bool haveWeapon(WeaponType weaponType);

        float ammo(WeaponType weaponType);
        void changeAmmo(WeaponType weaponType, float amount);

        void merge(const PlayerComponentPtr& other);

        inline const InventoryPtr& inventory() const { return inventory_; }

        inline bool jetpack() const { return jetpack_; }
        inline void setJetpack(bool value) { jetpack_ = value; }

        inline bool flashlight() const { return flashlight_->visible(); }
        inline void setFlashlight(bool value) { flashlight_->setVisible(value); }

        inline bool earpiece() const { return earpiece_; }
        inline void setEarpiece(bool value) { earpiece_ = value; }

        inline bool standing() const { return standing_; }
        inline void setStanding(bool value) { standing_ = value; }

        inline bool haveGun() const { return haveGun_; }
        inline void setHaveGun(bool value) { haveGun_ = value; }

        inline bool haveBackpack() const { return haveBackpack_; }
        inline void setHaveBackpack(bool value) { haveBackpack_ = value; }

        void changeLifeNoDamage(float value);

        inline void triggerRecoil() { recoilTweenTime_ = 0.0f; }

        inline void setDamagedByCoreProtectorMelee() { flags_[FlagDamagedByCoreProtectorMelee] = true; }
        inline void setDamagedByAcidWormOnBoat() { flags_[FlagDamagedByAcidWormOnBoat] = true; }
        inline void setDamagedByGas() { flags_[FlagDamagedByGas] = true; }

        inline bool flagLifeLost() const { return flags_[FlagLifeLost]; }
        inline bool flagDamaged() const { return flags_[FlagDamaged]; }
        inline bool flagDamagedByCoreProtectorMelee() const { return flags_[FlagDamagedByCoreProtectorMelee]; }
        inline bool flagDamagedByAcidWormOnBoat() const { return flags_[FlagDamagedByAcidWormOnBoat]; }
        inline bool flagDamagedByGas() const { return flags_[FlagDamagedByGas]; }

        inline float stamina() const { return stamina_; }
        void setStamina(float value);

        inline float maxStamina() const { return maxStamina_; }
        void setMaxStamina(float value);

        float staminaPercent() const;

        void changeStamina(float value);

    private:
        enum Flag
        {
            FlagLifeLost = 0,
            FlagDamaged,
            FlagDamagedByCoreProtectorMelee,
            FlagDamagedByAcidWormOnBoat,
            FlagDamagedByGas,
        };

        static const int FlagMax = FlagDamagedByGas;

        typedef EnumSet<Flag, FlagMax + 1> Flags;

        virtual void onRegister();

        virtual void onUnregister();

        b2Vec2 weaponPos_;
        float weaponAngle_;
        float weaponFlashDistance_;

        b2Vec2 exhaustPos_[2];
        float exhaustAngle_[2];

        UInt32 numHearts_;
        UInt32 numGems_;

        float prevAngVelocity_;

        WeaponComponentPtr weapons_[WeaponSlotMax + 1];
        float ammo_[WeaponTypeMax + 1];

        WeaponSlot primarySlot_;
        WeaponSlot secondarySlot_;

        float prevHealth_;

        InventoryPtr inventory_;

        AudioSourcePtr sndDeath_;
        AudioSourcePtr sndSwitch_;

        RenderQuadComponentPtr torsoRc_;
        AnimationComponentPtr torsoAc_;
        RenderQuadComponentPtr legsRc_;
        AnimationComponentPtr legsAc_;

        float torsoOrigHeight_;
        b2Vec2 torsoOrigPos_;

        bool jetpack_;
        ParticleEffectComponentPtr exhaust_[2];

        LightPtr flashlight_;
        bool earpiece_;
        bool standing_;

        TweeningPtr recoilTweening_;
        float recoilTweenTime_;

        Flags flags_;

        bool haveGun_;
        bool haveBackpack_;

        bool ghost_;

        float stamina_;
        float maxStamina_;
    };
}

#endif
