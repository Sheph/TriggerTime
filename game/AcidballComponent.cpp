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

#include "AcidballComponent.h"
#include "Scene.h"
#include "ExplosionComponent.h"
#include "Const.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PlayerComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    AcidballComponent::AcidballComponent(const CollisionDieComponentPtr& cdc)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      explosionTimeout_(0.0f),
      explosionImpulse_(0.0f),
      explosionDamage_(0.0f),
      toxicDamage_(0.0f),
      toxicDamageTimeout_(0.0f),
      t_(0.0f),
      done_(false),
      snd_(audio.createSound("acidball_stick.ogg")),
      acidWormOwner_(false)
    {
    }

    AcidballComponent::~AcidballComponent()
    {
    }

    void AcidballComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void AcidballComponent::update(float dt)
    {
        if (pec_->isAllowCompletion()) {
            if (pec_->isComplete()) {
                parent()->removeFromParent();
            }
            return;
        }

        if (!done_ && cdc_->collidedWith()) {
            parent()->setFreezable(false);

            xf_ = parent()->getTransform();

            done_ = true;

            if (cdc_->collidedWith()->body()->GetType() == b2_staticBody) {
                t_ = explosionTimeout_;
                snd_->play();
                parent()->setPosSmoothed(cdc_->collisionPoint());
            }

            if (acidWormOwner_ &&
                (cdc_->collidedWith()->type() == SceneObjectTypePlayer) &&
                scene()->inputBoat()->active()) {
                PlayerComponentPtr pc = scene()->player()->findComponent<PlayerComponent>();
                if (pc) {
                    pc->setDamagedByAcidWormOnBoat();
                }
            }
        }

        if (done_) {
            if (t_ <= 0.0f) {
                pec_->allowCompletion();

                parent()->findComponent<RenderQuadComponent>()->removeFromParent();
                parent()->findComponent<LightComponent>()->removeFromParent();

                SceneObjectPtr explosion = sceneObjectFactory.createExplosion4(zOrderExplosion);

                explosion->setTransform(xf_);

                explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                    explosionImpulse_, explosionDamage_, explosionDamageTypes_);

                scene()->addObject(explosion);

                SceneObjectPtr cloud = sceneObjectFactory.createToxicCloud1(toxicDamage_, toxicDamageTimeout_, zOrderExplosion - 1);

                cloud->setTransform(xf_);

                scene()->addObject(cloud);

                return;
            }

            t_ -= dt;
        }
    }

    void AcidballComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void AcidballComponent::onRegister()
    {
        pec_ = parent()->findComponent<ParticleEffectComponent>();
    }

    void AcidballComponent::onUnregister()
    {
    }
}
