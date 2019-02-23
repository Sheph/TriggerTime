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

#include "WeaponSeekerComponent.h"
#include "SeekerComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Utils.h"
#include "Const.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponSeekerComponent::WeaponSeekerComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeSeeker),
      explosionImpulse_(0),
      explosionDamage_(0),
      velocity_(0),
      interval_(0),
      seekVelocity_(0.0f),
      t_(0),
      snd_(audio.createSound("seeker_fire.ogg")),
      tweakPos_(true),
      tweakOffset_(b2Vec2_zero)
    {
    }

    WeaponSeekerComponent::~WeaponSeekerComponent()
    {
    }

    void WeaponSeekerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponSeekerComponent::update(float dt)
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

        b2Transform xf;

        if (tweakPos_) {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(tweakOffset_, b2Rot(angle())));
        } else {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(pos(), b2Rot(angle())));
        }

        SceneObjectPtr seeker = sceneObjectFactory.createSeeker(objectType(),
            explosionImpulse_, explosionDamage_);

        seeker->setTransform(xf);
        seeker->setLinearVelocity(seeker->getDirection(velocity_));

        SeekerComponentPtr c = seeker->findComponent<SeekerComponent>();

        if (tweakPos_) {
            c->setStartPos(parent()->getWorldPoint(pos()));
        }

        if (target_) {
            c->setTarget(target_);
            c->setSeekVelocity(seekVelocity_);
        }

        setupShot(seeker);

        scene()->addObject(seeker);

        light_->setPos(pos() + angle2vec(angle(), flashDistance()));
        light_->setVisible(true);

        if (haveSound()) {
            snd_->play();
        }

        t_ = interval_;
    }

    void WeaponSeekerComponent::reload()
    {
        t_ = interval_;
    }

    void WeaponSeekerComponent::onRegister()
    {
        light_ = boost::make_shared<PointLight>();
        light_->setVisible(false);
        light_->setColor(Color(0.0f, 1.0f, 1.0f, 1.0f));
        light_->setDiffuse(false);
        light_->setXray(true);
        light_->setDistance(6.0f);
        light_->setIntensity(1.1f);
        light_->setPos(pos() + angle2vec(angle(), flashDistance()));

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);
    }

    void WeaponSeekerComponent::onUnregister()
    {
        lightC_->removeFromParent();
        target_.reset();
    }
}
