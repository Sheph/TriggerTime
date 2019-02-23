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

#include "SentryComponent.h"
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
    SentryComponent::SentryComponent(const SceneObjectPtr& torsoObj,
        const RenderQuadComponentPtr& torsoRc,
        const RenderQuadComponentPtr& legsRc,
        const AnimationComponentPtr& torsoAc,
        const AnimationComponentPtr& legsAc,
        bool folded)
    : TargetableComponent(phaseThink),
      torsoObj_(torsoObj),
      torsoRc_(torsoRc),
      legsRc_(legsRc),
      torsoAc_(torsoAc),
      legsAc_(legsAc),
      state_(folded ? StateFolded : StateStand),
      unfold_(false),
      shootTime_(0.0f),
      readyToShoot_(false),
      sndUnfold_(audio.createSound("sentry_unfold.ogg")),
      sndTargetLost_(audio.createSound("sentry_target_lost.ogg")),
      shootDelay_(0.0f),
      targetDistance_(4.0f)
    {
        sndTarget_[0] = audio.createSound("sentry_target1.ogg");
        sndTarget_[1] = audio.createSound("sentry_target2.ogg");

        sndPatrolStart_[0] = audio.createSound("sentry_patrol_start1.ogg");
        sndPatrolStart_[1] = audio.createSound("sentry_patrol_start2.ogg");
        sndPatrolStart_[2] = audio.createSound("sentry_patrol_start3.ogg");
        sndPatrolEnd_ = audio.createSound("sentry_patrol_end.ogg");
    }

    SentryComponent::~SentryComponent()
    {
    }

    void SentryComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SentryComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createSentryGibbed((parent()->type() == SceneObjectTypeAlly) ? 2 : 1);

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(15.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            if (gibbed->type() != SceneObjectTypeGarbage) {
                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.5f);

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
                        boost::make_shared<FadeOutComponent>(0.5f, 1.5f);

                    (*it)->addComponent(component);
                }
            }

            scene()->addObjectUnparent(gibbed);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.sentry.explosionImpulse, settings.sentry.explosionDamage,
                SceneObjectTypes(SceneObjectTypeEnemy) | SceneObjectTypeEnemyBuilding);

            scene()->addObject(explosion);

            if (parent()->type() != SceneObjectTypeAlly) {
                scene()->spawnPowerup(parent()->pos());
                scene()->stats()->incEnemiesKilled();
            }

            parent()->removeFromParent();

            return;
        }

        if (state_ != StateFolded) {
            if (parent()->linearVelocity().LengthSquared() >= 2.0f) {
                if (legsAc_->currentAnimation() != AnimationWalk) {
                    legsAc_->startAnimation(AnimationWalk);
                }
            } else {
                if (legsAc_->currentAnimation() != AnimationDefault) {
                    legsAc_->startAnimation(AnimationDefault);
                }
            }
        }

        if ((state_ != StateFolded) && target()) {
            b2Vec2 targetDir = target()->pos() - torsoObj_->pos();

            b2Vec2 dir = angle2vec(torsoObj_->angle() + torsoObj_->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                torsoObj_->applyTorque(torsoObj_->inertia() * (settings.sentry.torsoTurnSpeed - torsoObj_->angularVelocity() + torsoObj_->angularVelocityDamped()), true);
            } else {
                torsoObj_->applyTorque(torsoObj_->inertia() * (-settings.sentry.torsoTurnSpeed - torsoObj_->angularVelocity() + torsoObj_->angularVelocityDamped()), true);
            }
        } else {
            b2Vec2 dir = angle2vec(torsoObj_->angle() + torsoObj_->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, parent()->getDirection(1.0f)) >= 0.0f) {
                torsoObj_->applyTorque(torsoObj_->inertia() * (settings.sentry.torsoTurnSpeed - torsoObj_->angularVelocity() + torsoObj_->angularVelocityDamped()), true);
            } else {
                torsoObj_->applyTorque(torsoObj_->inertia() * (-settings.sentry.torsoTurnSpeed - torsoObj_->angularVelocity() + torsoObj_->angularVelocityDamped()), true);
            }
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

        bool readyToShoot = false;

        weapon_->trigger(false);

        if (!patrol() && target() &&
            (state_ != StateFolded) &&
            (b2DistanceSquared(parent()->pos(), target()->pos()) <= settings.sentry.shootRadius * settings.sentry.shootRadius) &&
            (scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 3.0f, 3.0f))) {
            float angle = fabs(angleBetween(target()->pos() - torsoObj_->pos(), torsoObj_->getDirection(1.0f)));
            if ((readyToShoot_ && (angle <= settings.sentry.shootStopAngle)) ||
                (!readyToShoot_ && (angle <= settings.sentry.shootStartAngle))) {
                readyToShoot = true;
            }
        }

        switch (state_) {
        case StateFolded: {
            if (unfold_) {
                if (torsoAc_->paused()) {
                    torsoAc_->setPaused(false);
                    legsAc_->setPaused(false);
                    sndUnfold_->play();
                } else if (torsoAc_->animationFinished()) {
                    torsoAc_->startAnimation(AnimationDefault);
                    legsAc_->startAnimation(AnimationDefault);
                    state_ = StateStand;
                    parent()->setInvulnerable(false);
                    parent()->body()->SetType(b2_dynamicBody);

                    RenderHealthbarComponentPtr component =
                        boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.0f), 0, 3.0f, 0.5f, zOrderMarker + 1);

                    component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                    parent()->addComponent(component);
                }
            }
            break;
        }
        case StateStand: {
            if (target() && (b2DistanceSquared(parent()->pos(), target()->pos()) > walkRadiusSq())) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(settings.sentry.legsTurnSpeed);
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypes(SceneObjectTypeEnemy) | SceneObjectTypeEnemyBuilding);
                avoidBehavior_->setRadius(3.5f);
                avoidBehavior_->setLinearVelocity(settings.sentry.walkSpeed);
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(8.0f);
                detourBehavior_->setAngularVelocity(settings.sentry.legsTurnSpeed);
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateWalk;

                if (patrol() && !sndPlaying()) {
                    sndPatrolStart_[rand() % 3]->play();
                }
            }
            break;
        }
        case StateWalk: {
            if (!target() || (b2DistanceSquared(parent()->pos(), target()->pos()) <= standRadiusSq())) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateStand;

                if (patrol() && !sndPlaying()) {
                    sndPatrolEnd_->play();
                }

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

            if (parent()->linearVelocity().Length() < settings.sentry.walkSpeed) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(settings.sentry.walkSpeed * 4.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        default:
            assert(false);
            break;
        }

        if (readyToShoot) {
            if (!readyToShoot_) {
                shootTime_ = getRandom(0.0f, shootDelay_);
                if (!sndPlaying()) {
                    sndTarget_[rand() % 2]->play();
                }
            }

            if (shootTime_ <= 0.0f) {
                weapon_->trigger(true);
            }

            shootTime_ -= dt;
        } else if (readyToShoot_) {
            if (!sndPlaying()) {
                sndTargetLost_->play();
            }
        }

        readyToShoot_ = readyToShoot;
    }

    void SentryComponent::onRegister()
    {
        if (parent()->type() != SceneObjectTypeAlly) {
            scene()->stats()->incEnemiesSpawned();
        }

        if (state_ == StateFolded) {
            torsoAc_->startAnimation(AnimationUnfold);
            legsAc_->startAnimation(AnimationUnfold);
            torsoAc_->setPaused(true);
            legsAc_->setPaused(true);
            parent()->setInvulnerable(true);
            parent()->body()->SetType(b2_kinematicBody);
        } else {
            torsoAc_->startAnimation(AnimationDefault);
            legsAc_->startAnimation(AnimationDefault);

            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.0f), 0, 3.0f, 0.5f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            parent()->addComponent(component);
        }
    }

    void SentryComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    float SentryComponent::standRadiusSq() const
    {
        return patrol() ? (targetDistance_ * targetDistance_) : (settings.sentry.standRadius * settings.sentry.standRadius);
    }

    float SentryComponent::walkRadiusSq() const
    {
        return patrol() ? ((targetDistance_ + 1) * (targetDistance_ + 1)) : (settings.sentry.walkRadius * settings.sentry.walkRadius);
    }

    bool SentryComponent::sndPlaying() const
    {
        return ((sndTarget_[0]->status() == AudioSource::Playing) ||
            (sndTarget_[1]->status() == AudioSource::Playing) ||
            (sndTargetLost_->status() == AudioSource::Playing) ||
            (sndPatrolStart_[0]->status() == AudioSource::Playing) ||
            (sndPatrolStart_[1]->status() == AudioSource::Playing) ||
            (sndPatrolStart_[2]->status() == AudioSource::Playing) ||
            (sndPatrolEnd_->status() == AudioSource::Playing));
    }
}
