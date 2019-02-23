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

#ifndef _RENDERCOMPONENT_H_
#define _RENDERCOMPONENT_H_

#include "RenderComponentManager.h"
#include "OGL.h"
#include "af/EnumTraits.h"
#include <vector>

namespace af
{
    enum StencilOp
    {
        StencilOpKeep = GL_KEEP,
        StencilOpZero = GL_ZERO,
        StencilOpReplace = GL_REPLACE,
        StencilOpIncr = GL_INCR,
        StencilOpDecr = GL_DECR
    };

    enum StencilFunc
    {
        StencilFuncLess = GL_LESS,
        StencilFuncLequal = GL_LEQUAL,
        StencilFuncGreater = GL_GREATER,
        StencilFuncGequal = GL_GEQUAL,
        StencilFuncEqual = GL_EQUAL,
        StencilFuncNotEqual = GL_NOTEQUAL,
    };

    class RenderComponent : public Component
    {
    public:
        explicit RenderComponent(int zOrder = 0, bool renderAlways = false);
        virtual ~RenderComponent();

        virtual RenderComponentManager* manager() { return manager_; }
        inline void setManager(RenderComponentManager* value)
        {
            onSetManager(manager_, value);
        }

        inline int zOrder() const { return zOrder_; }
        inline void setZOrder(int value) { zOrder_ = value; }

        inline bool renderAlways() const { return renderAlways_; }
        inline void setRenderAlways(bool value) { renderAlways_ = value; }

        inline bool visible() const { return visible_; }
        inline void setVisible(bool value) { visible_ = value; }

        inline const Color& color() const { return color_; }
        inline void setColor(const Color& value) { color_ = value; }

        inline const Color& flashColor() const { return flashColor_; }
        inline void setFlashColor(const Color& value) { flashColor_ = value; }

        void stencilSet(StencilOp fail, StencilOp pass);

        void stencilUse(StencilFunc func, UInt32 ref);

        virtual void update(float dt) = 0;

        virtual void render(void* const* parts, size_t numParts) = 0;

        inline const std::string& name() const { return name_; }
        inline void setName(const std::string& value) { name_ = value; }

    protected:
        void applyStencil(bool apply);

    private:
        RenderComponentManager* manager_;

        int zOrder_;
        bool renderAlways_;
        bool visible_;
        Color color_;
        Color flashColor_;
        bool stencilSet_;
        StencilOp stencilFail_;
        StencilOp stencilPass_;
        bool stencilUse_;
        StencilFunc stencilFunc_;
        UInt32 stencilRef_;

        std::string name_;
    };

    AF_ENUMTRAITS(StencilOp);
    AF_ENUMTRAITS(StencilFunc);
}

#endif
