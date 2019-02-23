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

#ifndef _MECHCOMPONENT_H_
#define _MECHCOMPONENT_H_

#include "PhasedComponent.h"
#include "WeaponComponent.h"
#include "RenderQuadComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "Light.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class MechComponent : public boost::enable_shared_from_this<MechComponent>,
                          public PhasedComponent
    {
    public:
        enum
        {
            AnimationMelee2 = 1,
            AnimationPreGun,
            AnimationGun,
            AnimationPostGun
        };

        MechComponent(const SceneObjectPtr& torsoObj,
            const RenderQuadComponentPtr& torsoRc,
            const RenderQuadComponentPtr& legsRc,
            const AnimationComponentPtr& torsoAc,
            const AnimationComponentPtr& legsAc,
            const Points& melee2Points,
            float melee2Duration,
            const std::vector<LightPtr>& fireLights);
        ~MechComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void debugDraw();

        inline bool active() const { return active_; }
        void setActive(bool value);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

    private:
        enum State
        {
            StateIdle = 0,
            StateMelee2,
            StateMelee2Hit,
            StateMelee2Final,
            StatePreGun,
            StateGun,
            StatePostGun
        };

        virtual void onRegister();

        virtual void onUnregister();

        b2Vec2 calcMelee2Point() const;

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction,
            std::map<SceneObject*, b2Vec2>& objs);

        void walkSound();

        SceneObjectPtr torsoObj_;
        RenderQuadComponentPtr torsoRc_;
        RenderQuadComponentPtr legsRc_;
        AnimationComponentPtr torsoAc_;
        AnimationComponentPtr legsAc_;
        Points melee2Points_;
        float melee2Duration_;
        std::vector<LightPtr> fireLights_;

        WeaponComponentPtr weapon_;

        bool active_;

        State state_;

        size_t melee2Index_;
        float t_;

        AudioSourcePtr sndWalk_;
        bool walk1Sounded_;
        AudioSourcePtr sndMelee2_;

        bool dying_;
        float fireTime_[3];
        bool dead_;
    };

    typedef boost::shared_ptr<MechComponent> MechComponentPtr;
}

#endif
