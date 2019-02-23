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

#ifndef _LRMANAGER_H_
#define _LRMANAGER_H_

#include "LRSystemInterface.h"
#include "LRFileInterface.h"
#include "LRRenderInterface.h"
#include "af/Single.h"
#include <Rocket/Core/Context.h>

struct lua_State;

namespace af
{
    class LRManager : public Single<LRManager>
    {
    public:
        LRManager();
        ~LRManager();

        bool init();

        void reload();

        void shutdown();

        inline Rocket::Core::Context* activeContext() { return activeContext_; }
        inline void setActiveContext(Rocket::Core::Context* value) { activeContext_ = value; }

        void setLuaState(lua_State* L);

        inline LRSystemInterface& sysInterface() { return *sysInterface_; }

        inline bool initialized() const { return initialized_; }

    private:
        bool initialized_;

        LRFileInterface* fileInterface_;
        LRRenderInterface* renderInterface_;
        LRSystemInterface* sysInterface_;

        Rocket::Core::Context* activeContext_;
    };

    extern LRManager lrManager;
}

#endif
