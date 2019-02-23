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

#include "Settings.h"
#include "Utils.h"
#include "Logger.h"
#include <cmath>

namespace af
{
    SkillSettings skillSettings[SkillMax + 1];

    Settings settings;

    template <>
    Single<Settings>* Single<Settings>::single = NULL;

    SkillSettings::SkillSettings()
    {
    }

    SkillSettings::~SkillSettings()
    {
    }

    void SkillSettings::skillInit(const AppConfigPtr& appConfig)
    {
        std::vector<std::string> subKeys;

        /*
         * player.
         */

        player.numHearts = appConfig->getInt("player.numHearts");
        player.maxLife = appConfig->getFloat("player.maxLife");
        player.maxStamina = appConfig->getFloat("player.maxStamina");
        player.staminaLooseSpeed = appConfig->getFloat("player.staminaLooseSpeed");
        player.staminaRecoverySpeed = appConfig->getFloat("player.staminaRecoverySpeed");
        player.staminaFastRecoverySpeed = appConfig->getFloat("player.staminaFastRecoverySpeed");
        player.moveSpeed = appConfig->getFloat("player.moveSpeed");
        player.runSpeed = appConfig->getFloat("player.runSpeed");
        player.damageImpulseThreshold = appConfig->getFloat("player.damageImpulseThreshold");
        player.chargedRotateSpeed = deg2rad(appConfig->getFloat("player.chargedRotateSpeed"));
        player.walkLinearDamping = appConfig->getFloat("player.walkLinearDamping");
        player.jetpackLinearDamping = appConfig->getFloat("player.jetpackLinearDamping");
        player.angularDamping = appConfig->getFloat("player.angularDamping");
        player.shieldDuration = appConfig->getFloat("player.shieldDuration");
        player.explosionImpulse = appConfig->getFloat("player.explosionImpulse");
        player.explosionDamage = appConfig->getFloat("player.explosionDamage");
        player.ggAimWidth = appConfig->getFloat("player.ggAimWidth");
        player.ggAimLength = appConfig->getFloat("player.ggAimLength");
        player.ggAimDotVelocity = appConfig->getFloat("player.ggAimDotVelocity");
        player.ggAimDotDistance = appConfig->getFloat("player.ggAimDotDistance");
        player.ggAimRadiusPadding = appConfig->getFloat("player.ggAimRadiusPadding");
        player.ggTimeThreshold = appConfig->getFloat("player.ggTimeThreshold");
        player.ggHoldDistance = appConfig->getFloat("player.ggHoldDistance");
        player.ggHoldVelocity = appConfig->getFloat("player.ggHoldVelocity");
        player.ggShootVelocity = appConfig->getFloat("player.ggShootVelocity");
        player.ggBreakLength = appConfig->getFloat("player.ggBreakLength");
        player.ggBreakAngle = deg2rad(appConfig->getFloat("player.ggBreakAngle"));
        player.laserSight = appConfig->getBool("player.laserSight");
        player.blaster.damage = appConfig->getFloat("player.blaster.damage");
        player.blaster.velocity = appConfig->getFloat("player.blaster.velocity");
        player.blaster.interval = appConfig->getFloat("player.blaster.interval");
        player.shotgun.damage = appConfig->getFloat("player.shotgun.damage");
        player.shotgun.velocity = appConfig->getFloat("player.shotgun.velocity");
        player.shotgun.spreadAngle = deg2rad(appConfig->getFloat("player.shotgun.spreadAngle"));
        player.shotgun.numPellets = appConfig->getInt("player.shotgun.numPellets");
        player.shotgun.interval = appConfig->getFloat("player.shotgun.interval");
        player.superBlaster.damage = appConfig->getFloat("player.superBlaster.damage");
        player.superBlaster.velocity = appConfig->getFloat("player.superBlaster.velocity");
        player.superBlaster.interval = appConfig->getFloat("player.superBlaster.interval");
        player.rlauncher.explosionImpulse = appConfig->getFloat("player.rlauncher.explosionImpulse");
        player.rlauncher.explosionDamage = appConfig->getFloat("player.rlauncher.explosionDamage");
        player.rlauncher.velocity = appConfig->getFloat("player.rlauncher.velocity");
        player.rlauncher.interval = appConfig->getFloat("player.rlauncher.interval");
        player.proxmine.armTimeout = appConfig->getFloat("player.proxmine.armTimeout");
        player.proxmine.activationRadius = appConfig->getFloat("player.proxmine.activationRadius");
        player.proxmine.activationTimeout = appConfig->getFloat("player.proxmine.activationTimeout");
        player.proxmine.explosionTimeout = appConfig->getFloat("player.proxmine.explosionTimeout");
        player.proxmine.explosionImpulse = appConfig->getFloat("player.proxmine.explosionImpulse");
        player.proxmine.explosionDamage = appConfig->getFloat("player.proxmine.explosionDamage");
        player.proxmine.interval = appConfig->getFloat("player.proxmine.interval");
        player.chainsaw.damage = appConfig->getFloat("player.chainsaw.damage");
        player.machineGun.damage = appConfig->getFloat("player.machineGun.damage");
        player.machineGun.velocity = appConfig->getFloat("player.machineGun.velocity");
        player.machineGun.spreadAngle = deg2rad(appConfig->getFloat("player.machineGun.spreadAngle"));
        player.machineGun.interval = appConfig->getFloat("player.machineGun.interval");
        player.plasmaGun.damage = appConfig->getFloat("player.plasmaGun.damage");
        player.plasmaGun.velocity = appConfig->getFloat("player.plasmaGun.velocity");
        player.plasmaGun.spreadAngle = deg2rad(appConfig->getFloat("player.plasmaGun.spreadAngle"));
        player.plasmaGun.interval = appConfig->getFloat("player.plasmaGun.interval");
        player.lgun.damage = appConfig->getFloat("player.lgun.damage");
        player.lgun.length = appConfig->getFloat("player.lgun.length");
        player.lgun.impulse = appConfig->getFloat("player.lgun.impulse");

        /*
         * shroom guardian.
         */

        shroomGuardian.life = appConfig->getFloat("shroom guardian.life");
        shroomGuardian.shootAngleDeg = appConfig->getFloat("shroom guardian.shootAngle");
        shroomGuardian.shootTimeThreshold = appConfig->getFloat("shroom guardian.shootTimeThreshold");
        shroomGuardian.shootCountThreshold = appConfig->getInt("shroom guardian.shootCountThreshold");
        shroomGuardian.shootPauseThreshold = appConfig->getFloat("shroom guardian.shootPauseThreshold");
        shroomGuardian.shootSpeed = appConfig->getFloat("shroom guardian.shootSpeed");
        shroomGuardian.shootDamage = appConfig->getFloat("shroom guardian.shootDamage");
        shroomGuardian.jumpForwardDistance = appConfig->getFloat("shroom guardian.jumpForwardDistance");
        shroomGuardian.jumpBackDistance = appConfig->getFloat("shroom guardian.jumpBackDistance");
        shroomGuardian.jumpVelocityThreshold = appConfig->getFloat("shroom guardian.jumpVelocityThreshold");
        shroomGuardian.jumpForwardVelocity = appConfig->getFloat("shroom guardian.jumpForwardVelocity");
        shroomGuardian.jumpBackVelocity = appConfig->getFloat("shroom guardian.jumpBackVelocity");
        shroomGuardian.explosionImpulse = appConfig->getFloat("shroom guardian.explosionImpulse");
        shroomGuardian.explosionDamage = appConfig->getFloat("shroom guardian.explosionDamage");
        shroomGuardian.turnSpeed = deg2rad(appConfig->getFloat("shroom guardian.turnSpeed"));

        /*
         * shroom guardian 2.
         */

        shroomGuardian2.life = appConfig->getFloat("shroom guardian 2.life");
        shroomGuardian2.turnSpeed = deg2rad(appConfig->getFloat("shroom guardian 2.turnSpeed"));

        /*
         * shroom cannon.
         */

        shroomCannon.life = appConfig->getFloat("shroom cannon.life");
        shroomCannon.shootTimeThreshold = appConfig->getFloat("shroom cannon.shootTimeThreshold");
        shroomCannon.missileSpeed = appConfig->getFloat("shroom cannon.missileSpeed");
        shroomCannon.shootDamage = appConfig->getFloat("shroom cannon.shootDamage");

        /*
         * powerup health.
         */

        powerupHealth.probability = appConfig->getFloat("health powerup.probability");
        powerupHealth.timeout = appConfig->getFloat("health powerup.timeout");
        powerupHealth.radius = appConfig->getFloat("health powerup.radius");
        powerupHealth.color = appConfig->getColor("health powerup.color");
        powerupHealth.amount = appConfig->getFloat("health powerup.amount");

        /*
         * powerup gem.
         */

        powerupGem.probability = appConfig->getFloat("gem powerup.probability");
        powerupGem.timeout = appConfig->getFloat("gem powerup.timeout");
        powerupGem.radius = appConfig->getFloat("gem powerup.radius");
        powerupGem.color = appConfig->getColor("gem powerup.color");

        /*
         * powerup inventory.
         */

        powerupInventory.radius = appConfig->getFloat("inventory powerup.radius");
        powerupInventory.color = appConfig->getColor("inventory powerup.color");

        /*
         * powerup helmet.
         */

        powerupHelmet.radius = appConfig->getFloat("helmet powerup.radius");
        powerupHelmet.color = appConfig->getColor("helmet powerup.color");

        /*
         * powerup ammo.
         */

        powerupAmmo.radius = appConfig->getFloat("ammo powerup.radius");
        powerupAmmo.color = appConfig->getColor("ammo powerup.color");
        powerupAmmo.proxmineAmount[0] = appConfig->getFloat("ammo powerup.proxmineAmount.0");
        powerupAmmo.rlauncherAmount[0] = appConfig->getFloat("ammo powerup.rlauncherAmount.0");
        powerupAmmo.mgunAmount[0] = appConfig->getFloat("ammo powerup.mgunAmount.0");
        powerupAmmo.plasmagunAmount[0] = appConfig->getFloat("ammo powerup.plasmagunAmount.0");
        powerupAmmo.shotgunAmount[0] = appConfig->getFloat("ammo powerup.shotgunAmount.0");
        powerupAmmo.eshieldAmount[0] = appConfig->getFloat("ammo powerup.eshieldAmount.0");

        /*
         * powerup weapon.
         */

        powerupWeapon.radius = appConfig->getFloat("weapon powerup.radius");
        powerupWeapon.color = appConfig->getColor("weapon powerup.color");

        /*
         * powerup backpack.
         */

        powerupBackpack.radius = appConfig->getFloat("backpack powerup.radius");
        powerupBackpack.color = appConfig->getColor("backpack powerup.color");

        /*
         * powerup earpiece.
         */

        powerupEarpiece.radius = appConfig->getFloat("earpiece powerup.radius");
        powerupEarpiece.color = appConfig->getColor("earpiece powerup.color");

        /*
         * upgrade.
         */

        upgradeCosts[UpgradeIdExtraLife] = appConfig->getInt("upgrade.extraLife.cost");
        upgradeCosts[UpgradeIdMoreAmmo] = appConfig->getInt("upgrade.moreAmmo.cost");
        upgradeCosts[UpgradeIdArmor] = appConfig->getInt("upgrade.armor.cost");
        upgradeCosts[UpgradeIdSuperBlaster] = appConfig->getInt("upgrade.superBlaster.cost");

        /*
         * goal indicator.
         */

        goalIndicator.timeout = appConfig->getFloat("goal indicator.timeout");
        goalIndicator.alpha = appConfig->getFloat("goal indicator.alpha");
        goalIndicator.length = appConfig->getFloat("goal indicator.length");

        /*
         * puzzle hint.
         */

        puzzleHint.timeout = appConfig->getFloat("puzzle hint.timeout");
        puzzleHint.alpha = appConfig->getFloat("puzzle hint.alpha");
        puzzleHint.indicatorLength = appConfig->getFloat("puzzle hint.indicatorLength");
        puzzleHint.radius = appConfig->getFloat("puzzle hint.radius");
        puzzleHint.height = appConfig->getFloat("puzzle hint.height");
        puzzleHint.displacement = appConfig->getFloat("puzzle hint.displacement");
        puzzleHint.color = appConfig->getColor("puzzle hint.color");

        /*
         * placeholder.
         */

        placeholder.color = appConfig->getColor("placeholder.color");

        /*
         * timebomb.
         */

        timebomb.color = appConfig->getColor("timebomb.color");

        /*
         * shroom snake.
         */

        shroomSnake.life = appConfig->getFloat("shroom snake.life");
        shroomSnake.defAngle = 0.0f;

        subKeys = appConfig->getSubKeys("shroom snake.angle");

        if (subKeys.empty()) {
            LOG4CPLUS_ERROR(logger(), "Config values for \"shroom snake.angle\" not found");
        }

        for (std::vector<std::string>::const_iterator it = subKeys.begin();
             it != subKeys.end();
             ++it) {
            float angle = deg2rad(
                appConfig->getFloat(
                    std::string("shroom snake.angle.") + *it));
            if (it == subKeys.begin()) {
                shroomSnake.defAngle = angle;
            } else {
                shroomSnake.attackAngles.push_back(angle);
            }
        }

        /*
         * tetrobot.
         */

        tetrobot.life = appConfig->getFloat("tetrobot.life");
        tetrobot.trackTime = appConfig->getFloat("tetrobot.trackTime");
        tetrobot.trackSpeed = deg2rad(appConfig->getFloat("tetrobot.trackSpeed"));
        tetrobot.chargeTime = appConfig->getFloat("tetrobot.chargeTime");
        tetrobot.shootTime = appConfig->getFloat("tetrobot.shootTime");
        tetrobot.shootDamage = appConfig->getFloat("tetrobot.shootDamage");
        tetrobot.wallDistance = appConfig->getFloat("tetrobot.wallDistance");
        tetrobot.speed = appConfig->getFloat("tetrobot.speed");
        tetrobot.laserLength = appConfig->getFloat("tetrobot.laserLength");
        tetrobot.explosionImpulse = appConfig->getFloat("tetrobot.explosionImpulse");
        tetrobot.explosionDamage = appConfig->getFloat("tetrobot.explosionDamage");

        /*
         * tetrobot2.
         */

        tetrobot2.life = appConfig->getFloat("tetrobot2.life");
        tetrobot2.turnSpeed = deg2rad(appConfig->getFloat("tetrobot2.turnSpeed"));
        tetrobot2.walkSpeed = appConfig->getFloat("tetrobot2.walkSpeed");
        tetrobot2.shootMinTimeout = appConfig->getFloat("tetrobot2.shootMinTimeout");
        tetrobot2.shootMaxTimeout = appConfig->getFloat("tetrobot2.shootMaxTimeout");
        tetrobot2.aimDuration = appConfig->getFloat("tetrobot2.aimDuration");
        tetrobot2.shootDuration = appConfig->getFloat("tetrobot2.shootDuration");
        tetrobot2.shootDamage = appConfig->getFloat("tetrobot2.shootDamage");
        tetrobot2.shootLength = appConfig->getFloat("tetrobot2.shootLength");
        tetrobot2.shootImpulse = appConfig->getFloat("tetrobot2.shootImpulse");
        tetrobot2.explosionImpulse = appConfig->getFloat("tetrobot2.explosionImpulse");
        tetrobot2.explosionDamage = appConfig->getFloat("tetrobot2.explosionDamage");
        tetrobot2.damageImpulseThreshold = appConfig->getFloat("tetrobot2.damageImpulseThreshold");
        tetrobot2.damageMultiplier = appConfig->getFloat("tetrobot2.damageMultiplier");

        /*
         * tetrocrab.
         */

        tetrocrab.life = appConfig->getFloat("tetrocrab.life");
        tetrocrab.turnSpeed = deg2rad(appConfig->getFloat("tetrocrab.turnSpeed"));
        tetrocrab.walkSpeed = appConfig->getFloat("tetrocrab.walkSpeed");
        tetrocrab.biteTime = appConfig->getFloat("tetrocrab.biteTime");
        tetrocrab.biteDamage = appConfig->getFloat("tetrocrab.biteDamage");
        tetrocrab.damageImpulseThreshold = appConfig->getFloat("tetrocrab.damageImpulseThreshold");
        tetrocrab.damageMultiplier = appConfig->getFloat("tetrocrab.damageMultiplier");

        /*
         * tetrocrab2.
         */

        tetrocrab2.life = appConfig->getFloat("tetrocrab2.life");

        /*
         * tetrocrab 3.
         */

        tetrocrab3.life = appConfig->getFloat("tetrocrab3.life");
        tetrocrab3.turnSpeed = deg2rad(appConfig->getFloat("tetrocrab3.turnSpeed"));
        tetrocrab3.walkSpeed = appConfig->getFloat("tetrocrab3.walkSpeed");

        /*
         * tetrocrab4.
         */

        tetrocrab4.life = appConfig->getFloat("tetrocrab4.life");

        /*
         * scorp.
         */

        scorp.life = appConfig->getFloat("scorp.life");
        scorp.turnSpeed = deg2rad(appConfig->getFloat("scorp.turnSpeed"));
        scorp.walkSpeed = appConfig->getFloat("scorp.walkSpeed");
        scorp.attackRadius = appConfig->getFloat("scorp.attackRadius");
        scorp.minRadius = appConfig->getFloat("scorp.minRadius");
        scorp.attackTime = appConfig->getFloat("scorp.attackTime");
        scorp.shootAngle = deg2rad(appConfig->getFloat("scorp.shootAngle"));
        scorp.shootVelocity = appConfig->getFloat("scorp.shootVelocity");
        scorp.shootDamage = appConfig->getFloat("scorp.shootDamage");
        scorp.damageImpulseThreshold = appConfig->getFloat("scorp.damageImpulseThreshold");
        scorp.damageMultiplier = appConfig->getFloat("scorp.damageMultiplier");
        scorp.explosionImpulse = appConfig->getFloat("scorp.explosionImpulse");
        scorp.explosionDamage = appConfig->getFloat("scorp.explosionDamage");

        /*
         * scorp2.
         */

        scorp2.life = appConfig->getFloat("scorp2.life");

        /*
         * scorp3.
         */

        scorp3.life = appConfig->getFloat("scorp3.life");
        scorp3.walkSpeed = appConfig->getFloat("scorp3.walkSpeed");

        /*
         * scorp4.
         */

        scorp4.life = appConfig->getFloat("scorp4.life");

        /*
         * barrel1.
         */

        barrel1.life = appConfig->getFloat("barrel1.life");
        barrel1.damageImpulseThreshold = appConfig->getFloat("barrel1.damageImpulseThreshold");
        barrel1.damageMultiplier = appConfig->getFloat("barrel1.damageMultiplier");
        barrel1.explosionImpulse = appConfig->getFloat("barrel1.explosionImpulse");
        barrel1.explosionDamage = appConfig->getFloat("barrel1.explosionDamage");
        barrel1.blastDamageMultiplier = appConfig->getFloat("barrel1.blastDamageMultiplier");

        /*
         * barrel2.
         */

        barrel2.life = appConfig->getFloat("barrel2.life");
        barrel2.damageImpulseThreshold = appConfig->getFloat("barrel2.damageImpulseThreshold");
        barrel2.damageMultiplier = appConfig->getFloat("barrel2.damageMultiplier");

        /*
         * turret1.
         */

        turret1.life = appConfig->getFloat("turret1.life");
        turret1.damageImpulseThreshold = appConfig->getFloat("turret1.damageImpulseThreshold");
        turret1.damageMultiplier = appConfig->getFloat("turret1.damageMultiplier");
        turret1.turnSpeed = deg2rad(appConfig->getFloat("turret1.turnSpeed"));
        turret1.shootAngle = deg2rad(appConfig->getFloat("turret1.shootAngle"));
        turret1.shootDamage = appConfig->getFloat("turret1.shootDamage");
        turret1.shootVelocity = appConfig->getFloat("turret1.shootVelocity");
        turret1.shootDelay = appConfig->getFloat("turret1.shootDelay");
        turret1.blastDamageMultiplier = appConfig->getFloat("turret1.blastDamageMultiplier");
        turret1.explosionImpulse = appConfig->getFloat("turret1.explosionImpulse");
        turret1.explosionDamage = appConfig->getFloat("turret1.explosionDamage");

        /*
         * pyrobot.
         */

        pyrobot.life = appConfig->getFloat("pyrobot.life");
        pyrobot.turnSpeed = deg2rad(appConfig->getFloat("pyrobot.turnSpeed"));
        pyrobot.walkSpeed = appConfig->getFloat("pyrobot.walkSpeed");
        pyrobot.attackRadiusMin = appConfig->getFloat("pyrobot.attackRadiusMin");
        pyrobot.attackRadiusMax = appConfig->getFloat("pyrobot.attackRadiusMax");
        pyrobot.shootDamage = appConfig->getFloat("pyrobot.shootDamage");

        /*
         * boss skull.
         */

        bossSkull.life = appConfig->getFloat("boss skull.life");
        bossSkull.slowWalkSpeed = appConfig->getFloat("boss skull.slowWalkSpeed");
        bossSkull.fastWalkSpeed = appConfig->getFloat("boss skull.fastWalkSpeed");
        bossSkull.gunFrontTurnSpeed = deg2rad(appConfig->getFloat("boss skull.gun.front.turnSpeed"));
        bossSkull.gunFrontShootAngle = deg2rad(appConfig->getFloat("boss skull.gun.front.shootAngle"));
        bossSkull.gunFrontShootRadius = appConfig->getFloat("boss skull.gun.front.shootRadius");
        bossSkull.gunFrontShootVelocity = appConfig->getFloat("boss skull.gun.front.shootVelocity");
        bossSkull.gunFrontShootDelay = appConfig->getFloat("boss skull.gun.front.shootDelay");
        bossSkull.gunFrontShootDamage = appConfig->getFloat("boss skull.gun.front.shootDamage");
        bossSkull.gunBackTurnSpeed = deg2rad(appConfig->getFloat("boss skull.gun.back.turnSpeed"));
        bossSkull.gunBackShootAngle = deg2rad(appConfig->getFloat("boss skull.gun.back.shootAngle"));
        bossSkull.gunBackShootRadius = appConfig->getFloat("boss skull.gun.back.shootRadius");
        bossSkull.gunBackShootVelocity = appConfig->getFloat("boss skull.gun.back.shootVelocity");
        bossSkull.gunBackShootDelay = appConfig->getFloat("boss skull.gun.back.shootDelay");
        bossSkull.gunBackShootDamage = appConfig->getFloat("boss skull.gun.back.shootDamage");
        bossSkull.gunMidTurnSpeed = deg2rad(appConfig->getFloat("boss skull.gun.mid.turnSpeed"));
        bossSkull.gunMidShootAngle = deg2rad(appConfig->getFloat("boss skull.gun.mid.shootAngle"));
        bossSkull.gunMidShootRadius = appConfig->getFloat("boss skull.gun.mid.shootRadius");
        bossSkull.gunMidDamage = appConfig->getFloat("boss skull.gun.mid.damage");
        bossSkull.blastDamageMultiplier = appConfig->getFloat("boss skull.blastDamageMultiplier");

        /*
         * spider.
         */

        spider.legVelocity = appConfig->getFloat("spider.legVelocity");
        spider.attackVelocity = appConfig->getFloat("spider.attackVelocity");
        spider.attackTimeout = appConfig->getFloat("spider.attackTimeout");
        spider.attackDamage = appConfig->getFloat("spider.attackDamage");

        /*
         * baby spider.
         */

        babySpider.turnSpeed = deg2rad(appConfig->getFloat("baby spider.turnSpeed"));
        babySpider.biteTime = appConfig->getFloat("baby spider.biteTime");
        babySpider.fireDamageMultiplier = appConfig->getFloat("baby spider.fireDamageMultiplier");
        babySpider.damageImpulseThreshold = appConfig->getFloat("baby spider.damageImpulseThreshold");
        babySpider.damageMultiplier = appConfig->getFloat("baby spider.damageMultiplier");

        /*
         * baby spider 1.
         */

        babySpider1.life = appConfig->getFloat("baby spider 1.life");
        babySpider1.walkSpeed = appConfig->getFloat("baby spider 1.walkSpeed");
        babySpider1.biteDamage = appConfig->getFloat("baby spider 1.biteDamage");

        /*
         * baby spider 2.
         */

        babySpider2.life = appConfig->getFloat("baby spider 2.life");
        babySpider2.walkSpeed = appConfig->getFloat("baby spider 2.walkSpeed");
        babySpider2.biteDamage = appConfig->getFloat("baby spider 2.biteDamage");

        /*
         * spider nest.
         */

        spiderNest.life = appConfig->getFloat("spider nest.life");
        spiderNest.radius = appConfig->getFloat("spider nest.radius");
        spiderNest.damageImpulseThreshold = appConfig->getFloat("spider nest.damageImpulseThreshold");
        spiderNest.damageMultiplier = appConfig->getFloat("spider nest.damageMultiplier");

        /*
         * enforcer.
         */

        enforcer.turnSpeed = deg2rad(appConfig->getFloat("enforcer.turnSpeed"));
        enforcer.walkSpeed = appConfig->getFloat("enforcer.walkSpeed");
        enforcer.minRadius = appConfig->getFloat("enforcer.minRadius");
        enforcer.shootAngle = deg2rad(appConfig->getFloat("enforcer.shootAngle"));
        enforcer.damageImpulseThreshold = appConfig->getFloat("enforcer.damageImpulseThreshold");
        enforcer.damageMultiplier = appConfig->getFloat("enforcer.damageMultiplier");

        /*
         * enforcer 1.
         */

        enforcer1.life = appConfig->getFloat("enforcer 1.life");
        enforcer1.shootVelocity = appConfig->getFloat("enforcer 1.shootVelocity");
        enforcer1.shootDamage = appConfig->getFloat("enforcer 1.shootDamage");
        enforcer1.shootInterval = appConfig->getFloat("enforcer 1.shootInterval");
        enforcer1.color = appConfig->getColor("enforcer 1.color");

        /*
         * enforcer 2.
         */

        enforcer2.life = appConfig->getFloat("enforcer 2.life");
        enforcer2.shootVelocity = appConfig->getFloat("enforcer 2.shootVelocity");
        enforcer2.shootDamage = appConfig->getFloat("enforcer 2.shootDamage");
        enforcer2.shootTurnInterval = appConfig->getFloat("enforcer 2.shootTurnInterval");
        enforcer2.shootDelay = appConfig->getFloat("enforcer 2.shootDelay");
        enforcer2.color = appConfig->getColor("enforcer 2.color");

        /*
         * enforcer 3.
         */

        enforcer3.color = appConfig->getColor("enforcer 3.color");

        /*
         * sentry.
         */

        sentry.torsoTurnSpeed = deg2rad(appConfig->getFloat("sentry.torsoTurnSpeed"));
        sentry.legsTurnSpeed = deg2rad(appConfig->getFloat("sentry.legsTurnSpeed"));
        sentry.walkSpeed = appConfig->getFloat("sentry.walkSpeed");
        sentry.walkRadius = appConfig->getFloat("sentry.walkRadius");
        sentry.standRadius = appConfig->getFloat("sentry.standRadius");
        sentry.shootRadius = appConfig->getFloat("sentry.shootRadius");
        sentry.shootStartAngle = deg2rad(appConfig->getFloat("sentry.shootStartAngle"));
        sentry.shootStopAngle = deg2rad(appConfig->getFloat("sentry.shootStopAngle"));
        sentry.shootVelocity = appConfig->getFloat("sentry.shootVelocity");
        sentry.shootSpreadAngle = deg2rad(appConfig->getFloat("sentry.shootSpreadAngle"));
        sentry.shootTurnInterval = appConfig->getFloat("sentry.shootTurnInterval");
        sentry.explosionImpulse = appConfig->getFloat("sentry.explosionImpulse");
        sentry.explosionDamage = appConfig->getFloat("sentry.explosionDamage");

        /*
         * sentry 1.
         */

        sentry1.life = appConfig->getFloat("sentry 1.life");
        sentry1.shootDamage = appConfig->getFloat("sentry 1.shootDamage");
        sentry1.shootTurnDuration = appConfig->getFloat("sentry 1.shootTurnDuration");
        sentry1.shootLoopDelay = appConfig->getFloat("sentry 1.shootLoopDelay");

        /*
         * sentry 2.
         */

        sentry2.life = appConfig->getFloat("sentry 2.life");
        sentry2.shootDamage = appConfig->getFloat("sentry 2.shootDamage");
        sentry2.shootTurnDuration = appConfig->getFloat("sentry 2.shootTurnDuration");
        sentry2.shootLoopDelay = appConfig->getFloat("sentry 2.shootLoopDelay");

        /*
         * gorger.
         */

        gorger.life = appConfig->getFloat("gorger.life");
        gorger.turnSpeed = deg2rad(appConfig->getFloat("gorger.turnSpeed"));
        gorger.walkSpeed = appConfig->getFloat("gorger.walkSpeed");
        gorger.attackAngle = deg2rad(appConfig->getFloat("gorger.attackAngle"));
        gorger.attackDelayMin = appConfig->getFloat("gorger.attackDelayMin");
        gorger.attackDelayMax = appConfig->getFloat("gorger.attackDelayMax");
        gorger.meleeDamage = appConfig->getFloat("gorger.meleeDamage");
        gorger.shootExplosionImpulse = appConfig->getFloat("gorger.shootExplosionImpulse");
        gorger.shootExplosionDamage = appConfig->getFloat("gorger.shootExplosionDamage");
        gorger.shootVelocity = appConfig->getFloat("gorger.shootVelocity");
        gorger.shootRepeatProbability = appConfig->getFloat("gorger.shootRepeatProbability");
        gorger.explosionImpulse = appConfig->getFloat("gorger.explosionImpulse");
        gorger.explosionDamage = appConfig->getFloat("gorger.explosionDamage");
        gorger.blastDamageMultiplier = appConfig->getFloat("gorger.blastDamageMultiplier");

        /*
         * crate1.
         */

        crate1.life = appConfig->getFloat("crate1.life");
        crate1.damageImpulseThreshold = appConfig->getFloat("crate1.damageImpulseThreshold");
        crate1.damageMultiplier = appConfig->getFloat("crate1.damageMultiplier");

        /*
         * orbo.
         */

        orbo.life = appConfig->getFloat("orbo.life");
        orbo.turnSpeed = deg2rad(appConfig->getFloat("orbo.turnSpeed"));
        orbo.walkSpeed = appConfig->getFloat("orbo.walkSpeed");
        orbo.attackRadius = appConfig->getFloat("orbo.attackRadius");
        orbo.attackTimeout = appConfig->getFloat("orbo.attackTimeout");
        orbo.shootVelocity = appConfig->getFloat("orbo.shootVelocity");
        orbo.shootSpreadAngle = deg2rad(appConfig->getFloat("orbo.shootSpreadAngle"));
        orbo.shootTurnInterval = appConfig->getFloat("orbo.shootTurnInterval");
        orbo.shootTurnDuration = appConfig->getFloat("orbo.shootTurnDuration");
        orbo.shootDamage = appConfig->getFloat("orbo.shootDamage");
        orbo.shootLoopDelay = appConfig->getFloat("orbo.shootLoopDelay");
        orbo.shootNumTurns = appConfig->getInt("orbo.shootNumTurns");
        orbo.explosionImpulse = appConfig->getFloat("orbo.explosionImpulse");
        orbo.explosionDamage = appConfig->getFloat("orbo.explosionDamage");

        /*
         * warder.
         */

        warder.life = appConfig->getFloat("warder.life");
        warder.turnSpeed = deg2rad(appConfig->getFloat("warder.turnSpeed"));
        warder.walkSpeed = appConfig->getFloat("warder.walkSpeed");
        warder.meleeRadius = appConfig->getFloat("warder.meleeRadius");
        warder.meleeDamage[0] = appConfig->getFloat("warder.meleeDamage.0");
        warder.meleeDamage[1] = appConfig->getFloat("warder.meleeDamage.1");
        warder.meleeDamage[2] = appConfig->getFloat("warder.meleeDamage.2");
        warder.shootTimeout = appConfig->getFloat("warder.shootTimeout");
        warder.shootDamage = appConfig->getFloat("warder.shootDamage");
        warder.shootLength = appConfig->getFloat("warder.shootLength");
        warder.shootImpulse = appConfig->getFloat("warder.shootImpulse");
        warder.shootDuration = appConfig->getFloat("warder.shootDuration");
        warder.explosionImpulse = appConfig->getFloat("warder.explosionImpulse");
        warder.explosionDamage = appConfig->getFloat("warder.explosionDamage");

        /*
         * keeper.
         */

        keeper.badImages = appConfig->getBool("keeper.badImages");
        keeper.life = appConfig->getFloat("keeper.life");
        keeper.turnSpeed = deg2rad(appConfig->getFloat("keeper.turnSpeed"));
        keeper.walkSpeed = appConfig->getFloat("keeper.walkSpeed");
        keeper.attackAngle = deg2rad(appConfig->getFloat("keeper.attackAngle"));
        keeper.attackDelayMin = appConfig->getFloat("keeper.attackDelayMin");
        keeper.attackDelayMax = appConfig->getFloat("keeper.attackDelayMax");
        keeper.meleeDamage = appConfig->getFloat("keeper.meleeDamage");
        keeper.gunAngle = deg2rad(appConfig->getFloat("keeper.gunAngle"));
        keeper.gunVelocity = appConfig->getFloat("keeper.gunVelocity");
        keeper.gunDamage = appConfig->getFloat("keeper.gunDamage");
        keeper.gunInterval = appConfig->getFloat("keeper.gunInterval");
        keeper.plasmaAngle = deg2rad(appConfig->getFloat("keeper.plasmaAngle"));
        keeper.plasmaVelocity = appConfig->getFloat("keeper.plasmaVelocity");
        keeper.plasmaDamage = appConfig->getFloat("keeper.plasmaDamage");
        keeper.plasmaDuration = appConfig->getFloat("keeper.plasmaDuration");
        keeper.missileImpulse = appConfig->getFloat("keeper.missileImpulse");
        keeper.missileDamage = appConfig->getFloat("keeper.missileDamage");
        keeper.missileVelocity = appConfig->getFloat("keeper.missileVelocity");

        /*
         * boss core protector.
         */

        bossCoreProtector.moveSpeed = appConfig->getFloat("boss core protector.moveSpeed");
        bossCoreProtector.fastMoveSpeed = appConfig->getFloat("boss core protector.fastMoveSpeed");
        bossCoreProtector.shootMoveAngle = deg2rad(appConfig->getFloat("boss core protector.shootMoveAngle"));
        bossCoreProtector.shootMoveSpeed = appConfig->getFloat("boss core protector.shootMoveSpeed");
        bossCoreProtector.shootTimeout = appConfig->getFloat("boss core protector.shootTimeout");
        bossCoreProtector.shootDamage = appConfig->getFloat("boss core protector.shootDamage");
        bossCoreProtector.shootLength = appConfig->getFloat("boss core protector.shootLength");
        bossCoreProtector.shootImpulse = appConfig->getFloat("boss core protector.shootImpulse");
        bossCoreProtector.shootMinDuration = appConfig->getFloat("boss core protector.shootMinDuration");
        bossCoreProtector.shootMaxDuration = appConfig->getFloat("boss core protector.shootMaxDuration");
        bossCoreProtector.initVelocity = appConfig->getFloat("boss core protector.initVelocity");
        bossCoreProtector.aimVelocity = appConfig->getFloat("boss core protector.aimVelocity");
        bossCoreProtector.strikeVelocity = appConfig->getFloat("boss core protector.strikeVelocity");
        bossCoreProtector.closeDistance = appConfig->getFloat("boss core protector.closeDistance");
        bossCoreProtector.farDistance = appConfig->getFloat("boss core protector.farDistance");
        bossCoreProtector.meleeDamage = appConfig->getFloat("boss core protector.meleeDamage");
        bossCoreProtector.meleeRollAngle = deg2rad(appConfig->getFloat("boss core protector.meleeRollAngle"));

        /*
         * rod bundle.
         */

        rodBundle.life = appConfig->getFloat("rod bundle.life");

        /*
         * barrel3.
         */

        barrel3.life = appConfig->getFloat("barrel3.life");
        barrel3.damageImpulseThreshold = appConfig->getFloat("barrel3.damageImpulseThreshold");
        barrel3.damageMultiplier = appConfig->getFloat("barrel3.damageMultiplier");
        barrel3.toxicDamage = appConfig->getFloat("barrel3.toxicDamage");
        barrel3.toxicDamageTimeout = appConfig->getFloat("barrel3.toxicDamageTimeout");

        /*
         * acid worm.
         */

        acidworm.life = appConfig->getFloat("acid worm.life");
        acidworm.turnTorque = appConfig->getFloat("acid worm.turnTorque");
        acidworm.idleForce = appConfig->getFloat("acid worm.idleForce");
        acidworm.idleSpeed = deg2rad(appConfig->getFloat("acid worm.idleSpeed"));
        acidworm.spitTimeout = appConfig->getFloat("acid worm.spitTimeout");
        acidworm.spitForce1 = appConfig->getFloat("acid worm.spitForce1");
        acidworm.spitForce1Duration = appConfig->getFloat("acid worm.spitForce1Duration");
        acidworm.spitForce2 = appConfig->getFloat("acid worm.spitForce2");
        acidworm.spitForce2Duration = appConfig->getFloat("acid worm.spitForce2Duration");
        acidworm.shootAngle = deg2rad(appConfig->getFloat("acid worm.shootAngle"));
        acidworm.shootVelocity = appConfig->getFloat("acid worm.shootVelocity");
        acidworm.explosionTimeout = appConfig->getFloat("acid worm.explosionTimeout");
        acidworm.explosionImpulse = appConfig->getFloat("acid worm.explosionImpulse");
        acidworm.explosionDamage = appConfig->getFloat("acid worm.explosionDamage");
        acidworm.toxicDamage = appConfig->getFloat("acid worm.toxicDamage");
        acidworm.toxicDamageTimeout = appConfig->getFloat("acid worm.toxicDamageTimeout");
        acidworm.foldDelay = appConfig->getFloat("acid worm.foldDelay");
        acidworm.foldForce = appConfig->getFloat("acid worm.foldForce");
        acidworm.unfoldDelay = appConfig->getFloat("acid worm.unfoldDelay");

        /*
         * centipede.
         */

        centipede.life = appConfig->getFloat("centipede.life");
        centipede.turnSpeed = deg2rad(appConfig->getFloat("centipede.turnSpeed"));
        centipede.walkSpeed = appConfig->getFloat("centipede.walkSpeed");
        centipede.biteTime = appConfig->getFloat("centipede.biteTime");
        centipede.biteDamage = appConfig->getFloat("centipede.biteDamage");
        centipede.attackAngle = deg2rad(appConfig->getFloat("centipede.attackAngle"));
        centipede.attackTimeout = appConfig->getFloat("centipede.attackTimeout");
        centipede.spitDamage = appConfig->getFloat("centipede.spitDamage");
        centipede.spitMinShots = appConfig->getInt("centipede.spitMinShots");
        centipede.spitMaxShots = appConfig->getInt("centipede.spitMaxShots");
        centipede.spitTotalShots = appConfig->getInt("centipede.spitTotalShots");
        centipede.spitInterval = appConfig->getFloat("centipede.spitInterval");
        centipede.spitVelocity = appConfig->getFloat("centipede.spitVelocity");
        centipede.spitAngle = deg2rad(appConfig->getFloat("centipede.spitAngle"));

        /*
         * beetle 1.
         */

        beetle1.life = appConfig->getFloat("beetle 1.life");
        beetle1.turnSpeed = deg2rad(appConfig->getFloat("beetle 1.turnSpeed"));
        beetle1.walkSpeed = appConfig->getFloat("beetle 1.walkSpeed");
        beetle1.biteTime = appConfig->getFloat("beetle 1.biteTime");
        beetle1.biteDamage = appConfig->getFloat("beetle 1.biteDamage");
        beetle1.attackAngle = deg2rad(appConfig->getFloat("beetle 1.attackAngle"));
        beetle1.attackTimeout = appConfig->getFloat("beetle 1.attackTimeout");
        beetle1.spitDamage = appConfig->getFloat("beetle 1.spitDamage");
        beetle1.spitVelocity = appConfig->getFloat("beetle 1.spitVelocity");
        beetle1.spitAngle = deg2rad(appConfig->getFloat("beetle 1.spitAngle"));
        beetle1.numSpawn = appConfig->getInt("beetle 1.numSpawn");
        beetle1.damageImpulseThreshold = appConfig->getFloat("beetle 1.damageImpulseThreshold");
        beetle1.damageMultiplier = appConfig->getFloat("beetle 1.damageMultiplier");

        /*
         * beetle 2.
         */

        beetle2.life = appConfig->getFloat("beetle 2.life");
        beetle2.turnSpeed = deg2rad(appConfig->getFloat("beetle 2.turnSpeed"));
        beetle2.walkSpeed = appConfig->getFloat("beetle 2.walkSpeed");
        beetle2.explosionTimeout = appConfig->getFloat("beetle 2.explosionTimeout");
        beetle2.explosionImpulse = appConfig->getFloat("beetle 2.explosionImpulse");
        beetle2.explosionDamage = appConfig->getFloat("beetle 2.explosionDamage");
        beetle2.damageImpulseThreshold = appConfig->getFloat("beetle 2.damageImpulseThreshold");
        beetle2.damageMultiplier = appConfig->getFloat("beetle 2.damageMultiplier");

        /*
         * boat.
         */

        boat.turnSpeed = deg2rad(appConfig->getFloat("boat.turnSpeed"));
        boat.forwardSpeed = appConfig->getFloat("boat.forwardSpeed");
        boat.backSpeed = appConfig->getFloat("boat.backSpeed");
        boat.gunTurnSpeed = deg2rad(appConfig->getFloat("boat.gunTurnSpeed"));
        boat.shootDamage = appConfig->getFloat("boat.shootDamage");
        boat.shootLength = appConfig->getFloat("boat.shootLength");
        boat.shootImpulse = appConfig->getFloat("boat.shootImpulse");

        /*
         * power gen.
         */

        powerGen1.life = appConfig->getFloat("power gen 1.life");
        powerGen2.life = appConfig->getFloat("power gen 2.life");

        /*
         * boss squid.
         */

        bossSquid.life = appConfig->getFloat("boss squid.life");
        bossSquid.eyeLife = appConfig->getFloat("boss squid.eyeLife");
        bossSquid.secondPhaseLifePercent = appConfig->getFloat("boss squid.secondPhaseLifePercent");
        bossSquid.attackTimeout = appConfig->getFloat("boss squid.attackTimeout");
        bossSquid.attack1Damage = appConfig->getFloat("boss squid.attack1Damage");
        bossSquid.attack1MinShots = appConfig->getInt("boss squid.attack1MinShots");
        bossSquid.attack1MaxShots = appConfig->getInt("boss squid.attack1MaxShots");
        bossSquid.attack1TotalShots = appConfig->getInt("boss squid.attack1TotalShots");
        bossSquid.attack1Interval = appConfig->getFloat("boss squid.attack1Interval");
        bossSquid.attack1Velocity = appConfig->getFloat("boss squid.attack1Velocity");
        bossSquid.attack1Angle = deg2rad(appConfig->getFloat("boss squid.attack1Angle"));
        bossSquid.attack2Damage = appConfig->getFloat("boss squid.attack2Damage");
        bossSquid.attack2Velocity = appConfig->getFloat("boss squid.attack2Velocity");
        bossSquid.attack2Angle = deg2rad(appConfig->getFloat("boss squid.attack2Angle"));
        bossSquid.meleeDamage = appConfig->getFloat("boss squid.meleeDamage");
        bossSquid.mouthOpenTime = appConfig->getFloat("boss squid.mouthOpenTime");

        /*
         * mech.
         */

        mech.life = appConfig->getFloat("mech.life");
        mech.legsTurnSpeed = deg2rad(appConfig->getFloat("mech.legsTurnSpeed"));
        mech.torsoTurnSpeed = deg2rad(appConfig->getFloat("mech.torsoTurnSpeed"));
        mech.walkSpeed = appConfig->getFloat("mech.walkSpeed");
        mech.attackDamage = appConfig->getFloat("mech.attackDamage");
        mech.attackVelocity = appConfig->getFloat("mech.attackVelocity");
        mech.attackSpreadAngle = deg2rad(appConfig->getFloat("mech.attackSpreadAngle"));
        mech.attackInterval = appConfig->getFloat("mech.attackInterval");
        mech.meleeImpulse = appConfig->getFloat("mech.meleeImpulse");
        mech.meleeDamage = appConfig->getFloat("mech.meleeDamage");

        /*
         * homer.
         */

        homer.life = appConfig->getFloat("homer.life");
        homer.turnSpeed = deg2rad(appConfig->getFloat("homer.turnSpeed"));
        homer.walkSpeed = appConfig->getFloat("homer.walkSpeed");
        homer.closeRadius = appConfig->getFloat("homer.closeRadius");
        homer.attackAngle = deg2rad(appConfig->getFloat("homer.attackAngle"));
        homer.attackLargeTimeout = appConfig->getFloat("homer.attackLargeTimeout");
        homer.attackSmallTimeout = appConfig->getFloat("homer.attackSmallTimeout");
        homer.gunAngle = deg2rad(appConfig->getFloat("homer.gunAngle"));
        homer.gunVelocity = appConfig->getFloat("homer.gunVelocity");
        homer.gunDamage = appConfig->getFloat("homer.gunDamage");
        homer.gunInterval = appConfig->getFloat("homer.gunInterval");
        homer.gunDuration = appConfig->getFloat("homer.gunDuration");
        homer.missileImpulse = appConfig->getFloat("homer.missileImpulse");
        homer.missileDamage = appConfig->getFloat("homer.missileDamage");
        homer.missileVelocity = appConfig->getFloat("homer.missileVelocity");
        homer.missileSeekVelocity = deg2rad(appConfig->getFloat("homer.missileSeekVelocity"));
        homer.meleeDamage = appConfig->getFloat("homer.meleeDamage");
        homer.explosionImpulse = appConfig->getFloat("homer.explosionImpulse");
        homer.explosionDamage = appConfig->getFloat("homer.explosionDamage");

        /*
         * boss chopper.
         */

        bossChopper.life = appConfig->getFloat("boss chopper.life");
        bossChopper.slowTurnSpeed = deg2rad(appConfig->getFloat("boss chopper.slowTurnSpeed"));
        bossChopper.fastTurnSpeed = deg2rad(appConfig->getFloat("boss chopper.fastTurnSpeed"));
        bossChopper.slowWalkSpeed = appConfig->getFloat("boss chopper.slowWalkSpeed");
        bossChopper.fastWalkSpeed = appConfig->getFloat("boss chopper.fastWalkSpeed");
        bossChopper.blastDamageMultiplier = appConfig->getFloat("boss chopper.blastDamageMultiplier");
        bossChopper.gunAngle = deg2rad(appConfig->getFloat("boss chopper.gunAngle"));
        bossChopper.gunVelocity = appConfig->getFloat("boss chopper.gunVelocity");
        bossChopper.gunDamage = appConfig->getFloat("boss chopper.gunDamage");
        bossChopper.gunInterval = appConfig->getFloat("boss chopper.gunInterval");
        bossChopper.gunDuration = appConfig->getFloat("boss chopper.gunDuration");
        bossChopper.missileImpulse = appConfig->getFloat("boss chopper.missileImpulse");
        bossChopper.missileDamage = appConfig->getFloat("boss chopper.missileDamage");
        bossChopper.missileVelocity = appConfig->getFloat("boss chopper.missileVelocity");
        bossChopper.missileSeekVelocity = deg2rad(appConfig->getFloat("boss chopper.missileSeekVelocity"));
        bossChopper.napalmDamage = appConfig->getFloat("boss chopper.napalmDamage");
        bossChopper.shootDelay = appConfig->getFloat("boss chopper.shootDelay");

        /*
         * jars.
         */

        jar1.life = appConfig->getFloat("jar 1.life");
        jar2.life = appConfig->getFloat("jar 2.life");
        jar3.life = appConfig->getFloat("jar 3.life");
        jar4.life = appConfig->getFloat("jar 4.life");

        /*
         * creature.
         */

        subKeys = appConfig->getSubKeys("creature");

        if (subKeys.empty()) {
            LOG4CPLUS_ERROR(logger(), "Config values for \"creature\" not found");
        }

        creature.resize(subKeys.size());

        for (size_t i = 0; i < creature.size(); ++i) {
            creature[i].life = appConfig->getFloat("creature." + subKeys[i] + ".life");
            creature[i].damageImpulseThreshold = appConfig->getFloat("creature." + subKeys[i] + ".damageImpulseThreshold");
            creature[i].damageMultiplier = appConfig->getFloat("creature." + subKeys[i] + ".damageMultiplier");
        }

        /*
         * gas cloud 1.
         */

        gasCloud1.damage = appConfig->getFloat("gas cloud 1.damage");
        gasCloud1.damageTimeout = appConfig->getFloat("gas cloud 1.damageTimeout");
        gasCloud1.propagationDelay = appConfig->getFloat("gas cloud 1.propagationDelay");
        gasCloud1.propagationFactor = appConfig->getFloat("gas cloud 1.propagationFactor");
        gasCloud1.propagationMaxDelay = appConfig->getFloat("gas cloud 1.propagationMaxDelay");

        /*
         * octopus.
         */

        octopus.life = appConfig->getFloat("octopus.life");
        octopus.hitDamage = appConfig->getFloat("octopus.hitDamage");
        octopus.biteTime = appConfig->getFloat("octopus.biteTime");
        octopus.biteDamage = appConfig->getFloat("octopus.biteDamage");
        octopus.spitDamage = appConfig->getFloat("octopus.spitDamage");
        octopus.spitVelocity = appConfig->getFloat("octopus.spitVelocity");
        octopus.rollTorque = appConfig->getFloat("octopus.rollTorque");
        octopus.rollDuration = appConfig->getFloat("octopus.rollDuration");
        octopus.chargeForce = appConfig->getFloat("octopus.chargeForce");
        octopus.chargeDuration = appConfig->getFloat("octopus.chargeDuration");
        octopus.chargeDelay = appConfig->getFloat("octopus.chargeDelay");

        /*
         * walker.
         */

        walker.life = appConfig->getFloat("walker.life");
        walker.missileDelay = appConfig->getFloat("walker.missileDelay");
        walker.missileImpulse = appConfig->getFloat("walker.missileImpulse");
        walker.missileDamage = appConfig->getFloat("walker.missileDamage");
        walker.missileVelocity = appConfig->getFloat("walker.missileVelocity");
        walker.missileSeekVelocity = deg2rad(appConfig->getFloat("walker.missileSeekVelocity"));
        walker.spitDelay = appConfig->getFloat("walker.spitDelay");
        walker.spitDamage = appConfig->getFloat("walker.spitDamage");
        walker.spitMinShots = appConfig->getInt("walker.spitMinShots");
        walker.spitMaxShots = appConfig->getInt("walker.spitMaxShots");
        walker.spitTotalShots = appConfig->getInt("walker.spitTotalShots");
        walker.spitInterval = appConfig->getFloat("walker.spitInterval");
        walker.spitVelocity = appConfig->getFloat("walker.spitVelocity");
        walker.spitAngle = deg2rad(appConfig->getFloat("walker.spitAngle"));

        /*
         * snake.
         */

        snake.life = appConfig->getFloat("snake.life");
        snake.missileVelocity = appConfig->getFloat("snake.missileVelocity");
        snake.missileExplosionTimeout = appConfig->getFloat("snake.missileExplosionTimeout");
        snake.missileExplosionImpulse = appConfig->getFloat("snake.missileExplosionImpulse");
        snake.missileExplosionDamage = appConfig->getFloat("snake.missileExplosionDamage");
        snake.missileToxicDamage = appConfig->getFloat("snake.missileToxicDamage");
        snake.missileToxicDamageTimeout = appConfig->getFloat("snake.missileToxicDamageTimeout");
        snake.missileInterval = appConfig->getFloat("snake.missileInterval");
        snake.gunDamage = appConfig->getFloat("snake.gunDamage");
        snake.gunLength = appConfig->getFloat("snake.gunLength");
        snake.gunImpulse = appConfig->getFloat("snake.gunImpulse");
        snake.gunDuration = appConfig->getFloat("snake.gunDuration");
        snake.shootDelay = appConfig->getFloat("snake.shootDelay");

        /*
         * boss buddy.
         */

        bossBuddy.life = appConfig->getFloat("boss buddy.life");
        bossBuddy.attackMinTimeout = appConfig->getFloat("boss buddy.attackMinTimeout");
        bossBuddy.attackMaxTimeout = appConfig->getFloat("boss buddy.attackMaxTimeout");
        bossBuddy.attackPreTongueNum = appConfig->getInt("boss buddy.attackPreTongueNum");
        bossBuddy.seekerImpulse = appConfig->getFloat("boss buddy.seekerImpulse");
        bossBuddy.seekerDamage = appConfig->getFloat("boss buddy.seekerDamage");
        bossBuddy.seekerVelocity = appConfig->getFloat("boss buddy.seekerVelocity");
        bossBuddy.seekerSeekVelocity = deg2rad(appConfig->getFloat("boss buddy.seekerSeekVelocity"));
        bossBuddy.seekerNum = appConfig->getInt("boss buddy.seekerNum");
        bossBuddy.seekerInterval = appConfig->getFloat("boss buddy.seekerInterval");
        bossBuddy.plasmaAngle = deg2rad(appConfig->getFloat("boss buddy.plasmaAngle"));
        bossBuddy.plasmaVelocity = appConfig->getFloat("boss buddy.plasmaVelocity");
        bossBuddy.plasmaDamage = appConfig->getFloat("boss buddy.plasmaDamage");
        bossBuddy.plasmaInterval = appConfig->getFloat("boss buddy.plasmaInterval");
        bossBuddy.plasmaDuration = appConfig->getFloat("boss buddy.plasmaDuration");
        bossBuddy.missileExplosionImpulse = appConfig->getFloat("boss buddy.missileExplosionImpulse");
        bossBuddy.missileExplosionDamage = appConfig->getFloat("boss buddy.missileExplosionDamage");
        bossBuddy.missileVelocity = appConfig->getFloat("boss buddy.missileVelocity");
        bossBuddy.missileInterval = appConfig->getFloat("boss buddy.missileInterval");
        bossBuddy.napalmDamage = appConfig->getFloat("boss buddy.napalmDamage");
        bossBuddy.tongueAttackDuration = appConfig->getFloat("boss buddy.tongueAttackDuration");
        bossBuddy.tongueIdleDuration = appConfig->getFloat("boss buddy.tongueIdleDuration");
        bossBuddy.sideLife = appConfig->getFloat("boss buddy.sideLife");
        bossBuddy.sideMeleeDamage = appConfig->getFloat("boss buddy.sideMeleeDamage");
        bossBuddy.sideDeadDuration = appConfig->getFloat("boss buddy.sideDeadDuration");
        bossBuddy.tongueLife = appConfig->getFloat("boss buddy.tongueLife");
        bossBuddy.tongueAttackDamage = appConfig->getFloat("boss buddy.tongueAttackDamage");
        bossBuddy.tongueAttackMinShots = appConfig->getInt("boss buddy.tongueAttackMinShots");
        bossBuddy.tongueAttackMaxShots = appConfig->getInt("boss buddy.tongueAttackMaxShots");
        bossBuddy.tongueAttackTotalShots = appConfig->getInt("boss buddy.tongueAttackTotalShots");
        bossBuddy.tongueAttackInterval = appConfig->getFloat("boss buddy.tongueAttackInterval");
        bossBuddy.tongueAttackVelocity = appConfig->getFloat("boss buddy.tongueAttackVelocity");
        bossBuddy.tongueAttackAngle = deg2rad(appConfig->getFloat("boss buddy.tongueAttackAngle"));

        /*
         * boss natan.
         */

        bossNatan.life = appConfig->getFloat("boss natan.life");
        bossNatan.attackMinTimeout = appConfig->getFloat("boss natan.attackMinTimeout");
        bossNatan.attackMaxTimeout = appConfig->getFloat("boss natan.attackMaxTimeout");
        bossNatan.shootDuration = appConfig->getFloat("boss natan.shootDuration");
        bossNatan.shootDelay = appConfig->getFloat("boss natan.shootDelay");
        bossNatan.plasmaAngle = deg2rad(appConfig->getFloat("boss natan.plasmaAngle"));
        bossNatan.plasmaVelocity = appConfig->getFloat("boss natan.plasmaVelocity");
        bossNatan.plasmaDamage = appConfig->getFloat("boss natan.plasmaDamage");
        bossNatan.plasmaInterval = appConfig->getFloat("boss natan.plasmaInterval");
        bossNatan.spitAngle = deg2rad(appConfig->getFloat("boss natan.spitAngle"));
        bossNatan.spitVelocity = appConfig->getFloat("boss natan.spitVelocity");
        bossNatan.spitDamage = appConfig->getFloat("boss natan.spitDamage");
        bossNatan.spitInterval = appConfig->getFloat("boss natan.spitInterval");
        bossNatan.blasterVelocity = appConfig->getFloat("boss natan.blasterVelocity");
        bossNatan.blasterDamage = appConfig->getFloat("boss natan.blasterDamage");
        bossNatan.blasterInterval = appConfig->getFloat("boss natan.blasterInterval");
        bossNatan.ramDamage = appConfig->getFloat("boss natan.ramDamage");
        bossNatan.ramHitDamage = appConfig->getFloat("boss natan.ramHitDamage");
        bossNatan.meleeDamage = appConfig->getFloat("boss natan.meleeDamage");
        bossNatan.syringeDamage = appConfig->getFloat("boss natan.syringeDamage");
        bossNatan.napalmDamage = appConfig->getFloat("boss natan.napalmDamage");
        bossNatan.napalmInterval = appConfig->getFloat("boss natan.napalmInterval");
        bossNatan.napalmNum = appConfig->getInt("boss natan.napalmNum");

        /*
         * heaters.
         */

        heater1.life = appConfig->getFloat("heater 1.life");

        /*
         * guardian.
         */

        guardian.life = appConfig->getFloat("guardian.life");
        guardian.jumpVelocity = appConfig->getFloat("guardian.jumpVelocity");
        guardian.jumpTimeout = appConfig->getFloat("guardian.jumpTimeout");
        guardian.shootMinTimeout = appConfig->getFloat("guardian.shootMinTimeout");
        guardian.shootMaxTimeout = appConfig->getFloat("guardian.shootMaxTimeout");
        guardian.shootDamage = appConfig->getFloat("guardian.shootDamage");
        guardian.shootVelocity = appConfig->getFloat("guardian.shootVelocity");
        guardian.turnSpeed = deg2rad(appConfig->getFloat("guardian.turnSpeed"));
        guardian.explosionImpulse = appConfig->getFloat("guardian.explosionImpulse");
        guardian.explosionDamage = appConfig->getFloat("guardian.explosionDamage");

        /*
         * guardian 2.
         */

        guardian2.life = appConfig->getFloat("guardian 2.life");
        guardian2.jumpVelocity = appConfig->getFloat("guardian 2.jumpVelocity");
        guardian2.jumpTimeout = appConfig->getFloat("guardian 2.jumpTimeout");
        guardian2.shootMinTimeout = appConfig->getFloat("guardian 2.shootMinTimeout");
        guardian2.shootMaxTimeout = appConfig->getFloat("guardian 2.shootMaxTimeout");

        /*
         * sawer.
         */

        sawer.turnSpeed = deg2rad(appConfig->getFloat("sawer.turnSpeed"));
        sawer.throwForce1 = appConfig->getFloat("sawer.throwForce1");
        sawer.throwForce1Duration = appConfig->getFloat("sawer.throwForce1Duration");
        sawer.throwForce2 = appConfig->getFloat("sawer.throwForce2");
        sawer.throwForce2Duration = appConfig->getFloat("sawer.throwForce2Duration");
        sawer.attackMinTimeout = appConfig->getFloat("sawer.attackMinTimeout");
        sawer.attackMaxTimeout = appConfig->getFloat("sawer.attackMaxTimeout");
        sawer.deployDelay = appConfig->getFloat("sawer.deployDelay");
        sawer.deploySpeed = appConfig->getFloat("sawer.deploySpeed");
        sawer.fixedSawDamageTimeout = appConfig->getFloat("sawer.fixedSawDamageTimeout");
        sawer.fixedSawDamage = appConfig->getFloat("sawer.fixedSawDamage");
        sawer.fixedSawEnemyDamage = appConfig->getFloat("sawer.fixedSawEnemyDamage");
        sawer.fixedSawImpulse = appConfig->getFloat("sawer.fixedSawImpulse");
        sawer.sawPickupDelay = appConfig->getFloat("sawer.sawPickupDelay");
        sawer.sawLife = appConfig->getFloat("sawer.sawLife");
        sawer.sawTurnSpeed = deg2rad(appConfig->getFloat("sawer.sawTurnSpeed"));
        sawer.sawWalkSpeed = appConfig->getFloat("sawer.sawWalkSpeed");
        sawer.sawFastWalkSpeed = appConfig->getFloat("sawer.sawFastWalkSpeed");
        sawer.sawDamage = appConfig->getFloat("sawer.sawDamage");
        sawer.sawFastDamage = appConfig->getFloat("sawer.sawFastDamage");
        sawer.sawPenetrationFactor = appConfig->getFloat("sawer.sawPenetrationFactor");

        /*
         * health station.
         */

        healthStation.life = appConfig->getFloat("health station.life");

        /*
         * boss queen.
         */

        bossQueen.life = appConfig->getFloat("boss queen.life");
        bossQueen.attack1AimVelocity = appConfig->getFloat("boss queen.attack1AimVelocity");
        bossQueen.attack1StrikeVelocity = appConfig->getFloat("boss queen.attack1StrikeVelocity");
        bossQueen.attack1DownVelocity = appConfig->getFloat("boss queen.attack1DownVelocity");
        bossQueen.legDamage = appConfig->getFloat("boss queen.legDamage");

        /*
         * boss beholder.
         */

        bossBeholder.life = appConfig->getFloat("boss beholder.life");
        bossBeholder.verySlowLegVelocity = deg2rad(appConfig->getFloat("boss beholder.verySlowLegVelocity"));
        bossBeholder.slowLegVelocity = deg2rad(appConfig->getFloat("boss beholder.slowLegVelocity"));
        bossBeholder.fastLegVelocity = deg2rad(appConfig->getFloat("boss beholder.fastLegVelocity"));
        bossBeholder.shieldVelocity = deg2rad(appConfig->getFloat("boss beholder.shieldVelocity"));
        bossBeholder.legDamage = appConfig->getFloat("boss beholder.legDamage");
        bossBeholder.fireballVelocity = appConfig->getFloat("boss beholder.fireballVelocity");
        bossBeholder.fireballDamage = appConfig->getFloat("boss beholder.fireballDamage");
        bossBeholder.laserChargeTime = appConfig->getFloat("boss beholder.laserChargeTime");
        bossBeholder.laserDamage = appConfig->getFloat("boss beholder.laserDamage");
        bossBeholder.laserImpulse = appConfig->getFloat("boss beholder.laserImpulse");
        bossBeholder.napalmDamage = appConfig->getFloat("boss beholder.napalmDamage");
        bossBeholder.napalmVelocity = appConfig->getFloat("boss beholder.napalmVelocity");
    }

