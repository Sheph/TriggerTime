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

#ifndef _COLLISIONDIECOMPONENT_H_
#define _COLLISIONDIECOMPONENT_H_

#include "CollisionComponent.h"
#include "SceneObject.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    /*
     * When it collides with something it stores the object
     * collided with and removes itself and PhysicsComponent from
     * the scene. The object itself is not removed. The point is
     * to stop all further physics and collision calculations on
     * the object, but give a chance to process the collision
     * on think phase, i.e. make an explosion.
     */
    class CollisionDieComponent : public boost::enable_shared_from_this<CollisionDieComponent>,
                                  public CollisionComponent
    {
    public:
        CollisionDieComponent();
        ~CollisionDieComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void beginCollision(const Collision& collision);

        virtual void updateCollision(const CollisionUpdate& collisionUpdate);

        virtual void endCollision(SInt32 cookie);

        virtual void update(float dt);

        inline const SceneObjectPtr& collidedWith() const { return collidedWith_; }

        inline const b2Vec2& collisionPoint() const { return collisionPoint_; }

        inline const b2Vec2& collisionNormal() const { return collisionNormal_; }

        inline bool allowSensor() const { return allowSensor_; }
        inline void setAllowSensor(bool value) { allowSensor_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr collidedWith_;

        b2Vec2 collisionPoint_;
        b2Vec2 collisionNormal_;

        bool allowSensor_;
    };

    typedef boost::shared_ptr<CollisionDieComponent> CollisionDieComponentPtr;
}

#endif
