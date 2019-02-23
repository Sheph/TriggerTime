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

#include "CollisionSensorComponent.h"

namespace af
{
    CollisionSensorComponent::CollisionSensorComponent()
    : allowSensor_(false)
    {
    }

    CollisionSensorComponent::~CollisionSensorComponent()
    {
    }

    void CollisionSensorComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void CollisionSensorComponent::beginCollision(const Collision& collision)
    {
        if (!collision.getThisFixture(parent())->IsSensor()) {
            return;
        }

        if (!allowSensor_ && collision.getOtherFixture(parent())->IsSensor()) {
            return;
        }

        SceneObjectPtr obj = collision.getOther(parent())->shared_from_this();

        objects_[collision.cookie] = obj;

        if (listener_) {
            listener_->sensorEnter(obj);
        }
    }

    void CollisionSensorComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
    }

    void CollisionSensorComponent::endCollision(SInt32 cookie)
    {
        ObjectMap::iterator it = objects_.find(cookie);

        if (it == objects_.end()) {
            return;
        }

        if (listener_) {
            listener_->sensorExit(it->second);
        }

        objects_.erase(it);
    }

    void CollisionSensorComponent::update(float dt)
    {
    }

    void CollisionSensorComponent::onRegister()
    {
    }

    void CollisionSensorComponent::onUnregister()
    {
        objects_.clear();
        setListener(SensorListenerPtr());
    }
}
