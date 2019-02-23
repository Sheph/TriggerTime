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

#ifndef _RENDERCOMPONENTMANAGER_H_
#define _RENDERCOMPONENTMANAGER_H_

#include "ComponentManager.h"
#include "SceneObjectManager.h"
#include <Box2D/Box2D.h>
#include <set>
#include <list>
#include <vector>

namespace af
{
    class RenderComponentManager : public ComponentManager
    {
    public:
        RenderComponentManager();
        ~RenderComponentManager();

        virtual void cleanup();

        virtual void addComponent(const ComponentPtr& component);

        virtual void removeComponent(const ComponentPtr& component);

        virtual void freezeComponent(const ComponentPtr& component);

        virtual void thawComponent(const ComponentPtr& component);

        virtual void update(float dt);

        virtual void debugDraw();

        void render();

        SInt32 addAABB(RenderComponent* component,
                       const b2AABB& aabb,
                       void* data);

        void moveAABB(SInt32 cookie,
                      const b2AABB& prevAABB,
                      const b2AABB& aabb,
                      const b2Vec2& displacement);

        void removeAABB(SInt32 cookie);

        void queryObjects(const b2AABB& aabb, std::set<SceneObjectPtr>& objs);

        /*
         * For internal use only.
         */
        bool QueryCallback(SInt32 cookie);

    private:
        struct ProxyData
        {
            std::list<ProxyData>::iterator it;
            RenderComponent* component;
            void* data;
        };

        class QueryResultsComparer: public std::binary_function<size_t, size_t, bool>
        {
        public:
            explicit QueryResultsComparer(FastVector<RenderComponent*>& components)
            : components_(components)
            {
            }

            ~QueryResultsComparer() {}

            bool operator()(size_t l, size_t r) const;

        private:
            FastVector<RenderComponent*>& components_;
        };

        typedef std::list<ProxyData> ProxyDataList;

        std::set<RenderComponentPtr> components_;
        std::set<RenderComponentPtr> frozenComponents_;

        ProxyDataList proxyDataList_;

        b2DynamicTree tree_;

        FastVector<RenderComponent*> queryResultComponents_;
        FastVector<void*> queryResultData_;
        FastVector<void*> queryResultSortedData_;
        FastVector<size_t> queryResultIndices_;

        QueryResultsComparer comparer_;
    };
}

#endif
