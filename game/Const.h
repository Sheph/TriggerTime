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

#ifndef _CONST_H_
#define _CONST_H_

#include "af/Types.h"

namespace af
{
    /*
     * zOrder values.
     * @{
     */

    /*
     * Background, parallax, etc.
     */
    static const int zOrderBackground = -200;

    /*
     * Back plan, i.e. shrooms, etc.
     */
    static const int zOrderBack = -100;

    /*
     * Main plan, i.e. player, enemies, etc.
     */
    static const int zOrderMain = 0;

    /*
     * Sparks, exhaust, etc.
     */
    static const int zOrderEffects = 50;

    /*
     * Explosions, etc.
     */
    static const int zOrderExplosion = 70;

    /*
     * Front terrain.
     */
    static const int zOrderTerrain = 90;

    /*
     * Front plan, i.e. grass, etc.
     */
    static const int zOrderFront = 100;

    /*
     * Lights.
     */
    static const int zOrderLight = 120;

    /*
     * Target lines, life boxes, etc.
     */
    static const int zOrderMarker = 150;

    /*
     * Overall effects such as damage screen, etc.
     */
    static const int zOrderTop = 200;

    /*
     * @}
     */

    /*
     * Collision bits.
     * @{
     */

    static const UInt32 collisionBitGeneral = 1U << 0;
    static const UInt32 collisionBitPlayer = 1U << 1;
    static const UInt32 collisionBitAlly = 1U << 2;
    static const UInt32 collisionBitRock = 1U << 3;
    static const UInt32 collisionBitEnemy = 1U << 4;
    static const UInt32 collisionBitEnemyBuilding = 1U << 5;
    static const UInt32 collisionBitGizmo = 1U << 6;
    static const UInt32 collisionBitCustom1 = 1U << 7;
    static const UInt32 collisionBitCustom2 = 1U << 8;
    static const UInt32 collisionBitGarbage = 1U << 9;
    static const UInt32 collisionBitVehicle = 1U << 10;
    static const UInt32 collisionBitEnemyRock = 1U << 11;

    /*
     * @}
     */

    /*
     * Collision groups.
     * @{
     */

    static const int collisionGroupMissile = -100;
    static const int collisionGroupCustom1 = -300;
    static const int collisionGroupCustom2 = -400;
    static const int collisionGroupSpider = -500;
    static const int collisionGroupRoboArm = -600;
    static const int collisionGroupTentacle = -700;
    static const int collisionGroupCustom3 = -800;
    static const int collisionGroupCustom4 = 300;
    static const int collisionGroupCustom5 = 400;

    /*
     * @}
     */

    enum Skill
    {
        SkillEasy = 0,
        SkillNormal = 1,
        SkillHard = 2
    };

    static const int SkillMax = SkillHard;

    enum Language
    {
        LanguageEnglish = 0,
        LanguageRussian = 1
    };

    static const int LanguageMax = LanguageRussian;
}

#endif
