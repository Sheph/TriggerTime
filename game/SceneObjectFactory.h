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

#ifndef _SCENEOBJECTFACTORY_H_
#define _SCENEOBJECTFACTORY_H_

#include "af/Types.h"
#include "af/Single.h"
#include "SceneObject.h"
#include "AnimationComponent.h"
#include "WeaponComponent.h"
#include "PowerupComponent.h"
#include "Inventory.h"

namespace af
{
    class SceneObjectFactory : public Single<SceneObjectFactory>
    {
    public:
        SceneObjectFactory();
        ~SceneObjectFactory();

        bool init();

        void shutdown();

        void setupGarbage(const SceneObjectPtr& obj, bool cancelCollisions);

        SceneObjectPtr createDummy();

        SceneObjectPtr createPlayer();

        SceneObjectPtr createBackground(const std::string& image,
                                        float width,
                                        float height,
                                        const b2Vec2& velocity,
                                        int zOrder);

        SceneObjectPtr createShroomCannonRed1();

        SceneObjectPtr createShroomCannonRed1Missile(float damage);

        SceneObjectPtr createExplosion1(int zOrder);

        SceneObjectPtr createExplosion2(int zOrder);

        SceneObjectPtr createExplosion3(int zOrder);

        SceneObjectPtr createExplosion4(int zOrder);

        SceneObjectPtr createExplosion5(int zOrder);

        SceneObjectPtr createExplosion6(int zOrder);

        SceneObjectPtr createBlasterShot(SceneObjectType type, float damage, bool twisted, float velocity);

        SceneObjectPtr createShroomGuardianRed1();

        SceneObjectPtr createShroomGuardianRed2();

        SceneObjectPtr createJake();

        SceneObjectPtr createShroomSnake();

        SceneObjectPtr createRope(float length);

        SceneObjectPtr createBlocker(float width, float height);

        SceneObjectPtr createTetrobot();

        SceneObjectPtr createTetrobotGibbed();

        SceneObjectPtr createTetrobot2();

        SceneObjectPtr createTetrobot2Gibbed();

        ComponentPtr createLaser(const b2Vec2& pos,
            float angle, float length, float damage, int zOrder);

        SceneObjectPtr createTetrocrab();

        SceneObjectPtr createTetrocrab2();

        SceneObjectPtr createTetrocrab3();

        SceneObjectPtr createTetrocrab4();

        SceneObjectPtr createTetrocrabGibbed(int i);

        SceneObjectPtr createSpawner1();

        AnimationComponentPtr createLightningAnimation();

        SceneObjectPtr createScorp();

        SceneObjectPtr createScorp2();

        SceneObjectPtr createScorp3();

        SceneObjectPtr createScorp4();

        SceneObjectPtr createFireball(SceneObjectType type, float damage);

        SceneObjectPtr createTeleport1();

        SceneObjectPtr createPowerupHealth(float timeout, float amount);

        SceneObjectPtr createPowerupGem(float timeout);

        SceneObjectPtr createPowerupInventory(InventoryItem item);

        SceneObjectPtr createPowerupHelmet();

        SceneObjectPtr createPowerupAmmo(WeaponType type);

        SceneObjectPtr createPowerupWeapon(WeaponType type, float amount);

        SceneObjectPtr createPowerupBackpack();

        SceneObjectPtr createPowerupEarpiece();

        AnimationComponentPtr createArrowAnimation();

        SceneObjectPtr createBlood1(float height, int zOrder);

        ComponentPtr createInvulnerability(float radius, float duration, int zOrder);

        SceneObjectPtr createBloodStain1(float height, int zOrder, bool deadbody = false);

        SceneObjectPtr createBloodStain2(float height, int zOrder, bool deadbody = false);

        SceneObjectPtr createWeaponStation(WeaponType weaponType, float minAmmo);

        SceneObjectPtr createBuckshot(SceneObjectType type, float damage);

        SceneObjectPtr createBarrel1();

        SceneObjectPtr createBarrel1Gibbed();

        SceneObjectPtr createBarrel2();

        SceneObjectPtr createBarrel2Gibbed();

        SceneObjectPtr createBarrel3();

        SceneObjectPtr createBarrel3Gibbed();

        SceneObjectPtr createTurret1();

        ComponentPtr createGoal(const SceneObjectPtr& forObject);

        ComponentPtr createGoal(float radius, const b2Vec2& offset);

        SceneObjectPtr createPyrobot();

        SceneObjectPtr createKeyl();

        SceneObjectPtr createLaserBlocker(const Color& color, float width, float length, int zOrder, UInt32 maskBits);

        SceneObjectPtr createCircleSensor(float radius, UInt32 maskBits);

        ComponentPtr createCircle(float radius, int numSectors, const Color& color, int zOrder);

        ComponentPtr createPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder);