    Settings::Settings()
    {
    }

    Settings::~Settings()
    {
    }

    void Settings::init(const AppConfigPtr& appConfig,
        const AppConfigPtr& easyAppConfig,
        const AppConfigPtr& hardAppConfig)
    {
        std::vector<std::string> subKeys;

        /*
         * general.
         */

#ifdef _WIN32
        assets = appConfig->getString(".assets");
#endif
        developer = appConfig->getInt(".developer");
        viewWidth = appConfig->getInt(".viewWidth");
        viewHeight = appConfig->getInt(".viewHeight");
        gameHeight = appConfig->getFloat(".gameHeight");
        layoutWidth = appConfig->getInt(".layoutWidth");
        layoutHeight = appConfig->getInt(".layoutHeight");
        profileReportTimeoutMs = appConfig->getInt(".profileReportTimeoutMs");
        debugPath = appConfig->getBool(".debug.path");

        float tmp = appConfig->getFloat(".maxFPS");

        if (tmp > 0) {
            minRenderDt = std::floor(1000000.0f / tmp);
        } else {
            minRenderDt = 0;
        }

        viewAspect = static_cast<float>(viewWidth) / viewHeight;
        videoMode = -1;
        msaaMode = -1;
        vsync = false;
        fullscreen = false;
        trilinearFilter = false;
        viewX = 0;
        viewY = 0;
        gamma = 1.0f;

        dumpAssetList = appConfig->getBool(".dumpAssetList");
        debugKeys = appConfig->getBool(".debugKeys");
        atLeastGems = appConfig->getInt(".atLeastGems");
        allLevelsAccessible = appConfig->getBool(".allLevelsAccessible");

        subKeys.clear();

        subKeys.push_back("normal");
        subKeys.push_back("easy");
        subKeys.push_back("hard");

        int skillIndex = appConfig->getStringIndex(".skill", subKeys);

        if (skillIndex == 0) {
            skill = SkillNormal;
        } else if (skillIndex == 1) {
            skill = SkillEasy;
        } else {
            skill = SkillHard;
        }

        steamApiRequired = appConfig->getBool(".steamApiRequired");
        steamResetStats = appConfig->getBool(".steamResetStats");

        subKeys.clear();

        subKeys.push_back("en");
        subKeys.push_back("ru");

        int languageIndex = appConfig->getStringIndex(".language", subKeys);

        if (languageIndex == 0) {
            language = LanguageEnglish;
        } else {
            language = LanguageRussian;
        }

        subKeys = appConfig->getSubKeys(".winVideoMode");

        if (subKeys.empty()) {
            LOG4CPLUS_ERROR(logger(), "Config values for \"winVideoMode\" not found");
        }

        for (std::vector<std::string>::const_iterator it = subKeys.begin();
             it != subKeys.end();
             ++it) {
            b2Vec2 tmp = appConfig->getVec2(std::string(".winVideoMode.") + *it);
            winVideoModes.insert(VideoMode(tmp.x, tmp.y));
        }

        /*
         * object pool.
         */

        objectPool.maxBytes = appConfig->getInt("object pool.maxBytes");
        objectPool.warnThreshold = appConfig->getInt("object pool.warnThreshold");
        objectPool.reportTimeoutMs = appConfig->getInt("object pool.reportTimeoutMs");

        /*
         * touchscreen.
         */

        touchScreen.enabled = appConfig->getBool("touchscreen.enabled");
        touchScreen.movePadding = appConfig->getVec2("touchscreen.move.padding");
        touchScreen.moveRadius = appConfig->getFloat("touchscreen.move.radius");
        touchScreen.moveHandleRadius = appConfig->getFloat("touchscreen.move.handleRadius");
        touchScreen.primaryPadding = appConfig->getVec2("touchscreen.primary.padding");
        touchScreen.primaryRadius = appConfig->getFloat("touchscreen.primary.radius");
        touchScreen.primaryHandleRadius = appConfig->getFloat("touchscreen.primary.handleRadius");
        touchScreen.primaryTextSize = appConfig->getFloat("touchscreen.primary.textSize");
        touchScreen.primaryTextPos = appConfig->getVec2("touchscreen.primary.textPos");
        touchScreen.secondaryPadding = appConfig->getVec2("touchscreen.secondary.padding");
        touchScreen.secondaryRadius = appConfig->getFloat("touchscreen.secondary.radius");
        touchScreen.secondaryHandleRadius = appConfig->getFloat("touchscreen.secondary.handleRadius");
        touchScreen.secondaryTextSize = appConfig->getFloat("touchscreen.secondary.textSize");
        touchScreen.secondaryTextPos = appConfig->getVec2("touchscreen.secondary.textPos");
        touchScreen.switchPadding = appConfig->getVec2("touchscreen.switch.padding");
        touchScreen.switchRadius = appConfig->getFloat("touchscreen.switch.radius");
        touchScreen.slotRadius = appConfig->getFloat("touchscreen.slot.radius");
        touchScreen.slotDir = appConfig->getVec2("touchscreen.slot.dir");
        touchScreen.slotPrimaryPadding = appConfig->getVec2("touchscreen.slot.primaryPadding");
        touchScreen.slotSecondaryPadding = appConfig->getVec2("touchscreen.slot.secondaryPadding");
        touchScreen.slotTextSize = appConfig->getFloat("touchscreen.slot.textSize");
        touchScreen.slotTextPos = appConfig->getVec2("touchscreen.slot.textPos");
        touchScreen.alpha = appConfig->getFloat("touchscreen.alpha");
        touchScreen.switchAlpha = appConfig->getFloat("touchscreen.switchAlpha");

        /*
         * PC.
         */

        pc.primaryPadding = appConfig->getVec2("PC.primary.padding");
        pc.primaryRadius = appConfig->getFloat("PC.primary.radius");
        pc.primaryTextSize = appConfig->getFloat("PC.primary.textSize");
        pc.primaryTextPos = appConfig->getVec2("PC.primary.textPos");
        pc.secondaryPadding = appConfig->getVec2("PC.secondary.padding");
        pc.secondaryRadius = appConfig->getFloat("PC.secondary.radius");
        pc.secondaryTextSize = appConfig->getFloat("PC.secondary.textSize");
        pc.secondaryTextPos = appConfig->getVec2("PC.secondary.textPos");
        pc.alpha = appConfig->getFloat("PC.alpha");

        /*
         * physics.
         */

        physics.fixedTimestep = appConfig->getFloat("physics.fixedTimestep");
        physics.maxSteps = appConfig->getInt("physics.maxSteps");
        physics.debugShape = appConfig->getBool("physics.debug.shape");
        physics.debugJoint = appConfig->getBool("physics.debug.joint");
        physics.debugAABB = appConfig->getBool("physics.debug.aabb");
        physics.debugPair = appConfig->getBool("physics.debug.pair");
        physics.debugCenterOfMass = appConfig->getBool("physics.debug.centerOfMass");
        physics.slowmoFactor = appConfig->getFloat("physics.slowmoFactor");

        /*
         * light.
         */

        light.numBlur = appConfig->getInt("light.numBlur");
        light.lightmapScale = appConfig->getInt("light.lightmapScale");
        light.numRays = appConfig->getInt("light.numRays");
        light.ambient = appConfig->getColor("light.ambient");
        light.gammaCorrection = appConfig->getBool("light.gammaCorrection");
        light.edgeSmooth = appConfig->getFloat("light.edgeSmooth");

        /*
         * audio.
         */

        audio.enabled = appConfig->getBool("audio.enabled");
        audio.maxSources = appConfig->getInt("audio.maxSources");
        audio.soundCacheSize = appConfig->getInt("audio.soundCacheSize");
        audio.numStreamBuffers = appConfig->getInt("audio.numStreamBuffers");
        audio.streamBufferSize = appConfig->getInt("audio.streamBufferSize");

        subKeys = appConfig->getSubKeys("audio.factor");

        float audioMaxFactor = 1.0f;

        for (std::vector<std::string>::const_iterator it = subKeys.begin();
             it != subKeys.end(); ++it) {
            float factor = appConfig->getFloat(std::string("audio.factor.") + *it);
            audio.factors[*it + ".ogg"] = factor;
            if (factor > audioMaxFactor) {
                audioMaxFactor = factor;
            }
        }

        for (std::map<std::string, float>::iterator it = audio.factors.begin();
             it != audio.factors.end(); ++it) {
            it->second /= audioMaxFactor;
        }

        /*
         * letter box.
         */

        letterBox.distance = appConfig->getFloat("letter box.distance");
        letterBox.duration = appConfig->getFloat("letter box.duration");

        /*
         * HUD.
         */

        hud.enabled = appConfig->getBool("HUD.enabled");

        /*
         * crosshair.
         */

        crosshair.enabled = appConfig->getBool("crosshair.enabled");

        /*
         * blaster shot.
         */

        blasterShot.player[0] = appConfig->getColor("blaster shot.playerColor");
        blasterShot.player[1] = appConfig->getColor("blaster shot.playerLightColor");
        blasterShot.player[2] = appConfig->getColor("blaster shot.playerEffectColor1");
        blasterShot.player[3] = appConfig->getColor("blaster shot.playerEffectColor2");

        blasterShot.ally[0] = appConfig->getColor("blaster shot.allyColor");
        blasterShot.ally[1] = appConfig->getColor("blaster shot.allyLightColor");
        blasterShot.ally[2] = appConfig->getColor("blaster shot.allyEffectColor1");
        blasterShot.ally[3] = appConfig->getColor("blaster shot.allyEffectColor2");

        blasterShot.enemy[0] = appConfig->getColor("blaster shot.enemyColor");
        blasterShot.enemy[1] = appConfig->getColor("blaster shot.enemyLightColor");
        blasterShot.enemy[2] = appConfig->getColor("blaster shot.enemyEffectColor1");
        blasterShot.enemy[3] = appConfig->getColor("blaster shot.enemyEffectColor2");

        /*
         * bullet.
         */

        bullet.player[0] = appConfig->getColor("bullet.playerColor1");
        bullet.player[1] = appConfig->getColor("bullet.playerColor2");

        bullet.ally[0] = appConfig->getColor("bullet.allyColor1");
        bullet.ally[1] = appConfig->getColor("bullet.allyColor2");

        bullet.enemy[0] = appConfig->getColor("bullet.enemyColor1");
        bullet.enemy[1] = appConfig->getColor("bullet.enemyColor2");

        /*
         * dialog.
         */

        dialog.borderSize = appConfig->getFloat("dialog.borderSize");
        dialog.padding = appConfig->getFloat("dialog.padding");
        dialog.portraitSize = appConfig->getFloat("dialog.portraitSize");
        dialog.titleCharSize = appConfig->getFloat("dialog.titleCharSize");
        dialog.messageCharSize = appConfig->getFloat("dialog.messageCharSize");
        dialog.titlePlayerColor = appConfig->getColor("dialog.titlePlayerColor");
        dialog.titleAllyColor = appConfig->getColor("dialog.titleAllyColor");
        dialog.titleEnemyColor = appConfig->getColor("dialog.titleEnemyColor");
        dialog.messageColor = appConfig->getColor("dialog.messageColor");
        dialog.bgColor = appConfig->getColor("dialog.bgColor");
        dialog.letterTimeThreshold = appConfig->getFloat("dialog.letterTimeThreshold");
        dialog.blinkTimeThreshold = appConfig->getFloat("dialog.blinkTimeThreshold");

        /*
         * level completed.
         */

        levelCompleted.blackoutColor = appConfig->getColor("level completed.blackoutColor");
        levelCompleted.blackoutTime = appConfig->getFloat("level completed.blackoutTime");
        levelCompleted.charSize = appConfig->getFloat("level completed.charSize");
        levelCompleted.keyColor = appConfig->getColor("level completed.keyColor");
        levelCompleted.valueColor = appConfig->getColor("level completed.valueColor");
        levelCompleted.itemTimeout = appConfig->getFloat("level completed.itemTimeout");
        levelCompleted.buttonsTimeout = appConfig->getFloat("level completed.buttonsTimeout");

        /*
         * game over.
         */

        gameover.timeout = appConfig->getFloat("gameover.timeout");
        gameover.blackoutColor = appConfig->getColor("gameover.blackoutColor");
        gameover.blackoutTime = appConfig->getFloat("gameover.blackoutTime");
        gameover.buttonsTimeout = appConfig->getFloat("gameover.buttonsTimeout");

        /*
         * health bar.
         */

        healthBar.xOffset = appConfig->getFloat("healthbar.xOffset");
        healthBar.yOffset = appConfig->getFloat("healthbar.yOffset");
        healthBar.width = appConfig->getFloat("healthbar.width");
        healthBar.height = appConfig->getFloat("healthbar.height");

        /*
         * stamina bar.
         */

        staminaBar.xOffset = appConfig->getFloat("staminabar.xOffset");
        staminaBar.yOffset = appConfig->getFloat("staminabar.yOffset");
        staminaBar.width = appConfig->getFloat("staminabar.width");
        staminaBar.height = appConfig->getFloat("staminabar.height");

        /*
         * hermite path.
         */

        hermitePath.numIterations = appConfig->getInt("hermite path.numIterations");

        /*
         * tentacle.
         */

        tentacle.numIterations = appConfig->getInt("tentacle.numIterations");
        tentacle.step = appConfig->getFloat("tentacle.step");

        /*
         * skill stuff.
         */

        skillSettings[SkillEasy].skillInit(easyAppConfig);
        skillSettings[SkillNormal].skillInit(appConfig);
        skillSettings[SkillHard].skillInit(hardAppConfig);

        setSkill(skill);
    }

    void Settings::setSkill(Skill value)
    {
        skill = value;

        SkillSettings& ss = *this;

        ss = skillSettings[skill];
    }
}
