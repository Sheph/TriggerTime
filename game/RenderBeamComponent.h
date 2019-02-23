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

#ifndef _RENDERBEAMCOMPONENT_H_
#define _RENDERBEAMCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderBeamComponent : public boost::enable_shared_from_this<RenderBeamComponent>,
                                public RenderComponent
    {
    public:
        RenderBeamComponent(const b2Vec2& pos, float angle, float width,
                            const DrawablePtr& drawable,
                            int zOrder = 0);
        ~RenderBeamComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        void setLength(float length);

        void setAngle(float value);

        inline const DrawablePtr& drawable() const { return drawable_; }

        inline float width() const { return width_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void updatePoints(float length);

        b2AABB updateVertices();

        b2Transform t_;
        float width_;
        DrawablePtr drawable_;

        b2Vec2 points_[4];

        std::vector<float> vertices_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        float length_;
        SInt32 cookie_;

        bool dirty_;
    };

    typedef boost::shared_ptr<RenderBeamComponent> RenderBeamComponentPtr;
}

#endif
