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

#include "WeaponBlasterComponent.h"
#include "BlasterShotComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "af/Utils.h"

namespace af
{
    WeaponBlasterComponent::WeaponBlasterComponent(bool super, SceneObjectType objectType)
    : WeaponComponent(objectType, super ? WeaponTypeSuperBlaster : WeaponTypeBlaster),
      damage_(0),
      velocity_(0),
      turns_(1),
      shotsPerTurn_(1),
      turnInterval_(0),
      loopDelay_(0),
      tweakPos_(false),
      tweakOffset_(b2Vec2_zero),
      spreadAngle_(0.0f),
      curTurn_(0),
      t_(0),
      snd_(audio.createSound("bullet1.ogg"))
    {
    }

    WeaponBlasterComponent::~WeaponBlasterComponent()
    {
    }

    void WeaponBlasterComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponBlasterComponent::update(float dt)
    {
        t_ -= dt;

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

        float a = getRandom(-spreadAngle_ / 2.0f, spreadAngle_ / 2.0f);

        b2Transform realXf = xf;

        realXf.q = b2Mul(realXf.q, b2Rot(a));

        for (UInt32 i = 0; i < shotsPerTurn_; ++i) {
            scene()->stats()->incShotsFired(this);

            SceneObjectPtr shot = sceneObjectFactory.createBlasterShot(objectType(), damage_, weaponType() == WeaponTypeSuperBlaster,
                (weaponType() == WeaponTypeSuperBlaster) ? (velocity_ * 8.0f / 7.0f) : velocity_);

            shot->setTransform(realXf);
            shot->setLinearVelocity(shot->getDirection(velocity_));

            if (tweakPos_) {
                shot->findComponent<BlasterShotComponent>()->setStartPos(parent()->getWorldPoint(pos()));
            }

            setupShot(shot);

            scene()->addObject(shot);
        }

        if (haveSound()) {
            snd_->play();
        }

        updateCrosshair();

        ++curTurn_;

        if (curTurn_ >= turns_) {
            t_ = loopDelay_;
            curTurn_ = 0;
        } else {
            t_ = turnInterval_;
        }
    }

    void WeaponBlasterComponent::reload()
    {
        t_ = loopDelay_;
    }

    void WeaponBlasterComponent::onRegister()
    {
    }

    void WeaponBlasterComponent::onUnregister()
    {
    }
}
