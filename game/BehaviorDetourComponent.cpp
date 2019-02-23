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

#include "BehaviorDetourComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Utils.h"
#include <boost/bind.hpp>

namespace af
{
    BehaviorDetourComponent::BehaviorDetourComponent()
    : started_(false)
    {
        reset();
    }

    BehaviorDetourComponent::~BehaviorDetourComponent()
    {
    }

    void BehaviorDetourComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BehaviorDetourComponent::update(float dt)
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

        b2Vec2 dir = parent()->getDirection(aheadDistance_);

        bool found = rayCast(parent()->pos(), dir);

        if (!found) {
            if (!loop()) {
                finished_ = true;
            }
            return;
        }

        b2Rot rotL(seekAngle_);
        b2Rot rotR(-seekAngle_);

        b2Vec2 dirL = dir;
        b2Vec2 dirR = dir;

        for (float a = seekAngle_; a < b2_pi; a += seekAngle_) {
            dirL = b2Mul(rotL, dirL);

            found = rayCast(parent()->pos(), dirL);

            if (!found) {
                dirAngle_ = parent()->angle() + a + (b2_pi * 0.5f);
                dirFactor_ = 1.0f;
            } else {
                dirR = b2Mul(rotR, dirR);

                found = rayCast(parent()->pos(), dirR);

                if (!found) {
                    dirAngle_ = parent()->angle() - a - (b2_pi * 0.5f);
                    dirFactor_ = -1.0f;
                }
            }

            if (!found) {
                haveDir_ = true;

                apply();

                break;
            }
        }
    }

    void BehaviorDetourComponent::reset()
    {
        started_ = false;
        finished_ = false;
        aheadDistance_ = 1.0f;
        seekAngle_ = deg2rad(5);
        angularVelocity_ = 0.0f;
        timeout_ = 0.0f;
        rayDistance_ = 0.0f;
        setLoop(false);

        t_ = 0.0f;
        haveDir_ = false;
    }

    void BehaviorDetourComponent::start()
    {
        if (!scene()) {
            return;
        }

        started_ = true;
        finished_ = false;
    }

    void BehaviorDetourComponent::onRegister()
    {
    }

    void BehaviorDetourComponent::onUnregister()
    {
    }

    bool BehaviorDetourComponent::rayCast(const b2Vec2& pos, const b2Vec2& dir)
    {
        bool found = false;

        if (rayDistance_ == 0.0f) {
            scene()->rayCast(pos,
                pos + dir,
                boost::bind(&BehaviorDetourComponent::rayCastCb, this, _1, _2, _3, _4, boost::ref(found)));
        } else {
            b2Vec2 tmp = dir.Skew();
            tmp.Normalize();
            tmp *= rayDistance_;
            scene()->rayCast(pos + tmp,
                pos + tmp + dir,
                boost::bind(&BehaviorDetourComponent::rayCastCb, this, _1, _2, _3, _4, boost::ref(found)));
            if (!found) {
                found = false;
                scene()->rayCast(pos - tmp,
                    pos - tmp + dir,
                    boost::bind(&BehaviorDetourComponent::rayCastCb, this, _1, _2, _3, _4, boost::ref(found)));
            }
        }

        return found;
    }

    float BehaviorDetourComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, bool& found)
    {
        SceneObject* obj = SceneObject::fromFixture(fixture);

        if (fixture->IsSensor() || (obj == parent()) || !obj->detourable()) {
            return -1;
        }

        if ((obj->type() != SceneObjectTypeRock) &&
            (obj->type() != SceneObjectTypeTerrain) &&
            (obj->type() != SceneObjectTypeEnemyBuilding) &&
            ((obj->type() != SceneObjectTypeBlocker) || !parent()->collidesWith(fixture))) {
            return -1;
        }

        found = true;

        return 0;
    }

    void BehaviorDetourComponent::apply()
    {
        float nextAngle = parent()->angle() + parent()->angularVelocity() / 6.0f;

        if ((nextAngle * dirFactor_) >= (dirAngle_ * dirFactor_)) {
            parent()->applyTorque(parent()->inertia() * (-angularVelocity_ * dirFactor_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
        } else {
            parent()->applyTorque(parent()->inertia() * (angularVelocity_ * dirFactor_ - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
        }
    }
}
