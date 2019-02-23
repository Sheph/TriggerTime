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

#ifndef _PARTICLEEMMITER_H_
#define _PARTICLEEMMITER_H_

#include "af/GdxP.h"
#include "Image.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

namespace af
{
    struct Particle
    {
        Particle();

        void setImagePos(float x, float y);

        void setImageScale(float value);

        void setImageRotation(float value);

        void imageTranslate(float xAmount, float yAmount);

        void setImageColor(const Color& color);

        void setImageStartXf(const b2Transform& xf);

        void updateImageVertices(float aspect);

        bool active;
        b2Transform imageStartXf;
        b2Vec2 imagePos;
        float imageScaleX;
        float imageScaleY;
        float imageRotation;
        Color imageColor;
        bool imageDirty;
        float imageVertices[12];
        b2AABB imageAABB;
        int life;
        int currentLife;
        float scale;
        float scaleDiff;
        float rotation;
        float rotationDiff;
        float velocity;
        float velocityDiff;
        float angle;
        float angleDiff;
        float angleCos;
        float angleSin;
        float transparency;
        float transparencyDiff;
        float wind;
        float windDiff;
        float gravity;
        float gravityDiff;
        Color tint;
    };

    class ParticleEmmiter : boost::noncopyable
    {
    public:
        typedef std::vector<Particle> Particles;

        ParticleEmmiter(const b2Vec2& pos, float angle,
                        const GdxPEntry& entry,
                        bool fixedPos = false);
        ~ParticleEmmiter();

        void resetEmit();

        void allowCompletion();
        bool isAllowCompletion() const;

        bool isComplete();

        void setDuration(float value);
        float getDuration() const;

        void resetAABB(const b2Transform& xf);

        void update(const b2Transform& xf, float dt);

        void render();

        inline const b2AABB& aabb() const { return aabb_; }

        inline GdxPEntry& entry() { return entry_; }

        bool getStartPos(b2Vec2& pos) const;

        bool getFinalPos(b2Vec2& pos) const;

        inline const Particles& particles() const { return particles_; }

    private:
        enum UpdateFlag
        {
            UpdateScale = (1 << 0),
            UpdateAngle = (1 << 1),
            UpdateRotation = (1 << 2),
            UpdateVelocity = (1 << 3),
            UpdateWind = (1 << 4),
            UpdateGravity = (1 << 5),
            UpdateTint = (1 << 6),
            UpdateStartXf = (1 << 7)
        };

        void restart();

        void addParticle(const b2Transform& xf);

        void addParticles(const b2Transform& xf, int count);

        bool updateParticle(const b2Transform& xf, Particle& particle, float delta, int deltaMillis);

        void activateParticle(const b2Transform& xf, int index);

        GdxPEntry entry_;
        bool fixedPos_;
        Image image_;
        float accumulator_;
        Particles particles_;
        b2Transform xf_;
        int activeCount_;
        bool firstUpdate_;
        int updateFlags_;
        bool allowCompletion_;
        b2AABB aabb_;
        int emission_;
        int emissionDiff_;
        int emissionDelta_;
        int lifeOffset_;
        int lifeOffsetDiff_;
        int life_;
        int lifeDiff_;
        float spawnWidth_;
        float spawnWidthDiff_;
        float spawnHeight_;
        float spawnHeightDiff_;
        float duration_;
        float durationTimer_;
        float delay_;
        float delayTimer_;
    };

    typedef boost::shared_ptr<ParticleEmmiter> ParticleEmmiterPtr;
}

#endif
