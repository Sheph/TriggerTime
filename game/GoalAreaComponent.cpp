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

#include "GoalAreaComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "Settings.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    GoalAreaComponent::GoalAreaComponent()
    : PhasedComponent(phasePreRender),
      inside_(false),
      goalLastPos_(b2Vec2_zero)
    {
    }

    GoalAreaComponent::~GoalAreaComponent()
    {
    }

    void GoalAreaComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GoalAreaComponent::preRender(float dt)
    {
        if (obj_ && obj_->dead()) {
            inside_ = false;
        }

        if (inside_ && !indicator_) {
            indicator_ = boost::make_shared<GoalIndicatorComponent>(settings.goalIndicator.length, -1);

            indicator_->setPadding(settings.goalIndicator.length);
            indicator_->setColor(Color(1.0f, 1.0f, 1.0f, 0.0f));

            parent()->addComponent(indicator_);
        } else if (!inside_ && indicator_) {
            indicator_->setDirection(goalLastPos_ - obj_->smoothPos());
            if (fade(dt, false)) {
                indicator_->removeFromParent();
                indicator_.reset();
                obj_.reset();
            }
            return;
        }

        if (!indicator_) {
            return;
        }

        b2AABB aabb;

        if (!goals_.empty()) {
            aabb = scene()->camera()->findComponent<CameraComponent>()->getAABB();
        }

        const Goal* goal = NULL;
        float minDistSq = b2_maxFloat;

        for (Goals::const_iterator it = goals_.begin();
             it != goals_.end(); ++it) {
            b2Vec2 pos;
            if (it->obj) {
                if (it->aliveOnly && it->obj->dead()) {
                    continue;
                }
                pos = it->obj->pos();
            } else {
                pos = it->pos;
            }
            bool visible = pointInRect(pos, aabb);
            if (visible) {
                goal = NULL;
                break;
            }

            float distSq = b2DistanceSquared(obj_->smoothPos(), pos);
            if (distSq <= minDistSq) {
                minDistSq = distSq;
                goal = &(*it);
            }
        }

        if (goal) {
            b2Vec2 pos;
            if (goal->obj) {
                pos = goal->obj->pos();
            } else {
                pos = goal->pos;
            }
            goalLastPos_ = pos;
        }

        fade(dt, goal != NULL);
        indicator_->setDirection(goalLastPos_ - obj_->smoothPos());
    }

    void GoalAreaComponent::sensorEnter(const SceneObjectPtr& other)
    {
        setObject(other);
    }

    void GoalAreaComponent::sensorExit(const SceneObjectPtr& other)
    {
        setObject(SceneObjectPtr());
    }

    void GoalAreaComponent::setObject(const SceneObjectPtr& value)
    {
        inside_ = !!value;
        if (value) {
            obj_ = value;
        }
    }

    void GoalAreaComponent::addGoal(const b2Vec2& pos)
    {
        removeGoal(pos);
        goals_.push_back(Goal(pos));
    }

    void GoalAreaComponent::removeGoal(const b2Vec2& pos)
    {
        for (Goals::iterator it = goals_.begin();
             it != goals_.end(); ++it) {
            if (!it->obj && (it->pos == pos)) {
                goals_.erase(it);
                break;
            }
        }
    }

    void GoalAreaComponent::removeAllGoals()
    {
        goals_.clear();
    }

    void GoalAreaComponent::addGoal(const SceneObjectPtr& obj, bool aliveOnly)
    {
        removeGoal(obj);
        goals_.push_back(Goal(obj, aliveOnly));
    }

    void GoalAreaComponent::removeGoal(const SceneObjectPtr& obj)
    {
        for (Goals::iterator it = goals_.begin();
             it != goals_.end(); ++it) {
            if (it->obj && (it->obj == obj)) {
                goals_.erase(it);
                break;
            }
        }
    }

    void GoalAreaComponent::onRegister()
    {
    }

    void GoalAreaComponent::onUnregister()
    {
        obj_.reset();
        if (indicator_) {
            indicator_->removeFromParent();
            indicator_.reset();
        }
        goals_.clear();
    }

    bool GoalAreaComponent::fade(float dt, bool in)
    {
        Color c = indicator_->color();
        bool res = false;

        if (in) {
            c.rgba[3] += (settings.goalIndicator.alpha / settings.goalIndicator.timeout) * dt;
            if (c.rgba[3] > settings.goalIndicator.alpha) {
                c.rgba[3] = settings.goalIndicator.alpha;
                res = true;
            }
            indicator_->setColor(c);
        } else {
            c.rgba[3] -= (settings.goalIndicator.alpha / settings.goalIndicator.timeout) * dt;
            if (c.rgba[3] < 0.0f) {
                c.rgba[3] = 0.0f;
                res = true;
            }
            indicator_->setColor(c);
        }

        return res;
    }
}
