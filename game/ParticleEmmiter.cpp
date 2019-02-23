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

#include "ParticleEmmiter.h"
#include "af/Utils.h"
#include "AssetManager.h"
#include "Utils.h"
#include "Renderer.h"
#include <cmath>

namespace af
{
    Particle::Particle()
    : active(false),
      imagePos(0.0f, 0.0f),
      imageScaleX(1.0f),
      imageScaleY(1.0f),
      imageRotation(0.0f),
      imageColor(1.0f, 1.0f, 1.0f),
      imageDirty(true),
      life(0),
      currentLife(0),
      scale(0.0f),
      scaleDiff(0.0f),
      rotation(0.0f),
      rotationDiff(0.0f),
      velocity(0.0f),
      velocityDiff(0.0f),
      angle(0.0f),
      angleDiff(0.0f),
      angleCos(0.0f),
      angleSin(0.0f),
      transparency(0.0f),
      transparencyDiff(0.0f),
      wind(0.0f),
      windDiff(0.0f),
      gravity(0.0f),
      gravityDiff(0.0f)
    {
    }

    void Particle::setImagePos(float x, float y)
    {
        imagePos.x = x;
        imagePos.y = y;
        imageDirty = true;
    }

    void Particle::setImageScale(float value)
    {
        imageScaleX = value;
        imageScaleY = value;
        imageDirty = true;
    }

    void Particle::setImageRotation(float value)
    {
        imageRotation = value;
        imageDirty = true;
    }

    void Particle::imageTranslate(float xAmount, float yAmount)
    {
        imagePos.x += xAmount;
        imagePos.y += yAmount;
        imageDirty = true;
    }

    void Particle::setImageColor(const Color& color)
    {
        imageColor = color;
    }

    void Particle::setImageStartXf(const b2Transform& xf)
    {
        imageStartXf = xf;
        imageDirty = true;
    }

    void Particle::updateImageVertices(float aspect)
    {
        if (!imageDirty) {
            return;
        }

        b2Vec2 points[4];

        points[2] = b2Vec2(aspect / 2, 0.5f);
        points[0] = -points[2];

        if ((imageScaleX != 1.0f) ||
            (imageScaleY != 1.0f)) {
            points[0].x *= imageScaleX;
            points[2].x *= imageScaleX;
            points[0].y *= imageScaleY;
            points[2].y *= imageScaleY;
        }

        points[1].x = points[2].x;
        points[1].y = points[0].y;
        points[3].x = points[0].x;
        points[3].y = points[2].y;

        b2Transform t(imagePos, b2Rot(imageRotation));

        t = b2Mul(imageStartXf, t);

        points[0] = b2Mul(t, points[0]);
        points[1] = b2Mul(t, points[1]);
        points[2] = b2Mul(t, points[2]);
        points[3] = b2Mul(t, points[3]);

        imageVertices[0] = imageVertices[6] = points[0].x;
        imageVertices[1] = imageVertices[7] = points[0].y;
        imageVertices[2] = points[1].x;
        imageVertices[3] = points[1].y;
        imageVertices[4] = imageVertices[8] = points[2].x;
        imageVertices[5] = imageVertices[9] = points[2].y;
        imageVertices[10] = points[3].x;
        imageVertices[11] = points[3].y;

        imageDirty = false;

        imageAABB = computeAABB(points, 4);
    }

    ParticleEmmiter::ParticleEmmiter(const b2Vec2& pos,
                                     float angle,
                                     const GdxPEntry& entry,
                                     bool fixedPos)
    : entry_(entry),
      fixedPos_(fixedPos),
      image_(assetManager.getImage(entry.imageFileName)),
      accumulator_(0.0f),
      xf_(pos, b2Rot(angle)),
      activeCount_(0),
      firstUpdate_(false),
      updateFlags_(0),
      allowCompletion_(false),
      emission_(0),
      emissionDiff_(0),
      emissionDelta_(0),
      lifeOffset_(0),
      lifeOffsetDiff_(0),
      life_(0),
      lifeDiff_(0),
      spawnWidth_(0.0f),
      spawnWidthDiff_(0.0f),
      spawnHeight_(0.0f),
      spawnHeightDiff_(0.0f),
      duration_(1.0f),
      durationTimer_(0.0f),
      delay_(0.0f),
      delayTimer_(0.0f)
    {
        particles_.resize(entry_.maxCount);
        aabb_.lowerBound = b2Vec2(0.0f, 0.0f);
        aabb_.upperBound = b2Vec2(0.0f, 0.0f);
    }

    ParticleEmmiter::~ParticleEmmiter()
    {
    }

