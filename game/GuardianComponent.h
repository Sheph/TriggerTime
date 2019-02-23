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

#ifndef _GUARDIANCOMPONENT_H_
#define _GUARDIANCOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GuardianComponent : public boost::enable_shared_from_this<GuardianComponent>,
                              public TargetableComponent
    {
    public:
        GuardianComponent(float jumpVelocity, float shootMinTimeout, float shootMaxTimeout, float jumpTimeout);
        ~GuardianComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
        };

        virtual void onRegister();

        virtual void onUnregister();

        float jumpVelocity_;
        float shootMinTimeout_;
        float shootMaxTimeout_;
        float jumpTimeout_;

        WeaponComponentPtr weapon_;

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        float aimT_;
        float jumpT_;
        float shootT_;
        bool wasDetouring_;

        b2Rot idleRot_;
        int idleRotDir_;
    };

    typedef boost::shared_ptr<GuardianComponent> GuardianComponentPtr;
}

#endif
