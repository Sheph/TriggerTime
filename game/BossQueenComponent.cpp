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

#include "BossQueenComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "PhysicsBodyComponent.h"
#include "RenderHealthbarComponent.h"
#include "FadeOutComponent.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    static const float jointFrequency = 10.0f;
    static const float jointDampingRatio = 1.0f;

    BossQueenComponent::BossQueenComponent(const SceneObjectPtr legs[8],
        const b2Vec2 legTips[8],
        const b2Vec2& bodyTip,
        const b2Vec2 cover[4],
        const Points& spawn,
        const b2Vec2 dead[4],
        const AnimationComponentPtr& ac)
    : TargetableComponent(phaseThink),
      bodyTip_(bodyTip),
      spawn_(spawn),
      ac_(ac),
      biteDone_(true),
      biteTime_(0.0f),
      state_(StateAttack4),
      stateTime_(0.0f),
      newState_(-1),
      subState_(SubStateInitial),
      goalTime_(0.0f),
      deathFinished_(false),
      trapped_(false),
      sndAim_(audio.createStream("queen_aim.ogg")),
      sndShake_(audio.createStream("queen_shake.ogg")),
      sndAngry_(audio.createSound("queen_angry.ogg")),
      sndBite_(audio.createSound("queen_bite.ogg")),
      sndSpawn_(audio.createSound("queen_spawn.ogg")),
      sndStrike_(audio.createStream("queen_strike.ogg")),
      sndDie_(audio.createStream("queen_die.ogg"))
    {
        sndShake_->setLoop(true);
        for (int i = 0; i < 4; ++i) {
            cover_[i] = cover[i];
            dead_[i] = dead[i];
        }
        for (int i = 0; i < 8; ++i) {
            sndHit_[i] = audio.createSound("queen_hit.ogg");
            leg_[i] = legs[i];
            legTip_[i] = legTips[i];
            legState_[i] = LegStateDown;
        }
    }

    BossQueenComponent::~BossQueenComponent()
    {
    }

    void BossQueenComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossQueenComponent::update(float dt)
    {
        updateAutoTarget(dt);

        if (goal_) {
            goalTime_ += dt;

            if (goalTime_ >= 5.0f) {
                goal_->removeFromParent();
                goal_.reset();
            }
        }

        if (target()) {
            if ((state_ != StateAttack4) && (state_ != StateDie)) {
                b2Vec2 targetDir = target()->pos() - parent()->pos();

                b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

                if (fabs(angleBetween(targetDir, dir)) > deg2rad(10.0f)) {
                    if (b2Cross(dir, targetDir) >= 0.0f) {
                        parent()->applyTorque(2000000.0f, true);
                    } else {
                        parent()->applyTorque(-2000000.0f, true);
                    }
                }
            }

            stateTime_ += dt;
        }

        biteTime_ -= dt;

        if ((biteTime_ <= 0.0f) && target() && (state_ != StateDie)) {
            b2Vec2 targetDir = target()->pos() - parent()->getWorldPoint(bodyTip_);

            if (ac_->currentAnimation() == AnimationAttack) {
                if (ac_->animationFinished()) {
                    biteTime_ = 0.5f;
                    ac_->startAnimation(AnimationDefault);
                } else if (!biteDone_ && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                    biteDone_ = true;
                    sndBite_->play();
                    if (targetDir.Length() <= 2.5f) {
                        target()->changeLife(-20.0f);
                    }
                }
            } else if (targetDir.Length() <= 2.5f) {
                biteDone_ = false;
                ac_->startAnimation(AnimationAttack);
            }
        }

        bool calm = false;
        bool calmDown = false;

        switch (state_) {
        case StateAttack1:
            calmDown = (stateTime_ >= 5.0f);
            if (calmDown && (newState_ < 0)) {
                newState_ = rand() % (StateMax + 1);
                if (newState_ == state_) {
                    stateTime_ = 0.0f;
                    newState_ = -1;
                    calmDown = false;
                }
            }
            calm = updateAttack1(dt, calmDown);
            break;
        case StateAttack2:
            calmDown = (stateTime_ >= 1.0f);
            if (calmDown && (newState_ < 0)) {
                newState_ = rand() % (StateMax + 1);
                if (newState_ == state_) {
                    newState_ = 0;
                }
            }
            calm = updateAttack2(dt, calmDown);
            break;
        case StateAttack3:
            calmDown = (stateTime_ >= 1.0f);
            if (calmDown && (newState_ < 0)) {
                newState_ = rand() % (StateMax + 1);
                if (newState_ == state_) {
                    newState_ = 0;
                }
            }
            calm = updateAttack3(dt, calmDown);
            break;
        case StateAttack4:
            calmDown = (stateTime_ >= 1.0f);
            if (calmDown && (newState_ < 0)) {
                newState_ = rand() % (StateMax + 1);
                if (newState_ == state_) {
                    newState_ = 0;
                }
            }
            calm = updateAttack4(dt, calmDown);
            break;
        case StateDie:
            updateDie(dt);
            break;
        case StateTrapped1:
            if (target()) {
                if (stateTime_ >= 0.5f) {
                    stateTime_ = 0.0f;
                    state_ = StateTrapped2;
                    break;
                }
                parent()->applyForceToCenter(parent()->getDirection(50000.0f), true);
            }
            break;
        case StateTrapped2:
            if (target()) {
                if (stateTime_ >= 0.5f) {
                    stateTime_ = 0.0f;
                    state_ = StateTrapped1;
                    break;
                }
                parent()->applyForceToCenter(parent()->getDirection(-50000.0f), true);
            }
            break;
        default:
            assert(0);
            break;
        }

        if (calmDown && calm) {
            if (parent()->life() <= 0.0f) {
                newState_ = StateDie;
            }

            if (trapped_) {
                state_ = StateTrapped1;
                stateTime_ = 0.0f;
            } else {
                state_ = static_cast<State>(newState_);
                subState_ = SubStateInitial;
                stateTime_ = 0.0f;
                newState_ = -1;

                if ((goalTime_ == 0.0f) && !goal_) {
                    goal_ = sceneObjectFactory.createGoal(5.0f, b2Vec2(0.0f, -6.0f));
                    parent()->addComponent(goal_);
                }
            }
        }
    }

    bool BossQueenComponent::updateAttack1(float dt, bool calmDown)
    {
        bool calm = true;
        int attackLeg = -1;
        float minDist = b2_maxFloat;

        for (int i = 0; i < 4; ++i) {
            bool legMoved = updateLeg(i, dt);

            if (target() && (i < 4)) {
                if ((legDefaultPos_[i] - target()->pos()).LengthSquared() < minDist) {
                    minDist = (legDefaultPos_[i] - target()->pos()).LengthSquared();
                    attackLeg = i;
                }
            }

            switch (legState_[i]) {
            case LegStateDown: {
                calm &= legMoved;
                break;
            }
            case LegStateMeleeAim: {
                calm = false;
                if (legMoved) {
                    b2Vec2 p1 = legDefaultPos_[i] - 25.0f * bodyDefaultDir_;

                    b2Vec2 dir = (target()->pos() - p1);
                    dir.Normalize();

                    b2Vec2 p2 = p1 + 50.0f * dir;
                    bool found = false;

                    scene()->rayCast(p1, p2,
                        boost::bind(&BossQueenComponent::rayCastCb, this, _1, _2, _3, _4,
                            boost::ref(found), boost::ref(p2)));

                    moveLeg(i, p2, settings.bossQueen.attack1StrikeVelocity, EaseInQuad);
                    legState_[i] = LegStateMeleeStrike;
                }
                break;
            }
            case LegStateMeleeStrike: {
                calm = false;
                if (legMoved) {
                    sndHit_[i]->play();
                    scene()->camera()->findComponent<CameraComponent>()->shake(5.0f);
                    moveLeg(i, legDefaultPos_[i], settings.bossQueen.attack1DownVelocity, EaseOutQuad);
                    legState_[i] = LegStateDown;
                }
                break;
            }
            default:
                assert(0);
                break;
            }
        }

        if ((attackLeg >= 0) && !calmDown) {
            if (legState_[attackLeg] != LegStateDown) {
                int otherLeg;
                if (attackLeg == 0) {
                    otherLeg = 2;
                } else if (attackLeg == 2) {
                    otherLeg = 0;
                } else if (attackLeg == 1) {
                    otherLeg = 3;
                } else if (attackLeg == 3) {
                    otherLeg = 1;
                }
                if ((legState_[otherLeg] == LegStateDown) && ((legDefaultPos_[otherLeg] - target()->pos()).Length() < 15.0f)) {
                    attackLeg = otherLeg;
                }
            }

            if (legState_[attackLeg] != LegStateDown) {
                b2Vec2 targetDir = target()->pos() - parent()->getWorldPoint(bodyTip_);

                if (targetDir.Length() <= 8.0f) {
                    attackLeg = 2;
                    if (legState_[attackLeg] != LegStateDown) {
                        attackLeg = 3;
                    }
                }
            }

            if (legState_[attackLeg] == LegStateDown) {
                moveLeg(attackLeg, legDefaultPos_[attackLeg] - 25.0f * bodyDefaultDir_, settings.bossQueen.attack1AimVelocity, EaseOutQuad);
                legState_[attackLeg] = LegStateMeleeAim;
                if (sndAim_->status() != AudioSource::Playing) {
                    sndAim_->play();
                }
            }
        }

        return calm;
    }

    bool BossQueenComponent::updateAttack2(float dt, bool calmDown)
    {
        bool calm = true;
        bool legsMoved = true;
        bool someLegMoved = false;

        for (int i = 0; i < 4; ++i) {
            bool res = updateLeg(i, dt);
            legsMoved &= res;
            if (res) {
                someLegMoved = true;
            }
        }
        bool bodyMoved = updateBody(dt);

        switch (subState_) {
        case SubStateInitial: {
            if (!target()) {
                break;
            }

            calm = legsMoved;

            if (legsMoved && !calmDown) {
                moveLeg(0, legDefaultPos_[0] - 25.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(1, legDefaultPos_[1] - 25.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(2, legDefaultPos_[2] - 25.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(3, legDefaultPos_[3] - 25.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveBody(bodyDefaultPos_ - 3.0f * bodyDefaultDir_, 0.5f, EaseOutQuad);
                subState_ = SubStateAim;
                scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.5f);
                sndShake_->play();
            }
            break;
        }
        case SubStateAim: {
            calm = false;
            if (legsMoved) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                sndShake_->stop();
                sndStrike_->play();

                b2Vec2 dir = bodyDefaultDir_;
                dir = b2Mul(b2Rot(deg2rad(45.0f)), dir);

                b2Vec2 p2 = bodyDefaultPos_ + 50.0f * dir;
                bool found = false;

                scene()->rayCast(bodyDefaultPos_, p2,
                    boost::bind(&BossQueenComponent::rayCastCb, this, _1, _2, _3, _4,
                        boost::ref(found), boost::ref(p2)));

                moveLeg(2, p2, 50.0f, EaseInQuad);

                dir = bodyDefaultDir_;
                dir = b2Mul(b2Rot(-deg2rad(45.0f)), dir);

                p2 = bodyDefaultPos_ + 50.0f * dir;
                found = false;

                scene()->rayCast(bodyDefaultPos_, p2,
                    boost::bind(&BossQueenComponent::rayCastCb, this, _1, _2, _3, _4,
                        boost::ref(found), boost::ref(p2)));

                moveLeg(3, p2, 50.0f, EaseInQuad);

                dir = bodyDefaultDir_;
                dir = b2Mul(b2Rot(-deg2rad(25.0f)), dir);

                p2 = bodyDefaultPos_ + 50.0f * dir;
                found = false;

                scene()->rayCast(bodyDefaultPos_, p2,
                    boost::bind(&BossQueenComponent::rayCastCb, this, _1, _2, _3, _4,
                        boost::ref(found), boost::ref(p2)));

                moveLeg(0, p2, 50.0f, EaseInQuad);

                dir = bodyDefaultDir_;
                dir = b2Mul(b2Rot(deg2rad(25.0f)), dir);

                p2 = bodyDefaultPos_ + 50.0f * dir;
                found = false;

                scene()->rayCast(bodyDefaultPos_, p2,
                    boost::bind(&BossQueenComponent::rayCastCb, this, _1, _2, _3, _4,
                        boost::ref(found), boost::ref(p2)));

                moveLeg(1, p2, 50.0f, EaseInQuad);

                moveBody(bodyDefaultPos_ + 8.0f * bodyDefaultDir_, 15.0f, EaseInQuad);
                subState_ = SubStateMelee;
                shaked_ = false;
            }
            break;
        }
        case SubStateMelee: {
            calm = false;
            if (bodyJoint_) {
                if (bodyMoved) {
                    bodyJoint_->remove();
                    bodyJoint_.reset();
                }
            } else if (legsMoved) {
                moveLeg(0, legDefaultPos_[0], 50.0f, EaseOutQuad);
                moveLeg(1, legDefaultPos_[1], 50.0f, EaseOutQuad);
                moveLeg(2, legDefaultPos_[2], 20.0f, EaseInQuad);
                moveLeg(3, legDefaultPos_[3], 20.0f, EaseInQuad);
                subState_ = SubStateInitial;
            }
            if (someLegMoved && !shaked_) {
                sndHit_[2]->play();
                sndHit_[3]->play();
                scene()->camera()->findComponent<CameraComponent>()->shake(8.0f);
                shaked_ = true;
            }
            break;
        }
        default:
            assert(0);
            break;
        }

        return calm;
    }

    bool BossQueenComponent::updateAttack3(float dt, bool calmDown)
    {
        bool calm = true;
        bool legsMoved = true;

        for (int i = 0; i < 4; ++i) {
            legsMoved &= updateLeg(i, dt);
        }
        bool bodyMoved = updateBody(dt);

        switch (subState_) {
        case SubStateInitial: {
            if (!target()) {
                break;
            }

            calm = legsMoved;

            if (legsMoved && !calmDown) {
                moveLeg(0, cover_[0], 35.0f, EaseInQuad);
                moveLeg(1, cover_[1], 35.0f, EaseInQuad);
                moveLeg(2, cover_[2], 35.0f, EaseInQuad);
                moveLeg(3, cover_[3], 35.0f, EaseInQuad);
                subState_ = SubStateAim;
                sndAim_->play();
            }
            break;
        }
        case SubStateAim: {
            calm = false;
            if (legsMoved) {
                scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.5f);
                sndShake_->play();
                moveLeg(0, cover_[0] - 6.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(1, cover_[1] - 6.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(2, cover_[2] - 6.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveLeg(3, cover_[3] - 6.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                moveBody(bodyDefaultPos_ - 6.0f * bodyDefaultDir_, 5.0f, EaseOutQuad);
                subState_ = SubStateMelee;
                spawned_.clear();
                for (Points::const_iterator it = spawn_.begin();
                    it != spawn_.end(); ++it) {
                    SceneObjectPtr obj;

                    if ((rand() % 2) == 0) {
                        obj = sceneObjectFactory.createBabySpider1();
                    } else {
                        obj = sceneObjectFactory.createBabySpider2();
                    }

                    obj->setPos(*it);
                    obj->setAngle(vec2angle(bodyDefaultDir_));

                    obj->findComponent<TargetableComponent>()->setAutoTarget(true);

                    spawned_.push_back(obj);

                    scene()->addObject(obj);
                }
                shaked_ = false;
            }
            break;
        }
        case SubStateMelee: {
            calm = false;
            if (bodyJoint_) {
                if (bodyMoved) {
                    sndShake_->stop();
                    bodyJoint_->remove();
                    bodyJoint_.reset();
                }
            } else if (legsMoved) {
                if (!shaked_) {
                    scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                    sndSpawn_->play();
                    shaked_ = true;
                }

                bool allDead = true;
                for (std::vector<SceneObjectPtr>::const_iterator it = spawned_.begin();
                    it != spawned_.end(); ++it) {
                    if ((*it)->alive()) {
                        allDead = false;
                        break;
                    }
                }
                if (allDead) {
                    spawned_.clear();
                    moveLeg(0, legDefaultPos_[0], 50.0f, EaseInQuad);
                    moveLeg(1, legDefaultPos_[1], 50.0f, EaseInQuad);
                    moveLeg(2, legDefaultPos_[2], 50.0f, EaseInQuad);
                    moveLeg(3, legDefaultPos_[3], 50.0f, EaseInQuad);
                    subState_ = SubStateInitial;
                }
            }
            break;
        }
        default:
            assert(0);
            break;
        }

        return calm;
    }

    bool BossQueenComponent::updateAttack4(float dt, bool calmDown)
    {
        bool calm = true;

        switch (subState_) {
        case SubStateInitial: {
            if (!target()) {
                break;
            }
            bodyTweening_ = boost::make_shared<SingleTweening>(5.0f,
                VibrateFactor / 6.0f, EaseOutVibrate,
                0.0f, deg2rad(70.0f));
            bodyTweenTime_ = 0.0f;
            subState_ = SubStateAim;
            scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.5f);
            sndShake_->play();
            sndAngry_->play();
            break;
        }
        case SubStateAim: {
            if (!bodyTweening_) {
                break;
            }

            calm = false;

            float value = bodyTweening_->getValue(bodyTweenTime_);

            parent()->applyAngularImpulse(1000000.0f * normalizeAngle(value + vec2angle(bodyDefaultDir_) - parent()->angle()), true);

            bodyTweenTime_ += dt;
            if (bodyTweenTime_ >= 2.0f) {
                bodyTweening_.reset();
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                sndShake_->stop();
            }
            break;
        }
        default:
            assert(0);
            break;
        }

        return calm;
    }

    void BossQueenComponent::updateDie(float dt)
    {
        bool legsMoved = true;

        for (int i = 0; i < 4; ++i) {
            legsMoved &= updateLeg(i, dt);
        }

        switch (subState_) {
        case SubStateInitial: {
            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            FadeOutComponentPtr c = boost::make_shared<FadeOutComponent>(5.0f);
            c->setLightsOnly(true);
            parent()->addComponent(c);

            sndDie_->play();

            bodyTweening_ = boost::make_shared<SingleTweening>(5.0f,
                VibrateFactor / 6.0f, EaseOutVibrate,
                0.0f, deg2rad(70.0f));
            bodyTweenTime_ = 0.0f;

            moveLeg(0, dead_[0], 15.0f, EaseInQuad);
            moveLeg(1, dead_[1], 15.0f, EaseInQuad);
            moveLeg(2, dead_[2], 15.0f, EaseInQuad);
            moveLeg(3, dead_[3], 15.0f, EaseInQuad);

            subState_ = SubStateAim;
            break;
        }
        case SubStateAim: {
            if (!bodyTweening_) {
                if (legsMoved) {
                    scene()->stats()->incEnemiesKilled();

                    subState_ = SubStateMelee;
                    for (std::set<SceneObjectPtr>::iterator it = parent()->objects().begin();
                        it != parent()->objects().end(); ++it) {
                        if (((*it).get() != parent()) && ((*it)->name() != "back")) {
                            (*it)->findComponent<PhysicsBodyComponent>()->removeFromParent();
                        }
                    }
                }

                break;
            }

            float value = bodyTweening_->getValue(bodyTweenTime_);

            parent()->applyAngularImpulse(1000000.0f * (value + vec2angle(bodyDefaultDir_) - parent()->angle()), true);

            bodyTweenTime_ += dt;
            if (bodyTweenTime_ >= 2.0f) {
                bodyTweening_.reset();
            }
            break;
        }
        case SubStateMelee: {
            deathFinished_ = true;
            break;
        }
        default:
            assert(0);
            break;
        }
    }

    void BossQueenComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        dummyObj_ = sceneObjectFactory.createDummy();
        scene()->addObject(dummyObj_);

        for (int i = 0; i < 4; ++i) {
            cover_[i] = parent()->getWorldPoint(cover_[i]);
            dead_[i] = parent()->getWorldPoint(dead_[i]);
        }

        for (int i = 0; i < 8; ++i) {
            legDefaultPos_[i] = leg_[i]->getWorldPoint(legTip_[i]);
            moveLeg(i, legDefaultPos_[i], 0.5f, EaseLinear);
            legState_[i] = LegStateDown;
        }

        bodyDefaultPos_ = parent()->pos();
        bodyDefaultDir_ = parent()->getDirection(1.0f);

        for (Points::iterator it = spawn_.begin();
            it != spawn_.end(); ++it) {
            *it = parent()->getWorldPoint(*it);
        }
    }

    void BossQueenComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (int i = 0; i < 8; ++i) {
            leg_[i].reset();
        }

        dummyObj_->removeFromParent();
        dummyObj_.reset();
    }

    bool BossQueenComponent::updateLeg(int index, float dt)
    {
        if (!legJoint_[index] || !legTweening_[index]) {
            return true;
        }

        legTweenTime_[index] += dt;

        float len = legTweening_[index]->getValue(legTweenTime_[index]);

        if (len <= 0.1f) {
            legJoint_[index]->setLength(0.1f);
        } else {
            legJoint_[index]->setLength(len);
        }

        return ((legJoint_[index]->getAnchorA() - legJoint_[index]->getAnchorB()).Length() <= 1.0f) ||
            (legTweenTime_[index] >= (legTweening_[index]->duration() + 0.3f));
    }

    bool BossQueenComponent::updateBody(float dt)
    {
        if (!bodyJoint_ || !bodyTweening_) {
            return true;
        }

        bodyTweenTime_ += dt;

        float len = bodyTweening_->getValue(bodyTweenTime_);

        if (len <= 0.1f) {
            bodyJoint_->setLength(0.1f);
        } else {
            bodyJoint_->setLength(len);
        }

        return ((bodyJoint_->getAnchorA() - bodyJoint_->getAnchorB()).Length() <= 1.0f) ||
            (bodyTweenTime_ >= (bodyTweening_->duration() + 0.3f));
    }

    float BossQueenComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, bool& found, b2Vec2& finalPoint)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        if ((SceneObject::fromFixture(fixture)->type() != SceneObjectTypeRock) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeTerrain) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeBlocker)) {
            return -1;
        }

        found = true;
        finalPoint = point;

        return fraction;
    }

    void BossQueenComponent::moveLeg(int index, const b2Vec2& pos, float velocity, Easing easing)
    {
        if (legJoint_[index]) {
            legJoint_[index]->remove();
        }

        float len = (leg_[index]->getWorldPoint(legTip_[index]) - pos).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        legJoint_[index] = scene()->addDistanceJoint(dummyObj_, leg_[index],
            dummyObj_->getLocalPoint(pos), legTip_[index], len,
            jointFrequency, jointDampingRatio, false);

        legTweening_[index] = boost::make_shared<SingleTweening>(len / velocity, easing, len, 0.0f);
        legTweenTime_[index] = 0.0f;
    }

    void BossQueenComponent::moveBody(const b2Vec2& pos, float velocity, Easing easing)
    {
        if (bodyJoint_) {
            bodyJoint_->remove();
        }

        float len = (parent()->pos() - pos).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        bodyJoint_ = scene()->addDistanceJoint(dummyObj_, parent()->shared_from_this(),
            dummyObj_->getLocalPoint(pos), b2Vec2_zero, len,
            jointFrequency, jointDampingRatio, false);

        bodyTweening_ = boost::make_shared<SingleTweening>(len / velocity, easing, len, 0.0f);
        bodyTweenTime_ = 0.0f;
    }
}
