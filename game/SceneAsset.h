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

#ifndef _SCENEASSET_H_
#define _SCENEASSET_H_

#include "af/RUBEWorld.h"
#include "Scene.h"
#include "SceneObject.h"
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace af
{
    class SceneAsset : boost::noncopyable
    {
    public:
        typedef std::vector<SceneObject*> Objects;
        typedef std::vector<JointProxy*> Joints;

        explicit SceneAsset(const RUBEWorldPtr& world);
        ~SceneAsset();

        void process(Scene* scene,
            const b2Vec2& posOffset = b2Vec2_zero,
            float angleOffset = 0.0f);

        inline Objects& objects() { return objects_; }

        inline Joints& joints() { return joints_; }

        inline Scene::Instances& instances() { return instances_; }

    private:
        struct TentaclePart
        {
            TentaclePart()
            : obj(NULL)
            {
            }

            TentaclePart(SceneObject* obj, const RUBEPolygonFixturePtr& fixture)
            : obj(obj), fixture(fixture)
            {
            }

            TentaclePart(SceneObject* obj, const RUBECircleFixturePtr& fixture)
            : obj(obj), circleFixture(fixture)
            {
            }

            SceneObject* obj;
            RUBEPolygonFixturePtr fixture;
            RUBECircleFixturePtr circleFixture;
        };

        struct Tentacle
        {
            std::map<int, TentaclePart> parts;
            std::string image;
            int zOrder;
            Color color;
            float width1;
            float width2;
            bool flip;
            int numIterations;
            float tension;
            float step;
        };

        typedef std::map<std::string, Tentacle> TentacleMap;

        typedef SceneObjectPtr (SceneAsset::* CreateObjectFn)(Scene* scene, const RUBEBodyPtr& body);
        typedef std::map<std::string, CreateObjectFn> CreateObjectFnMap;

        void processBodies(Scene* scene, const b2Vec2& posOffset,
            float angleOffset);

        SceneObjectPtr processBody(const RUBEBodyPtr& body, Scene* scene, const b2Vec2& posOffset,
            float angleOffset, const b2Transform& xf, const SceneObjectPtr& rootObj);

        void processJoints(Scene* scene);

        void processInstances();

        void processSpecialImages(const RUBEBodyPtr& body, SceneObject* obj);

        void processImages(const RUBEBodyPtr& body, SceneObject* obj);

        void processLight(const std::string& type, const RUBEImagePtr& image,
            SceneObject* obj);

        void processFixtures(const RUBEBodyPtr& body, RUBEBodyPtr& newBody, SceneObject* obj, bool huge);

        void processTentacles();

        bool special(const RUBEImagePtr& image);

        SceneObjectPtr createDummy(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPlayer(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createRock(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTerrain(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createShroomCannonRed1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSensor(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createShroomGuardianRed1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createShroomGuardianRed2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBlocker(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBlocker2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createGarbage(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createJake(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createHermitePath(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createShroomSnake(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTetrobot(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTetrobot2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTetrocrab(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSpawner1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createScorp(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTeleport1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createGoalArea(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createHintArea(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createWeaponStation(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBarrel1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBarrel2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createTurret1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPyrobot(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createKeyl(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createLaserBlocker(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupInventory(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPlaceholder(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createRockPlaceholder(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createVehiclePlaceholder(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createAllyPlaceholder(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossSkull(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSpider(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createInstance(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupHelmet(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBabySpider1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBabySpider2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSpiderNest(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createWeaponTrap(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossQueen(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createEnforcer1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createEnforcer2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createEnforcer3(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSentry1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSentry2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createGorger1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupAmmo(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createRoboArm(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCrate(Scene* scene, const RUBEBodyPtr& body, int i);

        SceneObjectPtr createCrate1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCrate2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCrate3(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCrate4(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupWeapon(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createOrbo(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createWarder(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createKeeper(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupBackpack(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerupEarpiece(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createHealthStation(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createRodBundle(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossCoreProtector(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBarrel3(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createAcidWorm1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCentipede1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBeetle1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBeetle2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBoat(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerGen(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createPowerGen2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossSquid(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createMech(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createHomer(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createScientist(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossChopper(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createCreature(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createOctopus(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createWalker(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSnake(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossBuddy(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossNatan(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createGuardian(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createGuardian2(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createMilitary1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSarge1(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createEnemyBuilding(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createSawer(Scene* scene, const RUBEBodyPtr& body);

        SceneObjectPtr createBossBeholder(Scene* scene, const RUBEBodyPtr& body);

        RUBEWorldPtr world_;
        CreateObjectFnMap createObjectFnMap_;
        Objects objects_;
        Joints joints_;
        Scene::Instances instances_;
        TentacleMap tentacles_;

        b2Transform xf_;
    };

    typedef boost::shared_ptr<SceneAsset> SceneAssetPtr;
}

#endif
