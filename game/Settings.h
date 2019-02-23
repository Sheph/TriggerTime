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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "af/Single.h"
#include "af/AppConfig.h"
#include "Platform.h"
#include "UpgradeManager.h"
#include "Const.h"
#include <set>
#include <map>

namespace af
{
    class SkillSettings
    {
    public:
        struct Player
        {
            struct Blaster
            {
                float damage;
                float velocity;
                float interval;
            };

            struct Shotgun
            {
                float damage;
                float velocity;
                float spreadAngle;
                UInt32 numPellets;
                float interval;
            };

            struct SuperBlaster
            {
                float damage;
                float velocity;
                float interval;
            };

            struct RLauncher
            {
                float explosionImpulse;
                float explosionDamage;
                float velocity;
                float interval;
            };

            struct ProxMine
            {
                float armTimeout;
                float activationRadius;
                float activationTimeout;
                float explosionTimeout;
                float explosionImpulse;
                float explosionDamage;
                float interval;
            };

            struct Chainsaw
            {
                float damage;
            };

            struct MachineGun
            {
                float damage;
                float velocity;
                float spreadAngle;
                float interval;
            };

            struct PlasmaGun
            {
                float damage;
                float velocity;
                float spreadAngle;
                float interval;
            };

            struct LGun
            {
                float damage;
                float length;
                float impulse;
            };

            int numHearts;
            float maxLife;
            float maxStamina;
            float staminaLooseSpeed;
            float staminaRecoverySpeed;
            float staminaFastRecoverySpeed;
            float moveSpeed;
            float runSpeed;
            float damageImpulseThreshold;
            float chargedRotateSpeed;
            float walkLinearDamping;
            float jetpackLinearDamping;
            float angularDamping;
            float shieldDuration;
            float explosionImpulse;
            float explosionDamage;
            float ggAimWidth;
            float ggAimLength;
            float ggAimDotVelocity;
            float ggAimDotDistance;
            float ggAimRadiusPadding;
            float ggTimeThreshold;
            float ggHoldDistance;
            float ggHoldVelocity;
            float ggShootVelocity;
            float ggBreakLength;
            float ggBreakAngle;
            bool laserSight;
            Blaster blaster;
            Shotgun shotgun;
            SuperBlaster superBlaster;
            RLauncher rlauncher;
            ProxMine proxmine;
            Chainsaw chainsaw;
            MachineGun machineGun;
            PlasmaGun plasmaGun;
            LGun lgun;
        };

        struct ShroomGuardian
        {
            float life;
            float shootAngleDeg;
            float shootTimeThreshold;
            int shootCountThreshold;
            float shootPauseThreshold;
            float shootSpeed;
            float shootDamage;
            float jumpForwardDistance;
            float jumpBackDistance;
            float jumpVelocityThreshold;
            float jumpForwardVelocity;
            float jumpBackVelocity;
            float explosionImpulse;
            float explosionDamage;
            float turnSpeed;
        };

        struct ShroomGuardian2
        {
            float life;
            float turnSpeed;
        };

        struct ShroomCannon
        {
            float life;
            float shootTimeThreshold;
            float missileSpeed;
            float shootDamage;
        };

        struct PowerupHealth
        {
            float probability;
            float timeout;
            float radius;
            Color color;
            float amount;
        };

        struct PowerupGem
        {
            float probability;
            float timeout;
            float radius;
            Color color;
        };

        struct PowerupInventory
        {
            float radius;
            Color color;
        };

        struct PowerupHelmet
        {
            float radius;
            Color color;
        };

        struct PowerupBackpack
        {
            float radius;
            Color color;
        };

        struct PowerupAmmo
        {
            float radius;
            Color color;
            float proxmineAmount[1];
            float rlauncherAmount[1];
            float mgunAmount[1];
            float plasmagunAmount[1];
            float shotgunAmount[1];
            float eshieldAmount[1];
        };

        struct PowerupWeapon
        {
            float radius;
            Color color;
        };

        struct PowerupEarpiece
        {
            float radius;
            Color color;
        };

        struct GoalIndicator
        {
            float timeout;
            float alpha;
            float length;
        };

        struct PuzzleHint
        {
            float timeout;
            float alpha;
            float indicatorLength;
            float radius;
            float height;
            float displacement;
            Color color;
        };

        struct Placeholder
        {
            Color color;
        };

        struct Timebomb
        {
            Color color;
        };

