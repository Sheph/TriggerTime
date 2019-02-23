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

#include "BeetleComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "StunnedComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "CameraComponent.h"
#include "ExplosionComponent.h"
#include "InvulnerabilityComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BeetleComponent::BeetleComponent(const AnimationComponentPtr& torsoAc, const AnimationComponentPtr& torsoMaskAc,
        const RenderQuadComponentPtr& torsoRc, const RenderQuadComponentPtr& torsoMaskRc)
    : TargetableComponent(phaseThink),
      torsoAc_(torsoAc),
      torsoMaskAc_(torsoMaskAc),
      torsoRc_(torsoRc),
      torsoMaskRc_(torsoMaskRc),
      biteTime_(0.0f),
      shootTime_(0.0f),
      targetFound_(false),
      split_(false),
      kamikaze_(false),
      dying_(false),
      trapped_(false),
      spawnPowerup_(true),
      sndBite_(audio.createSound("baby_spider_bite.ogg")),
      sndDie_(audio.createSound("baby_spider_die.ogg")),
      sndShoot_(audio.createSound("beetle_attack.ogg"))
    {
    }

    BeetleComponent::~BeetleComponent()
    {
    }

    void BeetleComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BeetleComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            if (kamikaze_) {
                dieExplode();
            } else if (split_) {
                dieSplit();
            } else {
                dieNormal();
            }

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

        if (parent()->stunned()) {
            torsoAc_->setPaused(true);
            torsoMaskAc_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        biteTime_ -= dt;
        shootTime_ -= dt;

        if (dying_) {
            if (shootTime_ <= 0.0f) {
                parent()->changeLife(-parent()->life());
            }
            return;
        }

        if (!target()) {
            torsoAc_->setPaused(true);
            torsoMaskAc_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        if (!targetFound_) {
            shootTime_ = getRandom(settings.beetle1.attackTimeout * 0.5f, settings.beetle1.attackTimeout * 1.2f);

            targetFound_ = true;

            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(turnSpeed());
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(3.0f);
            avoidBehavior_->setLinearVelocity(walkSpeed());
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(5.0f);
            detourBehavior_->setAngularVelocity(turnSpeed());
            detourBehavior_->setTimeout(0.1f);
            detourBehavior_->start();
        }

        bool detouring = !detourBehavior_->finished();

        if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
            detourBehavior_->start();
        }

        torsoAc_->setPaused(false);
        torsoMaskAc_->setPaused(false);

        if (detouring) {
            interceptBehavior_->setTarget(SceneObjectPtr());
        } else {
            interceptBehavior_->setTarget(target());
        }

        if (parent()->linearVelocity().Length() < walkSpeed()) {
            parent()->applyForceToCenter(parent()->mass() *
                parent()->getDirection(walkSpeed() * 1.0f + parent()->linearVelocityDamped()), true);
        }

        if (kamikaze_) {
            if ((b2DistanceSquared(parent()->pos(), target()->pos()) <= 3.0f * 3.0f) && !trapped_) {
                parent()->setGravityGunAware(false);
                parent()->setGravityGunQuietCancel(true);

                dying_ = true;
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();
                torsoAc_->setPaused(true);
                torsoMaskAc_->setPaused(true);
                shootTime_ = settings.beetle2.explosionTimeout;
            }
        } else if ((biteTime_ <= 0.0f) &&
            (b2DistanceSquared(parent()->pos(), target()->pos()) <= (2.1f * 2.1f)) &&
            (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) <= deg2rad(45))) {
            if (!trapped_) {
                target()->changeLife(-settings.beetle1.biteDamage);
                sndBite_->play();
            }
            biteTime_ = settings.beetle1.biteTime;
            shootTime_ = getRandom(settings.beetle1.attackTimeout * 0.5f, settings.beetle1.attackTimeout * 1.2f);
        }

        if (weapon_) {
            if ((shootTime_ <= 0.0f) && (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) <= settings.beetle1.attackAngle)) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                    if (!trapped_) {
                        weapon_->triggerOnce();
                        sndShoot_->play();
                    }
                }
                shootTime_ = getRandom(settings.beetle1.attackTimeout * 0.5f, settings.beetle1.attackTimeout * 1.2f);
            }
        }
    }

    void BeetleComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void BeetleComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    float BeetleComponent::turnSpeed() const
    {
        return kamikaze_ ? settings.beetle2.turnSpeed : settings.beetle1.turnSpeed;
    }

    float BeetleComponent::walkSpeed() const
    {
        return kamikaze_ ? settings.beetle2.walkSpeed : settings.beetle1.walkSpeed;
    }

    void BeetleComponent::dieNormal()
    {
        if (torsoAc_->currentAnimation() == AnimationDefault) {
            parent()->setFreezable(false);

            if (interceptBehavior_) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();
            }

            torsoAc_->setPaused(false);
            torsoMaskAc_->setPaused(false);
            torsoAc_->startAnimation(AnimationDie);
            torsoMaskAc_->startAnimation(AnimationDie);

            parent()->setGravityGunAware(false);
            parent()->setGravityGunQuietCancel(true);
            StunnedComponentPtr sc = parent()->findComponent<StunnedComponent>();
            if (sc) {
                sc->removeFromParent();
                parent()->setStunned(false);
                parent()->findComponent<PhysicsBodyComponent>()->restoreFilter();
            }

            torsoRc_->setZOrder(zOrderBack + 1);
            torsoMaskRc_->setZOrder(zOrderBack + 2);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(4.0f, zOrderBack, parent()->activeDeadbody());

            bs->setPos(parent()->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            parent()->setLinearDamping(5.0f);
            parent()->setAngularDamping(5.0f);

            if (spawnPowerup_) {
                scene()->spawnPowerup(parent()->pos());
            }

            sndDie_->play();

            return;
        }

        if (!torsoAc_->animationFinished() || parent()->stunned()) {
            return;
        }

        if (!parent()->activeDeadbody()) {
            parent()->setFreezable(true);
            parent()->setFreezeRadius(4.0f);
        }

        torsoAc_->removeFromParent();
        torsoMaskAc_->removeFromParent();

        if (parent()->activeDeadbody()) {
            parent()->becomeDeadbody();
        } else {
            parent()->findComponent<PhysicsBodyComponent>()->removeFromParent();
        }

        scene()->stats()->incEnemiesKilled();

        removeFromParent();
    }

    void BeetleComponent::dieExplode()
    {
        SceneObjectPtr explosion = sceneObjectFactory.createExplosion5(zOrderExplosion);

        explosion->setTransform(parent()->getTransform());

        explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
            settings.beetle2.explosionImpulse, settings.beetle2.explosionDamage,
            SceneObjectTypes(SceneObjectTypePlayer) | SceneObjectTypeAlly |
            SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding);

        scene()->addObject(explosion);

        scene()->stats()->incEnemiesKilled();

        parent()->removeFromParent();
    }

    void BeetleComponent::dieSplit()
    {
        SceneObjectPtr gibbed = sceneObjectFactory.createBeetle1Gibbed(!!weapon_,
            torsoMaskRc_->color());

        gibbed->setTransformRecursive(parent()->getTransform());

        b2Vec2 velDir = parent()->getDirection(5.0f);
        b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

        gibbed->setLinearVelocity(parent()->linearVelocity() + velDir);
        gibbed->setAngularVelocity(parent()->angularVelocity());

        FadeOutComponentPtr component =
            boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

        gibbed->addComponent(component);

        SceneObjectPtr obj2 = sceneObjectFactory.createBlood1(2.0f, zOrderExplosion);

        obj2->setPos(gibbed->pos());
        obj2->setAngle(getRandom(0.0f, 2.0f * b2_pi));

        scene()->addObject(obj2);

        for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
             it != gibbed->objects().end(); ++it) {
            velDir = b2Mul(rot, velDir);

            (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);
            (*it)->setAngularVelocity(parent()->angularVelocity());

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

            (*it)->addComponent(component);

            SceneObjectPtr obj2 = sceneObjectFactory.createBlood1(1.5f, zOrderExplosion);

            obj2->setPos((*it)->pos());
            obj2->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(obj2);
        }

        scene()->addObject(gibbed);

        SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(4.0f, zOrderBack, parent()->activeDeadbody());

        bs->setPos(parent()->pos());
        bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

        scene()->addObject(bs);

        audio.playSound("tetrocrab_crack.ogg");

        if (spawnPowerup_) {
            scene()->spawnPowerup(parent()->pos());
        }

        rot = b2Rot(2.0f * b2_pi / settings.beetle1.numSpawn);

        b2Vec2 dir = b2Mul(b2Rot(parent()->angle() + b2_pi / 2.0f), b2Vec2(0.1f, 0.0f));

        PhysicsBodyComponentPtr pc = parent()->findComponent<PhysicsBodyComponent>();
        int groupIndex = 0;
        if (pc) {
            groupIndex = pc->filterGroupIndex();
        }

        for (UInt32 i = 0; i < settings.beetle1.numSpawn; ++i) {
            SceneObjectPtr spawned = sceneObjectFactory.createBeetle2();

            spawned->addComponent(boost::make_shared<InvulnerabilityComponent>(0.1f));

            spawned->setPos(parent()->pos() + dir);
            spawned->setAngle(vec2angle(dir));

            spawned->setLinearVelocity(parent()->linearVelocity());
            spawned->setAngularVelocity(parent()->angularVelocity());
            spawned->findComponent<TargetableComponent>()->setAutoTarget(true);
            scene()->addObject(spawned);

            if (pc) {
                /*
                 * Hack: inherit group index so that spawned beetles play nice with specific enemy blockers.
                 */
                spawned->findComponent<PhysicsBodyComponent>()->setFilterGroupIndex(
                    groupIndex);
            }

            dir = b2Mul(rot, dir);
        }

        scene()->stats()->incEnemiesKilled();

        parent()->removeFromParent();
    }
}
