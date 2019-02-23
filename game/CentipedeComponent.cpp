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

#include "CentipedeComponent.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "FadeOutComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    CentipedeComponent::CentipedeComponent(const std::vector<AnimationComponentPtr>& acs,
        const AnimationComponentPtr& headAc)
    : TargetableComponent(phaseThink),
      acs_(acs),
      headAc_(headAc),
      state_(StateIdle),
      t_(0.0f),
      biteTime_(0.0f),
      trapped_(false),
      spawnPowerup_(true),
      sndBite_(audio.createSound("centipede_bite.ogg")),
      sndDie_(audio.createSound("centipede_die.ogg"))
    {
        sndAttack_[0] = audio.createSound("centipede_attack1.ogg");
        sndAttack_[1] = audio.createSound("centipede_attack2.ogg");
    }

    CentipedeComponent::~CentipedeComponent()
    {
    }

    void CentipedeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void CentipedeComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            sndDie_->play();

            if (detourBehavior_) {
                detourBehavior_->reset();
                avoidBehavior_->reset();
                interceptBehavior_->reset();
            }

            for (std::vector<AnimationComponentPtr>::const_iterator it = acs_.begin();
                 it != acs_.end(); ++it) {
                (*it)->setPaused(true);
            }

            PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

            for (PhysicsJointComponent::Joints::const_iterator it = jc->joints().begin();
                 it != jc->joints().end(); ++it) {
                (*it)->remove();
            }

            parent()->findComponent<PhysicsBodyComponent>()->disableAllFixtures();
            parent()->setLinearDamping(9.0f);

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.0f);
            parent()->addComponent(component);

            std::set<SceneObjectPtr> tmp = parent()->objects();

            std::ostringstream os;

            os << "part" << (rand() % 9) + 1;

            std::string n1 = os.str();

            for (std::set<SceneObjectPtr>::const_iterator it = tmp.begin();
                 it != tmp.end(); ++it) {
                SceneObjectPtr obj = sceneObjectFactory.createBlood1(4.0f, zOrderExplosion - 1);

                obj->setPos((*it)->pos());
                obj->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(obj);

                if (((*it)->name() == n1) || ((*it)->name() == "tail")) {
                    (*it)->findComponent<PhysicsBodyComponent>()->setDensity(0.07f);
                    (*it)->resetMassData();

                    sceneObjectFactory.makeDebris(*it);

                    (*it)->setLinearDamping(6.0f);
                    (*it)->setAngularDamping(6.0f);

                    scene()->reparent(*it);

                    SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(4.0f, zOrderBack, (*it)->activeDeadbody());

                    bs->setPos((*it)->pos());
                    bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                    scene()->addObject(bs);
                } else {
                    (*it)->findComponent<PhysicsBodyComponent>()->disableAllFixtures();
                    (*it)->setLinearDamping(9.0f);

                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f, 1.0f);
                    (*it)->addComponent(component);
                }
            }

            if (spawnPowerup_) {
                scene()->spawnPowerup(parent()->pos());
            }

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        if (parent()->linearVelocity().LengthSquared() >= 3.0f) {
            for (std::vector<AnimationComponentPtr>::const_iterator it = acs_.begin();
                 it != acs_.end(); ++it) {
                (*it)->setPaused(false);
            }
        } else {
            for (std::vector<AnimationComponentPtr>::const_iterator it = acs_.begin();
                 it != acs_.end(); ++it) {
                (*it)->setPaused(true);
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

        t_ -= dt;
        biteTime_ -= dt;

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setAngularVelocity(actualVelocity(settings.centipede.turnSpeed));
                interceptBehavior_->setTarget(target());
                interceptBehavior_->start();

                avoidBehavior_->reset();
                avoidBehavior_->setTypes(SceneObjectTypeEnemy);
                avoidBehavior_->setRadius(5.0f);
                avoidBehavior_->setLinearVelocity(actualVelocity(settings.centipede.walkSpeed));
                avoidBehavior_->setTimeout(0.05f);
                avoidBehavior_->setLoop(true);
                avoidBehavior_->start();

                detourBehavior_->reset();
                detourBehavior_->setAheadDistance(5.0f);
                detourBehavior_->setAngularVelocity(actualVelocity(settings.centipede.turnSpeed));
                detourBehavior_->setTimeout(0.1f);
                detourBehavior_->start();

                state_ = StateWalk;

                resetShootTime();
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            float ang = fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos()));

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= 2.5f * 2.5f) {
                if ((ang <= deg2rad(30)) && !trapped_) {
                    detourBehavior_->reset();
                    avoidBehavior_->reset();
                    interceptBehavior_->setTarget(target());

                    state_ = StateBite;

                    break;
                }
            }

            if ((t_ <= 0.0f) && (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) <= settings.centipede.attackAngle)) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(parent()->pos(), 1.0f, 1.0f) && !trapped_) {
                    detourBehavior_->reset();
                    avoidBehavior_->reset();
                    interceptBehavior_->reset();

                    sndAttack_[rand() % 2]->play();

                    weapon_->triggerOnce();

                    state_ = StateShoot;

                    headAc_->startAnimation(AnimationAttack);

                    t_ = settings.centipede.spitInterval * (settings.centipede.spitTotalShots / settings.centipede.spitMinShots + 1);

                    break;
                } else {
                    resetShootTime();
                }
            }

            bool detouring = !detourBehavior_->finished();

            if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(target());
            }

            if (parent()->linearVelocity().Length() < settings.centipede.walkSpeed) {
                parent()->applyForceToCenter(actualVelocity(parent()->mass()) *
                    parent()->getDirection(settings.centipede.walkSpeed * 1.0f + parent()->linearVelocityDamped()), true);
            }

            break;
        }
        case StateBite: {
            if (!target() ||
                (b2DistanceSquared(parent()->pos(), target()->pos()) > 2.5f * 2.5f) ||
                (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) > deg2rad(30))) {
                interceptBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            if (biteTime_ <= 0.0f) {
                target()->changeLife(-settings.centipede.biteDamage);

                sndBite_->play();

                biteTime_ = settings.centipede.biteTime;

                headAc_->startAnimation(AnimationAttack);
            }

            break;
        }
        case StateShoot: {
            if (t_ <= 0.0f) {
                state_ = StateIdle;
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void CentipedeComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void CentipedeComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    float CentipedeComponent::actualVelocity(float value)
    {
        return value * (parent()->objects().size() + 1);
    }

    void CentipedeComponent::resetShootTime()
    {
        t_ = getRandom(settings.centipede.attackTimeout * 0.5f, settings.centipede.attackTimeout * 1.2f);
    }
}
