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

#include "RopeLaunchComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "PhysicsBodyComponent.h"
#include "RenderComponent.h"
#include "Const.h"
#include "Utils.h"
#include "Settings.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    RopeLaunchComponent::RopeLaunchComponent(const SceneObjectPtr& target, const b2Vec2& pos, float velocity)
    : PhasedComponent(phaseThink),
      pos_(pos),
      angle_(0.0f),
      target_(target),
      ropeC_(target->findComponent<PhysicsRopeComponent>()),
      velocity_(velocity),
      visibilityRadius_(ropeC_->segmentWidth() / 2),
      launched_(false)
    {
    }

    RopeLaunchComponent::~RopeLaunchComponent()
    {
    }

    void RopeLaunchComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void RopeLaunchComponent::update(float dt)
    {
        dummy_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (b2Mul(parent()->getTransform(), pos_) - dummy_->pos()));
        dummy_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(parent()->angle() + angle_ - dummy_->angle()));

        if (joints_.size() > 1) {
            for (std::vector<PrismaticJointProxyPtr>::iterator it = joints_.begin();
                it != joints_.end(); ++it) {
                (*it)->objectB()->setLinearVelocity(dummy_->getDirection(velocity_));
            }
        }

        while ((joints_.back()->getJointTranslation() < 0.0f) && (joints_.size() > 1)) {
            for (b2Fixture* f = joints_.back()->objectB()->body()->GetFixtureList(); f; f = f->GetNext()) {
                f->SetSensor(false);
            }
            RenderComponentPtr rc = joints_.back()->objectB()->findComponent<RenderComponent>();
            rc->setVisible(true);
            joints_.back()->remove();
            joints_.pop_back();
        }

        if (joints_.back()->getJointTranslation() <= visibilityRadius_) {
            RenderComponentPtr rc = joints_.back()->objectB()->findComponent<RenderComponent>();
            rc->setVisible(true);
        }

        if (joints_.size() <= 1) {
            launched_ = true;
        }
    }

    void RopeLaunchComponent::cut(float length)
    {
        if (joints_.size() <= 1) {
            return;
        }

        float curLen = visibilityRadius_ * 2.0f * ropeC_->objects().size();

        size_t numCut = 0;

        if (length >= curLen) {
            numCut = 0;
        } else {
            numCut = std::ceil((curLen - length) / (visibilityRadius_ * 2.0f));

            if (numCut > (joints_.size() - 1)) {
                numCut = joints_.size() - 1;
            }
        }

        if (numCut == 0) {
            return;
        }

        ropeC_->cutTail(numCut);

        joints_.erase(joints_.begin(), joints_.begin() + numCut);

        joints_.front()->setLimits(0.0f,
            visibilityRadius_ * 2.0f * (target_->objects().size() + 1));
        joints_.front()->enableLimit(true);
    }

    void RopeLaunchComponent::onRegister()
    {
        dummy_ = sceneObjectFactory.createDummy();

        dummy_->setPos(b2Mul(parent()->getTransform(), pos_));
        dummy_->setAngle(parent()->angle() + angle_);

        scene()->addObject(dummy_);

        for (std::vector<SceneObject*>::const_reverse_iterator it =
             ropeC_->objects().rbegin(); it != ropeC_->objects().rend();
             ++it) {
            setupNested((*it)->shared_from_this());
        }
    }

    void RopeLaunchComponent::onUnregister()
    {
        for (std::vector<PrismaticJointProxyPtr>::iterator it = joints_.begin();
            it != joints_.end(); ++it) {
            for (b2Fixture* f = (*it)->objectB()->body()->GetFixtureList(); f; f = f->GetNext()) {
                f->SetSensor(false);
            }
            RenderComponentPtr rc = (*it)->objectB()->findComponent<RenderComponent>();
            rc->setVisible(true);
            (*it)->remove();
        }

        joints_.clear();

        if (dummy_) {
            dummy_->removeFromParent();
        }

        dummy_.reset();

        ropeC_.reset();

        target_.reset();
    }

    void RopeLaunchComponent::setupNested(const SceneObjectPtr& obj)
    {
        obj->findComponent<RenderComponent>()->setVisible(false);
        for (b2Fixture* f = obj->body()->GetFixtureList(); f; f = f->GetNext()) {
            f->SetSensor(true);
        }
        JointProxyPtr joint;
        if (obj.get() == ropeC_->tail()) {
            joint = scene()->addPrismaticJoint(dummy_, obj,
                b2Vec2_zero, b2Vec2_zero, 0.0f,
                visibilityRadius_ * 2.0f * (target_->objects().size() + 1),
                b2Vec2(-1.0f, 0.0f));
        } else {
            joint = scene()->addPrismaticJoint(dummy_, obj,
                b2Vec2_zero, b2Vec2_zero, b2Vec2(-1.0f, 0.0f));
        }
        PrismaticJointProxyPtr pjoint = boost::dynamic_pointer_cast<PrismaticJointProxy>(joint);
        joints_.push_back(pjoint);
    }
}
