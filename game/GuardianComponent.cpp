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

#include "GuardianComponent.h"
#include "ExplosionComponent.h"
#include "PhysicsJointComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "RenderHealthbarComponent.h"
#include "Settings.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CollisionFilter.h"
#include "Const.h"
#include "Utils.h"
#include "CameraComponent.h"
#include "ParticleEffectComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    GuardianComponent::GuardianComponent(float jumpVelocity, float shootMinTimeout, float shootMaxTimeout, float jumpTimeout)
    : TargetableComponent(phaseThink),
      jumpVelocity_(jumpVelocity),
      shootMinTimeout_(shootMinTimeout),
      shootMaxTimeout_(shootMaxTimeout),
      jumpTimeout_(jumpTimeout),
      state_(StateIdle),
      aimT_(0.0f),
      jumpT_(0.0f),
      shootT_(0.0f),
      wasDetouring_(false),
      idleRot_(0.0f),
      idleRotDir_(((rand() % 2) == 0) ? 1 : -1)
    {
    }

    GuardianComponent::~GuardianComponent()
    {
    }

    void GuardianComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GuardianComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            if (interceptBehavior_) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();
            }
            weapon_->trigger(false);

            parent()->findComponent<ParticleEffectComponent>()->allowCompletion();

            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            for (PhysicsJointComponent::Joints::const_iterator it = jc->joints().begin();
                 it != jc->joints().end(); ++it) {
                (*it)->remove();
            }

            parent()->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.2f);
            parent()->addComponent(component);

            b2Vec2 velDir = parent()->getDirection(8.0f);
            b2Rot rot(2.0f * b2_pi / (parent()->objects().size() + 1));

            parent()->setAngularVelocity(b2_pi * 8.0f);

            std::set<SceneObjectPtr> tmp = parent()->objects();

            for (std::set<SceneObjectPtr>::const_iterator it = tmp.begin();
                 it != tmp.end(); ++it) {
                if (sceneObjectFactory.makeDebris(*it)) {
                    (*it)->setLinearDamping(6.0f);
                    (*it)->setAngularDamping(6.0f);

                    scene()->reparent(*it);
                } else {
                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f, 1.2f);
                    (*it)->addComponent(component);
                }

                velDir = b2Mul(rot, velDir);

                if ((rand() % 2) == 0) {
                    velDir = -velDir;
                }

                (*it)->setLinearVelocity((*it)->linearVelocity() + velDir);
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.guardian.explosionImpulse, settings.guardian.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            scene()->spawnPowerup(parent()->pos());

            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        if (!interceptBehavior_) {
            interceptBehavior_ = parent()->interceptBehavior();
        }

        if (!avoidBehavior_) {
            avoidBehavior_ = parent()->avoidBehavior();
        }

        if (!detourBehavior_) {
            detourBehavior_ = parent()->detourBehavior();
        }

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.guardian.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(4.5f);
                avoidBehavior_->setLinearVelocity(8.0f);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.guardian.turnSpeed);
                detourBehavior_->setRayDistance(1.0f);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateWalk;
                aimT_ = getRandom(shootMinTimeout_, shootMaxTimeout_);
                jumpT_ = 0.0f;
                shootT_ = 0.0f;
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            jumpT_ -= dt;

            bool detouring = !detourBehavior_->finished();

            b2Vec2 targetDir = target()->pos() - parent()->pos();

            float lenSq = targetDir.LengthSquared();

            float angle = fabs(angleBetween(parent()->getDirection(1.0f), targetDir));

            if (shootT_ > 0.0f) {
                weapon_->trigger(true);
                shootT_ -= dt;
            } else {
                weapon_->trigger(false);
                aimT_ -= dt;
                if (aimT_ <= 0.0f) {
                    if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                        if (angle < deg2rad(20.0f)) {
                            shootT_ = 0.5f;
                            aimT_ = getRandom(shootMinTimeout_, shootMaxTimeout_);
                        } else {
                            aimT_ = getRandom(0.4f, 0.8f);
                        }
                    } else {
                        aimT_ = getRandom(shootMinTimeout_, shootMaxTimeout_);
                    }
                }
            }

            if (!detouring && (lenSq > 8.0f * 8.0f)) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(target());
            }

            if ((jumpT_ < 0.0f) || (wasDetouring_ && !detouring)) {
                if (lenSq <= 8.0f * 8.0f) {
                    if (angle <= (b2_pi / 2.0f)) {
                        parent()->applyLinearImpulse(parent()->mass() * parent()->getDirection(-jumpVelocity_),
                            parent()->worldCenter(), true);
                    } else {
                        parent()->applyLinearImpulse(parent()->mass() * parent()->getDirection(jumpVelocity_),
                            parent()->worldCenter(), true);
                    }
                    jumpT_ = getRandom(jumpTimeout_, jumpTimeout_ * 2.0f);
                } else if ((lenSq > 17.0f * 17.0f) || (wasDetouring_ && !detouring)) {
                    parent()->applyLinearImpulse(parent()->mass() * parent()->getDirection(jumpVelocity_),
                         parent()->worldCenter(), true);
                    jumpT_ = getRandom(jumpTimeout_, jumpTimeout_ * 2.0f);
                }
            }

            wasDetouring_ = detouring;

            b2Vec2 v = b2Mul(idleRot_, parent()->getDirection(-100.0f));

            parent()->applyForceToCenter(v, true);

            idleRot_ = b2Mul(idleRot_, b2Rot(deg2rad(360.0f) * dt * idleRotDir_));

            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void GuardianComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void GuardianComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
