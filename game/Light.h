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

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class RenderLightComponent;

    class Light;
    typedef boost::shared_ptr<Light> LightPtr;

    class Light : boost::noncopyable
    {
    public:
        explicit Light(const std::string& name);
        virtual ~Light();

        inline const std::string& name() const { return name_; }

        inline bool active() const { return parent() != NULL; }

        inline RenderLightComponent* parent() const { return parent_; }

        void setDirty();

        inline void setDiffuse(bool value) { diffuse_ = value; }
        inline bool diffuse() const { return diffuse_; }

        void setPos(const b2Vec2& value);
        inline const b2Vec2& pos() const { return pos_; }

        void setAngle(float value);
        inline float angle() const { return angle_; }

        void setWorldTransform(const b2Transform& value);
        inline const b2Transform& worldTransform() const { return xf_; }

        void setNumRays(UInt32 value);
        inline UInt32 numRays() const { return numRays_; }

        void setColor(const Color& value);
        inline const Color& color() const { return color_; }

        inline void setVisible(bool value) { visible_ = value; }
        inline bool visible() const { return visible_; }

        void setSoftLength(float value);
        inline float softLength() const { return softLength_; }
        inline bool soft() const { return softLength_ != 0.0f; }

        inline void setXray(bool value) { xray_ = value; }
        inline bool xray() const { return xray_; }

        inline void setDynamic(bool value) { dynamic_ = value; }
        inline bool dynamic() const { return dynamic_; }

        inline void setReverse(bool value) { reverse_ = value; }
        inline bool reverse() const { return reverse_; }

        void setIntensity(float value);
        inline float intensity() const { return intensity_; }

        void setNearOffset(float value);
        inline float nearOffset() const { return nearOffset_; }

        inline void setHitBlockers(bool value) { hitBlockers_ = value; }
        inline bool hitBlockers() const { return hitBlockers_; }

        bool updateAABB(b2AABB& prevAABB, b2AABB& aabb, b2Vec2& displacement);

        virtual LightPtr sharedThis() = 0;

        virtual void update() = 0;

        virtual void render() = 0;

        void remove();

        /*
         * Internal, do not call.
         * @{
         */
        b2AABB adopt(RenderLightComponent* parent);
        void abandon();

        inline void setCookie(SInt32 value) { cookie_ = value; }
        inline SInt32 cookie() const { return cookie_; }

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
            float& finalFraction);

        /*
         * @}
         */

    protected:
        inline const b2Transform& finalTransform() const { return finalXf_; }

        inline float nearS(float s) const { return (reverse_ ? nearOffset_ : (s - nearOffset_)) * intensity_; }
        inline float farS(float s) const { return (reverse_ ? 1.0f : s) * intensity_; }

    private:
        virtual b2AABB calculateAABB() = 0;

        std::string name_;

        bool diffuse_;
        b2Vec2 pos_;
        float angle_;

        RenderLightComponent* parent_;

        SInt32 cookie_;

        b2Transform xf_;
        b2Transform finalXf_;
        UInt32 numRays_;
        Color color_;
        bool visible_;
        float softLength_;
        bool xray_;
        bool dynamic_;
        bool reverse_;
        float intensity_;
        float nearOffset_;
        bool hitBlockers_;

        bool dirty_;
        b2Vec2 prevAbsPos_;
        b2AABB prevAABB_;
    };
}

#endif
