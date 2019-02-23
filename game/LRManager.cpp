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

#include "LRManager.h"
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Core/Context.h>
#include <Rocket/Core/Lua/Interpreter.h>
#include <Rocket/Controls/Lua/Controls.h>

namespace af
{
    LRManager lrManager;

    template <>
    Single<LRManager>* Single<LRManager>::single = NULL;

    LRManager::LRManager()
    : initialized_(false),
      fileInterface_(NULL),
      renderInterface_(NULL),
      sysInterface_(NULL),
      activeContext_(NULL)
    {
    }

    LRManager::~LRManager()
    {
    }

    bool LRManager::init()
    {
        fileInterface_ = new LRFileInterface();
        renderInterface_ = new LRRenderInterface();
        sysInterface_ = new LRSystemInterface();

        Rocket::Core::SetFileInterface(fileInterface_);
        Rocket::Core::SetRenderInterface(renderInterface_);
        Rocket::Core::SetSystemInterface(sysInterface_);

        Rocket::Core::Initialise();
        Rocket::Controls::Initialise();

        Rocket::Core::String fontNames[1];

        fontNames[0] = "Xolonium-Bold.otf";

        for (size_t i = 0; i < sizeof(fontNames) / sizeof(fontNames[0]); ++i) {
            Rocket::Core::FontDatabase::LoadFontFace(Rocket::Core::String("ui/") + fontNames[i]);
        }

        initialized_ = true;

        return true;
    }

    void LRManager::reload()
    {
        Rocket::Core::ReleaseTextures();
    }

    void LRManager::shutdown()
    {
        Rocket::Core::Context* context = Rocket::Core::GetContext("menu");
        if (context) {
            context->RemoveReference();
        }

        Rocket::Core::Shutdown();

        delete fileInterface_;
        delete renderInterface_;
        delete sysInterface_;
    }

    void LRManager::setLuaState(lua_State* L)
    {
        if (L) {
            Rocket::Core::Lua::Interpreter::Initialise(L);
            Rocket::Controls::Lua::RegisterTypes(L);

            Rocket::Core::Context* context = Rocket::Core::GetContext("menu");
            if (context) {
                /*
                 * We need this on order to trigger new lua event handler instantiation ASAP.
                 * If we don't do this then we'll crash if e.g. an input event comes in
                 * before any actual document is loaded.
                 */
                context->LoadDocument("ui/dummy.rml")->RemoveReference();
            }
        }
        sysInterface_->setLuaState(L);
    }
}
