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

#include "HomerComponent.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "FadeOutComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    HomerComponent::HomerComponent(const AnimationComponentPtr& ac,
        const b2Vec2& meleePos, float meleeRadius)
    : TargetableComponent(phaseThink),
      ac_(ac),
      meleePos_(meleePos),
      meleeRadius_(meleeRadius),
      state_(StateIdle),
      t_(0.0f),
      rotateT_(0.0f),
      hadTarget_(false),
      rotateDir_((rand() % 2) == 0),
      missileDone_(false),
      meleeDone_(false)
    {
        sndMissile_[0] = audio.createSound("homer_missile1.ogg");
        sndGun_[0] = audio.createSound("homer_gun1.ogg");
        sndGun_[1] = audio.createSound("homer_gun2.ogg");
        sndMelee_[0] = audio.createSound("homer_melee1.ogg");
        sndMelee_[1] = audio.createSound("homer_melee2.ogg");
    }

    HomerComponent::~HomerComponent()
    {
    }

    void HomerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void HomerComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createHomerGibbed();

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
                        boost::make_shared<FadeOutComponent>(0.5f, 1.2f);

                    (*it)->addComponent(component);
                }
            }

            scene()->addObjectUnparent(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.homer.explosionImpulse, settings.homer.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(9.0f, zOrderBack, parent()->activeDeadbody());

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

        t_ -= dt;

        updateAutoTarget(dt);

        if (target() && !hadTarget_) {
            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(settings.homer.turnSpeed);
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(6.0f);
            avoidBehavior_->setLinearVelocity(settings.homer.walkSpeed);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(8.0f);
            detourBehavior_->setAngularVelocity(settings.homer.turnSpeed);
            detourBehavior_->setTimeout(0.05f);
            detourBehavior_->setRayDistance(1.0f);
            detourBehavior_->start();

            hadTarget_ = true;

            t_ = getRandom(settings.homer.attackLargeTimeout, 2.0f * settings.homer.attackLargeTimeout);
        } else if (!target() && hadTarget_) {
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();

            hadTarget_ = false;
        }

        bool inSight = false;

        if (target()) {
            bool detouring = !detourBehavior_->finished();

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > 8.0f * 8.0f)) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(target());
            }

            b2Vec2 v = b2Vec2_zero;

            inSight = scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 1.0f, 1.0f);

            if (!detouring && inSight) {
                rotateT_ -= dt;

                if (rotateT_ <= 0.0f) {
                    rotateDir_ = (rand() % 2) == 0;
                    rotateT_ = 1.0f;
                }

                float ang = b2_pi * dt / 2.0f;

                if (rotateDir_) {
                    ang = -ang;
                }

                v = target()->pos() - parent()->pos();
                v = b2Mul(b2Rot(ang), v) - v;
                v.Normalize();
            } else {
                rotateT_ = 0.0f;
            }

            if (parent()->linearVelocity().Length() < settings.homer.walkSpeed) {
                if (detouring || !inSight ||
                    (b2DistanceSquared(parent()->pos(), target()->pos()) > settings.homer.closeRadius * settings.homer.closeRadius)) {
                    v += parent()->getDirection(1.0f);
                    v.Normalize();
                }

                parent()->applyForceToCenter(parent()->mass() *
                    (settings.homer.walkSpeed * 1.0f + parent()->linearVelocityDamped()) * v, true);
            }
        }

        switch (state_) {
        case StateIdle: {
            if (!target()) {
                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= meleePos_.LengthSquared()) {
                float angle = fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f)));
                if (angle < deg2rad(45.0f)) {
                    ac_->startAnimation(AnimationMelee);
                    state_ = StateMelee;
                    meleeDone_ = false;
                    break;
                }
            }

            if (t_ <= 0.0f) {
                if (inSight) {
                    float angle = fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f)));
                    if (angle < settings.homer.attackAngle) {
                        if ((rand() % 2) == 0) {
                            ac_->startAnimation(AnimationPreMissile);
                            state_ = StatePreMissile;
                            if ((rand() % 3) == 0) {
                                sndMissile_[0]->play();
                            }
                        } else {
                            ac_->startAnimation(AnimationPreGun);
                            state_ = StatePreGun;
                            if ((rand() % 3) == 0) {
                                sndGun_[rand() % 2]->play();
                            }
                        }
                        break;
                    }
                    t_ = getRandom(settings.homer.attackSmallTimeout, 2.0f * settings.homer.attackSmallTimeout);
                } else {
                    t_ = getRandom(settings.homer.attackLargeTimeout, 2.0f * settings.homer.attackLargeTimeout);
                }
            }
            break;
        }
        case StatePreGun: {
            if (!ac_->animationFinished()) {
                break;
            }
            ac_->startAnimation(AnimationGun);
            state_ = StateGun;
            t_ = getRandom(settings.homer.gunDuration, 2.0f * settings.homer.gunDuration);
            weaponGun_->trigger(true);
            break;
        }
        case StateGun: {
            if (t_ >= 0.0f) {
                break;
            }
            ac_->startAnimation(AnimationPostGun);
            state_ = StatePostGun;
            weaponGun_->trigger(false);
            break;
        }
        case StatePostGun: {
            if (!ac_->animationFinished()) {
                break;
            }
            ac_->startAnimation(AnimationDefault);
            state_ = StateIdle;
            t_ = getRandom(settings.homer.attackLargeTimeout, 2.0f * settings.homer.attackLargeTimeout);
            break;
        }
        case StatePreMissile: {
            if (!ac_->animationFinished()) {
                break;
            }
            ac_->startAnimation(AnimationMissile);
            state_ = StateMissile;
            missileDone_ = false;
            break;
        }
        case StateMissile: {
            if (!missileDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                weaponMissile_->setTarget(target());
                weaponMissile_->triggerOnce();
                missileDone_ = true;
            }
            if (!ac_->animationFinished()) {
                break;
            }
            ac_->startAnimation(AnimationPostMissile);
            state_ = StatePostMissile;
            break;
        }
        case StatePostMissile: {
            if (!ac_->animationFinished()) {
                break;
            }
            ac_->startAnimation(AnimationDefault);
            state_ = StateIdle;
            t_ = getRandom(settings.homer.attackLargeTimeout, 2.0f * settings.homer.attackLargeTimeout);
            break;
        }
        case StateMelee: {
            if (!meleeDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.homer.meleeDamage);
                }
                meleeDone_ = true;
                sndMelee_[rand() % 2]->play();
            }

            if (!ac_->animationFinished()) {
                break;
            }

            ac_->startAnimation(AnimationDefault);
            state_ = StateIdle;
            t_ = getRandom(settings.homer.attackLargeTimeout, 2.0f * settings.homer.attackLargeTimeout);
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void HomerComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void HomerComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
