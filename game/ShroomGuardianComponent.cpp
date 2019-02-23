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

#include "ShroomGuardianComponent.h"
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
#include "GoalComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    ShroomGuardianComponent::ShroomGuardianComponent()
    : TargetableComponent(phaseThink),
      turnSpeed_(settings.shroomGuardian.turnSpeed),
      shootTime_(0.0f),
      shootCount_(0),
      hadTarget_(false),
      dying_(false),
      sndShoot_(audio.createSound("bullet1.ogg"))
    {
    }

    ShroomGuardianComponent::~ShroomGuardianComponent()
    {
    }

    void ShroomGuardianComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ShroomGuardianComponent::update(float dt)
    {
        if (dying_) {
            return;
        }

        if (parent()->life() <= 0) {
            parent()->setAngularDamping(1.0f);

            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            for (PhysicsJointComponent::Joints::const_iterator it = jc->joints().begin();
                 it != jc->joints().end(); ++it) {
                (*it)->remove();
            }

            parent()->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.5f);
            parent()->addComponent(component);

            b2Vec2 velDir = parent()->getDirection(8.0f);
            b2Rot rot(2.0f * b2_pi / (parent()->objects().size() + 1));

            parent()->setAngularVelocity(b2_pi * 8.0f);

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                 it != parent()->objects().end(); ++it) {
                SceneObjectPtr obj = sceneObjectFactory.createBlood1(3.0f, zOrderExplosion - 1);

                obj->setPos((*it)->pos());
                obj->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(obj);

                (*it)->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.5f);
                (*it)->addComponent(component);

                velDir = b2Mul(rot, velDir);

                if ((rand() % 2) == 0) {
                    velDir = -velDir;
                }

                (*it)->setLinearVelocity((*it)->linearVelocity() + velDir);
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.shroomGuardian.explosionImpulse, settings.shroomGuardian.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            scene()->spawnPowerup(parent()->pos());

            GoalComponentPtr gc = parent()->findComponent<GoalComponent>();
            if (gc) {
                /*
                 * It's not nice when we're actually dead, but goal indicator
                 * keeps hanging around until death sequence is finished.
                 */
                gc->removeFromParent();
            }
            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                /*
                 * Same goes for healthbar.
                 */
                hc->removeFromParent();
            }

            dying_ = true;

            return;
        }

        if (!avoidBehavior_) {
            avoidBehavior_ = parent()->avoidBehavior();
        }

        if (!detourBehavior_) {
            detourBehavior_ = parent()->detourBehavior();
        }

        updateAutoTarget(dt);

        shootTime_ -= dt;

        if ((shootTime_ <= 0.0f) && (shootCount_ > 0)) {
            SceneObjectPtr left = sceneObjectFactory.createBlasterShot(SceneObjectTypeEnemyMissile, settings.shroomGuardian.shootDamage, true, settings.shroomGuardian.shootSpeed);

            left->setPos(parent()->pos() + parent()->getDirection(0.3f).Skew() + parent()->getDirection(1.0f));
            left->setAngle(parent()->angle());
            left->setLinearVelocity(parent()->getDirection(settings.shroomGuardian.shootSpeed));

            scene()->addObject(left);

            SceneObjectPtr right = sceneObjectFactory.createBlasterShot(SceneObjectTypeEnemyMissile, settings.shroomGuardian.shootDamage, true, settings.shroomGuardian.shootSpeed);

            right->setPos(parent()->pos() - parent()->getDirection(0.3f).Skew() + parent()->getDirection(1.0f));
            right->setAngle(parent()->angle());
            right->setLinearVelocity(parent()->getDirection(settings.shroomGuardian.shootSpeed));

            scene()->addObject(right);

            sndShoot_->play();

            if (--shootCount_ == 0) {
                shootTime_ = settings.shroomGuardian.shootPauseThreshold;
            } else {
                shootTime_ = settings.shroomGuardian.shootTimeThreshold;
            }
        }

        if (!target()) {
            /*
             * No target, turn on damping
             * to calm down, do nothing.
             */

            parent()->setAngularDamping(1.0f);

            hadTarget_ = false;
            avoidBehavior_->reset();
            detourBehavior_->reset();

            return;
        } else if (!hadTarget_) {
            hadTarget_ = true;
            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(8.0f);
            avoidBehavior_->setLinearVelocity(settings.shroomGuardian.jumpForwardVelocity);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(8.0f);
            detourBehavior_->setAngularVelocity(turnSpeed_);
            detourBehavior_->setTimeout(0.1f);
            detourBehavior_->start();
        }

        b2Vec2 targetDir = target()->pos() - parent()->pos();

        parent()->setAngularDamping(0.0f);

        /*
         * Shoot when in sight.
         */

        float angle = rad2deg(fabs(angleBetween(parent()->getDirection(1.0f), targetDir)));

        if ((shootTime_ <= 0.0f) &&
            (angle <= settings.shroomGuardian.shootAngleDeg) &&
            (shootCount_ == 0)) {
            shootCount_ = settings.shroomGuardian.shootCountThreshold;
        }

        bool detouring = !detourBehavior_->finished();

        if (!detouring) {
            detourBehavior_->start();
        } else {
            return;
        }

        /*
         * Try to guess what the rotation will be in 1/3 sec.
         */

        b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 3.0f, 1.0f);

        if (b2Cross(dir, targetDir) >= 0.0f) {
            parent()->applyTorque(parent()->inertia() * (turnSpeed_ - parent()->angularVelocity()), true);
        } else {
            parent()->applyTorque(parent()->inertia() * (-turnSpeed_ - parent()->angularVelocity()), true);
        }

        /*
         * Make it "jump".
         */

        float jumpVelocity;

        if (targetDir.Length() > settings.shroomGuardian.jumpForwardDistance) {
            jumpVelocity = settings.shroomGuardian.jumpForwardVelocity;
        } else if (targetDir.Length() < settings.shroomGuardian.jumpBackDistance) {
            jumpVelocity = settings.shroomGuardian.jumpBackVelocity;
            if (angle < 90) {
                jumpVelocity = -jumpVelocity;
            }
        } else {
            return;
        }

        b2Vec2 vel = parent()->linearVelocity();

        if (vel.Length() < settings.shroomGuardian.jumpVelocityThreshold) {
            parent()->applyLinearImpulse(parent()->mass() * (parent()->getDirection(jumpVelocity) - vel),
                                         parent()->worldCenter(), true);
        }
    }

    void ShroomGuardianComponent::onRegister()
    {
    }

    void ShroomGuardianComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
