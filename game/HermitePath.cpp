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

#include "HermitePath.h"
#include "Settings.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    HermitePathIterator::HermitePathIterator(const HermitePath* path, int i, float pos)
    : path_(path),
      i_(i),
      pos_(pos)
    {
        if (path_->lengths_.empty()) {
            return;
        }

        if (i < 0) {
            i = 0;
            pos = 0.0f;
        }

        if (static_cast<size_t>(i) >= path_->lengths_.size()) {
            i = path_->lengths_.size() - 1;
            pos = path_->lengths_[i];
        }

        float t = pos / path_->lengths_[i];
        point_ = path_->samples_[i] + t * (path_->samples_[i + 1] - path_->samples_[i]);
    }

    HermitePathIterator::~HermitePathIterator()
    {
    }

    PathIteratorPtr HermitePathIterator::clone() const
    {
        return boost::make_shared<HermitePathIterator>(path_, i_, pos_);
    }

    const b2Vec2& HermitePathIterator::current() const
    {
        return point_;
    }

    bool HermitePathIterator::less(const PathIteratorPtr& other) const
    {
        const HermitePathIteratorPtr& r =
            boost::static_pointer_cast<HermitePathIterator>(other);

        return less(*r);
    }

    bool HermitePathIterator::eq(const PathIteratorPtr& other) const
    {
        const HermitePathIteratorPtr& r =
            boost::static_pointer_cast<HermitePathIterator>(other);

        return eq(*r);
    }

    void HermitePathIterator::step(float length)
    {
        if ((length == 0.0f) || path_->lengths_.empty()) {
            return;
        }

        if (i_ < 0) {
            if (length > 0.0f) {
                i_ = 0;
                pos_ = 0.0f;
            } else {
                return;
            }
        }

        if (static_cast<size_t>(i_) >= path_->lengths_.size()) {
            if (length > 0.0f) {
                return;
            } else {
                i_ = path_->lengths_.size() - 1;
                pos_ = path_->lengths_[i_];
            }
        }

        if (length > 0.0f) {
            stepForward(length);
        } else {
            stepBack(-length);
        }
    }

    void HermitePathIterator::stepForward(float length)
    {
        if (length > path_->length_) {
            if (loop()) {
                length = std::fmod(length, path_->length_);
                if (length == 0.0f) {
                    return;
                }
            } else {
                i_ = path_->lengths_.size();
                pos_ = 0.0f;
                point_ = path_->samples_[i_];
                return;
            }
        }

        if (loop()) {
            float l = 0.0f;
            float tmpPos = pos_ + length;
            size_t k = i_;

            while (true) {
                if (tmpPos <= (l + path_->lengths_[k])) {
                    i_ = k;
                    pos_ = tmpPos - l;

                    float t = pos_ / path_->lengths_[i_];
                    point_ = path_->samples_[i_] + t * (path_->samples_[i_ + 1] - path_->samples_[i_]);

                    return;
                }
                l += path_->lengths_[k];

                ++k;
                k %= path_->lengths_.size();
            }
        } else {
            float l = 0.0f;
            float tmpPos = pos_ + length;

            for (size_t k = i_; k < path_->lengths_.size(); ++k) {
                if (tmpPos <= (l + path_->lengths_[k])) {
                    i_ = k;
                    pos_ = tmpPos - l;

                    float t = pos_ / path_->lengths_[i_];
                    point_ = path_->samples_[i_] + t * (path_->samples_[i_ + 1] - path_->samples_[i_]);

                    return;
                }
                l += path_->lengths_[k];
            }

            i_ = path_->lengths_.size();
            pos_ = 0.0f;
            point_ = path_->samples_[i_];
        }
    }

    void HermitePathIterator::stepBack(float length)
    {
        if (length > path_->length_) {
            if (loop()) {
                length = std::fmod(length, path_->length_);
                if (length == 0.0f) {
                    return;
                }
            } else {
                i_ = -1;
                pos_ = 0.0f;
                point_ = path_->samples_[0];
                return;
            }
        }

        if (loop()) {
            float l = 0.0f;
            float tmpPos = path_->lengths_[i_] + length - pos_;
            size_t k = i_;

            while (true) {
                l += path_->lengths_[k];
                if (tmpPos < l) {
                    i_ = k;
                    pos_ = l - tmpPos;

                    float t = pos_ / path_->lengths_[i_];
                    point_ = path_->samples_[i_] + t * (path_->samples_[i_ + 1] - path_->samples_[i_]);

                    return;
                }

                if (k-- == 0) {
                    k = path_->lengths_.size() - 1;
                }
            }
        } else {
            float l = 0.0f;
            float tmpPos = path_->lengths_[i_] + length - pos_;

            for (int k = i_; k >= 0; --k) {
                l += path_->lengths_[k];
                if (tmpPos < l) {
                    i_ = k;
                    pos_ = l - tmpPos;

                    float t = pos_ / path_->lengths_[i_];
                    point_ = path_->samples_[i_] + t * (path_->samples_[i_ + 1] - path_->samples_[i_]);

                    return;
                }
            }

            i_ = -1;
            pos_ = 0.0f;
            point_ = path_->samples_[0];
        }
    }

    HermitePath::HermitePath(int numIterations, float c)
    : step_(1.0f / numIterations),
      c_(c),
      length_(0.0f)
    {
        if (numIterations <= 0) {
            step_ = 1.0f;
        }
    }

    HermitePath::HermitePath(float c)
    : step_(1.0f / settings.hermitePath.numIterations),
      c_(c),
      length_(0.0f)
    {
    }

    HermitePath::~HermitePath()
    {
    }

    void HermitePath::add(const b2Vec2& point)
    {
        p_.push_back(point);

        if (p_.size() >= 3) {
            const b2Vec2& p2 = p_[p_.size() - 1];
            const b2Vec2& p0 = p_[p_.size() - 3];
            m_.push_back((1.0f - c_) * 0.5f * (p2 - p0));
        }

        if (p_.size() >= 4) {
            b2Vec2 prev = at(p_.size() - 4, 0.0f);

            if (p_.size() == 4) {
                samples_.push_back(prev);
            }

            for (float t = step_; t <= (1.0f - step_); t += step_) {
                b2Vec2 cur = at(p_.size() - 4, t);
                samples_.push_back(cur);

                lengths_.push_back(b2Distance(prev, cur));
                length_ += lengths_.back();

                prev = cur;
            }

            b2Vec2 cur = at(p_.size() - 4, 1.0f);
            samples_.push_back(cur);

            lengths_.push_back(b2Distance(prev, cur));
            length_ += lengths_.back();
        }
    }

    void HermitePath::add(const b2Vec2* points, size_t numPoints)
    {
        for (size_t i = 0; i < numPoints; ++i) {
            add(points[i]);
        }
    }

    void HermitePath::addFront(const b2Vec2& point)
    {
        p_.insert(p_.begin(), point);

        if (p_.size() >= 3) {
            const b2Vec2& p2 = p_[2];
            const b2Vec2& p0 = p_[0];
            m_.insert(m_.begin(), (1.0f - c_) * 0.5f * (p2 - p0));
        }

        if (p_.size() >= 4) {
            b2Vec2 prev = at(0, 0.0f);

            std::vector<float> tmpLengths;
            Points tmpSamples;

            if (p_.size() == 4) {
                tmpSamples.push_back(prev);
            }

            for (float t = step_; t <= (1.0f - step_); t += step_) {
                b2Vec2 cur = at(0, t);
                tmpSamples.push_back(cur);

                tmpLengths.push_back(b2Distance(prev, cur));
                length_ += tmpLengths.back();

                prev = cur;
            }

            b2Vec2 cur = at(0, 1.0f);
            tmpSamples.push_back(cur);

            tmpLengths.push_back(b2Distance(prev, cur));
            length_ += tmpLengths.back();

            samples_.insert(samples_.begin(), tmpSamples.begin(), tmpSamples.end());
            lengths_.insert(lengths_.begin(), tmpLengths.begin(), tmpLengths.end());
        }
    }

    void HermitePath::addFront(const b2Vec2* points, size_t numPoints)
    {
        for (size_t i = 0; i < numPoints; ++i) {
            addFront(points[i]);
        }
    }

    PathPtr HermitePath::clone() const
    {
        HermitePathPtr p = boost::make_shared<HermitePath>(1.0f, 0.0f);

        /*
         * FIXME: hack.
         */
        p->step_ = step_;
        p->c_ = c_;
        p->p_ = p_;
        p->m_ = m_;
        p->length_ = length_;
        p->lengths_ = lengths_;
        p->samples_ = samples_;

        return p;
    }

    const Points& HermitePath::points() const
    {
        return p_;
    }

    float HermitePath::length() const
    {
        return length_;
    }

    PathIteratorPtr HermitePath::first() const
    {
        return boost::make_shared<HermitePathIterator>(this, 0, 0.0f);
    }

    PathIteratorPtr HermitePath::last() const
    {
        if (lengths_.empty()) {
            return boost::make_shared<HermitePathIterator>(this, 0, 0.0f);
        } else {
            return boost::make_shared<HermitePathIterator>(this, lengths_.size() - 1, lengths_.back());
        }
    }

    PathIteratorPtr HermitePath::end() const
    {
        if (lengths_.empty()) {
            return boost::make_shared<HermitePathIterator>(this, 0, 0.0f);
        } else {
            return boost::make_shared<HermitePathIterator>(this, lengths_.size(), 0.0f);
        }
    }

    PathIteratorPtr HermitePath::rend() const
    {
        if (lengths_.empty()) {
            return boost::make_shared<HermitePathIterator>(this, 0, 0.0f);
        } else {
            return boost::make_shared<HermitePathIterator>(this, -1, 0.0f);
        }
    }

    PathIteratorPtr HermitePath::find(float pos) const
    {
        if (lengths_.empty()) {
            return boost::make_shared<HermitePathIterator>(this, 0, 0.0f);
        }

        if (pos < 0.0f) {
            return boost::make_shared<HermitePathIterator>(this, -1, 0.0f);
        }

        float l = 0.0f;

        for (size_t i = 0; i < lengths_.size(); ++i) {
            if (pos <= (l + lengths_[i])) {
                return boost::make_shared<HermitePathIterator>(this, i, pos - l);
            }
            l += lengths_[i];
        }

        return boost::make_shared<HermitePathIterator>(this, lengths_.size(), 0.0f);
    }

    HermitePathIterator HermitePath::firstIt() const
    {
        return HermitePathIterator(this, 0, 0.0f);
    }

    HermitePathIterator HermitePath::lastIt() const
    {
        if (lengths_.empty()) {
            return HermitePathIterator(this, 0, 0.0f);
        } else {
            return HermitePathIterator(this, lengths_.size() - 1, lengths_.back());
        }
    }

    HermitePathIterator HermitePath::findIt(float pos) const
    {
        if (lengths_.empty()) {
            return HermitePathIterator(this, 0, 0.0f);
        }

        if (pos < 0.0f) {
            return HermitePathIterator(this, -1, 0.0f);
        }

        float l = 0.0f;

        for (size_t i = 0; i < lengths_.size(); ++i) {
            if (pos <= (l + lengths_[i])) {
                return HermitePathIterator(this, i, pos - l);
            }
            l += lengths_[i];
        }

        return HermitePathIterator(this, lengths_.size(), 0.0f);
    }

    HermitePathIterator HermitePath::endIt() const
    {
        if (lengths_.empty()) {
            return HermitePathIterator(this, 0, 0.0f);
        } else {
            return HermitePathIterator(this, lengths_.size(), 0.0f);
        }
    }

    HermitePathIterator HermitePath::rendIt() const
    {
        if (lengths_.empty()) {
            return HermitePathIterator(this, 0, 0.0f);
        } else {
            return HermitePathIterator(this, -1, 0.0f);
        }
    }

    void HermitePath::clear()
    {
        p_.clear();
        m_.clear();
        length_ = 0.0f;
        lengths_.clear();
        samples_.clear();
    }

    b2Vec2 HermitePath::at(int i, float t) const
    {
        float h00 = 2 * t * t * t - 3 * t * t + 1;
        float h10 = t * t * t - 2 * t * t + t;
        float h01 = - 2 * t * t * t + 3 * t * t;
        float h11 = t * t * t - t * t;

        const b2Vec2& p0 = p_[i + 1];
        const b2Vec2& p1 = p_[i + 2];

        const b2Vec2& m0 = m_[i];
        const b2Vec2& m1 = m_[i + 1];

        return h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1;
    }
}
