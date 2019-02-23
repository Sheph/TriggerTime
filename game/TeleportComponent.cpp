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

#include "TeleportComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "PhysicsJointComponent.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    TeleportComponent::TeleportComponent(const SceneObjectPtr& ring)
    : PhasedComponent(phaseThink),
      ring_(ring),
      beamRadius_(0.0f),
      i_(0),
      beamTime_(0.0f),
      tweenTime_(0.0f),
      tweenTime2_(0.0f),
      finished_(false),
      strmActive_(audio.createStream("teleport_active.ogg")),
      sndBeam_(audio.createSound("teleport_beam.ogg")),
      strmReady_(audio.createStream("teleport_ready.ogg"))
    {
    }

    TeleportComponent::~TeleportComponent()
    {
    }

    void TeleportComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TeleportComponent::update(float dt)
    {
        if (!joint_->motorEnabled()) {
            return;
        }

        if (tweening2_) {
            float value = tweening2_->getValue(tweenTime2_);

            light_->setDistance(value);

            finished_ = true;

            tweenTime2_ += dt;

            if (tweening2_->finished(tweenTime2_)) {
                tweening2_.reset();
                strmActive_->stop();
                strmReady_->stop();
                sndBeam_->play();
                for (size_t i = 0; i < sizeof(beams_) / sizeof(beams_[0]); ++i) {
                    beams_[i].render->removeFromParent();
                    beams_[i].render.reset();
                    beams_[i].animation->removeFromParent();
                    beams_[i].animation.reset();
                    lightC_->detachLight(beams_[i].light);
                    beams_[i].light->remove();
                    beams_[i].light.reset();
                }
                lightC_->detachLight(light_);
                light_->remove();
                light_.reset();
                joint_->enableMotor(false);
                ring_->setAngularDamping(1.5f);
            }
            return;
        }

        if (tweening_) {
            float value = tweening_->getValue(tweenTime_);

            light_->setDistance(value);

            tweenTime_ += dt;

            if (tweening_->finished(tweenTime_)) {
                tweening_.reset();
                SingleTweeningPtr tweening =
                    boost::make_shared<SingleTweening>(0.3f, EaseLinear, beamRadius_ * 15.0f, beamRadius_ * 20.0f);
                tweening2_ = tweening;
                tweenTime2_ = 0.0f;
            }
            return;
        }

        beamTime_ += dt;

        if (beamTime_ < 3.0f) {
            return;
        }

        if (i_ >= (sizeof(beams_) / sizeof(beams_[0]))) {
            if (!light_) {
                strmReady_->play();

                light_ = boost::make_shared<PointLight>();
                light_->setDynamic(false);
                light_->setDiffuse(false);
                light_->setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
                light_->setDistance(0.0f);

                lightC_->attachLight(light_);

                SingleTweeningPtr tweening =
                    boost::make_shared<SingleTweening>(8.0f, EaseInQuad, 0.0f, beamRadius_ * 5.0f);
                tweening_ = tweening;
                tweenTime_ = 0.0f;
            }
            return;
        }

        beams_[i_].animation = sceneObjectFactory.createLightningAnimation();

        beams_[i_].animation->startAnimation(AnimationDefault);

        float angle = (2.0f * b2_pi * i_) / (sizeof(beams_) / sizeof(beams_[0]));

        beams_[i_].render = boost::make_shared<RenderBeamComponent>(b2Vec2_zero,
            angle, 6.0f,
            beams_[i_].animation->drawable(), zOrderEffects - 1);

        beams_[i_].render->setLength(beamRadius_);

        beams_[i_].light = boost::make_shared<LineLight>();
        beams_[i_].light->setDynamic(false);
        beams_[i_].light->setDiffuse(false);
        beams_[i_].light->setColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
        beams_[i_].light->setDistance(5.0f);
        beams_[i_].light->setBothWays(true);
        beams_[i_].light->setAngle((b2_pi / 2.0f) + angle);
        beams_[i_].light->setPos(angle2vec(angle, beamRadius_ / 2.0f));
        beams_[i_].light->setLength(beamRadius_ / 2.0f);

        lightC_->attachLight(beams_[i_].light);

        parent()->addComponent(beams_[i_].animation);
        parent()->addComponent(beams_[i_].render);

        ++i_;
        beamTime_ = 0.0f;

        sndBeam_->play();
    }

    void TeleportComponent::start(float beamRadius)
    {
        ring_->setAngularDamping(0.0f);
        beamRadius_ = beamRadius;
        joint_->enableMotor(true);
        strmActive_->play();
        i_ = 0;
        beamTime_ = 0.0f;
        finished_ = false;
    }

    void TeleportComponent::onRegister()
    {
        PhysicsJointComponentPtr c =
            parent()->findComponent<PhysicsJointComponent>();

        joint_ = c->joint<RevoluteJointProxy>("joint");

        lightC_ = boost::make_shared<LightComponent>();

        parent()->addComponent(lightC_);
    }

    void TeleportComponent::onUnregister()
    {
        ring_.reset();
    }
}
