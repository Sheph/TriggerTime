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

#ifndef _BABYSPIDERCOMPONENT_H_
#define _BABYSPIDERCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "RenderQuadComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BabySpiderComponent : public boost::enable_shared_from_this<BabySpiderComponent>,
                                public TargetableComponent
    {
    public:
        enum
        {
            AnimationDie = 1,
        };

        BabySpiderComponent(const AnimationComponentPtr& ac, const RenderQuadComponentPtr& rc,
            float walkSpeed, float biteDamage);
        ~BabySpiderComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline void setTrapped() { trapped_ = true; }

        inline bool spawnPowerup() const { return spawnPowerup_; }
        inline void setSpawnPowerup(bool value) { spawnPowerup_ = value; }

        inline void setHaveSound(bool value) { haveSound_ = value; }
        inline bool haveSound() const { return haveSound_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        AnimationComponentPtr ac_;
        RenderQuadComponentPtr rc_;
        float walkSpeed_;
        float biteDamage_;
        float biteTime_;
        bool trapped_;
        bool spawnPowerup_;
        bool haveSound_;

        bool targetFound_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        AudioSourcePtr sndBite_;
        AudioSourcePtr sndDie_;
    };

    typedef boost::shared_ptr<BabySpiderComponent> BabySpiderComponentPtr;
}

#endif
