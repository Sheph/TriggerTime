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

#ifndef _POWERUPCOMPONENT_H_
#define _POWERUPCOMPONENT_H_

#include "PhasedComponent.h"
#include "CollisionDieComponent.h"
#include "LightComponent.h"
#include "PointLight.h"
#include "Tweening.h"
#include "ParticleEffectComponent.h"
#include "WeaponComponent.h"
#include "Inventory.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    enum PowerupType
    {
        PowerupTypeHealth = 0,
        PowerupTypeGem,
        PowerupTypeInventory,
        PowerupTypeHelmet,
        PowerupTypeAmmo,
        PowerupTypeBackpack,
        PowerupTypeEarpiece,
    };

    static const int PowerupTypeMax = PowerupTypeEarpiece;

    class PowerupComponent : public PhasedComponent
    {
    public:
        PowerupComponent(const CollisionDieComponentPtr& cdc,
            float timeout, float radius, const Color& color);
        virtual ~PowerupComponent();

        virtual void update(float dt);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        virtual void onPowerup(const SceneObjectPtr& obj) = 0;

        CollisionDieComponentPtr cdc_;
        bool haveTimeout_;
        float timeout_;
        float radius_;
        Color color_;

        LightComponentPtr lightC_;
        PointLightPtr light_;

        TweeningPtr tweening_;
        float tweenTime_;

        RenderComponentPtr rc_;
        Color rcColor_;

        ParticleEffectComponentPtr pec_;
    };

    typedef boost::shared_ptr<PowerupComponent> PowerupComponentPtr;

    class PowerupHealthComponent : public boost::enable_shared_from_this<PowerupHealthComponent>,
                                   public PowerupComponent
    {
    public:
        PowerupHealthComponent(const CollisionDieComponentPtr& cdc,
            float timeout, float radius, const Color& color, float amount)
        : PowerupComponent(cdc, timeout, radius, color),
          amount_(amount)
        {
        }

        ~PowerupHealthComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);

        float amount_;
    };

    typedef boost::shared_ptr<PowerupHealthComponent> PowerupHealthComponentPtr;

    class PowerupGemComponent : public boost::enable_shared_from_this<PowerupGemComponent>,
                                public PowerupComponent
    {
    public:
        PowerupGemComponent(const CollisionDieComponentPtr& cdc,
            float timeout, float radius, const Color& color)
        : PowerupComponent(cdc, timeout, radius, color)
        {
        }

        ~PowerupGemComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);
    };

    typedef boost::shared_ptr<PowerupGemComponent> PowerupGemComponentPtr;

    class PowerupInventoryComponent : public boost::enable_shared_from_this<PowerupInventoryComponent>,
                                      public PowerupComponent
    {
    public:
        PowerupInventoryComponent(InventoryItem item,
            const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color),
          item_(item)
        {
        }

        ~PowerupInventoryComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);

        InventoryItem item_;
    };

    typedef boost::shared_ptr<PowerupInventoryComponent> PowerupInventoryComponentPtr;

    class PowerupHelmetComponent : public boost::enable_shared_from_this<PowerupHelmetComponent>,
                                   public PowerupComponent
    {
    public:
        PowerupHelmetComponent(const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color)
        {
        }

        ~PowerupHelmetComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);
    };

    typedef boost::shared_ptr<PowerupHelmetComponent> PowerupHelmetComponentPtr;

    class PowerupAmmoComponent : public boost::enable_shared_from_this<PowerupAmmoComponent>,
                                 public PowerupComponent
    {
    public:
        PowerupAmmoComponent(WeaponType type, float amount,
            const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color),
          type_(type),
          amount_(amount)
        {
        }

        ~PowerupAmmoComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);

        WeaponType type_;
        float amount_;
    };

    typedef boost::shared_ptr<PowerupAmmoComponent> PowerupAmmoComponentPtr;

    class PowerupWeaponComponent : public boost::enable_shared_from_this<PowerupWeaponComponent>,
                                   public PowerupComponent
    {
    public:
        PowerupWeaponComponent(WeaponType type, float amount,
            const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color),
          type_(type),
          amount_(amount)
        {
        }

        ~PowerupWeaponComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);

        WeaponType type_;
        float amount_;
    };

    typedef boost::shared_ptr<PowerupWeaponComponent> PowerupWeaponComponentPtr;

    class PowerupBackpackComponent : public boost::enable_shared_from_this<PowerupBackpackComponent>,
                                     public PowerupComponent
    {
    public:
        PowerupBackpackComponent(const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color)
        {
        }

        ~PowerupBackpackComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);
    };

    typedef boost::shared_ptr<PowerupBackpackComponent> PowerupBackpackComponentPtr;

    class PowerupEarpieceComponent : public boost::enable_shared_from_this<PowerupEarpieceComponent>,
                                     public PowerupComponent
    {
    public:
        PowerupEarpieceComponent(const CollisionDieComponentPtr& cdc,
            float radius, const Color& color)
        : PowerupComponent(cdc, 0.0f, radius, color)
        {
        }

        ~PowerupEarpieceComponent()
        {
        }

        virtual void accept(ComponentVisitor& visitor)
        {
            visitor.visitPhasedComponent(shared_from_this());
        }

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

    private:
        virtual void onPowerup(const SceneObjectPtr& obj);
    };

    typedef boost::shared_ptr<PowerupEarpieceComponent> PowerupEarpieceComponentPtr;

    AF_ENUMTRAITS(PowerupType);
}

#endif
