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

#ifndef _DAMAGEFLASHCOMPONENT_H_
#define _DAMAGEFLASHCOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"
#include "RenderComponent.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class DamageFlashComponent : public boost::enable_shared_from_this<DamageFlashComponent>,
                                 public PhasedComponent
    {
    public:
        DamageFlashComponent(const std::vector<RenderComponentPtr>& rcs);
        DamageFlashComponent(const RenderComponentPtr& rc);
        DamageFlashComponent(const RenderComponentPtr& rc1, const RenderComponentPtr& rc2);
        DamageFlashComponent(const RenderComponentPtr& rc1, const RenderComponentPtr& rc2, const RenderComponentPtr& rc3);
        ~DamageFlashComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        virtual void onFreeze();

    private:
        virtual void onRegister();

        virtual void onUnregister();

        Color color_;

        std::vector<RenderComponentPtr> rcs_;
        std::vector<int> oldZ_;

        float prevHealth_;
        float t_;
        float duration_;
    };

    typedef boost::shared_ptr<DamageFlashComponent> DamageFlashComponentPtr;
}

#endif
