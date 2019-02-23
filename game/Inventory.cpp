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

#include "Inventory.h"
#include "AssetManager.h"

namespace af
{
    static const int InventoryItemIntValues[] =
    {
        InventoryItemDynomite,
        InventoryItemRedKey,
        InventoryItemBlueKey,
        InventoryItemYellowKey,
    };

    static const char* InventoryItemStrValues[] =
    {
        "dynomite",
        "redKey",
        "blueKey",
        "yellowKey",
    };

    AF_ENUMTRAITS_IMPL(InventoryItem, InventoryItem);

    static const struct
    {
        std::string imageName;
        std::string placeholderImageName;
        std::string assetPath;
        bool multiple;
    } itemData[InventoryItemMax + 1] = {
        { "common2/dynomite.png", "common2/placeholder_dynomite.png", "powerup_dynomite.json", true },
        { "common2/key_red.png", "common2/placeholder_key_red.png", "powerup_key_red.json", false },
        { "common2/key_blue.png", "common2/placeholder_key_blue.png", "powerup_key_blue.json", false },
        { "common2/key_yellow.png", "common2/placeholder_key_yellow.png", "powerup_key_yellow.json", false },
    };

    Inventory::Inventory()
    {
    }

    Inventory::~Inventory()
    {
    }

    bool Inventory::allowMultiple(InventoryItem item)
    {
        return itemData[item].multiple;
    }

    const std::string& Inventory::getImageName(InventoryItem item)
    {
        return itemData[item].imageName;
    }

    Image Inventory::getImage(InventoryItem item)
    {
        return assetManager.getImage(getImageName(item));
    }

    const std::string& Inventory::getPlaceholderImageName(InventoryItem item)
    {
        return itemData[item].placeholderImageName;
    }

    Image Inventory::getPlaceholderImage(InventoryItem item)
    {
        return assetManager.getImage(getPlaceholderImageName(item));
    }

    const std::string& Inventory::getAssetPath(InventoryItem item)
    {
        return itemData[item].assetPath;
    }

    bool Inventory::have(InventoryItem item) const
    {
        return count(item) > 0;
    }

    int Inventory::count(InventoryItem item) const
    {
        return state_.items[item];
    }

    void Inventory::give(InventoryItem item)
    {
        if (have(item) && !allowMultiple(item)) {
            return;
        }

        ++state_.items[item];
    }

    void Inventory::take(InventoryItem item)
    {
        if (!have(item)) {
            return;
        }

        --state_.items[item];
    }
}
