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

#ifndef _WEAPONPROXMINECOMPONENT_H_
#define _WEAPONPROXMINECOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponProxMineComponent : public boost::enable_shared_from_this<WeaponProxMineComponent>,
                                    public WeaponComponent
    {
    public:
        explicit WeaponProxMineComponent(SceneObjectType objectType);
        ~WeaponProxMineComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float armTimeout() const { return armTimeout_; }
        inline void setArmTimeout(float value) { armTimeout_ = value; }

        inline float activationRadius() const { return activationRadius_; }
        inline void setActivationRadius(float value) { activationRadius_ = value; }

        inline float activationTimeout() const { return activationTimeout_; }
        inline void setActivationTimeout(float value) { activationTimeout_ = value; }

        inline float explosionTimeout() const { return explosionTimeout_; }
        inline void setExplosionTimeout(float value) { explosionTimeout_ = value; }

        inline float explosionImpulse() const { return explosionImpulse_; }
        inline void setExplosionImpulse(float value) { explosionImpulse_ = value; }

        inline float explosionDamage() const { return explosionDamage_; }
        inline void setExplosionDamage(float value) { explosionDamage_ = value; }

        inline float interval() const { return interval_; }
        inline void setInterval(float value) { interval_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float armTimeout_;
        float activationRadius_;
        float activationTimeout_;
        float explosionTimeout_;
        float explosionImpulse_;
        float explosionDamage_;
        float interval_;
        float t_;
        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<WeaponProxMineComponent> WeaponProxMineComponentPtr;
}

#endif
