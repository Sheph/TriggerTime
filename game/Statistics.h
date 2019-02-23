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

#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "af/Types.h"
#include "SceneObjectManager.h"
#include "WeaponComponent.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class Statistics : boost::noncopyable
    {
    public:
        Statistics();
        ~Statistics();

        inline void incEnemiesSpawned() { ++enemiesSpawned_; }
        inline void incEnemiesKilled() { ++enemiesKilled_; }

        bool incShotsFired(WeaponComponent* weapon);
        bool incShotsHit(SceneObject* shot, SceneObject* target);

        bool incTimeFired(WeaponComponent* weapon, float value);
        bool incTimeHit(WeaponComponent* weapon, SceneObject* target, float value);

        inline void incPlayTime(float value) { playTime_ += value; }

        inline UInt32 enemiesSpawned() const { return enemiesSpawned_; }
        inline UInt32 enemiesKilled() const { return enemiesKilled_; }

        float accuracy() const;

        inline float playTime() const { return playTime_; }

        inline void incImpresses() { ++impresses_; }
        inline void resetImpresses() { impresses_ = 0; }
        inline UInt32 impresses() const { return impresses_; }

        inline void incNumAnimals() { ++numAnimals_; }
        inline void incNumKilledAnimals() { ++numKilledAnimals_; }

        inline UInt32 numAnimals() const { return numAnimals_; }
        inline UInt32 numKilledAnimals() const { return numKilledAnimals_; }

    private:
        static const float timeFireWeight_;

        UInt32 enemiesSpawned_;
        UInt32 enemiesKilled_;

        UInt32 shotsFired_;
        UInt32 shotsHit_;

        float timeFired_;
        float timeHit_;

        float playTime_;

        UInt32 impresses_;

        UInt32 numAnimals_;
        UInt32 numKilledAnimals_;
    };

    typedef boost::shared_ptr<Statistics> StatisticsPtr;
}

#endif
