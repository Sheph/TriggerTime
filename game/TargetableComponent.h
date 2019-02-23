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

#ifndef _TARGETABLECOMPONENT_H_
#define _TARGETABLECOMPONENT_H_

#include "PhasedComponent.h"
#include "SceneObject.h"

namespace af
{
    class TargetableComponent : public PhasedComponent
    {
    public:
        explicit TargetableComponent(UInt32 phases);
        virtual ~TargetableComponent();

        virtual void setAutoTarget(bool value) { autoTarget_ = value; }
        inline bool autoTarget() const { return autoTarget_; }

        virtual void setTarget(const SceneObjectPtr& value) { target_ = value; }
        inline const SceneObjectPtr& target() const { return target_; }

        inline bool patrol() const { return patrol_; }
        inline void setPatrol(bool value) { patrol_ = value; }

        inline bool followPlayer() const { return followPlayer_; }
        inline void setFollowPlayer(bool value) { followPlayer_ = value; }

        void updateAutoTarget(float dt);

    private:
        void updateAutoTargetEnemy(float dt);
        void updateAutoTargetAlly(float dt);

        bool autoTarget_;
        SceneObjectPtr target_;
        bool patrol_;
        float t_;
        bool followPlayer_;
    };

    typedef boost::shared_ptr<TargetableComponent> TargetableComponentPtr;
}

#endif
