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

#include "BossSquidComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Utils.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "RenderHealthbarComponent.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "SequentialTweening.h"
#include "FadeOutComponent.h"
#include "TentacleSwingComponent.h"
#include "TentaclePulseComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    BossSquidComponent::BossSquidComponent(const SceneObjectPtr tentacle[8],
        const SceneObjectPtr legControl[4],
        const SceneObjectPtr eyeControl[4],
        const SceneObjectPtr mouthControl[4],
        const SceneObjectPtr leg[4],
        const SceneObjectPtr mouth[4],
        const Points& explosionPos)
    : TargetableComponent(phaseThink),
      state_(StateInit),
      explosionPos_(explosionPos),
      legIndex_(0),
      t_(0.0f),
      upDown_(false),
      explosionIndex_(0),
      sndSpawnEyes_(audio.createSound("squid_spawn_eyes.ogg")),
      sndAngry_(audio.createSound("squid_alert3.ogg")),
      sndLegHit_(audio.createSound("queen_hit.ogg")),
      sndPain_(audio.createSound("squid_pain.ogg")),
      sndMouthMove_(audio.createStream("queen_shake.ogg")),
      sndDie_(audio.createStream("squid_die.ogg"))
    {
        for (int i = 0; i < 8; ++i) {
            tentacle_[i] = tentacle[i]->findComponent<TentacleSquidComponent>();
        }
        for (int i = 0; i < 4; ++i) {
            legControl_[i] = legControl[i];
            eyeControl_[i] = eyeControl[i];
            eyeDying_[i] = false;
            mouthControl_[i] = mouthControl[i];
            leg_[i] = leg[i];
            mouth_[i] = mouth[i];
        }
    }

    BossSquidComponent::~BossSquidComponent()
    {
    }

    void BossSquidComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossSquidComponent::update(float dt)
    {
        t_ -= dt;

        updateAutoTarget(dt);

        for (int i = 0; i < 8; ++i) {
            tentacle_[i]->setTarget(SceneObjectPtr());
        }

        if (target() && (state_ >= StateIdle)) {
            if (state_ >= StateSpin1) {
                for (int i = 0; i < 8; ++i) {
                    tentacle_[i]->setTarget(target());
                }
            } else {
                float angle = angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos());

                if ((angle >= -b2_pi / 4.0f) && (angle <= b2_pi / 4.0f)) {
                    tentacle_[0]->setTarget(target());
                    tentacle_[1]->setTarget(target());
                } else if ((angle > b2_pi / 4.0f) && (angle < 3.0f * b2_pi / 4.0f)) {
                    tentacle_[2]->setTarget(target());
                    tentacle_[3]->setTarget(target());
                } else if ((fabs(angle) >= 3.0f * b2_pi / 4.0f)) {
                    tentacle_[4]->setTarget(target());
                    tentacle_[5]->setTarget(target());
                } else {
                    tentacle_[6]->setTarget(target());
                    tentacle_[7]->setTarget(target());
                }
            }
        }

        bool legsMoved = true;
        bool eyesMoved = true;
        bool mouthsMoved = true;

        for (int i = 0; i < 4; ++i) {
            if (legControl_[i]) {
                legControl_[i]->setLinearVelocity(b2Vec2_zero);
                legControl_[i]->setAngularVelocity(0.0f);
            }
            eyeControl_[i]->setLinearVelocity(b2Vec2_zero);
            eyeControl_[i]->setAngularVelocity(0.0f);
            mouthControl_[i]->setLinearVelocity(b2Vec2_zero);
            mouthControl_[i]->setAngularVelocity(0.0f);
        }

        bool bodyMoved = updateBody(dt);

        for (int i = 0; i < 4; ++i) {
            legsMoved &= updateLeg(i, dt);
        }

        for (int i = 0; i < 4; ++i) {
            bool eyeMoved = updateEye(i, dt);
            eyesMoved &= eyeMoved;

            if (eye_[i] && eye_[i]->dead() && eyeMoved) {
                if (eyeDying_[i]) {
                    eyeDying_[i] = false;
                    eye_[i]->parentObject()->removeFromParent();
                    eye_[i].reset();
                } else {
                    RenderHealthbarComponentPtr hc =
                        eye_[i]->findComponent<RenderHealthbarComponent>();
                    if (hc) {
                        hc->removeFromParent();
                    }
                    eyeDying_[i] = true;
                    moveEye(i, -15.0f, 1.0f, EaseInQuad);
                    moveBodyPain(4.0f, deg2rad(10.0f));
                    sndPain_->play();
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            mouthsMoved &= updateMouth(i, dt);
        }

        switch (state_) {
        case StateInit: {
            if (!target()) {
                break;
            }

            for (std::vector<TentacleAttractComponentPtr>::iterator it = welds_.begin();
                it != welds_.end(); ++it) {
                (*it)->target()->removeFromParent();
                (*it)->removeFromParent();
            }
            welds_.clear();

            state_ = StateFirstSpawnEyes;

            spawnEyes();

            break;
        }
        case StateFirstSpawnEyes:
        case StateSpawnEyes: {
            if (!eyesMoved) {
                break;
            }

            for (int i = 0; i < 4; ++i) {
                eye_[i]->setInvulnerable(false);

                RenderHealthbarComponentPtr component =
                    boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 3.5f), 0, 8.0f, 1.2f, zOrderMarker + 1);

                component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                eye_[i]->addComponent(component);
            }

            if (state_ == StateFirstSpawnEyes) {
                state_ = StateFirstWait;
                t_ = 1.0f;
            } else {
                state_ = StateIdle;
                t_ = 1.0f;
            }

            break;
        }
        case StateFirstWait: {
            if (t_ >= 0.0f) {
                break;
            }
            for (int i = 0; i < 8; ++i) {
                tentacle_[i]->setMelee(true);
            }
            for (int i = 0; i < 4; ++i) {
                moveLeg(i, -5.0f, 0.25f, EaseOutQuad);
            }
            upDown_ = true;
            state_ = StateFirstAngry;
            t_ = 2.5f;
            sndAngry_->play();
            break;
        }
        case StateFirstAngry: {
            if (!legsMoved) {
                break;
            }

            if (!upDown_) {
                scene()->camera()->findComponent<CameraComponent>()->roll(deg2rad(5.0f));
                sndLegHit_->play();
            }

            if ((t_ >= 0.0f) || upDown_) {
                for (int i = 0; i < 4; ++i) {
                    if (upDown_) {
                        moveLeg(i, 5.0f, 0.25f, EaseInQuad);
                    } else {
                        moveLeg(i, -5.0f, 0.25f, EaseOutQuad);
                    }
                }

                upDown_ = !upDown_;
                break;
            }

            for (int i = 0; i < 8; ++i) {
                tentacle_[i]->setMelee(false);
            }
            state_ = StateIdle;
            t_ = 2.0f;
            break;
        }
        case StateIdle: {
            if (!target()) {
                break;
            }

            bool allEyesDead = true;
            for (int i = 0; i < 4; ++i) {
                if (eye_[i]) {
                    allEyesDead = false;
                    break;
                }
            }

            if (allEyesDead) {
                if (bodyMoved) {
                    scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);
                    sndMouthMove_->play();

                    showHealthbar();

                    state_ = StateMouthOpen;
                    for (int i = 0; i < 4; ++i) {
                        moveMouth(i, 5.0f, 1.5f, EaseOutQuad);
                    }
                }

                break;
            }

            if (t_ < 0.0f) {
                state_ = StateLegAim;
                moveLeg(legIndex_, -15.0f, 2.0f, EaseOutQuad);
            }

            break;
        }
        case StateLegAim: {
            if (!legsMoved) {
                break;
            }

            state_ = StateLegDown;
            moveLeg(legIndex_, 15.0f, 0.5f, EaseInQuad);

            break;
        }
        case StateLegDown: {
            if (!legsMoved) {
                break;
            }

            scene()->camera()->findComponent<CameraComponent>()->roll(deg2rad(5.0f));
            sndLegHit_->play();

            state_ = StateIdle;
            legIndex_ = (legIndex_ + 1) % 4;
            t_ = 0.25f;

            break;
        }
        case StateMouthOpen: {
            if (!mouthsMoved) {
                break;
            }
            t_ = settings.bossSquid.mouthOpenTime;
            state_ = StateMouthOpened;

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndMouthMove_->stop();

            break;
        }
        case StateMouthOpened: {
            if (t_ >= 0.0f) {
                break;
            }
            state_ = StateMouthClose;
            for (int i = 0; i < 4; ++i) {
                moveMouth(i, -5.0f, 1.5f, EaseOutQuad);
            }

            scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);
            sndMouthMove_->play();

            break;
        }
        case StateMouthClose: {
            if (!mouthsMoved) {
                break;
            }

            RenderHealthbarComponentPtr hc =
                parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }
            parent()->setInvulnerable(true);

            if (parent()->lifePercent() <= settings.bossSquid.secondPhaseLifePercent) {
                state_ = StateRetractLegs;

                std::vector<JointProxyPtr> joints =
                    parent()->findComponent<PhysicsJointComponent>()->joints<JointProxy>("leg_joint");

                for (std::vector<JointProxyPtr>::const_iterator it = joints.begin();
                    it != joints.end(); ++it) {
                    (*it)->remove();
                }

                for (int i = 0; i < 4; ++i) {
                    scene()->addWeldJointEx(legControl_[i], leg_[i], leg_[i]->pos(), 2.0f, 0.7f);
                    moveLeg(i, -30.0f, 2.0f, EaseOutQuad);
                }
            } else {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                sndMouthMove_->stop();

                state_ = StateSpawnEyes;

                spawnEyes();
            }

            break;
        }
        case StateRetractLegs: {
            if (!legsMoved) {
                break;
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);
            sndMouthMove_->stop();

            for (int i = 0; i < 4; ++i) {
                legControl_[i]->removeFromParent();
                legControl_[i].reset();
                scene()->addWeldJointEx(mouthControl_[i], leg_[i], leg_[i]->pos(), 2.0f, 0.7f);

                RenderTentacleComponentPtr tc = leg_[i]->findComponent<RenderTentacleComponent>();

                scene()->addWeldJointEx(mouthControl_[i], tc->objects()[3], tc->objects()[3]->pos(), 2.0f, 0.7f);

                for (int j = 0; j < 3; ++j) {
                    tc->objects()[j]->findComponent<PhysicsBodyComponent>()->disableAllFixtures();
                }
            }

            showHealthbar();

            moveBodyRotate(deg2rad(5.0f));

            state_ = StateSpin1;
            break;
        }
        case StateSpin1: {
            if (!mouthsMoved) {
                break;
            }
            state_ = StateSpin2;
            for (int i = 0; i < 4; ++i) {
                moveMouth(i, 5.0f, 0.8f, EaseInOutQuad);
            }
            break;
        }
        case StateSpin2: {
            if (!mouthsMoved) {
                break;
            }
            state_ = StateSpin3;
            t_ = 2.0f;
            break;
        }
        case StateSpin3: {
            if (parent()->dead()) {
                bodyTweening_.reset();

                RenderHealthbarComponentPtr hc =
                    parent()->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }

                FadeOutComponentPtr c =
                    boost::make_shared<FadeOutComponent>(0.4f * explosionPos_.size());
                c->setLightsOnly(true);

                parent()->addComponent(c);

                state_ = StateDie1;
                t_ = 0.0f;

                for (int i = 0; i < 8; ++i) {
                    tentacle_[i]->setMelee(true);
                }

                sndDie_->play();

                break;
            }

            if (t_ >= 0.0f) {
                break;
            }
            state_ = StateSpin1;
            for (int i = 0; i < 4; ++i) {
                moveMouth(i, -5.0f, 0.8f, EaseInOutQuad);
            }
            break;
        }
        case StateDie1: {
            if (t_ >= 0.0f) {
                break;
            }

            if (explosionIndex_ == static_cast<int>(explosionPos_.size()) - 1) {
                for (int i = 0; i < 8; ++i) {
                    tentacle_[i]->setMelee(false);
                }
            } else if (explosionIndex_ >= static_cast<int>(explosionPos_.size())) {
                for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                    it != parent()->objects().end(); ++it) {
                    ComponentPtr c = (*it)->findComponent<TentacleSwingComponent>();
                    if (c) {
                        c->removeFromParent();
                    }
                    c = (*it)->findComponent<TentaclePulseComponent>();
                    if (c) {
                        c->removeFromParent();
                    }
                }
                state_ = StateDie2;
                scene()->stats()->incEnemiesKilled();
                break;
            }

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);
            explosion->setPos(parent()->getWorldPoint(explosionPos_[explosionIndex_]));
            scene()->addObject(explosion);

            ++explosionIndex_;
            t_ = 0.4f;
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

    void BossSquidComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        parent()->setInvulnerable(true);

        for (int i = 0; i < 8; ++i) {
            RenderTentacleComponentPtr tc =
                tentacle_[i]->parent()->findComponent<RenderTentacleComponent>();

            TentacleAttractComponentPtr ac =
                boost::make_shared<TentacleAttractComponent>(3.0f, 0.5f);
            ac->setBone(2);

            SceneObjectPtr dummy = sceneObjectFactory.createDummy();
            dummy->setPos(tc->objects()[2]->pos());
            scene()->addObject(dummy);

            ac->setTarget(dummy);

            tentacle_[i]->parent()->addComponent(ac);

            welds_.push_back(ac);

            ac = boost::make_shared<TentacleAttractComponent>(3.0f, 0.5f);
            ac->setBone(6);

            dummy = sceneObjectFactory.createDummy();
            dummy->setPos(tc->objects()[6]->pos());
            scene()->addObject(dummy);

            ac->setTarget(dummy);

            tentacle_[i]->parent()->addComponent(ac);

            welds_.push_back(ac);
        }
    }

    void BossSquidComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (std::vector<TentacleAttractComponentPtr>::iterator it = welds_.begin();
            it != welds_.end(); ++it) {
            (*it)->target()->removeFromParent();
            (*it)->removeFromParent();
        }
        welds_.clear();
        for (int i = 0; i < 4; ++i) {
            legControl_[i].reset();
            eyeControl_[i].reset();
            if (eye_[i]) {
                eye_[i]->parentObject()->removeFromParent();
                eye_[i].reset();
            }
            mouthControl_[i].reset();
            leg_[i].reset();
            mouth_[i].reset();
        }
    }

    bool BossSquidComponent::updateLeg(int index, float dt)
    {
        if (!legTweening_[index]) {
            return true;
        }

        legTweenTime_[index] += dt;

        float nd = legTweening_[index]->getValue(legTweenTime_[index]);

        b2Vec2 dir = legControl_[index]->pos() - parent()->pos();

        float d = dir.Normalize();

        legControl_[index]->setLinearVelocity(legControl_[index]->linearVelocity() + ((nd - d) / dt) * dir);

        if (legTweening_[index]->finished(legTweenTime_[index] - dt)) {
            legTweening_[index].reset();
            return true;
        } else {
            return false;
        }
    }

    void BossSquidComponent::moveLeg(int index, float distance, float duration, Easing easing)
    {
        float d = (legControl_[index]->pos() - parent()->pos()).Length();

        legTweening_[index] = boost::make_shared<SingleTweening>(duration, easing, d, d + distance);
        legTweenTime_[index] = 0.0f;
    }

    bool BossSquidComponent::updateEye(int index, float dt)
    {
        if (!eyeTweening_[index]) {
            return true;
        }

        eyeTweenTime_[index] += dt;

        float nd = eyeTweening_[index]->getValue(eyeTweenTime_[index]);

        b2Vec2 dir = eyeControl_[index]->pos() - parent()->pos();

        float d = dir.Normalize();

        eyeControl_[index]->setLinearVelocity(eyeControl_[index]->linearVelocity() + ((nd - d) / dt) * dir);

        if (eyeTweening_[index]->finished(eyeTweenTime_[index] - dt)) {
            eyeTweening_[index].reset();
            return true;
        } else {
            return false;
        }
    }

    void BossSquidComponent::moveEye(int index, float distance, float duration, Easing easing)
    {
        float d = (eyeControl_[index]->pos() - parent()->pos()).Length();

        eyeTweening_[index] = boost::make_shared<SingleTweening>(duration, easing, d, d + distance);
        eyeTweenTime_[index] = 0.0f;
    }

    bool BossSquidComponent::updateMouth(int index, float dt)
    {
        if (!mouthTweening_[index]) {
            return true;
        }

        mouthTweenTime_[index] += dt;

        float nd = mouthTweening_[index]->getValue(mouthTweenTime_[index]);

        b2Vec2 dir = mouthControl_[index]->pos() - parent()->pos();

        float d = dir.Normalize();

        mouthControl_[index]->setLinearVelocity(mouthControl_[index]->linearVelocity() + ((nd - d) / dt) * dir);

        if (mouthTweening_[index]->finished(mouthTweenTime_[index] - dt)) {
            mouthTweening_[index].reset();
            return true;
        } else {
            return false;
        }
    }

    void BossSquidComponent::moveMouth(int index, float distance, float duration, Easing easing)
    {
        float d = (mouthControl_[index]->pos() - parent()->pos()).Length();

        mouthTweening_[index] = boost::make_shared<SingleTweening>(duration, easing, d, d + distance);
        mouthTweenTime_[index] = 0.0f;
    }

    bool BossSquidComponent::updateBody(float dt)
    {
        if (!bodyTweening_) {
            return true;
        }

        bodyTweenTime_ += dt;

        float value = bodyTweening_->getValue(bodyTweenTime_);

        b2Rot rot(value);

        for (int i = 0; i < 4; ++i) {
            b2Vec2 dir = eyeControl_[i]->pos() - parent()->pos();
            float len = dir.Normalize();

            b2Vec2 pos = parent()->pos() + b2Mul(rot, len * eyeControlInit_[i]);

            eyeControl_[i]->setLinearVelocity(eyeControl_[i]->linearVelocity() + (1.0f / dt) * (pos - eyeControl_[i]->pos()));
            eyeControl_[i]->setAngularVelocity((eyeControlInitAngle_[i] + value - eyeControl_[i]->angle()) * (1.0f / dt));

            dir = mouthControl_[i]->pos() - parent()->pos();
            len = dir.Normalize();

            pos = parent()->pos() + b2Mul(rot, len * mouthControlInit_[i]);

            mouthControl_[i]->setLinearVelocity(mouthControl_[i]->linearVelocity() + (1.0f / dt) * (pos - mouthControl_[i]->pos()));
            mouthControl_[i]->setAngularVelocity((mouthControlInitAngle_[i] + value - mouthControl_[i]->angle()) * (1.0f / dt));
        }

        if (bodyTweening_->finished(bodyTweenTime_ - dt)) {
            bodyTweening_.reset();
            return true;
        } else {
            return false;
        }
    }

    void BossSquidComponent::moveBodyPain(float duration, float angle)
    {
        bodyTweening_ = boost::make_shared<SingleTweening>(duration,
            VibrateFactor / 2.0f, EaseOutVibrate, 0.0f, angle);
        bodyTweenTime_ = 0.0f;

        moveBodyInit();
    }

    void BossSquidComponent::moveBodyRotate(float velocity)
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(b2_pi / velocity, EaseInOutQuad, -b2_pi / 2.0f, b2_pi / 2.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(b2_pi / velocity, EaseInOutQuad, b2_pi / 2.0f, -b2_pi / 2.0f));

        bodyTweening_ = tweening;
        bodyTweenTime_ = b2_pi / velocity / 2.0f;

        moveBodyInit();
    }

    void BossSquidComponent::moveBodyInit()
    {
        for (int i = 0; i < 4; ++i) {
            eyeControlInitAngle_[i] = eyeControl_[i]->angle();
            eyeControlInit_[i] = eyeControl_[i]->pos() - parent()->pos();
            eyeControlInit_[i].Normalize();
            mouthControlInitAngle_[i] = mouthControl_[i]->angle();
            mouthControlInit_[i] = mouthControl_[i]->pos() - parent()->pos();
            mouthControlInit_[i].Normalize();
        }
    }

    void BossSquidComponent::spawnEyes()
    {
        for (int i = 0; i < 4; ++i) {
            eye_[i] = sceneObjectFactory.createBossSquidEye();
            eye_[i]->setTransformRecursive(eyeControl_[i]->getTransform());
            parent()->addObject(eye_[i]);
            scene()->addWeldJointEx(eyeControl_[i], eye_[i], eyeControl_[i]->pos(), 6.0f, 0.7f);

            RenderTentacleComponentPtr tc =
               eye_[i]->findComponent<RenderTentacleComponent>();
            eye_[i] = tc->objects().back();
            eye_[i]->setInvulnerable(true);

            moveEye(i, 15.0f, 1.5f, EaseOutQuad);
        }

        sndSpawnEyes_->play();
    }

    void BossSquidComponent::showHealthbar()
    {
        parent()->setInvulnerable(false);

        RenderHealthbarComponentPtr component =
            boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 7.5f), 0, 12.0f, 2.0f, zOrderMarker + 1);

        component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

        parent()->addComponent(component);
    }
}