    void ParticleEmmiter::resetEmit()
    {
        emissionDelta_ = 0;
        durationTimer_ = duration_;
        for (size_t i = 0; i < particles_.size(); ++i) {
            particles_[i].active = false;
        }
        activeCount_ = 0;
        firstUpdate_ = true;
        allowCompletion_ = false;
        restart();
    }

    void ParticleEmmiter::allowCompletion()
    {
        allowCompletion_ = true;
        durationTimer_ = duration_;
    }

    bool ParticleEmmiter::isAllowCompletion() const
    {
        return allowCompletion_;
    }

    bool ParticleEmmiter::isComplete()
    {
        if (delayTimer_ < delay_) {
            return false;
        }

        return (durationTimer_ >= duration_) && (activeCount_ == 0);
    }

    void ParticleEmmiter::setDuration(float value)
    {
        entry_.continuous = false;
        duration_ = value * 1000;
        durationTimer_ = 0;
    }

    float ParticleEmmiter::getDuration() const
    {
        return duration_ / 1000;
    }

    void ParticleEmmiter::resetAABB(const b2Transform& xf)
    {
        b2Transform actualXf;

        if (fixedPos_) {
            actualXf = b2Transform(xf.p + xf_.p, b2Mul(xf.q, xf_.q));
        } else {
            actualXf = b2Mul(xf, xf_);
        }

        aabb_.lowerBound = actualXf.p;
        aabb_.upperBound = aabb_.lowerBound + b2Vec2(1.0f, 1.0f);
    }

    void ParticleEmmiter::update(const b2Transform& xf, float dt)
    {
        b2Transform actualXf;

        if (fixedPos_) {
            actualXf = b2Transform(xf.p + xf_.p, b2Mul(xf.q, xf_.q));
        } else {
            actualXf = b2Mul(xf, xf_);
        }

        /*
         * Starting dummy AABB.
         */

        b2AABB aabb;

        aabb.lowerBound = actualXf.p;
        aabb.upperBound = aabb.lowerBound + b2Vec2(1.0f, 1.0f);

        accumulator_ += dt * 1000;
        if (accumulator_ < 1) {
            return;
        }
        int deltaMillis = accumulator_;
        accumulator_ -= deltaMillis;

        if (delayTimer_ < delay_) {
            delayTimer_ += deltaMillis;
        } else {
            bool done = false;
            if (firstUpdate_) {
                firstUpdate_ = false;
                addParticle(actualXf);
            }

            if (durationTimer_ < duration_) {
                durationTimer_ += deltaMillis;
            } else {
                if (!entry_.continuous || allowCompletion_) {
                    done = true;
                } else {
                    restart();
                }
            }

            if (!done) {
                emissionDelta_ += deltaMillis;
                float emissionTime = emission_ + emissionDiff_ * entry_.emission.getScale(durationTimer_ / duration_);
                if (emissionTime > 0) {
                    emissionTime = 1000 / emissionTime;
                    if (emissionDelta_ >= emissionTime) {
                        int emitCount = emissionDelta_ / emissionTime;
                        emitCount = (std::min)(emitCount, entry_.maxCount - activeCount_);
                        emissionDelta_ -= emitCount * emissionTime;
                        emissionDelta_ = std::fmod(emissionDelta_, emissionTime);
                        addParticles(actualXf, emitCount);
                    }
                }
                if (activeCount_ < entry_.minCount) {
                    addParticles(actualXf, entry_.minCount - activeCount_);
                }
            }
        }

        int activeCount = activeCount_;
        for (size_t i = 0; i < particles_.size(); ++i) {
            if (!particles_[i].active) {
                continue;
            }
            if (updateParticle(actualXf, particles_[i], dt, deltaMillis)) {
                aabb.Combine(particles_[i].imageAABB);
            } else {
                particles_[i].active = false;
                --activeCount;
            }
        }
        activeCount_ = activeCount;

        aabb_ = aabb;
    }

