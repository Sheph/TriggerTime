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

#ifndef _INPUTCONTEXTPLAYERPC_H_
#define _INPUTCONTEXTPLAYERPC_H_

#include "InputContextPlayer.h"
#include "KnobComponent.h"

namespace af
{
    class InputContextPlayerPC : public InputContextPlayer
    {
    public:
        explicit InputContextPlayerPC(Scene* scene);
        ~InputContextPlayerPC();

        virtual void update(float dt);

        virtual bool movePressed(b2Vec2& direction) const;

        virtual bool primaryPressed(b2Vec2& direction) const;

        virtual bool secondaryPressed(b2Vec2& direction) const;

        virtual bool lookPressed(b2Vec2& pos, bool& relative) const;

        virtual bool runPressed() const;

        virtual bool suicidePressed() const;

        virtual bool ghostTriggered() const;

    private:
        virtual void doActivate();

        virtual void doDeactivate();

        b2Vec2 primaryPos_;
        float primaryRadius_;

        b2Vec2 secondaryPos_;
        float secondaryRadius_;

        KnobComponentPtr primaryKnob_;
        KnobComponentPtr secondaryKnob_;
    };
}

#endif
