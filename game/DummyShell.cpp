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

#include "DummyShell.h"
#include "AssetManager.h"

namespace af
{
    DummyShell::DummyShell()
    {
    }

    DummyShell::~DummyShell()
    {
    }

    void DummyShell::update()
    {
    }

    void DummyShell::reload()
    {
    }

    Image DummyShell::getAchievementImage(Achievement ach) const
    {
        return assetManager.getImage("ui1/achievement_inactive.png",
            Texture::WrapModeClamp, Texture::WrapModeClamp);
    }

    std::string DummyShell::getAchievementName(Achievement ach) const
    {
        return "Unknown";
    }

    std::string DummyShell::getAchievementDescription(Achievement ach) const
    {
        return "";
    }

    bool DummyShell::isAchieved(Achievement ach) const
    {
        return false;
    }

    void DummyShell::setAchieved(Achievement ach)
    {
    }

    std::pair<int, int> DummyShell::getAchievementProgress(Achievement ach) const
    {
        return std::make_pair(0, 0);
    }

    void DummyShell::incAchievementProgress(Achievement ach, int num)
    {
    }

    Language DummyShell::getLanguage() const
    {
        return LanguageEnglish;
    }
}
