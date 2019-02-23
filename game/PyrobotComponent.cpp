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

#include "PyrobotComponent.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    AudioSourcePtr PyrobotComponent::snd_;

    PyrobotComponent::PyrobotComponent(const AnimationComponentPtr& ac,
        const b2Transform& weapon1xf, const b2Transform& weapon2xf)
    : TargetableComponent(phaseThink),
      ac_(ac),
      state_(StateIdle),
      talked_(false)
    {
        weapons_[0] = boost::make_shared<WeaponFlamethrowerComponent>(SceneObjectTypeEnemyMissile);
        weapons_[0]->setTransform(weapon1xf);
        weapons_[0]->setDamage(settings.pyrobot.shootDamage);

        weapons_[1] = boost::make_shared<WeaponFlamethrowerComponent>(SceneObjectTypeEnemyMissile);
        weapons_[1]->setTransform(weapon2xf);
        weapons_[1]->setDamage(settings.pyrobot.shootDamage);
        weapons_[1]->setHaveSound(false);

        if (!snd_) {
            snd_ = audio.createSound("pyrobot_attack.ogg");
        }
    }

    PyrobotComponent::~PyrobotComponent()
    {
    }

    void PyrobotComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PyrobotComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            scene()->addObject(explosion);

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

        switch (state_) {
        case StateIdle: {
            if (!target()) {
                break;
            }

            if (patrol() && b2DistanceSquared(parent()->pos(), target()->pos()) <= 3.0f) {
                break;
            }

            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(settings.pyrobot.turnSpeed * 1.0f);
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(6.0f);
            avoidBehavior_->setLinearVelocity(settings.pyrobot.walkSpeed * 0.2f);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            if (shouldAttack()) {
                ac_->startAnimation(AnimationAttack);

                state_ = StateAttack;

                if (!talked_ && (snd_->status() != AudioSource::Playing)) {
                    snd_->play();
                }
                talked_ = true;
            } else {
                state_ = StateWalk;
                ac_->startAnimation(AnimationWalk);

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.pyrobot.turnSpeed * 1.0f);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->setRayDistance(1.0f);
                detourBehavior_->start();
            }
            break;
        }
        case StateWalk: {
            if (!target() || (patrol() && b2DistanceSquared(parent()->pos(), target()->pos()) <= 3.0f)) {
                ac_->startAnimation(AnimationDefault);

                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            if (shouldAttack()) {
                ac_->startAnimation(AnimationAttack);

                interceptBehavior_->setTarget(target());
                detourBehavior_->reset();

                state_ = StateAttack;

                if (!talked_ && (snd_->status() != AudioSource::Playing)) {
                    snd_->play();
                }
                talked_ = true;

                break;
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring) {
                detourBehavior_->start();
            }

            float walkSpeed;

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
                walkSpeed = settings.pyrobot.walkSpeed * 1.0f;
            } else {
                interceptBehavior_->setTarget(target());
                walkSpeed = settings.pyrobot.walkSpeed;
            }

            if (parent()->linearVelocity().Length() < walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(walkSpeed + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateAttack:
            if (!target() ||
                (b2DistanceSquared(parent()->pos(), target()->pos()) > (settings.pyrobot.attackRadiusMax * settings.pyrobot.attackRadiusMax)) ||
                patrol()) {
                ac_->startAnimation(AnimationDefault);

                interceptBehavior_->reset();
                avoidBehavior_->reset();

                state_ = StateIdle;

                weapons_[0]->trigger(false);
                weapons_[1]->trigger(false);

                break;
            }

            interceptBehavior_->setTarget(target());

            weapons_[0]->trigger(true);
            weapons_[1]->trigger(true);

            break;
        default:
            assert(false);
            break;
        }
    }

    void PyrobotComponent::onRegister()
    {
        parent()->addComponent(weapons_[0]);
        parent()->addComponent(weapons_[1]);
    }

    void PyrobotComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        ac_.reset();
        interceptBehavior_.reset();
        avoidBehavior_.reset();
        detourBehavior_.reset();
    }

    bool PyrobotComponent::shouldAttack()
    {
        if (patrol()) {
            return false;
        }

        if (b2DistanceSquared(parent()->pos(), target()->pos()) > (settings.pyrobot.attackRadiusMin * settings.pyrobot.attackRadiusMin)) {
            return false;
        }

        return true;
    }
}
