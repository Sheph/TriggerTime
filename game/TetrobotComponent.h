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

#ifndef _TETROBOTCOMPONENT_H_
#define _TETROBOTCOMPONENT_H_

#include "TargetableComponent.h"
#include "SceneObject.h"
#include "AudioManager.h"
#include "RenderDottedLineComponent.h"
#include "BehaviorAvoidComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class TetrobotComponent : public boost::enable_shared_from_this<TetrobotComponent>,
                              public TargetableComponent
    {
    public:
        explicit TetrobotComponent(const b2Vec2& bulletPos);
        ~TetrobotComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

    private:
        enum State
        {
            StateIdle = 0,
            StateTrack,
            StateCharge,
            StateAttack,
        };

        virtual void onRegister();

        virtual void onUnregister();

        void checkWall(const b2Vec2& dir);

        float wallRayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, float& finalFraction);

        float targetRayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint);

        SceneObjectPtr fakeMissile_;

        b2Vec2 bulletPos_;
        State state_;
        float stateTime_;
        bool rotateDir_;

        RenderDottedLineComponentPtr dottedLine_;
        ComponentPtr laser_;

        BehaviorAvoidComponentPtr avoidBehavior_;

        AudioSourcePtr sndCharge_;
        AudioSourcePtr sndAttack_;
    };

    typedef boost::shared_ptr<TetrobotComponent> TetrobotComponentPtr;
}

#endif
