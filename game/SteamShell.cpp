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

#include "SteamShell.h"
#include "AssetManager.h"
#include "TextureManager.h"
#include "Logger.h"
#include "Settings.h"
#include "af/Utils.h"
#include <log4cplus/ndc.h>
#include "steam/steam_api.h"

namespace af
{
    static const AppId_t gameAppId = 512920;

    extern "C" void __cdecl SteamWarningMessageHook(int nSeverity, const char* pchDebugText)
    {
        log4cplus::NDCContextCreator ndc("steam_api");

        if (nSeverity == 0) {
            LOG4CPLUS_INFO(logger(), pchDebugText);
        } else {
            LOG4CPLUS_WARN(logger(), pchDebugText);
        }

        if (nSeverity >= 1) {
            // place to set a breakpoint for catching API errors
            int x = 3;
            x = x;
        }
    }

    struct AchievementInfo
    {
        AchievementInfo()
        : reached(false),
          imageId(0),
          maxValue(0)
        {
        }

        std::string id;
        std::string name;
        std::string description;
        bool reached;
        int imageId;
        Image image;
        int maxValue;
    };

    class SteamShell::Impl
    {
    public:
        Impl()
        : userStatsReceivedFn(this, &Impl::userStatsReceived),
          userStatsStoredFn(this, &Impl::userStatsStored),
          userAchievementStoredFn(this, &Impl::userAchievementStored),
          lastTimeUs(0),
          statsValid(false),
          statsRequested(false),
          needStoreStats(false),
          storeStatsTimeout(0.0f),
          enemiesRoasted(0),
          enemiesKilledWithStuff(0),
          language(LanguageEnglish)
        {
            ach[AchievementGravityGunExpert].id = "GravityGunExpert";
            ach[AchievementRoasted].id = "Roasted";
            ach[AchievementArachnophobia].id = "Arachnophobia";
            ach[AchievementImpresses].id = "Impresses";
            ach[AchievementAimBot].id = "AimBot";
            ach[AchievementFreedom].id = "Freedom";
            ach[AchievementHeartbroken].id = "Heartbroken";
            ach[AchievementThatWasClose].id = "ThatWasClose";
            ach[AchievementNotAScratch].id = "NotAScratch";
            ach[AchievementEatThat].id = "EatThat";
            ach[AchievementFisticuffs].id = "Fisticuffs";
            ach[AchievementBugged].id = "Bugged";
            ach[AchievementWormFeeling].id = "WormFeeling";
            ach[AchievementDopefishLives].id = "DopefishLives";
            ach[AchievementTurnBack].id = "TurnBack";
            ach[AchievementDieAlready].id = "DieAlready";
            ach[AchievementFreshAir].id = "FreshAir";
            ach[AchievementSaveTheAnimals].id = "SaveTheAnimals";
            ach[AchievementKillTheAnimals].id = "KillTheAnimals";
            ach[AchievementImSorryBuddy].id = "ImSorryBuddy";
            ach[AchievementTheEnd].id = "TheEnd";
            ach[AchievementHardcore].id = "Hardcore";
            ach[AchievementChicken].id = "Chicken";
            ach[AchievementExplorer].id = "Explorer";

            ach[AchievementRoasted].maxValue = 50;
            ach[AchievementEatThat].maxValue = 300;
        }

        ~Impl()
        {
        }

        void unlockAchievement(Achievement a)
        {
            if (!statsValid || ach[a].reached) {
                return;
            }

            ach[a].reached = true;
            ach[a].imageId = 0;
            ach[a].image = Image();

            SteamUserStats()->SetAchievement(ach[a].id.c_str());

            needStoreStats = true;
            storeStatsTimeout = 0.0f;
        }

        void evaluateAchievement(Achievement a)
        {
            switch (a) {
            case AchievementRoasted:
                if (enemiesRoasted >= ach[a].maxValue) {
                    unlockAchievement(a);
                }
                break;
            case AchievementEatThat:
                if (enemiesKilledWithStuff >= ach[a].maxValue) {
                    unlockAchievement(a);
                }
                break;
            default:
                break;
            }
        }

