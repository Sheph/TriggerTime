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

#ifndef _GRAVITYGUNAIMCOMPONENT_H_
#define _GRAVITYGUNAIMCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"
#include "RenderDottedLineComponent.h"
#include "RenderDottedCircleComponent.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class GravityGunAimComponent : public boost::enable_shared_from_this<GravityGunAimComponent>,
                                   public PhasedComponent
    {
    public:
        explicit GravityGunAimComponent(const b2Vec2& pos);
        ~GravityGunAimComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const SceneObjectPtr& target() const { return target_; }

        inline const b2Vec2& hitPoint() const { return hitPoint_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        float rayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, b2Vec2& finalPoint,
            SceneObject*& obj);

        b2Vec2 p1_;
        b2Vec2 p2_;

        RenderDottedLineComponentPtr aimLine_;
        RenderDottedCircleComponentPtr aimCircle_;
        SceneObjectPtr target_;
        b2Vec2 hitPoint_;

        AudioSourcePtr sndAim_;
        AudioSourcePtr sndLocked_;
    };

    typedef boost::shared_ptr<GravityGunAimComponent> GravityGunAimComponentPtr;
}

#endif
