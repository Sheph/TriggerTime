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

#include "ToxicCloudComponent.h"
#include "SceneObject.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "Scene.h"
#include "Renderer.h"
#include "SceneObjectFactory.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    static const int32 circleSegments = 30;
    static const float32 circleIncrement = 2.0f * b2_pi / circleSegments;

    ToxicCloudComponent::ToxicCloudComponent(const ParticleEffectComponentPtr& pec,
        float distance, float duration1, float duration2, float damage, float damageTimeout, float effectDuration)
    : PhasedComponent(phaseThink),
      pec_(pec),
      tweenTime_(0.0f),
      damageT_(0.0f),
      effectT_(effectDuration),
      effectDuration_(effectDuration),
      damage_(damage),
      damageTimeout_(damageTimeout),
      snd_(audio.createSound("toxic2.ogg"))
    {
        light_ = boost::make_shared<PointLight>();
        light_->setColor(Color(0.0f, 1.0f, 0.0f, 0.0f));
        light_->setDynamic(false);
        light_->setDiffuse(false);
        light_->setDistance(distance);
        light_->setXray(true);
        light_->setIntensity(1.5f);

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>();

        tweening->addTweening(boost::make_shared<SingleTweening>(duration1,
            EaseOutQuad, 0.0f, 0.4f));
        tweening->addTweening(boost::make_shared<SingleTweening>(duration2,
            EaseInQuad, 0.4f, 0.0f));

        tweening_ = tweening;
    }

    ToxicCloudComponent::~ToxicCloudComponent()
    {
    }

    void ToxicCloudComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ToxicCloudComponent::update(float dt)
    {
        if (tweening_) {
            float value = tweening_->getValue(tweenTime_);

            light_->setColor(Color(0.0f, 1.0f, 0.0f, value));

            if (tweening_->finished(tweenTime_)) {
                tweening_.reset();
                light_->remove();
                light_.reset();
            }

            tweenTime_ += dt;
        }

        if (pec_->isComplete()) {
            parent()->removeFromParent();
            return;
        }

        float damage = -damage_ * b2Sqrt(effectT_ / effectDuration_);

        damageT_ += dt;
        effectT_ -= dt;

        if (damageT_ < damageTimeout_) {
            return;
        }

        damageT_ = 0.0f;

        if (effectT_ <= 0.0f) {
            return;
        }

        b2Vec2 center = damageCenter();
        float radius = damageRadius();

        if (scene()->player() && scene()->player()->alive() &&
            (b2DistanceSquared(scene()->player()->pos(), center) <= radius * radius)) {
            scene()->player()->changeLife(damage);
            for (int i = 0; i < 3; ++i) {
                b2Vec2 offset = angle2vec(getRandom(0.0f, 2.0f * b2_pi), getRandom(0.1f, 0.5f));
                scene()->addObject(sceneObjectFactory.createMissileHit1(MaterialFlesh, scene()->player()->pos() + offset));
            }
            snd_->play();
        }
    }

    void ToxicCloudComponent::debugDraw()
    {
        b2Vec2 center = damageCenter();
        float radius = damageRadius();

        float32 theta = 0.0f;
        b2Vec2 p0;

        renderer.setProgramColorLines(1.0f);

        RenderLineStrip rop = renderer.renderLineStrip();

        for (int32 i = 0; i < circleSegments; ++i) {
            b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

            rop.addVertex(v.x, v.y);

            if (i == 0) {
                p0 = v;
            }

            theta += circleIncrement;
        }

        rop.addVertex(p0.x, p0.y);

        rop.addColors(Color(1.0f, 1.0f, 1.0f));
    }

    void ToxicCloudComponent::onRegister()
    {
        light_->setWorldTransform(parent()->getSmoothTransform());

        scene()->lighting()->addLight(light_);
    }

    void ToxicCloudComponent::onUnregister()
    {
        if (light_) {
            light_->remove();
        }
    }

    b2Vec2 ToxicCloudComponent::damageCenter() const
    {
        return pec_->aabb().GetCenter();
    }

    float ToxicCloudComponent::damageRadius() const
    {
        b2Vec2 ext = pec_->aabb().GetExtents();
        return b2Min(ext.x, ext.y) * 0.65f;
    }
}