        STEAM_CALLBACK(Impl, userStatsReceived, UserStatsReceived_t, userStatsReceivedFn)
        {
            if (pParam->m_nGameID != gameAppId) {
                return;
            }

            log4cplus::NDCContextCreator ndc("steam_api");

            if (pParam->m_eResult != k_EResultOK) {
                LOG4CPLUS_ERROR(logger(), "RequestCurrentStats failed: " << pParam->m_eResult);
                return;
            }

            if (statsRequested) {
                LOG4CPLUS_DEBUG(logger(), "RequestCurrentStats succeeded");
            }

            for (int i = 0; i <= AchievementMax; ++i) {
                if (!SteamUserStats()->GetAchievement(ach[i].id.c_str(), &ach[i].reached)) {
                    LOG4CPLUS_ERROR(logger(), "Cannot get \"" << ach[i].id << "\" achievement status");
                } else {
                    ach[i].name = SteamUserStats()->GetAchievementDisplayAttribute(ach[i].id.c_str(), "name");
                    ach[i].description = SteamUserStats()->GetAchievementDisplayAttribute(ach[i].id.c_str(), "desc");
                    SteamUserStats()->GetAchievementIcon(ach[i].id.c_str());
                }
            }

            SteamUserStats()->GetStat("EnemiesRoasted", &enemiesRoasted);
            SteamUserStats()->GetStat("EnemiesKilledWithStuff", &enemiesKilledWithStuff);

            if (statsRequested) {
                statsValid = true;

                for (int i = 0; i <= AchievementMax; ++i) {
                    evaluateAchievement(static_cast<Achievement>(i));
                }
            }
        }

        STEAM_CALLBACK(Impl, userStatsStored, UserStatsStored_t, userStatsStoredFn)
        {
            if (pParam->m_nGameID != gameAppId) {
                return;
            }

            log4cplus::NDCContextCreator ndc("steam_api");

            if (pParam->m_eResult == k_EResultOK) {
                LOG4CPLUS_DEBUG(logger(), "StoreStats succeeded");
            } else if (pParam->m_eResult == k_EResultInvalidParam) {
                LOG4CPLUS_ERROR(logger(), "StoreStats failed: validation error");

                UserStatsReceived_t tmp;
                tmp.m_eResult = k_EResultOK;
                tmp.m_nGameID = gameAppId;
                userStatsReceived(&tmp);
            } else {
                LOG4CPLUS_ERROR(logger(), "StoreStats failed: " << pParam->m_eResult);
            }
        }

        STEAM_CALLBACK(Impl, userAchievementStored, UserAchievementStored_t, userAchievementStoredFn)
        {
            if (pParam->m_nGameID != gameAppId) {
                return;
            }

            log4cplus::NDCContextCreator ndc("steam_api");

            if (pParam->m_nMaxProgress == 0) {
                LOG4CPLUS_DEBUG(logger(), "Achievement \"" << pParam->m_rgchAchievementName << "\" unlocked");
            } else {
                LOG4CPLUS_DEBUG(logger(), "Achievement \"" << pParam->m_rgchAchievementName << "\" progress("
                    << pParam->m_nCurProgress << ", "
                    << pParam->m_nMaxProgress << ")");
            }
        }

        UInt64 lastTimeUs;

        bool statsValid;
        bool statsRequested;
        AchievementInfo ach[AchievementMax + 1];
        bool needStoreStats;
        float storeStatsTimeout;

        int enemiesRoasted;
        int enemiesKilledWithStuff;

        Language language;
    };

    SteamShell::SteamShell()
    : impl_(new Impl())
    {
    }

    SteamShell::~SteamShell()
    {
        delete impl_;
    }

    bool SteamShell::init()
    {
        log4cplus::NDCContextCreator ndc("steam_api");

        LOG4CPLUS_INFO(logger(), "Initializing...");

        if (SteamAPI_RestartAppIfNecessary(gameAppId)) {
            LOG4CPLUS_ERROR(logger(), "Steam is not running, starting Steam and re-launching the game.");
            return false;
        }

        if (!SteamAPI_Init()) {
            LOG4CPLUS_ERROR(logger(), "Steam must be running to play this game (SteamAPI_Init() failed).");
            return false;
        }

        SteamClient()->SetWarningMessageHook(&SteamWarningMessageHook);

        SteamUtils()->SetOverlayNotificationPosition(k_EPositionBottomRight);

        LOG4CPLUS_INFO(logger(), "Steam game language: " << SteamApps()->GetCurrentGameLanguage());

        if (SteamApps()->GetCurrentGameLanguage() == std::string("russian")) {
            impl_->language = LanguageRussian;
        }

        LOG4CPLUS_INFO(logger(), "Initialized");

        if (settings.steamResetStats) {
            SteamUserStats()->ResetAllStats(true);
        }

        UserStatsReceived_t tmp;
        tmp.m_eResult = k_EResultOK;
        tmp.m_nGameID = gameAppId;
        impl_->userStatsReceived(&tmp);

        impl_->statsRequested = true;

        SteamUserStats()->RequestCurrentStats();

        return true;
    }

    void SteamShell::shutdown()
    {
        log4cplus::NDCContextCreator ndc("steam_api");

        LOG4CPLUS_INFO(logger(), "Shutting down...");

        for (int i = 0; i <= AchievementMax; ++i) {
            impl_->ach[i].image = Image();
        }

        SteamAPI_Shutdown();

        LOG4CPLUS_INFO(logger(), "Shut down");
    }

