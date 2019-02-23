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

#ifndef _PUZZLEHINTAREACOMPONENT_H_
#define _PUZZLEHINTAREACOMPONENT_H_

#include "PhasedComponent.h"
#include "SensorListener.h"
#include "PuzzleHintIndicatorComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class PuzzleHintAreaComponent : public boost::enable_shared_from_this<PuzzleHintAreaComponent>,
                                    public PhasedComponent,
                                    public SensorListener
    {
    public:
        PuzzleHintAreaComponent();
        ~PuzzleHintAreaComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

        inline float timeout() const { return t_; }
        inline void setTimeout(float value) { t_ = value; }

        void setObject(const SceneObjectPtr& value);
        inline const SceneObjectPtr& object() const { return obj_; }

        void addHintTrigger(const b2Vec2& pos, float radius);

        void addHint(const b2Vec2& pos);
        void removeHint(const b2Vec2& pos);
        void removeAllHints();

    private:
        struct Hint
        {
            Hint()
            {
            }

            Hint(const b2Vec2& pos)
            : pos(pos),
              removing(false),
              alpha(0.0f)
            {
            }

            b2Vec2 pos;
            SceneObjectPtr obj;
            bool removing;
            float alpha;
        };

        typedef std::vector< std::pair<b2Vec2, float> > HintTriggers;
        typedef std::vector<Hint> Hints;

        virtual void onRegister();

        virtual void onUnregister();

        bool fade(float dt, bool in);

        HintTriggers hintTriggers_;
        bool triggered_;
        float t_;

        Hints hints_;
        SceneObjectPtr obj_;
        PuzzleHintIndicatorComponentPtr indicator_;
        bool inside_;
        b2Vec2 hintLastPos_;
    };

    typedef boost::shared_ptr<PuzzleHintAreaComponent> PuzzleHintAreaComponentPtr;
}

#endif
