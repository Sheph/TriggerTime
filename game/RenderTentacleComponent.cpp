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

#include "RenderTentacleComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"
#include "Settings.h"
#include <boost/make_shared.hpp>

namespace af
{
    RenderTentacleComponent::RenderTentacleComponent(const std::vector<SceneObjectPtr>& objs,
        const std::vector<RUBEPolygonFixturePtr>& fixtures,
        const DrawablePtr& drawable,
        int zOrder,
        int numIterations, float c, float step)
    : RenderComponent(zOrder),
      drawable_(drawable),
      cookie_(0),
      objs_(objs),
      N_(0),
      len_(0.0f),
      step_(step <= 0.0f ? settings.tentacle.step : step),
      dirty_(false),
      path_((numIterations <= 0 ? settings.tentacle.numIterations : numIterations), c),
      flip_(false),
      maxWidth_(0.0f)
    {
        assert(objs.size() == fixtures.size());

        b2Transform xf;
        xf.SetIdentity();

        for (size_t i = 0; i < objs.size(); ++i) {
            parts_.push_back(Part());
            Part& part = parts_.back();

            part.obj = objs[i];
            part.fixtureCenter = fixtures[i]->shape().m_centroid;

            b2AABB aabb;
            fixtures[i]->shape().ComputeAABB(&aabb, xf, 0);

            b2Vec2 tmp = aabb.GetExtents();

            part.points[0] = aabb.lowerBound - tmp;
            part.points[1] = aabb.upperBound + tmp;
            part.points[2] = b2Vec2(aabb.lowerBound.x, aabb.upperBound.y) + b2Vec2(-tmp.x, tmp.y);
            part.points[3] = b2Vec2(aabb.upperBound.x, aabb.lowerBound.y) + b2Vec2(tmp.x, -tmp.y);
        }

        b2AABB aabb;

        fixtures[0]->shape().ComputeAABB(&aabb, xf, 0);

        float dist = parts_[0].fixtureCenter.x - aabb.lowerBound.x;

        p2_ = parts_[0].fixtureCenter - b2Vec2(dist, 0.0f);
        p1_ = p2_ - b2Vec2(dist, 0.0f);

        fixtures.back()->shape().ComputeAABB(&aabb, xf, 0);

        dist = aabb.upperBound.x - parts_.back().fixtureCenter.x;

        pN1_ = parts_.back().fixtureCenter + b2Vec2(dist, 0.0f);
        pN2_ = pN1_ + b2Vec2(dist, 0.0f);
    }

    RenderTentacleComponent::RenderTentacleComponent(const std::vector<SceneObjectPtr>& objs,
        const std::vector<RUBECircleFixturePtr>& fixtures,
        const DrawablePtr& drawable,
        int zOrder,
        int numIterations, float c, float step)
    : RenderComponent(zOrder),
      drawable_(drawable),
      cookie_(0),
      objs_(objs),
      N_(0),
      len_(0.0f),
      step_(step <= 0.0f ? settings.tentacle.step : step),
      dirty_(false),
      path_((numIterations <= 0 ? settings.tentacle.numIterations : numIterations), c),
      flip_(false),
      maxWidth_(0.0f)
    {
        assert(objs.size() == fixtures.size());

        b2Transform xf;
        xf.SetIdentity();

        for (size_t i = 0; i < objs.size(); ++i) {
            parts_.push_back(Part());
            Part& part = parts_.back();

            part.obj = objs[i];
            part.fixtureCenter = fixtures[i]->shape().m_p;

            b2AABB aabb;
            fixtures[i]->shape().ComputeAABB(&aabb, xf, 0);

            b2Vec2 tmp = aabb.GetExtents();

            part.points[0] = aabb.lowerBound - tmp;
            part.points[1] = aabb.upperBound + tmp;
            part.points[2] = b2Vec2(aabb.lowerBound.x, aabb.upperBound.y) + b2Vec2(-tmp.x, tmp.y);
            part.points[3] = b2Vec2(aabb.upperBound.x, aabb.lowerBound.y) + b2Vec2(tmp.x, -tmp.y);
        }

        b2AABB aabb;

        fixtures[0]->shape().ComputeAABB(&aabb, xf, 0);

        float dist = parts_[0].fixtureCenter.x - aabb.lowerBound.x;

        p2_ = parts_[0].fixtureCenter - b2Vec2(dist, 0.0f);
        p1_ = p2_ - b2Vec2(dist, 0.0f);

        fixtures.back()->shape().ComputeAABB(&aabb, xf, 0);

        dist = aabb.upperBound.x - parts_.back().fixtureCenter.x;

        pN1_ = parts_.back().fixtureCenter + b2Vec2(dist, 0.0f);
        pN2_ = pN1_ + b2Vec2(dist, 0.0f);
    }

