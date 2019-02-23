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

#include "WarderComponent.h"
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
    WarderComponent::WarderComponent(const AnimationComponentPtr& ac,
        const b2Vec2 meleePos[3], const float meleeRadius[3])
    : TargetableComponent(phaseThink),
      ac_(ac),
      state_(StateIdle),
      t_(0.0f),
      actionDone_(false)
    {
        for (int i = 0; i < 3; ++i) {
            meleePos_[i] = meleePos[i];
            meleeRadius_[i] = meleeRadius[i];
        }

        sndMelee_[0] = audio.createSound("warder_melee1.ogg");
        sndMelee_[1] = audio.createSound("warder_melee2.ogg");
        sndMelee_[2] = audio.createSound("warder_melee3.ogg");

        sndShoot_[0] = audio.createSound("warder_shoot1.ogg");
        sndShoot_[1] = audio.createSound("warder_shoot2.ogg");

        sndWalk_[0] = audio.createSound("warder_walk1.ogg");
        sndWalk_[1] = audio.createSound("warder_walk2.ogg");
        sndWalk_[2] = audio.createSound("warder_walk3.ogg");
    }

    WarderComponent::~WarderComponent()
    {
    }

    void WarderComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WarderComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createWarderGibbed();

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
                settings.warder.explosionImpulse, settings.warder.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(8.0f, zOrderBack, parent()->activeDeadbody());

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

        switch (state_) {
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.warder.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(5.0f);
                avoidBehavior_->setLinearVelocity(settings.warder.walkSpeed);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.warder.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateWalk;

                ac_->startAnimation(AnimationWalk);

                resetShootTime();

                if ((rand() % 5) == 0) {
                    sndWalk_[rand() % 3]->play();
                }
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                ac_->startAnimation(AnimationDefault);

                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= (settings.warder.meleeRadius * settings.warder.meleeRadius)) {
                detourBehavior_->reset();
                interceptBehavior_->setTarget(target());

                int i = rand() % 3;

                state_ = static_cast<State>(StateMelee1 + i);

                ac_->startAnimation(AnimationMelee1 + i);

                actionDone_ = false;

                break;
            }

            if (t_ <= 0.0f) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                    detourBehavior_->reset();
                    interceptBehavior_->setTarget(target());

                    state_ = StateShoot;

                    ac_->startAnimation(AnimationShoot);

                    actionDone_ = false;

                    sndShoot_[rand() % 2]->play();

                    break;
                } else {
                    resetShootTime();
                }
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (8.0f * 8.0f))) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(target());
            }

            if (parent()->linearVelocity().Length() < settings.warder.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(settings.warder.walkSpeed * 2.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateMelee1:
        case StateMelee2:
        case StateMelee3: {
            int i = state_ - StateMelee1;

            if (ac_->animationFinished()) {
                if (target() && (b2DistanceSquared(parent()->pos(), target()->pos()) <= (settings.warder.meleeRadius * settings.warder.meleeRadius))) {
                    interceptBehavior_->setTarget(target());

                    int i = rand() % 3;

                    state_ = static_cast<State>(StateMelee1 + i);

                    ac_->startAnimation(AnimationMelee1 + i);

                    actionDone_ = false;

                    break;
                }

                interceptBehavior_->reset();
                avoidBehavior_->reset();

                state_ = StateIdle;

                ac_->startAnimation(AnimationDefault);

                break;
            }

            if (!actionDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                sndMelee_[i]->play();

                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_[i]), target()->pos()) <= meleeRadius_[i] * meleeRadius_[i])) {
                    target()->changeLife(-settings.warder.meleeDamage[i]);
                }

                actionDone_ = true;
            }

            break;
        }
        case StateShoot: {
            if (!ac_->animationFinished()) {
                break;
            }
            if (!actionDone_) {
                weapon_->trigger(true);
                actionDone_ = true;
                t_ = getRandom(settings.warder.shootDuration * 0.5f, settings.warder.shootDuration);
            }
            if (t_ <= 0.0f) {
                weapon_->trigger(false);

                interceptBehavior_->reset();
                avoidBehavior_->reset();

                state_ = StateIdle;

                ac_->startAnimation(AnimationDefault);
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void WarderComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void WarderComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    void WarderComponent::resetShootTime()
    {
        t_ = getRandom(settings.warder.shootTimeout * 0.5f, settings.warder.shootTimeout);
    }
}
