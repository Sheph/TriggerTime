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

#include "Light.h"
#include "Settings.h"
#include "RenderLightComponent.h"
#include "SceneObject.h"

namespace af
{
    Light::Light(const std::string& name)
    : name_(name),
      diffuse_(false),
      pos_(b2Vec2_zero),
      angle_(0),
      parent_(NULL),
      cookie_(0),
      numRays_(settings.light.numRays),
      color_(1.0f, 1.0f, 1.0f, 1.0f),
      visible_(true),
      softLength_(2.5f),
      xray_(false),
      dynamic_(true),
      reverse_(false),
      intensity_(1.0f),
      nearOffset_(0.0f),
      hitBlockers_(false),
      dirty_(true)
    {
        prevAABB_.lowerBound = prevAABB_.upperBound = b2Vec2_zero;
        xf_.SetIdentity();
        finalXf_ = xf_;
    }

    Light::~Light()
    {
    }

    b2AABB Light::adopt(RenderLightComponent* parent)
    {
        parent_ = parent;
        prevAbsPos_ = b2Mul(finalXf_, b2Vec2_zero);
        prevAABB_ = calculateAABB();

        return prevAABB_;
    }

    void Light::abandon()
    {
        parent_ = NULL;
    }

    float Light::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
        float& finalFraction)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        SceneObject* obj = SceneObject::fromFixture(fixture);

        if ((obj->type() != SceneObjectTypeRock) &&
            (obj->type() != SceneObjectTypeTerrain) &&
            (!hitBlockers_ || (obj->type() != SceneObjectTypeBlocker))) {
            return -1;
        }

        if (obj->glassy()) {
            return -1;
        }

        finalPoint = point;
        finalFraction = fraction;

        return fraction;
    }

    void Light::setPos(const b2Vec2& value)
    {
        if (pos_ != value) {
            pos_ = value;
            finalXf_ = b2Mul(xf_, b2Transform(pos_, b2Rot(angle_)));
            setDirty();
        }
    }

    void Light::setAngle(float value)
    {
        if (angle_ != value) {
            angle_ = value;
            finalXf_ = b2Mul(xf_, b2Transform(pos_, b2Rot(angle_)));
            setDirty();
        }
    }

    void Light::setSoftLength(float value)
    {
        if (softLength_ != value) {
            softLength_ = value;
            setDirty();
        }
    }

    void Light::setWorldTransform(const b2Transform& value)
    {
        if ((xf_.p != value.p) ||
            (xf_.q.c != value.q.c) ||
            (xf_.q.s != value.q.s)) {
            xf_ = value;
            finalXf_ = b2Mul(xf_, b2Transform(pos_, b2Rot(angle_)));
            setDirty();
        }
    }

    void Light::setNumRays(UInt32 value)
    {
        numRays_ = value;
    }

    void Light::setColor(const Color& value)
    {
        color_ = value;
        setDirty();
    }

    void Light::setIntensity(float value)
    {
        intensity_ = value;
        setDirty();
    }

    void Light::setNearOffset(float value)
    {
        if (nearOffset_ != value) {
            nearOffset_ = value;
            setDirty();
        }
    }

    bool Light::updateAABB(b2AABB& prevAABB, b2AABB& aabb, b2Vec2& displacement)
    {
        if (!dirty_) {
            return false;
        }

        aabb = calculateAABB();
        prevAABB = prevAABB_;

        b2Vec2 absPos = b2Mul(finalXf_, b2Vec2_zero);

        displacement = absPos - prevAbsPos_;

        prevAbsPos_ = absPos;
        prevAABB_ = aabb;
        dirty_ = false;

        return true;
    }

    void Light::remove()
    {
        if (parent()) {
            parent()->removeLight(sharedThis());
        }
    }

    void Light::setDirty()
    {
        dirty_ = true;
    }
}
