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

#include "TentacleSquidComponent.h"
#include "TentacleSwingComponent.h"
#include "CollisionCancelComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TentacleSquidComponent::TentacleSquidComponent(bool bottom)
    : TargetableComponent(phaseThink),
      bottom_(bottom),
      t_(0.0f),
      state_(StateIdle),
      melee_(false),
      sndMelee_(audio.createSound("squid_alert3.ogg"))
    {
    }

    TentacleSquidComponent::~TentacleSquidComponent()
    {
    }

    void TentacleSquidComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TentacleSquidComponent::update(float dt)
    {
        t_ -= dt;

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            if (melee_) {
                startMelee();
                break;
            }

            if (!target()) {
                break;
            }

            state_ = StateWalk;
            t_ = getRandom(settings.bossSquid.attackTimeout, 2.0f * settings.bossSquid.attackTimeout);

            break;
        }
        case StateWalk: {
            if (!target()) {
                state_ = StateIdle;
                break;
            }

            if (melee_ || (b2DistanceSquared(target()->pos(), parent()->pos()) <= 20.0f * 20.0f)) {
                if (!melee_) {
                    sndMelee_->play();
                }
                startMelee();
                break;
            }

            if (t_ <= 0.0f) {
                weapon_->triggerOnce();
                state_ = StateIdle;
                break;
            }

            break;
        }
        case StateMelee: {
            if (!melee_ && (!target() || (b2DistanceSquared(target()->pos(), parent()->pos()) >= 21.0f * 21.0f))) {
                state_ = StateIdle;
                for (std::vector<ComponentPtr>::const_iterator it = c_.begin();
                    it != c_.end(); ++it) {
                    (*it)->removeFromParent();
                }
                c_.clear();
                parent()->findComponent<TentacleSwingComponent>()->setPause(false);
                break;
            }

            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void TentacleSquidComponent::onRegister()
    {
        tentacle_ = parent()->findComponent<RenderTentacleComponent>();
    }

    void TentacleSquidComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }

    void TentacleSquidComponent::startMelee()
    {
        parent()->findComponent<TentacleSwingComponent>()->setPause(true);

        state_ = StateMelee;
        float t = getRandom(1.0f, 1.0f);
        float f = getRandom(160000.0f, 160000.0f);

        if (bottom_) {
            f *= -1.0f;
        }

        SceneObjectTypes types;

        types.set(SceneObjectTypePlayer);

        for (std::vector<SceneObjectPtr>::const_iterator it = tentacle_->objects().begin();
            it != tentacle_->objects().end(); ++it) {
            CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
            c->setFilter(types);
            c->setDamage(settings.bossSquid.meleeDamage);
            (*it)->addComponent(c);
            c_.push_back(c);
        }

        TentacleSwingComponentPtr c = boost::make_shared<TentacleSwingComponent>(
            t, EaseInOutQuad, f,
            t, EaseInOutQuad, -f);
        c->setBone(0);
        c->setTweenTime(0.0f);
        parent()->addComponent(c);
        c_.push_back(c);
    }
}
