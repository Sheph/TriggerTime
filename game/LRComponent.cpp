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

#include "LRComponent.h"
#include "LRManager.h"
#include "Settings.h"
#include "Renderer.h"
#include <Rocket/Core.h>

namespace af
{
    float LRComponent::t_ = 0.0f;

    LRComponent::LRComponent(const std::string& name, int zOrder)
    : UIComponent(zOrder),
      name_(name)
    {
    }

    LRComponent::~LRComponent()
    {
    }

    void LRComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitUIComponent(shared_from_this());
    }

    void LRComponent::update(float dt)
    {
        t_ += dt;
        lrManager.sysInterface().setTime(t_);
        context_->Update();
    }

    void LRComponent::render()
    {
        lrManager.sysInterface().setTime(t_);
        context_->Render();
        renderer.endScissor();
    }

    void LRComponent::onRegister()
    {
        context_ = Rocket::Core::GetContext(name_.c_str());

        if (!context_) {
            context_ = Rocket::Core::CreateContext(name_.c_str(), Rocket::Core::Vector2i(settings.layoutWidth, settings.layoutHeight));
            context_->LoadMouseCursor("ui/cursor.rml")->RemoveReference();
        }
    }

    void LRComponent::onUnregister()
    {
    }
}
