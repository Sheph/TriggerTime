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

#ifndef _SHROOMSNAKECOMPONENT_H_
#define _SHROOMSNAKECOMPONENT_H_

#include "PhasedComponent.h"
#include "SensorListener.h"
#include "AnimationComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class ShroomSnakeComponent : public boost::enable_shared_from_this<ShroomSnakeComponent>,
                                 public PhasedComponent,
                                 public SensorListener
    {
    public:
        enum
        {
            AnimationBite = 1,
            AnimationAttack = 2
        };

        ShroomSnakeComponent(const SceneObjectPtr& upperLip,
                             const b2Vec2& upperLipPos,
                             const SceneObjectPtr& lowerLip,
                             const b2Vec2& lowerLipPos,
                             const AnimationComponentPtr& ac,
                             float defAngle,
                             const std::vector<float>& attackAngles);
        ~ShroomSnakeComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr upperLip_;
        b2Vec2 upperLipPos_;
        SceneObjectPtr lowerLip_;
        b2Vec2 lowerLipPos_;
        AnimationComponentPtr ac_;
        float defAngle_;
        std::vector<float> attackAngles_;

        int numPlayerEntered_;

        SceneObjectPtr ally_;
        bool allyBitten_;
    };

    typedef boost::shared_ptr<ShroomSnakeComponent> ShroomSnakeComponentPtr;
}

#endif