        struct ShroomSnake
        {
            float life;
            float defAngle;
            std::vector<float> attackAngles;
        };

        struct Tetrobot
        {
            float life;
            float trackTime;
            float trackSpeed;
            float chargeTime;
            float shootTime;
            float shootDamage;
            float wallDistance;
            float speed;
            float laserLength;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Tetrobot2
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float shootMinTimeout;
            float shootMaxTimeout;
            float aimDuration;
            float shootDuration;
            float shootDamage;
            float shootLength;
            float shootImpulse;
            float explosionImpulse;
            float explosionDamage;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Tetrocrab
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float biteTime;
            float biteDamage;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Tetrocrab2
        {
            float life;
        };

        struct Tetrocrab3
        {
            float life;
            float turnSpeed;
            float walkSpeed;
        };

        struct Tetrocrab4
        {
            float life;
        };

        struct Scorp
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float attackRadius;
            float minRadius;
            float attackTime;
            float shootAngle;
            float shootVelocity;
            float shootDamage;
            float damageImpulseThreshold;
            float damageMultiplier;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Scorp2
        {
            float life;
        };

        struct Scorp3
        {
            float life;
            float walkSpeed;
        };

        struct Scorp4
        {
            float life;
        };

        struct Barrel1
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
            float explosionImpulse;
            float explosionDamage;
            float blastDamageMultiplier;
        };

        struct Barrel2
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Turret1
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
            float turnSpeed;
            float shootAngle;
            float shootDamage;
            float shootVelocity;
            float shootDelay;
            float blastDamageMultiplier;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Pyrobot
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float attackRadiusMin;
            float attackRadiusMax;
            float shootDamage;
        };

        struct BossSkull
        {
            float life;
            float slowWalkSpeed;
            float fastWalkSpeed;
            float gunFrontTurnSpeed;
            float gunFrontShootAngle;
            float gunFrontShootRadius;
            float gunFrontShootVelocity;
            float gunFrontShootDelay;
            float gunFrontShootDamage;
            float gunBackTurnSpeed;
            float gunBackShootAngle;
            float gunBackShootRadius;
            float gunBackShootVelocity;
            float gunBackShootDelay;
            float gunBackShootDamage;
            float gunMidTurnSpeed;
            float gunMidShootAngle;
            float gunMidShootRadius;
            float gunMidDamage;
            float blastDamageMultiplier;
        };

        struct Spider
        {
            float legVelocity;
            float attackVelocity;
            float attackTimeout;
            float attackDamage;
        };

        struct BabySpider
        {
            float turnSpeed;
            float biteTime;
            float fireDamageMultiplier;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct BabySpider1
        {
            float life;
            float walkSpeed;
            float biteDamage;
        };

        struct BabySpider2
        {
            float life;
            float walkSpeed;
            float biteDamage;
        };

        struct SpiderNest
        {
            float life;
            float radius;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Enforcer
        {
            float turnSpeed;
            float walkSpeed;
            float minRadius;
            float shootAngle;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Enforcer1
        {
            float life;
            float shootVelocity;
            float shootDamage;
            float shootInterval;
            Color color;
        };

        struct Enforcer2
        {
            float life;
            float shootVelocity;
            float shootDamage;
            float shootTurnInterval;
            float shootDelay;
            Color color;
        };

        struct Enforcer3
        {
            Color color;
        };

        struct Sentry
        {
            float torsoTurnSpeed;
            float legsTurnSpeed;
            float walkSpeed;
            float walkRadius;
            float standRadius;
            float shootRadius;
            float shootStartAngle;
            float shootStopAngle;
            float shootVelocity;
            float shootSpreadAngle;
            float shootTurnInterval;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Sentry1
        {
            float life;
            float shootDamage;
            float shootTurnDuration;
            float shootLoopDelay;
        };

        struct Sentry2
        {
            float life;
            float shootDamage;
            float shootTurnDuration;
            float shootLoopDelay;
        };

        struct Gorger
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float attackAngle;
            float attackDelayMin;
            float attackDelayMax;
            float meleeDamage;
            float shootExplosionImpulse;
            float shootExplosionDamage;
            float shootVelocity;
            float shootRepeatProbability;
            float explosionImpulse;
            float explosionDamage;
            float blastDamageMultiplier;
        };

        struct Crate1
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Orbo
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float attackRadius;
            float attackTimeout;
            float shootVelocity;
            float shootSpreadAngle;
            float shootTurnInterval;
            float shootTurnDuration;
            float shootDamage;
            float shootLoopDelay;
            int shootNumTurns;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Warder
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float meleeRadius;
            float meleeDamage[3];
            float shootTimeout;
            float shootDamage;
            float shootLength;
            float shootImpulse;
            float shootDuration;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Keeper
        {
            bool badImages;
            float life;
            float turnSpeed;
            float walkSpeed;
            float attackAngle;
            float attackDelayMin;
            float attackDelayMax;
            float meleeDamage;
            float gunAngle;
            float gunVelocity;
            float gunDamage;
            float gunInterval;
            float plasmaAngle;
            float plasmaVelocity;
            float plasmaDamage;
            float plasmaDuration;
            float missileImpulse;
            float missileDamage;
            float missileVelocity;
        };

        struct BossCoreProtector
        {
            float moveSpeed;
            float fastMoveSpeed;
            float shootMoveAngle;
            float shootMoveSpeed;
            float shootTimeout;
            float shootDamage;
            float shootLength;
            float shootImpulse;
            float shootMinDuration;
            float shootMaxDuration;
            float initVelocity;
            float aimVelocity;
            float strikeVelocity;
            float closeDistance;
            float farDistance;
            float meleeDamage;
            float meleeRollAngle;
        };

        struct RodBundle
        {
            float life;
        };

        struct Barrel3
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
            float toxicDamage;
            float toxicDamageTimeout;
        };

