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

#include "CollisionFilter.h"
#include "SceneObject.h"
#include "Const.h"
#include <boost/make_shared.hpp>

namespace af
{
    CollisionCookieFilter::CollisionCookieFilter()
    {
    }

    CollisionCookieFilter::~CollisionCookieFilter()
    {
    }

    bool CollisionCookieFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return cookies_.count(SceneObject::fromFixture(other)->cookie()) == 0;
    }

    void CollisionCookieFilter::add(SInt32 cookie)
    {
        cookies_.insert(cookie);
    }

    CollisionGroupFilter::CollisionGroupFilter()
    {
    }

    CollisionGroupFilter::~CollisionGroupFilter()
    {
    }

    bool CollisionGroupFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return groups_.count(other->GetFilterData().groupIndex) == 0;
    }

    void CollisionGroupFilter::add(SInt32 group)
    {
        groups_.insert(group);
    }

    CollisionDeadbodyFilter::CollisionDeadbodyFilter()
    {
    }

    CollisionDeadbodyFilter::~CollisionDeadbodyFilter()
    {
    }

    bool CollisionDeadbodyFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return thisOne->IsSensor() || SceneObject::fromFixture(other)->deadbodyAware();
    }

    CollisionTypeFilter::CollisionTypeFilter()
    {
    }

    CollisionTypeFilter::~CollisionTypeFilter()
    {
    }

    bool CollisionTypeFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return types_[SceneObject::fromFixture(other)->type()];
    }

    CollisionMissileFilter::CollisionMissileFilter()
    {
    }

    CollisionMissileFilter::~CollisionMissileFilter()
    {
    }

    bool CollisionMissileFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return hitTypes_[SceneObject::fromFixture(other)->type()] ||
            ((other->GetFilterData().groupIndex != static_cast<int16>(collisionGroupMissile)) && !missTypes_[SceneObject::fromFixture(other)->type()]);
    }

    CollisionBuzzSawFilter::CollisionBuzzSawFilter()
    {
    }

    CollisionBuzzSawFilter::~CollisionBuzzSawFilter()
    {
    }

    bool CollisionBuzzSawFilter::shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const
    {
        return (other->GetFilterData().groupIndex != static_cast<int16>(collisionGroupMissile)) &&
               ((SceneObject::fromFixture(other)->type() != SceneObjectTypeEnemyMissile) ||
                (SceneObject::fromFixture(thisOne)->type() != SceneObjectTypeEnemyMissile));
    }

    CollisionBuzzSawFilterPtr getBuzzSawFilter()
    {
        static CollisionBuzzSawFilterPtr buzzSawFilter;

        if (!buzzSawFilter) {
            buzzSawFilter = CollisionBuzzSawFilterPtr(new CollisionBuzzSawFilter());
        }

        return buzzSawFilter;
    }
}
