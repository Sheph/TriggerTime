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

#ifndef _WEAPONROPECOMPONENT_H_
#define _WEAPONROPECOMPONENT_H_

#include "WeaponComponent.h"
#include "RopeLaunchComponent.h"
#include "RopePullComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponRopeComponent : public boost::enable_shared_from_this<WeaponRopeComponent>,
                                public WeaponComponent
    {
    public:
        explicit WeaponRopeComponent(SceneObjectType objectType);
        ~WeaponRopeComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float length() const { return length_; }
        inline void setLength(float value) { length_ = value; }

        inline float launchSpeed() const { return launchSpeed_; }
        inline void setLaunchSpeed(float value) { launchSpeed_ = value; }

        inline float launchDistance() const { return launchDistance_; }
        inline void setLaunchDistance(float value) { launchDistance_ = value; }

        inline float pullSpeed() const { return pullSpeed_; }
        inline void setPullSpeed(float value) { pullSpeed_ = value; }

        inline float pullMaxForce() const { return pullMaxForce_; }
        inline void setPullMaxForce(float value) { pullMaxForce_ = value; }

        inline float interval() const { return interval_; }
        inline void setInterval(float value) { interval_ = value; }

        bool hit() const;

        void tear();

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float length_;
        float launchSpeed_;
        float launchDistance_;
        float pullSpeed_;
        float pullMaxForce_;
        float interval_;

        SceneObjectPtr rope_;
        RopeLaunchComponentPtr launcher_;
        RopePullComponentPtr puller_;
        b2Vec2 launchDir_;
        float t_;
        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<WeaponRopeComponent> WeaponRopeComponentPtr;
}

#endif
