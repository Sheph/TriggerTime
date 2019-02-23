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

#include "PhasedComponentManager.h"
#include "PhasedComponent.h"

namespace af
{
    PhasedComponentManager::PhasedComponentManager()
    {
    }

    PhasedComponentManager::~PhasedComponentManager()
    {
        assert(thinkComponents_.empty());
        assert(preRenderComponents_.empty());
        assert(frozenComponents_.empty());
    }

    void PhasedComponentManager::cleanup()
    {
        assert(thinkComponents_.empty());
        assert(preRenderComponents_.empty());
        assert(frozenComponents_.empty());
    }

    void PhasedComponentManager::addComponent(const ComponentPtr& component)
    {
        PhasedComponentPtr phasedComponent = boost::dynamic_pointer_cast<PhasedComponent>(component);
        assert(phasedComponent);

        assert(!component->manager());

        if ((phasedComponent->phases() & phaseThink) != 0) {
            thinkComponents_.insert(phasedComponent);
        }
        if ((phasedComponent->phases() & phasePreRender) != 0) {
            preRenderComponents_.insert(phasedComponent);
        }
        phasedComponent->setManager(this);
    }

    void PhasedComponentManager::removeComponent(const ComponentPtr& component)
    {
        PhasedComponentPtr phasedComponent = boost::dynamic_pointer_cast<PhasedComponent>(component);
        assert(phasedComponent);

        bool res = (thinkComponents_.erase(phasedComponent) > 0);
        res |= (preRenderComponents_.erase(phasedComponent) > 0);

        if (res || frozenComponents_.erase(phasedComponent)) {
            phasedComponent->setManager(NULL);
        }
    }

    void PhasedComponentManager::freezeComponent(const ComponentPtr& component)
    {
        PhasedComponentPtr phasedComponent = boost::static_pointer_cast<PhasedComponent>(component);

        thinkComponents_.erase(phasedComponent);
        preRenderComponents_.erase(phasedComponent);
        frozenComponents_.insert(phasedComponent);
        component->onFreeze();
    }

    void PhasedComponentManager::thawComponent(const ComponentPtr& component)
    {
        PhasedComponentPtr phasedComponent = boost::static_pointer_cast<PhasedComponent>(component);

        frozenComponents_.erase(phasedComponent);
        if ((phasedComponent->phases() & phaseThink) != 0) {
            thinkComponents_.insert(phasedComponent);
        }
        if ((phasedComponent->phases() & phasePreRender) != 0) {
            preRenderComponents_.insert(phasedComponent);
        }
        component->onThaw();
    }

    void PhasedComponentManager::update(float dt)
    {
        static std::vector<PhasedComponentPtr> tmp;

        tmp.reserve(thinkComponents_.size());

        for (std::set<PhasedComponentPtr>::iterator it = thinkComponents_.begin();
             it != thinkComponents_.end();
             ++it ) {
            tmp.push_back(*it);
        }

        for (std::vector<PhasedComponentPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if ((*it)->manager()) {
                (*it)->update(dt);
            }
        }

        tmp.resize(0);
    }

    void PhasedComponentManager::preRender(float dt)
    {
        static std::vector<PhasedComponentPtr> tmp;

        tmp.reserve(preRenderComponents_.size());

        for (std::set<PhasedComponentPtr>::iterator it = preRenderComponents_.begin();
             it != preRenderComponents_.end();
             ++it ) {
            tmp.push_back(*it);
        }

        for (std::vector<PhasedComponentPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if ((*it)->manager()) {
                (*it)->preRender(dt);
            }
        }

        tmp.resize(0);
    }

    void PhasedComponentManager::debugDraw()
    {
        for (std::set<PhasedComponentPtr>::iterator it = thinkComponents_.begin();
             it != thinkComponents_.end();
             ++it ) {
            (*it)->debugDraw();
        }
        for (std::set<PhasedComponentPtr>::iterator it = preRenderComponents_.begin();
             it != preRenderComponents_.end();
             ++it ) {
            (*it)->debugDraw();
        }
    }
}
