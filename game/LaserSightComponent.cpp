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

#include "LaserSightComponent.h"
#include "Scene.h"

namespace af
{
    LaserSightComponent::LaserSightComponent(const PlayerComponentPtr& pc,
        const RenderTrailComponentPtr& rc)
    : PhasedComponent(phasePreRender),
      pc_(pc),
      rc_(rc)
    {
    }

    LaserSightComponent::~LaserSightComponent()
    {
    }

    void LaserSightComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void LaserSightComponent::preRender(float dt)
    {
        if (!scene()->inputPlayer()->active() ||
            (pc_->altWeapon() &&
            (pc_->altWeapon()->aiming() || pc_->altWeapon()->charged()))) {
            rc_->setVisible(false);
            return;
        }

        if (!pc_->weapon() ||
            (pc_->weapon()->weaponType() != WeaponTypeRLauncher)) {
            rc_->setVisible(false);
            return;
        }

        rc_->setVisible(true);

        b2AABB aabb = c_->getTrueAABB();

        b2Vec2 ext = aabb.GetExtents();

        b2RayCastInput input;

        b2Transform xf = b2Mul(parent()->getSmoothTransform(), rc_->transform());

        input.p2 = b2Mul(xf, b2Vec2_zero);
        input.p1 = input.p2 + b2Mul(xf.q, b2Vec2((ext.x * ext.x + ext.y * ext.y) * 4.0f, 0.0f));
        input.maxFraction = 1.0f;

        b2RayCastOutput output;

        xf.SetIdentity();

        b2PolygonShape boundary;

        boundary.SetAsBox(ext.x, ext.y, aabb.GetCenter(), 0.0f);

        if (!boundary.RayCast(&output, input, xf, 0)) {
            return;
        }

        rc_->setLength((1.0f - output.fraction) * (input.p2 - input.p1).Length());
    }

    void LaserSightComponent::onRegister()
    {
        c_ = scene()->camera()->findComponent<CameraComponent>();
    }

    void LaserSightComponent::onUnregister()
    {
    }
}