    void SteamShell::update()
    {
        SteamAPI_RunCallbacks();

        UInt64 timeUs = getTimeUs();
        UInt32 deltaUs = 0;

        if (impl_->lastTimeUs != 0) {
            deltaUs = static_cast<UInt32>(timeUs - impl_->lastTimeUs);
        }

        impl_->lastTimeUs = timeUs;

        float dt = static_cast<float>(deltaUs) / 1000000.0f;

        if (impl_->needStoreStats) {
            impl_->storeStatsTimeout -= dt;

            if (impl_->storeStatsTimeout <= 0.0f) {
                log4cplus::NDCContextCreator ndc("steam_api");

                if (SteamUserStats()->StoreStats()) {
                    LOG4CPLUS_DEBUG(logger(), "StoreStats called");
                } else {
                    LOG4CPLUS_ERROR(logger(), "StoreStats call failed");
                }

                impl_->needStoreStats = false;
            }
        }
    }

    void SteamShell::reload()
    {
        for (int i = 0; i <= AchievementMax; ++i) {
            impl_->ach[i].imageId = 0;
            impl_->ach[i].image = Image();
        }
    }

    Image SteamShell::getAchievementImage(Achievement ach) const
    {
        if (impl_->ach[ach].imageId == 0) {
            log4cplus::NDCContextCreator ndc("steam_api");

            impl_->ach[ach].imageId = SteamUserStats()->GetAchievementIcon(impl_->ach[ach].id.c_str());

            if (impl_->ach[ach].imageId == 0) {
                return assetManager.getImage(impl_->ach[ach].reached ? "ui1/achievement_active.png" : "ui1/achievement_inactive.png",
                    Texture::WrapModeClamp, Texture::WrapModeClamp);
            }

            uint32 width, height;

            SteamUtils()->GetImageSize(impl_->ach[ach].imageId, &width, &height);

            if ((width == 0) || (height == 0)) {
                impl_->ach[ach].image = assetManager.getImage(impl_->ach[ach].reached ? "ui1/achievement_active.png" : "ui1/achievement_inactive.png",
                    Texture::WrapModeClamp, Texture::WrapModeClamp);

                return impl_->ach[ach].image;
            }

            std::vector<Byte> data(width * height * 4);

            SteamUtils()->GetImageRGBA(impl_->ach[ach].imageId, (uint8*)&data[0], data.size());

            TexturePtr tex = textureManager.genTexture(&data[0], width, height,
                Texture::WrapModeClamp, Texture::WrapModeClamp);

            impl_->ach[ach].image = Image(tex, 0, 0, tex->width(), tex->height());
        }

        return impl_->ach[ach].image;
    }

    std::string SteamShell::getAchievementName(Achievement ach) const
    {
        return impl_->ach[ach].name.empty() ? impl_->ach[ach].id : impl_->ach[ach].name;
    }

    std::string SteamShell::getAchievementDescription(Achievement ach) const
    {
        return impl_->ach[ach].description;
    }

    bool SteamShell::isAchieved(Achievement ach) const
    {
        return impl_->ach[ach].reached;
    }

    void SteamShell::setAchieved(Achievement ach)
    {
        switch (ach) {
        case AchievementRoasted:
        case AchievementEatThat:
            assert(0);
            return;
        default:
            impl_->unlockAchievement(ach);
            break;
        }
    }

    std::pair<int, int> SteamShell::getAchievementProgress(Achievement ach) const
    {
        switch (ach) {
        case AchievementRoasted:
            return std::make_pair(impl_->enemiesRoasted, impl_->ach[ach].maxValue);
        case AchievementEatThat:
            return std::make_pair(impl_->enemiesKilledWithStuff, impl_->ach[ach].maxValue);
        default:
            return std::make_pair(0, 0);
        }
    }

    void SteamShell::incAchievementProgress(Achievement ach, int num)
    {
        if (!impl_->statsValid) {
            return;
        }

        switch (ach) {
        case AchievementRoasted:
            impl_->enemiesRoasted += num;
            LOG4CPLUS_TRACE(logger(), "enemiesRoasted = " << impl_->enemiesRoasted);
            SteamUserStats()->SetStat("EnemiesRoasted", impl_->enemiesRoasted);
            impl_->evaluateAchievement(ach);
            break;
        case AchievementEatThat:
            impl_->enemiesKilledWithStuff += num;
            LOG4CPLUS_TRACE(logger(), "enemiesKilledWithStuff = " << impl_->enemiesKilledWithStuff);
            SteamUserStats()->SetStat("EnemiesKilledWithStuff", impl_->enemiesKilledWithStuff);
            impl_->evaluateAchievement(ach);
            break;
        default:
            assert(0);
            return;
        }

        if (!impl_->needStoreStats) {
            impl_->needStoreStats = true;
            impl_->storeStatsTimeout = 5.0f;
        }
    }

    Language SteamShell::getLanguage() const
    {
        return impl_->language;
    }
}
