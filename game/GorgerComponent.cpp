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

#include "GorgerComponent.h"
#include "Utils.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "Const.h"
#include "Settings.h"
#include "ExplosionComponent.h"
#include "FadeOutComponent.h"
#include "RenderHealthbarComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    GorgerComponent::GorgerComponent(const AnimationComponentPtr& ac,
        const b2Vec2& meleePos, float meleeRadius)
    : TargetableComponent(phaseThink),
      ac_(ac),
      meleePos_(meleePos),
      meleeRadius_(meleeRadius),
      state_(StateIdle),
      meleeDone_(false),
      shootDone_(false),
      attackTime_(0.0f),
      firstAngry_(false),
      sndMelee_(audio.createSound("gorger_melee.ogg")),
      sndHit_(audio.createSound("gorger_hit.ogg")),
      sndAngry_(audio.createSound("gorger_angry.ogg"))
    {
        sndShoot_[0] = audio.createSound("gorger_shoot1.ogg");
        sndShoot_[1] = audio.createSound("gorger_shoot2.ogg");
        weaponDone_[0] = weaponDone_[1] = false;
    }

    GorgerComponent::~GorgerComponent()
    {
    }

    void GorgerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GorgerComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createGorgerGibbed();

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(15.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            if (gibbed->type() != SceneObjectTypeGarbage) {
                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.2f);

                gibbed->addComponent(component);
            }

            gibbed->setLinearVelocity(parent()->linearVelocity());
            gibbed->setAngularVelocity(b2_pi * 8.0f);

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);
                (*it)->setAngularVelocity(b2_pi * 4.0f * (((rand() % 2) == 0) ? 1.0f : -1.0f));

                if ((*it)->type() != SceneObjectTypeGarbage) {
                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

                    (*it)->addComponent(component);
                }
            }

            scene()->addObjectUnparent(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.gorger.explosionImpulse, settings.gorger.explosionDamage,
                SceneObjectTypes(SceneObjectTypeEnemy) | SceneObjectTypeEnemyBuilding);

            scene()->addObject(explosion);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(10.0f, zOrderBack, parent()->activeDeadbody());

            bs->setPos(parent()->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            scene()->spawnPowerup(parent()->pos());

            scene()->stats()->incEnemiesKilled();

            parent()->removeFromParent();

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
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.gorger.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemyBuilding);
                avoidBehavior_->setRadius(6.0f);
                avoidBehavior_->setLinearVelocity(settings.gorger.walkSpeed * 0.3f);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setRayDistance(1.5f);
                detourBehavior_->setAngularVelocity(settings.gorger.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
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

                attackTime_ = getRandom(settings.gorger.attackDelayMin, settings.gorger.attackDelayMax);

                break;
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

                state_ = StateAim;

                break;
            }

            bool detouring = !detourBehavior_->finished();

            if ((attackTime_ <= 0.0f) && !detouring &&
                (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.gorger.attackAngle) &&
                scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                int r = rand() % 6;
                if (!firstAngry_) {
                    firstAngry_ = true;
                    r = 5;
                }
                if (((r == 0) || (r == 1) || (r == 2)) &&
                    (b2Distance(parent()->pos(), target()->pos()) <= meleePos_.Length() + meleeRadius_)) {
                    detourBehavior_->reset();
                    interceptBehavior_->reset();
                    avoidBehavior_->reset();

                    startMelee();
                } else if (r != 5) {
                    detourBehavior_->reset();
                    avoidBehavior_->reset();
                    interceptBehavior_->setTarget(target());

                    startShoot();
                } else {
                    detourBehavior_->reset();
                    interceptBehavior_->reset();
                    avoidBehavior_->reset();

                    parent()->setAngularVelocity(0.0f);

                    ac_->startAnimation(AnimationAngry);

                    state_ = StateAngry;

                    sndAngry_->play();

                    scene()->camera()->findComponent<CameraComponent>()->tremor(true);
                }

                break;
            }

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > 8.0f * 8.0f)) {
                detourBehavior_->start();
            }

            float walkSpeed;

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
                walkSpeed = 0.0f;
            } else {
                interceptBehavior_->setTarget(target());
                walkSpeed = settings.gorger.walkSpeed;
            }

            if (parent()->linearVelocity().Length() < settings.gorger.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(walkSpeed * 4.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateAim: {
            if (!target() || (b2DistanceSquared(parent()->pos(), target()->pos()) > meleePos_.LengthSquared())) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                state_ = StateIdle;

                break;
            }

            if (((rand() % 3) != 0) && (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.gorger.attackAngle)) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();

                startMelee();

                break;
            } else if (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= settings.gorger.attackAngle) {
                avoidBehavior_->reset();

                startShoot();

                break;
            }

            break;
        }
        case StateMelee: {
            if (ac_->animationFinished()) {
                state_ = StateIdle;

                attackTime_ = getRandom(settings.gorger.attackDelayMin, settings.gorger.attackDelayMax);

                break;
            }

            if (!meleeDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                sndHit_->play();
                scene()->camera()->findComponent<CameraComponent>()->shake(2.0f);

                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.gorger.meleeDamage);
                }

                meleeDone_ = true;
            }

            break;
        }
        case StatePreShoot: {
            if (ac_->animationFinished()) {
                state_ = StateShoot;

                shootDone_ = false;
                weaponDone_[0] = weaponDone_[1] = false;

                ac_->startAnimation(AnimationShoot);

                break;
            }
            interceptBehavior_->setAngularVelocity(settings.gorger.turnSpeed * (2.0f / 3.0f));
            break;
        }
        case StateShoot: {
            if (ac_->animationFinished()) {
                if (!shootDone_ && (getRandom(0.0f, 1.0f) < settings.gorger.shootRepeatProbability)) {
                    ac_->startAnimation(AnimationShoot);

                    shootDone_ = true;
                    weaponDone_[0] = weaponDone_[1] = false;
                } else {
                    state_ = StatePostShoot;

                    ac_->startAnimation(AnimationPostShoot);
                }

                break;
            }

            if (!weaponDone_[0] && (ac_->animationFrameIndex() >= 0)) {
                weapon_[0]->triggerOnce();
                weaponDone_[0] = true;
            } else if (!weaponDone_[1] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                weapon_[1]->triggerOnce();
                weaponDone_[1] = true;
            }

            break;
        }
        case StatePostShoot: {
            if (ac_->animationFinished()) {
                interceptBehavior_->reset();

                parent()->setAngularVelocity(0.0f);

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                attackTime_ = getRandom(settings.gorger.attackDelayMin, settings.gorger.attackDelayMax);

                break;
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void GorgerComponent::onFreeze()
    {
        scene()->camera()->findComponent<CameraComponent>()->tremor(false);
    }

    void GorgerComponent::setWalk()
    {
        ac_->startAnimation(AnimationWalk);
    }

    void GorgerComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void GorgerComponent::onUnregister()
    {
        scene()->camera()->findComponent<CameraComponent>()->tremor(false);
        setTarget(SceneObjectPtr());
    }

    void GorgerComponent::startMelee()
    {
        parent()->setAngularVelocity(0.0f);

        ac_->startAnimation(AnimationMelee);

        state_ = StateMelee;

        meleeDone_ = false;

        sndMelee_->play();
    }

    void GorgerComponent::startShoot()
    {
        ac_->startAnimation(AnimationPreShoot);

        state_ = StatePreShoot;

        sndShoot_[rand() % 2]->play();
    }
}
