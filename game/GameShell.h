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

#ifndef _GAMESHELL_H_
#define _GAMESHELL_H_

#include "af/Types.h"
#include "af/Single.h"
#include "Image.h"
#include "Const.h"
#include <boost/scoped_ptr.hpp>

namespace af
{
    enum Achievement
    {
        AchievementGravityGunExpert = 0,
        AchievementRoasted,
        AchievementArachnophobia,
        AchievementImpresses,
        AchievementAimBot,
        AchievementFreedom,
        AchievementHeartbroken,
        AchievementThatWasClose,
        AchievementNotAScratch,
        AchievementEatThat,
        AchievementFisticuffs,
        AchievementBugged,
        AchievementWormFeeling,
        AchievementDopefishLives,
        AchievementTurnBack,
        AchievementDieAlready,
        AchievementFreshAir,
        AchievementSaveTheAnimals,
        AchievementKillTheAnimals,
        AchievementImSorryBuddy,
        AchievementTheEnd,
        AchievementHardcore,
        AchievementChicken,
        AchievementExplorer,
    };

    static const int AchievementMax = AchievementExplorer;

    class GameShell : public Single<GameShell>
    {
    public:
        GameShell();
        virtual ~GameShell();

        virtual void update() = 0;

        virtual void reload() = 0;

        virtual Image getAchievementImage(Achievement ach) const = 0;
        virtual std::string getAchievementName(Achievement ach) const = 0;
        virtual std::string getAchievementDescription(Achievement ach) const = 0;

        virtual bool isAchieved(Achievement ach) const = 0;
        virtual void setAchieved(Achievement ach) = 0;

        virtual std::pair<int, int> getAchievementProgress(Achievement ach) const = 0;
        virtual void incAchievementProgress(Achievement ach, int num) = 0;

        virtual Language getLanguage() const = 0;

        std::string script_getAchievementImageName(Achievement ach) const;
    };

    typedef boost::scoped_ptr<GameShell> GameShellPtr;

    extern GameShellPtr gameShell;
}

#endif
