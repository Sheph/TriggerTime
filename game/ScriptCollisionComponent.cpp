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

#include "ScriptCollisionComponent.h"
#include "Utils.h"
#include "Logger.h"
#include "SceneObject.h"
#include "ScriptConverters.h"

namespace af
{
    ScriptCollisionComponent::ScriptCollisionComponent(luabind::object self)
    : self_(self)
    {
    }

    ScriptCollisionComponent::~ScriptCollisionComponent()
    {
    }

    void ScriptCollisionComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitCollisionComponent(shared_from_this());
    }

    void ScriptCollisionComponent::beginCollision(const Collision& collision)
    {
        std::vector<b2Vec2> points;
        std::vector<float> normalImpulses, tangentImpulses;

        for (int i = 0; i < collision.pointCount; ++i) {
            points.push_back(collision.worldManifold.points[i]);
            normalImpulses.push_back(collision.normalImpulses[i]);
            tangentImpulses.push_back(collision.tangentImpulses[i]);
        }

        AF_SCRIPT_CALL("beginCollision",
            collision.cookie,
            collision.getOther(parent())->shared_from_this(),
            points,
            normalImpulses,
            tangentImpulses);
    }

    void ScriptCollisionComponent::updateCollision(const CollisionUpdate& collisionUpdate)
    {
        std::vector<float> normalImpulses, tangentImpulses;

        for (int i = 0; i < collisionUpdate.pointCount; ++i) {
            normalImpulses.push_back(collisionUpdate.normalImpulses[i]);
            tangentImpulses.push_back(collisionUpdate.tangentImpulses[i]);
        }

        AF_SCRIPT_CALL("updateCollision",
            collisionUpdate.cookie,
            normalImpulses,
            tangentImpulses);
    }

    void ScriptCollisionComponent::endCollision(SInt32 cookie)
    {
        AF_SCRIPT_CALL("endCollision", cookie);
    }

    void ScriptCollisionComponent::update(float dt)
    {
        AF_SCRIPT_CALL("update", dt);
    }

    void ScriptCollisionComponent::onRegister()
    {
        AF_SCRIPT_CALL("onRegister");
    }

    void ScriptCollisionComponent::onUnregister()
    {
        AF_SCRIPT_CALL("onUnregister");
        self_ = luabind::object();
    }
}
