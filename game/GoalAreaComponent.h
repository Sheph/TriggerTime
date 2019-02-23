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

#ifndef _GOALAREACOMPONENT_H_
#define _GOALAREACOMPONENT_H_

#include "PhasedComponent.h"
#include "SensorListener.h"
#include "GoalIndicatorComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GoalAreaComponent : public boost::enable_shared_from_this<GoalAreaComponent>,
                              public PhasedComponent,
                              public SensorListener
    {
    public:
        GoalAreaComponent();
        ~GoalAreaComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

        void setObject(const SceneObjectPtr& value);
        inline const SceneObjectPtr& object() const { return obj_; }

        void addGoal(const b2Vec2& pos);
        void removeGoal(const b2Vec2& pos);
        void removeAllGoals();

        void addGoal(const SceneObjectPtr& obj, bool aliveOnly);
        void removeGoal(const SceneObjectPtr& obj);

    private:
        struct Goal
        {
            Goal(const b2Vec2& pos)
            : pos(pos)
            {
            }

            Goal(const SceneObjectPtr& obj, bool aliveOnly)
            : obj(obj),
              aliveOnly(aliveOnly)
            {
            }

            b2Vec2 pos;
            SceneObjectPtr obj;
            bool aliveOnly;
        };

        typedef std::vector<Goal> Goals;

        virtual void onRegister();

        virtual void onUnregister();

        bool fade(float dt, bool in);

        Goals goals_;
        SceneObjectPtr obj_;
        GoalIndicatorComponentPtr indicator_;
        bool inside_;
        b2Vec2 goalLastPos_;
    };

    typedef boost::shared_ptr<GoalAreaComponent> GoalAreaComponentPtr;
}

#endif
