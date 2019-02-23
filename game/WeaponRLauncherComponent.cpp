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

#include "WeaponRLauncherComponent.h"
#include "RocketComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Utils.h"
#include "Const.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponRLauncherComponent::WeaponRLauncherComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeRLauncher),
      explosionImpulse_(0),
      explosionDamage_(0),
      velocity_(0),
      interval_(0),
      t_(0),
      snd_(audio.createSound("rocket_fire.ogg")),
      tweakOffset_(b2Vec2_zero)
    {
    }

    WeaponRLauncherComponent::~WeaponRLauncherComponent()
    {
    }

    void WeaponRLauncherComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponRLauncherComponent::update(float dt)
    {
        t_ -= dt;

        if (t_ < (interval_ - 0.1f)) {
            light_->setVisible(false);
        }

        if ((t_ > 0.0f) || !triggerHeld()) {
            return;
        }

        if (!changeAmmo(1)) {
            return;
        }

        b2Transform xf = b2Mul(parent()->getTransform(),
            b2Transform(tweakOffset_, b2Rot(angle())));

        scene()->stats()->incShotsFired(this);

        SceneObjectPtr rocket = sceneObjectFactory.createRocket(objectType(),
            explosionImpulse_, explosionDamage_);

        rocket->setTransform(xf);
        rocket->setLinearVelocity(rocket->getDirection(velocity_));

        rocket->findComponent<RocketComponent>()->setStartPos(parent()->getWorldPoint(pos()));

        setupShot(rocket);

        scene()->addObject(rocket);

        light_->setPos(pos() + angle2vec(angle(), flashDistance()));
        light_->setVisible(true);

        if (haveSound()) {
            snd_->play();
        }

        updateCrosshair();

        t_ = interval_;
    }

    void WeaponRLauncherComponent::reload()
    {
        t_ = interval_;
    }

    void WeaponRLauncherComponent::onRegister()
    {
        light_ = boost::make_shared<PointLight>();
        light_->setVisible(false);
        light_->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
        light_->setDiffuse(false);
        light_->setXray(true);
        light_->setDistance(6.0f);
        light_->setIntensity(1.1f);
        light_->setPos(pos() + angle2vec(angle(), flashDistance()));

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);
    }

    void WeaponRLauncherComponent::onUnregister()
    {
        lightC_->removeFromParent();
    }
}
