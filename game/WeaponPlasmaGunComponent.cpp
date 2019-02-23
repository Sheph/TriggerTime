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

#include "WeaponPlasmaGunComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "PlasmaComponent.h"
#include "af/Utils.h"

namespace af
{
    WeaponPlasmaGunComponent::WeaponPlasmaGunComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypePlasmaGun),
      damage_(0),
      velocity_(0),
      spreadAngle_(0),
      numShots_(0),
      interval_(0),
      curShot_(0),
      t_(0),
      snd_(audio.createSound("plasma_fire.ogg")),
      tweakPos_(true),
      tweakOffset_(b2Vec2_zero)
    {
    }

    WeaponPlasmaGunComponent::~WeaponPlasmaGunComponent()
    {
    }

    void WeaponPlasmaGunComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponPlasmaGunComponent::update(float dt)
    {
        t_ -= dt;

        if (t_ > 0.0f) {
            return;
        }

        if (curShot_ == 0) {
            if (!triggerHeld()) {
                return;
            }

            if (!changeAmmo(1)) {
                return;
            }

            if (numShots_ == 1) {
                shotAngles_.resize(1);
                shotAngles_[0] = getRandom(-spreadAngle_ / 2.0f, spreadAngle_ / 2.0f);
            } else if (numShots_ == 2) {
                shotAngles_.resize(2);
                shotAngles_[0] = -getRandom(0.0f, spreadAngle_ / 2.0f);
                shotAngles_[1] = shotAngles_[0] + getRandom(spreadAngle_ / 4.0f, spreadAngle_ / 2.0f);
            } else {
                float a1 = -getRandom(spreadAngle_ / 5.0f, spreadAngle_ / 2.0f);
                float a2 = getRandom(spreadAngle_ / 5.0f, spreadAngle_ / 2.0f);

                shotAngles_.resize(numShots_);

                for (UInt32 i = 0; i < numShots_; ++i) {
                    float a = static_cast<float>(i) / (numShots_ - 1);
                    shotAngles_[i] = a1 * (1.0f - a) + a2 * a;
                }
            }

            std::random_shuffle(shotAngles_.begin(), shotAngles_.end());
        }

        b2Transform xf;

        if (tweakPos_) {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(tweakOffset_, b2Rot(angle())));
        } else {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(pos(), b2Rot(angle())));
        }

        scene()->stats()->incShotsFired(this);

        SceneObjectPtr shot = sceneObjectFactory.createPlasma(objectType(),
            damage_);

        float a = shotAngles_[curShot_];

        b2Transform realXf = xf;

        realXf.q = b2Mul(realXf.q, b2Rot(a));

        shot->setTransform(realXf);
        shot->setLinearVelocity(shot->getDirection(velocity_));

        if (tweakPos_) {
            shot->findComponent<PlasmaComponent>()->setStartPos(parent()->getWorldPoint(pos()));
        }

        setupShot(shot);

        scene()->addObject(shot);

        if ((curShot_ == 0) && haveSound()) {
            snd_->play();
        }

        updateCrosshair();

        ++curShot_;

        if (curShot_ >= numShots_) {
            curShot_ = 0;
            t_ = interval_;
        } else {
            t_ = dt;
        }
    }

    void WeaponPlasmaGunComponent::reload()
    {
        curShot_ = 0;
        t_ = interval_;
    }

    void WeaponPlasmaGunComponent::onRegister()
    {
    }

    void WeaponPlasmaGunComponent::onUnregister()
    {
    }
}
