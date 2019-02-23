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

#include "OrboComponent.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "RenderHealthbarComponent.h"
#include "FadeOutComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    OrboComponent::OrboComponent(const AnimationComponentPtr& ac)
    : TargetableComponent(phaseThink),
      ac_(ac),
      state_(StateIdle),
      t_(0.0f)
    {
    }

    OrboComponent::~OrboComponent()
    {
    }

    void OrboComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void OrboComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createOrboGibbed();

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(15.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            if (gibbed->type() != SceneObjectTypeGarbage) {
                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.7f, 1.0f);

                gibbed->addComponent(component);
            }

            gibbed->setLinearVelocity(parent()->linearVelocity());
            gibbed->setAngularVelocity((b2_pi * 8.0f));

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);

                if ((*it)->type() != SceneObjectTypeGarbage) {
                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.7f, 1.0f);

                    (*it)->addComponent(component);
                }
            }

            scene()->addObjectUnparent(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.orbo.explosionImpulse, settings.orbo.explosionDamage,
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

        if (parent()->stunned()) {
            weapon_[0]->trigger(false);
            weapon_[1]->trigger(false);
            ac_->setPaused(true);
            return;
        }

        t_ -= dt;

        ac_->setPaused(state_ == StateIdle);

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.orbo.turnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(5.0f);
                avoidBehavior_->setLinearVelocity(settings.orbo.walkSpeed);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.orbo.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateChase;
            }
            break;
        }
        case StateChase: {
            if (!target()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= settings.orbo.attackRadius * settings.orbo.attackRadius) {
                if (t_ <= 0.0f) {
                    avoidBehavior_->reset();
                    detourBehavior_->reset();
                    state_ = StateExtend;
                    ac_->startAnimation(AnimationExtend);

                    parent()->setInvulnerable(false);

                    RenderHealthbarComponentPtr component =
                        boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.0f), 0, 3.0f, 0.5f, zOrderMarker + 1);

                    component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                    parent()->addComponent(component);

                    break;
                } else {
                    interceptBehavior_->reset();
                    state_ = StateRunaway;
                    break;
                }
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(target());
            }

            if (parent()->linearVelocity().Length() < settings.orbo.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(settings.orbo.walkSpeed * 2.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateRunaway: {
            if (!target() || (t_ <= 0.0f) ||
                !scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring) {
                detourBehavior_->start();

                b2Vec2 targetDir = parent()->pos() - target()->pos();
                b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

                if (b2Cross(dir, targetDir) >= 0.0f) {
                    parent()->applyTorque(parent()->inertia() * (settings.orbo.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
                } else {
                    parent()->applyTorque(parent()->inertia() * (-settings.orbo.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
                }
            }

            if (parent()->linearVelocity().Length() < settings.orbo.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(settings.orbo.walkSpeed * 2.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateExtend: {
            interceptBehavior_->setTarget(target());
            if (ac_->animationFinished()) {
                state_ = StateAttack;
                t_ = (settings.orbo.shootTurnDuration + settings.orbo.shootLoopDelay) * settings.orbo.shootNumTurns;
            }
            break;
        }
        case StateAttack: {
            interceptBehavior_->setTarget(target());
            weapon_[0]->trigger(true);
            weapon_[1]->trigger(true);
            if ((t_ > 0.0f) && scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                break;
            }
            weapon_[0]->trigger(false);
            weapon_[1]->trigger(false);
            weapon_[0]->reload();
            weapon_[1]->reload();
            ac_->startAnimation(AnimationRetract);
            state_ = StateRetract;
            break;
        }
        case StateRetract: {
            interceptBehavior_->setTarget(target());
            if (ac_->animationFinished()) {
                parent()->setInvulnerable(true);
                parent()->findComponent<RenderHealthbarComponent>()->removeFromParent();

                interceptBehavior_->reset();
                state_ = StateIdle;
                ac_->startAnimation(AnimationDefault);
                ac_->setPaused(true);
                t_ = settings.orbo.attackTimeout;
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void OrboComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        parent()->setInvulnerable(true);
    }

    void OrboComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
