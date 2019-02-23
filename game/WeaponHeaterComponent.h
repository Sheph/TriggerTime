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

#ifndef _WEAPONHEATERCOMPONENT_H_
#define _WEAPONHEATERCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include "LightComponent.h"
#include "LineLight.h"
#include "PointLight.h"
#include "RenderTrailComponent.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponHeaterComponent : public boost::enable_shared_from_this<WeaponHeaterComponent>,
                                  public WeaponComponent
    {
    public:
        explicit WeaponHeaterComponent(SceneObjectType objectType);
        ~WeaponHeaterComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float length() const { return length_; }
        inline void setLength(float value) { length_ = value; }

        inline float impulse() const { return impulse_; }
        inline void setImpulse(float value) { impulse_ = value; }

        inline float duration() const { return duration_; }
        inline void setDuration(float value) { duration_ = value; }

        inline float interval() const { return interval_; }
        inline void setInterval(float value) { interval_ = value; }

        inline float minWidth() const { return minWidth_; }
        inline void setMinWidth(float value) { minWidth_ = value; }

        inline float maxWidth() const { return maxWidth_; }
        inline void setMaxWidth(float value) { maxWidth_ = value; }

        inline bool measure() const { return measure_; }
        inline void setMeasure(bool value) { measure_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
            SceneObject*& obj);

        float damage_;
        float length_;
        float impulse_;
        float duration_;
        float interval_;
        float t_;
        float minWidth_;
        float maxWidth_;
        bool measure_;

        SceneObjectPtr fakeMissile_;
        TweeningPtr tweening_;
        RenderTrailComponentPtr rc_;
        LightComponentPtr lightC_;
        LineLightPtr light_;
        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<WeaponHeaterComponent> WeaponHeaterComponentPtr;
}

#endif
