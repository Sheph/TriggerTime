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

#include "FadeOutComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"
#include "SceneObject.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    FadeOutComponent::FadeOutComponent(float duration, float delay)
    : PhasedComponent(phasePreRender),
      tweenTime_(0.0f),
      lightsOnly_(false)
    {
        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        if (delay > 0.0f) {
            tweening->addTweening(boost::make_shared<SingleTweening>(delay, EaseLinear, 1.0f, 1.0f));
        }
        tweening->addTweening(boost::make_shared<SingleTweening>(duration, EaseOutQuad, 1.0f, 0.0f));

        tweening_ = tweening;
    }

    FadeOutComponent::~FadeOutComponent()
    {
    }

    void FadeOutComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void FadeOutComponent::preRender(float dt)
    {
        if (!tweening_) {
            return;
        }

        float value = tweening_->getValue(tweenTime_);

        if (!lightsOnly_) {
            for (std::map<RenderComponentPtr, float>::const_iterator it = rc_.begin();
                 it != rc_.end(); ++it) {
                Color c = it->first->color();
                c.rgba[3] = it->second * value;
                it->first->setColor(c);
            }
        }

        for (std::map<LightPtr, float>::const_iterator it = lights_.begin();
             it != lights_.end(); ++it) {
            Color c = it->first->color();
            c.rgba[3] = it->second * value;
            it->first->setColor(c);
        }

        tweenTime_ += dt;
        if (tweening_->finished(tweenTime_)) {
            tweening_.reset();
            if (lightsOnly_) {
                removeFromParent();
            } else {
                parent()->removeFromParent();
            }
        }
    }

    void FadeOutComponent::onRegister()
    {
        for (std::vector<ComponentPtr>::const_iterator it = parent()->components().begin();
             it != parent()->components().end();
             ++it ) {
            const RenderComponentPtr& rc = boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                rc_[rc] = rc->color().rgba[3];
            }
            const LightComponentPtr& lc = boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    lights_[*jt] = (*jt)->color().rgba[3];
                }
            }
        }
    }

    void FadeOutComponent::onUnregister()
    {
        rc_.clear();
        lights_.clear();
    }
}
