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

#ifndef _WEAPONBLASTERCOMPONENT_H_
#define _WEAPONBLASTERCOMPONENT_H_

#include "WeaponComponent.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class WeaponBlasterComponent : public boost::enable_shared_from_this<WeaponBlasterComponent>,
                                   public WeaponComponent
    {
    public:
        WeaponBlasterComponent(bool super, SceneObjectType objectType);
        ~WeaponBlasterComponent();

        virtual void reload();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline float damage() const { return damage_; }
        inline void setDamage(float value) { damage_ = value; }

        inline float velocity() const { return velocity_; }
        inline void setVelocity(float value) { velocity_ = value; }

        inline UInt32 turns() const { return turns_; }
        inline void setTurns(UInt32 value) { turns_ = value; }

        inline UInt32 shotsPerTurn() const { return shotsPerTurn_; }
        inline void setShotsPerTurn(UInt32 value) { shotsPerTurn_ = value; }

        inline float turnInterval() const { return turnInterval_; }
        inline void setTurnInterval(float value) { turnInterval_ = value; }

        inline float loopDelay() const { return loopDelay_; }
        inline void setLoopDelay(float value) { loopDelay_ = value; }

        inline bool tweakPos() const { return tweakPos_; }
        inline void setTweakPos(bool value) { tweakPos_ = value; }

        inline const b2Vec2& tweakOffset() const { return tweakOffset_; }
        inline void setTweakOffset(const b2Vec2& value) { tweakOffset_ = value; }

        inline float spreadAngle() const { return spreadAngle_; }
        inline void setSpreadAngle(float value) { spreadAngle_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float damage_;
        float velocity_;
        UInt32 turns_;
        UInt32 shotsPerTurn_;
        float turnInterval_;
        float loopDelay_;
        bool tweakPos_;
        b2Vec2 tweakOffset_;
        float spreadAngle_;

        UInt32 curTurn_;
        float t_;

        AudioSourcePtr snd_;
    };

    typedef boost::shared_ptr<WeaponBlasterComponent> WeaponBlasterComponentPtr;
}

#endif
