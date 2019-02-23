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

#include "BabySpiderComponent.h"
#include "PhysicsBodyComponent.h"
#include "StunnedComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BabySpiderComponent::BabySpiderComponent(const AnimationComponentPtr& ac,
        const RenderQuadComponentPtr& rc,
        float walkSpeed, float biteDamage)
    : TargetableComponent(phaseThink),
      ac_(ac),
      rc_(rc),
      walkSpeed_(walkSpeed),
      biteDamage_(biteDamage),
      biteTime_(0.0f),
      trapped_(false),
      spawnPowerup_(true),
      haveSound_(true),
      targetFound_(false),
      sndBite_(audio.createSound("baby_spider_bite.ogg")),
      sndDie_(audio.createSound("baby_spider_die.ogg"))
    {
    }

    BabySpiderComponent::~BabySpiderComponent()
    {
    }

    void BabySpiderComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BabySpiderComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            if (ac_->currentAnimation() == AnimationDefault) {
                parent()->setFreezable(false);

                if (interceptBehavior_) {
                    interceptBehavior_->reset();
                    avoidBehavior_->reset();
                    detourBehavior_->reset();
                }

                ac_->setPaused(false);
                ac_->startAnimation(AnimationDie);

                parent()->setGravityGunAware(false);
                parent()->setGravityGunQuietCancel(true);
                StunnedComponentPtr sc = parent()->findComponent<StunnedComponent>();
                if (sc) {
                    sc->removeFromParent();
                    parent()->setStunned(false);
                    parent()->findComponent<PhysicsBodyComponent>()->restoreFilter();
                }

                rc_->setZOrder(zOrderBack + 1);

                SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(4.0f, zOrderBack, parent()->activeDeadbody());

                bs->setPos(parent()->pos());
                bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(bs);

                parent()->setLinearDamping(5.0f);
                parent()->setAngularDamping(5.0f);

                if (spawnPowerup_) {
                    scene()->spawnPowerup(parent()->pos());
                }

                if (haveSound_) {
                    sndDie_->play();
                }

                return;
            }

            if (!ac_->animationFinished() || parent()->stunned()) {
                return;
            }

            if (!parent()->activeDeadbody()) {
                parent()->setFreezable(true);
                parent()->setFreezeRadius(4.0f);
            }

            ac_->removeFromParent();

            if (parent()->activeDeadbody()) {
                parent()->becomeDeadbody();
            } else {
                parent()->findComponent<PhysicsBodyComponent>()->removeFromParent();
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

        if (parent()->stunned()) {
            ac_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        biteTime_ -= dt;

        if (!target()) {
            ac_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        if (!targetFound_) {
            targetFound_ = true;

            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(settings.babySpider.turnSpeed);
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(2.0f);
            avoidBehavior_->setLinearVelocity(walkSpeed_);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(5.0f);
            detourBehavior_->setAngularVelocity(settings.babySpider.turnSpeed);
            detourBehavior_->setTimeout(0.1f);
            detourBehavior_->start();
        }

        bool detouring = !detourBehavior_->finished();

        if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
            detourBehavior_->start();
        }

        ac_->setPaused(false);

        if (detouring) {
            interceptBehavior_->setTarget(SceneObjectPtr());
        } else {
            interceptBehavior_->setTarget(target());
        }

        if (parent()->linearVelocity().Length() < walkSpeed_) {
            parent()->applyForceToCenter(parent()->mass() *
                parent()->getDirection(walkSpeed_ * 1.0f + parent()->linearVelocityDamped()), true);
        }

        if ((biteTime_ <= 0.0f) &&
            (b2DistanceSquared(parent()->pos(), target()->pos()) <= (2.1f * 2.1f)) &&
            (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) <= deg2rad(45))) {
            if (!trapped_) {
                target()->changeLife(-biteDamage_);
                sndBite_->play();
            }
            biteTime_ = settings.babySpider.biteTime;
        }
    }

    void BabySpiderComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void BabySpiderComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        ac_.reset();
        interceptBehavior_.reset();
        avoidBehavior_.reset();
        detourBehavior_.reset();
    }
}
