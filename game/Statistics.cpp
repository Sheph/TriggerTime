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

#include "Statistics.h"
#include "SceneObject.h"

namespace af
{
    const float Statistics::timeFireWeight_ = 10.0f;

    Statistics::Statistics()
    : enemiesSpawned_(0),
      enemiesKilled_(0),
      shotsFired_(0),
      shotsHit_(0),
      timeFired_(0.0f),
      timeHit_(0.0f),
      playTime_(0.0f),
      impresses_(0),
      numAnimals_(0),
      numKilledAnimals_(0)
    {
    }

    Statistics::~Statistics()
    {
    }

    bool Statistics::incShotsFired(WeaponComponent* weapon)
    {
        if (weapon->objectType() == SceneObjectTypePlayerMissile) {
            ++shotsFired_;
            return true;
        }
        return false;
    }

    bool Statistics::incShotsHit(SceneObject* shot, SceneObject* target)
    {
        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
            SceneObjectTypeEnemyBuilding | SceneObjectTypeGizmo;

        if ((!shot || (shot->type() == SceneObjectTypePlayerMissile)) && hitTypes[target->type()]) {
            ++shotsHit_;
            return true;
        }
        return false;
    }

    bool Statistics::incTimeFired(WeaponComponent* weapon, float value)
    {
        if (weapon->objectType() == SceneObjectTypePlayerMissile) {
            timeFired_ += value;
            return true;
        }
        return false;
    }

    bool Statistics::incTimeHit(WeaponComponent* weapon, SceneObject* target, float value)
    {
        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
            SceneObjectTypeEnemyBuilding | SceneObjectTypeGizmo;

        if ((weapon->objectType() == SceneObjectTypePlayerMissile) && hitTypes[target->type()]) {
            timeHit_ += value;
            return true;
        }
        return false;
    }

    float Statistics::accuracy() const
    {
        float shotsFired = static_cast<float>(shotsFired_) + (timeFired_ * timeFireWeight_);
        float shotsHit = static_cast<float>(shotsHit_) + (timeHit_ * timeFireWeight_);

        if (shotsFired <= FLT_EPSILON) {
            return 1.0f;
        }

        float acc = shotsHit / shotsFired;

        if (acc > 1.0f) {
            acc = 1.0f;
        }

        return acc;
    }
}
