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

#include "NapalmComponent.h"
#include "ExplosionComponent.h"
#include "SceneObjectFactory.h"
#include "RenderQuadComponent.h"
#include "PhysicsBodyComponent.h"
#include "LightComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "SingleTweening.h"
#include "AssetManager.h"
#include <boost/make_shared.hpp>

namespace af
{
    NapalmComponent::NapalmComponent(const ParticleEffectComponentPtr& pec,
        float damage, bool haveSound)
    : TargetableComponent(phaseThink),
      pec_(pec),
      damage_(damage),
      snd_(haveSound ? audio.createSound("napalm_ball.ogg") : AudioSourcePtr())
    {
    }

    NapalmComponent::~NapalmComponent()
    {
    }

    void NapalmComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void NapalmComponent::update(float dt)
    {
        updateAutoTarget(dt);

        if (!target()) {
            return;
        }

        const ParticleEmmiter::Particles& particles = pec_->emmiters()[1]->particles();

        bool hit = false;

        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].active || (particles[i].currentLife <= 400)) {
                continue;
            }

            if (b2DistanceSquared(b2Mul(particles[i].imageStartXf, particles[i].imagePos), target()->pos()) < 5.5f * 5.5f) {
                hit = true;
                break;
            }
        }

        if (hit) {
            target()->changeLife(-damage_ * dt);
        }
    }

    void NapalmComponent::onFreeze()
    {
        parent()->removeFromParent();
    }

    void NapalmComponent::onRegister()
    {
        if (snd_) {
            snd_->play();
        }
    }

    void NapalmComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
    }
}
