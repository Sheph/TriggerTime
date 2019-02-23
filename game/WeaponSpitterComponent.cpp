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

#include "WeaponSpitterComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "SpitComponent.h"
#include "af/Utils.h"

namespace af
{
    WeaponSpitterComponent::WeaponSpitterComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeSpitter),
      damage_(0.0f),
      velocity_(0.0f),
      spreadAngle_(0.0f),
      spreadRandom_(true),
      minShots_(1),
      maxShots_(1),
      totalShots_(1),
      shotInterval_(0.0f),
      interval_(0.0f),
      minColor_(1.0f, 1.0f, 1.0f),
      maxColor_(1.0f, 1.0f, 1.0f),
      minHeight_(1.0f),
      maxHeight_(1.0f),
      i_(1),
      curShot_(0),
      t_(0),
      tweakPos_(true),
      tweakOffset_(b2Vec2_zero)
    {
    }

    WeaponSpitterComponent::~WeaponSpitterComponent()
    {
    }

    void WeaponSpitterComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponSpitterComponent::update(float dt)
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
        }

        UInt32 numShots = minShots_ + (rand() % (maxShots_ - minShots_ + 1));

        b2Transform xf;

        if (tweakPos_) {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(tweakOffset_, b2Rot(angle())));
        } else {
            xf = b2Mul(parent()->getTransform(),
                b2Transform(pos(), b2Rot(angle())));
        }

        if (spreadRandom_) {
            for (UInt32 i = 0; i < numShots; ++i) {
                float a = getRandom(-spreadAngle_ / 2.0f, spreadAngle_ / 2.0f);

                createShot(xf, b2Rot(a));
            }
        } else {
            if ((numShots % 2) != 0) {
                createShot(xf, b2Rot(0.0f));
            }

            if (numShots >= 2) {
                b2Rot rotL((spreadAngle_ / 2.0f) / (numShots / 2));
                b2Rot rotR(-(spreadAngle_ / 2.0f) / (numShots / 2));
                b2Rot rl(0.0f);
                b2Rot rr(0.0f);

                for (UInt32 i = 0; i < numShots / 2; ++i) {
                    rl = b2Mul(rotL, rl);
                    rr = b2Mul(rotR, rr);
                    createShot(xf, rl);
                    createShot(xf, rr);
                }
            }
        }

        if ((curShot_ == 0) && haveSound() && snd_) {
            snd_->play();
        }

        curShot_ += numShots;

        if (curShot_ >= totalShots_) {
            curShot_ = 0;
            t_ = interval_;
        } else {
            t_ = shotInterval_;
        }
    }

    void WeaponSpitterComponent::reload()
    {
        curShot_ = 0;
        t_ = interval_;
    }

    void WeaponSpitterComponent::onRegister()
    {
    }

    void WeaponSpitterComponent::onUnregister()
    {
    }

    void WeaponSpitterComponent::createShot(const b2Transform& xf, const b2Rot& rot)
    {
        SceneObjectPtr shot = sceneObjectFactory.createSpit(i_, objectType(),
            damage_, getRandom(minHeight_, maxHeight_),
            getRandom(minColor_, maxColor_));

        b2Transform realXf = xf;

        realXf.q = b2Mul(realXf.q, rot);

        shot->setTransform(realXf);
        shot->setLinearVelocity(shot->getDirection(velocity_));

        if (tweakPos_) {
            shot->findComponent<SpitComponent>()->setStartPos(parent()->getWorldPoint(pos()));
        }

        setupShot(shot);

        scene()->addObject(shot);
    }
}
