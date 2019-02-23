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

#include "ProxMineComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "SceneObjectFactory.h"
#include "ExplosionComponent.h"

namespace af
{
    ProxMineComponent::ProxMineComponent(const SceneObjectTypes& damageTypes)
    : PhasedComponent(phaseThink),
      damageTypes_(damageTypes),
      armTimeout_(0.0f),
      activationRadius_(0.0f),
      activationTimeout_(0.0f),
      explosionTimeout_(0.0f),
      explosionImpulse_(0.0f),
      explosionDamage_(0.0f),
      state_(StateIdle),
      t_(0.0f),
      beepT_(0.0f),
      beepHoldT_(0.0f),
      snd_(audio.createSound("proxmine_arm.ogg"))
    {
    }

    ProxMineComponent::~ProxMineComponent()
    {
    }

    void ProxMineComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ProxMineComponent::update(float dt)
    {
        t_ -= dt;

        switch (state_) {
        case StateIdle: {
            if (t_ <= 0.0f) {
                state_ = StateArmed;

                light_->setVisible(true);
                parent()->setActive(false);
                parent()->setActive(true);

                t_ = explosionTimeout_;

                snd_->play();

                break;
            }
            break;
        }
        case StateArmed: {
            if (t_ <= 0.0f) {
                activate(NULL);
                break;
            }

            break;
        }
        case StateActivated: {
            if (t_ <= 0.0f) {
                SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

                explosion->setTransform(parent()->getTransform());

                explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                    explosionImpulse_, explosionDamage_, damageTypes_);

                scene()->addObject(explosion);

                parent()->removeFromParent();

                return;
            }

            beepT_ -= dt;
            beepHoldT_ -= dt;

            if (beepT_ <= 0.0f) {
                light_->setVisible(false);
                beepT_ = 0.2f;
                beepHoldT_ = 0.05f;
                snd_->play();
            }

            if (beepHoldT_ <= 0.0f) {
                light_->setVisible(true);
            }

            break;
        }
        default:
            assert(0);
            break;
        }
    }

    void ProxMineComponent::sensorEnter(const SceneObjectPtr& other)
    {
        activate(other.get());
    }

    void ProxMineComponent::sensorExit(const SceneObjectPtr& other)
    {
    }

    void ProxMineComponent::onRegister()
    {
        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            if (f->IsSensor()) {
                f->GetShape()->m_radius = activationRadius_;
            } else {
                b2Filter filter = f->GetFilterData();

                filter.categoryBits = collisionBitPlayer | collisionBitEnemy;
                filter.maskBits = 0xFFFF;

                f->SetFilterData(filter);
            }
        }

        light_ = parent()->findComponent<LightComponent>()->lights()[0];

        t_ = armTimeout_;

        parent()->setActive(true);
    }

    void ProxMineComponent::onUnregister()
    {
    }

    void ProxMineComponent::activate(SceneObject* other)
    {
        if (state_ != StateArmed) {
            return;
        }

        if (other) {
            scene()->stats()->incShotsHit(NULL, other);
        }

        state_ = StateActivated;

        t_ = activationTimeout_;
    }
}