    RenderTentacleComponent::~RenderTentacleComponent()
    {
    }

    void RenderTentacleComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderTentacleComponent::update(float dt)
    {
        bool dirty = false;

        b2Vec2 prevPos = parts_[0].prevPos;

        for (size_t i = 0; i < parts_.size(); ++i) {
            if ((parts_[i].obj->smoothPos() != parts_[i].prevPos) ||
                (parts_[i].obj->smoothAngle() != parts_[i].prevAngle)) {
                parts_[i].prevPos = parts_[i].obj->smoothPos();
                parts_[i].prevAngle = parts_[i].obj->smoothAngle();
                dirty = true;
            }
        }

        if (!dirty) {
            return;
        }

        dirty_ = true;

        b2AABB aabb = calculateAABB();

        b2Vec2 displacement = parts_[0].obj->smoothPos() - prevPos;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevAABB_ = aabb;
    }

    void RenderTentacleComponent::render(void* const* parts, size_t numParts)
    {
        if (dirty_) {
            dirty_ = false;
            updatePath();
        }

        renderer.setProgramDef(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        HermitePathIterator it = path_.firstIt();
        HermitePathIterator lastIt = path_.lastIt();

        int timeIndex1 = 1;
        int timeIndex2 = 1;

        b2Vec2 tmp = (path_.points()[1] - path_.points()[0]).Skew();
        tmp.Normalize();

        float k = flip_ ? 1.0f : -1.0f;

        b2Vec2 prev = it.current();
        b2Vec2 pu = prev + k * calc1At(timeIndex1, 0.0f) * tmp;
        b2Vec2 pl = prev - k * calc2At(timeIndex2, 0.0f) * tmp;

        float len = 0.0f;

        float factor = path_.length() / len_;

        float step = step_ * factor;

        it.step(step);
        len += step;

        int i = 0;

        while (it.less(lastIt) && (i < N_)) {
            b2Vec2 v = it.current();

            b2Vec2 tmp = (v - prev).Skew();
            tmp.Normalize();

            b2Vec2 cpl, cpu;

            cpu = v + k * calc1At(timeIndex1, len) * tmp;
            cpl = v - k * calc2At(timeIndex2, len) * tmp;

            rop.addVertex(pu.x, pu.y);
            rop.addVertex(pl.x, pl.y);
            rop.addVertex(cpl.x, cpl.y);

            rop.addVertex(pu.x, pu.y);
            rop.addVertex(cpl.x, cpl.y);
            rop.addVertex(cpu.x, cpu.y);

            it.step(step);
            len += step;

            prev = v;
            pu = cpu;
            pl = cpl;

            ++i;
        }

        rop.addColors(color());

        rop.addTexCoords(&texCoords_[0], i * 6);
    }

    void RenderTentacleComponent::debugDraw()
    {
        updatePath();

        renderer.setProgramColorLines(2.0f);

        RenderLineStrip rop1 = renderer.renderLineStrip();

        HermitePathIterator it = path_.firstIt();
        HermitePathIterator lastIt = path_.lastIt();

        while (it.less(lastIt)) {
            b2Vec2 v = it.current();

            rop1.addVertex(v.x, v.y);

            it.step(step_);
        }

        {
            b2Vec2 v = lastIt.current();

            rop1.addVertex(v.x, v.y);
        }

        rop1.addColors(Color(1.0f, 1.0f, 1.0f));

        renderer.setProgramColorPoints(5.0f);

        RenderSimple rop2 = renderer.renderPoints();

        for (Points::const_iterator pIt = path_.points().begin();
             pIt != path_.points().end();
             ++pIt) {
            b2Vec2 v = *pIt;

            rop2.addVertex(v.x, v.y);
        }

        rop2.addColors(Color(1.0f, 0.0f, 0.0f));
    }

    void RenderTentacleComponent::onRegister()
    {
        for (size_t i = 0; i < parts_.size(); ++i) {
            parts_[i].prevPos = parts_[i].obj->smoothPos();
            parts_[i].prevAngle = parts_[i].obj->smoothAngle();
        }

        prevAABB_ = calculateAABB();
        cookie_ = manager()->addAABB(this, prevAABB_, NULL);

        if (maxWidth_ == 0.0f) {
            float maxWidth1 = 0.0f;
            for (size_t i = 0; i < widths1_.size(); ++i) {
                if (widths1_[i] > maxWidth1) {
                    maxWidth1 = widths1_[i];
                }
            }

            float maxWidth2 = 0.0f;
            for (size_t i = 0; i < widths2_.size(); ++i) {
                if (widths2_[i] > maxWidth2) {
                    maxWidth2 = widths2_[i];
                }
            }

            maxWidth_ = maxWidth1 + maxWidth2;

            updateTexCoords();
        }
    }

    void RenderTentacleComponent::onUnregister()
    {
        parts_.clear();
        objs_.clear();
        manager()->removeAABB(cookie_);
    }

    void RenderTentacleComponent::addTexCoord2(GLfloat x, GLfloat y)
    {
        texCoords_.push_back(x);
        texCoords_.push_back(y);
    }

    b2AABB RenderTentacleComponent::calculateAABB() const
    {
        static std::vector<b2Vec2> tmp;

        tmp.clear();

        for (size_t i = 0; i < parts_.size(); ++i) {
            parts_[i].smoothXf = parts_[i].obj->getSmoothTransform();
            tmp.push_back(b2Mul(parts_[i].smoothXf, parts_[i].points[0]));
            tmp.push_back(b2Mul(parts_[i].smoothXf, parts_[i].points[1]));
            tmp.push_back(b2Mul(parts_[i].smoothXf, parts_[i].points[2]));
            tmp.push_back(b2Mul(parts_[i].smoothXf, parts_[i].points[3]));
        }

        return computeAABB(&tmp[0], tmp.size());
    }

    void RenderTentacleComponent::updatePath()
    {
        path_.clear();

        path_.add(b2Mul(parts_.back().smoothXf, pN2_));
        path_.add(b2Mul(parts_.back().smoothXf, pN1_));

        for (size_t i = parts_.size(); i > 0; --i) {
            b2Vec2 v = b2Mul(parts_[i - 1].smoothXf, parts_[i - 1].fixtureCenter);

            path_.add(v);
        }

        path_.add(b2Mul(parts_[0].smoothXf, p2_));
        path_.add(b2Mul(parts_[0].smoothXf, p1_));
    }

    void RenderTentacleComponent::resetTimeline1(int num, float value)
    {
        timeline1_.resize(num, 0.0f);
        widths1_.resize(num, value);
    }

    void RenderTentacleComponent::resetTimeline2(int num, float value)
    {
        timeline2_.resize(num, 0.0f);
        widths2_.resize(num, value);
    }

    void RenderTentacleComponent::set1At(int i, float t, float value)
    {
        timeline1_[i] = t;
        widths1_[i] = value;
    }

    void RenderTentacleComponent::set2At(int i, float t, float value)
    {
        timeline2_[i] = t;
        widths2_[i] = value;
    }

    float RenderTentacleComponent::calc1At(int& i, float len)
    {
        float a = len / path_.length();

        int endIndex = -1;

        for (; i < static_cast<int>(timeline1_.size()); ++i) {
            if (timeline1_[i] > a) {
                endIndex = i;
                break;
            }
        }

        if (endIndex == -1) {
            return widths1_[timeline1_.size() - 1];
        }

        int startIndex = endIndex - 1;
        float startValue = widths1_[startIndex];
        float startTime = timeline1_[startIndex];

        return startValue +
            (widths1_[endIndex] - startValue) *
            ((a - startTime) / (timeline1_[endIndex] - startTime));
    }

    float RenderTentacleComponent::calc2At(int& i, float len)
    {
        float a = len / path_.length();

        int endIndex = -1;

        for (; i < static_cast<int>(timeline2_.size()); ++i) {
            if (timeline2_[i] > a) {
                endIndex = i;
                break;
            }
        }

        if (endIndex == -1) {
            return widths2_[timeline2_.size() - 1];
        }

        int startIndex = endIndex - 1;
        float startValue = widths2_[startIndex];
        float startTime = timeline2_[startIndex];

        return startValue +
            (widths2_[endIndex] - startValue) *
            ((a - startTime) / (timeline2_[endIndex] - startTime));
    }

    void RenderTentacleComponent::setMaxWidth(float value)
    {
        if (value != maxWidth_) {
            maxWidth_ = value;
            updateTexCoords();
        }
    }

    void RenderTentacleComponent::updateTexCoords()
    {
        updatePath();

        HermitePathIterator it = path_.firstIt();
        HermitePathIterator lastIt = path_.lastIt();

        len_ = path_.length();

        float len = 0.0f;

        it.step(step_);
        len += step_;

        texCoords_.clear();
        N_ = 0;

        while (it.less(lastIt)) {
            float ll = len / (drawable_->image().aspect() * maxWidth_);

            len += step_;

            float nll = len / (drawable_->image().aspect() * maxWidth_);

            addTexCoord2(ll, drawable_->image().texCoords()[1]);
            addTexCoord2(ll, drawable_->image().texCoords()[5]);
            addTexCoord2(nll, drawable_->image().texCoords()[5]);

            addTexCoord2(ll, drawable_->image().texCoords()[1]);
            addTexCoord2(nll, drawable_->image().texCoords()[5]);
            addTexCoord2(nll, drawable_->image().texCoords()[1]);

            it.step(step_);

            ++N_;
        }
    }
}
