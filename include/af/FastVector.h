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

#ifndef _AF_FASTVECTOR_H_
#define _AF_FASTVECTOR_H_

#include <vector>
#include <cstddef>
#include <cassert>
#include <string.h>
#include <stdlib.h>

namespace af
{
    /*
     * We need to use this crap instead of std::vector<T> in some places to
     * avoid default value initialization overhead. std::vector<T> has absolutely
     * no means to resize or copy stuff to the end without doing std::fill first.
     * Sick...
     */
    template <class T>
    class FastVector
    {
    public:
        typedef std::vector<T> SlowVector;

        FastVector()
        : size_(0)
        {
        }

        ~FastVector()
        {
        }

        inline bool empty() const
        {
            return size_ == 0;
        }

        inline typename SlowVector::iterator begin()
        {
            return v_.begin();
        }

        inline typename SlowVector::const_iterator begin() const
        {
            return v_.begin();
        }

        inline typename SlowVector::iterator end()
        {
            return v_.begin() + size_;
        }

        inline typename SlowVector::const_iterator end() const
        {
            return v_.begin() + size_;
        }

        inline void insert(typename SlowVector::iterator position,
            const T* first, const T* last)
        {
            size_t numInsert = last - first;

            if (position == (v_.begin() + size_)) {
                if ((size_ + numInsert) > v_.size()) {
                    v_.resize(size_ + numInsert);
                }
                ::memcpy(&v_[size_], first, numInsert * sizeof(T));
            } else {
                /*
                 * TODO: implement when needed.
                 */
                assert(false);
                exit(1);
            }
            size_ += numInsert;
        }

        inline void push_back(const T& val)
        {
            if ((size_ + 1) > v_.size()) {
                v_.push_back(val);
                ++size_;
            } else {
                v_[size_++] = val;
            }
        }

        inline size_t size() const
        {
            return size_;
        }

        inline typename SlowVector::reference operator[](size_t n)
        {
            return v_[n];
        }

        inline typename SlowVector::const_reference operator[](size_t n) const
        {
            return v_[n];
        }

        inline void resize(size_t n)
        {
            if (n > v_.size()) {
                v_.resize(n);
            }
            size_ = n;
        }

        inline void clear()
        {
            size_ = 0;
        }

        inline size_t capacity() const
        {
            return v_.capacity();
        }

    private:
        SlowVector v_;
        size_t size_;
    };
}

#endif
