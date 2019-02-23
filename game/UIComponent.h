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

#ifndef _UICOMPONENT_H_
#define _UICOMPONENT_H_

#include "UIComponentManager.h"

namespace af
{
    class UIComponent : public Component
    {
    public:
        explicit UIComponent(int zOrder = 0)
        : manager_(NULL),
          zOrder_(zOrder),
          visible_(true)
        {
        }

        virtual ~UIComponent() {}

        virtual UIComponentManager* manager() { return manager_; }
        inline void setManager(UIComponentManager* value)
        {
            onSetManager(manager_, value);
        }

        inline int zOrder() const { return zOrder_; }
        inline void setZOrder(int value) { zOrder_ = value; }

        inline bool visible() const { return visible_; }
        inline void setVisible(bool value) { visible_ = value; }

        virtual void update(float dt) = 0;

        virtual void render() = 0;

    private:
        UIComponentManager* manager_;

        int zOrder_;
        bool visible_;
    };
}

#endif
