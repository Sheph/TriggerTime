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

#ifndef _SENTRYCOMPONENT_H_
#define _SENTRYCOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "AnimationComponent.h"
#include "RenderQuadComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SentryComponent : public boost::enable_shared_from_this<SentryComponent>,
                            public TargetableComponent
    {
    public:
        enum
        {
            AnimationWalk = 1,
            AnimationUnfold = 2,
        };

        SentryComponent(const SceneObjectPtr& torsoObj,
            const RenderQuadComponentPtr& torsoRc,
            const RenderQuadComponentPtr& legsRc,
            const AnimationComponentPtr& torsoAc,
            const AnimationComponentPtr& legsAc,
            bool folded);
        ~SentryComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

        inline void setShootDelay(float value) { shootDelay_ = value; }

        inline void unfold() { unfold_ = true; }

        inline float targetDistance() const { return targetDistance_; }
        inline void setTargetDistance(float value) { targetDistance_ = value; }

    private:
        enum State
        {
            StateFolded = 0,
            StateStand,
            StateWalk,
        };

        virtual void onRegister();

        virtual void onUnregister();

        float standRadiusSq() const;

        float walkRadiusSq() const;

        bool sndPlaying() const;

        SceneObjectPtr torsoObj_;
        RenderQuadComponentPtr torsoRc_;
        RenderQuadComponentPtr legsRc_;
        AnimationComponentPtr torsoAc_;
        AnimationComponentPtr legsAc_;

        WeaponComponentPtr weapon_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        bool unfold_;
        float shootTime_;
        bool readyToShoot_;

        AudioSourcePtr sndUnfold_;
        AudioSourcePtr sndTarget_[2];
        AudioSourcePtr sndTargetLost_;

        AudioSourcePtr sndPatrolStart_[3];
        AudioSourcePtr sndPatrolEnd_;

        float shootDelay_;

        float targetDistance_;
    };

    typedef boost::shared_ptr<SentryComponent> SentryComponentPtr;
}

#endif
