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

#include "KeeperComponent.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "Const.h"
#include "Settings.h"
#include "ExplosionComponent.h"
#include "FadeOutComponent.h"
#include "RenderHealthbarComponent.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    KeeperComponent::KeeperComponent(const AnimationComponentPtr& ac,
        const b2Vec2& meleePos, float meleeRadius, bool folded)
    : TargetableComponent(phaseThink),
      ac_(ac),
      meleePos_(meleePos),
      meleeRadius_(meleeRadius),
      state_(folded ? StateFolded : StateIdle),
      attackTime_(0.0f),
      weaponStarted_(false),
      weaponDone_(false),
      firstAngry_(!folded),
      unfold_(false),
      origZOrder_(0),
      sndAngry_(audio.createSound("keeper_angry.ogg")),
      sndMelee_(audio.createSound("keeper_melee.ogg")),
      sndDeath_(audio.createSound("keeper_death.ogg")),
      sndCrawlOut_(audio.createSound("keeper_crawlout.ogg"))
    {
        sndShoot_[0] = audio.createSound("keeper_shoot1.ogg");
        sndShoot_[1] = audio.createSound("keeper_shoot2.ogg");
    }

    KeeperComponent::~KeeperComponent()
    {
    }

    void KeeperComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void KeeperComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            if (ac_->currentAnimation() != AnimationDeath) {
                parent()->setFreezable(false);

                scene()->camera()->findComponent<CameraComponent>()->tremor(false);

                if (interceptBehavior_) {
                    interceptBehavior_->reset();
                    avoidBehavior_->reset();
                    detourBehavior_->reset();
                }
                weaponGun_->trigger(false);
                weaponPlasma_->trigger(false);
                weaponMissile_->trigger(false);

                ac_->startAnimation(AnimationDeath);

                RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }

                weaponStarted_ = false;

                sndDeath_->play();

                return;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                parent()->findComponent<RenderQuadComponent>()->setZOrder(zOrderBack + 1);

                if (parent()->activeDeadbody()) {
                    parent()->becomeDeadbody();
                } else {
                    parent()->findComponent<PhysicsBodyComponent>()->removeFromParent();
                }
                weaponStarted_ = true;
                sndMelee_->play();
                scene()->camera()->findComponent<CameraComponent>()->shake(2.0f);
            }

            if (!ac_->animationFinished()) {
                return;
            }

            if (!parent()->activeDeadbody()) {
                parent()->setFreezable(true);
                parent()->setFreezeRadius(4.0f);
            }

            ac_->removeFromParent();

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

        attackTime_ -= dt;

        switch (state_) {
        case StateFolded: {
            if (!unfold_) {
                break;
            }

            if (ac_->paused()) {
                ac_->setPaused(false);
                weaponStarted_ = false;
                sndCrawlOut_->play();
                parent()->setActive(true);
                parent()->findComponent<RenderQuadComponent>()->setZOrder(zOrderBack + 1);
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                parent()->findComponent<RenderQuadComponent>()->setZOrder(origZOrder_);
                weaponStarted_ = true;
            }

            if (ac_->animationFinished()) {
                ac_->startAnimation(AnimationDefault);
                state_ = StateIdle;
                parent()->setInvulnerable(false);

                RenderHealthbarComponentPtr component =
                    boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.0f), 0, 6.0f, 1.0f, zOrderMarker + 1);

                component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                parent()->addComponent(component);
            }

            break;
        }
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.keeper.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemyBuilding);
                avoidBehavior_->setRadius(9.0f);
                avoidBehavior_->setLinearVelocity(settings.keeper.walkSpeed * 0.3f);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setRayDistance(1.75f);
                detourBehavior_->setAngularVelocity(settings.keeper.turnSpeed);
                detourBehavior_->setTimeout(0.0f);
                detourBehavior_->start();

                ac_->startAnimation(AnimationWalk);

                state_ = StateWalk;

                break;
            }
            break;
        }
        case StateAngry: {
            if (ac_->animationFinished()) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);

                break;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                sndAngry_->play();
                scene()->camera()->findComponent<CameraComponent>()->tremor(true);
                weaponStarted_ = true;
            }

            if (!weaponDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(1))) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                weaponDone_ = true;
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                ac_->startAnimation(AnimationDefault);

                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                state_ = StateIdle;

                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= meleePos_.LengthSquared()) {
                detourBehavior_->reset();
                interceptBehavior_->setTarget(target());

                state_ = StateCloseup;

                break;
            }

            bool detouring = !detourBehavior_->finished();

            if ((attackTime_ <= 0.0f) && !detouring &&
                (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.keeper.attackAngle)) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 4.0f, 4.0f)) {
                    detourBehavior_->reset();
                    avoidBehavior_->reset();
                    interceptBehavior_->setTarget(target());

                    int r = rand() % 10;

                    if (firstAngry_) {
                        firstAngry_ = false;
                        r = 9;
                    }

                    if ((r >= 0) && (r <= 2)) {
                        startGun();
                    } else if ((r >= 3) && (r <= 5)) {
                        startPlasma();
                    } else if ((r >= 6) && (r <= 8)) {
                        startMissile();
                    } else {
                        detourBehavior_->reset();
                        interceptBehavior_->reset();
                        avoidBehavior_->reset();

                        parent()->setAngularVelocity(0.0f);

                        ac_->startAnimation(AnimationAngry);

                        state_ = StateAngry;

                        weaponStarted_ = false;
                        weaponDone_ = false;
                    }

                    break;
                } else {
                    attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);
                }
            }

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > 8.0f * 8.0f)) {
                detourBehavior_->start();
            }

            float walkSpeed;

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
                walkSpeed = settings.keeper.walkSpeed;
            } else {
                interceptBehavior_->setTarget(target());
                walkSpeed = settings.keeper.walkSpeed;
            }

            if (parent()->linearVelocity().Length() < settings.keeper.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(walkSpeed * 4.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateGun: {
            if (ac_->animationFinished()) {
                interceptBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);

                break;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                weaponGun_->trigger(true);
                weaponStarted_ = true;
            }

            if (!weaponDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(1))) {
                weaponGun_->trigger(false);
                weaponDone_ = true;
            }

            break;
        }
        case StatePrePlasma: {
            if (ac_->animationFinished()) {
                state_ = StatePlasma;

                ac_->startAnimation(AnimationPlasma);

                attackTime_ = settings.keeper.plasmaDuration;

                break;
            }
            break;
        }
        case StatePlasma: {
            if (((attackTime_ <= 0.0f) ||
                 !scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 13.0f, 13.0f) ||
                 !target() ||
                 target()->dead() ||
                 (b2DistanceSquared(parent()->pos(), target()->pos()) <= meleePos_.LengthSquared())) &&
                (ac_->animationFrameIndex() == 0)) {
                state_ = StatePostPlasma;

                ac_->startAnimation(AnimationPostPlasma);

                break;
            }

            if (ac_->animationFrameIndex() > 0) {
                weaponStarted_ = false;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() == 0)) {
                weaponPlasma_->triggerOnce();
                weaponStarted_ = true;
            }

            break;
        }
        case StatePostPlasma: {
            if (ac_->animationFinished()) {
                interceptBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);

                break;
            }
            break;
        }
        case StateMissile: {
            if (ac_->animationFinished()) {
                interceptBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);

                break;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                weaponMissile_->triggerOnce();
                weaponStarted_ = true;
            }

            break;
        }
        case StateMelee: {
            if (ac_->animationFinished()) {
                interceptBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.keeper.attackDelayMin, settings.keeper.attackDelayMax);

                break;
            }

            if (!weaponStarted_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                sndMelee_->play();

                scene()->camera()->findComponent<CameraComponent>()->shake(2.0f);

                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.keeper.meleeDamage);
                }

                weaponStarted_ = true;
            }

            break;
        }
        case StateCloseup: {
            if (!target() || (b2DistanceSquared(parent()->pos(), target()->pos()) > meleePos_.LengthSquared())) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                state_ = StateIdle;

                break;
            }

            if (((rand() % 3) != 0) && (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.keeper.attackAngle)) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();

                startMelee();

                break;
            } else if (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.keeper.attackAngle) {
                avoidBehavior_->reset();

                startMissile();

                break;
            }

            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void KeeperComponent::onFreeze()
    {
        scene()->camera()->findComponent<CameraComponent>()->tremor(false);
    }

    void KeeperComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        if (state_ == StateFolded) {
            parent()->setInvulnerable(true);
            parent()->setActive(false);
            origZOrder_ = parent()->findComponent<RenderQuadComponent>()->zOrder();
            parent()->findComponent<RenderQuadComponent>()->setZOrder(zOrderBack - 5);
            ac_->startAnimation(AnimationCrawlOut);
            ac_->setPaused(true);
        } else {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.0f), 0, 6.0f, 1.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            parent()->addComponent(component);
        }
    }

    void KeeperComponent::onUnregister()
    {
        scene()->camera()->findComponent<CameraComponent>()->tremor(false);
        setTarget(SceneObjectPtr());
    }

    void KeeperComponent::startGun()
    {
        ac_->startAnimation(AnimationGun);

        state_ = StateGun;

        weaponStarted_ = false;
        weaponDone_ = false;

        sndShoot_[rand() % 2]->play();
    }

    void KeeperComponent::startPlasma()
    {
        ac_->startAnimation(AnimationPrePlasma);

        state_ = StatePrePlasma;

        weaponStarted_ = false;

        sndShoot_[rand() % 2]->play();
    }

    void KeeperComponent::startMissile()
    {
        ac_->startAnimation(AnimationMissile);

        state_ = StateMissile;

        weaponStarted_ = false;

        sndShoot_[rand() % 2]->play();
    }

    void KeeperComponent::startMelee()
    {
        parent()->setAngularVelocity(0.0f);

        ac_->startAnimation(AnimationMelee);

        state_ = StateMelee;

        weaponStarted_ = false;

        sndShoot_[rand() % 2]->play();
    }
}
