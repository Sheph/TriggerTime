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

#ifndef _INVENTORY_H_
#define _INVENTORY_H_

#include "Image.h"
#include "af/Types.h"
#include "af/EnumTraits.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    enum InventoryItem
    {
        InventoryItemDynomite = 0,
        InventoryItemRedKey = 1,
        InventoryItemBlueKey = 2,
        InventoryItemYellowKey = 3,
    };

    static const int InventoryItemMax = InventoryItemYellowKey;

    struct InventoryState
    {
        InventoryState()
        {
            memset(&items[0], 0, sizeof(items));
        }

        inline bool operator==(const InventoryState& rhs) const
        {
            return (::memcmp(items, rhs.items, sizeof(rhs.items)) == 0);
        }

        inline bool operator!=(const InventoryState& rhs) const
        {
            return !(*this == rhs);
        }

        int items[InventoryItemMax + 1];
    };

    class Inventory : boost::noncopyable
    {
    public:
        Inventory();
        ~Inventory();

        static bool allowMultiple(InventoryItem item);

        static const std::string& getImageName(InventoryItem item);

        static Image getImage(InventoryItem item);

        static const std::string& getAssetPath(InventoryItem item);

        static const std::string& getPlaceholderImageName(InventoryItem item);

        static Image getPlaceholderImage(InventoryItem item);

        bool have(InventoryItem item) const;
        int count(InventoryItem item) const;

        void give(InventoryItem item);
        void take(InventoryItem item);

        inline const InventoryState& state() const { return state_; }

    private:
        InventoryState state_;
    };

    typedef boost::shared_ptr<Inventory> InventoryPtr;

    AF_ENUMTRAITS(InventoryItem);
}

#endif
