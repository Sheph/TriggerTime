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

#ifndef _SEEKERCOMPONENT_H_
#define _SEEKERCOMPONENT_H_

#include "TargetableComponent.h"
#include "CollisionDieComponent.h"
#include "ParticleEffectComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SeekerComponent : public boost::enable_shared_from_this<SeekerComponent>,
                            public TargetableComponent
    {
    public:
        SeekerComponent(const CollisionDieComponentPtr& cdc,
            const ParticleEffectComponentPtr& pec,
            float explosionImpulse, float explosionDamage, const SceneObjectTypes& damageTypes);
        ~SeekerComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void onFreeze();

        void setStartPos(const b2Vec2& value) { haveStartPos_ = true; startPos_ = value; }

        inline float seekVelocity() const { return seekVelocity_; }
        inline void setSeekVelocity(float value) { seekVelocity_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        CollisionDieComponentPtr cdc_;
        ParticleEffectComponentPtr pec_;
        float explosionImpulse_;
        float explosionDamage_;
        SceneObjectTypes damageTypes_;

        bool haveStartPos_;
        b2Vec2 startPos_;
        b2Vec2 actualLinearVelocity_;
        bool wasFreezable_;
        float linearVelocityLen_;
        float seekVelocity_;
    };

    typedef boost::shared_ptr<SeekerComponent> SeekerComponentPtr;
}

#endif
