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

#include "BossChopperComponent.h"
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
#include "FadeOutComponent.h"
#include "AssetManager.h"
#include "af/Utils.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    static const int32 circleSegments = 30;
    static const float32 circleIncrement = 2.0f * b2_pi / circleSegments;

    BossChopperComponent::BossChopperComponent(const Points& explosionPos)
    : TargetableComponent(phaseThink),
      explosionPos_(explosionPos),
      state_(StateInit),
      t_(0.0f),
      dir_(false),
      close_(false),
      canDie_(false),
      shootState_(ShootStateIdle),
      shootT_(0.0f),
      shootStop_(false),
      charged_(false),
      nextCharge_(false),
      chargeProb_(1000),
      sndChopper_(audio.createSound("chopper.ogg")),
      sndNapalm_(audio.createStream("napalm.ogg")),
      explosionIndex_(0)
    {
        sndChopper_->setLoop(true);
    }

    BossChopperComponent::~BossChopperComponent()
    {
    }

    void BossChopperComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossChopperComponent::update(float dt)
    {
        t_ -= dt;
        shootT_ -= dt;

        updateAutoTarget(dt);

        if (target() && (state_ != StateCharge) && (state_ < StateDie3)) {
            float angularVelocity = 0.0f;

            b2Vec2 parentDir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            float angle = angleBetween(parentDir, target()->pos() - parent()->pos());

            if (state_ == StateChargePost) {
                angularVelocity = settings.bossChopper.fastTurnSpeed;
            } else {
                angularVelocity = settings.bossChopper.slowTurnSpeed;
            }

            if (angle < 0.0f) {
                angularVelocity = -angularVelocity;
            }

            if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angularVelocity = angle / settings.physics.fixedTimestep;
            }

            parent()->applyTorque(parent()->inertia() * (angularVelocity - parent()->angularVelocity()), true);
        }

        switch (shootState_) {
        case ShootStateIdle: {
            break;
        }
        case ShootStateDelay: {
            if (shootT_ > 0.0f) {
                break;
            }

            if (shootStop_) {
                shootState_ = ShootStateIdle;
                break;
            }

            bool inSight = scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 15.0f, 15.0f);

            if ((rand() % 4) == 0) {
                shootT_ = getRandom(settings.bossChopper.shootDelay * 2.0f, settings.bossChopper.shootDelay * 4.0f);
                weaponMissile_->setTarget(target());
                if (inSight) {
                    weaponMissile_->triggerOnce();
                }
            } else {
                shootState_ = ShootStateGun;
                shootT_ = settings.bossChopper.gunDuration;
                if (inSight) {
                    for (Weapons::const_iterator it = guns_.begin();
                        it != guns_.end(); ++it) {
                        (*it)->trigger(true);
                    }
                }
            }

            break;
        }
        case ShootStateGun: {
            if (shootT_ > 0.0f) {
                break;
            }

            for (Weapons::const_iterator it = guns_.begin();
                it != guns_.end(); ++it) {
                (*it)->trigger(false);
            }

            if (shootStop_) {
                shootState_ = ShootStateIdle;
            } else {
                shootState_ = ShootStateDelay;
                shootT_ = getRandom(settings.bossChopper.shootDelay, settings.bossChopper.shootDelay * 2.0f);
            }

            break;
        }
        default:
            assert(false);
            break;
        }

        switch (state_) {
        case StateInit: {
            if (!target()) {
                sndChopper_->stop();
                stopShoot();
                break;
            }

            if (sndChopper_->status() != AudioSource::Playing) {
                sndChopper_->play();
            }

            if (canDie_ && (parent()->life() <= 0)) {
                state_ = StateDie1;
                stopShoot();
                break;
            }

            if (patrol()) {
                state_ = StatePatrol;
                stopShoot();
                break;
            }

            if (!nextCharge_) {
                nextCharge_ = (rand() % chargeProb_) == 0;
                if (!nextCharge_) {
                    chargeProb_ = 3;
                }
            }

            if (nextCharge_ &&
                (shootState_ <= ShootStateDelay) &&
                (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) < deg2rad(15.0f))) {
                stopShoot();
                chargeDir_ = target()->pos() - parent()->pos();
                chargeDir_.Normalize();
                chargePos_ = target()->pos();
                state_ = StateCharge;
                charged_ = false;
                t_ = 1.2f;
                nextCharge_ = false;
                chargeProb_ = 2;

                pec_ = assetManager.getParticleEffect("napalm.p", b2Vec2_zero, 0.0f);
                pec_->setZOrder(zOrderEffects);
                pec_->resetEmit();
                parent()->addComponent(pec_);

                sndNapalm_->play();

                break;
            }

            startShoot();
            state_ = StateStrafe;
            dir_ = (rand() % 2) == 0;
            t_ = getRandom(1.0f, 2.0f);

            break;
        }
        case StateStrafe: {
            if (t_ <= 0.0f) {
                state_ = StateInit;
                break;
            }

            if (parent()->linearVelocity().Length() < settings.bossChopper.slowWalkSpeed) {
                b2Vec2 v = target()->pos() - parent()->pos();
                v = b2Mul(b2Rot(dir_ ? (b2_pi * dt / 2.0f) : (-b2_pi * dt / 2.0f)), v) - v;
                v.Normalize();

                if (close_) {
                    if (b2DistanceSquared(parent()->pos(), target()->pos()) > 20.0f * 20.0f) {
                        close_ = false;
                    }
                } else {
                    if (b2DistanceSquared(parent()->pos(), target()->pos()) > 15.0f * 15.0f) {
                        v += parent()->getDirection(1.5f);
                        v.Normalize();
                    } else {
                        close_ = true;
                    }
                }

                parent()->applyForceToCenter(parent()->mass() * (settings.bossChopper.slowWalkSpeed * 2.0f) * v, true);
            }

            break;
        }
        case StateCharge: {
            if (!charged_) {
                if (b2DistanceSquared(parent()->pos(), chargePos_) < 15.0f * 15.0f) {
                    t_ = 0.5f;
                    charged_ = true;
                    break;
                }
            }

            chargeDamage(dt);

            if (t_ <= 0.0f) {
                if (oldPec_) {
                    oldPec_->removeFromParent();
                    oldPec_.reset();
                }
                state_ = StateChargePost;
                t_ = 1.5f;
                break;
            }
            if (parent()->linearVelocity().Length() < settings.bossChopper.fastWalkSpeed) {
                parent()->applyForceToCenter(parent()->mass() * settings.bossChopper.fastWalkSpeed * 2.0f * chargeDir_, true);
            }
            break;
        }
        case StateChargePost: {
            if ((t_ <= 0.5f) && !oldPec_) {
                oldPec_ = pec_;
                oldPec_->allowCompletion();
            }
            chargeDamage(dt);
            if (t_ <= 0.0f) {
                state_ = StateInit;
                pec_.reset();
                sndNapalm_->stop();
                break;
            }
            break;
        }
        case StatePatrol: {
            if (!patrol()) {
                state_ = StateInit;
                parent()->setLinearDamping(origLinearDamping_);
                parent()->setAngularDamping(origAngularDamping_);
                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= 10.0f * 10.0f) {
                parent()->setLinearDamping(4.0f);
                parent()->setAngularDamping(4.0f);
                break;
            } else {
                parent()->setLinearDamping(origLinearDamping_);
                parent()->setAngularDamping(origAngularDamping_);
            }

            if (parent()->linearVelocity().Length() < settings.bossChopper.slowWalkSpeed) {
                parent()->applyForceToCenter(parent()->mass() * parent()->getDirection(settings.bossChopper.slowWalkSpeed * 2.0f), true);
            }

            break;
        }
        case StateDie1: {
            if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 1.0f, 1.0f)) {
                state_ = StateDie3;
                t_ = 0.0f;
                RenderHealthbarComponentPtr hc =
                    parent()->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }
                break;
            }
            state_ = StateDie2;
            dir_ = (rand() % 2) == 0;
            t_ = getRandom(1.0f, 2.0f);
            break;
        }
        case StateDie2: {
            if (t_ <= 0.0f) {
                state_ = StateDie1;
                break;
            }

            if (parent()->linearVelocity().Length() < settings.bossChopper.slowWalkSpeed) {
                b2Vec2 v = target()->pos() - parent()->pos();
                v = b2Mul(b2Rot(dir_ ? (b2_pi * dt / 2.0f) : (-b2_pi * dt / 2.0f)), v) - v;
                v.Normalize();

                if (close_) {
                    if (b2DistanceSquared(parent()->pos(), target()->pos()) > 20.0f * 20.0f) {
                        close_ = false;
                    }
                } else {
                    if (b2DistanceSquared(parent()->pos(), target()->pos()) > 15.0f * 15.0f) {
                        v += parent()->getDirection(1.5f);
                        v.Normalize();
                    } else {
                        close_ = true;
                    }
                }

                parent()->applyForceToCenter(parent()->mass() * (settings.bossChopper.slowWalkSpeed * 2.0f) * v, true);
            }
            break;
        }
        case StateDie3: {
            if (t_ >= 0.0f) {
                break;
            }

            if (explosionIndex_ >= static_cast<int>(explosionPos_.size())) {
                break;
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(115);
            explosion->setPos(parent()->getWorldPoint(explosionPos_[explosionIndex_]));
            scene()->addObject(explosion);

            ++explosionIndex_;
            t_ = 0.4f;

            if (explosionIndex_ == 5) {
                scene()->stats()->incEnemiesKilled();

                PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

                for (PhysicsJointComponent::Joints::const_iterator it = jc->joints().begin();
                     it != jc->joints().end(); ++it) {
                    (*it)->remove();
                }

                b2Vec2 velDir = parent()->getDirection(15.0f);
                b2Rot rot(2.0f * b2_pi / (parent()->objects().size() + 1));

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 0.8f);
                parent()->addComponent(component);

                for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                     it != parent()->objects().end(); ++it) {
                    velDir = b2Mul(rot, velDir);

                    (*it)->setAngularDamping(2.0f);
                    (*it)->setLinearVelocity(parent()->linearVelocity() + velDir);

                    (*it)->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f, 0.8f);
                    (*it)->addComponent(component);
                }
                break;
            }

            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void BossChopperComponent::debugDraw()
    {
        if (!pec_) {
            return;
        }

        const ParticleEmmiter::Particles& particles = pec_->emmiters()[1]->particles();

        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].active || (particles[i].currentLife <= 400)) {
                continue;
            }

            b2Vec2 center = b2Mul(particles[i].imageStartXf, particles[i].imagePos);
            float radius = 5.5f;

            float32 theta = 0.0f;
            b2Vec2 p0;

            renderer.setProgramColorLines(1.0f);

            RenderLineStrip rop = renderer.renderLineStrip();

            for (int32 i = 0; i < circleSegments; ++i) {
                b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

                rop.addVertex(v.x, v.y);

                if (i == 0) {
                    p0 = v;
                }

                theta += circleIncrement;
            }

            rop.addVertex(p0.x, p0.y);

            rop.addColors(Color(1.0f, 1.0f, 1.0f));
        }
    }

    void BossChopperComponent::onRegister()
    {
        origLinearDamping_ = parent()->linearDamping();
        origAngularDamping_ = parent()->angularDamping();

        scene()->stats()->incEnemiesSpawned();
    }

    void BossChopperComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        sndChopper_->stop();
    }

    void BossChopperComponent::startShoot()
    {
        if (shootState_ == ShootStateIdle) {
            shootState_ = ShootStateDelay;
            shootT_ = 0.0f;
        }
        shootStop_ = false;
    }

    void BossChopperComponent::stopShoot()
    {
        shootStop_ = true;
    }

    void BossChopperComponent::chargeDamage(float dt)
    {
        const ParticleEmmiter::Particles& particles = pec_->emmiters()[1]->particles();

        bool hit = false;

        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].active || (particles[i].currentLife <= 400)) {
                continue;
            }

            if (b2DistanceSquared(b2Mul(particles[i].imageStartXf, particles[i].imagePos), target()->pos()) < 5.5f * 5.5f) {
                hit = true;
                break;
            }
        }

        if (hit) {
            target()->changeLife(-settings.bossChopper.napalmDamage * dt);
        }
    }
}
