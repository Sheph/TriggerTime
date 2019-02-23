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

#ifndef _WEAPONPISTOLCOMPONENT_H_
#define _WEAPONPISTOLCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include "RenderQuadComponent.h"
#include "AnimationComponent.h"
#include "PointLight.h"
#include "LightComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponPistolComponent : public boost::enable_shared_from_this<WeaponPistolComponent>,
                                  public WeaponComponent
    {
    public:
        explicit WeaponPistolComponent(SceneObjectType objectType);
        ~WeaponPistolComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float velocity() const { return velocity_; }
        inline void setVelocity(float value) { velocity_ = value; }

        inline float interval() const { return interval_; }
        inline void setInterval(float value) { interval_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float damage_;
        float velocity_;
        float interval_;
        float t_;
        AudioSourcePtr snd_;
        RenderQuadComponentPtr flash_;
        AnimationComponentPtr flashAc_;
        PointLightPtr light_;
        LightComponentPtr lightC_;
    };

    typedef boost::shared_ptr<WeaponPistolComponent> WeaponPistolComponentPtr;
}

#endif
