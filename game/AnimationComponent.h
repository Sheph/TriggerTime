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

#ifndef _ANIMATIONCOMPONENT_H_
#define _ANIMATIONCOMPONENT_H_

#include "PhasedComponent.h"
#include "Animation.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>
#include <map>

namespace af
{
    enum
    {
        AnimationNone = -1,
        AnimationDefault = 0
    };

    class AnimationComponent : public boost::enable_shared_from_this<AnimationComponent>,
                               public PhasedComponent
    {
    public:
        explicit AnimationComponent(const DrawablePtr& drawable = DrawablePtr());
        ~AnimationComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        void addAnimation(int id, const std::string& animation, float factor = 1.0f);

        void startAnimation(int id);
        void startAnimationRandomTime(int id);

        inline int currentAnimation() const { return curAnimation_; }

        bool animationFinished() const;

        int animationFrameIndex() const;

        int animationSpecialIndex(int i) const;

        float animationDuration() const;

        inline void setDrawable(const DrawablePtr& value) { drawable_ = value; }
        inline DrawablePtr drawable() const { return drawable_; }

        inline void setPaused(bool value) { paused_ = value; }
        inline bool paused() const { return paused_; }

        void script_addAnimationForceLoop(int id, const std::string& animation, float factor = 1.0f);

        void setAnimationFactor(int id, float factor);

    private:
        typedef std::map<int, Animation> Animations;
        typedef std::map<int, float> Factors;

        virtual void onRegister();

        virtual void onUnregister();

        Animations animations_;
        Factors factors_;
        int curAnimation_;
        float animationTime_;

        DrawablePtr drawable_;

        bool paused_;
    };

    typedef boost::shared_ptr<AnimationComponent> AnimationComponentPtr;
}

#endif
