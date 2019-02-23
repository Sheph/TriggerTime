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

#include "TargetableComponent.h"
#include "Scene.h"
#include "CameraComponent.h"
#include <boost/bind.hpp>

namespace af
{
    TargetableComponent::TargetableComponent(UInt32 phases)
    : PhasedComponent(phases),
      autoTarget_(false),
      patrol_(false),
      t_(0.0f),
      followPlayer_(false)
    {
    }

    TargetableComponent::~TargetableComponent()
    {
    }

    void TargetableComponent::updateAutoTarget(float dt)
    {
        if (!autoTarget_ || !scene()) {
            return;
        }

        if (parent()->type() == SceneObjectTypeAlly) {
            updateAutoTargetAlly(dt);
        } else {
            updateAutoTargetEnemy(dt);
        }
    }

    void TargetableComponent::updateAutoTargetEnemy(float dt)
    {
        if (scene()->combatAllies().empty()) {
            if (scene()->player() && scene()->player()->alive()) {
                target_ = scene()->player();
            }
            return;
        }

        t_ -= dt;

        if (t_ > 0.0f) {
            return;
        }

        float minDist = b2_maxFloat;
        SceneObject* obj = NULL;

        const std::set<SceneObjectPtr>& targets = scene()->queryCombatTargets();

        for (std::set<SceneObjectPtr>::const_iterator it = targets.begin();
            it != targets.end(); ++it) {
            SceneObject* tmp = (*it).get();

            if (!tmp->scene() || tmp->dead()) {
                continue;
            }

            if ((tmp->type() != SceneObjectTypePlayer) && (tmp->type() != SceneObjectTypeAlly)) {
                continue;
            }

            float dist = (tmp->pos() - parent()->pos()).LengthSquared();

            if (dist < minDist) {
                minDist = dist;
                obj = tmp;
            }
        }

        bool hadTarget = !!target_;

        if (obj) {
            target_ = obj->shared_from_this();
        }

        if (hadTarget) {
            t_ = 1.0f;
        } else {
            t_ = 0.1f;
        }
    }

    void TargetableComponent::updateAutoTargetAlly(float dt)
    {
        if (target_ && (target_->type() != SceneObjectTypePlayer) && target_->alive()) {
            CameraComponentPtr c = scene()->camera()->findComponent<CameraComponent>();
            if (c->rectVisible(target_->pos(), 4.0f, 4.0f)) {
                return;
            }
            target_.reset();
        }

        if (target_ && target_->dead()) {
            target_.reset();
        }

        t_ -= dt;

        if (t_ > 0.0f) {
            return;
        }

        float minDist = b2_maxFloat;
        SceneObject* obj = NULL;

        const std::set<SceneObjectPtr>& targets = scene()->queryCombatTargets();

        for (std::set<SceneObjectPtr>::const_iterator it = targets.begin();
            it != targets.end(); ++it) {
            SceneObject* tmp = (*it).get();

            if (!tmp->scene() || tmp->dead()) {
                continue;
            }

            if ((tmp->type() != SceneObjectTypeEnemy) && (tmp->type() != SceneObjectTypeEnemyBuilding)) {
                continue;
            }

            float dist = (tmp->pos() - parent()->pos()).LengthSquared();

            if (dist < minDist) {
                minDist = dist;
                obj = tmp;
            }
        }

        if (obj) {
            target_ = obj->shared_from_this();
            patrol_ = false;
        } else if (followPlayer_ && scene()->player() && scene()->player()->alive()) {
            target_ = scene()->player();
            patrol_ = true;
        }

        t_ = 0.1f;
    }
}
