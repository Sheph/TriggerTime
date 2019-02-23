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

#include "RenderComponent.h"
#include "Renderer.h"

namespace af
{
    static const int StencilOpIntValues[] =
    {
        StencilOpKeep,
        StencilOpZero,
        StencilOpReplace,
        StencilOpIncr,
        StencilOpDecr
    };

    static const char* StencilOpStrValues[] =
    {
        "keep",
        "zero",
        "replace",
        "incr",
        "decr",
    };

    static const int StencilFuncIntValues[] =
    {
        StencilFuncLess,
        StencilFuncLequal,
        StencilFuncGreater,
        StencilFuncGequal,
        StencilFuncEqual,
        StencilFuncNotEqual,
    };

    static const char* StencilFuncStrValues[] =
    {
        "less",
        "lequal",
        "greater",
        "gequal",
        "equal",
        "notequal",
    };

    AF_ENUMTRAITS_IMPL(StencilOp, StencilOp);
    AF_ENUMTRAITS_IMPL(StencilFunc, StencilFunc);

    RenderComponent::RenderComponent(int zOrder, bool renderAlways)
    : manager_(NULL),
      zOrder_(zOrder),
      renderAlways_(renderAlways),
      visible_(true),
      color_(1.0f, 1.0, 1.0f, 1.0f),
      flashColor_(1.0f, 1.0, 1.0f, 0.0f),
      stencilSet_(false),
      stencilUse_(false)
    {
    }

    RenderComponent::~RenderComponent()
    {
    }

    void RenderComponent::stencilSet(StencilOp fail, StencilOp pass)
    {
        stencilSet_ = true;
        stencilFail_ = fail;
        stencilPass_ = pass;
    }

    void RenderComponent::stencilUse(StencilFunc func, UInt32 ref)
    {
        stencilUse_ = true;
        stencilFunc_ = func;
        stencilRef_ = ref;
    }

    void RenderComponent::applyStencil(bool apply)
    {
        if (!stencilSet_ && !stencilUse_) {
            return;
        }

        if (!apply) {
            renderer.stencilEnd();
            return;
        }

        if (stencilSet_) {
            renderer.stencilSet(stencilFail_, stencilPass_);
        }

        if (stencilUse_) {
            renderer.stencilUse(stencilFunc_, stencilRef_);
        }
    }
}
