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

#include "DudeComponent.h"
#include "Utils.h"

namespace af
{
    DudeComponent::DudeComponent(const RenderQuadComponentPtr& legsRc,
        const AnimationComponentPtr& legsAc,
        const AnimationComponentPtr& torsoAc)
    : TargetableComponent(phaseThink),
      legsRc_(legsRc),
      legsAc_(legsAc),
      torsoAc_(torsoAc),
      legsAngle_(0.0f),
      haveGun_(false)
    {
    }

    DudeComponent::~DudeComponent()
    {
    }

    void DudeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void DudeComponent::update(float dt)
    {
        int defAnimation;
        int walkAnimation;

        if (haveGun_) {
            defAnimation = AnimationGunDefault;
            walkAnimation = AnimationGunWalk;
        } else {
            defAnimation = AnimationDefault;
            walkAnimation = AnimationWalk;
        }

        legsRc_->setAngle(parent()->angle() + legsAngle_);

        if (parent()->linearVelocity().LengthSquared() >= 2.0f) {
            if (legsAc_->currentAnimation() != AnimationWalk) {
                legsAc_->startAnimation(AnimationWalk);
            }
            if (torsoAc_) {
                if (torsoAc_->currentAnimation() != walkAnimation) {
                    torsoAc_->startAnimation(walkAnimation);
                }
            }
        } else {
            if (legsAc_->currentAnimation() != AnimationDefault) {
                legsAc_->startAnimation(AnimationDefault);
            }
            if (torsoAc_) {
                if (torsoAc_->currentAnimation() != defAnimation) {
                    torsoAc_->startAnimationRandomTime(defAnimation);
                }
            }
        }
    }

    void DudeComponent::onRegister()
    {
    }

    void DudeComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
