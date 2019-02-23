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

#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

#include "InputContextPlayer.h"
#include "InputContextUI.h"
#include "InputContextCutscene.h"
#include "InputContextRoboArm.h"
#include "InputContextBoat.h"
#include "InputContextMech.h"
#include "InputContextTurret.h"
#include <boost/shared_ptr.hpp>

namespace af
{
    class Scene;

    class InputController : boost::noncopyable
    {
    public:
        explicit InputController();
        virtual ~InputController();

        void update(float dt);

        bool pausePressed() const;

        bool zoomInPressed() const;

        bool zoomOutPressed() const;

        bool zoomResetPressed() const;

        inline bool gameDebugPressed() const { return gameDebugPressed_; }

        inline bool physicsDebugPressed() const { return physicsDebugPressed_; }

        inline bool slowmoPressed() const { return slowmoPressed_; }

        inline bool cullPressed() const { return cullPressed_; }

        virtual InputContextUI* menuUI() = 0;
        virtual InputContextUI* gameUI() = 0;
        virtual InputContextPlayer* player() = 0;
        virtual InputContextCutscene* cutscene() = 0;
        virtual InputContextRoboArm* roboArm() = 0;
        virtual InputContextBoat* boat() = 0;
        virtual InputContextMech* mech() = 0;
        virtual InputContextTurret* turret() = 0;

        virtual const InputContextUI* menuUI() const = 0;
        virtual const InputContextUI* gameUI() const = 0;
        virtual const InputContextPlayer* player() const = 0;
        virtual const InputContextCutscene* cutscene() const = 0;
        virtual const InputContextRoboArm* roboArm() const = 0;
        virtual const InputContextBoat* boat() const = 0;
        virtual const InputContextMech* mech() const = 0;
        virtual const InputContextTurret* turret() const = 0;

    private:
        bool updateContext(InputContext* c, float dt);

        bool gameDebugPressed_;
        bool physicsDebugPressed_;
        bool slowmoPressed_;
        bool cullPressed_;
    };

    typedef boost::shared_ptr<InputController> InputControllerPtr;
}

#endif
