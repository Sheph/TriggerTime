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

#include "ScorpComponent.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    ScorpComponent::ScorpComponent(const AnimationComponentPtr& ac, float walkSpeed)
    : TargetableComponent(phaseThink),
      ac_(ac),
      walkSpeed_(walkSpeed),
      state_(StateIdle),
      t_(0.0f),
      fired_(false),
      weapon_(boost::make_shared<WeaponFireballComponent>(SceneObjectTypeEnemyMissile))
    {
        weapon_->setVelocity(settings.scorp.shootVelocity);
        weapon_->setDamage(settings.scorp.shootDamage);
        weapon_->setInterval(settings.scorp.attackTime);
    }

    ScorpComponent::~ScorpComponent()
    {
    }

    void ScorpComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ScorpComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion3(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.scorp.explosionImpulse, settings.scorp.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

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

        t_ -=  dt;

        switch (state_) {
        case StateIdle: {
            if (parent()->stunned()) {
                state_ = StateStunned;
            } else if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.scorp.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                state_ = StateStand;
            }
            break;
        }
        case StateStand:
            if (parent()->stunned()) {
                interceptBehavior_->reset();
                state_ = StateStunned;
            } else if (!target()) {
                interceptBehavior_->reset();
                state_ = StateIdle;
            } else if (shouldAttack()) {
                ac_->startAnimation(AnimationAttack);

                interceptBehavior_->reset();

                state_ = StateAttack;
            } else if (b2DistanceSquared(parent()->pos(), target()->pos()) >= (settings.scorp.minRadius * settings.scorp.minRadius)) {
                state_ = StateWalk;
                ac_->startAnimation(AnimationWalk);

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(3.5f);
                avoidBehavior_->setLinearVelocity(walkSpeed_);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.scorp.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();
            }
            interceptBehavior_->setTarget(target());
            break;
        case StateWalk: {
            if (parent()->stunned()) {
                ac_->setPaused(true);

                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateStunned;
                break;
            }

            if (!target()) {
                ac_->startAnimation(AnimationDefault);

                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            if (shouldAttack()) {
                ac_->startAnimation(AnimationAttack);

                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateAttack;

                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) < (settings.scorp.minRadius * settings.scorp.minRadius)) {
                interceptBehavior_->setTarget(target());

                avoidBehavior_->reset();
                detourBehavior_->reset();

                ac_->startAnimation(AnimationDefault);

                state_ = StateStand;

                break;
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

            if (parent()->linearVelocity().Length() < walkSpeed_) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(walkSpeed_ + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateAttack:
            if (parent()->stunned()) {
                ac_->setPaused(true);

                state_ = StateStunned;

                fired_ = false;

                break;
            }

            if ((ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0)) && !fired_) {
                weapon_->triggerOnce();
                fired_ = true;
            }

            if (ac_->animationFinished()) {
                t_ = weapon_->interval();

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;

                fired_ = false;

                break;
            }

            break;
        case StateStunned:
            if (!parent()->stunned()) {
                ac_->setPaused(false);

                t_ = weapon_->interval();

                ac_->startAnimation(AnimationDefault);

                state_ = StateIdle;
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    void ScorpComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        parent()->addComponent(weapon_);
    }

    void ScorpComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        ac_.reset();
        interceptBehavior_.reset();
        avoidBehavior_.reset();
        detourBehavior_.reset();
    }

    bool ScorpComponent::shouldAttack()
    {
        if (t_ > 0.0f) {
            return false;
        }

        if (b2DistanceSquared(parent()->pos(), target()->pos()) > (settings.scorp.attackRadius * settings.scorp.attackRadius)) {
            return false;
        }

        float angle = fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos()));

        return (angle <= settings.scorp.shootAngle);
    }
}
