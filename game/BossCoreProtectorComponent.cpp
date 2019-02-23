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

#include "BossCoreProtectorComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "PhysicsBodyComponent.h"
#include "RenderHealthbarComponent.h"
#include "FadeOutComponent.h"
#include "CollisionCancelComponent.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    static const float dieCalmTime = 3.0f;

    BossCoreProtectorComponent::BossCoreProtectorComponent(const SceneObjectPtr& head,
        const SceneObjectPtr arm[2],
        const b2Vec2 armTip[2],
        const b2Vec2 armElbow[2],
        const b2Vec2 armInitPos[2],
        const b2Vec2 armAimPos[2],
        const b2Vec2 armWeaponPos[2],
        const b2Vec2 armAngryPos[2][2],
        const b2Vec2 armDiePos[2][2])
    : TargetableComponent(phaseThink),
      head_(head),
      state_(StateInit),
      t_(0.0f),
      startPos_(b2Vec2_zero),
      arenaPos_(b2Vec2_zero),
      arenaRadius_(0.0f),
      dir_(0),
      first_(false),
      count_(0),
      started_(false),
      die_(false),
      sndWhoosh_(audio.createSound("coreprotector_whoosh.ogg")),
      sndMove_(audio.createSound("coreprotector_move.ogg")),
      sndAngry_(audio.createSound("coreprotector_angry.ogg")),
      sndAngryHit_(audio.createSound("metal_hit1.ogg")),
      sndDie_(audio.createSound("coreprotector_die.ogg")),
      sndDieCalmDown_(audio.createSound("coreprotector_die_calmdown.ogg"))
    {
        sndMove_->setLoop(true);
        sndDie_->setLoop(true);
        sndAttack_[0] = audio.createSound("coreprotector_attack1.ogg");
        sndAttack_[1] = audio.createSound("coreprotector_attack2.ogg");
        sndAttack_[2] = audio.createSound("coreprotector_attack3.ogg");
        sndAttack_[3] = audio.createSound("coreprotector_attack4.ogg");
        for (int i = 0; i < 2; ++i) {
            arm_[i] = arm[i];
            armTip_[i] = armTip[i];
            armElbow_[i] = armElbow[i];
            armInitPos_[i] = armInitPos[i];
            armAimPos_[i] = armAimPos[i];
            armWeaponPos_[i] = armWeaponPos[i];
            armAngryPos_[0][i] = armAngryPos[0][i];
            armAngryPos_[1][i] = armAngryPos[1][i];
            armDiePos_[0][i] = armDiePos[0][i];
            armDiePos_[1][i] = armDiePos[1][i];
        }
    }

    BossCoreProtectorComponent::~BossCoreProtectorComponent()
    {
    }

    void BossCoreProtectorComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossCoreProtectorComponent::update(float dt)
    {
        if (!parent()->active()) {
            return;
        }

        b2Vec2 tmp = parent()->pos() - arenaPos_;
        tmp.Normalize();
        tmp = (moveObj_->pos() - arenaPos_).Length() * tmp + arenaPos_;

        dummyObj_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (tmp - dummyObj_->pos()));
        if (arenaRadius_ != 0.0f) {
            dummyObj_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(vec2angle(arenaPos_ - parent()->pos()) - dummyObj_->angle()));
        }

        bool armsMoved = true;

        for (int i = 0; i < 2; ++i) {
            armsMoved &= updateArm(i, dt);
        }

        if (!started_) {
            if (sndMove_->status() != AudioSource::Playing) {
                sndMove_->play();
            }

            b2Vec2 curDir = startPos_ - moveObj_->pos();
            float len = curDir.Normalize();

            float vel = dt * 20.0f;

            if (vel > len) {
                vel = len;
            }

            moveObj_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * vel * curDir);

            if (fabs(vel) <= b2_epsilon) {
                started_ = true;
                sndMove_->stop();
            }

            return;
        }

        updateAutoTarget(dt);

        float ang2 = 0.0f;

        bool stopMoveSound = true;

        if ((state_ == StateDie1) || (state_ == StateDie2)) {
            float a = t_ / dieCalmTime;
            float tq = 400000.0f * (1.0f - a) + 4000.0f * a;

            head_->applyTorque(tq * -dir_, true);

            b2Vec2 curDir = moveObj_->pos() - arenaPos_;
            b2Vec2 targetDir = diePos_ - arenaPos_;

            ang2 = fabs(angleBetween(curDir, targetDir));

            float ang = dt * settings.bossCoreProtector.moveSpeed / arenaRadius_;
            createNormalMoveJoint();

            if (ang > ang2) {
                ang = ang2;
            }

            ang *= 1.0f * dir_;

            curDir.Normalize();
            curDir *= arenaRadius_;

            if (t_ == 0.0f) {
                moveObj_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (arenaPos_ + b2Mul(b2Rot(ang), curDir) - moveObj_->pos()));
            }

            if (fabs(ang) <= b2_epsilon) {
                if (t_ == 0.0f) {
                    moveObj_->setLinearVelocity(b2Vec2_zero);
                    FadeOutComponentPtr c = boost::make_shared<FadeOutComponent>(dieCalmTime);
                    c->setLightsOnly(true);
                    head_->addComponent(c);
                    sndDie_->stop();
                    sndDieCalmDown_->play();
                }
                t_ += dt;
                if (t_ >= dieCalmTime) {
                    CollisionCancelComponentPtr c = parent()->findComponent<CollisionCancelComponent>();
                    if (c) {
                        c->removeFromParent();
                    }
                    for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                         it != parent()->objects().end(); ++it) {
                        CollisionCancelComponentPtr c = (*it)->findComponent<CollisionCancelComponent>();
                        if (c) {
                            c->removeFromParent();
                        }
                    }
                    removeFromParent();
                    return;
                }
            }
        } else if (target()) {
            {
                b2Vec2 targetDir = target()->pos() - head_->pos();

                b2Vec2 dir = angle2vec(head_->angle(), 1.0f);

                if (fabs(angleBetween(targetDir, dir)) > deg2rad(1.0f)) {
                    if (b2Cross(dir, targetDir) >= 0.0f) {
                        head_->applyTorque(40000.0f, true);
                    } else {
                        head_->applyTorque(-40000.0f, true);
                    }
                }
            }

            b2Vec2 curDir = moveObj_->pos() - arenaPos_;
            b2Vec2 targetDir = target()->pos() - arenaPos_;

            ang2 = fabs(angleBetween(curDir, targetDir));

            float ang;

            if ((state_ == StateAttack3Strike) && (dir_ != 0)) {
                ang = (dt * settings.bossCoreProtector.shootMoveSpeed / arenaRadius_) * dir_;
                createNormalMoveJoint();
            } else {
                float dr = targetDir.Length();

                if (arenaRadius_ <= settings.bossCoreProtector.closeDistance + dr) {
                    ang = dt * settings.bossCoreProtector.fastMoveSpeed / arenaRadius_;
                    createFastMoveJoint();
                    if (ang2 > deg2rad(15)) {
                        if (sndMove_->status() != AudioSource::Playing) {
                            sndMove_->play();
                        }
                        stopMoveSound = false;
                    }
                } else {
                    ang = dt * settings.bossCoreProtector.moveSpeed / arenaRadius_;
                    createNormalMoveJoint();
                }

                if (ang > ang2) {
                    ang = ang2;
                }

                if (b2Cross(curDir, targetDir) < 0.0f) {
                    ang = -ang;
                }
            }

            curDir.Normalize();
            curDir *= arenaRadius_;

            moveObj_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (arenaPos_ + b2Mul(b2Rot(ang), curDir) - moveObj_->pos()));
        }

        if (stopMoveSound) {
            sndMove_->stop();
        }

        switch (state_) {
        case StateInit: {
            if (checkDie()) {
                break;
            }

            if (!target()) {
                break;
            }

            if (checkClose()) {
                break;
            }

            if (normalMoveJoint_ && armsMoved) {
                t_ += dt;
            } else {
                t_ = 0.0f;
            }

            if (t_ > settings.bossCoreProtector.shootTimeout) {
                t_ = 0.0f;

                if ((rand() % 7) == 6) {
                    startAngry();
                } else {
                    attackSound();
                    moveArm(0, dummyObj_->getWorldPoint(armWeaponPos_[0]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, true);
                    moveArm(1, dummyObj_->getWorldPoint(armWeaponPos_[1]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, true);
                    state_ = StateAttack3Aim;
                }
                break;
            }

            break;
        }
        case StateAttack1Aim: {
            if (!armsMoved) {
                break;
            }

            if (checkDie()) {
                break;
            }

            if (checkFar()) {
                break;
            }

            if (!first_) {
                int r = rand() % 9;
                if ((r >= 0) && (r <= 1)) {
                    startAttack2();
                    break;
                } else if (r == 2) {
                    startAngry();
                    break;
                }
            }

            first_ = false;

            moveArm(0, target()->pos(), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true, true);
            moveArm(1, dummyObj_->getWorldPoint(armAimPos_[1]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, false);
            state_ = StateAttack1Strike;

            break;
        }
        case StateAttack1Strike: {
            if (!armsMoved) {
                break;
            }

            if (checkDie()) {
                break;
            }

            if (checkFar()) {
                break;
            }

            moveArm(0, dummyObj_->getWorldPoint(armAimPos_[0]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, false);
            moveArm(1, target()->pos(), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true, true);
            state_ = StateAttack1Aim;

            break;
        }
        case StateAttack2Aim: {
            if (!armsMoved) {
                break;
            }

            if (checkDie()) {
                break;
            }

            if (checkFar()) {
                break;
            }

            moveArm(0, target()->pos(), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true, true);
            moveArm(1, target()->pos(), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true);
            state_ = StateAttack2Strike;

            break;
        }
        case StateAttack2Strike: {
            if (!armsMoved) {
                break;
            }

            attackFinished();

            break;
        }
        case StateAttack3Aim: {
            if (!armsMoved) {
                break;
            }

            if (checkDie()) {
                break;
            }

            weapon_->trigger(true);
            state_ = StateAttack3Strike;
            if (ang2 < settings.bossCoreProtector.shootMoveAngle) {
                dir_ = ((rand() % 2) == 0) ? 1 : -1;
            } else {
                dir_ = 0;
            }

            break;
        }
        case StateAttack3Strike: {
            t_ += dt;
            if ((t_ < settings.bossCoreProtector.shootMaxDuration) &&
                !(fastMoveJoint_ && (t_ > settings.bossCoreProtector.shootMinDuration))) {
                break;
            }

            t_ = 0.0f;
            weapon_->trigger(false);

            attackFinished();

            break;
        }
        case StateAngry1: {
            if (!armsMoved) {
                break;
            }

            if (!first_) {
                scene()->camera()->findComponent<CameraComponent>()->roll(settings.bossCoreProtector.meleeRollAngle);
                sndAngryHit_->play();
            }

            first_ = false;

            moveArm(0, dummyObj_->getWorldPoint(armAngryPos_[0][0]), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true);
            moveArm(1, dummyObj_->getWorldPoint(armAngryPos_[1][1]), settings.bossCoreProtector.strikeVelocity, EaseOutQuad, true);

            state_ = StateAngry2;

            break;
        }
        case StateAngry2: {
            if (!armsMoved) {
                break;
            }

            scene()->camera()->findComponent<CameraComponent>()->roll(settings.bossCoreProtector.meleeRollAngle);
            sndAngryHit_->play();

            if (--count_ == 0) {
                attackFinished();
            } else {
                moveArm(0, dummyObj_->getWorldPoint(armAngryPos_[1][0]), settings.bossCoreProtector.strikeVelocity, EaseOutQuad, true);
                moveArm(1, dummyObj_->getWorldPoint(armAngryPos_[0][1]), settings.bossCoreProtector.strikeVelocity, EaseInQuad, true);

                state_ = StateAngry1;
            }

            break;
        }
        case StateDie1: {
            if (!armsMoved) {
                break;
            }

            float a = t_ / dieCalmTime;
            float vel = settings.bossCoreProtector.strikeVelocity * (1.0f - a) + 5.0f * a;

            moveArm(0, dummyObj_->getWorldPoint(armDiePos_[1][0]), vel, EaseInQuad, true);
            moveArm(1, dummyObj_->getWorldPoint(armDiePos_[1][1]), vel, EaseOutQuad, true);

            state_ = StateDie2;

            break;
        }
        case StateDie2: {
            if (!armsMoved) {
                break;
            }

            float a = t_ / dieCalmTime;
            float vel = settings.bossCoreProtector.strikeVelocity * (1.0f - a) + 5.0f * a;

            moveArm(0, dummyObj_->getWorldPoint(armDiePos_[0][0]), vel, EaseInQuad, true);
            moveArm(1, dummyObj_->getWorldPoint(armDiePos_[0][1]), vel, EaseOutQuad, true);

            state_ = StateDie1;

            break;
        }
        default:
            assert(0);
            break;
        }
    }

    void BossCoreProtectorComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        dummyObj_ = sceneObjectFactory.createDummy();
        dummyObj_->setTransform(parent()->getTransform());
        scene()->addObject(dummyObj_);

        moveObj_ = sceneObjectFactory.createDummy();
        moveObj_->setTransform(parent()->getTransform());
        scene()->addObject(moveObj_);

        createNormalMoveJoint();

        moveArm(0, dummyObj_->getWorldPoint(armInitPos_[0]), 13.0f, EaseLinear, true);
        moveArm(1, dummyObj_->getWorldPoint(armInitPos_[1]), 13.0f, EaseLinear, true);
    }

    void BossCoreProtectorComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (int i = 0; i < 2; ++i) {
            arm_[i].reset();
        }

        dummyObj_->removeFromParent();
        dummyObj_.reset();

        moveObj_->removeFromParent();
        moveObj_.reset();
    }

    bool BossCoreProtectorComponent::updateArm(int index, float dt)
    {
        if (!armJoint_[index] || !armTweening_[index]) {
            return true;
        }

        armTweenTime_[index] += dt;

        float len = armTweening_[index]->getValue(armTweenTime_[index]);

        if (armHaveSound_[index] && (armTweenTime_[index] >= armTweening_[index]->duration() * 2.0f / 3.0f)) {
            armHaveSound_[index] = false;
            sndWhoosh_->play();
        }

        if (len <= 0.1f) {
            armJoint_[index]->setLength(0.1f);
        } else {
            armJoint_[index]->setLength(len);
        }

        return ((armJoint_[index]->getAnchorA() - armJoint_[index]->getAnchorB()).Length() <= 1.0f) ||
            (armTweenTime_[index] >= (armTweening_[index]->duration() + 0.1f));
    }

    void BossCoreProtectorComponent::moveArm(int index, const b2Vec2& pos, float velocity, Easing easing, bool tip, bool haveSound)
    {
        if (armJoint_[index]) {
            armJoint_[index]->remove();
        }

        float len = (arm_[index]->getWorldPoint(tip ? armTip_[index] : armElbow_[index]) - pos).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        armJoint_[index] = scene()->addDistanceJoint(dummyObj_, arm_[index],
            dummyObj_->getLocalPoint(pos), tip ? armTip_[index] : armElbow_[index], len,
            10.0f, 1.0f, false);

        armTweening_[index] = boost::make_shared<SingleTweening>(len / velocity, easing, len, 0.0f);
        armTweenTime_[index] = 0.0f;
        armHaveSound_[index] = haveSound;
    }

    bool BossCoreProtectorComponent::checkClose()
    {
        if (!target()) {
            return false;
        }

        if (b2DistanceSquared(dummyObj_->pos(), target()->pos()) <= settings.bossCoreProtector.closeDistance * settings.bossCoreProtector.closeDistance) {
            int r;

            if (state_ == StateInit) {
                r = rand() % 7;
            } else {
                r = rand() % 6;
            }

            if (r == 6) {
                startAngry();
            } else if ((r >= 0) && (r <= 1)) {
                startAttack2();
            } else {
                attackSound();
                moveArm(0, dummyObj_->getWorldPoint(armAimPos_[0]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, false);
                state_ = StateAttack1Aim;
                first_ = true;
            }

            return true;
        }

        return false;
    }

    bool BossCoreProtectorComponent::checkFar()
    {
        if (!target()) {
            return true;
        }

        if (b2DistanceSquared(dummyObj_->pos(), target()->pos()) > settings.bossCoreProtector.farDistance * settings.bossCoreProtector.farDistance) {
            moveToInit();
            return true;
        }

        return false;
    }

    void BossCoreProtectorComponent::moveToInit()
    {
        moveArm(0, dummyObj_->getWorldPoint(armInitPos_[0]), settings.bossCoreProtector.initVelocity, EaseLinear, true);
        moveArm(1, dummyObj_->getWorldPoint(armInitPos_[1]), settings.bossCoreProtector.initVelocity, EaseLinear, true);
        state_ = StateInit;
    }

    void BossCoreProtectorComponent::attackFinished()
    {
        if (checkDie()) {
            return;
        }

        if (!checkClose()) {
            moveToInit();
        }
    }

    void BossCoreProtectorComponent::createNormalMoveJoint()
    {
        if (!normalMoveJoint_) {
            if (fastMoveJoint_) {
                fastMoveJoint_->remove();
                fastMoveJoint_.reset();
            }
            normalMoveJoint_ = scene()->addDistanceJoint(moveObj_, parent()->shared_from_this(),
                b2Vec2_zero, b2Vec2_zero, 0.0f,
                2.0f, 0.0f, false);
        }
    }

    void BossCoreProtectorComponent::createFastMoveJoint()
    {
        if (!fastMoveJoint_) {
            if (normalMoveJoint_) {
                normalMoveJoint_->remove();
                normalMoveJoint_.reset();
            }
            fastMoveJoint_ = scene()->addDistanceJoint(moveObj_, parent()->shared_from_this(),
                b2Vec2_zero, b2Vec2_zero, 0.0f,
                2.5f, 0.0f, false);
        }
    }

    void BossCoreProtectorComponent::startAttack2()
    {
        attackSound();
        moveArm(0, dummyObj_->getWorldPoint(armAimPos_[0]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, false);
        moveArm(1, dummyObj_->getWorldPoint(armAimPos_[1]), settings.bossCoreProtector.aimVelocity, EaseOutQuad, false);
        state_ = StateAttack2Aim;
    }

    void BossCoreProtectorComponent::startAngry()
    {
        sndAngry_->play();
        moveArm(0, dummyObj_->getWorldPoint(armAngryPos_[1][0]), settings.bossCoreProtector.strikeVelocity, EaseOutQuad, true);
        state_ = StateAngry1;
        count_ = 3;
        first_ = true;
    }

    void BossCoreProtectorComponent::attackSound()
    {
        if ((sndAttack_[0]->status() != AudioSource::Playing) &&
            (sndAttack_[1]->status() != AudioSource::Playing) &&
            (sndAttack_[2]->status() != AudioSource::Playing) &&
            (sndAttack_[3]->status() != AudioSource::Playing)){
            sndAttack_[rand() % 4]->play();
        }
    }

    bool BossCoreProtectorComponent::checkDie()
    {
        if (!die_) {
            return false;
        }

        scene()->stats()->incEnemiesKilled();

        state_ = StateDie1;

        b2Vec2 curDir = moveObj_->pos() - arenaPos_;
        b2Vec2 targetDir = diePos_ - arenaPos_;

        dir_ = (b2Cross(curDir, targetDir) >= 0.0f) ? -1 : 1;

        moveArm(0, dummyObj_->getWorldPoint(armDiePos_[0][0]), settings.bossCoreProtector.strikeVelocity, EaseOutQuad, true);
        moveArm(1, dummyObj_->getWorldPoint(armDiePos_[0][1]), settings.bossCoreProtector.strikeVelocity, EaseOutQuad, true);

        t_ = 0.0f;

        sndDie_->play();

        return true;
    }
}
