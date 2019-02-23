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

#ifndef _INPUTCONTEXTUI_H_
#define _INPUTCONTEXTUI_H_

#include "InputContext.h"
#include <Rocket/Core/Context.h>

namespace af
{
    class InputContextUI : public InputContext
    {
    public:
        InputContextUI(Scene* scene, Rocket::Core::Context* rc): InputContext(scene), rc_(rc) {}
        virtual ~InputContextUI() {}

        virtual bool leftPressed() const = 0;

        virtual bool rightPressed() const = 0;

        virtual bool upPressed() const = 0;

        virtual bool downPressed() const = 0;

        virtual bool okPressed() const = 0;

        virtual bool havePointer() const = 0;

        virtual bool pressed(b2Vec2* point) const = 0;

        virtual bool triggered() const = 0;

    protected:
        inline Rocket::Core::Context* rc() { return rc_; }

    private:
        Rocket::Core::Context* rc_;
    };
}

#endif
