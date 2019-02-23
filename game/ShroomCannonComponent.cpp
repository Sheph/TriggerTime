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

#include "ShroomCannonComponent.h"
#include "Settings.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CollisionFilter.h"
#include "Const.h"
#include <boost/make_shared.hpp>

namespace af
{
    ShroomCannonComponent::ShroomCannonComponent(const AnimationComponentPtr& ac,
                                                 const b2Vec2& missilePos)
    : PhasedComponent(phaseThink),
      ac_(ac),
      missilePos_(missilePos),
      numEntered_(0),
      shootTime_(0.0f)
    {
    }

    ShroomCannonComponent::~ShroomCannonComponent()
    {
    }

    void ShroomCannonComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ShroomCannonComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            scene()->addObject(explosion);

            parent()->removeFromParent();

            return;
        }

        shootTime_ -= dt;

        switch (ac_->currentAnimation()) {
        case AnimationAttack:
            if (ac_->animationFinished()) {
                SceneObjectPtr missile = sceneObjectFactory.createShroomCannonRed1Missile(settings.shroomCannon.shootDamage);

                b2Vec2 pos = b2Mul(parent()->getTransform().q, missilePos_);

                missile->setPos(parent()->pos() + pos);
                missile->setLinearVelocity(parent()->getDirection(settings.shroomCannon.missileSpeed));

                CollisionCookieFilterPtr collisionFilter =
                    boost::make_shared<CollisionCookieFilter>();

                collisionFilter->add(parent()->cookie());

                missile->setCollisionFilter(collisionFilter);

                scene()->addObject(missile);

                ac_->startAnimation(AnimationReload);
            }
            return;
        case AnimationReload:
            if (!ac_->animationFinished()) {
                return;
            }
            ac_->startAnimation(AnimationDefault);
            break;
        default:
            break;
        }

        if (numEntered_ <= 0) {
            /*
             * No targets.
             */

            return;
        }

        if (shootTime_ > 0.0f) {
            /*
             * Not yet, wait until timeout.
             */

            return;
        }

        shootTime_ = settings.shroomCannon.shootTimeThreshold;

        ac_->startAnimation(AnimationAttack);
    }

    void ShroomCannonComponent::sensorEnter(const SceneObjectPtr& other)
    {
        ++numEntered_;
    }

    void ShroomCannonComponent::sensorExit(const SceneObjectPtr& other)
    {
        --numEntered_;
    }

    void ShroomCannonComponent::onRegister()
    {
    }

    void ShroomCannonComponent::onUnregister()
    {
    }
}
