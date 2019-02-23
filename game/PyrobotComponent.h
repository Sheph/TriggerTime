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

#ifndef _PYROBOTCOMPONENT_H_
#define _PYROBOTCOMPONENT_H_

#include "TargetableComponent.h"
#include "AnimationComponent.h"
#include "SceneObject.h"
#include "WeaponFlamethrowerComponent.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class PyrobotComponent : public boost::enable_shared_from_this<PyrobotComponent>,
                             public TargetableComponent
    {
    public:
        enum
        {
            AnimationWalk = 1,
            AnimationAttack = 2
        };

        PyrobotComponent(const AnimationComponentPtr& ac,
            const b2Transform& weapon1xf, const b2Transform& weapon2xf);
        ~PyrobotComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateAttack,
        };

        virtual void onRegister();

        virtual void onUnregister();

        bool shouldAttack();

        static AudioSourcePtr snd_;

        AnimationComponentPtr ac_;
        WeaponFlamethrowerComponentPtr weapons_[2];

        State state_;

        BehaviorInterceptComponentPtr interceptBehavior_;
        BehaviorAvoidComponentPtr avoidBehavior_;
        BehaviorDetourComponentPtr detourBehavior_;

        bool talked_;
    };

    typedef boost::shared_ptr<PyrobotComponent> PyrobotComponentPtr;
}

#endif