    void ParticleEmmiter::render()
    {
        bool setProg = true;

        if (entry_.additive) {
            renderer.blendFunc(GL_SRC_ALPHA, GL_ONE);
        }

        for (size_t i = 0; i < particles_.size(); ++i) {
            if (!particles_[i].active) {
                continue;
            }

            if (setProg) {
                renderer.setProgramDef(image_.texture());
                setProg = false;
            }

            RenderSimple rop = renderer.renderTriangles();

            rop.addVertices(&particles_[i].imageVertices[0], 6);
            rop.addTexCoords(image_.texCoords(), 6);
            rop.addColors(particles_[i].imageColor);
        }

        if (entry_.additive) {
            renderer.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }

    bool ParticleEmmiter::getStartPos(b2Vec2& pos) const
    {
        float lenSq = (std::numeric_limits<float>::max)();
        const Particle* p = NULL;

        for (size_t i = 0; i < particles_.size(); ++i) {
            if (!particles_[i].active || (particles_[i].currentLife <= 0)) {
                continue;
            }
            float tmp = particles_[i].imagePos.LengthSquared();
            if (tmp < lenSq) {
                lenSq = tmp;
                p = &particles_[i];
            }
        }

        if (!p) {
            return false;
        }

        pos = b2Mul(p->imageStartXf, b2Vec2(b2Sqrt(lenSq), 0.0f));

        return true;
    }

    bool ParticleEmmiter::getFinalPos(b2Vec2& pos) const
    {
        float lenSq = 0.0f;
        const Particle* p = NULL;

        for (size_t i = 0; i < particles_.size(); ++i) {
            if (!particles_[i].active || (particles_[i].currentLife <= 0)) {
                continue;
            }
            float tmp = particles_[i].imagePos.LengthSquared();
            if (tmp >= lenSq) {
                lenSq = tmp;
                p = &particles_[i];
            }
        }

        if (!p) {
            return false;
        }

        pos = b2Mul(p->imageStartXf, b2Vec2(b2Sqrt(lenSq), 0.0f));

        return true;
    }

    void ParticleEmmiter::restart()
    {
        delay_ = entry_.delay.active() ? entry_.delay.newLowValue() : 0;
        delayTimer_ = 0;

        durationTimer_ -= duration_;
        duration_ = entry_.duration.newLowValue();

        emission_ = entry_.emission.newLowValue();
        emissionDiff_ = entry_.emission.newHighValue();
        if (!entry_.emission.relative()) {
            emissionDiff_ -= emission_;
        }

        life_ = entry_.life.newLowValue();
        lifeDiff_ = entry_.life.newHighValue();
        if (!entry_.life.relative()) {
            lifeDiff_ -= life_;
        }

        lifeOffset_ = entry_.lifeOffset.active() ? entry_.lifeOffset.newLowValue() : 0;
        lifeOffsetDiff_ = entry_.lifeOffset.newHighValue();
        if (!entry_.lifeOffset.relative()) {
            lifeOffsetDiff_ -= lifeOffset_;
        }

        spawnWidth_ = entry_.spawnWidth.newLowValue();
        spawnWidthDiff_ = entry_.spawnWidth.newHighValue();
        if (!entry_.spawnWidth.relative()) {
            spawnWidthDiff_ -= spawnWidth_;
        }

        spawnHeight_ = entry_.spawnHeight.newLowValue();
        spawnHeightDiff_ = entry_.spawnHeight.newHighValue();
        if (!entry_.spawnHeight.relative()) {
            spawnHeightDiff_ -= spawnHeight_;
        }

        updateFlags_ = 0;

        if (entry_.angle.active() && (entry_.angle.timeline().size() > 1)) {
            updateFlags_ |= UpdateAngle;
        }
        if (entry_.velocity.active()) {
            updateFlags_ |= UpdateVelocity;
        }
        if (entry_.scale.timeline().size() > 1) {
            updateFlags_ |= UpdateScale;
        }
        if (entry_.rotation.active() && (entry_.rotation.timeline().size() > 1)) {
            updateFlags_ |= UpdateRotation;
        }
        if (entry_.wind.active()) {
            updateFlags_ |= UpdateWind;
        }
        if (entry_.gravity.active()) {
            updateFlags_ |= UpdateGravity;
        }
        if (entry_.tint.timeline().size() > 1) {
            updateFlags_ |= UpdateTint;
        }
        if (entry_.attached) {
            updateFlags_ |= UpdateStartXf;
        }
    }

    void ParticleEmmiter::addParticle(const b2Transform& xf)
    {
        int activeCount = activeCount_;
        if (activeCount == entry_.maxCount) {
            return;
        }
        for (size_t i = 0; i < particles_.size(); ++i) {
            if (!particles_[i].active) {
                activateParticle(xf, i);
                particles_[i].active = true;
                activeCount_ = activeCount + 1;
                break;
            }
        }
    }

    void ParticleEmmiter::addParticles(const b2Transform& xf, int count)
    {
        count = (std::min)(count, entry_.maxCount - activeCount_);
        if (count == 0) {
            return;
        }
        size_t index = 0;

        for (int i = 0; i < count; ++i) {
            for (; index < particles_.size(); ++index) {
                if (!particles_[index].active) {
                    activateParticle(xf, index);
                    particles_[index++].active = true;
                    goto outer;
                }
            }
            break;
        outer:
            ;
        }

        activeCount_ += count;
    }

    bool ParticleEmmiter::updateParticle(const b2Transform& xf,
                                         Particle& particle,
                                         float delta,
                                         int deltaMillis)
    {
        int life = particle.currentLife - deltaMillis;
        if (life <= 0) {
            return false;
        }
        particle.currentLife = life;

        float percent = 1.0f - static_cast<float>(particle.currentLife) / particle.life;
        int updateFlags = updateFlags_;

        if ((updateFlags & UpdateScale) != 0) {
            particle.setImageScale(particle.scale + particle.scaleDiff * entry_.scale.getScale(percent));
        }

        if ((updateFlags & UpdateVelocity) != 0) {
            float velocity = (particle.velocity + particle.velocityDiff * entry_.velocity.getScale(percent)) * delta;

            float velocityX, velocityY;
            if ((updateFlags & UpdateAngle) != 0) {
                float angle = particle.angle + particle.angleDiff * entry_.angle.getScale(percent);
                velocityX = velocity * std::cos(deg2rad(angle));
                velocityY = velocity * std::sin(deg2rad(angle));
                if ((updateFlags & UpdateRotation) != 0) {
                    float rotation = particle.rotation + particle.rotationDiff * entry_.rotation.getScale(percent);
                    if (entry_.aligned) {
                        rotation += angle;
                    }
                    particle.setImageRotation(deg2rad(rotation));
                }
            } else {
                velocityX = velocity * particle.angleCos;
                velocityY = velocity * particle.angleSin;
                if (entry_.aligned || (updateFlags & UpdateRotation) != 0) {
                    float rotation = particle.rotation + particle.rotationDiff * entry_.rotation.getScale(percent);
                    if (entry_.aligned) {
                        rotation += particle.angle;
                    }
                    particle.setImageRotation(deg2rad(rotation));
                }
            }

            if ((updateFlags & UpdateWind) != 0) {
                velocityX += (particle.wind + particle.windDiff * entry_.wind.getScale(percent)) * delta;
            }

            if ((updateFlags & UpdateGravity) != 0) {
                velocityY += (particle.gravity + particle.gravityDiff * entry_.gravity.getScale(percent)) * delta;
            }

            particle.imageTranslate(velocityX, velocityY);
        } else {
            if ((updateFlags & UpdateRotation) != 0) {
                particle.setImageRotation(deg2rad(particle.rotation + particle.rotationDiff * entry_.rotation.getScale(percent)));
            }
        }

        Color color;

        if ((updateFlags & UpdateTint) != 0) {
            color = entry_.tint.getColor(percent);
        } else {
            color = particle.tint;
        }

        color.rgba[3] = particle.transparency + particle.transparencyDiff * entry_.transparency.getScale(percent);

        particle.setImageColor(color);

        if ((updateFlags & UpdateStartXf) != 0) {
            particle.setImageStartXf(xf);
        }

        particle.updateImageVertices(image_.aspect());

        return true;
    }

    void ParticleEmmiter::activateParticle(const b2Transform& xf, int index)
    {
        particles_[index] = Particle();

        Particle& particle = particles_[index];

        particle.imageStartXf = xf;

        float percent = durationTimer_ / duration_;
        int updateFlags = updateFlags_;

        particle.currentLife = particle.life = life_ + static_cast<int>(lifeDiff_ * entry_.life.getScale(percent));

        if (entry_.velocity.active()) {
            particle.velocity = entry_.velocity.newLowValue();
            particle.velocityDiff = entry_.velocity.newHighValue();
            if (!entry_.velocity.relative()) {
                particle.velocityDiff -= particle.velocity;
            }
        }

        particle.angle = entry_.angle.newLowValue();
        particle.angleDiff = entry_.angle.newHighValue();
        if (!entry_.angle.relative()) {
            particle.angleDiff -= particle.angle;
        }
        float angle = 0;
        if ((updateFlags & UpdateAngle) == 0) {
            angle = particle.angle + particle.angleDiff * entry_.angle.getScale(0);
            particle.angle = angle;
            particle.angleCos = std::cos(deg2rad(angle));
            particle.angleSin = std::sin(deg2rad(angle));
        }

        particle.scale = entry_.scale.newLowValue() / image_.aspect();
        particle.scaleDiff = entry_.scale.newHighValue() / image_.aspect();
        if (!entry_.scale.relative()) {
            particle.scaleDiff -= particle.scale;
        }
        particle.setImageScale(particle.scale + particle.scaleDiff * entry_.scale.getScale(0));

        if (entry_.rotation.active()) {
            particle.rotation = entry_.rotation.newLowValue();
            particle.rotationDiff = entry_.rotation.newHighValue();
            if (!entry_.rotation.relative()) {
                particle.rotationDiff -= particle.rotation;
            }
            float rotation = particle.rotation + particle.rotationDiff * entry_.rotation.getScale(0);
            if (entry_.aligned) {
                rotation += angle;
            }
            particle.setImageRotation(deg2rad(rotation));
        }

        if (entry_.wind.active()) {
            particle.wind = entry_.wind.newLowValue();
            particle.windDiff = entry_.wind.newHighValue();
            if (!entry_.wind.relative()) {
                particle.windDiff -= particle.wind;
            }
        }

        if (entry_.gravity.active()) {
            particle.gravity = entry_.gravity.newLowValue();
            particle.gravityDiff = entry_.gravity.newHighValue();
            if (!entry_.gravity.relative()) {
                particle.gravityDiff -= particle.gravity;
            }
        }

        particle.tint = entry_.tint.getColor(0);

        particle.transparency = entry_.transparency.newLowValue();
        particle.transparencyDiff = entry_.transparency.newHighValue() - particle.transparency;

        float x = 0.0f;
        if (entry_.xOffset.active()) {
            x += entry_.xOffset.newLowValue();
        }
        float y = 0.0f;
        if (entry_.yOffset.active()) {
            y += entry_.yOffset.newLowValue();
        }
        switch (entry_.spawnShape.shape()) {
        case GdxPSpawnShapeSquare: {
            float width = spawnWidth_ + (spawnWidthDiff_ * entry_.spawnWidth.getScale(percent));
            float height = spawnHeight_ + (spawnHeightDiff_ * entry_.spawnHeight.getScale(percent));
            x += getRandom(0.0f, width) - width / 2;
            y += getRandom(0.0f, height) - height / 2;
            break;
        }
        case GdxPSpawnShapeEllipse: {
            float width = spawnWidth_ + (spawnWidthDiff_ * entry_.spawnWidth.getScale(percent));
            float height = spawnHeight_ + (spawnHeightDiff_ * entry_.spawnHeight.getScale(percent));
            float radiusX = width / 2;
            float radiusY = height / 2;
            if ((radiusX == 0) || (radiusY == 0)) {
                break;
            }
            float scaleY = radiusX / radiusY;
            if (entry_.spawnShape.edges()) {
                float spawnAngle;
                switch (entry_.spawnShape.side()) {
                case GdxPSpawnEllipseTop:
                    spawnAngle = -getRandom(0.0f, 179.0f);
                    break;
                case GdxPSpawnEllipseBottom:
                    spawnAngle = getRandom(0.0f, 179.0f);
                    break;
                default:
                    spawnAngle = getRandom(0.0f, 360.0f);
                    break;
                }
                float cosDeg = std::cos(deg2rad(spawnAngle));
                float sinDeg = std::sin(deg2rad(spawnAngle));
                x += cosDeg * radiusX;
                y += sinDeg * radiusX / scaleY;
                if ((updateFlags & UpdateAngle) == 0) {
                    particle.angle = spawnAngle;
                    particle.angleCos = cosDeg;
                    particle.angleSin = sinDeg;
                }
            } else {
                float radius2 = radiusX * radiusX;
                while (true) {
                    float px = getRandom(0.0f, width) - radiusX;
                    float py = getRandom(0.0f, width) - radiusX;
                    if (px * px + py * py <= radius2) {
                        x += px;
                        y += py / scaleY;
                        break;
                    }
                }
            }
            break;
        }
        case GdxPSpawnShapeLine: {
            float width = spawnWidth_ + (spawnWidthDiff_ * entry_.spawnWidth.getScale(percent));
            float height = spawnHeight_ + (spawnHeightDiff_ * entry_.spawnHeight.getScale(percent));
            if (width != 0) {
                float lineX = width * getRandom(0.0f, 1.0f);
                x += lineX;
                y += lineX * (height / width);
            } else
                y += height * getRandom(0.0f, 1.0f);
            break;
        }
        default:
            break;
        }

        particle.setImagePos(x, y);

        int offsetTime = (int)(lifeOffset_ + lifeOffsetDiff_ * entry_.lifeOffset.getScale(percent));
        if (offsetTime > 0) {
            if (offsetTime >= particle.currentLife) {
                offsetTime = particle.currentLife - 1;
            }
            updateParticle(xf, particle, offsetTime / 1000.0f, offsetTime);
        }

        particle.updateImageVertices(image_.aspect());
    }
}
