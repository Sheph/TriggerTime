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

#include "PhysicsBodyComponent.h"
#include "SceneObject.h"

namespace af
{
    PhysicsBodyComponent::PhysicsBodyComponent(const RUBEBodyPtr& rubeBody)
    : rubeBody_(rubeBody),
      haveFilterOverride_(false)
    {
    }

    PhysicsBodyComponent::~PhysicsBodyComponent()
    {
    }

    void PhysicsBodyComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhysicsComponent(shared_from_this());
    }

    void PhysicsBodyComponent::onFreeze()
    {
        if (parent()->freezePhysics()) {
            parent()->body()->SetActive(false);
        }
    }

    void PhysicsBodyComponent::onThaw()
    {
        if (parent()->freezePhysics()) {
            parent()->body()->SetActive(parent()->actualActive());
        }
    }

    void PhysicsBodyComponent::setFilterOverride(const b2Filter& value)
    {
        filterOverride_ = value;
        haveFilterOverride_ = true;
    }

    b2AABB PhysicsBodyComponent::computeAABB(const b2Transform &t) const
    {
        return rubeBody_->computeAABB(t);
    }

    void PhysicsBodyComponent::enableFixture(const std::string& fixtureName)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            RUBEFixture* rf = static_cast<RUBEFixture*>(f->GetUserData());
            if (fixtureName == rf->name()) {
                if (haveFilterOverride_) {
                    f->SetFilterData(filterOverride_);
                } else {
                    f->SetFilterData(rf->fixtureDef().filter);
                }
            }
        }
    }

    void PhysicsBodyComponent::disableFixture(const std::string& fixtureName)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            RUBEFixture* rf = static_cast<RUBEFixture*>(f->GetUserData());
            if (fixtureName == rf->name()) {
               /*
                * It would be great to just call 'DestroyFixture' here, however, this is
                * dangerous because the fixture might be used somewhere in the engine. e.g. collision
                * manager could have queued this fixture for report to collision component and once
                * that collision component fires it would see garbage and the game will crash. The
                * only choice we have is somehow disabling the fixture, i.e. 0'ing out its collision filter.
                * Once again, note! Do not destroy fixtures anywhere in the engine!
                */
                b2Filter filter = f->GetFilterData();

                filter.categoryBits = 0;
                filter.maskBits = 0;
                filter.groupIndex = 0;

                f->SetFilterData(filter);
            }
        }
    }

    void PhysicsBodyComponent::enableAllFixtures()
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            RUBEFixture* rf = static_cast<RUBEFixture*>(f->GetUserData());
            if (haveFilterOverride_) {
                f->SetFilterData(filterOverride_);
            } else {
                f->SetFilterData(rf->fixtureDef().filter);
            }
        }
    }

    void PhysicsBodyComponent::disableAllFixtures()
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            b2Filter filter = f->GetFilterData();

            filter.categoryBits = 0;
            filter.maskBits = 0;
            filter.groupIndex = 0;

            f->SetFilterData(filter);
        }
    }

    int PhysicsBodyComponent::filterGroupIndex() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->GetFilterData().groupIndex;
    }

    void PhysicsBodyComponent::setFilterGroupIndex(int groupIndex)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            b2Filter filter = f->GetFilterData();

            filter.groupIndex = groupIndex;

            f->SetFilterData(filter);
        }
    }

    UInt32 PhysicsBodyComponent::filterCategoryBits() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->GetFilterData().categoryBits;
    }

    void PhysicsBodyComponent::setFilterCategoryBits(UInt32 categoryBits)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            b2Filter filter = f->GetFilterData();

            filter.categoryBits = categoryBits;

            f->SetFilterData(filter);
        }
    }

    UInt32 PhysicsBodyComponent::filterMaskBits() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->GetFilterData().maskBits;
    }

    void PhysicsBodyComponent::setFilterMaskBits(UInt32 maskBits)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            b2Filter filter = f->GetFilterData();

            filter.maskBits = maskBits;

            f->SetFilterData(filter);
        }
    }

    float PhysicsBodyComponent::density() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->GetDensity();
    }

    void PhysicsBodyComponent::setDensity(float density)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            f->SetDensity(density);
        }
    }

    b2Filter PhysicsBodyComponent::getFilter() const
    {
        assert(parent()->body());

        for (const b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            return f->GetFilterData();
        }

        return b2Filter();
    }

    void PhysicsBodyComponent::setFilter(const b2Filter& value)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            f->SetFilterData(value);
        }
    }

    void PhysicsBodyComponent::restoreFilter()
    {
        enableAllFixtures();
    }

    bool PhysicsBodyComponent::sensor() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->IsSensor();
    }

    void PhysicsBodyComponent::setSensor(bool value)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            f->SetSensor(value);
        }
    }

    void PhysicsBodyComponent::refilter()
    {
        if (parent()->body()) {
            for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
                f->Refilter();
            }
        }
    }

    float PhysicsBodyComponent::friction() const
    {
        assert(parent()->body());

        return parent()->body()->GetFixtureList()->GetFriction();
    }

    void PhysicsBodyComponent::setFriction(float friction)
    {
        assert(parent()->body());

        for (b2Fixture* f = parent()->body()->GetFixtureList(); f; f = f->GetNext()) {
            f->SetFriction(friction);
        }
    }

    void PhysicsBodyComponent::onRegister()
    {
        b2BodyDef bodyDef = parent()->bodyDef();

        b2Body* b = manager()->world().CreateBody(&bodyDef);

        for (int i = 0; i < rubeBody_->numFixtures(); ++i) {
            b2Fixture* f;
            if (haveFilterOverride_) {
                b2FixtureDef def = rubeBody_->fixture(i)->fixtureDef();
                def.filter = filterOverride_;
                f = b->CreateFixture(&def);
            } else {
                f = b->CreateFixture(&rubeBody_->fixture(i)->fixtureDef());
            }
            f->SetUserData(rubeBody_->fixture(i).get());
        }

        parent()->setBody(b);
    }

    void PhysicsBodyComponent::onUnregister()
    {
        parent()->setActive(false);
        parent()->body()->SetActive(false);
    }
}
