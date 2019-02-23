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

#ifndef _RENDERLBOLTCOMPONENT_H_
#define _RENDERLBOLTCOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderLBoltComponent : public boost::enable_shared_from_this<RenderLBoltComponent>,
                                 public RenderComponent
    {
    public:
        RenderLBoltComponent(const b2Vec2& pos, float angle,
                             const Image& image,
                             int zOrder = 0);
        ~RenderLBoltComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline float length() const { return length_; }
        void setLength(float value);

        inline float subLength() const { return subLength_; }
        void setSubLength(float value);

        inline float width() const { return width_; }
        void setWidth(float value);

        inline float maxOffset() const { return maxOffset_; }
        void setMaxOffset(float value);

        inline float segmentLength() const { return segmentLength_; }
        inline void setSegmentLength(float value) { segmentLength_ = value; }

        inline const b2Transform& transform() const { return t_; }

        void setDirty() { dirty_ = true; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        b2AABB calculateAABB();

        void updatePoints();

        void updateVertices();

        b2Transform t_;
        Image image_;
        float length_;
        float subLength_;
        float width_;
        float maxOffset_;
        float segmentLength_;

        Points points_;

        std::vector<float> vertices_;
        std::vector<float> texCoords_;

        b2Vec2 prevPos_;
        float prevAngle_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        bool dirty_;
        bool needsUpdatePoints_;
        bool needsUpdateVertices_;
    };

    typedef boost::shared_ptr<RenderLBoltComponent> RenderLBoltComponentPtr;
}

#endif
