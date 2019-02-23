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

#ifndef _COLLISIONFILTER_H_
#define _COLLISIONFILTER_H_

#include "SceneObjectManager.h"
#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <set>

namespace af
{
    class SceneObject;

    class CollisionFilter : boost::noncopyable
    {
    public:
        CollisionFilter() {}
        virtual ~CollisionFilter() {}

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const = 0;
    };

    typedef boost::shared_ptr<CollisionFilter> CollisionFilterPtr;

    class CollisionCookieFilter : public CollisionFilter
    {
    public:
        CollisionCookieFilter();
        ~CollisionCookieFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;

        void add(SInt32 cookie);

    private:
        std::set<SInt32> cookies_;
    };

    typedef boost::shared_ptr<CollisionCookieFilter> CollisionCookieFilterPtr;

    class CollisionGroupFilter : public CollisionFilter
    {
    public:
        CollisionGroupFilter();
        ~CollisionGroupFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;

        void add(SInt32 group);

    private:
        std::set<SInt32> groups_;
    };

    typedef boost::shared_ptr<CollisionGroupFilter> CollisionGroupFilterPtr;

    class CollisionDeadbodyFilter : public CollisionFilter
    {
    public:
        CollisionDeadbodyFilter();
        ~CollisionDeadbodyFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;
    };

    typedef boost::shared_ptr<CollisionDeadbodyFilter> CollisionDeadbodyFilterPtr;

    class CollisionTypeFilter : public CollisionFilter
    {
    public:
        CollisionTypeFilter();
        ~CollisionTypeFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;

        inline void setTypes(const SceneObjectTypes& value) { types_ = value; }

    private:
        SceneObjectTypes types_;
    };

    typedef boost::shared_ptr<CollisionTypeFilter> CollisionTypeFilterPtr;

    class CollisionMissileFilter : public CollisionFilter
    {
    public:
        CollisionMissileFilter();
        ~CollisionMissileFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;

        inline void setHitTypes(const SceneObjectTypes& value) { hitTypes_ = value; }
        inline void setMissTypes(const SceneObjectTypes& value) { missTypes_ = value; }

    private:
        SceneObjectTypes hitTypes_;
        SceneObjectTypes missTypes_;
    };

    typedef boost::shared_ptr<CollisionMissileFilter> CollisionMissileFilterPtr;

    class CollisionBuzzSawFilter;
    typedef boost::shared_ptr<CollisionBuzzSawFilter> CollisionBuzzSawFilterPtr;

    CollisionBuzzSawFilterPtr getBuzzSawFilter();

    class CollisionBuzzSawFilter : public CollisionFilter
    {
    public:
        ~CollisionBuzzSawFilter();

        virtual bool shouldCollideWith(b2Fixture* other, b2Fixture* thisOne) const;

    private:
        CollisionBuzzSawFilter();

        friend CollisionBuzzSawFilterPtr getBuzzSawFilter();
    };
}

#endif
