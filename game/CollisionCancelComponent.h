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

#ifndef _COLLISIONCANCELCOMPONENT_H_
#define _COLLISIONCANCELCOMPONENT_H_

#include "CollisionComponent.h"
#include "SceneObjectManager.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class CollisionCancelComponent : public boost::enable_shared_from_this<CollisionCancelComponent>,
                                     public CollisionComponent
    {
    public:
        CollisionCancelComponent();
        ~CollisionCancelComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preSolve(SInt32 cookie, b2Contact* contact);

        virtual void beginCollision(const Collision& collision);

        virtual void updateCollision(const CollisionUpdate& collisionUpdate);

        virtual void endCollision(SInt32 cookie);

        virtual void update(float dt);

        inline const SceneObjectTypes& filter() const { return typeFilter_; }
        inline void setFilter(const SceneObjectTypes& value) { typeFilter_ = value; }

        void addCookieFilter(SInt32 cookie);

        void script_addObjectFilter(const SceneObjectPtr& obj);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline const AudioSourcePtr& damageSound() const { return sndDamage_; }
        inline void setDamageSound(const AudioSourcePtr& value) { sndDamage_ = value; }

        inline float roll() const { return roll_; }
        inline void setRoll(float amount) { roll_ = amount; }

        inline bool immediate() const { return immediate_; }
        inline void setImmediate(bool value) { immediate_ = value; }

        inline bool noStatic() const { return noStatic_; }
        inline void setNoStatic(bool value) { noStatic_ = value; }

        inline void setCoreProtectorMelee() { coreProtectorMelee_ = true; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        bool check(SceneObject* obj);

        std::set<SInt32> cookies_;

        SceneObjectTypes typeFilter_;

        std::set<SInt32> cookieFilter_;

        float damage_;
        float roll_;
        bool immediate_;

        AudioSourcePtr sndDamage_;
        bool noStatic_;

        bool coreProtectorMelee_;
    };

    typedef boost::shared_ptr<CollisionCancelComponent> CollisionCancelComponentPtr;
}

#endif
