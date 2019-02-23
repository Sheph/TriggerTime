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

#include "WeaponGGComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "InputManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponGGComponent::WeaponGGComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeGG),
      ggTime_(0)
    {
    }

    WeaponGGComponent::~WeaponGGComponent()
    {
    }

    void WeaponGGComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponGGComponent::update(float dt)
    {
        ggTime_ -= dt;

        if (canceled()) {
            if (gg_) {
                gg_->releaseTarget(false);

                gg_->removeFromParent();
                gg_.reset();
            } else if (ggAim_) {
                ggAim_->removeFromParent();
                ggAim_.reset();
            }

            ggTime_ = settings.player.ggTimeThreshold;

            return;
        }

        if ((ggTime_ <= 0.0f) && triggerHeld()) {
            if (gg_) {
                gg_->releaseTarget(true);

                gg_->removeFromParent();
                gg_.reset();

                ggTime_ = settings.player.ggTimeThreshold;
            } else if (!ggAim_) {
                ggAim_ = boost::make_shared<GravityGunAimComponent>(b2Vec2(0.9f, 0.0f));
                parent()->addComponent(ggAim_);
            }
        } else if (ggAim_) {
            SceneObjectPtr target = ggAim_->target();
            b2Vec2 tmp = ggAim_->hitPoint();

            ggAim_->removeFromParent();
            ggAim_.reset();

            if (target && target->scene() && !target->gravityGunQuietCancel()) {
                gg_ = boost::make_shared<GravityGunComponent>(b2Vec2(0.9f, 0.0f),
                    target,
                    b2Vec2(0.9f + settings.player.ggHoldDistance + (tmp - target->pos()).Length(), 0.0f));
                parent()->addComponent(gg_);
            }
        } else if (gg_) {
            if (!gg_->target() || !gg_->target()->scene() || gg_->target()->gravityGunQuietCancel()) {
                gg_->removeFromParent();
                gg_.reset();

                ggTime_ = settings.player.ggTimeThreshold;
            }
        }
    }

    void WeaponGGComponent::reload()
    {
    }

    SceneObjectPtr WeaponGGComponent::heldObject() const
    {
        if (gg_ && gg_->target() && gg_->target()->active()) {
            return gg_->target();
        } else {
            return SceneObjectPtr();
        }
    }

    void WeaponGGComponent::onRegister()
    {
    }

    void WeaponGGComponent::onUnregister()
    {
        if (ggAim_) {
            ggAim_->removeFromParent();
            ggAim_.reset();
        }
        if (gg_) {
            gg_->removeFromParent();
            gg_.reset();
        }
    }
}
