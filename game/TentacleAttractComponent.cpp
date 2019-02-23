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

#include "TentacleAttractComponent.h"
#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    TentacleAttractComponent::TentacleAttractComponent(float frequency, float dampingRatio)
    : TargetableComponent(phaseThink),
      bone_(-1),
      frequency_(frequency),
      dampingRatio_(dampingRatio)
    {
    }

    TentacleAttractComponent::~TentacleAttractComponent()
    {
    }

    void TentacleAttractComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void TentacleAttractComponent::update(float dt)
    {
        if (!target()) {
            if (joint_) {
                joint_->remove();
                joint_.reset();
            }
            return;
        }

        if (!joint_) {
            assert(bone_ < static_cast<int>(tentacle_->objects().size()));

            dummy_->setTransform(target()->getTransform());
            joint_ = scene()->addDistanceJoint(tentacle_->objects()[bone_], dummy_,
                b2Vec2_zero, b2Vec2_zero, 0.0f,
                frequency_, dampingRatio_, false);
        }

        dummy_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (target()->pos() - dummy_->pos()));
        dummy_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(target()->angle() - dummy_->angle()));
    }

    void TentacleAttractComponent::onRegister()
    {
        tentacle_ = parent()->findComponent<RenderTentacleComponent>();

        dummy_ = sceneObjectFactory.createDummy();
        scene()->addObject(dummy_);

        if (bone_ == -1) {
            bone_ = tentacle_->objects().size() - 1;
        }

        assert(bone_ < static_cast<int>(tentacle_->objects().size()));

        if (target()) {
            dummy_->setTransform(target()->getTransform());
            joint_ = scene()->addDistanceJoint(tentacle_->objects()[bone_], dummy_,
                b2Vec2_zero, b2Vec2_zero, 0.0f,
                frequency_, dampingRatio_, false);
        }
    }

    void TentacleAttractComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        dummy_->removeFromParent();
        dummy_.reset();
    }
}
