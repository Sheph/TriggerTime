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

#ifndef _TENTACLEINFLATECOMPONENT_H_
#define _TENTACLEINFLATECOMPONENT_H_

#include "PhasedComponent.h"
#include "RenderTentacleComponent.h"
#include "RenderQuadComponent.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class TentacleInflateComponent : public boost::enable_shared_from_this<TentacleInflateComponent>,
                                     public PhasedComponent
    {
    public:
        TentacleInflateComponent(int numPoints, float radius, float velocity,
            const RenderQuadComponentPtr& rc);
        ~TentacleInflateComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void preRender(float dt);

        inline bool done() const { return done_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        int numPoints_;
        float radius_;
        float velocity_;

        RenderTentacleComponentPtr tentacle_;
        RenderQuadComponentPtr rc_;

        float width_;
        float rcOrigHeight_;
        b2Vec2 rcOrigPos_;
        bool done_;
    };

    typedef boost::shared_ptr<TentacleInflateComponent> TentacleInflateComponentPtr;
}

#endif
