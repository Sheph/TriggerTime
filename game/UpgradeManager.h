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

#ifndef _UPGRADEMANAGER_H_
#define _UPGRADEMANAGER_H_

#include "af/Single.h"
#include "af/Types.h"
#include "Image.h"

namespace af
{
    enum UpgradeId
    {
        UpgradeIdExtraLife = 0,
        UpgradeIdMoreAmmo,
        UpgradeIdArmor,
        UpgradeIdSuperBlaster,
    };

    static const int UpgradeIdMax = UpgradeIdSuperBlaster;

    class UpgradeManager : public Single<UpgradeManager>
    {
    public:
        UpgradeManager();
        ~UpgradeManager();

        bool init();

        void shutdown();

        static const std::string& getImageName(UpgradeId upgrade);

        static Image getImage(UpgradeId upgrade);

        static UInt32 getCost(UpgradeId upgrade);

        void give(UpgradeId upgrade, float amount);

        float count(UpgradeId upgrade) const;

        void clear();

    private:
        float upgrades_[UpgradeIdMax + 1];
    };

    extern UpgradeManager upgradeManager;
}

#endif
