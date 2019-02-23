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

#include "BossBeholderComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "Renderer.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "RenderHealthbarComponent.h"
#include "CollisionCancelComponent.h"
#include "FadeOutComponent.h"
#include "AssetManager.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "QuadPulseComponent.h"
#include "af/Utils.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    BossBeholderComponent::BossBeholderComponent(const SceneObjectPtr& legs,
        const SceneObjectPtr& guns,
        const LightPtr& light,
        const LightPtr fireLight[6])
    : TargetableComponent(phaseThink),
      legs_(legs),
      guns_(guns),
      light_(light),
      state_(StateInit),
      t_(0.0f),
      shootState_(ShootStateIdle),
      shootT_(0.0f),
      gunNum_(0),
      legsAccelerate_(0),
      firstPatrol_(true),
      walkRot_(0.0f),
      checkRot_(2.0f * b2_pi / numCheckRays),
      patrolPoint_(b2Vec2_zero),
      sndShake_(audio.createSound("queen_shake.ogg")),
      sndCharge_(audio.createSound("tetrobot_charge.ogg")),
      sndLaser_(audio.createSound("beholder_laser.ogg"))
    {
        fireLight_[0] = fireLight[0];
        fireLight_[1] = fireLight[1];
        fireLight_[2] = fireLight[2];
        fireLight_[3] = fireLight[3];
        fireLight_[4] = fireLight[4];
        fireLight_[5] = fireLight[5];

        fireTime_[0] = 0.0001f;
        fireTime_[1] = 0.5f;
        fireTime_[2] = 1.0f;
        fireTime_[3] = 1.5f;
        fireTime_[4] = 2.0f;
        fireTime_[5] = 3.0f;

        sndShake_->setLoop(true);
        sndLaser_->setLoop(true);
    }

    BossBeholderComponent::~BossBeholderComponent()
    {
    }

    void BossBeholderComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossBeholderComponent::update(float dt)
    {
        legs_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (parent()->pos() - legs_->pos()));
        guns_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (parent()->pos() - guns_->pos()));

        for (int i = 0; i < 3; ++i) {
            shield_[i]->setAngle(shield_[i]->angle() + settings.bossBeholder.shieldVelocity * dt);
        }

        t_ -= dt;
        shootT_ -= dt;
        lightTweenTime_ += dt;
        megaTime_ -= dt;

        Color c = light_->color();
        c.rgba[3] = lightTweening_->getValue(lightTweenTime_);
        light_->setColor(c);

        updateAutoTarget(dt);

        if ((parent()->life() <= 0) && (state_ < StateDie1)) {
            scene()->stats()->incEnemiesKilled();

            state_ = StateDie1;
            shootState_ = ShootStateDie;

            for (int i = 0; i < 3; ++i) {
                shield_[i]->trigger(true);
            }

            for (int i = 0; i < 8; ++i) {
                gun_[i]->trigger(false);
                laser_[i]->trigger(false);
                aim_[i]->trigger(false);
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndShake_->stop();
            sndLaser_->stop();

            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            for (std::set<SceneObjectPtr>::iterator it = parent()->objects().begin();
                it != parent()->objects().end(); ++it) {
                CollisionCancelComponentPtr c = (*it)->findComponent<CollisionCancelComponent>();
                if (c) {
                    c->setDamage(0.0f);
                    c->setDamageSound(AudioSourcePtr());
                    c->setRoll(0.0f);
                    c->setImmediate(true);
                }
            }

            lightTweening_ = boost::make_shared<SingleTweening>(1.0f, EaseInQuad, light_->color().rgba[3], 0.0f);
            lightTweenTime_ = 0.0f;

            parent()->setLinearDamping(2.0f);
            parent()->setAngularDamping(2.0f);
        }

        switch (shootState_) {
        case ShootStateIdle: {
            if (!target()) {
                break;
            }

            if ((state_ != StateWalk) || (shootT_ >= 0.0f)) {
                break;
            }

            if (gunNum_ >= 5) {
                int r = rand() % 7;

                if ((r >= 0) && (r <= 1)) {
                    gunNum_ = 0;
                    shootState_ = ShootStateLaserStart;
                    shootT_ = 1.0f;

                    startWalkVerySlow();

                    break;
                } else if ((r >= 2) && (r <= 3)) {
                    gunNum_ = 0;
                    shootState_ = ShootStateNapalmPre;
                    shootT_ = 1.0f;

                    startWalkSlow(false);

                    break;
                } else if (r == 4) {
                    gunNum_ = 0;

                    startPatrol();

                    break;
                }
            }

            shootState_ = ShootStateGunExtend;
            shootT_ = 1.0f;

            for (int i = 0; i < 8; ++i) {
                gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
            }

            guns_->setAngularVelocity(deg2rad(30.0f));

            ++gunNum_;

            break;
        }
        case ShootStateGunExtend: {
            if (shootT_ >= 0.0f) {
                break;
            }

            shootState_ = ShootStateGun;
            shootT_ = 0.0f;
            shootNum_ = 0;

            break;
        }
        case ShootStateGun: {
            if (shootT_ >= 0.0f) {
                break;
            }

            if (shootNum_ == -1) {
                shootState_ = ShootStateGunRetract;
                shootT_ = 0.5f;

                for (int i = 0; i < 8; ++i) {
                    gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
                }

                break;
            }

            for (int i = 0; i < 8; ++i) {
                gun_[i]->triggerOnce();
            }

            ++shootNum_;
            shootT_ = 0.2f;

            if (shootNum_ == 5) {
                shootNum_ = -1;
                shootT_ = 0.2f;
            }

            break;
        }
        case ShootStateGunRetract: {
            if (shootT_ >= 0.0f) {
                break;
            }

            guns_->setAngularVelocity(0.0f);
            shootState_ = ShootStateIdle;
            shootT_ = getRandom(0.0f, 0.5f);

            break;
        }
        case ShootStateLaserStart: {
            if (shootT_ >= 0.0f) {
                break;
            }

            shootState_ = ShootStateLaserExtend;
            shootT_ = 1.0f;

            for (int i = 0; i < 8; ++i) {
                gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
            }

            break;
        }
        case ShootStateLaserExtend: {
            if (shootT_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                aim_[i]->trigger(true);
            }

            sndCharge_->play();

            shootState_ = ShootStateLaserAim;
            shootT_ = settings.bossBeholder.laserChargeTime;

            break;
        }
        case ShootStateLaserAim: {
            if (shootT_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                aim_[i]->trigger(false);
                laser_[i]->trigger(true);
            }

            sndLaser_->play();

            guns_->setAngularVelocity(deg2rad(19.0f));

            if ((rand() % 2) == 0) {
                guns_->setAngularVelocity(-guns_->angularVelocity());
            }

            shootT_ = getRandom(3.0f, 6.0f);

            shootState_ = ShootStateLaser;
            shootNum_ = 3 + (rand() % 3);

            break;
        }
        case ShootStateLaser: {
            if (shootT_ >= 0.0f) {
                break;
            }

            if ((rand() % 2) == 0) {
                guns_->setAngularVelocity(-guns_->angularVelocity());
            }

            shootT_ = getRandom(3.0f, 6.0f);
            --shootNum_;

            if (shootNum_ == 0) {
                for (int i = 0; i < 8; ++i) {
                    laser_[i]->trigger(false);
                }
                shootState_ = ShootStateLaserPost;
                shootT_ = 1.0f;

                sndLaser_->stop();
            }

            break;
        }
        case ShootStateLaserPost: {
            if (shootT_ >= 0.0f) {
                break;
            }

            shootState_ = ShootStateLaserRetract;
            shootT_ = 0.5f;

            for (int i = 0; i < 8; ++i) {
                gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
            }

            break;
        }
        case ShootStateLaserRetract: {
            if (shootT_ >= 0.0f) {
                break;
            }

            guns_->setAngularVelocity(0.0f);
            shootState_ = ShootStateIdle;
            shootT_ = getRandom(0.0f, 0.5f);

            startWalkSlow(true);

            break;
        }
        case ShootStateNapalmPre: {
            if (shootT_ >= 0.0f) {
                break;
            }

            startWalkFast(false);

            shootState_ = ShootStateNapalmPre2;
            shootT_ = 1.0f;

            break;
        }
        case ShootStateNapalmPre2: {
            if (shootT_ >= 0.0f) {
                break;
            }

            startWalkFast(true);

            shootState_ = ShootStateNapalm;
            shootT_ = getRandom(1.5f, 4.0f);
            shootNum_ = 4 + (rand() % 4);

            break;
        }
        case ShootStateNapalm: {
            if (shootT_ >= 0.0f) {
                break;
            }

            if (shootNum_ == 0) {
                shootState_ = ShootStateNapalmPost;
                shootT_ = 1.0f;

                startWalkFast(false);

                break;
            }

            SceneObjectPtr napalm = sceneObjectFactory.createNapalm2(settings.bossBeholder.napalmDamage, true);

            napalm->setTransform(b2Transform(parent()->pos(), b2Rot(vec2angle(target()->pos() - parent()->pos()))));
            napalm->setLinearVelocity(napalm->getDirection(settings.bossBeholder.napalmVelocity));

            scene()->addObject(napalm);

            shootT_ = getRandom(1.5f, 4.0f);
            --shootNum_;

            break;
        }
        case ShootStateNapalmPost: {
            if (shootT_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                legJoint_[i]->setMotorSpeed(-legJoint_[i]->motorSpeed());
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndShake_->stop();

            shootState_ = ShootStateIdle;
            shootT_ = getRandom(1.0f, 1.5f);

            startWalkSlow(true);

            break;
        }
        case ShootStateMegaPre1: {
            if (shootT_ >= 0.0f) {
                break;
            }

            startWalkFast(false, false);

            shootState_ = ShootStateMegaPre2;
            shootT_ = 1.0f;

            break;
        }
        case ShootStateMegaPre2: {
            if (shootT_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
            }

            guns_->setAngularVelocity(deg2rad(120.0f));

            shootState_ = ShootStateMegaPre3;
            shootT_ = 1.0f;

            break;
        }
        case ShootStateMegaPre3: {
            if (shootT_ >= 0.0f) {
                break;
            }

            shootState_ = ShootStateMega;
            shootT_ = 0.0f;
            shootNum_ = 5 + (rand() % 6);
            megaTime_ = getRandom(8.0f, 12.0f);

            break;
        }
        case ShootStateMega: {
            if (megaTime_ <= 0.0f) {
                shootState_ = ShootStateMegaPost1;
                shootT_ = 1.0f;
                break;
            }

            if (shootT_ >= 0.0f) {
                break;
            }

            if (shootNum_ == 0) {
                shootNum_ = 5 + (rand() % 6);
                if ((rand() % 2) == 0) {
                    guns_->setAngularVelocity(-guns_->angularVelocity());
                }
            }

            for (int i = 0; i < 8; ++i) {
                gun_[i]->triggerOnce();
            }

            shootT_ = 0.25f;
            --shootNum_;

            break;
        }
        case ShootStateMegaPost1: {
            if (shootT_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                gunJoint_[i]->setMotorSpeed(-gunJoint_[i]->motorSpeed());
            }

            shootState_ = ShootStateMegaPost2;
            shootT_ = 0.5f;

            break;
        }
        case ShootStateMegaPost2: {
            if (shootT_ >= 0.0f) {
                break;
            }

            guns_->setAngularVelocity(0.0f);

            for (int i = 0; i < 8; ++i) {
                legJoint_[i]->setMotorSpeed(-legJoint_[i]->motorSpeed());
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndShake_->stop();

            shootState_ = ShootStateMegaPost3;
            shootT_ = 1.0f;

            startWalkSlow(false);

            break;
        }
        case ShootStateMegaPost3: {
            if (shootT_ >= 0.0f) {
                break;
            }

            shootState_ = ShootStateIdle;
            shootT_ = getRandom(0.5f, 1.0f);

            startWalkSlow(true);

            break;
        }
        case ShootStateDie: {
            break;
        }
        default:
            assert(false);
            break;
        }

        if (state_ < StateDie1) {
            if (legsAccelerate_ > 0) {
                float accel = (settings.bossBeholder.fastLegVelocity - settings.bossBeholder.slowLegVelocity) * dt * 1.5f;

                if (-legs_->angularVelocity() < (settings.bossBeholder.fastLegVelocity - accel)) {
                    legs_->setAngularVelocity(legs_->angularVelocity() - accel);
                } else {
                    legs_->setAngularVelocity(-settings.bossBeholder.fastLegVelocity);
                }
            } else if (legsAccelerate_ == 0) {
                if (-legs_->angularVelocity() > settings.bossBeholder.slowLegVelocity) {
                    float accel = (settings.bossBeholder.fastLegVelocity - settings.bossBeholder.slowLegVelocity) * dt * 1.5f;
                    if (-legs_->angularVelocity() > (settings.bossBeholder.slowLegVelocity + accel)) {
                        legs_->setAngularVelocity(legs_->angularVelocity() + accel);
                    } else {
                        legs_->setAngularVelocity(-settings.bossBeholder.slowLegVelocity);
                    }
                } else if (-legs_->angularVelocity() < settings.bossBeholder.slowLegVelocity) {
                    float accel = (settings.bossBeholder.slowLegVelocity - settings.bossBeholder.verySlowLegVelocity) * dt * 1.5f;
                    if (-legs_->angularVelocity() < (settings.bossBeholder.slowLegVelocity - accel)) {
                        legs_->setAngularVelocity(legs_->angularVelocity() - accel);
                    } else {
                        legs_->setAngularVelocity(-settings.bossBeholder.slowLegVelocity);
                    }
                }
            } else {
                float accel = (settings.bossBeholder.slowLegVelocity - settings.bossBeholder.verySlowLegVelocity) * dt * 1.5f;

                if (-legs_->angularVelocity() > (settings.bossBeholder.verySlowLegVelocity + accel)) {
                    legs_->setAngularVelocity(legs_->angularVelocity() + accel);
                } else {
                    legs_->setAngularVelocity(-settings.bossBeholder.verySlowLegVelocity);
                }
            }
        } else {
            float accel = deg2rad(180.0f) * dt;

            if (-legs_->angularVelocity() > accel) {
                legs_->setAngularVelocity(legs_->angularVelocity() + accel);
            } else {
                legs_->setAngularVelocity(0.0f);
            }

            if (guns_->angularVelocity() > 0.0f) {
                if (guns_->angularVelocity() > accel) {
                    guns_->setAngularVelocity(guns_->angularVelocity() - accel);
                } else {
                    guns_->setAngularVelocity(0.0f);
                }
            } else if (guns_->angularVelocity() < 0.0f) {
                if (-guns_->angularVelocity() > accel) {
                    guns_->setAngularVelocity(guns_->angularVelocity() + accel);
                } else {
                    guns_->setAngularVelocity(0.0f);
                }
            }
        }

        switch (state_) {
        case StateInit: {
            if (!target()) {
                break;
            }

            if (patrol()) {
                patrolPoint_ = target()->pos();
                startPatrol();
                break;
            }

            break;
        }
        case StatePatrol: {
            float vel = 20.0f;

            if (b2DistanceSquared(parent()->pos(), patrolPoint_) <= 5.0f * 5.0f) {
                vel = 3.5f;
                legsAccelerate_ = 0;
            } else {
                legsAccelerate_ = 1;
            }

            if (b2DistanceSquared(parent()->pos(), patrolPoint_) <= 1.0f * 1.0f) {
                if (firstPatrol_) {
                    firstPatrol_ = false;
                    state_ = StateAngry1;
                    t_ = 1.0f;
                } else {
                    t_ = 0.0f;
                    startWalkSlow(false);

                    shootState_ = ShootStateMegaPre1;
                    shootT_ = 1.0f;
                }
                break;
            }

            if (parent()->linearVelocity().Length() < vel) {
                b2Vec2 tmp = patrolPoint_ - parent()->pos();
                tmp.Normalize();
                parent()->applyForceToCenter(parent()->mass() * (vel * 2.0f + parent()->linearVelocityDamped()) * tmp, true);
            }

            break;
        }
        case StateAngry1: {
            if (t_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 3; ++i) {
                shield_[i]->trigger(true);
            }

            state_ = StateAngry2;
            t_ = 1.0f;

            break;
        }
        case StateAngry2: {
            if (t_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 3; ++i) {
                shield_[i]->trigger(false);
            }

            for (int i = 0; i < 8; ++i) {
                legJoint_[i]->setMotorSpeed(-legJoint_[i]->motorSpeed());
            }

            legsAccelerate_ = 1;

            scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);
            sndShake_->play();

            state_ = StateAngry3;
            t_ = 2.0f;

            break;
        }
        case StateAngry3: {
            if (t_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 8; ++i) {
                legJoint_[i]->setMotorSpeed(-legJoint_[i]->motorSpeed());
            }

            legsAccelerate_ = 0;

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndShake_->stop();

            state_ = StateAngry4;
            t_ = 2.0f;

            break;
        }
        case StateAngry4: {
            if (t_ >= 0.0f) {
                break;
            }

            t_ = 0.0f;
            startWalkSlow(true);

            shootT_ = 1.5f;

            parent()->setInvulnerable(false);

            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 6.5f), 0, 12.0f, 1.8f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            parent()->addComponent(component);

            break;
        }
        case StateWalk: {
            b2Vec2 dir(walkCheckDistance_, 0.0f);

            b2Vec2 p = parent()->pos();

            int totalHit = 0;
            b2Vec2 totalMiss = b2Vec2_zero;

            for (int i = 0; i < numCheckRays; ++i) {
                b2Vec2 pt = p + dir;

                bool hit = false;

                parent()->scene()->rayCast(p, pt,
                    boost::bind(&BossBeholderComponent::rayCastCb, this,
                    _1, _2, _3, _4, boost::ref(hit)));

                if (hit) {
                    ++totalHit;
                } else {
                    totalMiss += dir;
                }

                dir = b2Mul(checkRot_, dir);
            }

            if (totalHit != 0) {
                t_ = walkDuration_ * 0.5f;
                if (b2Cross(walkDir_, totalMiss) >= 0.0f) {
                    walkRot_.Set(walkAngularVelocity_ * dt);
                } else {
                    walkRot_.Set(-walkAngularVelocity_ * dt);
                }
            }

            if (t_ < 0.0f) {
                t_ = walkDuration_;

                int r = rand() % 3;

                if (r == 0) {
                    walkRot_.Set(walkAngularVelocity_ * dt);
                } else if (r == 1) {
                    walkRot_.Set(-walkAngularVelocity_ * dt);
                } else {
                    walkRot_.Set(0.0f);
                }
            }

            walkDir_ = b2Mul(walkRot_, walkDir_);

            if (parent()->linearVelocity().Length() < walkLinearVelocity_) {
                parent()->applyForceToCenter(parent()->mass() * (walkLinearVelocity_ * 2.0f + parent()->linearVelocityDamped()) * walkDir_, true);
            }

            break;
        }
        case StateDie1: {
            bool done = true;

            for (int i = 0; i < 6; ++i) {
                if (fireTime_[i] > 0.0f) {
                    done = false;

                    fireTime_[i] -= dt;

                    if (fireTime_[i] <= 0.0f) {
                        fireTime_[i] = 0.0f;

                        SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);
                        explosion->setPos(parent()->getWorldPoint(fireLight_[i]->pos()));
                        scene()->addObject(explosion);

                        fireLight_[i]->setVisible(true);

                        ParticleEffectComponentPtr pec = assetManager.getParticleEffect("fire1.p",
                            b2Vec2_zero, 0.0f, false);

                        pec->setTransform(b2Transform(fireLight_[i]->pos(), b2Rot(0.0f)));

                        pec->setFixedAngle(true);

                        pec->setZOrder(zOrderEffects);

                        pec->resetEmit();

                        parent()->addComponent(pec);
                    }
                }
            }

            if (done) {
                state_ = StateDie2;

                QuadPulseComponentPtr qpc = parent()->findComponent<QuadPulseComponent>();
                if (qpc) {
                    qpc->removeFromParent();
                }
            }

            break;
        }
        case StateDie2: {
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void BossBeholderComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseOutQuad, light_->color().rgba[3], 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.5f, EaseInQuad, 1.0f, light_->color().rgba[3]));

        lightTweening_ = tweening;
        lightTweenTime_ = 0.0f;

        std::vector<PrismaticJointProxyPtr> tmp =
            parent()->findComponent<PhysicsJointComponent>()->joints<PrismaticJointProxy>("leg_joint");

        assert(tmp.size() == 8);

        for (int i = 0; i < 8; ++i) {
            legJoint_[i] = tmp[i];
        }

        tmp = parent()->findComponent<PhysicsJointComponent>()->joints<PrismaticJointProxy>("gun_joint");

        assert(tmp.size() == 8);

        for (int i = 0; i < 8; ++i) {
            gunJoint_[i] = tmp[i];
        }

        walkDir_ = parent()->getDirection(1.0f);

        parent()->setInvulnerable(true);
    }

    void BossBeholderComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        legs_.reset();
        guns_.reset();
    }

    void BossBeholderComponent::startPatrol()
    {
        state_ = StatePatrol;
    }

    void BossBeholderComponent::startWalkVerySlow()
    {
        state_ = StateWalk;
        walkDuration_ = 0.5f;
        walkLinearVelocity_ = 5.0f;
        walkAngularVelocity_ = deg2rad(90.0f);
        walkCheckDistance_ = 15.0f;
        legsAccelerate_ = -1;
    }

    void BossBeholderComponent::startWalkSlow(bool move)
    {
        state_ = StateWalk;
        walkDuration_ = 0.5f;
        walkLinearVelocity_ = move ? 10.0f : 0.0f;
        walkAngularVelocity_ = deg2rad(90.0f);
        walkCheckDistance_ = 15.0f;
        legsAccelerate_ = 0;
    }

    void BossBeholderComponent::startWalkFast(bool move, bool sound)
    {
        state_ = StateWalk;
        walkDuration_ = 0.5f;
        walkLinearVelocity_ = move ? 15.0f : 0.0f;
        walkAngularVelocity_ = deg2rad(120.0f);
        walkCheckDistance_ = 15.0f;

        if (legsAccelerate_ != 1) {
            for (int i = 0; i < 8; ++i) {
                legJoint_[i]->setMotorSpeed(-legJoint_[i]->motorSpeed());
            }

            scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);
            if (sound) {
                sndShake_->play();
            }
        }

        legsAccelerate_ = 1;
    }

    float BossBeholderComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, bool& hit)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        if ((SceneObject::fromFixture(fixture)->type() != SceneObjectTypeRock) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeTerrain) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeBlocker)) {
            return -1;
        }

        PhysicsBodyComponentPtr pc = SceneObject::fromFixture(fixture)->findComponent<PhysicsBodyComponent>();
        if (pc && ((pc->filterCategoryBits() & collisionBitRock) != 0)) {
            /*
             * Hack: don't raycast against rocks.
             */
            return -1;
        }

        hit = true;

        return 0.0f;
    }
}
