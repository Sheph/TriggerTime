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

#include "BehaviorAvoidComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Utils.h"
#include <boost/bind.hpp>

namespace af
{
    std::set<SceneObject*> BehaviorAvoidComponent::tmpObjs_;

    BehaviorAvoidComponent::BehaviorAvoidComponent()
    : started_(false)
    {
        reset();
    }

    BehaviorAvoidComponent::~BehaviorAvoidComponent()
    {
    }

    void BehaviorAvoidComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BehaviorAvoidComponent::update(float dt)
    {
        if (!started_ || finished() || parent()->stunned()) {
            return;
        }

        t_ -= dt;

        if (t_ > 0.0f) {
            if (haveDir_) {
                apply();
            } else if (!loop()) {
                finished_ = true;
            }
            return;
        }

        haveDir_ = false;
        t_ = timeout_;

        b2AABB aabb;

        aabb.lowerBound = parent()->pos() - b2Vec2(radius_, radius_);
        aabb.upperBound = parent()->pos() + b2Vec2(radius_, radius_);

        tmpObjs_.clear();

        b2Vec2 accum = b2Vec2_zero;

        scene()->queryAABB(aabb,
            boost::bind(&BehaviorAvoidComponent::queryAABBCb, this, _1,
                boost::ref(accum)));

        if (tmpObjs_.empty()) {
            if (!loop()) {
                finished_ = true;
            }
            return;
        }

        accum = -accum;

        accum.Normalize();

        haveDir_ = true;
        dir_ = accum;

        apply();
    }

    void BehaviorAvoidComponent::reset()
    {
        started_ = false;
        finished_ = false;
        types_ = SceneObjectTypeEnemy;
        radius_ = 1.0f;
        linearVelocity_ = 0.0f;
        timeout_ = 0.0f;
        setLoop(false);

        t_ = 0.0f;
        haveDir_ = false;
    }

    void BehaviorAvoidComponent::start()
    {
        if (!scene()) {
            return;
        }

        started_ = true;
        finished_ = false;
    }

    void BehaviorAvoidComponent::onRegister()
    {
    }

    void BehaviorAvoidComponent::onUnregister()
    {
    }

    bool BehaviorAvoidComponent::queryAABBCb(b2Fixture* fixture, b2Vec2& accum)
    {
        if (fixture->IsSensor()) {
            return true;
        }

        SceneObject* obj = SceneObject::fromFixture(fixture);

        if ((obj == parent()) || (obj->parent() == parent()) ||
            !types_[obj->type()]) {
            return true;
        }

        if (tmpObjs_.count(obj) > 0) {
            return true;
        }

        b2Vec2 tmp = obj->pos() - parent()->pos();

        if (tmp.Length() > radius_) {
            return true;
        }

        accum += tmp;

        tmpObjs_.insert(obj);

        return true;
    }

    void BehaviorAvoidComponent::apply()
    {
        parent()->applyForceToCenter(parent()->mass() * (linearVelocity_ + parent()->linearVelocityDamped()) * dir_, true);
    }
}
