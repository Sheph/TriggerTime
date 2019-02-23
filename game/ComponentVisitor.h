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

#ifndef _COMPONENTVISITOR_H_
#define _COMPONENTVISITOR_H_

#include "af/Types.h"
#include <boost/shared_ptr.hpp>

namespace af
{
    class Component;
    class PhasedComponent;
    class PhysicsComponent;
    class CollisionComponent;
    class RenderComponent;
    class UIComponent;

    typedef boost::shared_ptr<Component> ComponentPtr;
    typedef boost::shared_ptr<PhasedComponent> PhasedComponentPtr;
    typedef boost::shared_ptr<PhysicsComponent> PhysicsComponentPtr;
    typedef boost::shared_ptr<CollisionComponent> CollisionComponentPtr;
    typedef boost::shared_ptr<RenderComponent> RenderComponentPtr;
    typedef boost::shared_ptr<UIComponent> UIComponentPtr;

    class ComponentVisitor
    {
    public:
        ComponentVisitor() {}
        virtual ~ComponentVisitor() {}

        virtual void visitPhasedComponent(const PhasedComponentPtr& component) = 0;
        virtual void visitPhysicsComponent(const PhysicsComponentPtr& component) = 0;
        virtual void visitCollisionComponent(const CollisionComponentPtr& component) = 0;
        virtual void visitRenderComponent(const RenderComponentPtr& component) = 0;
        virtual void visitUIComponent(const UIComponentPtr& component) = 0;
    };
}

#endif
