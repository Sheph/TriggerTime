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

#include "WalkerComponent.h"
#include "Const.h"
#include "Scene.h"
#include "Utils.h"
#include "Renderer.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "CameraComponent.h"
#include "RenderHealthbarComponent.h"
#include "RenderQuadComponent.h"
#include "SequentialTweening.h"
#include "FadeOutComponent.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    WalkerComponent::WalkerComponent(const SceneObjectPtr& heart,
        const SceneObjectPtr legs[4],
        const b2Vec2 legTips[4])
    : TargetableComponent(phaseThink),
      heart_(heart),
      state_(StateInit),
      legIndex_(0),
      shootT_(0.0f),
      expose_(false),
      t_(0.0f),
      doorIndex_(0),
      healthBefore_(0.0f),
      angry_(false),
      sndAngry_(audio.createSound("walker_angry.ogg")),
      sndExposed_(audio.createSound("walker_exposed.ogg")),
      sndDie_(audio.createSound("walker_die.ogg")),
      stepSounded_(true)
    {
        for (int i = 0; i < 4; ++i) {
            legs_[i] = legs[i];
            legTips_[i] = legTips[i];
        }

        sndStep_[0] = audio.createSound("walker_step1.ogg");
        sndStep_[1] = audio.createSound("walker_step2.ogg");
    }

    WalkerComponent::~WalkerComponent()
    {
    }

    void WalkerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WalkerComponent::update(float dt)
    {
        updateAutoTarget(dt);

        t_ -= dt;
        shootT_ -= dt;

        if (target() && (shootT_ <= 0.0f)) {
            if ((state_ < StateExposing) && scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                    parent()->pos(), 15.0f, 15.0f)) {
                b2Vec2 dir = target()->pos() - parent()->pos();

                int i = rand() % 2;

                if (fabs(angleBetween(dir, legDir_[0])) <= (b2_pi / 2.0f)) {
                    weapon_[0 + i]->setTarget(target());
                    weapon_[0 + i]->triggerOnce();
                } else {
                    weapon_[2 + i]->setTarget(target());
                    weapon_[2 + i]->triggerOnce();
                }
            }
            shootT_ = getRandom(settings.walker.missileDelay, settings.walker.missileDelay * 2.0f);
        }

        if ((state_ < StateDie1) && (state_ != StateExposed)) {
            b2Vec2 dir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            if (b2Cross(dir, legDir_[0]) >= 0.0f) {
                parent()->applyTorque(20000.0f, true);
            } else {
                parent()->applyTorque(-20000.0f, true);
            }
        }

        bool legsMoved = true;

        updateBody(dt);

        for (int i = 0; i < 4; ++i) {
            legsMoved &= updateLeg(i, dt);
        }

        switch (state_) {
        case StateInit: {
            for (int i = 0; i < 4; ++i) {
                move(i, 0.0f);
                moveLegDown(i, 40.0f, EaseLinear);
            }
            state_ = StateDown;
            break;
        }
        case StateDown: {
            if (!legsMoved) {
                break;
            }

            if ((legIndex_ == 0) && !stepSounded_) {
                stepSounded_ = true;
                sndStep_[0]->stop();
                sndStep_[1]->stop();
                sndStep_[rand() % 2]->play();
                scene()->camera()->findComponent<CameraComponent>()->roll(deg2rad(1.0f));
            }

            if ((legIndex_ == 0) && (!target() || expose_ || angry_)) {
                for (std::vector<RevoluteJointProxyPtr>::const_iterator it = motorJoints_.begin();
                     it != motorJoints_.end(); ++it) {
                    (*it)->enableMotor(false);
                }
                if (expose_) {
                    expose_ = false;
                    if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                        parent()->pos(), 10.0f, 10.0f)) {
                        state_ = StateExposing;
                        t_ = 1.0f;
                        doorJoint_[doorIndex_]->setLimits(0, b2_pi / 2.0f);

                        healthBefore_ = heart_->life();

                        heart_->setInvulnerable(false);

                        RenderHealthbarComponentPtr component =
                            boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.5f), 0, 8.0f, 1.2f, zOrderMarker + 1);

                        component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                        heart_->addComponent(component);

                        break;
                    }
                }
                if (angry_) {
                    angry_ = false;
                    state_ = StateAngry;
                    t_ = 2.0f;

                    SequentialTweeningPtr tweening =
                        boost::make_shared<SequentialTweening>(true);

                    tweening->addTweening(boost::make_shared<SingleTweening>(0.15f, EaseInOutQuad, -80000.0f, 80000.0f));
                    tweening->addTweening(boost::make_shared<SingleTweening>(0.15f, EaseInOutQuad, 80000.0f, -80000.0f));

                    angryTweening_= tweening;
                    angryTweenTime_ = 0.0f;

                    scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.5f);

                    sndAngry_->play();

                    break;
                }
                if (!target()) {
                    break;
                }
            }

            for (std::vector<RevoluteJointProxyPtr>::const_iterator it = motorJoints_.begin();
                 it != motorJoints_.end(); ++it) {
                (*it)->enableMotor(true);
            }

            float dist = 5.0f;
            if ((legIndex_ == 2.0f) &&
                (((legPos_[0] - legPos_[2]).Length() >= 39.0f) ||
                 ((legPos_[1] - legPos_[3]).Length() >= 39.0f))) {
                dist = 6.0f;
            }

            move(legIndex_, dist);

            if (legIndex_ == 0) {
                moveBody(14.0f, 20.0f, EaseLinear);
                stepSounded_ = false;
            }

            moveLegUp(legIndex_, 4.0f, 80.0f, EaseLinear);
            state_ = StateUp;
            break;
        }
        case StateUp: {
            if (!legsMoved) {
                break;
            }
            moveLegDown(legIndex_, 80.0f, EaseLinear);
            legIndex_ = (legIndex_ + 1) % 4;
            state_ = StateDown;
            break;
        }
        case StateExposing: {
            if (heart_->dead()) {
                RenderHealthbarComponentPtr hc =
                    heart_->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }

                t_ = 0.5f;
                state_ = StateDie1;
                doorJoint_[doorIndex_]->setMotorSpeed(-doorJoint_[doorIndex_]->motorSpeed());
                break;
            }

            if (t_ >= 0.0f) {
                break;
            }

            state_ = StateExposed;
            t_ = 4.0f;

            SequentialTweeningPtr tweening =
                boost::make_shared<SequentialTweening>(true);

            tweening->addTweening(boost::make_shared<SingleTweening>(0.25f, EaseInOutQuad, 80000.0f, 80000.0f));
            tweening->addTweening(boost::make_shared<SingleTweening>(0.25f, EaseInOutQuad, -80000.0f, -80000.0f));

            exposedTweening_= tweening;
            exposedTweenTime_ = 0.0f;

            for (int i = 0; i < 4; ++i) {
                exposedWeapon_[i]->trigger(true);
            }

            scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);

            sndExposed_->play();

            break;
        }
        case StateExposed: {
            float value = exposedTweening_->getValue(exposedTweenTime_);

            parent()->applyTorque(value, true);

            exposedTweenTime_ += dt;

            if (t_ >= 0.0f) {
                break;
            }

            for (int i = 0; i < 4; ++i) {
                exposedWeapon_[i]->trigger(false);
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);

            doorJoint_[doorIndex_]->setMotorSpeed(-doorJoint_[doorIndex_]->motorSpeed());
            state_ = StateHiding;
            t_ = 1.0f;

            break;
        }
        case StateHiding: {
            if (t_ >= 0.0f) {
                break;
            }

            RenderHealthbarComponentPtr hc =
                heart_->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            heart_->setInvulnerable(true);

            doorJoint_[doorIndex_]->setLimits(0, 0);
            doorJoint_[doorIndex_]->setMotorSpeed(-doorJoint_[doorIndex_]->motorSpeed());

            if (heart_->dead()) {
                startDie2();
                break;
            }

            state_ = StateDown;

            if ((healthBefore_ - heart_->life()) > 25.0f) {
                doorIndex_ = 1 - doorIndex_;
            }

            break;
        }
        case StateAngry: {
            float value = angryTweening_->getValue(angryTweenTime_);

            parent()->applyForceToCenter(value * parent()->getDirection(1.0f).Skew(), true);
            parent()->applyTorque(value * 2.0f, true);

            angryTweenTime_ += dt;

            if (t_ >= 0.0f) {
                break;
            }

            scene()->camera()->findComponent<CameraComponent>()->tremor(false);

            state_ = StateDown;

            shootT_ = 4.0f;

            break;
        }
        case StateDie1: {
            if (t_ >= 0.0f) {
                break;
            }

            doorJoint_[doorIndex_]->setLimits(0, 0);
            doorJoint_[doorIndex_]->setMotorSpeed(-doorJoint_[doorIndex_]->motorSpeed());

            startDie2();

            break;
        }
        case StateDie2: {
            if (t_ >= 0.0f) {
                break;
            }

            if (dieObjs_.empty()) {
                parent()->findComponentByName<RenderQuadComponent>("pad")->setZOrder(zOrderBack - 1);

                parent()->setLinearVelocity(b2Vec2(0, 0));
                parent()->setAngularVelocity(0.0f);
                parent()->body()->SetType(b2_kinematicBody);

                state_ = StateDie3;
                scene()->stats()->incEnemiesKilled();
                break;
            }

            SceneObjectPtr obj = dieObjs_.back();
            dieObjs_.pop_back();

            obj->findComponent<PhysicsBodyComponent>()->disableAllFixtures();
            obj->setLinearDamping(1.0f);
            obj->setAngularDamping(1.0f);
            obj->setLinearVelocity(obj->linearVelocity() + dieVelDir_);
            obj->setAngularVelocity(b2_pi * 4.0f * (((rand() % 2) == 0) ? 1.0f : -1.0f));

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(1.0f);

            obj->addComponent(component);

            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(obj->getTransform());

            scene()->addObject(explosion);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(6.0f, zOrderBack);

            bs->setPos(obj->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            dieVelDir_ = b2Mul(dieRot_, dieVelDir_);

            if ((rand() % 2) == 0) {
                dieVelDir_ = -dieVelDir_;
            }

            t_ = 0.3f;

            break;
        }
        case StateDie3: {
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void WalkerComponent::debugDraw()
    {
        static const Color colors[] = {
            Color(1.0f, 1.0f, 1.0f),
            Color(1.0f, 0.0f, 0.0f)
        };

        renderer.setProgramColorLines(2.0f);

        for (int i = 0; i < 4; i += 2) {
            RenderLineStrip rop = renderer.renderLineStrip();
            rop.addVertex(legPos_[i].x, legPos_[i].y);
            rop.addVertex(legPos_[i + 1].x, legPos_[i + 1].y);
            rop.addColors(colors[i / 2]);
        }
    }

    void WalkerComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        heart_->setInvulnerable(true);

        dummyObj_ = sceneObjectFactory.createDummy();
        scene()->addObject(dummyObj_);

        legPos_[0] = legPos_[1] = parent()->pos() + parent()->getDirection(13.0f);
        legPos_[2] = legPos_[3] = parent()->pos() - parent()->getDirection(13.0f);
        legDir_[0] = legDir_[1] = parent()->getDirection(1.0f);

        PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

        motorJoints_ = jc->joints<RevoluteJointProxy>("motor_joint");

        for (std::vector<RevoluteJointProxyPtr>::const_iterator it = motorJoints_.begin();
             it != motorJoints_.end(); ++it) {
            (*it)->enableMotor(false);
        }

        doorJoint_[0] = jc->joint<RevoluteJointProxy>("door1_joint");
        doorJoint_[1] = jc->joint<RevoluteJointProxy>("door2_joint");
    }

    void WalkerComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());

        heart_.reset();
        for (int i = 0; i < 4; ++i) {
            legs_[i].reset();
        }

        dummyObj_->removeFromParent();
        dummyObj_.reset();
    }

    void WalkerComponent::move(int index, float distance)
    {
        static float seekAngle = deg2rad(2.0f);

        if ((index % 2) != 0) {
            return;
        }

        b2Vec2 dir = legDir_[index / 2];

        b2Vec2 p1 = 0.5f * (legPos_[index] + legPos_[index + 1]) + distance * dir;

        b2Rot rot(-seekAngle);

        dir = dir.Skew();

        float minLen = b2_maxFloat;

        for (float a = seekAngle; a < (b2_pi - seekAngle); a += seekAngle) {
            dir = b2Mul(rot, dir);

            b2Vec2 p2 = p1 + 50.0f * dir.Skew();

            scene()->rayCast(p1, p2,
                boost::bind(&WalkerComponent::rayCastCb, this, _1, _2, _3, _4,
                    boost::ref(p2)));

            b2Vec2 p3 = p1 - 50.0f * dir.Skew();

            scene()->rayCast(p1, p3,
                boost::bind(&WalkerComponent::rayCastCb, this, _1, _2, _3, _4,
                    boost::ref(p3)));

            if ((p3 - p2).LengthSquared() < minLen) {
                minLen = (p3 - p2).LengthSquared();

                legPos_[index] = p2;
                legPos_[index + 1] = p3;
                legDir_[index / 2] = dir;
            }
        }
    }

    bool WalkerComponent::updateLeg(int index, float dt)
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

        return legTweening_[index]->finished(legTweenTime_[index]);
    }

    void WalkerComponent::moveLegUp(int index, float distance, float velocity, Easing easing)
    {
        float len = legJoint_[index]->length();

        if (len < distance) {
            legTweening_[index] = boost::make_shared<SingleTweening>(
                (distance - len) / velocity, easing, len, distance);
            legTweenTime_[index] = 0.0f;
        }
    }

    void WalkerComponent::moveLegDown(int index, float velocity, Easing easing)
    {
        if (legJoint_[index]) {
            legJoint_[index]->remove();
        }

        float len = (legs_[index]->getWorldPoint(legTips_[index]) - legPos_[index]).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        legJoint_[index] = scene()->addDistanceJoint(dummyObj_, legs_[index],
            dummyObj_->getLocalPoint(legPos_[index]), legTips_[index], len,
            10.0f, 1.0f, false);

        legTweening_[index] = boost::make_shared<SingleTweening>(len / velocity, easing, len, 0.0f);
        legTweenTime_[index] = 0.0f;
    }

    bool WalkerComponent::updateBody(float dt)
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

        bool res = bodyTweening_->finished(bodyTweenTime_);

        if (res) {
            bodyJoint_->remove();
            bodyJoint_.reset();
        }

        return res;
    }

    void WalkerComponent::moveBody(float distance, float velocity, Easing easing)
    {
        static float seekAngle = deg2rad(2.0f);

        b2Vec2 dir = legDir_[0];

        b2Vec2 p1 = 0.5f * (legPos_[0] + legPos_[1]) - distance * dir;

        b2Rot rot(-seekAngle);

        dir = dir.Skew();

        float minLen = b2_maxFloat;

        b2Vec2 p = p1;

        for (float a = seekAngle; a < (b2_pi - seekAngle); a += seekAngle) {
            dir = b2Mul(rot, dir);

            b2Vec2 p2 = p1 + 50.0f * dir.Skew();

            scene()->rayCast(p1, p2,
                boost::bind(&WalkerComponent::rayCastCb, this, _1, _2, _3, _4,
                    boost::ref(p2)));

            b2Vec2 p3 = p1 - 50.0f * dir.Skew();

            scene()->rayCast(p1, p3,
                boost::bind(&WalkerComponent::rayCastCb, this, _1, _2, _3, _4,
                    boost::ref(p3)));

            if ((p3 - p2).LengthSquared() < minLen) {
                minLen = (p3 - p2).LengthSquared();

                p = 0.5f * (p2 + p3);
            }
        }

        if (bodyJoint_) {
            bodyJoint_->remove();
        }

        float len = (parent()->pos() - p).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        bodyJoint_ = scene()->addDistanceJoint(dummyObj_, parent()->shared_from_this(),
            dummyObj_->getLocalPoint(p), b2Vec2_zero, len,
            10.0f, 1.0f, false);

        bodyTweening_ = boost::make_shared<SingleTweening>(len / velocity, easing, len, 0.0f);
        bodyTweenTime_ = 0.0f;
    }

    float WalkerComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        SceneObject* obj = SceneObject::fromFixture(fixture);

        if ((obj->type() != SceneObjectTypeBlocker) ||
            (obj->parentObject() == parent())) {
            return -1;
        }

        finalPoint = point;

        return fraction;
    }

    void WalkerComponent::startDie2()
    {
        for (int i = 0; i < 4; ++i) {
            if (legJoint_[i]) {
                legJoint_[i]->remove();
                legJoint_[i].reset();
            }
        }

        PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

        PhysicsJointComponent::Joints joints = jc->joints();

        for (PhysicsJointComponent::Joints::const_iterator it = joints.begin();
             it != joints.end(); ++it) {
            (*it)->remove();
        }

        dieObjs_ = parent()->getObjects();
        dieVelDir_ = parent()->getDirection(30.0f);
        dieRot_ = b2Rot(2.0f * b2_pi / (dieObjs_.size() + 1));

        state_ = StateDie2;
        t_ = 0.0f;

        sndDie_->play();
    }
}
