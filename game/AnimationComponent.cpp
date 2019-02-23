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

#include "AnimationComponent.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Logger.h"
#include "AssetManager.h"
#include "af/Utils.h"

namespace af
{
    AnimationComponent::AnimationComponent(const DrawablePtr& drawable)
    : PhasedComponent(phasePreRender),
      curAnimation_(AnimationNone),
      animationTime_(0.0f),
      drawable_(drawable),
      paused_(false)
    {
    }

    AnimationComponent::~AnimationComponent()
    {
    }

    void AnimationComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void AnimationComponent::preRender(float dt)
    {
        Animations::const_iterator it = animations_.find(curAnimation_);

        if (it == animations_.end()) {
            return;
        }

        if (!paused_) {
            Factors::const_iterator jt = factors_.find(curAnimation_);

            float factor = 1.0f;

            if (jt != factors_.end()) {
                factor = jt->second;
            }

            animationTime_ += dt * factor;
        }

        if (drawable_) {
            drawable_->setImage(it->second.getFrame(animationTime_));
        }
    }

    void AnimationComponent::addAnimation(int id, const std::string& animation, float factor)
    {
        animations_.insert(std::make_pair(id, assetManager.getAnimation(animation, factor)));
    }

    void AnimationComponent::startAnimation(int id)
    {
        if (id == AnimationNone) {
            curAnimation_ = id;
            animationTime_ = 0.0f;

            return;
        }

        Animations::const_iterator it = animations_.find(id);

        if (it == animations_.end()) {
            LOG4CPLUS_ERROR(logger(), "Animation not found - " << id);
            return;
        }

        curAnimation_ = id;
        animationTime_ = 0.0f;
    }

    void AnimationComponent::startAnimationRandomTime(int id)
    {
        Animations::const_iterator it = animations_.find(id);

        if (it == animations_.end()) {
            LOG4CPLUS_ERROR(logger(), "Animation not found - " << id);
            return;
        }

        curAnimation_ = id;
        animationTime_ = getRandom(0.0f, it->second.duration());
    }

    bool AnimationComponent::animationFinished() const
    {
        Animations::const_iterator it = animations_.find(curAnimation_);

        if (it == animations_.end()) {
            return true;
        }

        return it->second.finished(animationTime_);
    }

    int AnimationComponent::animationFrameIndex() const
    {
        Animations::const_iterator it = animations_.find(curAnimation_);

        if (it == animations_.end()) {
            return -1;
        }

        return it->second.getFrameIndex(animationTime_);
    }

    int AnimationComponent::animationSpecialIndex(int i) const
    {
        Animations::const_iterator it = animations_.find(curAnimation_);

        if (it == animations_.end()) {
            return -1;
        }

        return it->second.getSpecialIndex(i);
    }

    float AnimationComponent::animationDuration() const
    {
        Animations::const_iterator it = animations_.find(curAnimation_);

        if (it == animations_.end()) {
            return 0.0f;
        }

        return it->second.duration();
    }

    void AnimationComponent::script_addAnimationForceLoop(int id, const std::string& animation, float factor)
    {
        Animation anim = assetManager.getAnimation(animation, factor);

        anim.setLoop(true);

        animations_.insert(std::make_pair(id, anim));
    }

    void AnimationComponent::setAnimationFactor(int id, float factor)
    {
        factors_[id] = factor;
    }

    void AnimationComponent::onRegister()
    {
    }

    void AnimationComponent::onUnregister()
    {
    }
}
