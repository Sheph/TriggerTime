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

#ifndef _TENTACLESWINGCOMPONENT_H_
#define _TENTACLESWINGCOMPONENT_H_

#include "PhasedComponent.h"
#include "RenderTentacleComponent.h"
#include "SingleTweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class TentacleSwingComponent : public boost::enable_shared_from_this<TentacleSwingComponent>,
                                   public PhasedComponent
    {
    public:
        TentacleSwingComponent();
        TentacleSwingComponent(float t1, Easing easing1, float torque1,
            float t2, Easing easing2, float torque2);
        ~TentacleSwingComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        void setLoop(float t1, Easing easing1, float torque1,
            float t2, Easing easing2, float torque2);

        void setSingle(float torque1, float torque2, float t, Easing easing);

        inline int bone() const { return bone_; }
        inline void setBone(int value) { bone_ = value; }

        inline void setTweenTime(float value) { tweenTime_ = value; }

        inline bool pause() const { return pause_; }
        inline void setPause(bool value) { pause_ = value; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        RenderTentacleComponentPtr tentacle_;
        int bone_;

        TweeningPtr tweening_;
        float tweenTime_;
        bool pause_;
    };

    typedef boost::shared_ptr<TentacleSwingComponent> TentacleSwingComponentPtr;
}

#endif
