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

#include "UserData.h"
#include "Platform.h"
#include "Logger.h"
#include "Settings.h"
#include "af/StreamAppConfig.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    UserData userData;

    template <>
    Single<UserData>* Single<UserData>::single = NULL;

    UserData::UserData()
    {
        for (int i = 0; i <= SkillMax; ++i) {
            numGemsCached_[i] = 0;
        }
    }

    UserData::~UserData()
    {
    }

    bool UserData::init()
    {
        readUserData();
        if (numGemsCached_[settings.skill] < settings.atLeastGems) {
            giveGems(settings.atLeastGems - numGemsCached_[settings.skill]);
        }
        return true;
    }

    void UserData::shutdown()
    {
        writeUserData();
    }

    UInt32 UserData::numGems()
    {
        return numGemsCached_[settings.skill];
    }

    void UserData::giveGems(UInt32 value)
    {
        numGemsCached_[settings.skill] += value;
        writeUserData();
    }

    void UserData::takeGems(UInt32 value)
    {
        if (numGemsCached_[settings.skill] > value) {
            numGemsCached_[settings.skill] -= value;
        } else {
            numGemsCached_[settings.skill] = 0;
        }
        writeUserData();
    }

    bool UserData::levelAccessible(const std::string& name)
    {
        if (settings.allLevelsAccessible) {
            return true;
        }

        return levelsAccessibleCached_[settings.skill].count(name);
    }

    void UserData::setLevelAccessible(const std::string& name)
    {
        levelsAccessibleCached_[settings.skill].insert(name);
        writeUserData();
    }

    void UserData::readUserData()
    {
        for (int i = 0; i <= SkillMax; ++i) {
            numGemsCached_[i] = 0;
            levelsAccessibleCached_[i].clear();

            levelsAccessibleCached_[i].insert("e1m1");
            levelsAccessibleCached_[i].insert("e1m2");
        }

        std::string userSaveData = platform->readUserSaveData();

        if (userSaveData.empty()) {
            return;
        }

        for (size_t i = 0; i < userSaveData.size(); ++i) {
            userSaveData[i] -= 0x80;
        }

        userSaveData = base64Decode(userSaveData);

        boost::shared_ptr<af::StreamAppConfig> sd =
            boost::make_shared<af::StreamAppConfig>();

        std::istringstream is(userSaveData);

        if (!sd->load(is)) {
            LOG4CPLUS_WARN(logger(), "Error parsing user save data file!");
            return;
        }

        if (!sd->haveKey(".gems")) {
            return;
        }

        int gems = sd->getInt(".gems");
        if (gems < 0) {
            return;
        }

        numGemsCached_[SkillNormal] = gems;

        std::vector<std::string> levels = sd->getSubKeys("level");

        for (std::vector<std::string>::const_iterator it = levels.begin();
            it != levels.end(); ++it) {
            if (sd->getBool(std::string("level.") + *it)) {
                levelsAccessibleCached_[SkillNormal].insert(*it);
            }
        }

        if (sd->haveKey(".easy-gems")) {
            gems = sd->getInt(".easy-gems");
            if (gems >= 0) {
                numGemsCached_[SkillEasy] = gems;

                levels = sd->getSubKeys("easy-level");

                for (std::vector<std::string>::const_iterator it = levels.begin();
                    it != levels.end(); ++it) {
                    if (sd->getBool(std::string("easy-level.") + *it)) {
                        levelsAccessibleCached_[SkillEasy].insert(*it);
                    }
                }
            }
        }

        if (sd->haveKey(".hard-gems")) {
            gems = sd->getInt(".hard-gems");
            if (gems >= 0) {
                numGemsCached_[SkillHard] = gems;

                levels = sd->getSubKeys("hard-level");

                for (std::vector<std::string>::const_iterator it = levels.begin();
                    it != levels.end(); ++it) {
                    if (sd->getBool(std::string("hard-level.") + *it)) {
                        levelsAccessibleCached_[SkillHard].insert(*it);
                    }
                }
            }
        }
    }

    void UserData::writeUserData()
    {
        std::ostringstream os;

        os << "easy-gems=" << numGemsCached_[SkillEasy] << "\n";
        os << "gems=" << numGemsCached_[SkillNormal] << "\n";
        os << "hard-gems=" << numGemsCached_[SkillHard] << "\n\n";

        os << "[easy-level]\n";
        for (StringSet::const_iterator it = levelsAccessibleCached_[SkillEasy].begin();
            it != levelsAccessibleCached_[SkillEasy].end(); ++it) {
            os << *it << "=true\n";
        }

        os << "[level]\n";
        for (StringSet::const_iterator it = levelsAccessibleCached_[SkillNormal].begin();
            it != levelsAccessibleCached_[SkillNormal].end(); ++it) {
            os << *it << "=true\n";
        }

        os << "[hard-level]\n";
        for (StringSet::const_iterator it = levelsAccessibleCached_[SkillHard].begin();
            it != levelsAccessibleCached_[SkillHard].end(); ++it) {
            os << *it << "=true\n";
        }

        std::string res = base64Encode(os.str());

        for (size_t i = 0; i < res.size(); ++i) {
            res[i] += 0x80;
        }

        platform->writeUserSaveData(res);
    }
}
