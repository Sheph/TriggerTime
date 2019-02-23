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

#ifndef _RENDERPROJCOMPONENT_H_
#define _RENDERPROJCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class RenderProjComponent : public boost::enable_shared_from_this<RenderProjComponent>,
                                public RenderComponent
    {
    public:
        RenderProjComponent(const b2Vec2 points[4],
                            const DrawablePtr& drawable,
                            int zOrder = 0);
        ~RenderProjComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline const DrawablePtr& drawable() const { return drawable_; }

        inline bool fixedPos() const { return fixedPos_; }
        void setFixedPos(bool value);

        inline bool flip() const { return flip_; }
        void setFlip(bool value);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void updatePoints();

        b2AABB updateVertices();

        b2Vec2 points_[4];
        DrawablePtr drawable_;

        std::vector<float> vertices_;
        float width1_;
        float width2_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        bool fixedPos_;
        bool flip_;
        bool dirty_;
    };

    typedef boost::shared_ptr<RenderProjComponent> RenderProjComponentPtr;
}

#endif
