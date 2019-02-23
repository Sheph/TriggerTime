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

#include "DamageFlashComponent.h"
#include "SequentialTweening.h"
#include "SingleTweening.h"
#include "Utils.h"
#include "SceneObject.h"
#include <boost/make_shared.hpp>
#include <limits>

namespace af
{
    DamageFlashComponent::DamageFlashComponent(const std::vector<RenderComponentPtr>& rcs)
    : PhasedComponent(phasePreRender),
      rcs_(rcs),
      prevHealth_(0.0f),
      t_(0.0f)
    {
    }

    DamageFlashComponent::DamageFlashComponent(const RenderComponentPtr& rc)
    : PhasedComponent(phasePreRender),
      prevHealth_(0.0f),
      t_(0.0f)
    {
        rcs_.push_back(rc);
    }

    DamageFlashComponent::DamageFlashComponent(const RenderComponentPtr& rc1,
        const RenderComponentPtr& rc2)
    : PhasedComponent(phasePreRender),
      prevHealth_(0.0f),
      t_(0.0f)
    {
        rcs_.push_back(rc1);
        rcs_.push_back(rc2);
    }

    DamageFlashComponent::DamageFlashComponent(const RenderComponentPtr& rc1,
        const RenderComponentPtr& rc2,
        const RenderComponentPtr& rc3)
    : PhasedComponent(phasePreRender),
      prevHealth_(0.0f),
      t_(0.0f)
    {
        rcs_.push_back(rc1);
        rcs_.push_back(rc2);
        rcs_.push_back(rc3);
    }

    DamageFlashComponent::~DamageFlashComponent()
    {
    }

    void DamageFlashComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void DamageFlashComponent::preRender(float dt)
    {
        if ((t_ <= 0.0f) && (parent()->life() < prevHealth_) && !parent()->dead()) {
            t_ = duration_;
            int minZ = (std::numeric_limits<int>::max)();
            for (size_t i = 0; i < rcs_.size(); ++i) {
                if (rcs_[i]->zOrder() < minZ) {
                    minZ = rcs_[i]->zOrder();
                }
                oldZ_[i] = rcs_[i]->zOrder();
            }
            for (size_t i = 0; i < rcs_.size(); ++i) {
                rcs_[i]->setFlashColor(color_);
                rcs_[i]->setZOrder(rcs_[i]->zOrder() - minZ + 121);
            }
        }

        prevHealth_ = parent()->life();

        if (t_ <= 0.0f) {
            return;
        }

        t_ -= dt;

        if (t_ <= 0.0f) {
            for (size_t i = 0; i < rcs_.size(); ++i) {
                rcs_[i]->setFlashColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
                rcs_[i]->setZOrder(oldZ_[i]);
            }
        }
    }

    void DamageFlashComponent::onFreeze()
    {
        if (t_ > 0.0f) {
            t_ = 0.0f;
            for (size_t i = 0; i < rcs_.size(); ++i) {
                rcs_[i]->setFlashColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
                rcs_[i]->setZOrder(oldZ_[i]);
            }
        }
    }

    void DamageFlashComponent::onRegister()
    {
        prevHealth_ = parent()->life();

        switch (parent()->type()) {
        case SceneObjectTypeEnemy:
        case SceneObjectTypeEnemyBuilding:
            color_ = Color(1.0f, 0.0f, 0.0f, 0.5f);
            duration_ = 0.05f;
            break;
        case SceneObjectTypeGizmo:
        default:
            color_ = Color(1.0f, 1.0f, 0.0f, 0.5f);
            duration_ = 0.05f;
            break;
        }

        oldZ_.resize(rcs_.size());
    }

    void DamageFlashComponent::onUnregister()
    {
        if (t_ > 0.0f) {
            t_ = 0.0f;
            for (size_t i = 0; i < rcs_.size(); ++i) {
                rcs_[i]->setFlashColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
                rcs_[i]->setZOrder(oldZ_[i]);
            }
        }
    }
}