        struct AcidWorm
        {
            float life;
            float turnTorque;
            float idleForce;
            float idleSpeed;
            float spitTimeout;
            float spitForce1;
            float spitForce1Duration;
            float spitForce2;
            float spitForce2Duration;
            float shootAngle;
            float shootVelocity;
            float explosionTimeout;
            float explosionImpulse;
            float explosionDamage;
            float toxicDamage;
            float toxicDamageTimeout;
            float foldDelay;
            float foldForce;
            float unfoldDelay;
        };

        struct Centipede
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float biteTime;
            float biteDamage;
            float attackAngle;
            float attackTimeout;
            float spitDamage;
            UInt32 spitMinShots;
            UInt32 spitMaxShots;
            UInt32 spitTotalShots;
            float spitInterval;
            float spitVelocity;
            float spitAngle;
        };

        struct Beetle1
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float biteTime;
            float biteDamage;
            float attackAngle;
            float attackTimeout;
            float spitDamage;
            float spitVelocity;
            float spitAngle;
            UInt32 numSpawn;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Beetle2
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float explosionTimeout;
            float explosionImpulse;
            float explosionDamage;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct Boat
        {
            float turnSpeed;
            float forwardSpeed;
            float backSpeed;
            float gunTurnSpeed;
            float shootDamage;
            float shootLength;
            float shootImpulse;
        };

        struct PowerGen
        {
            float life;
        };

        struct BossSquid
        {
            float life;
            float eyeLife;
            float secondPhaseLifePercent;
            float attackTimeout;
            float attack1Damage;
            UInt32 attack1MinShots;
            UInt32 attack1MaxShots;
            UInt32 attack1TotalShots;
            float attack1Interval;
            float attack1Velocity;
            float attack1Angle;
            float attack2Damage;
            float attack2Velocity;
            float attack2Angle;
            float meleeDamage;
            float mouthOpenTime;
        };

        struct Mech
        {
            float life;
            float legsTurnSpeed;
            float torsoTurnSpeed;
            float walkSpeed;
            float attackDamage;
            float attackVelocity;
            float attackSpreadAngle;
            float attackInterval;
            float meleeImpulse;
            float meleeDamage;
        };

        struct Homer
        {
            float life;
            float turnSpeed;
            float walkSpeed;
            float closeRadius;
            float attackAngle;
            float attackLargeTimeout;
            float attackSmallTimeout;
            float gunAngle;
            float gunVelocity;
            float gunDamage;
            float gunInterval;
            float gunDuration;
            float missileImpulse;
            float missileDamage;
            float missileVelocity;
            float missileSeekVelocity;
            float meleeDamage;
            float explosionImpulse;
            float explosionDamage;
        };

