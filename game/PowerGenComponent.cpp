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

#include "PowerGenComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "FadeOutComponent.h"
#include "RenderHealthbarComponent.h"
#include "AssetManager.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    PowerGenComponent::PowerGenComponent(const RenderQuadComponentPtr& rc,
        const RenderQuadComponentPtr& lampRc,
        const LightPtr& light,
        const LightPtr fireLight[3])
    : PhasedComponent(phasePreRender),
      rc_(rc),
      lampRc_(lampRc),
      light_(light),
      dying_(false),
      wasFreezable_(false)
    {
        fireLight_[0] = fireLight[0];
        fireLight_[1] = fireLight[1];
        fireLight_[2] = fireLight[2];

        fireTime_[0] = 0.0001f;
        fireTime_[1] = 0.5f;
        fireTime_[2] = 1.0f;
    }

    PowerGenComponent::~PowerGenComponent()
    {
    }

    void PowerGenComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PowerGenComponent::preRender(float dt)
    {
        if (dying_) {
            bool done = true;

            for (int i = 0; i < 3; ++i) {
                if (fireTime_[i] > 0.0f) {
                    done = false;

                    fireTime_[i] -= dt;

                    if (fireTime_[i] <= 0.0f) {
                        fireTime_[i] = 0.0f;
                        light_->setVisible(false);

                        SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);
                        explosion->setPos(parent()->getWorldPoint(fireLight_[i]->pos()));
                        scene()->addObject(explosion);

                        fireLight_[i]->setVisible(true);

                        ParticleEffectComponentPtr pec = assetManager.getParticleEffect("fire1.p",
                            b2Vec2_zero, 0.0f, false);

                        pec->setTransform(b2Transform(fireLight_[i]->pos(), b2Rot(0.0f)));

                        pec->setFixedAngle(true);

                        pec->setZOrder(zOrderEffects);

                        pec->resetEmit();

                        parent()->addComponent(pec);
                    }
                }
            }

            if (done) {
                parent()->setFreezable(wasFreezable_);
                removeFromParent();
            }

            return;
        }

        if (parent()->life() <= 0) {
            wasFreezable_ = parent()->freezable();
            parent()->setFreezable(false);
            dying_ = true;

            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            return;
        }

        float value = tweening_->getValue(tweenTime_);

        rc_->setPos(pos_[0] + b2Vec2(0.0f, value));
        lampRc_->setPos(pos_[1] + b2Vec2(0.0f, value));
        light_->setPos(pos_[2] + b2Vec2(0.0f, value));

        tweenTime_ += dt;
    }

    void PowerGenComponent::onRegister()
    {
        pos_[0] = rc_->pos();
        pos_[1] = lampRc_->pos();
        pos_[2] = light_->pos();

        SequentialTweeningPtr tweening = boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(0.05f, EaseLinear, -0.08f, 0.08f));
        tweening->addTweening(boost::make_shared<SingleTweening>(0.05f, EaseLinear, 0.08f, -0.08f));

        tweening_ = tweening;
        tweenTime_ = getRandom(0.0f, tweening->duration());
    }

    void PowerGenComponent::onUnregister()
    {
    }
}
