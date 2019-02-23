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

#ifndef _PARTICLEEFFECTCOMPONENT_H_
#define _PARTICLEEFFECTCOMPONENT_H_

#include "RenderComponent.h"
#include "ParticleEmmiter.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class ParticleEffectComponent : public boost::enable_shared_from_this<ParticleEffectComponent>,
                                    public RenderComponent
    {
    public:
        typedef std::vector<ParticleEmmiterPtr> Emmiters;

        explicit ParticleEffectComponent(int zOrder = 0);
        ~ParticleEffectComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        void addEmmiter(const ParticleEmmiterPtr& emmiter);
        inline Emmiters& emmiters() { return emmiters_; }

        void resetEmit();

        void allowCompletion();
        bool isAllowCompletion() const;

        bool isComplete();

        void setDuration(float value);
        float getDuration() const;

        inline void setTransform(const b2Transform& value) { xf_ = value; }
        inline const b2Transform& transform() const { return xf_; }

        inline const b2AABB& aabb() const { return prevAABB_; }

        inline void setFixedAngle(bool value) { fixedAngle_ = value; }
        inline bool fixedAngle() const { return fixedAngle_; }

        inline void setPaused(bool value)
        {
            if (paused_ && !value) {
                first_ = true;
            }
            paused_ = value;
        }
        inline bool paused() const { return paused_; }

        inline void setTimeFactor(int value) { timeFactor_ = value; }
        inline int timeFactor() const { return timeFactor_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        Emmiters emmiters_;

        b2Vec2 prevPos_;
        b2Transform prevXf_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        b2Transform xf_;

        bool fixedAngle_;
        bool paused_;

        int timeFactor_;
        bool first_;
    };

    typedef boost::shared_ptr<ParticleEffectComponent> ParticleEffectComponentPtr;
}

#endif
