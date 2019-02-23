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

#include "ParticleEffectComponent.h"
#include "SceneObject.h"

namespace af
{
    ParticleEffectComponent::ParticleEffectComponent(int zOrder)
    : RenderComponent(zOrder),
      cookie_(0),
      fixedAngle_(false),
      paused_(false),
      timeFactor_(1),
      first_(true)
    {
        xf_.SetIdentity();
    }

    ParticleEffectComponent::~ParticleEffectComponent()
    {
    }

    void ParticleEffectComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void ParticleEffectComponent::update(float dt)
    {
        if (emmiters_.empty()) {
            return;
        }

        if (!paused_) {
            if ((timeFactor_ != 1) && !first_) {
                float subDt = dt / timeFactor_;
                float totalDt = 0.0f;
                for (int j = 0; j < (timeFactor_ - 1); ++j) {
                    float a = static_cast<float>(j + 1) / timeFactor_;

                    b2Transform curXf = parent()->getSmoothTransform();

                    curXf.p = a * curXf.p + (1.0f - a) * prevXf_.p;
                    curXf.q.s = a * curXf.q.s + (1.0f - a) * prevXf_.q.s;
                    curXf.q.c = a * curXf.q.c + (1.0f - a) * prevXf_.q.c;

                    b2Transform xf = b2Mul(curXf, xf_);

                    if (fixedAngle_) {
                        xf.q = xf_.q;
                    }

                    for (size_t i = 0; i < emmiters_.size(); ++i) {
                        emmiters_[i]->update(curXf, subDt);
                    }

                    totalDt += subDt;
                }

                b2Transform xf = b2Mul(parent()->getSmoothTransform(), xf_);

                if (fixedAngle_) {
                    xf.q = xf_.q;
                }

                for (size_t i = 0; i < emmiters_.size(); ++i) {
                    emmiters_[i]->update(xf, dt - totalDt);
                }
            } else {
                b2Transform xf = b2Mul(parent()->getSmoothTransform(), xf_);

                if (fixedAngle_) {
                    xf.q = xf_.q;
                }

                for (size_t i = 0; i < emmiters_.size(); ++i) {
                    emmiters_[i]->update(xf, dt);
                }
            }

            prevXf_ = parent()->getSmoothTransform();

            first_ = false;
        }

        b2AABB aabb = emmiters_[0]->aabb();

        for (size_t i = 1; i < emmiters_.size(); ++i) {
            aabb.Combine(emmiters_[i]->aabb());
        }

        if ((parent()->smoothPos() == prevPos_) &&
            (aabb.lowerBound == prevAABB_.lowerBound) &&
            (aabb.upperBound == prevAABB_.upperBound)) {
            return;
        }

        b2Vec2 displacement = parent()->smoothPos() - prevPos_;

        manager()->moveAABB(cookie_, prevAABB_, aabb, displacement);

        prevPos_ = parent()->smoothPos();
        prevAABB_ = aabb;
    }

    void ParticleEffectComponent::render(void* const* parts, size_t numParts)
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            emmiters_[i]->render();
        }
    }

    void ParticleEffectComponent::addEmmiter(const ParticleEmmiterPtr& emmiter)
    {
        emmiters_.push_back(emmiter);
    }

    void ParticleEffectComponent::resetEmit()
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            emmiters_[i]->resetEmit();
        }
    }

    void ParticleEffectComponent::allowCompletion()
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            emmiters_[i]->allowCompletion();
        }
    }

    bool ParticleEffectComponent::isAllowCompletion() const
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            if (!emmiters_[i]->isAllowCompletion()) {
                return false;
            }
        }
        return true;
    }

    bool ParticleEffectComponent::isComplete()
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            if (!emmiters_[i]->isComplete()) {
                return false;
            }
        }
        return true;
    }

    void ParticleEffectComponent::setDuration(float value)
    {
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            emmiters_[i]->setDuration(value);
        }
    }

    float ParticleEffectComponent::getDuration() const
    {
        float duration = 0.0f;
        for (size_t i = 0; i < emmiters_.size(); ++i) {
            duration = b2Max(duration, emmiters_[i]->getDuration());
        }
        return duration;
    }

    void ParticleEffectComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();

        if (emmiters_.empty()) {
            return;
        }

        b2Transform xf = b2Mul(parent()->getSmoothTransform(), xf_);

        emmiters_[0]->resetAABB(xf);
        prevAABB_ = emmiters_[0]->aabb();

        for (size_t i = 1; i < emmiters_.size(); ++i) {
            emmiters_[i]->resetAABB(xf);
            prevAABB_.Combine(emmiters_[i]->aabb());
        }

        cookie_ = manager()->addAABB(this, prevAABB_, NULL);
    }

    void ParticleEffectComponent::onUnregister()
    {
        if (!emmiters_.empty()) {
            manager()->removeAABB(cookie_);
        }
    }
}
