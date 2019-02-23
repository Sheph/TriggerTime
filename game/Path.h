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

#ifndef _PATH_H_
#define _PATH_H_

#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class PathIterator;
    typedef boost::shared_ptr<PathIterator> PathIteratorPtr;

    class PathIterator
    {
    public:
        PathIterator()
        : loop_(false)
        {
        }

        virtual ~PathIterator() {}

        virtual PathIteratorPtr clone() const = 0;

        virtual const b2Vec2& current() const = 0;

        virtual bool less(const PathIteratorPtr& other) const = 0;

        virtual bool eq(const PathIteratorPtr& other) const = 0;

        inline bool lessEq(const PathIteratorPtr& other) const
        {
            return less(other) || eq(other);
        }

        inline bool gt(const PathIteratorPtr& other) const
        {
            return !less(other) && !eq(other);
        }

        inline bool gtEq(const PathIteratorPtr& other) const
        {
            return !less(other);
        }

        virtual void step(float length) = 0;

        inline bool loop() const { return loop_; }
        inline void setLoop(bool value) { loop_ = value; }

    private:
        bool loop_;
    };

    class Path;
    typedef boost::shared_ptr<Path> PathPtr;

    class Path
    {
    public:
        Path() {}
        virtual ~Path() {}

        virtual PathPtr clone() const = 0;

        virtual void add(const b2Vec2& point) = 0;

        virtual void add(const b2Vec2* points, size_t numPoints) = 0;

        virtual void addFront(const b2Vec2& point) = 0;

        virtual void addFront(const b2Vec2* points, size_t numPoints) = 0;

        virtual const Points& points() const = 0;

        virtual float length() const = 0;

        virtual PathIteratorPtr first() const = 0;

        virtual PathIteratorPtr last() const = 0;

        virtual PathIteratorPtr find(float pos) const = 0;

        virtual PathIteratorPtr end() const = 0;

        virtual PathIteratorPtr rend() const = 0;
    };
}

#endif