        ComponentPtr createPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha);

        ComponentPtr createRockPlaceholder(float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha);

        ComponentPtr createVehiclePlaceholder(float radius, int numSectors, int zOrder, float alpha);

        ComponentPtr createAllyPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha);

        ComponentPtr createTimebomb(InventoryItem item, float height, int imageZOrder, int circleZOrder);

        SceneObjectPtr createBossSkull();

        SceneObjectPtr createSpider();

        SceneObjectPtr createBabySpider1();

        SceneObjectPtr createBabySpider2();

        SceneObjectPtr createSpiderNest(float probabilities[1], bool dead);

        SceneObjectPtr createWeaponTrap(WeaponType weaponType, SceneObjectType objectType);

        SceneObjectPtr createBossQueen();

        SceneObjectPtr createEnforcer1();

        SceneObjectPtr createEnforcer2();

        SceneObjectPtr createEnforcer3();

        SceneObjectPtr createBullet(SceneObjectType type, float height, float damage);

        SceneObjectPtr createSentry1(bool folded);

        SceneObjectPtr createSentry2(bool folded);

        SceneObjectPtr createSentryGibbed(int i);

        SceneObjectPtr createGorger1();

        SceneObjectPtr createRocket(SceneObjectType type, float explosionImpulse, float explosionDamage);

        SceneObjectPtr createGorgerGibbed();

        SceneObjectPtr createProxMine(SceneObjectType type);

        SceneObjectPtr createRoboArm();

        SceneObjectPtr createCrate(int i, float powerupProbabilities[PowerupTypeMax + 1], float ammoProbabilities[WeaponTypeMax + 1]);

        SceneObjectPtr createCrateGibbed(int i);

        SceneObjectPtr createOrbo();

        SceneObjectPtr createOrboGibbed();

        SceneObjectPtr createWarder();

        SceneObjectPtr createWarderGibbed();

        SceneObjectPtr createPlasma(SceneObjectType type, float damage);

        SceneObjectPtr createKeeper(bool folded);

        SceneObjectPtr createHealthStation(float amount);

        SceneObjectPtr createRodBundle();

        SceneObjectPtr createSummon1(bool haveSound, int zOrder);

        SceneObjectPtr createSummon2(bool haveSound, int zOrder);

        SceneObjectPtr createBossCoreProtector();

        SceneObjectPtr createToxicSplash1(int zOrder);

        SceneObjectPtr createToxicSplash2(int zOrder);

        SceneObjectPtr createToxicCloud1(float damage, float damageTimeout, int zOrder);

        SceneObjectPtr createMissileHit1(const SceneObjectPtr& obj, const b2Vec2& pos);

        SceneObjectPtr createMissileHit1(Material material, const b2Vec2& pos);

        SceneObjectPtr createAcidWorm1(bool folded, bool foldable, float unfoldRadius);

        SceneObjectPtr createAcidball(SceneObjectType type);

        SceneObjectPtr createCentipede1();

        SceneObjectPtr createSpit(int i, SceneObjectType type, float damage, float height, const Color& color);

        SceneObjectPtr createBeetle1(bool shoot, bool split);

        SceneObjectPtr createBeetle2();

        SceneObjectPtr createBeetle1Gibbed(bool shoot, const Color& color);

        SceneObjectPtr createBoat();

        ComponentPtr script_createQuad(const std::string& image, float height, int zOrder);

        SceneObjectPtr createPowerGen(bool rusted);

        SceneObjectPtr createPowerGen2(bool rusted);

        SceneObjectPtr createBossSquid();

        SceneObjectPtr createBossSquidEye();

        SceneObjectPtr createMech();

        SceneObjectPtr createHomer();

        SceneObjectPtr createSeeker(SceneObjectType type, float explosionImpulse, float explosionDamage);

        SceneObjectPtr createHomerGibbed();

        SceneObjectPtr createEShield(SceneObjectType type);

        SceneObjectPtr createFakeMissile(SceneObjectType type);

        SceneObjectPtr createScientist();

        SceneObjectPtr createBossChopper();

        SceneObjectPtr createCreature(int i);

        SceneObjectPtr createGasCloud1(float delay, int zOrder);

        SceneObjectPtr createGasCloud1Inner(float delay, float offset, int zOrder);

        SceneObjectPtr createOctopus();

        SceneObjectPtr createWalker();

        SceneObjectPtr createSnake();

        SceneObjectPtr createBossBuddy();

        SceneObjectPtr createBuddyTongue();

        SceneObjectPtr createNapalm(float damage, bool haveSound);

        SceneObjectPtr createNapalm2(float damage, bool haveSound);

        SceneObjectPtr createBossNatan();

        SceneObjectPtr createPowerBeam(float radius, float length);

        SceneObjectPtr createGuardian();

        SceneObjectPtr createGuardian2();

        SceneObjectPtr createMilitary1();

        SceneObjectPtr createSarge1();

        SceneObjectPtr createSawer(float length, bool fixed);

        SceneObjectPtr createSawerSaw();

        SceneObjectPtr createSawerSawGibbed();

        bool makeDebris(const SceneObjectPtr& obj);

        SceneObjectPtr createPuzzleHint();

        SceneObjectPtr createBossBeholder();

    private:
        SceneObjectPtr createEnforcer(float life, float shootInterval, const Color& color,
            b2Vec2& missilePos, float& missileAngle, float& missileFlashDistance);

        SceneObjectPtr createShroomGuardianRed(float life, float turnSpeed, const Color& color);

        SceneObjectPtr createPlaceholderSensor(const std::string& image, bool imageFlip, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, UInt32 maskBits, float alpha);

        SceneObjectPtr createBabySpider(float scale, float life, float walkSpeed, float biteDamage, bool isSmall);

        SceneObjectPtr createSentry(bool enemy, bool folded, float life,
            float shootDamage,
            float shootTurnDuration,
            float shootLoopDelay,
            const std::string& base);

        SceneObjectPtr createBeetle(float scale, float walkSpeed, const Color& color, b2Vec2& missilePos, float& missileAngle);

        SceneObjectPtr createTetrocrabInternal(const std::string& assetPath, const Color& color, int idx, float life, float walkSpeed, float walkAccel, float turnSpeed);

        SceneObjectPtr createScorpInternal(const Color& color, const std::string& nameBase, float life, float walkSpeed);

        void makeDeadbody(const SceneObjectPtr& obj, float radius);
    };

    extern SceneObjectFactory sceneObjectFactory;
}

#endif
