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

#ifndef _AF_ENUMSET_H_
#define _AF_ENUMSET_H_

#include <bitset>

namespace af
{
    template <class T, int N>
    class EnumSet
    {
    public:
        EnumSet()
        {
        }

        EnumSet(T value)
        {
            bs_.set(value);
        }

        ~EnumSet()
        {
        }

        inline bool operator[](T v) const
        {
            return bs_[v];
        }

        inline typename std::bitset<N>::reference operator[](T v)
        {
            return bs_[v];
        }

        inline bool all() const
        {
            return bs_.count() == N;
        }

        inline bool any() const
        {
            return bs_.any();
        }

        inline bool empty() const
        {
            return bs_.none();
        }

        inline size_t count() const
        {
            return bs_.count();
        }

        inline EnumSet<T, N>& operator&=(const EnumSet<T, N>& other)
        {
            bs_ &= other.bs_;
            return *this;
        }

        inline EnumSet<T, N>& operator|=(const EnumSet<T, N>& other)
        {
            bs_ |= other.bs_;
            return *this;
        }

        inline EnumSet<T, N>& operator^=(const EnumSet<T, N>& other)
        {
            bs_ ^= other.bs_;
            return *this;
        }

        inline EnumSet<T, N> operator~() const
        {
            EnumSet<T, N> res;
            res.bs_ = ~bs_;
            return res;
        }

        inline void set(T v)
        {
            bs_[v] = true;
        }

        inline void reset(T v)
        {
            bs_[v] = false;
        }

        inline void flip(T v)
        {
            bs_[v] = !bs_[v];
        }

        inline void setAll()
        {
            bs_.set();
        }

        inline void resetAll()
        {
            bs_.reset();
        }

        inline void flipAll()
        {
            bs_.flip();
        }

    private:
        std::bitset<N> bs_;
    };

    template <class T, int N>
    inline EnumSet<T, N> operator&(const EnumSet<T, N>& lhs, const EnumSet<T, N>& rhs)
    {
        EnumSet<T, N> res;
        res.bs_ = lhs.bs_ & rhs.bs_;
        return res;
    }

    template <class T, int N>
    inline EnumSet<T, N> operator|(const EnumSet<T, N>& lhs, const EnumSet<T, N>& rhs)
    {
        EnumSet<T, N> res;
        res.bs_ = lhs.bs_ | rhs.bs_;
        return res;
    }

    template <class T, int N>
    inline EnumSet<T, N> operator^(const EnumSet<T, N>& lhs, const EnumSet<T, N>& rhs)
    {
        EnumSet<T, N> res;
        res.bs_ = lhs.bs_ ^ rhs.bs_;
        return res;
    }

    template <class T, int N>
    inline EnumSet<T, N> operator&(const EnumSet<T, N>& lhs, T rhs)
    {
        EnumSet<T, N> res = lhs;
        res &= rhs;
        return res;
    }

    template <class T, int N>
    inline EnumSet<T, N> operator|(const EnumSet<T, N>& lhs, T rhs)
    {
        EnumSet<T, N> res = lhs;
        res |= rhs;
        return res;
    }

    template <class T, int N>
    inline EnumSet<T, N> operator^(const EnumSet<T, N>& lhs, T rhs)
    {
        EnumSet<T, N> res = lhs;
        res ^= rhs;
        return res;
    }
}

#endif