        struct BossChopper
        {
            float life;
            float slowTurnSpeed;
            float fastTurnSpeed;
            float slowWalkSpeed;
            float fastWalkSpeed;
            float blastDamageMultiplier;
            float gunAngle;
            float gunVelocity;
            float gunDamage;
            float gunInterval;
            float gunDuration;
            float missileImpulse;
            float missileDamage;
            float missileVelocity;
            float missileSeekVelocity;
            float napalmDamage;
            float shootDelay;
        };

        struct Jar
        {
            float life;
        };

        struct Creature
        {
            float life;
            float damageImpulseThreshold;
            float damageMultiplier;
        };

        struct GasCloud1
        {
            float damage;
            float damageTimeout;
            float propagationDelay;
            float propagationFactor;
            float propagationMaxDelay;
        };

        struct Octopus
        {
            float life;
            float hitDamage;
            float biteTime;
            float biteDamage;
            float spitDamage;
            float spitVelocity;
            float rollTorque;
            float rollDuration;
            float chargeForce;
            float chargeDuration;
            float chargeDelay;
        };

        struct Walker
        {
            float life;
            float missileDelay;
            float missileImpulse;
            float missileDamage;
            float missileVelocity;
            float missileSeekVelocity;
            float spitDelay;
            float spitDamage;
            UInt32 spitMinShots;
            UInt32 spitMaxShots;
            UInt32 spitTotalShots;
            float spitInterval;
            float spitVelocity;
            float spitAngle;
        };

        struct Snake
        {
            float life;
            float missileVelocity;
            float missileExplosionTimeout;
            float missileExplosionImpulse;
            float missileExplosionDamage;
            float missileToxicDamage;
            float missileToxicDamageTimeout;
            float missileInterval;
            float gunDamage;
            float gunLength;
            float gunImpulse;
            float gunDuration;
            float shootDelay;
        };

        struct BossBuddy
        {
            float life;
            float attackMinTimeout;
            float attackMaxTimeout;
            int attackPreTongueNum;
            float seekerImpulse;
            float seekerDamage;
            float seekerVelocity;
            float seekerSeekVelocity;
            int seekerNum;
            float seekerInterval;
            float plasmaAngle;
            float plasmaVelocity;
            float plasmaDamage;
            float plasmaInterval;
            float plasmaDuration;
            float missileExplosionImpulse;
            float missileExplosionDamage;
            float missileVelocity;
            float missileInterval;
            float napalmDamage;
            float tongueAttackDuration;
            float tongueIdleDuration;
            float sideLife;
            float sideMeleeDamage;
            float sideDeadDuration;
            float tongueLife;
            float tongueAttackDamage;
            UInt32 tongueAttackMinShots;
            UInt32 tongueAttackMaxShots;
            UInt32 tongueAttackTotalShots;
            float tongueAttackInterval;
            float tongueAttackVelocity;
            float tongueAttackAngle;
        };

        struct BossNatan
        {
            float life;
            float attackMinTimeout;
            float attackMaxTimeout;
            float shootDuration;
            float shootDelay;
            float plasmaAngle;
            float plasmaVelocity;
            float plasmaDamage;
            float plasmaInterval;
            float spitAngle;
            float spitVelocity;
            float spitDamage;
            float spitInterval;
            float blasterVelocity;
            float blasterDamage;
            float blasterInterval;
            float ramDamage;
            float ramHitDamage;
            float meleeDamage;
            float syringeDamage;
            float napalmDamage;
            float napalmInterval;
            UInt32 napalmNum;
        };

        struct Heater
        {
            float life;
        };

        struct Guardian
        {
            float life;
            float jumpVelocity;
            float jumpTimeout;
            float shootMinTimeout;
            float shootMaxTimeout;
            float shootDamage;
            float shootVelocity;
            float turnSpeed;
            float explosionImpulse;
            float explosionDamage;
        };

        struct Guardian2
        {
            float life;
            float jumpVelocity;
            float jumpTimeout;
            float shootMinTimeout;
            float shootMaxTimeout;
        };

        struct Sawer
        {
            float turnSpeed;
            float throwForce1;
            float throwForce1Duration;
            float throwForce2;
            float throwForce2Duration;
            float attackMinTimeout;
            float attackMaxTimeout;
            float deployDelay;
            float deploySpeed;
            float fixedSawDamageTimeout;
            float fixedSawDamage;
            float fixedSawEnemyDamage;
            float fixedSawImpulse;
            float sawPickupDelay;
            float sawLife;
            float sawTurnSpeed;
            float sawWalkSpeed;
            float sawFastWalkSpeed;
            float sawDamage;
            float sawFastDamage;
            float sawPenetrationFactor;
        };

