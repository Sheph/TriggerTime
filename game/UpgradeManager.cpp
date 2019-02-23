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

#include "UpgradeManager.h"
#include "AssetManager.h"
#include "Settings.h"

namespace af
{
    UpgradeManager upgradeManager;

    template <>
    Single<UpgradeManager>* Single<UpgradeManager>::single = NULL;

    static const std::string upgradeImageNames[UpgradeIdMax + 1] = {
        "common1/healthbar_heart.png",
        "common1/upgrade_ammo.png",
        "common1/upgrade_armor.png",
        "common1/upgrade_blaster.png"
    };

    UpgradeManager::UpgradeManager()
    {
        clear();
    }

    UpgradeManager::~UpgradeManager()
    {
    }

    bool UpgradeManager::init()
    {
        return true;
    }

    void UpgradeManager::shutdown()
    {
    }

    const std::string& UpgradeManager::getImageName(UpgradeId upgrade)
    {
        return upgradeImageNames[upgrade];
    }

    Image UpgradeManager::getImage(UpgradeId upgrade)
    {
        return assetManager.getImage(getImageName(upgrade));
    }

    UInt32 UpgradeManager::getCost(UpgradeId upgrade)
    {
        return settings.upgradeCosts[upgrade];
    }

    void UpgradeManager::give(UpgradeId upgrade, float amount)
    {
        upgrades_[upgrade] += amount;
    }

    float UpgradeManager::count(UpgradeId upgrade) const
    {
        return upgrades_[upgrade];
    }

    void UpgradeManager::clear()
    {
        for (int i = 0; i <= UpgradeIdMax; ++i) {
            upgrades_[i] = 0.0f;
        }
    }
}
