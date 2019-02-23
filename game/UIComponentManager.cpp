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

#include "UIComponentManager.h"
#include "UIComponent.h"
#include "Scene.h"
#include <map>

namespace af
{
    UIComponentManager::UIComponentManager()
    {
    }

    UIComponentManager::~UIComponentManager()
    {
        assert(components_.empty());
    }

    void UIComponentManager::cleanup()
    {
        assert(components_.empty());
    }

    void UIComponentManager::addComponent(const ComponentPtr& component)
    {
        UIComponentPtr uiComponent = boost::dynamic_pointer_cast<UIComponent>(component);
        assert(uiComponent);

        assert(!component->manager());

        components_.insert(uiComponent);
        uiComponent->setManager(this);
    }

    void UIComponentManager::removeComponent(const ComponentPtr& component)
    {
        UIComponentPtr uiComponent = boost::dynamic_pointer_cast<UIComponent>(component);
        assert(uiComponent);

        if (components_.erase(uiComponent)) {
            uiComponent->setManager(NULL);
        }
    }

    void UIComponentManager::freezeComponent(const ComponentPtr& component)
    {
    }

    void UIComponentManager::thawComponent(const ComponentPtr& component)
    {
    }

    void UIComponentManager::update(float dt)
    {
        static std::vector<UIComponentPtr> tmp;

        tmp.reserve(components_.size());

        for (std::set<UIComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            tmp.push_back(*it);
        }

        for (std::vector<UIComponentPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if ((*it)->manager() &&
                (!scene()->paused() || ((*it)->zOrder() > 0))) {
                (*it)->update(dt);
            }
        }

        tmp.resize(0);
    }

    void UIComponentManager::debugDraw()
    {
        for (std::set<UIComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            (*it)->debugDraw();
        }
    }

    void UIComponentManager::render()
    {
        std::multimap<int, UIComponentPtr> tmp;

        for (std::set<UIComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            tmp.insert(std::make_pair((*it)->zOrder(), *it));
        }

        for (std::multimap<int, UIComponentPtr>::const_iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if (it->second->visible()) {
                it->second->render();
            }
        }
    }
}
