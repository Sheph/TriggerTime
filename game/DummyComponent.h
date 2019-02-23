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

#ifndef _DUMMYCOMPONENT_H_
#define _DUMMYCOMPONENT_H_

#include "PhasedComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class DummyComponent : public boost::enable_shared_from_this<DummyComponent>,
                           public PhasedComponent
    {
    public:
        DummyComponent(const std::string& name, const b2Vec2& pos, float angle,
            float width, float length, int zOrder);
        ~DummyComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        inline const std::string& name() const { return name_; }
        inline const b2Vec2& pos() const { return xf_.p; }
        inline float angle() const { return angle_; }
        inline const b2Transform& transform() const { return xf_; }
        inline int zOrder() const { return zOrder_; }
        inline float width() const { return width_; }
        inline float length() const { return length_; }

        b2Vec2 worldPos() const;
        b2Transform worldTransform() const;

    private:
        virtual void onRegister();

        virtual void onUnregister();

        std::string name_;
        b2Transform xf_;
        float angle_;
        int zOrder_;
        float width_;
        float length_;
    };

    typedef boost::shared_ptr<DummyComponent> DummyComponentPtr;
}

#endif
