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

#include "SnakeComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "SceneObjectFactory.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "CameraComponent.h"
#include "RenderHealthbarComponent.h"
#include "FadeOutComponent.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    SnakeComponent::SnakeComponent(const SceneObjectPtr& eye,
        const RenderTentacleComponentPtr& tentacle)
    : TargetableComponent(phaseThink),
      eye_(eye),
      state_(StateIdle),
      desiredState_(StateIdle),
      numInside_(0),
      t_(0.0f),
      inSight_(true),
      sndAngry_(audio.createSound("snake_angry.ogg")),
      sndMegaShoot_(audio.createSound("snake_megashoot.ogg")),
      sndBite_(audio.createSound("snake_bite.ogg")),
      sndSight_(audio.createSound("snake_sight.ogg")),
      sndDie_(audio.createSound("snake_die.ogg"))
    {
        for (int i = 4; i < static_cast<int>(tentacle->objects().size()); i += 5) {
            dieObjs_.push_back(tentacle->objects()[i]);
        }
    }

    SnakeComponent::~SnakeComponent()
    {
    }

    void SnakeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SnakeComponent::update(float dt)
    {
        t_ -= dt;

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            state_ = desiredState_;
            if (state_ == StateMegaShoot) {
                startMegaShoot();
            } else if (state_ == StateWalk) {
                startWalk();
            } else if (state_ == StateAngry) {
                startAngry();
            } else if (state_ == StateDie1) {
                startDie();
            }
            break;
        }
        case StateWalk: {
            if (desiredState_ != state_) {
                state_ = desiredState_;
                if (state_ == StateMegaShoot) {
                    startMegaShoot();
                } else if (state_ == StateAngry) {
                    startAngry();
                } else if (state_ == StateDie1) {
                    startDie();
                }
                break;
            }

            if (numInside_ > 0) {
                for (std::vector<RevoluteJointProxyPtr>::const_iterator it = fangJoints_.begin();
                     it != fangJoints_.end(); ++it) {
                    (*it)->setMotorSpeed(-(*it)->motorSpeed());
                }
                state_ = StateBite1;
                t_ = 0.25f;
                sndBite_->play();
                break;
            }

            bool inSight = scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 4.0f, 4.0f);

            if (!inSight_ && inSight && (sndSight_->status() != AudioSource::Playing)) {
                sndSight_->play();
            }

            inSight_ = inSight;

            if (target() && (t_ <= 0.0f)) {
                if (inSight) {
                    if (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) <= deg2rad(25.0f)) {
                        if ((rand() % 3) != 0) {
                            weaponGun_->trigger(true);
                            state_ = StateShoot;
                            t_ = getRandom(settings.snake.gunDuration / 2.0f, settings.snake.gunDuration);
                        } else {
                            weaponMissile_->triggerOnce();
                            t_ = getRandom(settings.snake.shootDelay / 2.0f, settings.snake.shootDelay);
                        }
                        break;
                    }
                }
                t_ = getRandom(settings.snake.shootDelay / 2.0f, settings.snake.shootDelay);
            }

            break;
        }
        case StateBite1: {
            if (t_ >= 0.0f) {
                break;
            }

            for (std::vector<RevoluteJointProxyPtr>::const_iterator it = fangJoints_.begin();
                 it != fangJoints_.end(); ++it) {
                (*it)->setMotorSpeed(-(*it)->motorSpeed());
            }
            state_ = StateBite2;
            t_ = 0.25f;

            break;
        }
        case StateBite2: {
            if (t_ >= 0.0f) {
                break;
            }
            state_ = StateWalk;
            startWalk();
            break;
        }
        case StateMegaShoot: {
            if (desiredState_ != state_) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                weaponMissile_->trigger(false);
                weaponMissile_->setHaveSound(true);
                state_ = desiredState_;
                if (state_ == StateWalk) {
                    startWalk();
                } else if (state_ == StateAngry) {
                    startAngry();
                } else if (state_ == StateDie1) {
                    startDie();
                }
                break;
            }

            if (t_ <= 0.0f) {
                weaponMissile_->trigger(true);
            }

            float value = tweening_->getValue(tweenTime_);

            parent()->applyForceToCenter(value * megaShootForce_ * parent()->getDirection(1.0f).Skew(), true);
            parent()->applyTorque(value * megaShootTorque_, true);

            tweenTime_ += dt;

            break;
        }
        case StateShoot: {
            if ((t_ <= 0.0f) || (numInside_ > 0) || (desiredState_ != StateWalk)) {
                weaponGun_->trigger(false);
                state_ = StateWalk;
                startWalk();
                break;
            }
            break;
        }
        case StateAngry: {
            if (desiredState_ != state_) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                state_ = desiredState_;
                if (state_ == StateWalk) {
                    startWalk();
                } else if (state_ == StateMegaShoot) {
                    startMegaShoot();
                } else if (state_ == StateDie1) {
                    startDie();
                }
                break;
            }

            float value = tweening_->getValue(tweenTime_);

            parent()->applyForceToCenter(value * megaShootForce_ * parent()->getDirection(1.0f).Skew(), true);
            parent()->applyTorque(value * megaShootTorque_, true);

            tweenTime_ += dt;

            break;
        }
        case StateDie1: {
            if (t_ <= 0.0f) {
                scene()->camera()->findComponent<CameraComponent>()->tremor(false);
                state_ = StateDie2;
                scene()->stats()->incEnemiesKilled();
                break;
            }

            float value = tweening_->getValue(tweenTime_);

            parent()->applyForceToCenter(value * megaShootForce_ * parent()->getDirection(1.0f).Skew(), true);
            parent()->applyTorque(value * megaShootTorque_, true);
            value *= -1.0f;

            for (std::vector<SceneObjectPtr>::const_iterator it = dieObjs_.begin();
                 it != dieObjs_.end(); ++it) {
                (*it)->applyForceToCenter(value * dieForce_ * (*it)->getDirection(1.0f).Skew(), true);
                (*it)->applyTorque(value * dieTorque_, true);
                value *= -1.0f;
            }

            tweenTime_ += dt;

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

    void SnakeComponent::sensorEnter(const SceneObjectPtr& other)
    {
        if ((other.get() == parent()) || (other->parentObject() == parent())) {
            return;
        }

        ++numInside_;
    }

    void SnakeComponent::sensorExit(const SceneObjectPtr& other)
    {
        if ((other.get() == parent()) || (other->parentObject() == parent())) {
            return;
        }

        --numInside_;
    }

    void SnakeComponent::onRegister()
    {
        PhysicsJointComponentPtr jc = parent()->findComponent<PhysicsJointComponent>();

        fangJoints_ = jc->joints<RevoluteJointProxy>("fang_joint");

        scene()->stats()->incEnemiesSpawned();
    }

    void SnakeComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        eye_.reset();
        dieObjs_.clear();
    }

    void SnakeComponent::startMegaShoot()
    {
        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(megaShootDelay_ / 2.0f, EaseInOutQuad, -1.0, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(megaShootDelay_ / 2.0f, EaseInOutQuad, 1.0f, -1.0f));

        tweening_= tweening;
        tweenTime_ = 0.0f;

        t_ = getRandom(0.0f, 1.0f);

        scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);

        sndMegaShoot_->play();
        weaponMissile_->setHaveSound(false);
    }

    void SnakeComponent::startWalk()
    {
        t_ = getRandom(settings.snake.shootDelay / 2.0f, settings.snake.shootDelay);
    }

    void SnakeComponent::startAngry()
    {
        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(megaShootDelay_ / 2.0f, EaseInOutQuad, -1.0, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(megaShootDelay_ / 2.0f, EaseInOutQuad, 1.0f, -1.0f));

        tweening_= tweening;
        tweenTime_ = 0.0f;

        scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);

        sndAngry_->play();
    }

    void SnakeComponent::startDie()
    {
        RenderHealthbarComponentPtr hc =
            eye_->findComponent<RenderHealthbarComponent>();
        if (hc) {
            hc->removeFromParent();
        }

        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.15f, EaseInOutQuad, -1.0, 1.0f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.15f, EaseInOutQuad, 1.0f, -1.0f));

        tweening_= tweening;
        tweenTime_ = 0.0f;
        t_ = 2.0f;

        FadeOutComponentPtr c = boost::make_shared<FadeOutComponent>(t_);
        c->setLightsOnly(true);
        parent()->addComponent(c);

        for (std::vector<RevoluteJointProxyPtr>::const_iterator it = fangJoints_.begin();
             it != fangJoints_.end(); ++it) {
            if ((*it)->motorSpeed() >= 0.0f) {
                (*it)->setMotorSpeed(-b2_pi / 4.0f);
            } else {
                (*it)->setMotorSpeed(b2_pi / 4.0f);
            }
        }

        sndDie_->play();

        scene()->camera()->findComponent<CameraComponent>()->tremorStart(0.3f);
    }

    void SnakeComponent::setDie(float headForce, float headTorque, float force, float torque)
    {
        desiredState_ = StateDie1;
        megaShootForce_ = headForce;
        megaShootTorque_ = headTorque;
        dieForce_ = force;
        dieTorque_ = torque;
        parent()->findComponent<PhysicsBodyComponent>()->setFilterGroupIndex(collisionGroupTentacle);
        for (std::set<SceneObjectPtr>::const_iterator it = parent()->objects().begin();
             it != parent()->objects().end(); ++it) {
            (*it)->findComponent<PhysicsBodyComponent>()->setFilterGroupIndex(collisionGroupTentacle);
        }
    }
}
