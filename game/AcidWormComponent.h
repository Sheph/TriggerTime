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

#ifndef _ACIDWORMCOMPONENT_H_
#define _ACIDWORMCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class AcidWormComponent : public boost::enable_shared_from_this<AcidWormComponent>,
                              public TargetableComponent
    {
    public:
        enum
        {
            AnimationAttack = 1
        };

        AcidWormComponent(const SceneObjectPtr& head, const SceneObjectPtr& body, bool folded, bool foldable, float unfoldRadius,
            const AnimationComponentPtr& ac);
        ~AcidWormComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

    private:
        enum State
        {
            StateFolded = 0,
            StateIdle,
            StateWalk,
            StateSpit1,
            StateSpit2,
            StatePreFold,
            StateFold1,
            StateFold2,
            StateUnfold1,
            StateUnfold2,
        };

        virtual void onRegister();

        virtual void onUnregister();

        void walk(float dt);

        void setupHealth(bool invulnerable);

        SceneObjectPtr head_;
        SceneObjectPtr body_;
        bool foldable_;
        float unfoldRadius_;
        AnimationComponentPtr ac_;
        bool firstFolded_;
        b2Rot idleRot_;
        float t_;

        State state_;

        WeaponComponentPtr weapon_;

        std::map<SceneObject*, float> sizeMap_;

        TweeningPtr tweening_;

        AudioSourcePtr sndSplash_;
        AudioSourcePtr sndAttack_[2];
    };

    typedef boost::shared_ptr<AcidWormComponent> AcidWormComponentPtr;
}

#endif
