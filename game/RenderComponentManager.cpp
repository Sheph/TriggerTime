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

#include "RenderComponentManager.h"
#include "RenderComponent.h"
#include "CameraComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "InputManager.h"

namespace af
{
    bool RenderComponentManager::QueryResultsComparer::operator()(size_t l, size_t r) const
    {
        if (components_[l]->zOrder() < components_[r]->zOrder()) {
            return true;
        } else if (components_[l]->zOrder() == components_[r]->zOrder()) {
            return components_[l] < components_[r];
        }

        return false;
    }

    RenderComponentManager::RenderComponentManager()
    : comparer_(queryResultComponents_)
    {
    }

    RenderComponentManager::~RenderComponentManager()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());
    }

    void RenderComponentManager::cleanup()
    {
        assert(components_.empty());
        assert(frozenComponents_.empty());
    }

    void RenderComponentManager::addComponent(const ComponentPtr& component)
    {
        RenderComponentPtr renderComponent = boost::dynamic_pointer_cast<RenderComponent>(component);
        assert(renderComponent);

        assert(!component->manager());

        components_.insert(renderComponent);
        renderComponent->setManager(this);
    }

    void RenderComponentManager::removeComponent(const ComponentPtr& component)
    {
        RenderComponentPtr renderComponent = boost::dynamic_pointer_cast<RenderComponent>(component);
        assert(renderComponent);

        if (components_.erase(renderComponent) ||
            frozenComponents_.erase(renderComponent)) {
            renderComponent->setManager(NULL);
        }
    }

    void RenderComponentManager::freezeComponent(const ComponentPtr& component)
    {
        RenderComponentPtr renderComponent = boost::static_pointer_cast<RenderComponent>(component);

        components_.erase(renderComponent);
        frozenComponents_.insert(renderComponent);
        component->onFreeze();
    }

    void RenderComponentManager::thawComponent(const ComponentPtr& component)
    {
        RenderComponentPtr renderComponent = boost::static_pointer_cast<RenderComponent>(component);

        frozenComponents_.erase(renderComponent);
        components_.insert(renderComponent);
        component->onThaw();
    }

    void RenderComponentManager::update(float dt)
    {
        queryResultComponents_.clear();
        queryResultData_.clear();
        queryResultIndices_.clear();

        for (std::set<RenderComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            (*it)->update(dt);
            if ((*it)->renderAlways()) {
                queryResultIndices_.push_back(queryResultData_.size());
                queryResultComponents_.push_back(it->get());
                queryResultData_.push_back(NULL);
            }
        }

        if (scene()->input()->cullPressed()) {
            tree_.Query(this, scene()->camera()->findComponent<CameraComponent>()->getTrueAABB());
        } else {
            tree_.Query(this, scene()->camera()->findComponent<CameraComponent>()->getAABB());
        }

        std::sort(&queryResultIndices_[0],
            &queryResultIndices_[0] + queryResultIndices_.size(),
            comparer_);

        queryResultSortedData_.resize(queryResultData_.size());

        for (size_t i = 0; i < queryResultIndices_.size(); ++i) {
            queryResultSortedData_[i] = queryResultData_[queryResultIndices_[i]];
        }
    }

    void RenderComponentManager::debugDraw()
    {
        for (std::set<RenderComponentPtr>::iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            (*it)->debugDraw();
        }
    }

    void RenderComponentManager::render()
    {
        if (queryResultIndices_.empty()) {
            return;
        }

        size_t* cur = &queryResultIndices_[0];
        size_t* end = cur + queryResultIndices_.size();

        while (cur < end) {
            size_t* next = std::upper_bound(cur, end, *cur, comparer_);

            RenderComponent* rc = queryResultComponents_[*cur];
            if (rc->visible()) {
                rc->render(&queryResultSortedData_[0] + (cur - &queryResultIndices_[0]), next - cur);
            }

            cur = next;
        }
    }

    SInt32 RenderComponentManager::addAABB(RenderComponent* component,
                                           const b2AABB& aabb,
                                           void* data)
    {
        ProxyDataList::iterator it = proxyDataList_.insert(proxyDataList_.end(),
                                                           ProxyData());

        ProxyData& pd = proxyDataList_.back();

        pd.it = it;
        pd.component = component;
        pd.data = data;

        return tree_.CreateProxy(aabb, &pd);
    }

    void RenderComponentManager::moveAABB(SInt32 cookie,
                                          const b2AABB& prevAABB,
                                          const b2AABB& aabb,
                                          const b2Vec2& displacement)
    {
        if (displacement.LengthSquared() > b2_maxTranslationSquared) {
            tree_.MoveProxy(cookie, aabb, b2Vec2_zero);
            return;
        }

        b2AABB combinedAABB;

        combinedAABB.Combine(prevAABB, aabb);

        if (combinedAABB.GetPerimeter() > aabb.GetPerimeter() + scene()->gameHeight() * 4.0f) {
            b2AABB tmp = aabb;

            tmp.upperBound.x = combinedAABB.upperBound.x + 100.0f + tmp.upperBound.x - tmp.lowerBound.x;
            tmp.lowerBound.x = combinedAABB.upperBound.x + 100.0f;

            tree_.MoveProxy(cookie, tmp, b2Vec2_zero);
            tree_.MoveProxy(cookie, aabb, b2Vec2_zero);
        } else {
            tree_.MoveProxy(cookie, combinedAABB, displacement);
        }
    }

    void RenderComponentManager::removeAABB(SInt32 cookie)
    {
        ProxyData* pd = reinterpret_cast<ProxyData*>(tree_.GetUserData(cookie));

        proxyDataList_.erase(pd->it);

        tree_.DestroyProxy(cookie);
    }

    void RenderComponentManager::queryObjects(const b2AABB& aabb, std::set<SceneObjectPtr>& objs)
    {
        queryResultComponents_.clear();
        queryResultData_.clear();
        queryResultIndices_.clear();

        tree_.Query(this, aabb);

        for (size_t i = 0; i < queryResultComponents_.size(); ++i) {
            objs.insert(queryResultComponents_[i]->parent()->shared_from_this());
        }
    }

    bool RenderComponentManager::QueryCallback(SInt32 cookie)
    {
        ProxyData* pd = reinterpret_cast<ProxyData*>(tree_.GetUserData(cookie));

        queryResultIndices_.push_back(queryResultData_.size());
        queryResultComponents_.push_back(pd->component);
        queryResultData_.push_back(pd->data);

        return true;
    }
}
