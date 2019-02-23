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

#ifndef _RENDERTERRAINCOMPONENT_H_
#define _RENDERTERRAINCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    /*
     * Renders simple polygon 'points' (not necessarily convex) with
     * simple polygon (not necessarily convex) holes in it - 'holes'.
     * 'image' is stretched to 'imageWidth' x 'imageHeight' and repeated.
     *
     * Note! Do not move object around, this component is optimized for
     * static objects and doesn't recalculate AABB.
     */
    class RenderTerrainComponent : public boost::enable_shared_from_this<RenderTerrainComponent>,
                                   public RenderComponent
    {
    public:
        RenderTerrainComponent(const Points& points,
                               const std::vector<Points>& holes,
                               const DrawablePtr& drawable,
                               float imageWidth, float imageHeight,
                               const b2Vec2& imageOffset,
                               int zOrder = 0);
        ~RenderTerrainComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline const DrawablePtr& drawable() const { return drawable_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        std::vector<float> vertices_;
        std::vector<float> texCoords_;

        DrawablePtr drawable_;

        std::vector<SInt32> cookies_;
    };

    typedef boost::shared_ptr<RenderTerrainComponent> RenderTerrainComponentPtr;
}

#endif
