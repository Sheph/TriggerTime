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

#ifndef _HERMITE_PATH_H_
#define _HERMITE_PATH_H_

#include "Path.h"

namespace af
{
    class HermitePath;

    class HermitePathIterator : public PathIterator
    {
    public:
        HermitePathIterator(const HermitePath* path, int i, float pos);
        ~HermitePathIterator();

        virtual PathIteratorPtr clone() const;

        virtual const b2Vec2& current() const;

        virtual bool less(const PathIteratorPtr& other) const;

        virtual bool eq(const PathIteratorPtr& other) const;

        inline bool less(const HermitePathIterator& other) const
        {
            if (i_ == other.i_) {
                return pos_ < other.pos_;
            } else {
                return i_ < other.i_;
            }
        }

        inline bool eq(const HermitePathIterator& other) const
        {
            return (i_ == other.i_) && (pos_ == other.pos_);
        }

        inline bool lessEq(const HermitePathIterator& other) const
        {
            return less(other) || eq(other);
        }

        inline bool gt(const HermitePathIterator& other) const
        {
            return !less(other) && !eq(other);
        }

        inline bool gtEq(const HermitePathIterator& other) const
        {
            return !less(other);
        }

        virtual void step(float length);

    private:
        void stepForward(float length);

        void stepBack(float length);

        const HermitePath* path_;
        int i_;
        float pos_;
        b2Vec2 point_;
    };

    typedef boost::shared_ptr<HermitePathIterator> HermitePathIteratorPtr;

    /*
     * Implementation of cubic hermite spline with
     * cardinal interpolation.
     * See: http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Cardinal_spline
     */
    class HermitePath : public Path
    {
    public:
        HermitePath(int numIterations, float c);
        explicit HermitePath(float c);
        ~HermitePath();

        /*
         * Invalidates iterators.
         * @{
         */
        virtual void add(const b2Vec2& point);
        virtual void add(const b2Vec2* points, size_t numPoints);

        virtual void addFront(const b2Vec2& point);
        virtual void addFront(const b2Vec2* points, size_t numPoints);
        /*
         * @}
         */

        virtual PathPtr clone() const;

        virtual const Points& points() const;

        virtual float length() const;

        virtual PathIteratorPtr first() const;

        virtual PathIteratorPtr last() const;

        virtual PathIteratorPtr find(float pos) const;

        virtual PathIteratorPtr end() const;

        virtual PathIteratorPtr rend() const;

        inline float tension() const { return c_; }

        HermitePathIterator firstIt() const;

        HermitePathIterator lastIt() const;

        HermitePathIterator findIt(float pos) const;

        HermitePathIterator endIt() const;

        HermitePathIterator rendIt() const;

        void clear();

    private:
        friend class HermitePathIterator;

        b2Vec2 at(int i, float t) const;

        float step_;
        float c_;
        Points p_;
        Points m_;

        float length_;
        std::vector<float> lengths_;
        Points samples_;
    };

    typedef boost::shared_ptr<HermitePath> HermitePathPtr;
}

#endif