        struct HealthStation
        {
            float life;
        };

        struct BossQueen
        {
            float life;
            float attack1AimVelocity;
            float attack1StrikeVelocity;
            float attack1DownVelocity;
            float legDamage;
        };

        struct BossBeholder
        {
            float life;
            float verySlowLegVelocity;
            float slowLegVelocity;
            float fastLegVelocity;
            float shieldVelocity;
            float legDamage;
            float fireballVelocity;
            float fireballDamage;
            float laserChargeTime;
            float laserDamage;
            float laserImpulse;
            float napalmDamage;
            float napalmVelocity;
        };

        SkillSettings();
        ~SkillSettings();

        void skillInit(const AppConfigPtr& appConfig);

        Player player;
        ShroomGuardian shroomGuardian;
        ShroomGuardian2 shroomGuardian2;
        ShroomCannon shroomCannon;
        PowerupHealth powerupHealth;
        PowerupGem powerupGem;
        PowerupInventory powerupInventory;
        PowerupHelmet powerupHelmet;
        PowerupAmmo powerupAmmo;
        PowerupWeapon powerupWeapon;
        PowerupBackpack powerupBackpack;
        PowerupEarpiece powerupEarpiece;
        UInt32 upgradeCosts[UpgradeIdMax + 1];
        GoalIndicator goalIndicator;
        PuzzleHint puzzleHint;
        Placeholder placeholder;
        Timebomb timebomb;
        ShroomSnake shroomSnake;
        Tetrobot tetrobot;
        Tetrobot2 tetrobot2;
        Tetrocrab tetrocrab;
        Tetrocrab2 tetrocrab2;
        Tetrocrab3 tetrocrab3;
        Tetrocrab4 tetrocrab4;
        Scorp scorp;
        Scorp2 scorp2;
        Scorp3 scorp3;
        Scorp4 scorp4;
        Barrel1 barrel1;
        Barrel2 barrel2;
        Turret1 turret1;
        Pyrobot pyrobot;
        BossSkull bossSkull;
        Spider spider;
        BabySpider babySpider;
        BabySpider1 babySpider1;
        BabySpider2 babySpider2;
        SpiderNest spiderNest;
        Enforcer enforcer;
        Enforcer1 enforcer1;
        Enforcer2 enforcer2;
        Enforcer3 enforcer3;
        Sentry sentry;
        Sentry1 sentry1;
        Sentry2 sentry2;
        Gorger gorger;
        Crate1 crate1;
        Orbo orbo;
        Warder warder;
        Keeper keeper;
        BossCoreProtector bossCoreProtector;
        RodBundle rodBundle;
        Barrel3 barrel3;
        AcidWorm acidworm;
        Centipede centipede;
        Beetle1 beetle1;
        Beetle2 beetle2;
        Boat boat;
        PowerGen powerGen1;
        PowerGen powerGen2;
        BossSquid bossSquid;
        Mech mech;
        Homer homer;
        BossChopper bossChopper;
        Jar jar1;
        Jar jar2;
        Jar jar3;
        Jar jar4;
        std::vector<Creature> creature;
        GasCloud1 gasCloud1;
        Octopus octopus;
        Walker walker;
        Snake snake;
        BossBuddy bossBuddy;
        BossNatan bossNatan;
        Heater heater1;
        Guardian guardian;
        Guardian2 guardian2;
        Sawer sawer;
        HealthStation healthStation;
        BossQueen bossQueen;
        BossBeholder bossBeholder;
    };

