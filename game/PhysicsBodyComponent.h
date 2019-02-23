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

#ifndef _PHYSICSBODYCOMPONENT_H_
#define _PHYSICSBODYCOMPONENT_H_

#include "af/RUBEBody.h"
#include "PhysicsComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class PhysicsBodyComponent : public boost::enable_shared_from_this<PhysicsBodyComponent>,
                                 public PhysicsComponent
    {
    public:
        explicit PhysicsBodyComponent(const RUBEBodyPtr& rubeBody);
        ~PhysicsBodyComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void onFreeze();

        virtual void onThaw();

        void setFilterOverride(const b2Filter& value);

        b2AABB computeAABB(const b2Transform &t) const;

        void enableFixture(const std::string& fixtureName);
        void disableFixture(const std::string& fixtureName);

        void enableAllFixtures();
        void disableAllFixtures();

        int filterGroupIndex() const;
        void setFilterGroupIndex(int groupIndex);

        UInt32 filterCategoryBits() const;
        void setFilterCategoryBits(UInt32 categoryBits);

        UInt32 filterMaskBits() const;
        void setFilterMaskBits(UInt32 maskBits);

        float density() const;
        void setDensity(float density);

        b2Filter getFilter() const;
        void setFilter(const b2Filter& value);
        void restoreFilter();

        bool sensor() const;
        void setSensor(bool value);

        void refilter();

        float friction() const;
        void setFriction(float friction);

        inline const RUBEBodyPtr& rubeBody() const { return rubeBody_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        RUBEBodyPtr rubeBody_;

        bool haveFilterOverride_;
        b2Filter filterOverride_;
    };

    typedef boost::shared_ptr<PhysicsBodyComponent> PhysicsBodyComponentPtr;
}

#endif
