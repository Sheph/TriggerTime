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

#ifndef _SCRIPTSENSORLISTENER_H_
#define _SCRIPTSENSORLISTENER_H_

#include "SensorListener.h"
#include "luainc.h"
#include <luabind/luabind.hpp>

namespace af
{
    class ScriptSensorListener : public SensorListener,
                                 public luabind::wrap_base
    {
    public:
        /*
         * Important! All our lua derived classes must have
         * 'self' as the first argument and lua self must be
         * passed into it from the script. The reason is lua garbage
         * collection, consider this lua code:
         *
         * obj:setListener(MyListener());
         *
         * now, an instance of lua class 'MyListener' was created
         * and passed to C++, but this instance is not referred anywhere
         * form the lua, thus, it'll be garbage collected, this will result
         * in very subtle bugs such as lua assertions somewhere in the middle
         * of the game: wrapper_base.hpp: assert(!lua_isnil(L, -1));
         */
        explicit ScriptSensorListener(luabind::object self);
        ~ScriptSensorListener();

        virtual void sensorEnter(const SceneObjectPtr& other);

        virtual void sensorExit(const SceneObjectPtr& other);

    protected:
        virtual void endUse();

    private:
        luabind::object self_;
    };

    typedef boost::shared_ptr<SensorListener> SensorListenerPtr;
}

#endif
