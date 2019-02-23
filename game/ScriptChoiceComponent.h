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

#ifndef _SCRIPTCHOICECOMPONENT_H_
#define _SCRIPTCHOICECOMPONENT_H_

#include "ChoiceComponent.h"
#include <boost/enable_shared_from_this.hpp>
#include "luainc.h"
#include <luabind/luabind.hpp>

namespace af
{
    class ScriptChoiceComponent : public boost::enable_shared_from_this<ScriptChoiceComponent>,
                                  public ChoiceComponent,
                                  public luabind::wrap_base
    {
    public:
        /*
         * Important! See ScriptSensorListener.h
         */
        explicit ScriptChoiceComponent(luabind::object self, int zOrder = 0);
        ~ScriptChoiceComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        virtual void onPress(int i);

        luabind::object self_;
    };

    typedef boost::shared_ptr<ScriptChoiceComponent> ScriptChoiceComponentPtr;
}

#endif
