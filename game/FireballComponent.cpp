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

#include "FireballComponent.h"
#include "ParticleEffectComponent.h"
#include "LightComponent.h"
#include "SingleTweening.h"
#include "Scene.h"
#include <boost/make_shared.hpp>

namespace af
{
    FireballComponent::FireballComponent(const CollisionDieComponentPtr& cdc,
                                         const AnimationComponentPtr& ac,
                                         float damage)
    : PhasedComponent(phaseThink),
      cdc_(cdc),
      ac_(ac),
      damage_(damage),
      boom_(false),
      tweenTime_(0.0f),
      snd_(audio.createSound("fireball_shoot.ogg")),
      sndHit_(audio.createSound("fireball_boom.ogg")),
      haveSound_(true),
      hitHaveSound_(true)
    {
    }

    FireballComponent::~FireballComponent()
    {
    }

    void FireballComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void FireballComponent::update(float dt)
    {
        if (boom_) {
            if (tweening_) {
                float value = tweening_->getValue(tweenTime_);

                Color c = light_->color();

                c.rgba[3] = value;

                light_->setColor(c);

                if (tweening_->finished(tweenTime_)) {
                    tweening_.reset();
                }

                tweenTime_ += dt;
            }

            if (ac_->animationFinished()) {
                parent()->removeFromParent();
            }

            return;
        }

        if (cdc_->collidedWith()) {
            cdc_->collidedWith()->changeLife2(parent(), -damage_);

            ac_->startAnimation(AnimationBoom);

            ParticleEffectComponentPtr peffect =
                parent()->findComponent<ParticleEffectComponent>();

            if (peffect) {
                /*
                 * Let particle effect finish nicely.
                 */
                peffect->allowCompletion();
            }

            LightComponentPtr lc = parent()->findComponent<LightComponent>();

            if (lc && !lc->lights().empty()) {
                /*
                 * Let light go darker.
                 */
                light_ = lc->lights().front();

                tweening_ = boost::make_shared<SingleTweening>(ac_->animationDuration(),
                    EaseOutQuad, light_->color().rgba[3], 0.0f);
                tweenTime_ = 0.0f;
            }

            boom_ = true;

            if (hitHaveSound_) {
                sndHit_->play();
            }
        }
    }

    void FireballComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void FireballComponent::onRegister()
    {
        if (haveSound_) {
            snd_->play();
        }
    }

    void FireballComponent::onUnregister()
    {
    }
}
