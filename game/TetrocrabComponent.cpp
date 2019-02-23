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

#include "TetrocrabComponent.h"
#include "FadeOutComponent.h"
#include "RenderQuadComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TetrocrabComponent::TetrocrabComponent(int i, const AnimationComponentPtr& ac,
        float walkSpeed, float walkAccel, float turnSpeed)
    : TargetableComponent(phaseThink),
      i_(i),
      ac_(ac),
      walkSpeed_(walkSpeed),
      walkAccel_(walkAccel),
      turnSpeed_(turnSpeed),
      biteTime_(0.0f),
      targetFound_(false),
      sndBite_(audio.createSound("tetrocrab_bite.ogg"))
    {
    }

    TetrocrabComponent::~TetrocrabComponent()
    {
    }

    void TetrocrabComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TetrocrabComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr gibbed = sceneObjectFactory.createTetrocrabGibbed(i_);

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(5.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            gibbed->setLinearVelocity(velDir);

            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

            gibbed->addComponent(component);

            SceneObjectPtr obj2 = sceneObjectFactory.createBlood1(3.0f, zOrderExplosion);

            obj2->setPos(gibbed->pos());
            obj2->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(obj2);

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(velDir);

                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

                (*it)->addComponent(component);
            }

            scene()->addObject(gibbed);

            SceneObjectPtr bs = sceneObjectFactory.createBloodStain1(3.0f, zOrderBack, parent()->activeDeadbody());

            bs->setPos(gibbed->pos());
            bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

            scene()->addObject(bs);

            audio.playSound("tetrocrab_crack.ogg");

            scene()->spawnPowerup(parent()->pos());

            scene()->stats()->incEnemiesKilled();

            parent()->removeFromParent();

            return;
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

        updateAutoTarget(dt);

        if (ac_->animationFinished()) {
            ac_->startAnimation(AnimationDefault);
        }

        if (parent()->stunned()) {
            ac_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        biteTime_ -= dt;

        if (!target()) {
            ac_->setPaused(true);
            targetFound_ = false;
            interceptBehavior_->reset();
            avoidBehavior_->reset();
            detourBehavior_->reset();
            return;
        }

        if (!targetFound_) {
            targetFound_ = true;

            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(turnSpeed_);
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(2.0f);
            avoidBehavior_->setLinearVelocity(walkSpeed_);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(5.0f);
            detourBehavior_->setAngularVelocity(turnSpeed_);
            detourBehavior_->setTimeout(0.1f);
            detourBehavior_->start();
        }

        bool detouring = !detourBehavior_->finished();

        if (!detouring && (b2DistanceSquared(parent()->pos(), target()->pos()) > (5.0f * 5.0f))) {
            detourBehavior_->start();
        }

        ac_->setPaused(false);

        if (detouring) {
            interceptBehavior_->setTarget(SceneObjectPtr());
        } else {
            interceptBehavior_->setTarget(target());
        }

        if (parent()->linearVelocity().Length() < walkSpeed_) {
            parent()->applyForceToCenter(parent()->mass() *
                parent()->getDirection(walkSpeed_ * walkAccel_ + parent()->linearVelocityDamped()), true);
        }

        /*
         * Bite :)
         */

        if ((biteTime_ <= 0.0f) &&
            (b2DistanceSquared(parent()->pos(), target()->pos()) <= (2.1f * 2.1f)) &&
            (fabs(angleBetween(parent()->getDirection(1.0f), target()->pos() - parent()->pos())) <= deg2rad(45))) {
            target()->changeLife(-settings.tetrocrab.biteDamage);

            sndBite_->play();

            biteTime_ = settings.tetrocrab.biteTime;

            ac_->startAnimation(AnimationBite);
        }
    }

    void TetrocrabComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void TetrocrabComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        ac_.reset();
        interceptBehavior_.reset();
        avoidBehavior_.reset();
        detourBehavior_.reset();
    }
}
