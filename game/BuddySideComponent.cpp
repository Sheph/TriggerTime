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

#include "BuddySideComponent.h"
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
    BuddySideComponent::BuddySideComponent(int index, const Layer layers[8])
    : PhasedComponent(phaseThink),
      index_(index),
      state_(StateDead),
      idleRot_(0.0f),
      deadFinal_(false),
      prevHealth_(0.0f),
      painT_(0.0f)
    {
        switch (index) {
        case 1: t_ = settings.bossBuddy.sideDeadDuration / 2.0f; break;
        case 2: t_ = settings.bossBuddy.sideDeadDuration; break;
        default:
        case 3: t_ = settings.bossBuddy.sideDeadDuration; break;
        }

        for (int i = 0; i < 8; ++i) {
            layers_[i] = layers[i];
        }

        sndMelee_[0] = audio.createSound("buddy_melee1.ogg");
        sndMelee_[1] = audio.createSound("buddy_melee2.ogg");
        sndWhoosh_[0] = audio.createSound("buddy_whoosh1.ogg");
        sndWhoosh_[1] = audio.createSound("buddy_whoosh2.ogg");
        sndPain_[0] = audio.createSound("buddy_side_pain1.ogg");
        sndPain_[1] = audio.createSound("buddy_side_pain2.ogg");
        sndDie_ = audio.createSound("buddy_side_die.ogg");
        sndResurrect_ = audio.createSound("buddy_side_resurrect.ogg");
    }

    BuddySideComponent::~BuddySideComponent()
    {
    }

    void BuddySideComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BuddySideComponent::update(float dt)
    {
        painT_ -= dt;

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (layers_[i].elbowS[j]) {
                    layers_[i].elbowS[j]->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (layers_[i].elbow[j]->pos() - layers_[i].elbowS[j]->pos()));
                    layers_[i].elbowS[j]->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(layers_[i].elbow[j]->angle() - layers_[i].elbowS[j]->angle()));
                }
                if (layers_[i].armS[j]) {
                    layers_[i].armS[j]->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (layers_[i].arm[j]->pos() - layers_[i].armS[j]->pos()));
                    layers_[i].armS[j]->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(layers_[i].arm[j]->angle() - layers_[i].armS[j]->angle()));
                }
            }
        }

        if (target()) {
            t_ -= dt;
        }

        if (target() && (state_ != StateDead)) {
            if (!layers_[4].head->dead() && (layers_[4].head->life() < prevHealth_)) {
                if (painT_ <= 0.0f) {
                    sndPain_[0]->stop();
                    sndPain_[1]->stop();
                    sndPain_[rand() % 2]->play();
                    painT_ = 0.65f;
                }
            }

            prevHealth_ = layers_[4].head->life();

            b2Vec2 targetDir = target()->pos() - layers_[7].head->pos();

            b2Vec2 dir = angle2vec(layers_[7].head->angle() + layers_[7].head->angularVelocity() / 3.0f, 1.0f);

            if (b2Cross(dir, targetDir) >= 0.0f) {
                layers_[7].head->applyTorque(1000.0f, true);
            } else {
                layers_[7].head->applyTorque(-1000.0f, true);
            }

            if (layers_[0].head->linearVelocity().Length() <= 20.0f) {
                b2Vec2 tmp = target()->pos() - layers_[0].head->pos();
                tmp.Normalize();
                layers_[0].head->applyForceToCenter(300.0f * tmp, true);
            }
        }

        switch (state_) {
        case StateIdle: {
            if (target()) {
                state_ = StateWalk;
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                state_ = StateIdle;

                break;
            }

            if (layers_[4].head->dead()) {
                RenderHealthbarComponentPtr hc = layers_[7].head->findComponent<RenderHealthbarComponent>();
                if (hc) {
                    hc->removeFromParent();
                }
                state_ = StateDead;
                t_ = settings.bossBuddy.sideDeadDuration;
                if (!deadFinal_) {
                    sndDie_->play();
                }
                break;
            }

            if ((b2DistanceSquared(layers_[0].head->pos(), target()->pos()) <= (15.0f * 15.0f)) && (t_ <= 0.0f)) {
                state_ = ((rand() % 3) != 0) ? StateMelee1 : StateOneSideMelee1;
                whichSide_ = rand() % 2;
                t_ = 0.5f;
                sndMelee_[rand() % 2]->play();
                whooshed_ = false;
                break;
            }

            b2Vec2 v = b2Mul(idleRot_, b2Vec2(30.0f, 0.0f));

            layers_[7].head->applyForceToCenter(v, true);

            idleRot_ = b2Mul(idleRot_, b2Rot(deg2rad(90.0f) * dt));

            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 2; ++j) {
                    b2Vec2 v = b2Mul(layers_[i].armRot[j], b2Vec2(2.0f, 0.0f));

                    layers_[i].arm[j]->applyForceToCenter(v, true);

                    layers_[i].armRot[j] = b2Mul(layers_[i].armRot[j], b2Rot(deg2rad(180.0f) * dt * layers_[i].armRotDir[j]));
                }
            }

            break;
        }
        case StateMelee1: {
            if (t_ <= 0.0f) {
                state_ = StateMelee2;
                t_ = 0.5f;

                enableHands(true);

                break;
            }
            for (int i = 5; i <= 7; ++i) {
                layers_[i].elbow[0]->applyTorque(100.0f, true);
                layers_[i].elbow[1]->applyTorque(-100.0f, true);
                layers_[i].head->applyForceToCenter(layers_[i].head->getDirection(-100.0f), true);
            }
            break;
        }
        case StateMelee2: {
            if (!whooshed_ && (t_ <= 0.25f)) {
                whooshed_ = true;
                sndWhoosh_[rand() % 2]->play();
            }
            if ((t_ <= 0.0f) || layers_[4].head->dead()) {
                state_ = StateWalk;
                t_ = 1.0f;

                enableHands(false);

                break;
            }
            for (int i = 5; i <= 7; ++i) {
                layers_[i].elbow[0]->applyTorque(-200.0f, true);
                layers_[i].elbow[1]->applyTorque(200.0f, true);
                layers_[i].arm[0]->applyTorque(-20.0f, true);
                layers_[i].arm[1]->applyTorque(20.0f, true);
                layers_[i].head->applyForceToCenter(layers_[i].head->getDirection(100.0f), true);
            }
            break;
        }
        case StateOneSideMelee1: {
            if (t_ <= 0.0f) {
                state_ = StateOneSideMelee2;
                t_ = 0.5f;

                enableHands(true);

                break;
            }
            int k = ((whichSide_ == 0) ? 1 : -1);
            for (int i = 5; i <= 7; ++i) {
                layers_[i].elbow[whichSide_]->applyTorque(100.0f * k, true);
                layers_[i].head->applyForceToCenter(layers_[i].head->getDirection(-100.0f), true);
                layers_[i].head->applyTorque(500.0f * k, true);
            }
            break;
        }
        case StateOneSideMelee2: {
            if (!whooshed_ && (t_ <= 0.25f)) {
                whooshed_ = true;
                sndWhoosh_[rand() % 2]->play();
            }
            if ((t_ <= 0.0f) || layers_[4].head->dead()) {
                state_ = StateWalk;
                t_ = 1.0f;

                enableHands(false);

                break;
            }
            int k = ((whichSide_ == 0) ? 1 : -1);
            for (int i = 5; i <= 7; ++i) {
                layers_[i].elbow[whichSide_]->applyTorque(-200.0f * k, true);
                layers_[i].arm[whichSide_]->applyTorque(-20.0f * k, true);
                layers_[i].head->applyForceToCenter(layers_[i].head->getDirection(100.0f), true);
                layers_[i].head->applyTorque(-1000.0f * k, true);
            }
            break;
        }
        case StateDead: {
            if ((t_ <= 0.0f) && !deadFinal_) {
                resurrect();
                state_ = StateWalk;
                t_ = 1.0f;
                sndResurrect_->play();
                break;
            }
            for (int i = 0; i < 8; ++i) {
                layers_[i].elbow[0]->applyTorque(-70.0f, true);
                layers_[i].elbow[1]->applyTorque(70.0f, true);
                layers_[i].arm[0]->applyTorque(-5.0f, true);
                layers_[i].arm[1]->applyTorque(5.0f, true);
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void BuddySideComponent::onRegister()
    {
        for (int i = 0; i < 8; ++i) {
            b2Rot rot(getRandom(0.0f, b2_pi * 2.0f));

            layers_[i].armRot[0] = rot;
            layers_[i].armRot[1] = rot;

            int dir = ((rand() % 2) == 0) ? 1 : -1;

            layers_[i].armRotDir[0] = dir;
            layers_[i].armRotDir[1] = -dir;
        }

        prevHealth_ = layers_[4].head->life();
    }

    void BuddySideComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (int i = 0; i < 8; ++i) {
            layers_[i] = Layer();
        }
    }

    void BuddySideComponent::resurrect()
    {
        layers_[4].head->setBlastDamageMultiplier(4.0f);

        layers_[4].head->setMaxLife(settings.bossBuddy.sideLife);
        layers_[4].head->setLife(settings.bossBuddy.sideLife);

        RenderHealthbarComponentPtr component =
            boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 8.5f), 0, 12.0f, 2.0f, zOrderMarker + 1);

        component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
        component->setTarget(layers_[4].head);

        layers_[7].head->addComponent(component);

        prevHealth_ = layers_[4].head->life();
    }

    void BuddySideComponent::enableHands(bool enable)
    {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (layers_[i].elbowS[j]) {
                    layers_[i].elbowS[j]->findComponent<PhysicsBodyComponent>()->setSensor(!enable);
                }
                if (layers_[i].armS[j]) {
                    layers_[i].armS[j]->findComponent<PhysicsBodyComponent>()->setSensor(!enable);
                }
            }
        }
    }

    void BuddySideComponent::setDeadFinal()
    {
        layers_[4].head->setLife(0.0f);
        deadFinal_ = true;
    }
}
