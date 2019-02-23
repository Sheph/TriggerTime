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

#include "PhysicsRopeComponent.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    PhysicsRopeComponent::PhysicsRopeComponent(const Objects& objects, float segmentWidth)
    : objects_(objects), segmentWidth_(segmentWidth), hit_(false)
    {
    }

    PhysicsRopeComponent::~PhysicsRopeComponent()
    {
    }

    void PhysicsRopeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhysicsComponent(shared_from_this());
    }

    void PhysicsRopeComponent::cutTail(size_t n)
    {
        if (objects_.size() <= 2) {
            return;
        }

        n = (std::min)(objects_.size() - 2, n);

        for (size_t i = objects_.size() - n; i < objects_.size(); ++i) {
            objects_[i]->removeFromParent();
        }

        objects_.erase(objects_.begin() + (objects_.size() - n), objects_.end());

        if (objects_.size() > 2) {
            for (size_t i = 2; i < objects_.size(); ++i) {
                scene()->addRopeJoint(objects_.back()->shared_from_this(),
                    objects_[objects_.size() - i - 1]->shared_from_this(),
                    b2Vec2_zero, b2Vec2_zero,
                    segmentWidth_ * i);
            }
        }
    }

    void PhysicsRopeComponent::onRegister()
    {
        for (size_t i = 1; i < objects_.size(); ++i) {
            scene()->addRevoluteJoint(objects_[i - 1]->shared_from_this(),
                objects_[i]->shared_from_this(),
                b2Vec2(-segmentWidth_ / 2, 0),
                b2Vec2(segmentWidth_ / 2, 0));
        }

        if (objects_.size() > 2) {
            /*
             * It's not enough to join rope segments with a revolute joint,
             * Box2D solver only solves joints locally, what this means is that
             * rope segments that are far away from each other can "break", i.e.
             * the rope can go crazy if one segment of the rope is fixed and
             * the other is moving fast. Solution - use rope joints in between
             * rope segments. Ideally this requires n^2 joints, but we'll settle
             * with much less for now.
             */
            for (size_t i = 1; i < objects_.size(); ++i) {
                scene()->addRopeJoint(objects_.front()->shared_from_this(),
                    objects_[i]->shared_from_this(),
                    b2Vec2_zero, b2Vec2_zero,
                    segmentWidth_ * i);
                scene()->addRopeJoint(objects_.back()->shared_from_this(),
                    objects_[objects_.size() - i - 1]->shared_from_this(),
                    b2Vec2_zero, b2Vec2_zero,
                    segmentWidth_ * i);
                scene()->addRopeJoint(objects_[i - 1]->shared_from_this(),
                    objects_[i]->shared_from_this(),
                    b2Vec2_zero, b2Vec2_zero,
                    segmentWidth_);
            }
        }
    }

    void PhysicsRopeComponent::onUnregister()
    {
    }
}
