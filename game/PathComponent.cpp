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

#include "PathComponent.h"
#include "Settings.h"
#include "Renderer.h"
#include "SceneObject.h"

namespace af
{
    static std::vector<float> tmpLines;
    static std::vector<float> tmpPoints;

    static const float pathIncrement = 0.2f;

    static UInt32 pathDebugIndex = 0;

    static const Color pathColors[][2] =
    {
        { Color(1.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 1.0f) },
        { Color(1.0f, 1.0f, 0.0f), Color(1.0f, 0.0f, 0.0f) },
        { Color(1.0f, 0.0f, 1.0f), Color(0.0f, 1.0f, 1.0f) },
        { Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f) },
        { Color(0.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f) }
    };

    static const int SideIntValues[] =
    {
        PathComponent::SideLeft,
        PathComponent::SideRight,
        PathComponent::SideTop,
        PathComponent::SideBottom
    };

    static const char* SideStrValues[] =
    {
        "left",
        "right",
        "top",
        "bottom"
    };

    AF_ENUMTRAITS_IMPL(Side, PathComponent::Side);

    PathComponent::PathComponent()
    : PhasedComponent(phaseThink),
      debugIndex_(++pathDebugIndex)
    {
    }

    PathComponent::~PathComponent()
    {
    }

    void PathComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PathComponent::update(float dt)
    {
    }

    void PathComponent::debugDraw()
    {
        if (!settings.debugPath || !path_ || path_->points().empty()) {
            return;
        }

        UInt32 ci = debugIndex_ % (sizeof(pathColors)/sizeof(pathColors[0]));

        renderer.setProgramColorLines(2.0f);

        RenderLineStrip rop1 = renderer.renderLineStrip();

        PathIteratorPtr it = path_->first();
        PathIteratorPtr lastIt = path_->last();

        const b2Transform& xf = parent()->getTransform();

        while (it->less(lastIt)) {
            b2Vec2 v = b2Mul(xf, it->current());

            rop1.addVertex(v.x, v.y);

            it->step(pathIncrement);
        }

        {
            b2Vec2 v = b2Mul(xf, lastIt->current());

            rop1.addVertex(v.x, v.y);
        }

        rop1.addColors(pathColors[ci][0]);

        renderer.setProgramColorPoints(5.0f);

        RenderSimple rop2 = renderer.renderPoints();

        for (Points::const_iterator pIt = path_->points().begin();
             pIt != path_->points().end();
             ++pIt) {
            b2Vec2 v = b2Mul(xf, *pIt);

            rop2.addVertex(v.x, v.y);
        }

        rop2.addColors(pathColors[ci][1]);
    }

    void PathComponent::onRegister()
    {
    }

    void PathComponent::onUnregister()
    {
    }
}
