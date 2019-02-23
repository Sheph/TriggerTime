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

#ifndef _BUDDYSIDECOMPONENT_H_
#define _BUDDYSIDECOMPONENT_H_

#include "PhasedComponent.h"
#include "AudioManager.h"
#include "WeaponComponent.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class BuddySideComponent : public boost::enable_shared_from_this<BuddySideComponent>,
                               public PhasedComponent
    {
    public:
        struct Layer
        {
            Layer()
            {
            }

            Layer(const SceneObjectPtr& head,
                const SceneObjectPtr& upperElbow,
                const SceneObjectPtr& lowerElbow,
                const SceneObjectPtr& upperArm,
                const SceneObjectPtr& lowerArm)
            : head(head)
            {
                elbow[0] = upperElbow;
                elbow[1] = lowerElbow;
                arm[0] = upperArm;
                arm[1] = lowerArm;
            }

            Layer(const SceneObjectPtr& head,
                const SceneObjectPtr& upperElbow,
                const SceneObjectPtr& lowerElbow,
                const SceneObjectPtr& upperArm,
                const SceneObjectPtr& lowerArm,
                const SceneObjectPtr& upperElbowS,
                const SceneObjectPtr& lowerElbowS,
                const SceneObjectPtr& upperArmS,
                const SceneObjectPtr& lowerArmS)
            : head(head)
            {
                elbow[0] = upperElbow;
                elbow[1] = lowerElbow;
                arm[0] = upperArm;
                arm[1] = lowerArm;
                elbowS[0] = upperElbowS;
                elbowS[1] = lowerElbowS;
                armS[0] = upperArmS;
                armS[1] = lowerArmS;
            }

            SceneObjectPtr head;
            SceneObjectPtr elbow[2];
            SceneObjectPtr arm[2];
            SceneObjectPtr elbowS[2];
            SceneObjectPtr armS[2];
            b2Rot armRot[2];
            int armRotDir[2];
        };

        BuddySideComponent(int index, const Layer layers[8]);
        ~BuddySideComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void setTarget(const SceneObjectPtr& value) { target_ = value; }
        inline const SceneObjectPtr& target() const { return target_; }

        void setDeadFinal();

    private:
        enum State
        {
            StateIdle = 0,
            StateWalk,
            StateMelee1,
            StateMelee2,
            StateOneSideMelee1,
            StateOneSideMelee2,
            StateDead
        };

        virtual void onRegister();

        virtual void onUnregister();

        void resurrect();

        void enableHands(bool enable);

        SceneObjectPtr target_;

        int index_;
        Layer layers_[8];

        State state_;

        float t_;

        b2Rot idleRot_;

        int whichSide_;
        bool deadFinal_;

        bool whooshed_;
        float prevHealth_;
        float painT_;

        AudioSourcePtr sndMelee_[2];
        AudioSourcePtr sndWhoosh_[2];
        AudioSourcePtr sndPain_[2];
        AudioSourcePtr sndDie_;
        AudioSourcePtr sndResurrect_;
    };

    typedef boost::shared_ptr<BuddySideComponent> BuddySideComponentPtr;
}

#endif
