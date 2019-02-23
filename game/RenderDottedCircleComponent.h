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

#ifndef _RENDERDOTTEDCIRCLECOMPONENT_H_
#define _RENDERDOTTEDCIRCLECOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderDottedCircleComponent : public boost::enable_shared_from_this<RenderDottedCircleComponent>,
                                        public RenderComponent
    {
    public:
        RenderDottedCircleComponent(const b2Vec2& pos, float width,
                                    const DrawablePtr& drawable,
                                    int zOrder = 0);
        ~RenderDottedCircleComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline void setRadius(float value) { radius_ = value; }
        inline float radius() const { return radius_; }

        inline const DrawablePtr& drawable() const { return drawable_; }

        inline float width() const { return width_; }

        inline void setDotVelocity(float value, float forRadius) { dotVelocity_ = value; dotVelocityRadius_ = forRadius; }
        inline float dotVelocity() const { return dotVelocity_; }

        void setDotDistance(float value, float forRadius);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        b2Transform xf_;
        float width_;
        DrawablePtr drawable_;

        float radius_;
        float dotVelocity_;
        float dotVelocityRadius_;
        int numSectors_;
        float t_;

        std::vector<float> vertices_;
        std::vector<float> texCoords_;
    };

    typedef boost::shared_ptr<RenderDottedCircleComponent> RenderDottedCircleComponentPtr;
}

#endif
