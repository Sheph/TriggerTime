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

#include "WeaponProxMineComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "ProxMineComponent.h"
#include "af/Utils.h"

namespace af
{
    WeaponProxMineComponent::WeaponProxMineComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeProxMine),
      armTimeout_(0.0f),
      activationRadius_(0.0f),
      activationTimeout_(0.0f),
      explosionTimeout_(0.0f),
      explosionImpulse_(0.0f),
      explosionDamage_(0.0f),
      interval_(0.0f),
      t_(0.0f),
      snd_(audio.createSound("proxmine_put.ogg"))
    {
    }

    WeaponProxMineComponent::~WeaponProxMineComponent()
    {
    }

    void WeaponProxMineComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponProxMineComponent::update(float dt)
    {
        t_ -= dt;

        if ((t_ > 0.0f) || !triggerHeld()) {
            return;
        }

        if (!changeAmmo(1)) {
            return;
        }

        b2Transform xf = b2Mul(parent()->getTransform(),
            b2Transform(pos(), b2Rot(angle())));

        scene()->stats()->incShotsFired(this);

        SceneObjectPtr pm = sceneObjectFactory.createProxMine(objectType());

        pm->setTransform(xf);

        ProxMineComponentPtr c = pm->findComponent<ProxMineComponent>();

        c->setArmTimeout(armTimeout_);
        c->setActivationRadius(activationRadius_);
        c->setActivationTimeout(activationTimeout_);
        c->setExplosionTimeout(explosionTimeout_);
        c->setExplosionImpulse(explosionImpulse_);
        c->setExplosionDamage(explosionDamage_);

        scene()->addObject(pm);

        if (haveSound()) {
            snd_->play();
        }

        t_ = interval_;
    }

    void WeaponProxMineComponent::reload()
    {
        t_ = interval_;
    }

    void WeaponProxMineComponent::onRegister()
    {
    }

    void WeaponProxMineComponent::onUnregister()
    {
    }
}
