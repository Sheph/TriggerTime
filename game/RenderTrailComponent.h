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

#ifndef _RENDERTRAILCOMPONENT_H_
#define _RENDERTRAILCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderTrailComponent : public boost::enable_shared_from_this<RenderTrailComponent>,
                                 public RenderComponent
    {
    public:
        RenderTrailComponent(const b2Vec2& pos, float angle,
                             const DrawablePtr& drawable,
                             int zOrder = 0);
        ~RenderTrailComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline float length() const { return length_; }
        void setLength(float length);

        void setAngle(float value);

        inline const DrawablePtr& drawable() const { return drawable_; }

        inline float width1() const { return width1_; }
        void setWidth1(float value);
        void setWidth1NoTex(float value);

        inline float width2() const { return width2_; }
        void setWidth2(float value);
        void setWidth2NoTex(float value);

        inline const Color& color2() const { return color2_; }
        inline void setColor2(const Color& value) { color2_ = value; }

        inline const b2Transform& transform() const { return t_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void updatePoints();

        b2AABB updateVertices();

        b2Transform t_;
        float width1_;
        float width2_;
        DrawablePtr drawable_;
        Color color2_;

        b2Vec2 points_[4];

        std::vector<float> vertices_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        float length_;
        SInt32 cookie_;

        bool dirty_;

        float texWidth1_;
        float texWidth2_;
    };

    typedef boost::shared_ptr<RenderTrailComponent> RenderTrailComponentPtr;
}

#endif
