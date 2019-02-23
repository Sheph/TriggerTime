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

#ifndef _COLLISIONBUZZSAWMISSILECOMPONENT_H_
#define _COLLISIONBUZZSAWMISSILECOMPONENT_H_

#include "CollisionComponent.h"
#include "SceneObjectManager.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class CollisionBuzzSawMissileComponent : public boost::enable_shared_from_this<CollisionBuzzSawMissileComponent>,
                                             public CollisionComponent
    {
    public:
        CollisionBuzzSawMissileComponent();
        ~CollisionBuzzSawMissileComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preSolve(SInt32 cookie, b2Contact* contact);

        virtual void beginCollision(const Collision& collision);

        virtual void updateCollision(const CollisionUpdate& collisionUpdate);

        virtual void endCollision(SInt32 cookie);

        virtual void update(float dt);

        inline void setDamage(float value) { damage_ = value; }

    private:
        typedef std::set<SceneObjectPtr> HitSet;

        virtual void onRegister();

        virtual void onUnregister();

        void hit(SceneObject* otherObj, const b2Vec2& pt);

        HitSet hitSet_;
        AudioSourcePtr sndHit_;
        float damage_;
        int numEnemiesKilled_;
    };

    typedef boost::shared_ptr<CollisionBuzzSawMissileComponent> CollisionBuzzSawMissileComponentPtr;
}

#endif
