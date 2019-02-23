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

#include "BossBuddyComponent.h"
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
#include "RenderTentacleComponent.h"
#include "PhysicsRopeComponent.h"
#include "BuddyTongueComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    BossBuddyComponent::BossBuddyComponent(const BuddySideComponentPtr sides[3],
        const SceneObjectPtr& head, const SceneObjectPtr& headS,
        const b2Transform& tongueXf, const SceneObjectPtr pipes[2],
        const b2Transform eyeletXf[2], const SceneObjectPtr& brain,
        const SceneObjectPtr& root3, const SceneObjectPtr& root3S,
        const b2Transform& napalmXf, const SceneObjectPtr jaws[2],
        const SceneObjectPtr skull[2], const SceneObjectPtr& frontGun)
    : TargetableComponent(phaseThink),
      head_(head),
      headS_(headS),
      tongueXf_(tongueXf),
      brain_(brain),
      root3_(root3),
      root3S_(root3S),
      napalmXf_(napalmXf),
      frontGun_(frontGun),
      state_(StateIdle),
      idleRot_(0.0f),
      idleRotDir_(1),
      idleRotT_(1.0f),
      t_(0.0f),
      attackNum_(0),
      canSwallow_(false),
      prevHealth_(0.0f),
      painT_(0.0f)
    {
        sides_[0] = sides[0];
        sides_[1] = sides[1];
        sides_[2] = sides[2];
        pipes_[0] = pipes[0];
        pipes_[1] = pipes[1];
        eyeletXf_[0] = eyeletXf[0];
        eyeletXf_[1] = eyeletXf[1];
        jaws_[0] = jaws[0];
        jaws_[1] = jaws[1];
        skull_[0] = skull[0];
        skull_[1] = skull[1];

        sndJawOpen_ = audio.createSound("buddy_jaw_open.ogg");
        sndJawClose_ = audio.createSound("buddy_jaw_close.ogg");
        sndSidegunExtend_ = audio.createSound("buddy_sidegun_extend.ogg");
        sndSidegunRetract_ = audio.createSound("buddy_sidegun_retract.ogg");
        sndFrontgunExtend_ = audio.createSound("buddy_frontgun_extend.ogg");
        sndFrontgunRetract_ = audio.createSound("buddy_frontgun_retract.ogg");
        sndTongueCaptured_ = audio.createSound("buddy_tongue_captured.ogg");
        sndBrainPain_[0] = audio.createSound("buddy_brain_pain1.ogg");
        sndBrainPain_[1] = audio.createSound("buddy_brain_pain2.ogg");
    }

    BossBuddyComponent::~BossBuddyComponent()
    {
    }

    void BossBuddyComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossBuddyComponent::update(float dt)
    {
        headS_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (head_->pos() - headS_->pos()));
        headS_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(head_->angle() - headS_->angle()));

        root3S_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (root3_->pos() - root3S_->pos()));
        root3S_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(root3_->angle() - root3S_->angle()));

        t_ -= dt;
        painT_ -= dt;

        if (!brain_->dead() && (brain_->life() < prevHealth_)) {
            if (painT_ <= 0.0f) {
                sndBrainPain_[0]->stop();
                sndBrainPain_[1]->stop();
                sndBrainPain_[rand() % 2]->play();
                painT_ = 0.4f;
            }
        }

        prevHealth_ = brain_->life();

        updateAutoTarget(dt);

        if (target() && (sides_[0]->parent()->body()->GetType() == b2_kinematicBody) && (state_ != StateDead) && (state_ != StateDead2)) {
            for (int i = 0; i < 3; ++i) {
                sides_[i]->parent()->body()->SetType(b2_dynamicBody);
            }
        }

        for (int i = 0; i < 3; ++i) {
            sides_[i]->setTarget(target());
        }

        if (tongue_) {
            tongue_->findComponent<TargetableComponent>()->setTarget((state_ == StateTongueAttack) ? target() : SceneObjectPtr());
        }

        if (target() && (state_ != StateDead) && (state_ != StateDead2)) {
            if ((state_ != StateNapalmLeanBack) && (state_ != StateNapalmStrike) &&
                (state_ != StateSwallowLeanBack) && (state_ != StateSwallowStrike)) {
                idleRotT_ -= dt;
            }

            if (idleRotT_ <= 0.0f) {
                idleRotT_ = 2.0f;
                idleRotDir_ = -idleRotDir_;
            }

            if (state_ < StateCaptured) {
                if ((state_ != StateNapalmLeanBack) && (state_ != StateNapalmStrike) &&
                    (state_ != StateSwallowLeanBack) && (state_ != StateSwallowStrike)) {
                    b2Vec2 v = b2Mul(idleRot_, head_->getDirection(-350.0f));

                    head_->applyForceToCenter(v, true);

                    idleRot_ = b2Mul(idleRot_, b2Rot(deg2rad(120.0f) * dt * idleRotDir_));
                }

                b2Vec2 targetDir = target()->pos() - head_->pos();

                b2Vec2 dir = angle2vec(head_->angle() + head_->angularVelocity() / 3.0f, 1.0f);

                if (b2Cross(dir, targetDir) >= 0.0f) {
                    head_->applyTorque(6500.0f, true);
                } else {
                    head_->applyTorque(-6500.0f, true);
                }

                if (parent()->linearVelocity().Length() <= 20.0f) {
                    b2Vec2 tmp = target()->pos() - parent()->pos();
                    tmp.Normalize();
                    parent()->applyForceToCenter(800.0f * tmp, true);
                }
            } else {
                b2Vec2 v = b2Mul(idleRot_, head_->getDirection(-350.0f));

                head_->applyForceToCenter(v, true);

                idleRot_ = b2Mul(idleRot_, b2Rot(deg2rad(120.0f) * dt * idleRotDir_));

                b2Vec2 targetDir = tongue_->pos() - head_->pos();

                b2Vec2 dir = angle2vec(head_->angle() + head_->angularVelocity() / 3.0f, 1.0f);

                if (b2Cross(dir, targetDir) >= 0.0f) {
                    head_->applyTorque(6500.0f, true);
                } else {
                    head_->applyTorque(-6500.0f, true);
                }

                if (parent()->linearVelocity().Length() <= 20.0f) {
                    b2Vec2 tmp = tongue_->pos() - origPos_;
                    tmp.Normalize();
                    parent()->applyForceToCenter(-800.0f * tmp, true);
                }
            }
        }

        switch (state_) {
        case StateIdle: {
            if (target()) {
                startWalk();
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                state_ = StateIdle;

                break;
            }

            if (t_ > 0.0f) {
                break;
            }

            if (!scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 7.5f, 7.5f)) {
                t_ = getRandom(1.0f, 3.0f);
                break;
            }

            bool showTongue;

            if (attackNum_ >= settings.bossBuddy.attackPreTongueNum) {
                showTongue = ((rand() % 6) != 0);
            } else {
                showTongue = ((rand() % 6) == 0);
            }

            ++attackNum_;

            if (showTongue) {
                openJaw();
                state_ = StateTongueOpenJaws;
                t_ = 1.0f;
                attackNum_ = 0;
            } else {
                int r = rand() % 3;

                if (r == 0) {
                    openJaw();
                    state_ = StateFrontGunOpenJaws;
                    t_ = 1.0f;
                } else if (r == 1) {
                    sideGunJoints_[0]->setMotorSpeed(-sideGunJoints_[0]->motorSpeed());
                    sideGunJoints_[1]->setMotorSpeed(-sideGunJoints_[1]->motorSpeed());
                    state_ = StateSideGunExtend;
                    t_ = 1.5f;
                    sndSidegunExtend_->play();
                } else {
                    openJaw();
                    state_ = StateNapalmOpenJaws;
                    t_ = 1.0f;
                }
            }

            break;
        }
        case StateTongueOpenJaws: {
            if (t_ <= 0.0f) {
                tongue_ = sceneObjectFactory.createBuddyTongue();
                tongue_->setTransformRecursive(b2Mul(headS_->getTransform(), tongueXf_));
                tongue_->setInvulnerable(true);
                scene()->addObject(tongue_);

                SceneObjectPtr last;

                for (std::set<SceneObjectPtr>::const_iterator it = tongue_->objects().begin();
                    it != tongue_->objects().end(); ++it) {
                    RenderTentacleComponentPtr tc = (*it)->findComponent<RenderTentacleComponent>();
                    if (tc) {
                        last = tc->objects().back();
                        break;
                    }
                }

                tongueJointLen_ = (tongue_->pos() - last->pos()).Length() + 2.0f;
                tongueJoint_ = scene()->addPrismaticJoint(headS_, last,
                    headS_->getLocalPoint(last->pos()), b2Vec2_zero, 0.0f,
                    tongueJointLen_,
                    b2Vec2(1.0f, 0.0f), true);

                tongueJoint_->setMaxMotorForce(2000.0f);
                tongueJoint_->setMotorSpeed(30.0f);
                tongueJoint_->enableMotor(true);

                state_ = StateTongueExtend;
                t_ = 1.0f;
                break;
            }
            break;
        }
        case StateTongueExtend: {
            if (t_ <= 0.0f) {
                tongue_->setInvulnerable(false);

                RenderHealthbarComponentPtr component =
                    boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.5f), 0, 8.0f, 1.2f, zOrderMarker + 1);
                component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
                tongue_->addComponent(component);

                state_ = StateTongueAttack;
                t_ = settings.bossBuddy.tongueAttackDuration;
                tongueDead_ = false;
                break;
            }
            break;
        }
        case StateTongueAttack: {
            if (tongueRoped()) {
                tongueWeldJoint_ = scene()->addWeldJoint(head_, tongueJoint_->objectB()->shared_from_this(),
                    tongueJoint_->objectB()->pos(), false);
                tongueJoint_->remove();
                tongueJoint_.reset();

                idleRotT_ = 1.0f;
                idleRot_ = b2Rot(0.0f);

                state_ = StateCaptured;
                t_ = 0.0f;
                sndTongueCaptured_->play();
                break;
            }
            if (!tongueDead_ && tongue_->dead()) {
                tongueDead_ = true;
                t_ = settings.bossBuddy.tongueIdleDuration;
            }
            if (t_ <= 0.0f) {
                tongueStartRetract();
                break;
            }
            break;
        }
        case StateTongueRetract: {
            if (t_ <= 0.0f) {
                tongue_->removeFromParent();
                tongue_.reset();
                if (brain_->dead()) {
                    state_ = StateSwallowAim;
                    t_ = 0.0f;
                } else {
                    closeJaw();
                    state_ = StateTongueCloseJaws;
                    t_ = 1.0f;
                }
                break;
            }
            break;
        }
        case StateTongueCloseJaws: {
            if (t_ <= 0.0f) {
                startWalk();
                break;
            }
            break;
        }
        case StateCaptured: {
            if (!tongueRoped()) {
                tongueStopCaptured();
                break;
            }

            bool allRoped = true;

            for (int i = 0; i < 2; ++i) {
                if (!rope_[i] || !rope_[i]->scene() || !rope_[i]->findComponent<PhysicsRopeComponent>()) {
                    allRoped = false;
                    break;
                }
            }

            if (allRoped) {
                state_ = StatePreOpenSkull;
                t_ = 1.0f;
                break;
            }

            break;
        }
        case StatePreOpenSkull: {
            if (t_ <= 0.0f) {
                skullJoints_[0]->setLimits(-deg2rad(34.0f), 0.0f);
                skullJoints_[1]->setLimits(0.0f, deg2rad(34.0f));
                t_ = 1.0f;
                state_ = StateOpenSkull;
                sndFrontgunExtend_->play();
                break;
            }
            break;
        }
        case StateOpenSkull: {
            if (t_ <= 0.0f) {
                brainJoint_->setMotorSpeed(-brainJoint_->motorSpeed());
                t_ = 1.0f;
                state_ = StateBrainExtend;
                sndFrontgunExtend_->play();
                break;
            }
            break;
        }
        case StateBrainExtend: {
            if (t_ <= 0.0f) {
                brain_->setInvulnerable(false);

                RenderHealthbarComponentPtr component =
                    boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.5f), 0, 12.0f, 1.8f, zOrderMarker + 1);
                component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
                brain_->addComponent(component);
                state_ = StateBrainExposed;
                break;
            }
            break;
        }
        case StateBrainExposed: {
            if (brainDead()) {
                for (int i = 0; i < 3; ++i) {
                    sides_[i]->setDeadFinal();
                }
            }
            if (!tongueRoped()) {
                brain_->setInvulnerable(true);
                RenderHealthbarComponentPtr hc =
                    brain_->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }
                brainJoint_->setMotorSpeed(-brainJoint_->motorSpeed());

                state_ = StateBrainRetract;
                t_ = 1.0f;
                sndFrontgunRetract_->play();
                break;
            }
            break;
        }
        case StateBrainRetract: {
            if (t_ <= 0.0f) {
                skullJoints_[0]->setMotorSpeed(-skullJoints_[0]->motorSpeed());
                skullJoints_[1]->setMotorSpeed(-skullJoints_[1]->motorSpeed());
                t_ = 1.0f;
                state_ = StateCloseSkull;
                sndFrontgunRetract_->play();
                break;
            }
            break;
        }
        case StateCloseSkull: {
            if (t_ <= 0.0f) {
                skullJoints_[0]->setLimits(0.0f, 0.0f);
                skullJoints_[1]->setLimits(0.0f, 0.0f);
                skullJoints_[0]->setMotorSpeed(-skullJoints_[0]->motorSpeed());
                skullJoints_[1]->setMotorSpeed(-skullJoints_[1]->motorSpeed());
                tongueStopCaptured();
                break;
            }
            break;
        }
        case StateSideGunExtend: {
            if (t_ <= 0.0f) {
                if ((rand() % 2) == 0) {
                    for (int i = 0; i < 2; ++i) {
                        sideWeapon_[i]->trigger(true);
                    }
                    t_ = settings.bossBuddy.plasmaDuration;
                    state_ = StateSideGun;
                } else {
                    t_ = 0.0f;
                    state_ = StateSideAltGun;
                    numMissiles_ = 2 + (rand() % 3);
                }
                break;
            }
            break;
        }
        case StateSideGun: {
            if (t_ <= 0.0f) {
                for (int i = 0; i < 2; ++i) {
                    sideWeapon_[i]->trigger(false);
                }
                t_ = 0.5f;
                state_ = StateSideGunPreRetract;
                break;
            }
            break;
        }
        case StateSideAltGun: {
            if (t_ <= 0.0f) {
                if (numMissiles_ <= 0) {
                    t_ = 0.0f;
                    state_ = StateSideGunPreRetract;
                } else {
                    sideAltWeapon_[rand() % 2]->triggerOnce();
                    t_ = getRandom(settings.bossBuddy.missileInterval, settings.bossBuddy.missileInterval * 1.5f);
                    --numMissiles_;
                }
                break;
            }
            break;
        }
        case StateSideGunPreRetract: {
            if (t_ <= 0.0f) {
                sideGunJoints_[0]->setMotorSpeed(-sideGunJoints_[0]->motorSpeed());
                sideGunJoints_[1]->setMotorSpeed(-sideGunJoints_[1]->motorSpeed());
                t_ = 0.5f;
                state_ = StateSideGunRetract;
                sndSidegunRetract_->play();
                break;
            }
            break;
        }
        case StateSideGunRetract: {
            if (t_ <= 0.0f) {
                startWalk();
                break;
            }
            break;
        }
        case StateFrontGunOpenJaws: {
            if (t_ <= 0.0f) {
                frontGunJoint_->setMotorSpeed(-frontGunJoint_->motorSpeed());
                state_ = StateFrontGunExtend;
                t_ = 1.0f;
                sndFrontgunExtend_->play();
                break;
            }
            break;
        }
        case StateFrontGunExtend: {
            if (t_ <= 0.0f) {
                state_ = StateFrontGun;
                t_ = 1.0f;
                numMissiles_ = settings.bossBuddy.seekerNum;
                break;
            }
            break;
        }
        case StateFrontGun: {
            if (t_ <= 0.0f) {
                if (numMissiles_ <= 0) {
                    frontGunJoint_->setMotorSpeed(-frontGunJoint_->motorSpeed());
                    state_ = StateFrontGunRetract;
                    t_ = 1.0f;
                    sndFrontgunRetract_->play();
                } else {
                    frontWeapon_->setTarget(target());
                    frontWeapon_->triggerOnce();
                    t_ = settings.bossBuddy.seekerInterval;
                    --numMissiles_;
                }
                break;
            }
            break;
        }
        case StateFrontGunRetract: {
            if (t_ <= 0.0f) {
                closeJaw();
                state_ = StateFrontGunCloseJaws;
                t_ = 1.0f;
                break;
            }
            break;
        }
        case StateFrontGunCloseJaws: {
            if (t_ <= 0.0f) {
                startWalk();
                break;
            }
            break;
        }
        case StateNapalmOpenJaws: {
            if (t_ <= 0.0f) {
                state_ = StateNapalmLeanBack;
                t_ = 0.3f;
                break;
            }
            break;
        }
        case StateNapalmLeanBack: {
            if (t_ <= 0.0f) {
                state_ = StateNapalmStrike;
                t_ = 0.5f;
                break;
            }
            head_->applyForceToCenter(head_->getDirection(-5000.0f), true);
            break;
        }
        case StateNapalmStrike: {
            if (t_ <= 0.0f) {
                SceneObjectPtr napalm = sceneObjectFactory.createNapalm(settings.bossBuddy.napalmDamage, true);

                napalm->setTransform(b2Mul(head_->getTransform(), napalmXf_));
                napalm->setLinearVelocity(napalm->getDirection(60.0f));
                scene()->addObject(napalm);

                idleRotT_ = 1.0f;
                idleRot_ = b2Rot(0.0f);
                state_ = StateNapalmPost;
                t_ = 0.4f;
                break;
            }
            head_->applyForceToCenter(head_->getDirection(2500.0f), true);
            break;
        }
        case StateNapalmPost: {
            if (t_ <= 0.0f) {
                closeJaw();
                state_ = StateNapalmCloseJaws;
                t_ = 1.0f;
                break;
            }
            break;
        }
        case StateNapalmCloseJaws: {
            if (t_ <= 0.0f) {
                startWalk();
                break;
            }
            break;
        }
        case StateSwallowAim: {
            if (canSwallow_ && (fabs(angleBetween(target()->pos() - head_->pos(), head_->getDirection(1.0f))) <= deg2rad(10.0f))) {
                state_ = StateSwallowLeanBack;
                t_ = 0.3f;
                break;
            }
            break;
        }
        case StateSwallowLeanBack: {
            if (t_ <= 0.0f) {
                head_->findComponent<PhysicsBodyComponent>()->setSensor(true);
                frontGun_->findComponent<PhysicsBodyComponent>()->setSensor(true);
                brain_->findComponent<PhysicsBodyComponent>()->setSensor(true);
                for (int i = 0; i < 2; ++i) {
                    jaws_[i]->findComponent<PhysicsBodyComponent>()->setSensor(true);
                    skull_[i]->findComponent<PhysicsBodyComponent>()->setSensor(true);
                }
                state_ = StateSwallowStrike;
                t_ = 0.5f;
                break;
            }
            head_->applyForceToCenter(head_->getDirection(-5000.0f), true);
            break;
        }
        case StateSwallowStrike: {
            if (b2DistanceSquared(target()->pos(), b2Mul(head_->getTransform(), tongueXf_).p) <= (2.5f * 2.5f)) {
                scene()->addWeldJoint(dummy_, head_, b2Mul(head_->getTransform(), tongueXf_).p);
                for (int i = 0; i < 3; ++i) {
                    sides_[i]->parent()->setLinearVelocity(b2Vec2_zero);
                    sides_[i]->parent()->setAngularVelocity(0.0f);
                    sides_[i]->parent()->body()->SetType(b2_kinematicBody);
                }
                state_ = StateDead;
                jawJoints_[0]->setMotorSpeed(-jawJoints_[0]->motorSpeed() * 2.0f);
                jawJoints_[1]->setMotorSpeed(-jawJoints_[1]->motorSpeed() * 2.0f);
                t_ = 2.0f;
                sndJawClose_->play();
                break;
            }
            b2Vec2 tmp = target()->pos() - head_->pos();
            tmp.Normalize();
            head_->applyForceToCenter(2500.0f * tmp, true);
            break;
        }
        case StateDead: {
            if (t_ <= 0.0f) {
                for (int i = 0; i < 2; ++i) {
                    jaws_[i]->removeFromParent();
                    pipes_[i]->removeFromParent();
                }
                head_->findComponent<LightComponent>()->removeFromParent();
                brain_->removeFromParent();
                frontGun_->removeFromParent();
                state_ = StateDead2;
                scene()->stats()->incEnemiesKilled();
                break;
            }
            break;
        }
        case StateDead2: {
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void BossBuddyComponent::sensorEnter(const SceneObjectPtr& other)
    {
        PhysicsRopeComponentPtr rc = other->findComponent<PhysicsRopeComponent>();

        if (!rc) {
            return;
        }

        if (!tongueRoped()) {
            return;
        }

        float minDist = b2_maxFloat;
        int idx = 0;

        for (int i = 0; i < 2; ++i) {
            float dist = b2DistanceSquared(other->pos(), b2Mul(pipes_[i]->getTransform(), eyeletXf_[i].p));
            if (dist < minDist) {
                minDist = dist;
                idx = i;
            }
        }

        if (rope_[idx] && rope_[idx]->scene() && rope_[idx]->findComponent<PhysicsRopeComponent>()) {
            return;
        }

        other->setTransform(b2Mul(pipes_[idx]->getTransform(), eyeletXf_[idx]));
        scene()->addWeldJoint(other, pipes_[idx], b2Mul(pipes_[idx]->getTransform(), eyeletXf_[idx].p), false);

        rope_[idx] = other;

        rc->setHit();

        sndTongueCaptured_->play();
    }

    void BossBuddyComponent::sensorExit(const SceneObjectPtr& other)
    {
    }

    void BossBuddyComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        dummy_ = sceneObjectFactory.createDummy();
        scene()->addObject(dummy_);

        PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

        jawJoints_[0] = jc->joint<RevoluteJointProxy>("jaw0_joint");
        jawJoints_[1] = jc->joint<RevoluteJointProxy>("jaw1_joint");
        skullJoints_[0] = jc->joint<RevoluteJointProxy>("skull0_joint");
        skullJoints_[1] = jc->joint<RevoluteJointProxy>("skull1_joint");
        brainJoint_ = jc->joint<PrismaticJointProxy>("brain_joint");
        sideGunJoints_[0] = jc->joint<PrismaticJointProxy>("gun1_joint");
        sideGunJoints_[1] = jc->joint<PrismaticJointProxy>("gun2_joint");
        frontGunJoint_ = jc->joint<PrismaticJointProxy>("gun0_joint");

        origPos_ = parent()->pos();

        brain_->setInvulnerable(true);

        for (int i = 0; i < 3; ++i) {
            sides_[i]->parent()->body()->SetType(b2_kinematicBody);
        }

        prevHealth_ = brain_->life();
    }

    void BossBuddyComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (int i = 0; i < 3; ++i) {
            sides_[i]->setTarget(target());
        }
        head_.reset();
        headS_.reset();
        tongue_.reset();
        for (int i = 0; i < 2; ++i) {
            rope_[i].reset();
            pipes_[i].reset();
            jaws_[i].reset();
            skull_[i].reset();
        }
        brain_.reset();
        root3_.reset();
        root3S_.reset();
        frontGun_.reset();
        dummy_->removeFromParent();
        dummy_.reset();
    }

    bool BossBuddyComponent::tongueRoped() const
    {
        if (!tongue_) {
            return false;
        }
        BuddyTongueComponentPtr c = tongue_->findComponent<BuddyTongueComponent>();
        if (!c->rope()) {
            return false;
        }
        return !!c->rope()->findComponent<PhysicsRopeComponent>();
    }

    void BossBuddyComponent::tongueStartRetract()
    {
        RenderHealthbarComponentPtr hc =
            tongue_->findComponent<RenderHealthbarComponent>();
        if (hc) {
            hc->removeFromParent();
        }
        tongue_->findComponent<LightComponent>()->lights()[0]->setVisible(false);
        tongue_->setInvulnerable(true);

        tongueJoint_->setMotorSpeed(-tongueJoint_->motorSpeed());
        state_ = StateTongueRetract;
        t_ = 1.0f;
    }

    void BossBuddyComponent::tongueStopCaptured()
    {
        tongueJoint_ = scene()->addPrismaticJoint(headS_, tongueWeldJoint_->objectB()->shared_from_this(),
            headS_->getLocalPoint(tongueWeldJoint_->objectB()->pos()), b2Vec2_zero, -tongueJointLen_,
            0.0f, b2Vec2(1.0f, 0.0f), true);
        tongueJoint_->setMaxMotorForce(2000.0f);
        tongueJoint_->setMotorSpeed(30.0f);
        tongueJoint_->enableMotor(true);
        tongueWeldJoint_->remove();
        tongueWeldJoint_.reset();

        idleRotT_ = 1.0f;
        idleRot_ = b2Rot(0.0f);

        tongueStartRetract();
    }

    void BossBuddyComponent::startWalk()
    {
        state_ = StateWalk;
        t_ = getRandom(settings.bossBuddy.attackMinTimeout, settings.bossBuddy.attackMaxTimeout);
    }

    void BossBuddyComponent::setDead()
    {
        state_ = StateDead;
        t_ = 0.0f;
        for (int i = 0; i < 3; ++i) {
            sides_[i]->setDeadFinal();
        }
    }

    void BossBuddyComponent::openJaw()
    {
        sndJawOpen_->play();
        jawJoints_[0]->setMotorSpeed(-jawJoints_[0]->motorSpeed());
        jawJoints_[1]->setMotorSpeed(-jawJoints_[1]->motorSpeed());
    }

    void BossBuddyComponent::closeJaw()
    {
        sndJawClose_->play();
        jawJoints_[0]->setMotorSpeed(-jawJoints_[0]->motorSpeed());
        jawJoints_[1]->setMotorSpeed(-jawJoints_[1]->motorSpeed());
    }
}
