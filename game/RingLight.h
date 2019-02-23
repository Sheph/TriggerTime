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

#ifndef _RINGLIGHT_H_
#define _RINGLIGHT_H_

#include "Light.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class RingLight : public boost::enable_shared_from_this<RingLight>,
                      public Light
    {
    public:
        explicit RingLight(const std::string& name = "");
        virtual ~RingLight();

        void setDistance(float value);
        inline float distance() const { return distance_; }

        void setNearDistance(float value);
        inline float nearDistance() const { return nearDistance_; }

        void setFarDistance(float value);
        inline float farDistance() const { return farDistance_; }

        virtual LightPtr sharedThis() { return shared_from_this(); }

        virtual void update();

        virtual void render();

    private:
        virtual b2AABB calculateAABB();

        void addOneDir(const b2Vec2& p, float dist, float len);

        std::vector<float> vertices_;
        std::vector<float> colors_;
        std::vector<float> s_;

        std::vector<float> softVertices_;
        std::vector<float> softColors_;
        std::vector<float> softS_;

        float distance_;
        float nearDistance_;
        float farDistance_;
    };

    typedef boost::shared_ptr<RingLight> RingLightPtr;
}

#endif
