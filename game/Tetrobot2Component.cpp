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

#include "Tetrobot2Component.h"
#include "ExplosionComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "FadeOutComponent.h"
#include "AssetManager.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

namespace af
{
    Tetrobot2Component::Tetrobot2Component()
    : TargetableComponent(phaseThink),
      state_(StateIdle),
      t_(0.0f),
      rotateT_(0.0f),
      rotateDir_((rand() % 2) == 0),
      sndCharge_(audio.createSound("tetrobot_charge.ogg"))
    {
    }

    Tetrobot2Component::~Tetrobot2Component()
    {
    }

    void Tetrobot2Component::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void Tetrobot2Component::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createTetrobot2Gibbed();

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(15.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.5f);

            gibbed->addComponent(component);

            gibbed->setLinearVelocity(parent()->linearVelocity());
            gibbed->setAngularVelocity((b2_pi * 8.0f));

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);
                (*it)->setAngularVelocity(getRandom(-b2_pi * 4.0f, b2_pi * 4.0f));

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.5f);

                (*it)->addComponent(component);
            }

            scene()->addObject(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.tetrobot2.explosionImpulse, settings.tetrobot2.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            scene()->spawnPowerup(parent()->pos());

            scene()->stats()->incEnemiesKilled();

            parent()->removeFromParent();

            return;
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
                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(4.5f);
                avoidBehavior_->setLinearVelocity(settings.tetrobot2.walkSpeed);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(5.0f);
                detourBehavior_->setAngularVelocity(settings.tetrobot2.turnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateWalk;
                t_ = getRandom(settings.tetrobot2.shootMinTimeout, settings.tetrobot2.shootMaxTimeout);
                rotateT_ = 0.0f;
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
                detourBehavior_->start();
            }

            if (detouring) {
                rotateT_ = 0.0f;
            } else {
                b2Vec2 targetDir = target()->pos() - parent()->pos();
                b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 3.0f, 1.0f);

                if (b2Cross(dir, targetDir) >= 0.0f) {
                    parent()->applyTorque(parent()->inertia() * (settings.tetrobot2.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
                } else {
                    parent()->applyTorque(parent()->inertia() * (-settings.tetrobot2.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
                }
            }

            if (t_ <= 0.0f) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f)) {
                    avoidBehavior_->reset();
                    detourBehavior_->reset();

                    state_ = StatePreAim;
                    t_ = 0.8f;

                    break;
                } else {
                    t_ = getRandom(0.5f, 1.0f);
                }
            }

            b2Vec2 v = b2Vec2_zero;

            bool inSight = scene()->camera()->findComponent<CameraComponent>()->entireRectVisible(
                parent()->pos(), 3.0f, 3.0f);

            if (!detouring) {
                rotateT_ -= dt;

                if (rotateT_ <= 0.0f) {
                    rotateDir_ = (rand() % 2) == 0;
                    rotateT_ = settings.tetrobot2.shootMaxTimeout;
                }

                float fraction = 1.0f;

                b2Vec2 wallDir = parent()->getDirection(5.0f);
                wallDir = wallDir.Skew();
                if (rotateDir_) {
                    wallDir = -wallDir;
                }

                scene()->rayCast(parent()->pos(), parent()->pos() + wallDir,
                    boost::bind(&Tetrobot2Component::rayCastCb, this, _1, _2, _3, _4, boost::ref(fraction)));

                if (fraction < 1.0f) {
                    rotateDir_ = !rotateDir_;
                    rotateT_ = settings.tetrobot2.shootMaxTimeout;
                }

                float ang = b2_pi * dt / 2.0f;

                if (rotateDir_) {
                    ang = -ang;
                }

                v = target()->pos() - parent()->pos();
                v = b2Mul(b2Rot(ang), v) - v;
                v.Normalize();
            }

            if (parent()->linearVelocity().Length() < settings.tetrobot2.walkSpeed) {
                if (detouring || !inSight) {
                    v += parent()->getDirection(1.0f);
                    v.Normalize();
                }

                parent()->applyForceToCenter(parent()->mass() *
                    (settings.tetrobot2.walkSpeed * 1.0f + parent()->linearVelocityDamped()) * v, true);
            }

            break;
        }
        case StatePreAim: {
            if (t_ <= 0.0f) {
                sndCharge_->play();

                aimWeapon_->trigger(true);
                t_ = settings.tetrobot2.aimDuration;
                state_ = StateAim;
                break;
            }

            b2Vec2 targetDir = target()->pos() - parent()->pos();
            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 3.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                parent()->applyTorque(parent()->inertia() * (settings.tetrobot2.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            } else {
                parent()->applyTorque(parent()->inertia() * (-settings.tetrobot2.turnSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            }

            break;
        }
        case StateAim: {
            if (t_ <= 0.0f) {
                aimWeapon_->trigger(false);
                weapon_->triggerOnce();
                state_ = StateAttack;
                t_ = settings.tetrobot2.shootDuration;
                break;
            }
            break;
        }
        case StateAttack: {
            if (t_ <= 0.0f) {
                state_ = StateIdle;
                break;
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void Tetrobot2Component::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void Tetrobot2Component::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    float Tetrobot2Component::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, float& finalFraction)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        if ((SceneObject::fromFixture(fixture)->type() != SceneObjectTypeTerrain) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeRock) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeEnemyBuilding) &&
            ((SceneObject::fromFixture(fixture)->type() != SceneObjectTypeBlocker) || !parent()->collidesWith(fixture))) {
            return -1;
        }

        finalFraction = fraction;

        return 0.0f;
    }
}
