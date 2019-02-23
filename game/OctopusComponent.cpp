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

#include "OctopusComponent.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "CollisionCancelComponent.h"
#include "RenderQuadComponent.h"
#include "RenderTentacleComponent.h"
#include "RenderHealthbarComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    OctopusComponent::OctopusComponent()
    : TargetableComponent(phaseThink),
      state_(StateInit),
      t_(0.0f),
      rollDir_(1),
      biteTime_(0.0f),
      sndBite_(audio.createSound("octopus_bite.ogg")),
      sndDie_(audio.createSound("octopus_die.ogg"))
    {
        sndCharge_[0] = audio.createSound("octopus_charge3.ogg");
        sndCharge_[1] = audio.createSound("octopus_charge4.ogg");
    }

    OctopusComponent::~OctopusComponent()
    {
    }

    void OctopusComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void OctopusComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            sndDie_->play();
            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            CollisionCancelComponentPtr c = parent()->findComponent<CollisionCancelComponent>();
            if (c) {
                c->setDamage(0.0f);
                c->setDamageSound(AudioSourcePtr());
                c->setRoll(0.0f);
                c->setFilter(c->filter() | SceneObjectTypeEnemyBuilding);
            }
            parent()->setType(SceneObjectTypeGarbage);
            parent()->findComponent<PhysicsBodyComponent>()->setFilterCategoryBits(collisionBitGarbage);
            parent()->findComponent<PhysicsBodyComponent>()->setFilterGroupIndex(collisionGroupMissile);
            parent()->findComponent<RenderQuadComponent>()->setZOrder(zOrderBack + 2);

            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            std::vector<WeldJointProxyPtr> weldJoints = jc->joints<WeldJointProxy>("joint");

            for (std::vector<WeldJointProxyPtr>::const_iterator it = weldJoints.begin();
                 it != weldJoints.end(); ++it) {
                (*it)->setDampingRatio(0.01f);
                (*it)->setFrequency(0.01f);
            }

            PhysicsJointComponent::Joints joints = jc->joints<JointProxy>("br_joint");

            for (PhysicsJointComponent::Joints::const_iterator it = joints.begin();
                 it != joints.end(); ++it) {
                (*it)->remove();
            }

            b2Vec2 velDir = parent()->getDirection(2.0f);
            b2Rot rot(2.0f * b2_pi / (parent()->objects().size() + 1));

            parent()->setAngularVelocity(b2_pi * 8.0f);

            parent()->setLinearVelocity(0.5f * parent()->linearVelocity());

            for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
                 it != parent()->objects().end(); ++it) {
                (*it)->setType(SceneObjectTypeGarbage);
                (*it)->findComponent<PhysicsBodyComponent>()->setFilterCategoryBits(collisionBitGarbage);

                RenderTentacleComponentPtr c = (*it)->findComponent<RenderTentacleComponent>();
                if (c) {
                    c->setZOrder(zOrderBack + 1);
                }

                CollisionCancelComponentPtr cc = (*it)->findComponent<CollisionCancelComponent>();
                if (cc) {
                    cc->setFilter(cc->filter() | SceneObjectTypeEnemyBuilding);
                }

                SceneObjectPtr obj = sceneObjectFactory.createBlood1(3.0f, zOrderExplosion - 1);

                obj->setPos((*it)->pos());
                obj->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(obj);

                velDir = b2Mul(rot, velDir);

                if ((rand() % 2) == 0) {
                    velDir = -velDir;
                }

                (*it)->setLinearVelocity(0.5f * (*it)->linearVelocity() + velDir);
            }

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(12.0f, zOrderBack);

            bs->setPos(parent()->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            audio.playSound("tetrocrab_crack.ogg");

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        t_ -= dt;
        biteTime_ -= dt;

        updateAutoTarget(dt);

        if (target()) {
            if ((biteTime_ <= 0.0f) && (b2DistanceSquared(parent()->pos(), target()->pos()) <= (2.8f * 2.8f))) {
                target()->changeLife(-settings.octopus.biteDamage);

                sndBite_->play();

                biteTime_ = settings.octopus.biteTime;
            }
        }

        switch (state_) {
        case StateInit: {
            if (!target()) {
                break;
            }

            startRoll(false);
            break;
        }
        case StateRoll: {
            parent()->applyTorque(settings.octopus.rollTorque * rollDir_, true);

            if (t_ <= 0.0f) {
                if ((rand() % 4) != 0) {
                    chargeDir_ = target()->pos() - parent()->pos();
                    chargeDir_.Normalize();
                    state_ = StateCharge;
                    t_ = getRandom(settings.octopus.chargeDuration / 2.0f, settings.octopus.chargeDuration * 2.0f);
                    if ((rand() % 2) == 0) {
                        sndCharge_[rand() % 2]->play();
                    }
                } else {
                    startRoll(true);
                }
                break;
            }

            break;
        }
        case StateCharge: {
            parent()->applyForceToCenter(settings.octopus.chargeForce * chargeDir_, true);

            if (t_ <= 0.0f) {
                state_ = StateChargePost;
                t_ = settings.octopus.chargeDelay;
                break;
            }

            break;
        }
        case StateChargePost: {
            if (t_ <= 0.0f) {
                startRoll((rand() % 3) == 0);
                break;
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void OctopusComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void OctopusComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    void OctopusComponent::startRoll(bool shoot)
    {
        t_ = getRandom(0.25f, settings.octopus.rollDuration);
        state_ = StateRoll;
        rollDir_ = ((rand() % 2) == 0) ? 1 : -1;

        if (shoot) {
            for (std::vector<WeaponComponentPtr>::const_iterator it = weapons_.begin();
                 it != weapons_.end(); ++it) {
                (*it)->triggerOnce();
            }
        }
    }
}
