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

#include "PlasmaComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    PlasmaComponent::PlasmaComponent(const CollisionDieComponentPtr& cdc,
        const ParticleEffectComponentPtr& pec,
        float damage)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      pec_(pec),
      damage_(damage),
      haveStartPos_(false),
      wasFreezable_(false),
      tweenTime_(0.0f)
    {
    }

    PlasmaComponent::~PlasmaComponent()
    {
    }

    void PlasmaComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PlasmaComponent::update(float dt)
    {
        if (tweening_) {
            if (!tweening_->finished(tweenTime_)) {
                tweenTime_ += dt;

                float value = tweening_->getValue(tweenTime_);

                Color c = light_->color();

                c.rgba[3] = value;

                light_->setColor(c);
            } else if (pec_->isComplete()) {
                parent()->removeFromParent();
            }

            return;
        }

        if (cdc_->collidedWith()) {
            cdc_->collidedWith()->changeLife2(parent(), -damage_);

            scene()->stats()->incShotsHit(parent(), cdc_->collidedWith().get());

            parent()->resetSmooth();

            pec_->removeFromParent();

            LightComponentPtr lc = parent()->findComponent<LightComponent>();

            light_ = lc->lights().front();

            light_->setVisible(true);

            light_->setPos(parent()->getLocalPoint(cdc_->collisionPoint()));

            tweening_ = boost::make_shared<SingleTweening>(0.8f,
                EaseOutQuad, light_->color().rgba[3], 0.0f);
            tweenTime_ = 0.0f;

            pec_ = assetManager.getParticleEffect("plasma2.p",
               parent()->getLocalPoint(cdc_->collisionPoint()),
               0.0f);

            pec_->setZOrder(zOrderExplosion);

            pec_->resetEmit();

            parent()->addComponent(pec_);

            return;
        }

        if (haveStartPos_) {
            if (b2DistanceSquared(parent()->pos(), startPos_) > 0.000001f) {
                parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
            } else {
                parent()->setVisible(true);
                parent()->setLinearVelocity(actualLinearVelocity_);
                parent()->resetSmooth();
                parent()->setFreezable(wasFreezable_);
                haveStartPos_ = false;

                /*
                 * FIXME: A hack in order to reach final plasma state instantly.
                 */
                float t = 0.0f;
                while (t < 0.5f) {
                    parent()->findComponent<ParticleEffectComponent>()->update(settings.physics.fixedTimestep);
                    t += settings.physics.fixedTimestep;
                }
            }
        }
    }

    void PlasmaComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void PlasmaComponent::onRegister()
    {
        if (haveStartPos_) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            parent()->setVisible(false);
            actualLinearVelocity_ = parent()->linearVelocity();
            parent()->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (startPos_ - parent()->pos()));
        }
    }

    void PlasmaComponent::onUnregister()
    {
    }
}
