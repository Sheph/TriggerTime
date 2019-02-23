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

#ifndef _SPIDERNESTCOMPONENT_H_
#define _SPIDERNESTCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SpiderNestComponent : public boost::enable_shared_from_this<SpiderNestComponent>,
                                public TargetableComponent
    {
    public:
        enum
        {
            AnimationDie = 1,
        };

        SpiderNestComponent(const AnimationComponentPtr& ac,
            const b2Transform spawn[3],
            float radius, const float probabilities[1]);
        ~SpiderNestComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline void setDead() { dead_ = true; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        AnimationComponentPtr ac_;
        b2Transform spawn_[3];
        float radius_;
        float probabilities_[1];
        bool dead_;

        AudioSourcePtr sndCrack_;
    };

    typedef boost::shared_ptr<SpiderNestComponent> SpiderNestComponentPtr;
}

#endif
