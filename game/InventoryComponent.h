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

#ifndef _INVENTORYCOMPONENT_H_
#define _INVENTORYCOMPONENT_H_

#include "UIComponent.h"
#include "TextArea.h"
#include "Inventory.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class InventoryComponent : public boost::enable_shared_from_this<InventoryComponent>,
                               public UIComponent
    {
    public:
        InventoryComponent(const b2Vec2& pos, float height);
        ~InventoryComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

        inline float spacing() const { return spacing_; }
        inline void setSpacing(float value) { spacing_ = value; }

        inline float padding() const { return padding_; }
        inline void setPadding(float value) { padding_ = value; }

        inline const InventoryPtr& inventory() const { return inventory_; }
        inline void setInventory(const InventoryPtr& value) { inventory_ = value; }

    private:
        struct Item
        {
            Image image;
            TextAreaPtr text;
        };

        typedef std::vector<Item> Items;

        virtual void onRegister();

        virtual void onUnregister();

        void renderQuad(const Image& image, const b2Vec2& pos,
                        float width, float height);

        b2Vec2 pos_;
        float height_;
        Image frame_;
        float spacing_;
        float padding_;
        Items items_;
        InventoryPtr inventory_;
        InventoryState lastState_;
    };

    typedef boost::shared_ptr<InventoryComponent> InventoryComponentPtr;
}

#endif
