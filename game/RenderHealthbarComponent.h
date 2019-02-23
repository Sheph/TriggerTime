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

#ifndef _RENDERHEALTHBARCOMPONENT_H_
#define _RENDERHEALTHBARCOMPONENT_H_

#include "RenderComponent.h"
#include "Image.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RenderHealthbarComponent : public boost::enable_shared_from_this<RenderHealthbarComponent>,
                                     public RenderComponent
    {
    public:
        RenderHealthbarComponent(const b2Vec2& pos, float angle,
            float width, float height, int zOrder = 0);
        ~RenderHealthbarComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        virtual void setTarget(const SceneObjectPtr& value) { target_ = value; }
        inline const SceneObjectPtr& target() const { return target_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        b2AABB updateVertices();

        void updateLifeVertices();

        inline SceneObject* realTarget() { return target_ ? target_.get() : parent(); }

        b2Vec2 barPos_;
        float barAngle_;
        float width_;
        float height_;

        std::vector<float> bgVertices_;
        std::vector<float> endVertices_;
        std::vector<float> endTexCoords_;
        std::vector<float> lifeVertices_;

        Image bg_;
        Image end_;
        Image life_;

        b2Vec2 prevPos_;
        float prevCameraAngle_;
        float prevLifePercent_;
        b2AABB prevAABB_;
        SInt32 cookie_;

        SceneObjectPtr target_;
    };

    typedef boost::shared_ptr<RenderHealthbarComponent> RenderHealthbarComponentPtr;
}

#endif
