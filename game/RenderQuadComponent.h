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

#ifndef _RENDERQUADCOMPONENT_H_
#define _RENDERQUADCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    /*
     * Renders quad. 'image' is stretched upon entire quad.
     */
    class RenderQuadComponent : public boost::enable_shared_from_this<RenderQuadComponent>,
                                public RenderComponent
    {
    public:
        RenderQuadComponent(const b2Vec2& pos, float angle,
                            float width, float height,
                            const DrawablePtr& drawable,
                            int zOrder = 0);
        ~RenderQuadComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline const b2Vec2& pos() const { return pos_; }
        void setPos(const b2Vec2& value);

        inline float angle() const { return angle_; }
        void setAngle(float value);

        inline float width() const { return width_; }
        inline float height() const { return height_; }
        void setSizes(float w, float h);

        void setWidth(float value, bool keepAspect = true);
        void setHeight(float value, bool keepAspect = true);

        inline const DrawablePtr& drawable() const { return drawable_; }

        inline bool mask() const { return mask_; }
        inline void setMask(bool value) { mask_ = value; }

        inline bool fixedPos() const { return fixedPos_; }
        void setFixedPos(bool value);

        inline bool flip() const { return flip_; }
        void setFlip(bool value);

        inline void script_setHeight(float value) { setHeight(value); }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void updatePoints();

        b2AABB updateVertices();

        b2Vec2 pos_;
        float angle_;
        float width_;
        float height_;
        float aspect_;

        b2Vec2 points_[4];
        DrawablePtr drawable_;

        std::vector<float> vertices_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        bool mask_;
        bool fixedPos_;
        bool flip_;
        bool dirty_;
    };

    typedef boost::shared_ptr<RenderQuadComponent> RenderQuadComponentPtr;
}

#endif
