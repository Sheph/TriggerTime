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

#include "SpawnerComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    SpawnerComponent::SpawnerComponent(const b2Vec2& pos, float glowRadius)
    : PhasedComponent(phaseThink),
      pos_(pos),
      glowRadius_(glowRadius),
      tweenTime_(0.0f),
      sndDone_(audio.createSound("spawner_done.ogg"))
    {
    }

    SpawnerComponent::~SpawnerComponent()
    {
    }

    void SpawnerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SpawnerComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            scene()->addObject(explosion);

            parent()->removeFromParent();

            return;
        }

        if (!tweening_) {
            return;
        }

        float value = tweening_->getValue(tweenTime_);

        lineLight_->setDistance(value * 2.0f);
        pointLight_->setDistance(value * glowRadius_);

        tweenTime_ += dt;

        if (tweening_->finished(tweenTime_)) {
            cleanupSpawn();
        }
    }

    void SpawnerComponent::startSpawn(const b2Vec2& dest)
    {
        cleanupSpawn();

        beamAnimation_ = sceneObjectFactory.createLightningAnimation();

        beamAnimation_->startAnimation(AnimationDefault);

        float angle = vec2angle(dest - pos_);

        beam_ = boost::make_shared<RenderBeamComponent>(pos_,
            angle, 4.0f,
            beamAnimation_->drawable(), zOrderEffects - 1);

        beam_->setLength((dest - pos_).Length());

        lineLight_ = boost::make_shared<LineLight>();
        lineLight_->setDiffuse(false);
        lineLight_->setColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
        lineLight_->setXray(true);
        lineLight_->setDistance(0.0f);
        lineLight_->setBothWays(true);
        lineLight_->setAngle((b2_pi / 2.0f) + angle);
        lineLight_->setPos(pos_ + angle2vec(angle, (dest - pos_).Length() / 2));
        lineLight_->setLength((dest - pos_).Length() / 2);

        pointLight_ = boost::make_shared<PointLight>();
        pointLight_->setColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
        pointLight_->setDistance(0.0f);
        pointLight_->setXray(true);
        pointLight_->setPos(pos_);

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(lineLight_);
        lightC_->attachLight(pointLight_);

        parent()->addComponent(beam_);
        parent()->addComponent(beamAnimation_);
        parent()->addComponent(lightC_);

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseOutQuad, 0.0f, 4.0f / 3.0f));

        SequentialTweeningPtr tweening2 = boost::make_shared<SequentialTweening>(true);

        tweening2->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (4.0f / 3.0f), (3.0f / 4.0f)));
        tweening2->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));

        tweening->addTweening(tweening2);

        tweening_ = tweening;
        tweenTime_ = 0.0f;
    }

    void SpawnerComponent::finishSpawn()
    {
        if (tweening_) {
            SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

            float t = std::fmod(tweenTime_, 0.6f);

            if (t >= 0.3f) {
                tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (4.0f / 3.0f), (3.0f / 4.0f)));
                tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));
                tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInQuad, (4.0f / 3.0f), 0.0f));
                tweenTime_ = t - 0.3f;
            } else {
                tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInOutQuad, (3.0f / 4.0f), (4.0f / 3.0f)));
                tweening->addTweening(boost::make_shared<SingleTweening>(0.3f, EaseInQuad, (4.0f / 3.0f), 0.0f));
                tweenTime_ = t;
            }

            tweening_ = tweening;

            sndDone_->play();
        }
    }

    void SpawnerComponent::cleanupSpawn()
    {
        if (beam_) {
            beam_->removeFromParent();
            beam_.reset();
            beamAnimation_->removeFromParent();
            beamAnimation_.reset();
            lightC_->removeFromParent();
            lightC_.reset();
            lineLight_.reset();
            pointLight_.reset();
            tweening_.reset();
        }
    }

    void SpawnerComponent::onRegister()
    {
    }

    void SpawnerComponent::onUnregister()
    {
        cleanupSpawn();
    }
}
