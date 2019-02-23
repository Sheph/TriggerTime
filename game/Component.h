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

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "ComponentVisitor.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class Scene;
    class SceneObject;
    class ComponentManager;

    class Component : boost::noncopyable
    {
    public:
        Component();
        virtual ~Component();

        virtual ComponentPtr sharedThis() = 0;

        virtual void accept(ComponentVisitor& visitor) = 0;

        virtual ComponentManager* manager() = 0;

        virtual void debugDraw();

        virtual void onFreeze();

        virtual void onThaw();

        inline SceneObject* parent() { return parent_; }
        inline const SceneObject* parent() const { return parent_; }
        inline void setParent(SceneObject* value) { parent_ = value; }

        Scene* scene();

        void removeFromParent();

        boost::shared_ptr<SceneObject> script_parent() const;

    protected:
        template <class ComponentManagerT>
        inline void onSetManager(ComponentManagerT*& manager,
                                 ComponentManagerT* value)
        {
            if (!manager && value) {
                manager = value;
                onRegister();
            } else if (manager && !value) {
                onUnregister();
                manager = NULL;
            } else {
                assert(false);
            }
        }

    private:
        virtual void onRegister() = 0;

        virtual void onUnregister() = 0;

        mutable SceneObject* parent_;
    };
}

#endif
