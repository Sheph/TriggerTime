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

#ifndef _RENDERSTRIPECOMPONENT_H_
#define _RENDERSTRIPECOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include "Path.h"
#include "af/EnumTraits.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    /*
     * Renders stripe that consists of polygons given. 'image' is repeated
     * along the stripe (given width = imageWidth) and is stretched in
     * bottom-top direction.
     *
     * 'huge' is for optimization. Huge stripes usually don't move around much,
     * so we can have an AABB for each polygon. Non-huge stripes usually move around
     * a lot, so we'll have a single AABB for an entire stripe.
     */
    class RenderStripeComponent : public boost::enable_shared_from_this<RenderStripeComponent>,
                                  public RenderComponent
    {
    public:
        enum Side
        {
            SideLeft = 0,
            SideRight,
            SideTop,
            SideBottom
        };

        RenderStripeComponent(bool huge,
                              Side firstPolygonSide,
                              Side baseSide,
                              const std::vector<Points>& polygons,
                              const DrawablePtr& drawable,
                              float imageWidth,
                              int zOrder = 0);

        RenderStripeComponent(bool huge,
                              Side firstPoint,
                              Side baseSide,
                              const PathPtr& path,
                              float radius,
                              float step,
                              const DrawablePtr& drawable,
                              int zOrder = 0);

        RenderStripeComponent(bool huge,
                              Side firstPoint,
                              Side baseSide,
                              const Points& points,
                              float radius,
                              const DrawablePtr& drawable,
                              int zOrder = 0);

        ~RenderStripeComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline const DrawablePtr& drawable() const { return drawable_; }

    private:
        struct Polygon
        {
            Points points;
            std::vector<float> vertices;
            std::vector<float> texCoords;
            b2AABB prevAABB;
        };

        typedef std::vector<Polygon> Polygons;

        static Points tmp_;

        virtual void onRegister();

        virtual void onUnregister();

        void create(bool huge,
                    Side firstPolygonSide,
                    Side baseSide,
                    const std::vector<Points>& polygons,
                    const DrawablePtr& drawable,
                    float imageWidth);

        void updateVertices(bool updateAABBs);

        bool huge_;

        DrawablePtr drawable_;
        float prevY0_, prevY1_;

        Polygons polygons_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        std::vector<SInt32> cookies_;
    };

    typedef boost::shared_ptr<RenderStripeComponent> RenderStripeComponentPtr;

    AF_ENUMTRAITS(RenderStripeComponent::Side);
}

#endif
