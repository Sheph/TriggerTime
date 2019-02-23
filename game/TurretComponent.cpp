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

#include "TurretComponent.h"
#include "Utils.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "Const.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "RenderHealthbarComponent.h"
#include "CameraComponent.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    TurretComponent::TurretComponent(const SceneObjectPtr& tower,
        const std::vector<b2Transform>& shotPoses)
    : TargetableComponent(phaseThink),
      tower_(tower),
      hadTarget_(false),
      inSight_(false)
    {
        for (std::vector<b2Transform>::const_iterator it = shotPoses.begin();
             it != shotPoses.end(); ++it) {
            weapons_.push_back(boost::make_shared<WeaponBlasterComponent>(false, SceneObjectTypeEnemyMissile));
            weapons_.back()->setTransform(*it);
            weapons_.back()->setDamage(settings.turret1.shootDamage);
            weapons_.back()->setVelocity(settings.turret1.shootVelocity);
            weapons_.back()->setTurns(1);
            weapons_.back()->setShotsPerTurn(1);
            weapons_.back()->setLoopDelay(settings.turret1.shootDelay);
            weapons_.back()->setHaveSound(false);
        }

        if (!weapons_.empty()) {
            weapons_.front()->setHaveSound(true);
        }
    }

    TurretComponent::~TurretComponent()
    {
    }

    void TurretComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TurretComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.turret1.explosionImpulse, settings.turret1.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            tower_->removeFromParent();

            parent()->findComponent<LightComponent>()->lights()[0]->setVisible(true);

            ParticleEffectComponentPtr pec = assetManager.getParticleEffect("fire1.p",
                b2Vec2_zero, 0.0f, false);

            pec->setFixedAngle(true);

            pec->setZOrder(zOrderEffects);

            pec->resetEmit();

            parent()->addComponent(pec);

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        updateAutoTarget(dt);

        if (!target()) {
            seekBehavior_->reset();
            hadTarget_ = false;
            return;
        }

        if (!hadTarget_) {
            seekBehavior_->setAngularVelocity(settings.turret1.turnSpeed);
            seekBehavior_->setUseTorque(true);
            seekBehavior_->setLoop(true);
            seekBehavior_->start();
            hadTarget_ = true;
        }

        seekBehavior_->setTarget(target());

        float angle = fabs(angleBetween(tower_->getDirection(1.0f), target()->pos() - tower_->pos()));

        bool inSight = scene()->camera()->findComponent<CameraComponent>()->rectVisible(
            parent()->pos(), 4.0f, 4.0f);

        if (inSight && !inSight_) {
            for (std::vector<WeaponBlasterComponentPtr>::const_iterator it = weapons_.begin();
                 it != weapons_.end(); ++it) {
                (*it)->reload();
            }
        }

        inSight_ = inSight;

        bool hold = (angle <= settings.turret1.shootAngle) && inSight_;

        for (std::vector<WeaponBlasterComponentPtr>::const_iterator it = weapons_.begin();
             it != weapons_.end(); ++it) {
            (*it)->trigger(hold);
        }
    }

    void TurretComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        for (std::vector<WeaponBlasterComponentPtr>::const_iterator it = weapons_.begin();
             it != weapons_.end(); ++it) {
            tower_->addComponent(*it);
        }

        seekBehavior_ = tower_->seekBehavior();
    }

    void TurretComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
