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

#include "EnforcerComponent.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "PhysicsBodyComponent.h"
#include "Const.h"
#include "Settings.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    EnforcerComponent::EnforcerComponent(const RenderQuadComponentPtr& torsoRc,
        const RenderQuadComponentPtr& torsoMaskRc,
        const RenderQuadComponentPtr& legsRc,
        const AnimationComponentPtr& torsoAc,
        const AnimationComponentPtr& torsoMaskAc,
        const AnimationComponentPtr& legsAc,
        float shootInterval)
    : TargetableComponent(phaseThink),
      torsoRc_(torsoRc),
      torsoMaskRc_(torsoMaskRc),
      legsRc_(legsRc),
      torsoAc_(torsoAc),
      torsoMaskAc_(torsoMaskAc),
      legsAc_(legsAc),
      shootInterval_(shootInterval),
      targetFound_(false),
      sndDie_(audio.createSound("enforcer_die.ogg")),
      shootTime_(0.0f),
      readyToShoot_(false),
      detour_(true),
      targetDistance_(4.0f)
    {
    }

    EnforcerComponent::~EnforcerComponent()
    {
    }

    void EnforcerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void EnforcerComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            if (torsoAc_->currentAnimation() == AnimationDefault) {
                parent()->setFreezable(false);

                if (interceptBehavior_) {
                    interceptBehavior_->reset();
                    avoidBehavior_->reset();
                    detourBehavior_->reset();
                    seekBehavior_->reset();
                }
                weapon_->trigger(false);

                legsRc_->removeFromParent();
                legsAc_->removeFromParent();

                torsoAc_->setPaused(false);
                torsoMaskAc_->setPaused(false);
                torsoAc_->startAnimation(AnimationDie);
                torsoMaskAc_->startAnimation(AnimationDie);

                torsoRc_->setZOrder(zOrderBack + 1);
                torsoMaskRc_->setZOrder(zOrderBack + 2);

                SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(8.0f, zOrderBack, parent()->activeDeadbody());

                bs->setPos(parent()->pos());
                bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(bs);

                parent()->setLinearDamping(5.0f);
                parent()->setAngularDamping(5.0f);

                scene()->spawnPowerup(parent()->pos());

                sndDie_->play();

                return;
            }

            if (!torsoAc_->animationFinished()) {
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

            return;
        }

        if (parent()->linearVelocity().LengthSquared() >= 2.0f) {
            legsRc_->setAngle(vec2angle(parent()->linearVelocity()) - (b2_pi / 2.0f));
            torsoAc_->setPaused(false);
            torsoMaskAc_->setPaused(false);
            legsAc_->setPaused(false);
        } else {
            torsoAc_->setPaused(true);
            torsoMaskAc_->setPaused(true);
            legsAc_->setPaused(true);
            torsoAc_->startAnimation(AnimationDefault);
            torsoMaskAc_->startAnimation(AnimationDefault);
            legsAc_->startAnimation(AnimationDefault);
            legsRc_->setAngle(vec2angle(parent()->getDirection(1.0f)) - (b2_pi / 2.0f));
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

        if (!seekBehavior_) {
            seekBehavior_ = parent()->seekBehavior();
        }

        updateAutoTarget(dt);

        if (!target()) {
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            seekBehavior_->reset();
            weapon_->trigger(false);
            readyToShoot_ = false;
            return;
        }

        if (!targetFound_) {
            targetFound_ = true;

            interceptBehavior_->reset();
            interceptBehavior_->setLinearVelocity(settings.enforcer.walkSpeed);
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(2.0f);
            avoidBehavior_->setLinearVelocity(settings.enforcer.walkSpeed);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            if (detour_) {
                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(5.0f);
                detourBehavior_->setAngularVelocity(settings.enforcer.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();
            }

            seekBehavior_->reset();
            seekBehavior_->setAngularVelocity(settings.enforcer.turnSpeed);
            seekBehavior_->setTarget(target());
            seekBehavior_->setUseTorque(true);
            seekBehavior_->start();
        }

        bool detouring = detour_ ? !detourBehavior_->finished() : false;

        if (detour_ && !detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
            detourBehavior_->start();
        }

        weapon_->trigger(false);

        bool readyToShoot = false;

        if (detouring) {
            interceptBehavior_->setTarget(SceneObjectPtr());
            seekBehavior_->setTarget(SceneObjectPtr());

            if (parent()->linearVelocity().Length() < settings.enforcer.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(settings.enforcer.walkSpeed + parent()->linearVelocityDamped()), true);
            }
        } else {
            if (patrol()) {
                interceptBehavior_->setTarget(SceneObjectPtr());

                if ((b2DistanceSquared(parent()->pos(), target()->pos()) > targetDistance_ * targetDistance_) &&
                    (parent()->linearVelocity().Length() < settings.enforcer.walkSpeed)) {
                    parent()->applyForceToCenter(parent()->mass() *
                        parent()->getDirection(settings.enforcer.walkSpeed + parent()->linearVelocityDamped()), true);
                }
            } else if (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) > settings.enforcer.shootAngle) {
                interceptBehavior_->setTarget(SceneObjectPtr());

                if (parent()->linearVelocity().Length() < settings.enforcer.walkSpeed) {
                    parent()->applyForceToCenter(parent()->mass() *
                        parent()->getDirection(settings.enforcer.walkSpeed + parent()->linearVelocityDamped()), true);
                }
            } else {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                    parent()->pos(), 3.0f, 3.0f)) {
                    readyToShoot = true;
                }

                if (b2DistanceSquared(parent()->pos(), target()->pos()) > (settings.enforcer.minRadius * settings.enforcer.minRadius)) {
                    interceptBehavior_->setTarget(target());
                } else {
                    interceptBehavior_->setTarget(SceneObjectPtr());
                }
            }
            seekBehavior_->setTarget(target());
        }

        if (readyToShoot) {
            if (!readyToShoot_) {
                shootTime_ = getRandom(0.0f, shootInterval_ * 1.5f);
            }

            if (shootTime_ <= 0.0f) {
                if (!patrol()) {
                    weapon_->trigger(true);
                }
            }

            shootTime_ -= dt;
        }

        readyToShoot_ = readyToShoot;
    }

    void EnforcerComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void EnforcerComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
