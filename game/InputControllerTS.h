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

#ifndef _INPUTCONTROLLERTS_H_
#define _INPUTCONTROLLERTS_H_

#include "InputController.h"
#include "InputContextCutsceneTS.h"
#include "InputContextPlayerTS.h"
#include "InputContextUITS.h"
#include "InputContextRoboArmTS.h"
#include "InputContextBoatTS.h"
#include "InputContextMechTS.h"
#include "InputContextTurretTS.h"

namespace af
{
    class InputControllerTS : public InputController
    {
    public:
        explicit InputControllerTS(Scene* scene, Rocket::Core::Context* rc);
        ~InputControllerTS();

        virtual InputContextUI* menuUI() { return &menuUI_; }
        virtual InputContextUI* gameUI() { return &gameUI_; }
        virtual InputContextPlayer* player() { return &player_; }
        virtual InputContextCutscene* cutscene() { return &cutscene_; }
        virtual InputContextRoboArm* roboArm() { return &roboarm_; }
        virtual InputContextBoat* boat() { return &boat_; }
        virtual InputContextMech* mech() { return &mech_; }
        virtual InputContextTurret* turret() { return &turret_; }

        virtual const InputContextUI* menuUI() const { return &menuUI_; }
        virtual const InputContextUI* gameUI() const { return &gameUI_; }
        virtual const InputContextPlayer* player() const { return &player_; }
        virtual const InputContextCutscene* cutscene() const { return &cutscene_; }
        virtual const InputContextRoboArm* roboArm() const { return &roboarm_; }
        virtual const InputContextBoat* boat() const { return &boat_; }
        virtual const InputContextMech* mech() const { return &mech_; }
        virtual const InputContextTurret* turret() const { return &turret_; }

    private:
        InputContextUITS menuUI_;
        InputContextUITS gameUI_;
        InputContextPlayerTS player_;
        InputContextCutsceneTS cutscene_;
        InputContextRoboArmTS roboarm_;
        InputContextBoatTS boat_;
        InputContextMechTS mech_;
        InputContextTurretTS turret_;
    };
}

#endif
