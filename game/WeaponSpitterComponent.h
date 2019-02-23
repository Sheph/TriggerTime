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

#ifndef _WEAPONSPITTERCOMPONENT_H_
#define _WEAPONSPITTERCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponSpitterComponent : public boost::enable_shared_from_this<WeaponSpitterComponent>,
                                   public WeaponComponent
    {
    public:
        explicit WeaponSpitterComponent(SceneObjectType objectType);
        ~WeaponSpitterComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float velocity() const { return velocity_; }
        inline void setVelocity(float value) { velocity_ = value; }

        inline float spreadAngle() const { return spreadAngle_; }
        inline void setSpreadAngle(float value) { spreadAngle_ = value; }

        inline bool spreadRandom() const { return spreadRandom_; }
        inline void setSpreadRandom(bool value) { spreadRandom_ = value; }

        inline UInt32 minShots() const { return minShots_; }
        inline void setMinShots(UInt32 value) { minShots_ = value; }

        inline UInt32 maxShots() const { return maxShots_; }
        inline void setMaxShots(UInt32 value) { maxShots_ = value; }

        inline UInt32 totalShots() const { return totalShots_; }
        inline void setTotalShots(UInt32 value) { totalShots_ = value; }

        inline float shotInterval() const { return shotInterval_; }
        inline void setShotInterval(float value) { shotInterval_ = value; }

        inline float interval() const { return interval_; }
        inline void setInterval(float value) { interval_ = value; }

        inline const Color& minColor() const { return minColor_; }
        inline void setMinColor(const Color& value) { minColor_ = value; }

        inline const Color& maxColor() const { return maxColor_; }
        inline void setMaxColor(const Color& value) { maxColor_ = value; }

        inline float minHeight() const { return minHeight_; }
        inline void setMinHeight(float value) { minHeight_ = value; }

        inline float maxHeight() const { return maxHeight_; }
        inline void setMaxHeight(float value) { maxHeight_ = value; }

        inline int I() const { return i_; }
        inline void setI(int value) { i_ = value; }

        inline const AudioSourcePtr& sound() const { return snd_; }
        inline void setSound(const AudioSourcePtr& value) { snd_ = value; }

        inline bool tweakPos() const { return tweakPos_; }
        inline void setTweakPos(bool value) { tweakPos_ = value; }

        inline const b2Vec2& tweakOffset() const { return tweakOffset_; }
        inline void setTweakOffset(const b2Vec2& value) { tweakOffset_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void createShot(const b2Transform& xf, const b2Rot& rot);

        float damage_;
        float velocity_;
        float spreadAngle_;
        bool spreadRandom_;
        UInt32 minShots_;
        UInt32 maxShots_;
        UInt32 totalShots_;
        float shotInterval_;
        float interval_;
        Color minColor_;
        Color maxColor_;
        float minHeight_;
        float maxHeight_;
        int i_;

        UInt32 curShot_;
        float t_;

        AudioSourcePtr snd_;
        bool tweakPos_;
        b2Vec2 tweakOffset_;
    };

    typedef boost::shared_ptr<WeaponSpitterComponent> WeaponSpitterComponentPtr;
}

#endif
