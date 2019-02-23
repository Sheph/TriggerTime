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

#include "SpiderNestComponent.h"
#include "PhysicsBodyComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Const.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    SpiderNestComponent::SpiderNestComponent(const AnimationComponentPtr& ac,
        const b2Transform spawn[3],
        float radius, const float probabilities[1])
    : TargetableComponent(phaseThink),
      ac_(ac),
      radius_(radius),
      dead_(false),
      sndCrack_(audio.createSound("spidernest_crack.ogg"))
    {
        std::copy(&spawn[0], &spawn[0] + 3, &spawn_[0]);
        std::copy(&probabilities[0], &probabilities[0] + 1, &probabilities_[0]);
    }

    SpiderNestComponent::~SpiderNestComponent()
    {
    }

    void SpiderNestComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SpiderNestComponent::update(float dt)
    {
        if ((parent()->life() <= 0) || dead_) {
            if (ac_->currentAnimation() == AnimationDefault) {
                ac_->startAnimation(AnimationDie);
                if (!dead_) {
                    sndCrack_->play();
                }
                return;
            }

            if (!ac_->animationFinished()) {
                return;
            }

            ac_->removeFromParent();

            parent()->findComponent<PhysicsBodyComponent>()->removeFromParent();

            if (!dead_) {
                for (int i = 0; i < 3; ++i) {
                    SceneObjectPtr spider;

                    if (getRandom(0.0f, 1.0f) <= probabilities_[0]) {
                        spider = sceneObjectFactory.createBabySpider1();
                    } else {
                        spider = sceneObjectFactory.createBabySpider2();
                    }

                    spider->setTransform(b2Mul(parent()->getTransform(), spawn_[i]));
                    TargetableComponentPtr targetable = spider->findComponent<TargetableComponent>();
                    TargetableComponentPtr parentTargetable = parent()->findComponent<TargetableComponent>();
                    targetable->setTarget(parentTargetable->target());
                    targetable->setAutoTarget(parentTargetable->autoTarget());

                    scene()->addObject(spider);
                }
            }

            scene()->stats()->incEnemiesKilled();

            removeFromParent();

            return;
        }

        updateAutoTarget(dt);

        if (!target()) {
            return;
        }

        if (b2DistanceSquared(parent()->pos(), target()->pos()) <= radius_ * radius_) {
            parent()->changeLife(-parent()->life());
        }
    }

    void SpiderNestComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();
    }

    void SpiderNestComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
