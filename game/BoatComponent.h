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

#ifndef _BOATCOMPONENT_H_
#define _BOATCOMPONENT_H_

#include "PhasedComponent.h"
#include "WeaponComponent.h"
#include "JointProxy.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BoatComponent : public boost::enable_shared_from_this<BoatComponent>,
                          public PhasedComponent
    {
    public:
        BoatComponent(const SceneObjectPtr& gun,
            const std::string& gunJointName);
        ~BoatComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline bool active() const { return active_; }
        void setActive(bool value);

        inline const WeaponComponentPtr& weapon() const { return weapon_; }
        inline void setWeapon(const WeaponComponentPtr& value) { weapon_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr gun_;
        std::string gunJointName_;
        RevoluteJointProxyPtr joint_;

        WeaponComponentPtr weapon_;

        SceneObjectPtr prevPlayer_;
        JointProxyPtr playerJoint_;

        bool active_;

        AudioSourcePtr sndIdle_;
        AudioSourcePtr sndStart_;
        AudioSourcePtr sndStop_;
        AudioSourcePtr sndMove_;
    };

    typedef boost::shared_ptr<BoatComponent> BoatComponentPtr;
}

#endif
