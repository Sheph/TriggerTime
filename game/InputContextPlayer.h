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

#ifndef _INPUTCONTEXTPLAYER_H_
#define _INPUTCONTEXTPLAYER_H_

#include "InputContext.h"

namespace af
{
    class InputContextPlayer : public InputContext
    {
    public:
        explicit InputContextPlayer(Scene* scene)
        : InputContext(scene),
          secondaryShowOff_(false),
          primarySwitchShowOff_(false),
          secondarySwitchShowOff_(false)
        {
        }

        virtual ~InputContextPlayer() {}

        virtual bool movePressed(b2Vec2& direction) const = 0;

        virtual bool primaryPressed(b2Vec2& direction) const = 0;

        virtual bool secondaryPressed(b2Vec2& direction) const = 0;

        virtual bool lookPressed(b2Vec2& pos, bool& relative) const = 0;

        virtual bool runPressed() const = 0;

        virtual bool suicidePressed() const = 0;

        virtual bool ghostTriggered() const = 0;

        inline bool secondaryShowOff() const { return secondaryShowOff_; }
        inline void setSecondaryShowOff(bool value) { secondaryShowOff_ = value; }

        inline bool primarySwitchShowOff() const { return primarySwitchShowOff_; }
        inline void setPrimarySwitchShowOff(bool value) { primarySwitchShowOff_ = value; }

        inline bool secondarySwitchShowOff() const { return secondarySwitchShowOff_; }
        inline void setSecondarySwitchShowOff(bool value) { secondarySwitchShowOff_ = value; }

    private:
        bool secondaryShowOff_;
        bool primarySwitchShowOff_;
        bool secondarySwitchShowOff_;
    };
}

#endif
