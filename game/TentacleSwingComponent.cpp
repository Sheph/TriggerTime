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

#include "TentacleSwingComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SequentialTweening.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TentacleSwingComponent::TentacleSwingComponent()
    : PhasedComponent(phaseThink),
      bone_(1),
      pause_(false)
    {
    }

    TentacleSwingComponent::TentacleSwingComponent(float t1, Easing easing1, float torque1,
        float t2, Easing easing2, float torque2)
    : PhasedComponent(phaseThink),
      bone_(1),
      pause_(false)
    {
        setLoop(t1, easing1, torque1, t2, easing2, torque2);
    }

    TentacleSwingComponent::~TentacleSwingComponent()
    {
    }

    void TentacleSwingComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TentacleSwingComponent::update(float dt)
    {
        if (!tweening_ || pause_) {
            return;
        }

        float value = tweening_->getValue(tweenTime_);

        assert(bone_ < static_cast<int>(tentacle_->objects().size()));

        tentacle_->objects()[bone_]->applyTorque(value, true);

        tweenTime_ += dt;
    }

    void TentacleSwingComponent::setLoop(float t1, Easing easing1, float torque1,
        float t2, Easing easing2, float torque2)
    {
        SequentialTweeningPtr tweening =
            boost::make_shared<SequentialTweening>(true);

        tweening->addTweening(boost::make_shared<SingleTweening>(t1, easing1, torque2, torque1));
        tweening->addTweening(boost::make_shared<SingleTweening>(t2, easing2, torque1, torque2));

        tweening_= tweening;
        tweenTime_ = getRandom(0.0f, tweening_->duration());
    }

    void TentacleSwingComponent::setSingle(float torque1, float torque2, float t, Easing easing)
    {
        tweening_= boost::make_shared<SingleTweening>(t, easing, torque1, torque2);
        tweenTime_ = 0.0f;
    }

    void TentacleSwingComponent::onRegister()
    {
        tentacle_ = parent()->findComponent<RenderTentacleComponent>();
    }

    void TentacleSwingComponent::onUnregister()
    {
    }
}
