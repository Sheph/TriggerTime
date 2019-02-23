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

#ifndef _COLLISIONCOMPONENT_H_
#define _COLLISIONCOMPONENT_H_

#include "CollisionComponentManager.h"

namespace af
{
    class CollisionComponent : public Component
    {
    public:
        CollisionComponent()
        : manager_(NULL)
        {
        }

        virtual ~CollisionComponent() {}

        virtual CollisionComponentManager* manager() { return manager_; }
        inline void setManager(CollisionComponentManager* value)
        {
            onSetManager(manager_, value);
        }

        virtual void preSolve(SInt32 cookie, b2Contact* contact) {}

        virtual void beginCollision(const Collision& collision) = 0;

        virtual void updateCollision(const CollisionUpdate& collisionUpdate) = 0;

        virtual void endCollision(SInt32 cookie) = 0;

        virtual void update(float dt) = 0;

    private:
        CollisionComponentManager* manager_;
    };
}

#endif
