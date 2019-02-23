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

#ifndef _RENDERTENTACLECOMPONENT_H_
#define _RENDERTENTACLECOMPONENT_H_

#include "RenderComponent.h"
#include "Drawable.h"
#include "HermitePath.h"
#include "af/RUBEFixture.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class RenderTentacleComponent : public boost::enable_shared_from_this<RenderTentacleComponent>,
                                    public RenderComponent
    {
    public:
        RenderTentacleComponent(const std::vector<SceneObjectPtr>& objs,
                                const std::vector<RUBEPolygonFixturePtr>& fixtures,
                                const DrawablePtr& drawable,
                                int zOrder = 0,
                                int numIterations = 0, float c = 0.0f, float step = 0.0f);
        RenderTentacleComponent(const std::vector<SceneObjectPtr>& objs,
                                const std::vector<RUBECircleFixturePtr>& fixtures,
                                const DrawablePtr& drawable,
                                int zOrder = 0,
                                int numIterations = 0, float c = 0.0f, float step = 0.0f);
        ~RenderTentacleComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        virtual void debugDraw();

        inline bool flip() const { return flip_; }
        inline void setFlip(bool value) { flip_ = value; }

        void resetTimeline1(int num, float value = 0.0f);
        inline int timeline1Size() const { return timeline1_.size(); }

        void resetTimeline2(int num, float value = 0.0f);
        inline int timeline2Size() const { return timeline2_.size(); }

        inline float time1At(int i) const { return timeline1_[i]; }
        inline float width1At(int i) const { return widths1_[i]; }
        void set1At(int i, float t, float value);

        inline float time2At(int i) const { return timeline2_[i]; }
        inline float width2At(int i) const { return widths2_[i]; }
        void set2At(int i, float t, float value);

        inline const std::vector<SceneObjectPtr>& objects() const { return objs_; }

        inline float maxWidth() const { return maxWidth_; }
        void setMaxWidth(float value);

        inline const Path& path() const { return path_; }

        float calc1At(int& i, float len);

        float calc2At(int& i, float len);

    private:
        struct Part
        {
            SceneObjectPtr obj;
            b2Vec2 fixtureCenter;
            b2Vec2 points[4];
            b2Vec2 prevPos;
            float prevAngle;
            b2Transform smoothXf;
        };

        typedef std::vector<Part> Parts;

        virtual void onRegister();

        virtual void onUnregister();

        void addTexCoord2(GLfloat x, GLfloat y);

        b2AABB calculateAABB() const;

        void updatePath();

        void updateTexCoords();

        DrawablePtr drawable_;

        b2AABB prevAABB_;
        SInt32 cookie_;

        std::vector<SceneObjectPtr> objs_;
        mutable Parts parts_;
        b2Vec2 p1_;
        b2Vec2 p2_;
        b2Vec2 pN1_;
        b2Vec2 pN2_;

        int N_;
        float len_;
        float step_;
        bool dirty_;

        HermitePath path_;

        std::vector<float> texCoords_;

        std::vector<float> timeline1_;
        std::vector<float> widths1_;

        std::vector<float> timeline2_;
        std::vector<float> widths2_;

        bool flip_;

        float maxWidth_;
    };

    typedef boost::shared_ptr<RenderTentacleComponent> RenderTentacleComponentPtr;
}

#endif
