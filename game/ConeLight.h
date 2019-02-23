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

#ifndef _CONELIGHT_H_
#define _CONELIGHT_H_

#include "Light.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class ConeLight : public boost::enable_shared_from_this<ConeLight>,
                      public Light
    {
    public:
        explicit ConeLight(const std::string& name = "");
        virtual ~ConeLight();

        void setNearDistance(float value);
        inline float nearDistance() const { return nearDistance_; }

        void setFarDistance(float value);
        inline float farDistance() const { return farDistance_; }

        void setConeAngle(float value);
        inline float coneAngle() const { return coneAngle_; }

        virtual LightPtr sharedThis() { return shared_from_this(); }

        virtual void update();

        virtual void render();

    private:
        virtual b2AABB calculateAABB();

        void addNearEdge();

        void addEdge(const b2Vec2& pt, const b2Vec2& endPt, float fraction, bool dir);

        std::vector<float> vertices_;
        std::vector<float> colors_;
        std::vector<float> s_;

        std::vector<float> softVertices_;
        std::vector<float> softColors_;
        std::vector<float> softS_;

        std::vector<float> edgeVertices_;
        std::vector<float> edgeColors_;
        std::vector<float> edgeS_;

        float nearDistance_;
        float farDistance_;
        float coneAngle_;
    };

    typedef boost::shared_ptr<ConeLight> ConeLightPtr;
}

#endif
