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

#ifndef _RENDERLIGHTCOMPONENT_H_
#define _RENDERLIGHTCOMPONENT_H_

#include "RenderComponent.h"
#include "Light.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class RenderLightComponent : public boost::enable_shared_from_this<RenderLightComponent>,
                                 public RenderComponent
    {
    public:
        explicit RenderLightComponent(int zOrder);
        ~RenderLightComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render(void* const* parts, size_t numParts);

        inline void setAmbientLight(const Color& color) { ambient_ = color; }
        inline const Color& ambientLight() const { return ambient_; }

        inline void setNumBlur(UInt32 value) { numBlur_ = value; }
        inline UInt32 numBlur() const { return numBlur_; }
        inline bool blur() const { return numBlur_ != 0; }

        void setGammaCorrection(bool value);
        inline bool gammaCorrection() const { return gammaCorrection_; }
        inline float gammaCorrected(float distance) const
        {
            distance *= gammaCorrectionParam_;
            return distance < 0.01f ? 0.01f : distance;
        }

        void addLight(const LightPtr& light);

        void removeLight(const LightPtr& light);

    private:
        typedef std::vector<LightPtr> Lights;

        virtual void onRegister();

        virtual void onUnregister();

        Color ambient_;
        UInt32 numBlur_;
        bool gammaCorrection_;
        float gammaCorrectionParam_;

        Lights lights_;

        bool needLightsUpdate_;
    };

    typedef boost::shared_ptr<RenderLightComponent> RenderLightComponentPtr;
}

#endif
