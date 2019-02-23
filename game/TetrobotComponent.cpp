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

#include "TetrobotComponent.h"
#include "ExplosionComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "LaserComponent.h"
#include "FadeOutComponent.h"
#include "AssetManager.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

namespace af
{
    TetrobotComponent::TetrobotComponent(const b2Vec2& bulletPos)
    : TargetableComponent(phaseThink),
      bulletPos_(bulletPos),
      state_(StateIdle),
      stateTime_(0.0f),
      rotateDir_((rand() % 2) == 0),
      sndCharge_(audio.createSound("tetrobot_charge.ogg")),
      sndAttack_(audio.createSound("tetrobot_attack.ogg"))
    {
    }

    TetrobotComponent::~TetrobotComponent()
    {
    }

    void TetrobotComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TetrobotComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createTetrobotGibbed();

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

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.5f);

                (*it)->addComponent(component);
            }

            scene()->addObject(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.tetrobot.explosionImpulse, settings.tetrobot.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            scene()->spawnPowerup(parent()->pos());

            parent()->removeFromParent();

            return;
        }

        if (!avoidBehavior_) {
            avoidBehavior_ = parent()->avoidBehavior();
        }

        updateAutoTarget(dt);

        stateTime_ -= dt;

        switch (state_) {
        case StateIdle:
            if (target() && (b2DistanceSquared(target()->pos(), parent()->pos()) <= settings.tetrobot.laserLength * settings.tetrobot.laserLength)) {
                state_ = StateTrack;
                stateTime_ = getRandom(settings.tetrobot.trackTime * (0.5f), settings.tetrobot.trackTime * (4.0f / 3.0f));

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(4.5f);
                avoidBehavior_->setLinearVelocity(settings.tetrobot.speed);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();
            }
            break;
        case StateTrack: {
            if (stateTime_ <= 0.0f) {
                state_ = StateCharge;
                stateTime_ = settings.tetrobot.chargeTime;
                rotateDir_ = !rotateDir_;
                sndCharge_->play();

                dottedLine_ = boost::make_shared<RenderDottedLineComponent>(bulletPos_, 0.0f,
                    0.2f, assetManager.getDrawable("common2/dot.png"), zOrderMarker);

                dottedLine_->setColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
                dottedLine_->setDotVelocity(5.0f);
                dottedLine_->setDotDistance(0.5f);

                parent()->addComponent(dottedLine_);

                avoidBehavior_->reset();

                break;
            }

            if (b2DistanceSquared(target()->pos(), parent()->pos()) > settings.tetrobot.laserLength * settings.tetrobot.laserLength) {
                state_ = StateIdle;

                avoidBehavior_->reset();

                break;
            }

            b2Vec2 wallDir = -parent()->getDirection(settings.tetrobot.wallDistance);

            checkWall(wallDir);
            wallDir = wallDir.Skew();
            checkWall(wallDir);
            wallDir = -wallDir;
            checkWall(wallDir);

            float ang = b2_pi * dt / 2.0f;

            if (rotateDir_) {
                ang = -ang;
            }

            if (parent()->linearVelocity().Length() < settings.tetrobot.speed) {
                b2Vec2 v = target()->pos() - parent()->pos();

                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                    parent()->pos(), 1.0f, 1.0f)) {
                    v = b2Mul(b2Rot(ang), v) - v;
                    v.Normalize();
                } else {
                    b2Vec2 tmp = b2Mul(b2Rot(ang), v) - v;
                    v.Normalize();
                    v = tmp + (tmp.Length() / 2.0f) * v;
                    v.Normalize();
                }

                parent()->applyForceToCenter(parent()->mass() *
                    (settings.tetrobot.speed * 1.0f + parent()->linearVelocityDamped()) * v, true);
            }

            b2Vec2 targetDir = target()->pos() - parent()->pos();

            /*
             * Try to guess what the rotation will be in 1/3 sec.
             */

            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 3.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                parent()->applyTorque(parent()->inertia() * (settings.tetrobot.trackSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            } else {
                parent()->applyTorque(parent()->inertia() * (-settings.tetrobot.trackSpeed - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            }
            break;
        }
        case StateCharge:
            if (stateTime_ <= 0.0f) {
                dottedLine_->removeFromParent();
                dottedLine_.reset();
                laser_ =
                    sceneObjectFactory.createLaser(bulletPos_, 0.0f,
                        settings.tetrobot.laserLength, settings.tetrobot.shootDamage,
                        zOrderExplosion - 1);

                parent()->addComponent(laser_);

                state_ = StateAttack;
                stateTime_ = settings.tetrobot.shootTime;
                sndAttack_->play();
            } else {
                b2Vec2 p1 = b2Mul(parent()->getTransform(), bulletPos_);
                b2Vec2 p2 = b2Mul(parent()->getTransform(), b2Vec2(settings.tetrobot.laserLength, 0.0f));
                b2Vec2 pt = p2;

                scene()->rayCast(p1, p2, boost::bind(&TetrobotComponent::targetRayCastCb, this,
                    _1, _2, _3, _4, boost::ref(pt)));

                dottedLine_->setLength((pt - p1).Length());
            }
            break;
        case StateAttack:
            if (stateTime_ <= 0.0f) {
                if (laser_) {
                    laser_->removeFromParent();
                    laser_.reset();
                }

                state_ = StateIdle;
            }
            break;
        default:
            break;
        }
    }

    void TetrobotComponent::onRegister()
    {
        fakeMissile_ = sceneObjectFactory.createFakeMissile(SceneObjectTypeEnemyMissile);
        scene()->addObject(fakeMissile_);
    }

    void TetrobotComponent::onUnregister()
    {
        fakeMissile_->removeFromParent();
        fakeMissile_.reset();

        setTarget(SceneObjectPtr());
        dottedLine_.reset();
        laser_.reset();
    }

    void TetrobotComponent::checkWall(const b2Vec2& dir)
    {
        float fraction = 1.0f;

        scene()->rayCast(parent()->pos(), parent()->pos() + dir,
            boost::bind(&TetrobotComponent::wallRayCastCb, this, _1, _2, _3, _4, boost::ref(fraction)));

        if (fraction < 1.0f) {
            float vel = parent()->linearVelocity().Length();
            if (vel < settings.tetrobot.speed) {
                b2Vec2 tmp = -dir;
                tmp.Normalize();

                parent()->applyForceToCenter(parent()->mass() *
                    (settings.tetrobot.speed * 1.0f + parent()->linearVelocityDamped()) * tmp, true);
            }
        }
    }

    float TetrobotComponent::wallRayCastCb(b2Fixture* fixture, const b2Vec2& point,
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

        return fraction;
    }

    float TetrobotComponent::targetRayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint)
    {
        if (fixture->IsSensor() || !fakeMissile_->collidesWith(fixture)) {
            return -1;
        }

        finalPoint = point;

        return fraction;
    }
}
