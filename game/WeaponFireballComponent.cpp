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

#include "WeaponFireballComponent.h"
#include "FireballComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"

namespace af
{
    WeaponFireballComponent::WeaponFireballComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeFireball),
      damage_(0),
      velocity_(0),
      interval_(0),
      t_(0),
      hitHaveSound_(true)
    {
    }

    WeaponFireballComponent::~WeaponFireballComponent()
    {
    }

    void WeaponFireballComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponFireballComponent::update(float dt)
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

        SceneObjectPtr shot = sceneObjectFactory.createFireball(objectType(), damage_);

        if (!hitHaveSound_ || !haveSound()) {
            shot->findComponent<FireballComponent>()->setHaveSound(haveSound(), hitHaveSound_);
        }

        shot->setTransform(xf);
        shot->setLinearVelocity(shot->getDirection(velocity_));

        setupShot(shot);

        scene()->addObject(shot);

        t_ = interval_;
    }

    void WeaponFireballComponent::reload()
    {
        t_ = interval_;
    }

    void WeaponFireballComponent::onRegister()
    {
    }

    void WeaponFireballComponent::onUnregister()
    {
    }
}
