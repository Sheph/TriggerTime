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

#ifndef _COLLISIONDAMAGECOMPONENT_H_
#define _COLLISIONDAMAGECOMPONENT_H_

#include "CollisionComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class CollisionDamageComponent : public boost::enable_shared_from_this<CollisionDamageComponent>,
                                     public CollisionComponent
    {
    public:
        CollisionDamageComponent();
        ~CollisionDamageComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void beginCollision(const Collision& collision);

        virtual void updateCollision(const CollisionUpdate& collisionUpdate);

        virtual void endCollision(SInt32 cookie);

        virtual void update(float dt);

        inline float impulseThreshold() const { return impulseThreshold_; }
        inline void setImpulseThreshold(float value) { impulseThreshold_ = value; }

        inline float multiplier() const { return multiplier_; }
        inline void setMultiplier(float value) { multiplier_ = value; }

        inline const AudioSourcePtr& damageSound() const { return sndDamage_; }
        inline void setDamageSound(const AudioSourcePtr& value) { sndDamage_ = value; }

        inline void setFilter(const SceneObjectTypes& value) { typeFilter_ = value; }

        void addCookieFilter(SInt32 cookie);

        void script_addObjectFilter(const SceneObjectPtr& obj);

        inline bool enabled() const { return enabled_; }
        inline void setEnabled(bool value) { enabled_ = value; }

    private:
        struct Info
        {
            Info()
            : multiplier(0.0f),
              eatThat(false)
            {
            }

            Info(float multiplier, bool eatThat)
            : multiplier(multiplier),
              eatThat(eatThat)
            {
            }

            float multiplier;
            bool eatThat;
        };

        virtual void onRegister();

        virtual void onUnregister();

        void damageSelf(float avg, bool eatThat);

        bool enabled_;
        float impulseThreshold_;
        float multiplier_;

        SceneObjectTypes typeFilter_;

        std::set<SInt32> cookieFilter_;

        std::map<SInt32, Info> cookies_;

        AudioSourcePtr sndDamage_;
    };

    typedef boost::shared_ptr<CollisionDamageComponent> CollisionDamageComponentPtr;
}

#endif
