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

#ifndef _WEAPONFLAMETHROWERCOMPONENT_H_
#define _WEAPONFLAMETHROWERCOMPONENT_H_

#include "WeaponComponent.h"
#include "ParticleEffectComponent.h"
#include "AudioManager.h"
#include "LightComponent.h"
#include "ConeLight.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponFlamethrowerComponent : public boost::enable_shared_from_this<WeaponFlamethrowerComponent>,
                                        public WeaponComponent
    {
    public:
        explicit WeaponFlamethrowerComponent(SceneObjectType objectType);
        ~WeaponFlamethrowerComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        virtual void onFreeze();

        virtual void onThaw();

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        void setHaveLight(bool value);
        inline bool haveLight() const { return !!light_; }

        void setScale(float value, float duration);
        inline float scale() const { return scaleTweening_->getValue(scaleTweenTime_); }

        void setColors(const Color& color1, const Color& color2);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction,
            std::map<SceneObject*, float>& objs);

        void applyScale();

        void applyColors();

        ParticleEffectComponentPtr pec_;
        bool stopping_;
        float damageAngle_;
        UInt32 numRays_;
        float damage_;
        AudioSourcePtr snd_;
        bool sndFrozen_;
        LightComponentPtr lightC_;
        ConeLightPtr light_;

        TweeningPtr scaleTweening_;
        float scaleTweenTime_;

        bool captured_;
        float capturedValues_[2];

        std::pair<Color, Color> colors_;
        bool haveColors_;
    };

    typedef boost::shared_ptr<WeaponFlamethrowerComponent> WeaponFlamethrowerComponentPtr;
}

#endif
