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

#ifndef _LINELIGHT_H_
#define _LINELIGHT_H_

#include "Light.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class LineLight : public boost::enable_shared_from_this<LineLight>,
                      public Light
    {
    public:
        explicit LineLight(const std::string& name = "");
        virtual ~LineLight();

        void setLength(float value);
        inline float length() const { return length_; }

        void setDistance(float value);
        inline float distance() const { return distance_; }

        void setBothWays(bool value);
        inline bool bothWays() const { return bothWays_; }

        virtual LightPtr sharedThis() { return shared_from_this(); }

        virtual void update();

        virtual void render();

    private:
        virtual b2AABB calculateAABB();

        void addOneDir(const b2Vec2& p1, const b2Vec2& p2,
            b2Vec2& endP1, b2Vec2& endP2,
            float& s1, float& s2);

        void addEdge(const b2Vec2& p1, const b2Vec2& p2,
                     float s1, float s2);

        std::vector<float> vertices_;
        std::vector<float> colors_;
        std::vector<float> s_;

        std::vector<float> softVertices_;
        std::vector<float> softColors_;
        std::vector<float> softS_;

        float length_;
        float distance_;
        bool bothWays_;
    };

    typedef boost::shared_ptr<LineLight> LineLightPtr;
}

#endif
