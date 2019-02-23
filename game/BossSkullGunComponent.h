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

#ifndef _BOSSSKULLGUNCOMPONENT_H_
#define _BOSSSKULLGUNCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"
#include "WeaponComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BossSkullGunComponent : public boost::enable_shared_from_this<BossSkullGunComponent>,
                                  public PhasedComponent
    {
    public:
        typedef std::vector<WeaponComponentPtr> Weapons;

        BossSkullGunComponent(const Weapons& weapons, float turnSpeed,
            float shootAngle, float shootRadius, const std::string& jointName);
        ~BossSkullGunComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void setTarget(const SceneObjectPtr& value) { target_ = value; }

        inline void setAlwaysFire(bool value) { alwaysFire_ = value; }

        inline const Weapons& weapons() const { return weapons_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr target_;
        bool alwaysFire_;

        Weapons weapons_;
        float turnSpeed_;
        float shootAngle_;
        float shootRadius_;
        std::string jointName_;
        RevoluteJointProxyPtr joint_;

        BehaviorSeekComponentPtr seekBehavior_;
        bool hadTarget_;
    };

    typedef boost::shared_ptr<BossSkullGunComponent> BossSkullGunComponentPtr;
}

#endif
