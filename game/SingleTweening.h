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

#ifndef _SINGLETWEENING_H_
#define _SINGLETWEENING_H_

#include "Tweening.h"

namespace af
{
    enum Easing
    {
        EaseLinear = 0,
        EaseInQuad = 1,
        EaseOutQuad = 2,
        EaseInOutQuad = 3,
        EaseInElastic = 4,
        EaseOutElastic = 5,
        EaseInOutElastic = 6,
        EaseInVibrate = 7,
        EaseOutVibrate = 8,
    };

    static const float ElasticFactor = 0.3f;

    static const float VibrateFactor = 0.3f;

    class SingleTweening : public Tweening
    {
    public:
        SingleTweening(float duration, Easing easing, bool loop = false);
        SingleTweening(float duration, Easing easing,
                       float start, float end, bool loop = false);
        SingleTweening(float duration, float factor, Easing easing, bool loop = false);
        SingleTweening(float duration, float factor, Easing easing,
                       float start, float end, bool loop = false);
        ~SingleTweening();

        inline float start() const { return start_; }

        inline float end() const { return end_; }

        virtual float duration() const { return duration_; }

        virtual float factor() const { return factor_; }

        virtual float getValue(float timeVal) const;

        virtual bool finished(float timeVal) const;

    private:
        void setEasing(Easing easing);

        float start_;
        float end_;
        float duration_;
        float (*easing_)(float, float);
        float factor_;
    };

    typedef boost::shared_ptr<SingleTweening> SingleTweeningPtr;
}

#endif