    class Settings : public SkillSettings,
                     public Single<Settings>
    {
    public:
        struct ObjectPool
        {
            int maxBytes;
            int warnThreshold;
            UInt32 reportTimeoutMs;
        };

        struct TouchScreen
        {
            bool enabled;
            b2Vec2 movePadding;
            float moveRadius;
            float moveHandleRadius;
            b2Vec2 primaryPadding;
            float primaryRadius;
            float primaryHandleRadius;
            float primaryTextSize;
            b2Vec2 primaryTextPos;
            b2Vec2 secondaryPadding;
            float secondaryRadius;
            float secondaryHandleRadius;
            float secondaryTextSize;
            b2Vec2 secondaryTextPos;
            b2Vec2 switchPadding;
            float switchRadius;
            float slotRadius;
            b2Vec2 slotDir;
            b2Vec2 slotPrimaryPadding;
            b2Vec2 slotSecondaryPadding;
            float slotTextSize;
            b2Vec2 slotTextPos;
            float alpha;
            float switchAlpha;
        };

        struct PC
        {
            b2Vec2 primaryPadding;
            float primaryRadius;
            float primaryTextSize;
            b2Vec2 primaryTextPos;
            b2Vec2 secondaryPadding;
            float secondaryRadius;
            float secondaryTextSize;
            b2Vec2 secondaryTextPos;
            float alpha;
        };

        struct Physics
        {
            /*
             * Physics will step with this rate.
             */
            float fixedTimestep;

            /*
             * Maximum number of physics steps to avoid "spiral of death".
             */
            UInt32 maxSteps;

            bool debugShape;
            bool debugJoint;
            bool debugAABB;
            bool debugPair;
            bool debugCenterOfMass;
            float slowmoFactor;
        };

        struct Light
        {
            int numBlur;
            int lightmapScale;
            int numRays;
            Color ambient;
            bool gammaCorrection;
            float edgeSmooth;
        };

        struct Audio
        {
            bool enabled;
            int maxSources;
            int soundCacheSize;
            int numStreamBuffers;
            int streamBufferSize;
            std::map<std::string, float> factors;
        };

        struct LetterBox
        {
            float distance;
            float duration;
        };

        struct HUD
        {
            bool enabled;
        };

        struct Crosshair
        {
            bool enabled;
        };

        struct BlasterShot
        {
            Color player[4];
            Color ally[4];
            Color enemy[4];
        };

        struct Bullet
        {
            Color player[2];
            Color ally[2];
            Color enemy[2];
        };

        struct Dialog
        {
            float borderSize;
            float padding;
            float portraitSize;
            float titleCharSize;
            float messageCharSize;
            Color titlePlayerColor;
            Color titleAllyColor;
            Color titleEnemyColor;
            Color messageColor;
            Color bgColor;
            float letterTimeThreshold;
            float blinkTimeThreshold;
        };

        struct LevelCompleted
        {
            Color blackoutColor;
            float blackoutTime;
            float charSize;
            Color keyColor;
            Color valueColor;
            float itemTimeout;
            float buttonsTimeout;
        };

        struct GameOver
        {
            float timeout;
            Color blackoutColor;
            float blackoutTime;
            float buttonsTimeout;
        };

        struct HealthBar
        {
            float xOffset;
            float yOffset;
            float width;
            float height;
        };

        struct StaminaBar
        {
            float xOffset;
            float yOffset;
            float width;
            float height;
        };

        struct HermitePath
        {
            int numIterations;
        };

        struct Tentacle
        {
            int numIterations;
            float step;
        };

        Settings();
        ~Settings();

        void init(const AppConfigPtr& appConfig,
            const AppConfigPtr& easyAppConfig,
            const AppConfigPtr& hardAppConfig);

        inline void setDeveloper(UInt32 value) { developer = value; }

        inline void setGamma(float value) { gamma = value; }

        void setSkill(Skill value);

#ifdef _WIN32
        std::string assets;
#endif
        UInt32 developer;
        UInt32 viewWidth;
        UInt32 viewHeight;
        float viewAspect;
        float gameHeight;
        UInt32 layoutWidth;
        UInt32 layoutHeight;
        UInt32 profileReportTimeoutMs;
        bool debugPath;
        UInt32 minRenderDt;
        int videoMode;
        int msaaMode;
        bool vsync;
        bool fullscreen;
        bool trilinearFilter;
        UInt32 viewX;
        UInt32 viewY;
        float gamma;
        bool dumpAssetList;
        bool debugKeys;
        UInt32 atLeastGems;
        bool allLevelsAccessible;
        Skill skill;
        bool steamApiRequired;
        bool steamResetStats;
        Language language;
        std::set<VideoMode> winVideoModes;
        ObjectPool objectPool;
        TouchScreen touchScreen;
        PC pc;
        Physics physics;
        Light light;
        Audio audio;
        LetterBox letterBox;
        HUD hud;
        Crosshair crosshair;
        BlasterShot blasterShot;
        Bullet bullet;
        Dialog dialog;
        LevelCompleted levelCompleted;
        GameOver gameover;
        HealthBar healthBar;
        StaminaBar staminaBar;
        HermitePath hermitePath;
        Tentacle tentacle;
    };

    extern Settings settings;
}

#endif
