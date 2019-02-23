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

#include "AcidWormComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "RenderQuadComponent.h"
#include "RenderHealthbarComponent.h"
#include "SingleTweening.h"
#include "PhysicsJointComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    AcidWormComponent::AcidWormComponent(const SceneObjectPtr& head, const SceneObjectPtr& body, bool folded, bool foldable, float unfoldRadius,
        const AnimationComponentPtr& ac)
    : TargetableComponent(phaseThink),
      head_(head),
      body_(body),
      foldable_(foldable),
      unfoldRadius_(unfoldRadius),
      ac_(ac),
      firstFolded_(folded),
      idleRot_(0.0f),
      t_(0.0f),
      state_(folded ? StateFolded : StateIdle),
      sndSplash_(audio.createSound("splash1.ogg"))
    {
        sndAttack_[0] = audio.createSound("acidworm_attack1.ogg");
        sndAttack_[1] = audio.createSound("acidworm_attack2.ogg");
    }

    AcidWormComponent::~AcidWormComponent()
    {
    }

    void AcidWormComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void AcidWormComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            ac_->setPaused(true);

            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            for (PhysicsJointComponent::Joints::const_iterator it = jc->joints().begin();
                 it != jc->joints().end(); ++it) {
                (*it)->remove();
            }

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 0.8f);
            parent()->addComponent(component);

            b2Vec2 velDir = body_->linearVelocity();
            b2Rot rot(2.0f * b2_pi / parent()->objects().size());

            float len = velDir.Normalize();

            if (len > 30.0f) {
                velDir *= 30.0f;
            } else if (len < 15.0f) {
                velDir *= 15.0f;
            } else {
                velDir *= len;
            }

            b2Vec2 velDirOrig = velDir;

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                 it != parent()->objects().end(); ++it) {
                SceneObjectPtr obj = sceneObjectFactory.createBlood1(6.0f, zOrderFront + 1);

                obj->setPos((*it)->pos());
                obj->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(obj);

                (*it)->findComponent<PhysicsBodyComponent>()->disableAllFixtures();

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 0.8f);
                (*it)->addComponent(component);

                velDir = b2Mul(rot, velDir);

                if ((rand() % 2) == 0) {
                    velDir = -velDir;
                }

                (*it)->setAngularDamping(2.0f);

                if ((*it) != body_) {
                    (*it)->setLinearVelocity((*it)->linearVelocity() + velDir);
                } else {
                    (*it)->setLinearVelocity(velDirOrig);
                }

                (*it)->setAngularVelocity((*it)->angularVelocity() + b2_pi * getRandom(1.5f, 4.0f) * (((rand() % 2) == 0) ? 1.0f : -1.0f));
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderFront);

            explosion->setTransform(body_->getTransform());

            scene()->addObject(explosion);

            RenderHealthbarComponentPtr hc = head_->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        t_ -= dt;

        if ((ac_->currentAnimation() == AnimationAttack) && ac_->animationFinished()) {
            ac_->startAnimation(AnimationDefault);
        }

        updateAutoTarget(dt);

        if (target() && (state_ >= StateIdle) && (state_ <= StateSpit2)) {
            b2Vec2 targetDir = target()->pos() - head_->pos();

            b2Vec2 dir = angle2vec(head_->angle() + head_->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                head_->applyTorque(settings.acidworm.turnTorque, true);
            } else {
                head_->applyTorque(-settings.acidworm.turnTorque, true);
            }
        }

        switch (state_) {
        case StateIdle: {
            if (!target()) {
                break;
            }
            state_ = StateWalk;
            t_ = getRandom(settings.acidworm.spitTimeout * 0.7f, settings.acidworm.spitTimeout * 1.2f);
            break;
        }
        case StateWalk: {
            if (!target()) {
                state_ = StateIdle;
                break;
            }

            walk(dt);

            if (t_ <= 0.0f) {
                t_ = getRandom(settings.acidworm.spitTimeout * 0.7f, settings.acidworm.spitTimeout * 1.2f);

                if ((fabs(angleBetween(target()->pos() - head_->pos(), head_->getDirection(1.0f))) <= settings.acidworm.shootAngle) &&
                    scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 8.0f, 8.0f)) {
                    state_ = StateSpit1;
                    t_ = settings.acidworm.spitForce1Duration;
                    sndAttack_[rand() % 2]->play();
                    break;
                }
            }

            break;
        }
        case StateSpit1: {
            if (t_ <= 0.0f) {
                state_ = StateSpit2;
                t_ = settings.acidworm.spitForce2Duration;
                break;
            }
            head_->applyForceToCenter(head_->getDirection(settings.acidworm.spitForce1), true);
            break;
        }
        case StateSpit2: {
            if (t_ <= 0.0f) {
                idleRot_ = b2Rot(head_->angle() + b2_pi);
                weapon_->triggerOnce();
                ac_->startAnimation(AnimationAttack);
                if (foldable_) {
                    state_ = StatePreFold;
                    t_ = settings.acidworm.foldDelay;
                } else {
                    state_ = StateIdle;
                }
                break;
            }
            head_->applyForceToCenter(head_->getDirection(settings.acidworm.spitForce2), true);
            break;
        }
        case StatePreFold: {
            if (t_ <= 0.0f) {
                state_ = StateFold1;
                t_ = 0.5f;
                break;
            }

            walk(dt);

            break;
        }
        case StateFold1: {
            if (t_ <= 0.0f) {
                state_ = StateFold2;
                tweening_ = boost::make_shared<SingleTweening>(0.5f, EaseInQuad, 1.0f, 0.6f);
                t_ = 0.0f;
                SceneObjectPtr obj = sceneObjectFactory.createToxicSplash2(zOrderBack + 1);
                obj->setTransform(parent()->getTransform());
                scene()->addObject(obj);
                sndSplash_->play();
                setupHealth(true);
                break;
            }

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                it != parent()->objects().end(); ++it) {
                b2Vec2 tmp = parent()->pos() - (*it)->pos();
                tmp.Normalize();
                (*it)->applyForceToCenter(settings.acidworm.foldForce * tmp, true);
            }
            break;
        }
        case StateFold2: {
            if (tweening_->finished(-t_)) {
                parent()->setVisibleRecursive(false);
                parent()->setActiveRecursive(false);
                t_ = getRandom(settings.acidworm.spitTimeout * 0.7f, settings.acidworm.spitTimeout * 1.2f);
                state_ = StateFolded;
                break;
            }

            float val = tweening_->getValue(-t_);

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                it != parent()->objects().end(); ++it) {
                RenderQuadComponentPtr rc = (*it)->findComponent<RenderQuadComponent>();
                rc->setHeight(sizeMap_[it->get()] * val);
            }

            break;
        }
        case StateFolded: {
            if (!target()) {
                break;
            }

            if (firstFolded_ && (b2DistanceSquared(target()->pos(), parent()->pos()) <= unfoldRadius_ * unfoldRadius_)) {
                t_ = 0.0f;
            }

            if (t_ <= 0.0f) {
                t_ = getRandom(settings.acidworm.spitTimeout * 0.7f, settings.acidworm.spitTimeout * 1.2f);

                if ((!firstFolded_ || (b2DistanceSquared(target()->pos(), parent()->pos()) <= unfoldRadius_ * unfoldRadius_)) &&
                    scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 8.0f, 8.0f)) {
                    firstFolded_ = false;
                    state_ = StateUnfold1;
                    t_ = 0.3f;
                    SceneObjectPtr obj = sceneObjectFactory.createToxicSplash2(zOrderBack + 1);
                    obj->setTransform(parent()->getTransform());
                    scene()->addObject(obj);
                    sndSplash_->play();
                    break;
                }
            }
            break;
        }
        case StateUnfold1: {
            if (t_ <= 0.0f) {
                if (target()) {
                    parent()->setAngleRecursive(normalizeAngle(vec2angle(target()->pos() - parent()->pos()) + (parent()->angle() - head_->angle())));
                }
                parent()->setVisibleRecursive(true);
                parent()->setActiveRecursive(true);
                tweening_ = boost::make_shared<SingleTweening>(0.5f, EaseOutQuad, 0.6f, 1.0f);
                t_ = 0.0f;
                state_ = StateUnfold2;
                break;
            }
            break;
        }
        case StateUnfold2: {
            if (tweening_->finished(-t_)) {
                for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                    it != parent()->objects().end(); ++it) {
                    RenderQuadComponentPtr rc = (*it)->findComponent<RenderQuadComponent>();
                    rc->setHeight(sizeMap_[it->get()]);
                }
                state_ = StateWalk;
                t_ = getRandom(settings.acidworm.unfoldDelay * 0.7f, settings.acidworm.unfoldDelay * 1.2f);
                setupHealth(false);
                break;
            }

            float val = tweening_->getValue(-t_);

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                it != parent()->objects().end(); ++it) {
                RenderQuadComponentPtr rc = (*it)->findComponent<RenderQuadComponent>();
                rc->setHeight(sizeMap_[it->get()] * val);
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void AcidWormComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
            it != parent()->objects().end(); ++it) {
            RenderQuadComponentPtr rc = (*it)->findComponent<RenderQuadComponent>();
            sizeMap_[it->get()] = rc->height();
            if (state_ == StateFolded) {
                rc->setHeight(sizeMap_[it->get()] * 0.6f);
                (*it)->setPos(parent()->pos());
            }
        }

        if (state_ == StateFolded) {
            parent()->setVisibleRecursive(false);
            parent()->setActiveRecursive(false);
        }

        setupHealth(state_ == StateFolded);
    }

    void AcidWormComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        head_.reset();
        body_.reset();
    }

    void AcidWormComponent::walk(float dt)
    {
        b2Vec2 v = b2Mul(idleRot_, b2Vec2(settings.acidworm.idleForce, 0.0f));

        head_->applyForceToCenter(v, true);

        idleRot_ = b2Mul(idleRot_, b2Rot(settings.acidworm.idleSpeed * dt));
    }

    void AcidWormComponent::setupHealth(bool invulnerable)
    {
        parent()->setInvulnerable(invulnerable);

        if (invulnerable) {
            RenderHealthbarComponentPtr c = head_->findComponent<RenderHealthbarComponent>();
            if (c) {
                c->removeFromParent();
            }
        } else {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 3.0f), 0, 6.0f, 0.8f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
            component->setTarget(parent()->shared_from_this());

            head_->addComponent(component);
        }
    }
}
