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

#ifndef _FOOTAGETRACKCOMPONENT_H_
#define _FOOTAGETRACKCOMPONENT_H_

#include "PhasedComponent.h"
#include "RenderQuadComponent.h"
#include "RenderTextComponent.h"
#include "SingleTweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class FootageTrackComponent : public boost::enable_shared_from_this<FootageTrackComponent>,
                                  public PhasedComponent
    {
    public:
        FootageTrackComponent(const SceneObjectPtr& target, const b2Vec2& infoPos, const std::string& infoImage, float infoImageHeight,
            const std::string& infoText, float infoTextSize, int zOrder);
        ~FootageTrackComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        inline float aimMinScale() const { return aimMinScale_; }
        inline void setAimMinScale(float value) { aimMinScale_ = value; }

        inline float aimScale() const { return aimScale_; }
        inline void setAimScale(float value) { aimScale_ = value; }

        inline float aimTime() const { return aimTime_; }
        inline void setAimTime(float value) { aimTime_ = value; }

        inline float trackTime() const { return trackTime_; }
        inline void setTrackTime(float value) { trackTime_ = value; }

        inline float transitionTime() const { return transitionTime_; }
        inline void setTransitionTime(float value) { transitionTime_ = value; }

        inline float imageTime() const { return imageTime_; }
        inline void setImageTime(float value) { imageTime_ = value; }

        inline float textTimeout() const { return textTimeout_; }
        inline void setTextTimeout(float value) { textTimeout_ = value; }

        inline float keepTime() const { return keepTime_; }
        inline void setKeepTime(float value) { keepTime_ = value; }

        inline float fadeTime() const { return fadeTime_; }
        inline void setFadeTime(float value) { fadeTime_ = value; }

        inline const Color& trackColor() const { return trackColor_; }
        inline void setTrackColor(const Color& value) { trackColor_ = value; }

        inline const Color& imageColor() const { return imageColor_; }
        inline void setImageColor(const Color& value) { imageColor_ = value; }

        inline const Color& textColor() const { return textColor_; }
        inline void setTextColor(const Color& value) { textColor_ = value; }

    private:
        enum State
        {
            StateAim = 0,
            StateTrack,
            StateTransition,
            StateImage,
            StateText,
        };

        virtual void onRegister();

        virtual void onUnregister();

        SceneObjectPtr target_;
        b2Vec2 infoPos_;
        DrawablePtr infoDrawable_;
        float infoImageHeight_;
        std::string infoText_;
        float infoTextSize_;
        int zOrder_;

        float aimMinScale_;
        float aimScale_;
        float aimTime_;
        float trackTime_;
        float transitionTime_;
        float imageTime_;
        float textTimeout_;
        float keepTime_;
        float fadeTime_;

        Color trackColor_;
        Color imageColor_;
        Color textColor_;

        State state_;
        float t_;
        float t2_;
        TweeningPtr tweening_;
        RenderQuadComponentPtr rc_;
        RenderQuadComponentPtr rcInfo_;
        RenderQuadComponentPtr rcWhite_;
        RenderTextComponentPtr rcText_;
        float targetSize_;
        b2Vec2 targetCapturedPos_;
    };

    typedef boost::shared_ptr<FootageTrackComponent> FootageTrackComponentPtr;
}

#endif
