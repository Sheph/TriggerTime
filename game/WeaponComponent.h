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

#ifndef _WEAPONCOMPONENT_H_
#define _WEAPONCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObjectManager.h"
#include "Image.h"
#include "AudioManager.h"
#include "CollisionFilter.h"
#include "af/EnumTraits.h"

namespace af
{
    enum WeaponType
    {
        WeaponTypeGG = 0,
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

    static const int WeaponTypeMax = WeaponTypeAim;

    enum WeaponSlot
    {
        WeaponSlotPrimary0 = 0,
        WeaponSlotPrimary1,
        WeaponSlotPrimary2,
        WeaponSlotPrimary3,
        WeaponSlotSecondary0,
        WeaponSlotSecondary1,
        WeaponSlotSecondary2,
    };

    static const int WeaponSlotMax = WeaponSlotSecondary2;

    inline bool weaponSlotPrimary(WeaponSlot slot)
    {
        return slot < WeaponSlotSecondary0;
    }

    inline WeaponType ammoWeaponType(WeaponType type)
    {
        if (type == WeaponTypeSuperShotgun) {
            return WeaponTypeShotgun;
        } else if (type == WeaponTypeLGun) {
            return WeaponTypePlasmaGun;
        } else {
            return type;
        }
    }

    class WeaponComponent : public PhasedComponent
    {
    public:
        WeaponComponent(SceneObjectType objectType, WeaponType weaponType);
        virtual ~WeaponComponent();

        virtual void reload() = 0;

        virtual bool aiming() const;

        virtual bool charged() const;

        void trigger(bool hold);

        void triggerOnce();

        void cancel();

        inline SceneObjectType objectType() const { return objectType_; }
        inline WeaponType weaponType() const { return weaponType_; }

        bool triggerHeld() const;
        bool canceled() const;

        inline void setPos(const b2Vec2& value) { pos_ = value; }
        inline const b2Vec2& pos() const { return pos_; }

        inline void setAngle(float value) { angle_ = value; }
        inline float angle() const { return angle_; }

        inline void setFlashDistance(float value) { flashDistance_ = value; }
        inline float flashDistance() const { return flashDistance_; }

        inline bool flashQuad() const { return flashQuad_; }
        inline void setFlashQuad(bool value) { flashQuad_ = value; }

        inline void setHaveSound(bool value) { haveSound_ = value; }
        inline bool haveSound() const { return haveSound_; }

        inline void setUseFilter(bool value) { useFilter_ = value; }
        inline bool useFilter() const { return useFilter_; }

        void addToFilter(SInt32 cookie);

        inline void setUseFreeze(bool value) { useFreeze_ = value; }
        inline bool useFreeze() const { return useFreeze_; }

        inline void setTriggerCrosshair(bool value) { triggerCrosshair_ = value; }
        inline bool triggerCrosshair() const { return triggerCrosshair_; }

        void setTransform(const b2Transform& value);

        inline void setAmmo(float* value) { ammo_ = value; }
        inline bool haveAmmo() const { return ammo_ != NULL; }
        inline float ammo() const { return ammo_ ? *ammo_ : 0.0f; }

        const char* ammoStr() const;

        static const std::string& getImageName(WeaponType type);

        static Image getImage(WeaponType type);

        static const std::string& getAmmoAssetPath(WeaponType type);

        static WeaponSlot getSlot(WeaponType type);

        static bool isDirectional(WeaponType type);

    protected:
        bool changeAmmo(float amount, bool quiet = false);

        void setupShot(const SceneObjectPtr& obj);

        void updateCrosshair();

    private:
        SceneObjectType objectType_;
        WeaponType weaponType_;
        mutable bool triggerHeld_;
        mutable bool once_;
        mutable bool cancel_;
        b2Vec2 pos_;
        float angle_;
        bool haveSound_;
        float flashDistance_;
        bool flashQuad_;
        float* ammo_;
        mutable bool emptyClicked_;
        bool useFilter_;
        bool useFreeze_;
        bool triggerCrosshair_;

        AudioSourcePtr sndEmptyClick_;
        CollisionFilterPtr collisionFilter_;
    };

    typedef boost::shared_ptr<WeaponComponent> WeaponComponentPtr;

    AF_ENUMTRAITS(WeaponType);
}

#endif
