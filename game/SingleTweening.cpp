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

#include "SingleTweening.h"
#include <cmath>

namespace af
{
    float EasingLinear(float t, float factor)
    {
        return t;
    }

    float EasingQuad(float t, float factor)
    {
        return t * t;
    }

    float EasingElastic(float t, float p)
    {
        float v = t - 1;

        return -std::pow(2.0f, 10.0f * v) *
            std::sin((v - p / 4.0f) * 2.0f * b2_pi / p);
    }

    float EasingInVibrate(float t, float p)
    {
        float v = t - 1;

        return -std::pow(2.0f, 10.0f * v) *
            std::sin((v - p / 2.0f) * 2.0f * b2_pi / p);
    }

    float EasingOutVibrate(float t, float factor)
    {
        return EasingInVibrate(1.0f - t, factor);
    }

    template <float (*easing)(float, float)>
    float EaseIn(float t, float factor)
    {
        return easing(t, factor);
    }

    template <float (*easing)(float, float)>
    float EaseOut(float t, float factor)
    {
        return 1.0f - EaseIn<easing>(1.0f - t, factor);
    }

    template <float (*easing)(float, float)>
    float EaseInOut(float t, float factor)
    {
        if (t < 0.5f) {
            return EaseIn<easing>(2.0f * t, factor) / 2.0f;
        } else {
            return 0.5f + EaseOut<easing>(2.0f * t - 1.0f, factor) / 2.0f;
        }
    }

    SingleTweening::SingleTweening(float duration, Easing easing, bool loop)
    : Tweening(loop),
      start_(0.0f),
      end_(1.0f),
      duration_(duration)
    {
        switch (easing) {
        case EaseInElastic:
        case EaseOutElastic:
        case EaseInOutElastic:
            factor_ = ElasticFactor;
            break;
        case EaseInVibrate:
        case EaseOutVibrate:
            factor_ = VibrateFactor;
            break;
        default:
            factor_ = 0.0f;
            break;
        }
        setEasing(easing);
    }

    SingleTweening::SingleTweening(float duration, Easing easing,
                                   float start, float end, bool loop)
    : Tweening(loop),
      start_(start),
      end_(end),
      duration_(duration)
    {
        switch (easing) {
        case EaseInElastic:
        case EaseOutElastic:
        case EaseInOutElastic:
            factor_ = ElasticFactor;
            break;
        case EaseInVibrate:
        case EaseOutVibrate:
            factor_ = VibrateFactor;
            break;
        default:
            factor_ = 0.0f;
            break;
        }
        setEasing(easing);
    }

    SingleTweening::SingleTweening(float duration, float factor, Easing easing, bool loop)
    : Tweening(loop),
      start_(0.0f),
      end_(1.0f),
      duration_(duration),
      factor_(factor)
    {
        setEasing(easing);
    }

    SingleTweening::SingleTweening(float duration, float factor, Easing easing,
                                   float start, float end, bool loop)
    : Tweening(loop),
      start_(start),
      end_(end),
      duration_(duration),
      factor_(factor)
    {
        setEasing(easing);
    }

    SingleTweening::~SingleTweening()
    {
    }

    float SingleTweening::getValue(float timeVal) const
    {
        if (loop()) {
            timeVal = std::fmod(timeVal, duration_);
            if (timeVal < 0.0f) {
                timeVal += duration_;
            }
        } else if (timeVal > duration_) {
            timeVal = duration_;
        } else if (timeVal < 0.0f) {
            timeVal = 0.0f;
        }

        float scale = easing_(timeVal / duration_, factor_);
        return start_ + scale * (end_ - start_);
    }

    bool SingleTweening::finished(float timeVal) const
    {
        return (!loop() && ((timeVal < 0.0f) || (timeVal > duration_)));
    }

    void SingleTweening::setEasing(Easing easing)
    {
        switch (easing) {
        case EaseLinear:
            easing_ = EasingLinear;
            break;
        case EaseInQuad:
            easing_ = EaseIn<EasingQuad>;
            break;
        case EaseOutQuad:
            easing_ = EaseOut<EasingQuad>;
            break;
        case EaseInOutQuad:
            easing_ = EaseInOut<EasingQuad>;
            break;
        case EaseInElastic:
            easing_ = EaseIn<EasingElastic>;
            break;
        case EaseOutElastic:
            easing_ = EaseOut<EasingElastic>;
            break;
        case EaseInOutElastic:
            easing_ = EaseInOut<EasingElastic>;
            break;
        case EaseInVibrate:
            easing_ = EasingInVibrate;
            break;
        case EaseOutVibrate:
            easing_ = EasingOutVibrate;
            break;
        default:
            assert(false);
            easing_ = EasingLinear;
            break;
        }
    }
}
