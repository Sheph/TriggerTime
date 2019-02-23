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

#include "ShroomSnakeComponent.h"
#include "Scene.h"
#include "Settings.h"

namespace af
{
    ShroomSnakeComponent::ShroomSnakeComponent(const SceneObjectPtr& upperLip,
                                               const b2Vec2& upperLipPos,
                                               const SceneObjectPtr& lowerLip,
                                               const b2Vec2& lowerLipPos,
                                               const AnimationComponentPtr& ac,
                                               float defAngle,
                                               const std::vector<float>& attackAngles)
    : PhasedComponent(phaseThink),
      upperLip_(upperLip),
      upperLipPos_(upperLipPos),
      lowerLip_(lowerLip),
      lowerLipPos_(lowerLipPos),
      ac_(ac),
      defAngle_(defAngle),
      attackAngles_(attackAngles),
      numPlayerEntered_(0),
      allyBitten_(false)
    {
    }

    ShroomSnakeComponent::~ShroomSnakeComponent()
    {
    }

    void ShroomSnakeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void ShroomSnakeComponent::update(float dt)
    {
        if (ac_->animationFinished()) {
            if (ally_ && !ally_->scene()) {
                ally_.reset();
                allyBitten_ = false;
            }

            if (ally_ && ally_->scene()) {
                if (!allyBitten_) {
                    if (ac_->currentAnimation() == AnimationBite) {
                        scene()->addWeldJoint(lowerLip_, ally_, ally_->pos());
                        scene()->addWeldJoint(upperLip_, ally_, ally_->pos());
                        allyBitten_ = true;
                    } else {
                        ac_->startAnimation(AnimationBite);
                    }
                }
            } else if (numPlayerEntered_ > 0) {
                ac_->startAnimation(AnimationAttack);
            } else if (ac_->currentAnimation() != AnimationDefault) {
                ac_->startAnimation(AnimationDefault);
            }
        }

        const b2Transform& t = parent()->getTransform();

        float angle;

        if (ac_->currentAnimation() != AnimationDefault) {
            angle = attackAngles_[ac_->animationFrameIndex()];
        } else {
            angle = defAngle_;
        }

        b2Vec2 pos = b2Mul(t, upperLipPos_);

        upperLip_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (pos - upperLip_->pos()));
        upperLip_->setAngle(parent()->angle() + angle);

        pos = b2Mul(t, lowerLipPos_);

        lowerLip_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (pos - lowerLip_->pos()));
        lowerLip_->setAngle(parent()->angle() - angle);
    }

    void ShroomSnakeComponent::sensorEnter(const SceneObjectPtr& other)
    {
        if (other->type() == SceneObjectTypePlayer) {
            ++numPlayerEntered_;
        } else if (!ally_) {
            ally_ = other;
        }
    }

    void ShroomSnakeComponent::sensorExit(const SceneObjectPtr& other)
    {
        if (other->type() == SceneObjectTypePlayer) {
            --numPlayerEntered_;
        }
    }

    void ShroomSnakeComponent::onRegister()
    {
    }

    void ShroomSnakeComponent::onUnregister()
    {
        ally_.reset();
    }
}
