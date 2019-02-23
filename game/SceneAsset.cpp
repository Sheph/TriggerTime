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

#include "SceneAsset.h"
#include "SceneObjectFactory.h"
#include "AssetManager.h"
#include "Logger.h"
#include "PhysicsBodyComponent.h"
#include "RenderPolygonsComponent.h"
#include "RenderTerrainComponent.h"
#include "RenderStripeComponent.h"
#include "RenderQuadComponent.h"
#include "RenderChainComponent.h"
#include "CollisionSensorComponent.h"
#include "PathComponent.h"
#include "HermitePath.h"
#include "PointLight.h"
#include "ConeLight.h"
#include "LineLight.h"
#include "RingLight.h"
#include "LightComponent.h"
#include "GoalAreaComponent.h"
#include "TargetableComponent.h"
#include "PlaceholderComponent.h"
#include "RenderProjComponent.h"
#include "PowerupComponent.h"
#include "DummyComponent.h"
#include "RenderTentacleComponent.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "PuzzleHintAreaComponent.h"
#include <log4cplus/ndc.h>
#include <boost/make_shared.hpp>

namespace af
{
    struct Stripe
    {
        std::vector<Points> polygons;
        RenderStripeComponent::Side firstPolygonSide;
        RenderStripeComponent::Side baseSide;
        std::string image;
        float imageWidth;
        int zOrder;
        bool huge;
    };

    typedef std::map<std::string, Stripe> Stripes;

    struct Terrain
    {
        Terrain()
        : imageWidth(0.0f),
          imageHeight(0.0f),
          zOrder(0),
          stencilSet(false),
          stencilUse(false)
        {
        }

        Points points;
        std::vector<Points> holes;
        std::string image;
        float imageWidth;
        float imageHeight;
        int zOrder;
        bool stencilSet;
        StencilOp stencilFail;
        StencilOp stencilPass;
        bool stencilUse;
        StencilFunc stencilFunc;
        UInt32 stencilRef;
        Color color;
    };

    typedef std::map<std::string, Terrain> Terrains;

    struct HermiteStripe
    {
        RenderStripeComponent::Side firstPoint;
        float tension;
        float radius;
        float step;
        Points points;
        RenderStripeComponent::Side baseSide;
        std::string image;
        int zOrder;
        Color color;
        bool huge;
    };

    typedef std::map<std::string, HermiteStripe> HermiteStripes;

    struct Chain
    {
        PathComponent::Side firstPoint;
        float tension;
        float step;
        std::pair<float, float> heights;
        std::pair<float, float> angles;
        Points points;
        std::string image;
        int zOrder;
    };

    typedef std::vector<Chain> Chains;

    struct PolygonGroup
    {
        std::vector<Points> polygons;
        std::string image;
        float imageWidth;
        float imageHeight;
        int zOrder;
        Color color;
    };

    typedef std::map<std::string, PolygonGroup> PolygonGroups;

    struct LineStripe
    {
        RenderStripeComponent::Side firstPoint;
        float radius;
        Points points;
        RenderStripeComponent::Side baseSide;
        std::string image;
        int zOrder;
        Color color;
        bool huge;
    };

    typedef std::map<std::string, LineStripe> LineStripes;

    struct Proj
    {
        std::string name;
        Points points;
        std::string image;
        int zOrder;
        Color color;
        bool fixedPos;
        bool flip;
        bool visible;
        RenderStripeComponent::Side firstPoint;
    };

    typedef std::vector<Proj> Projs;

    class SideComparer: public std::binary_function< std::pair<int, b2Vec2>, std::pair<int, b2Vec2>, bool>
    {
    public:
        explicit SideComparer(RenderStripeComponent::Side side)
        : side_(side)
        {
        }

        ~SideComparer() {}

        bool operator()(const std::pair<int, b2Vec2>& l, const std::pair<int, b2Vec2>& r) const
        {
            switch (side_) {
            case RenderStripeComponent::SideLeft:
                return (l.second.x == r.second.x) ? l.second.y > r.second.y : l.second.x < r.second.x;
            case RenderStripeComponent::SideRight:
                return (l.second.x == r.second.x) ? l.second.y < r.second.y : l.second.x > r.second.x;
            case RenderStripeComponent::SideTop:
                return (l.second.y == r.second.y) ? l.second.x > r.second.x : l.second.y > r.second.y;
            case RenderStripeComponent::SideBottom:
                return (l.second.y == r.second.y) ? l.second.x < r.second.x : l.second.y < r.second.y;
            default:
                assert(false);
                return false;
            }
        }

    private:
        RenderStripeComponent::Side side_;
    };

    SceneAsset::SceneAsset(const RUBEWorldPtr& world)
    : world_(world)
    {
        createObjectFnMap_.insert(std::make_pair("dummy", &SceneAsset::createDummy));
        createObjectFnMap_.insert(std::make_pair("player", &SceneAsset::createPlayer));
        createObjectFnMap_.insert(std::make_pair("rock", &SceneAsset::createRock));
        createObjectFnMap_.insert(std::make_pair("terrain", &SceneAsset::createTerrain));
        createObjectFnMap_.insert(std::make_pair("shroom_cannon_red1", &SceneAsset::createShroomCannonRed1));
        createObjectFnMap_.insert(std::make_pair("sensor", &SceneAsset::createSensor));
        createObjectFnMap_.insert(std::make_pair("shroom_guardian_red1", &SceneAsset::createShroomGuardianRed1));
        createObjectFnMap_.insert(std::make_pair("shroom_guardian_red2", &SceneAsset::createShroomGuardianRed2));
        createObjectFnMap_.insert(std::make_pair("blocker", &SceneAsset::createBlocker));
        createObjectFnMap_.insert(std::make_pair("blocker2", &SceneAsset::createBlocker2));
        createObjectFnMap_.insert(std::make_pair("garbage", &SceneAsset::createGarbage));
        createObjectFnMap_.insert(std::make_pair("jake", &SceneAsset::createJake));
        createObjectFnMap_.insert(std::make_pair("hermite_path", &SceneAsset::createHermitePath));
        createObjectFnMap_.insert(std::make_pair("shroom_snake", &SceneAsset::createShroomSnake));
        createObjectFnMap_.insert(std::make_pair("tetrobot", &SceneAsset::createTetrobot));
        createObjectFnMap_.insert(std::make_pair("tetrobot2", &SceneAsset::createTetrobot2));
        createObjectFnMap_.insert(std::make_pair("tetrocrab", &SceneAsset::createTetrocrab));
        createObjectFnMap_.insert(std::make_pair("spawner1", &SceneAsset::createSpawner1));
        createObjectFnMap_.insert(std::make_pair("scorp", &SceneAsset::createScorp));
        createObjectFnMap_.insert(std::make_pair("teleport1", &SceneAsset::createTeleport1));
        createObjectFnMap_.insert(std::make_pair("goal_area", &SceneAsset::createGoalArea));
        createObjectFnMap_.insert(std::make_pair("hint_area", &SceneAsset::createHintArea));
        createObjectFnMap_.insert(std::make_pair("weapon_station", &SceneAsset::createWeaponStation));
        createObjectFnMap_.insert(std::make_pair("barrel1", &SceneAsset::createBarrel1));
        createObjectFnMap_.insert(std::make_pair("barrel2", &SceneAsset::createBarrel2));
        createObjectFnMap_.insert(std::make_pair("turret1", &SceneAsset::createTurret1));
        createObjectFnMap_.insert(std::make_pair("pyrobot", &SceneAsset::createPyrobot));
        createObjectFnMap_.insert(std::make_pair("keyl", &SceneAsset::createKeyl));
        createObjectFnMap_.insert(std::make_pair("laser_blocker", &SceneAsset::createLaserBlocker));
        createObjectFnMap_.insert(std::make_pair("powerup_inventory", &SceneAsset::createPowerupInventory));
        createObjectFnMap_.insert(std::make_pair("placeholder", &SceneAsset::createPlaceholder));
        createObjectFnMap_.insert(std::make_pair("rock_placeholder", &SceneAsset::createRockPlaceholder));
        createObjectFnMap_.insert(std::make_pair("vehicle_placeholder", &SceneAsset::createVehiclePlaceholder));
        createObjectFnMap_.insert(std::make_pair("ally_placeholder", &SceneAsset::createAllyPlaceholder));
        createObjectFnMap_.insert(std::make_pair("boss_skull", &SceneAsset::createBossSkull));
        createObjectFnMap_.insert(std::make_pair("spider", &SceneAsset::createSpider));
        createObjectFnMap_.insert(std::make_pair("instance", &SceneAsset::createInstance));
        createObjectFnMap_.insert(std::make_pair("powerup_helmet", &SceneAsset::createPowerupHelmet));
        createObjectFnMap_.insert(std::make_pair("baby_spider1", &SceneAsset::createBabySpider1));
        createObjectFnMap_.insert(std::make_pair("baby_spider2", &SceneAsset::createBabySpider2));
        createObjectFnMap_.insert(std::make_pair("spider_nest", &SceneAsset::createSpiderNest));
        createObjectFnMap_.insert(std::make_pair("weapon_trap", &SceneAsset::createWeaponTrap));
        createObjectFnMap_.insert(std::make_pair("boss_queen", &SceneAsset::createBossQueen));
        createObjectFnMap_.insert(std::make_pair("enforcer1", &SceneAsset::createEnforcer1));
        createObjectFnMap_.insert(std::make_pair("enforcer2", &SceneAsset::createEnforcer2));
        createObjectFnMap_.insert(std::make_pair("enforcer3", &SceneAsset::createEnforcer3));
        createObjectFnMap_.insert(std::make_pair("sentry1", &SceneAsset::createSentry1));
        createObjectFnMap_.insert(std::make_pair("sentry2", &SceneAsset::createSentry2));
        createObjectFnMap_.insert(std::make_pair("gorger1", &SceneAsset::createGorger1));
        createObjectFnMap_.insert(std::make_pair("powerup_ammo", &SceneAsset::createPowerupAmmo));
        createObjectFnMap_.insert(std::make_pair("roboarm", &SceneAsset::createRoboArm));
        createObjectFnMap_.insert(std::make_pair("crate1", &SceneAsset::createCrate1));
        createObjectFnMap_.insert(std::make_pair("crate2", &SceneAsset::createCrate2));
        createObjectFnMap_.insert(std::make_pair("crate3", &SceneAsset::createCrate3));
        createObjectFnMap_.insert(std::make_pair("crate4", &SceneAsset::createCrate4));
        createObjectFnMap_.insert(std::make_pair("powerup_weapon", &SceneAsset::createPowerupWeapon));
        createObjectFnMap_.insert(std::make_pair("orbo", &SceneAsset::createOrbo));
        createObjectFnMap_.insert(std::make_pair("warder", &SceneAsset::createWarder));
        createObjectFnMap_.insert(std::make_pair("keeper", &SceneAsset::createKeeper));
        createObjectFnMap_.insert(std::make_pair("powerup_backpack", &SceneAsset::createPowerupBackpack));
        createObjectFnMap_.insert(std::make_pair("powerup_earpiece", &SceneAsset::createPowerupEarpiece));
        createObjectFnMap_.insert(std::make_pair("health_station", &SceneAsset::createHealthStation));
        createObjectFnMap_.insert(std::make_pair("rod_bundle", &SceneAsset::createRodBundle));
        createObjectFnMap_.insert(std::make_pair("boss_coreprotector", &SceneAsset::createBossCoreProtector));
        createObjectFnMap_.insert(std::make_pair("barrel3", &SceneAsset::createBarrel3));
        createObjectFnMap_.insert(std::make_pair("acidworm1", &SceneAsset::createAcidWorm1));
        createObjectFnMap_.insert(std::make_pair("centipede1", &SceneAsset::createCentipede1));
        createObjectFnMap_.insert(std::make_pair("beetle1", &SceneAsset::createBeetle1));
        createObjectFnMap_.insert(std::make_pair("beetle2", &SceneAsset::createBeetle2));
        createObjectFnMap_.insert(std::make_pair("boat", &SceneAsset::createBoat));
        createObjectFnMap_.insert(std::make_pair("powergen", &SceneAsset::createPowerGen));
        createObjectFnMap_.insert(std::make_pair("powergen2", &SceneAsset::createPowerGen2));
        createObjectFnMap_.insert(std::make_pair("boss_squid", &SceneAsset::createBossSquid));
        createObjectFnMap_.insert(std::make_pair("mech", &SceneAsset::createMech));
        createObjectFnMap_.insert(std::make_pair("homer", &SceneAsset::createHomer));
        createObjectFnMap_.insert(std::make_pair("scientist", &SceneAsset::createScientist));
        createObjectFnMap_.insert(std::make_pair("boss_chopper", &SceneAsset::createBossChopper));
        createObjectFnMap_.insert(std::make_pair("creature", &SceneAsset::createCreature));
        createObjectFnMap_.insert(std::make_pair("octopus", &SceneAsset::createOctopus));
        createObjectFnMap_.insert(std::make_pair("walker", &SceneAsset::createWalker));
        createObjectFnMap_.insert(std::make_pair("snake", &SceneAsset::createSnake));
        createObjectFnMap_.insert(std::make_pair("boss_buddy", &SceneAsset::createBossBuddy));
        createObjectFnMap_.insert(std::make_pair("boss_natan", &SceneAsset::createBossNatan));
        createObjectFnMap_.insert(std::make_pair("guardian", &SceneAsset::createGuardian));
        createObjectFnMap_.insert(std::make_pair("guardian2", &SceneAsset::createGuardian2));
        createObjectFnMap_.insert(std::make_pair("military1", &SceneAsset::createMilitary1));
        createObjectFnMap_.insert(std::make_pair("sarge1", &SceneAsset::createSarge1));
        createObjectFnMap_.insert(std::make_pair("enemy_building", &SceneAsset::createEnemyBuilding));
        createObjectFnMap_.insert(std::make_pair("sawer", &SceneAsset::createSawer));
        createObjectFnMap_.insert(std::make_pair("boss_beholder", &SceneAsset::createBossBeholder));
    }

    SceneAsset::~SceneAsset()
    {
    }

    void SceneAsset::process(Scene* scene, const b2Vec2& posOffset,
        float angleOffset)
    {
        log4cplus::NDCContextCreator ndc(world_->name());

        processBodies(scene, posOffset, angleOffset);
        processJoints(scene);
        processInstances();
        processTentacles();
    }

    void SceneAsset::processBodies(Scene* scene, const b2Vec2& posOffset,
        float angleOffset)
    {
        std::string rootBodyName = world_->stringProp("rootBody");

        b2Transform xf(posOffset, b2Rot(angleOffset));

        objects_.resize(world_->numBodies());

        SceneObjectPtr rootObj;
        int rootObjI = -1;

        if (!rootBodyName.empty()) {
            for (int i = 0; i < world_->numBodies(); ++i) {
                RUBEBodyPtr body = world_->body(i);
                if (body->name() == rootBodyName) {
                    std::ostringstream os;
                    os << "body " << i;
                    log4cplus::NDCContextCreator ndc(os.str());

                    rootObj = processBody(body, scene, posOffset, angleOffset, xf, SceneObjectPtr());

                    if (rootObj) {
                        objects_[i] = rootObj.get();
                    }

                    rootObjI = i;
                    break;
                }
            }
            if (!rootObj) {
                LOG4CPLUS_ERROR(logger(), "Root object \"" << rootBodyName << "\" not found, not doing any nesting");
            }
        }

        for (int i = 0; i < world_->numBodies(); ++i) {
            RUBEBodyPtr body = world_->body(i);
            if (i != rootObjI) {
                std::ostringstream os;
                os << "body " << i;
                log4cplus::NDCContextCreator ndc(os.str());

                SceneObjectPtr obj = processBody(body, scene, posOffset, angleOffset, xf, rootObj);
                if (obj) {
                    objects_[i] = obj.get();
                }
            }
        }
    }

    SceneObjectPtr SceneAsset::processBody(const RUBEBodyPtr& body, Scene* scene, const b2Vec2& posOffset,
        float angleOffset, const b2Transform& xf, const SceneObjectPtr& rootObj)
    {
        std::string bodyClass = body->stringProp("class");

        CreateObjectFnMap::const_iterator it =
            createObjectFnMap_.find(bodyClass);

        if (it == createObjectFnMap_.end()) {
            LOG4CPLUS_ERROR(logger(), "Unknown body class - \"" << bodyClass << "\", skipping");
            return SceneObjectPtr();
        }

        xf_ = xf;

        SceneObjectPtr obj = (this->*(it->second))(scene, body);

        if (!obj) {
            return SceneObjectPtr();
        }

        obj->setName(body->name());
        if (!obj->gravityGunAware()) {
            obj->setGravityGunAware(body->boolProp("gravityGunAware"));
        }

        processSpecialImages(body, obj.get());

        if (!body->bodyDef().active) {
            obj->setActiveRecursive(false);
        }

        if (!body->boolProp("visible", true)) {
            obj->setVisibleRecursive(false);
        }

        if (obj->freezeRadius() == 0.0f) {
            obj->setFreezeRadius(body->floatProp("freezeRadius"));
        }

        if (!obj->freezable()) {
            obj->setFreezable(body->boolProp("freezable"));
        }

        if (!obj->glassy()) {
            obj->setGlassy(body->boolProp("glassy"));
        }

        if (obj->freezePhysics()) {
            obj->setFreezePhysics(body->boolProp("freezePhysics", true));
        }

        if (obj->detourable()) {
            obj->setDetourable(body->boolProp("detourable", true));
        }

        obj->setCollisionImpulseMultiplier(
            body->floatProp("collisionImpulseMultiplier",
                obj->collisionImpulseMultiplier()));

        if (posOffset != b2Vec2_zero) {
            obj->setPosRecursive(b2Mul(xf, obj->pos()));
        }

        if (angleOffset != 0.0f) {
            obj->setAngleRecursive(obj->angle() + angleOffset);
        }

        if (rootObj) {
            rootObj->addObject(obj);
        } else {
            scene->addObject(obj);
        }

        return obj;
    }

    void SceneAsset::processJoints(Scene* scene)
    {
        joints_.resize(world_->numJoints());

        for (int i = 0; i < world_->numJoints(); ++i) {
            RUBEGearJointPtr gearJoint =
               boost::dynamic_pointer_cast<RUBEGearJoint>(world_->joint(i));

            if (!gearJoint) {
                b2JointDef& jointDef = world_->joint(i)->jointDef();

                jointDef.bodyA = objects_[world_->joint(i)->bodyAIndex()]->body();
                jointDef.bodyB = objects_[world_->joint(i)->bodyBIndex()]->body();

                joints_[i] = scene->addJoint(&jointDef, world_->joint(i)->name()).get();
            }
        }

        for (int i = 0; i < world_->numJoints(); ++i) {
            RUBEGearJointPtr gearJoint =
               boost::dynamic_pointer_cast<RUBEGearJoint>(world_->joint(i));

            if (gearJoint) {
                b2GearJointDef& jointDef = gearJoint->jointDef();

                jointDef.bodyA = objects_[gearJoint->bodyAIndex()]->body();
                jointDef.bodyB = objects_[gearJoint->bodyBIndex()]->body();
                jointDef.joint1 = joints_[gearJoint->joint1Index()]->joint();
                jointDef.joint2 = joints_[gearJoint->joint2Index()]->joint();

                joints_[i] = scene->addJoint(&jointDef, gearJoint->name()).get();
            }
        }
    }

    void SceneAsset::processInstances()
    {
        for (int i = 0; i < world_->numObjects(); ++i) {
            RUBEObjectPtr obj = world_->object(i);

            SceneInstancePtr& instance = instances_.insert(std::make_pair(obj->name(),
                boost::make_shared<SceneInstance>(obj->pos(), obj->angle())))->second;

            for (RUBEObject::Bodies::const_iterator it = obj->bodies().begin();
                 it != obj->bodies().end(); ++it) {
                instance->objects().push_back(objects_[*it]->shared_from_this());
            }

            for (RUBEObject::Joints::const_iterator it = obj->joints().begin();
                 it != obj->joints().end(); ++it) {
                instance->joints().push_back(joints_[*it]->sharedThis());
            }
        }
    }

    void SceneAsset::processSpecialImages(const RUBEBodyPtr& body, SceneObject* obj)
    {
        for (int i = 0; i < body->numImages(); ++i) {
            RUBEImagePtr image = body->image(i);

            std::string peffect = image->stringProp("peffect");
            std::string light = image->stringProp("light");
            bool dummy = image->boolProp("dummy");

            if (!peffect.empty()) {
                ParticleEffectComponentPtr component =
                    assetManager.getParticleEffect(peffect, image->pos(),
                                                   image->angle());

                component->setZOrder(image->zOrder());

                component->resetEmit();

                obj->addComponent(component);
            } else if (!light.empty()) {
                processLight(light, image, obj);
            } else if (dummy) {
                ComponentPtr component =
                    boost::make_shared<DummyComponent>(
                        image->name(),
                        image->pos(), image->angle(),
                        image->height(), image->aspectScale() * image->height(),
                        image->zOrder());
                obj->addComponent(component);
            }
        }
    }

    void SceneAsset::processImages(const RUBEBodyPtr& body, SceneObject* obj)
    {
        for (int i = 0; i < body->numImages(); ++i) {
            RUBEImagePtr image = body->image(i);

            if (!special(image)) {
                DrawablePtr drawable = assetManager.getDrawable(image->fileName());

                RenderQuadComponentPtr component =
                    boost::make_shared<RenderQuadComponent>(
                        image->pos(), image->angle(),
                        image->aspectScale() * image->height() * drawable->image().aspect(), image->height(),
                        drawable,
                        image->zOrder());

                component->setName(image->name());
                component->setColor(image->color());
                component->setVisible(image->boolProp("visible", true));
                component->setFixedPos(image->boolProp("fixedPos"));
                component->setFlip(image->flip());

                if (image->boolProp("stencilSet")) {
                    component->stencilSet(
                        image->enumProp("stencilFail", StencilOpKeep),
                        image->enumProp("stencilPass", StencilOpKeep));
                }

                if (image->boolProp("stencilUse")) {
                    component->stencilUse(
                        image->enumProp("stencilFunc", StencilFuncLess),
                        image->intProp("stencilRef"));
                }

                component->setMask(image->boolProp("mask"));

                obj->addComponent(component);
            }
        }
    }

    void SceneAsset::processLight(const std::string& type, const RUBEImagePtr& image,
        SceneObject* obj)
    {
        LightPtr light;

        if (type == "point") {
            PointLightPtr tmp = boost::make_shared<PointLight>(image->name());

            tmp->setDistance(image->floatProp("distance"));

            light = tmp;
        } else if (type == "cone") {
            ConeLightPtr tmp = boost::make_shared<ConeLight>(image->name());

            tmp->setNearDistance(image->floatProp("nearDistance"));
            tmp->setFarDistance(image->floatProp("distance"));
            tmp->setConeAngle(deg2rad(image->floatProp("coneAngle", 45.0f)));

            light = tmp;
        } else if (type == "line") {
            LineLightPtr tmp = boost::make_shared<LineLight>(image->name());

            tmp->setLength(image->floatProp("length"));
            tmp->setDistance(image->floatProp("distance"));
            tmp->setBothWays(image->boolProp("bothWays"));

            light = tmp;
        } else if (type == "ring") {
            RingLightPtr tmp = boost::make_shared<RingLight>(image->name());

            tmp->setDistance(image->floatProp("distance"));
            tmp->setNearDistance(image->floatProp("nearDistance"));
            tmp->setFarDistance(image->floatProp("farDistance"));

            light = tmp;
        } else {
            LOG4CPLUS_ERROR(logger(), "Unknown light type - \"" << type << "\", skipping");
            return;
        }

        light->setDiffuse(image->boolProp("diffuse"));
        light->setPos(image->pos());
        light->setAngle(image->angle());
        light->setNumRays(image->intProp("numRays", settings.light.numRays));
        light->setColor(image->color());
        light->setVisible(image->boolProp("visible", true));
        light->setSoftLength(image->floatProp("softLength", 2.5f));
        light->setXray(image->boolProp("xray", false));
        light->setDynamic(image->boolProp("dynamic", true));
        light->setReverse(image->boolProp("reverse"));
        light->setIntensity(image->floatProp("intensity", 1.0f));
        light->setNearOffset(image->floatProp("nearOffset"));
        light->setHitBlockers(image->boolProp("hitBlockers"));

        LightComponentPtr component = obj->findComponent<LightComponent>();

        if (!component) {
            component = boost::make_shared<LightComponent>();
            obj->addComponent(component);
        }

        component->attachLight(light);
    }

    void SceneAsset::processFixtures(const RUBEBodyPtr& body, RUBEBodyPtr& newBody, SceneObject* obj, bool huge)
    {
        PolygonGroups polygonGroups;
        Stripes stripes;
        Terrains terrains;
        HermiteStripes hermiteStripes;
        Chains chains;
        LineStripes lineStripes;
        Projs projs;

        for (int i = 0; i < body->numFixtures(); ++i) {
            std::string fixtureClass = body->fixture(i)->stringProp("class");

            if (RUBEChainFixturePtr chainFixture =
                boost::dynamic_pointer_cast<RUBEChainFixture>(body->fixture(i))) {
                if (fixtureClass == "terrain_points") {
                    if (!chainFixture->fixtureDef().isSensor) {
                        newBody->addFixture(chainFixture);
                    }
                    Terrain& terrain = terrains[chainFixture->name()];
                    if (terrain.points.empty()) {
                        terrain.image = chainFixture->stringProp("image");
                        terrain.imageWidth = chainFixture->floatProp("image_width");
                        terrain.imageHeight = chainFixture->floatProp("image_height");
                        terrain.zOrder = chainFixture->intProp("zorder");
                        if (chainFixture->boolProp("stencilSet")) {
                            terrain.stencilSet = true;
                            terrain.stencilFail = chainFixture->enumProp("stencilFail", StencilOpKeep);
                            terrain.stencilPass = chainFixture->enumProp("stencilPass", StencilOpKeep);
                        }
                        if (chainFixture->boolProp("stencilUse")) {
                            terrain.stencilUse = true;
                            terrain.stencilFunc = chainFixture->enumProp("stencilFunc", StencilFuncLess);
                            terrain.stencilRef = chainFixture->intProp("stencilRef");
                        }
                        terrain.color = chainFixture->colorProp("color");
                    }
                    terrain.points.insert(terrain.points.end(),
                        &chainFixture->shape().m_vertices[0],
                        &chainFixture->shape().m_vertices[0] + chainFixture->shape().m_count - 1);
                } else if (fixtureClass == "terrain_hole") {
                    if (!chainFixture->fixtureDef().isSensor) {
                        newBody->addFixture(chainFixture);
                    }
                    Terrain& terrain = terrains[chainFixture->name()];
                    Points tmp(&chainFixture->shape().m_vertices[0],
                               &chainFixture->shape().m_vertices[0] + chainFixture->shape().m_count - 1);
                    terrain.holes.push_back(tmp);
                } else if (fixtureClass == "chain") {
                    chains.push_back(Chain());
                    Chain& chain = chains.back();
                    if (chain.points.empty()) {
                        chain.firstPoint = chainFixture->enumProp("first_point", PathComponent::SideLeft);
                        chain.tension = chainFixture->floatProp("tension");
                        chain.step = chainFixture->floatProp("step", 1.0f);
                        chain.heights.first = chainFixture->floatProp("height1");
                        chain.heights.second = chainFixture->floatProp("height2");
                        chain.angles.first = deg2rad(chainFixture->floatProp("angle1"));
                        chain.angles.second = deg2rad(chainFixture->floatProp("angle2"));
                        chain.image = chainFixture->stringProp("image");
                        chain.zOrder = chainFixture->intProp("zorder");
                    }
                    chain.points.insert(chain.points.end(),
                        &chainFixture->shape().m_vertices[0],
                        &chainFixture->shape().m_vertices[0] + chainFixture->shape().m_count);
                } else {
                    LOG4CPLUS_ERROR(logger(), "Unknown fixture class - \"" << fixtureClass << "\", skipping");
                }
            } else if (RUBEPolygonFixturePtr polygonFixture =
                       boost::dynamic_pointer_cast<RUBEPolygonFixture>(body->fixture(i))) {
                if (fixtureClass == "stripe") {
                    Stripe& stripe = stripes[polygonFixture->name()];
                    Points tmp(&polygonFixture->shape().m_vertices[0],
                               &polygonFixture->shape().m_vertices[0] + polygonFixture->shape().m_count);
                    if (stripe.polygons.empty()) {
                        stripe.firstPolygonSide = polygonFixture->enumProp("first_polygon", RenderStripeComponent::SideLeft);
                        stripe.baseSide = polygonFixture->enumProp("base", RenderStripeComponent::SideLeft);
                        stripe.image = polygonFixture->stringProp("image");
                        stripe.imageWidth = polygonFixture->floatProp("image_width");
                        stripe.zOrder = polygonFixture->intProp("zorder");
                        stripe.huge = polygonFixture->boolProp("huge", huge);
                    }
                    stripe.polygons.push_back(tmp);
                } else if (fixtureClass == "polygon") {
                    newBody->addFixture(polygonFixture);
                    PolygonGroup& pg = polygonGroups[polygonFixture->name()];
                    if (pg.polygons.empty()) {
                        pg.image = polygonFixture->stringProp("image");
                        pg.imageWidth = polygonFixture->floatProp("image_width");
                        pg.imageHeight = polygonFixture->floatProp("image_height");
                        pg.zOrder = polygonFixture->intProp("zorder");
                        pg.color = polygonFixture->colorProp("color");
                    }
                    pg.polygons.push_back(Points(&polygonFixture->shape().m_vertices[0],
                        &polygonFixture->shape().m_vertices[0] + polygonFixture->shape().m_count));
                } else if (fixtureClass == "proj") {
                    if (!polygonFixture->fixtureDef().isSensor) {
                        newBody->addFixture(polygonFixture);
                    }
                    projs.push_back(Proj());
                    Proj& proj = projs.back();
                    proj.name = polygonFixture->name();
                    proj.points.insert(proj.points.end(),
                        &polygonFixture->shape().m_vertices[0],
                        &polygonFixture->shape().m_vertices[0] + polygonFixture->shape().m_count);
                    proj.image = polygonFixture->stringProp("image");
                    proj.zOrder = polygonFixture->intProp("zorder");
                    proj.color = polygonFixture->colorProp("color");
                    proj.fixedPos = polygonFixture->boolProp("image_fixedPos");
                    proj.flip = polygonFixture->boolProp("image_flip");
                    proj.visible = polygonFixture->boolProp("image_visible", true);
                    proj.firstPoint = polygonFixture->enumProp("first_point", RenderStripeComponent::SideLeft);
                } else if (fixtureClass == "hermite_stripe") {
                    /*
                     * We're only interested in circle fixtures in hermite stripes, ignore polygons.
                     */
                } else if (fixtureClass == "line_stripe") {
                    /*
                     * We're only interested in circle fixtures in line stripes, ignore polygons.
                     */
                } else if (fixtureClass == "tentacle") {
                    newBody->addFixture(polygonFixture);
                    Tentacle& tentacle = tentacles_[polygonFixture->name()];
                    tentacle.parts[body->intProp("segId")] = TentaclePart(obj, polygonFixture);
                    if (body->intProp("segId") == 0) {
                        tentacle.image = polygonFixture->stringProp("image");
                        tentacle.zOrder = polygonFixture->intProp("zorder");
                        tentacle.color = polygonFixture->colorProp("color");
                        tentacle.width1 = polygonFixture->floatProp("width1");
                        tentacle.width2 = polygonFixture->floatProp("width2");
                        tentacle.flip = polygonFixture->boolProp("image_flip");
                        tentacle.numIterations = polygonFixture->intProp("num_iterations");
                        tentacle.tension = polygonFixture->floatProp("tension");
                        tentacle.step = polygonFixture->floatProp("step");
                    }
                } else {
                    LOG4CPLUS_ERROR(logger(), "Unknown fixture class - \"" << fixtureClass << "\", skipping");
                }
            } else if (RUBECircleFixturePtr circleFixture =
                       boost::dynamic_pointer_cast<RUBECircleFixture>(body->fixture(i))) {
                if (fixtureClass == "hermite_stripe") {
                    HermiteStripe& stripe = hermiteStripes[circleFixture->name()];
                    if (stripe.points.empty()) {
                        stripe.firstPoint = circleFixture->enumProp("first_point", RenderStripeComponent::SideLeft);
                        stripe.tension = circleFixture->floatProp("tension");
                        stripe.radius = circleFixture->shape().m_radius;
                        stripe.step = circleFixture->floatProp("step", 1.0f);
                        stripe.baseSide = circleFixture->enumProp("base", RenderStripeComponent::SideLeft);
                        stripe.image = circleFixture->stringProp("image");
                        stripe.zOrder = circleFixture->intProp("zorder");
                        stripe.color = circleFixture->colorProp("color");
                        stripe.huge = circleFixture->boolProp("huge", huge);
                    }
                    stripe.points.push_back(circleFixture->shape().m_p);
                } else if (fixtureClass == "polygon") {
                    newBody->addFixture(circleFixture);
                    PolygonGroup& pg = polygonGroups[circleFixture->name()];

                    int numSectors = circleFixture->intProp("num_sectors");

                    Points points(numSectors);

                    for (int i = 0; i < numSectors; ++i) {
                        float a = i * 2 * b2_pi / numSectors;
                        points[i] = circleFixture->shape().m_p +
                            circleFixture->shape().m_radius * b2Vec2(cosf(a), sinf(a));
                    }

                    if (pg.polygons.empty()) {
                        pg.image = circleFixture->stringProp("image");
                        pg.imageWidth = circleFixture->floatProp("image_width");
                        pg.imageHeight = circleFixture->floatProp("image_height");
                        pg.zOrder = circleFixture->intProp("zorder");
                        pg.color = circleFixture->colorProp("color");
                    }
                    pg.polygons.push_back(points);
                } else if (fixtureClass == "line_stripe") {
                    LineStripe& stripe = lineStripes[circleFixture->name()];
                    if (stripe.points.empty()) {
                        stripe.firstPoint = circleFixture->enumProp("first_point", RenderStripeComponent::SideLeft);
                        stripe.radius = circleFixture->shape().m_radius;
                        stripe.baseSide = circleFixture->enumProp("base", RenderStripeComponent::SideLeft);
                        stripe.image = circleFixture->stringProp("image");
                        stripe.zOrder = circleFixture->intProp("zorder");
                        stripe.color = circleFixture->colorProp("color");
                        stripe.huge = circleFixture->boolProp("huge", huge);
                    }
                    stripe.points.push_back(circleFixture->shape().m_p);
                } else if (fixtureClass == "tentacle") {
                    newBody->addFixture(circleFixture);
                    Tentacle& tentacle = tentacles_[circleFixture->name()];
                    tentacle.parts[body->intProp("segId")] = TentaclePart(obj, circleFixture);
                    if (body->intProp("segId") == 0) {
                        tentacle.image = circleFixture->stringProp("image");
                        tentacle.zOrder = circleFixture->intProp("zorder");
                        tentacle.color = circleFixture->colorProp("color");
                        tentacle.width1 = circleFixture->floatProp("width1");
                        tentacle.width2 = circleFixture->floatProp("width2");
                        tentacle.flip = circleFixture->boolProp("image_flip");
                        tentacle.numIterations = circleFixture->intProp("num_iterations");
                        tentacle.tension = circleFixture->floatProp("tension");
                        tentacle.step = circleFixture->floatProp("step");
                    }
                } else {
                    LOG4CPLUS_ERROR(logger(), "Unknown fixture class - \"" << fixtureClass << "\", skipping");
                }
            }
        }

        for (PolygonGroups::const_iterator it = polygonGroups.begin();
             it != polygonGroups.end();
             ++it) {
            if (!it->second.image.empty()) {
                RenderPolygonsComponentPtr component =
                    boost::make_shared<RenderPolygonsComponent>(it->second.polygons,
                        assetManager.getImage(it->second.image),
                        it->second.imageWidth, it->second.imageHeight,
                        it->second.zOrder);

                component->setName(it->first);

                component->setColor(it->second.color);

                obj->addComponent(component);
            }
        }

        for (Terrains::const_iterator it = terrains.begin();
             it != terrains.end();
             ++it) {
            if (!it->second.image.empty()) {
                RenderTerrainComponentPtr component =
                    boost::make_shared<RenderTerrainComponent>(it->second.points,
                        it->second.holes,
                        assetManager.getDrawable(it->second.image),
                        it->second.imageWidth, it->second.imageHeight,
                        body->bodyDef().position,
                        it->second.zOrder);

                if (it->second.stencilSet) {
                    component->stencilSet(
                        it->second.stencilFail,
                        it->second.stencilPass);
                }

                if (it->second.stencilUse) {
                    component->stencilUse(
                        it->second.stencilFunc,
                        it->second.stencilRef);
                }

                component->setName(it->first);

                component->setColor(it->second.color);

                obj->addComponent(component);
            }
        }

        for (Stripes::const_iterator it = stripes.begin();
             it != stripes.end();
             ++it) {
            RenderStripeComponentPtr component =
                boost::make_shared<RenderStripeComponent>(
                    it->second.huge,
                    it->second.firstPolygonSide,
                    it->second.baseSide,
                    it->second.polygons,
                    assetManager.getDrawable(it->second.image,
                                             Texture::WrapModeRepeat,
                                             Texture::WrapModeClamp),
                    it->second.imageWidth,
                    it->second.zOrder);

            component->setName(it->first);

            obj->addComponent(component);
        }

        for (HermiteStripes::const_iterator it = hermiteStripes.begin();
             it != hermiteStripes.end();
             ++it) {
            b2Vec2 p1 = it->second.points.front();
            b2Vec2 p2 = it->second.points.back();
            bool reverse = false;

            switch (it->second.firstPoint) {
            case RenderStripeComponent::SideLeft:
                reverse = p1.x > p2.x;
                break;
            case RenderStripeComponent::SideRight:
                reverse = p1.x < p2.x;
                break;
            case RenderStripeComponent::SideTop:
                reverse = p1.y < p2.y;
                break;
            case RenderStripeComponent::SideBottom:
                reverse = p1.y > p2.y;
                break;
            default:
                assert(false);
                break;
            }

            HermitePathPtr hermitePath =
                boost::make_shared<HermitePath>(settings.hermitePath.numIterations,
                                                it->second.tension);

            if (reverse) {
                for (size_t i = 0; i < it->second.points.size(); ++i) {
                    hermitePath->add(it->second.points[it->second.points.size() - i - 1]);
                }
            } else {
                hermitePath->add(&it->second.points[0], it->second.points.size());
            }

            RenderStripeComponentPtr component =
                boost::make_shared<RenderStripeComponent>(
                    it->second.huge,
                    it->second.firstPoint,
                    it->second.baseSide,
                    hermitePath,
                    it->second.radius,
                    it->second.step,
                    assetManager.getDrawable(it->second.image,
                                             Texture::WrapModeRepeat,
                                             Texture::WrapModeClamp),
                    it->second.zOrder);

            component->setName(it->first);

            component->setColor(it->second.color);

            obj->addComponent(component);
        }

        for (Chains::const_iterator it = chains.begin();
             it != chains.end();
             ++it) {
            b2Vec2 p1 = it->points.front();
            b2Vec2 p2 = it->points.back();
            bool reverse = false;

            switch (it->firstPoint) {
            case PathComponent::SideLeft:
                reverse = p1.x > p2.x;
                break;
            case PathComponent::SideRight:
                reverse = p1.x < p2.x;
                break;
            case PathComponent::SideTop:
                reverse = p1.y < p2.y;
                break;
            case PathComponent::SideBottom:
                reverse = p1.y > p2.y;
                break;
            default:
                assert(false);
                break;
            }

            HermitePathPtr hermitePath =
                boost::make_shared<HermitePath>(settings.hermitePath.numIterations,
                                                it->tension);

            if (reverse) {
                for (size_t i = 0; i < it->points.size(); ++i) {
                    hermitePath->add(it->points[it->points.size() - i - 1]);
                }
            } else {
                hermitePath->add(&it->points[0], it->points.size());
            }

            RenderChainComponentPtr component =
                boost::make_shared<RenderChainComponent>(
                    hermitePath,
                    it->step,
                    assetManager.getDrawable(it->image),
                    it->heights,
                    it->angles,
                    it->zOrder);

            obj->addComponent(component);
        }

        for (LineStripes::iterator it = lineStripes.begin();
             it != lineStripes.end();
             ++it) {
            b2Vec2 p1 = it->second.points.front();
            b2Vec2 p2 = it->second.points.back();
            bool reverse = false;

            switch (it->second.firstPoint) {
            case RenderStripeComponent::SideLeft:
                reverse = p1.x > p2.x;
                break;
            case RenderStripeComponent::SideRight:
                reverse = p1.x < p2.x;
                break;
            case RenderStripeComponent::SideTop:
                reverse = p1.y < p2.y;
                break;
            case RenderStripeComponent::SideBottom:
                reverse = p1.y > p2.y;
                break;
            default:
                assert(false);
                break;
            }

            if (reverse) {
                std::reverse(it->second.points.begin(), it->second.points.end());
            }

            RenderStripeComponentPtr component =
                boost::make_shared<RenderStripeComponent>(
                    it->second.huge,
                    it->second.firstPoint,
                    it->second.baseSide,
                    it->second.points,
                    it->second.radius,
                    assetManager.getDrawable(it->second.image,
                                             Texture::WrapModeRepeat,
                                             Texture::WrapModeClamp),
                    it->second.zOrder);

            component->setName(it->first);

            component->setColor(it->second.color);

            obj->addComponent(component);
        }

        for (Projs::const_iterator it = projs.begin();
             it != projs.end();
             ++it) {
            assert(it->points.size() == 4);
            if (it->points.size() != 4) {
                LOG4CPLUS_ERROR(logger(), "Proj fixture \"" << it->name << "\" has number of points != 4, skipping");
                continue;
            }

            std::pair<int, b2Vec2> tmp[4];

            for (size_t i = 0; i < sizeof(tmp)/sizeof(tmp[0]); ++i) {
                tmp[i].first = i;
                tmp[i].second = it->points[i];
            }

            std::sort(&tmp[0], &tmp[0] + 4, SideComparer(it->firstPoint));

            int first = tmp[0].first;

            b2Vec2 points[4];

            for (size_t i = 0; i < sizeof(tmp)/sizeof(tmp[0]); ++i) {
                points[i] = it->points[(first + i) % 4];
            }

            DrawablePtr drawable = assetManager.getDrawable(it->image);

            RenderProjComponentPtr component =
                boost::make_shared<RenderProjComponent>(&points[0],
                    drawable,
                    it->zOrder);

            component->setName(it->name);
            component->setColor(it->color);
            component->setVisible(it->visible);
            component->setFixedPos(it->fixedPos);
            component->setFlip(it->flip);

            obj->addComponent(component);
        }
    }

    void SceneAsset::processTentacles()
    {
        std::vector<SceneObjectPtr> objs;
        std::vector<RUBEPolygonFixturePtr> fixtures;
        std::vector<RUBECircleFixturePtr> circleFixtures;

        for (TentacleMap::const_iterator it = tentacles_.begin(); it != tentacles_.end(); ++it) {
            objs.clear();
            fixtures.clear();
            circleFixtures.clear();

            for (std::map<int, TentaclePart>::const_iterator jt = it->second.parts.begin();
                jt != it->second.parts.end(); ++jt) {
                objs.push_back(jt->second.obj->shared_from_this());
                if (jt->second.fixture) {
                    fixtures.push_back(jt->second.fixture);
                } else {
                    circleFixtures.push_back(jt->second.circleFixture);
                }
            }

            RenderTentacleComponentPtr component;

            if (fixtures.empty()) {
                component = boost::make_shared<RenderTentacleComponent>(objs, circleFixtures,
                    assetManager.getDrawable(it->second.image,
                        Texture::WrapModeRepeat,
                        Texture::WrapModeClamp),
                    it->second.zOrder,
                    it->second.numIterations, it->second.tension,
                    it->second.step);
            } else {
                component = boost::make_shared<RenderTentacleComponent>(objs, fixtures,
                    assetManager.getDrawable(it->second.image,
                        Texture::WrapModeRepeat,
                        Texture::WrapModeClamp),
                    it->second.zOrder,
                    it->second.numIterations, it->second.tension,
                    it->second.step);
            }

            component->setFlip(it->second.flip);
            component->resetTimeline1(2);
            component->resetTimeline2(2);
            component->set1At(0, 0.0f, it->second.width1 / 2.0f);
            component->set1At(1, 1.0f, it->second.width2 / 2.0f);
            component->set2At(0, 0.0f, it->second.width1 / 2.0f);
            component->set2At(1, 1.0f, it->second.width2 / 2.0f);
            component->setColor(it->second.color);

            (*it->second.parts.begin()).second.obj->addComponent(component);
        }
    }

    bool SceneAsset::special(const RUBEImagePtr& image)
    {
        return !image->stringProp("peffect").empty() ||
               !image->stringProp("light").empty() ||
               image->boolProp("dummy");
    }

    SceneObjectPtr SceneAsset::createDummy(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(body->bodyDef());

        return obj;
    }

    SceneObjectPtr SceneAsset::createPlayer(Scene* scene, const RUBEBodyPtr& body)
    {
        scene->setRespawnPoint(
            b2Transform(body->bodyDef().position, b2Rot(body->bodyDef().angle)));

        return SceneObjectPtr();
    }

    SceneObjectPtr SceneAsset::createRock(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeRock);

        obj->setBodyDef(newBody->bodyDef());

        processFixtures(body, newBody, obj.get(), false);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        processImages(body, obj.get());

        return obj;
    }

    SceneObjectPtr SceneAsset::createTerrain(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeTerrain);

        obj->setBodyDef(newBody->bodyDef());

        processFixtures(body, newBody, obj.get(), true);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        processImages(body, obj.get());

        return obj;
    }

    SceneObjectPtr SceneAsset::createShroomCannonRed1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createShroomCannonRed1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSensor(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(body->bodyDef());

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(body);

            obj->addComponent(component);
        }

        {
            ComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createShroomGuardianRed1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createShroomGuardianRed1();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createShroomGuardianRed2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createShroomGuardianRed2();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBlocker(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeBlocker);

        obj->setBodyDef(body->bodyDef());

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(body);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBlocker2(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeBlocker);

        obj->setBodyDef(newBody->bodyDef());

        processFixtures(body, newBody, obj.get(), false);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        processImages(body, obj.get());

        return obj;
    }

    SceneObjectPtr SceneAsset::createGarbage(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeGarbage);

        obj->setBodyDef(newBody->bodyDef());

        processFixtures(body, newBody, obj.get(), false);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        processImages(body, obj.get());

        sceneObjectFactory.setupGarbage(obj, body->boolProp("cancelCollisions"));

        return obj;
    }

    SceneObjectPtr SceneAsset::createJake(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createJake();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createHermitePath(Scene* scene, const RUBEBodyPtr& body)
    {
        PathComponent::Side firstPoint = PathComponent::SideLeft;
        float tension = 0.0f;
        Points points;
        int numIterations = 0;

        for (int i = 0; i < body->numFixtures(); ++i) {
            if (RUBEChainFixturePtr chainFixture =
                boost::dynamic_pointer_cast<RUBEChainFixture>(body->fixture(i))) {

                firstPoint = chainFixture->enumProp("first_point", PathComponent::SideLeft);
                tension = chainFixture->floatProp("tension");
                numIterations = chainFixture->intProp("num_iterations", settings.hermitePath.numIterations);

                points.clear();

                points.insert(points.end(),
                              chainFixture->shape().m_vertices,
                              chainFixture->shape().m_vertices + chainFixture->shape().m_count);

                break;
            } else if (RUBECircleFixturePtr circleFixture =
                       boost::dynamic_pointer_cast<RUBECircleFixture>(body->fixture(i))) {
                if (points.empty()) {
                    firstPoint = circleFixture->enumProp("first_point", PathComponent::SideLeft);
                    tension = circleFixture->floatProp("tension");
                    numIterations = circleFixture->intProp("num_iterations", settings.hermitePath.numIterations);
                }

                points.push_back(circleFixture->shape().m_p);
            }
        }

        if (points.empty()) {
            LOG4CPLUS_ERROR(logger(), "Unsupported fixtures in hermite path, skipping");

            return SceneObjectPtr();
        }

        b2Vec2 p1 = points.front();
        b2Vec2 p2 = points.back();
        bool reverse = false;

        switch (firstPoint) {
        case PathComponent::SideLeft:
            reverse = p1.x > p2.x;
            break;
        case PathComponent::SideRight:
            reverse = p1.x < p2.x;
            break;
        case PathComponent::SideTop:
            reverse = p1.y < p2.y;
            break;
        case PathComponent::SideBottom:
            reverse = p1.y > p2.y;
            break;
        default:
            assert(false);
            break;
        }

        HermitePathPtr hermitePath =
            boost::make_shared<HermitePath>(numIterations, tension);

        if (reverse) {
            for (size_t i = 0; i < points.size(); ++i) {
                hermitePath->add(points[points.size() - i - 1]);
            }
        } else {
            hermitePath->add(&points[0], points.size());
        }

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        PathComponentPtr component =
            boost::make_shared<PathComponent>();

        component->setPath(hermitePath);

        obj->addComponent(component);

        return obj;
    }

    SceneObjectPtr SceneAsset::createShroomSnake(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createShroomSnake();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createTetrobot(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createTetrobot();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createTetrobot2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createTetrobot2();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createTetrocrab(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createTetrocrab();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSpawner1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSpawner1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createScorp(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createScorp();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createTeleport1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createTeleport1();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createGoalArea(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        for (int i = 0; i < body->numFixtures(); ++i) {
            if (RUBEChainFixturePtr chainFixture =
                boost::dynamic_pointer_cast<RUBEChainFixture>(body->fixture(i))) {
                RUBEPolygonFixturePtr polygonFixture =
                    boost::make_shared<RUBEPolygonFixture>(body->fixture(i)->name());
                polygonFixture->fixtureDef() = body->fixture(i)->fixtureDef();
                polygonFixture->fixtureDef().shape = &polygonFixture->shape();
                polygonFixture->shape().Set(&chainFixture->shape().m_vertices[0],
                    chainFixture->shape().m_count - 1);
                newBody->addFixture(polygonFixture);
            }
        }

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(newBody->bodyDef());

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        GoalAreaComponentPtr thinkComponent =
            boost::make_shared<GoalAreaComponent>();

        obj->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            obj->addComponent(component);
        }

        for (int i = 0; i < body->numImages(); ++i) {
            RUBEImagePtr image = body->image(i);

            if (!special(image)) {
                thinkComponent->addGoal(obj->getWorldPoint(image->pos()));
            }
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createHintArea(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        for (int i = 0; i < body->numFixtures(); ++i) {
            if (RUBEChainFixturePtr chainFixture =
                boost::dynamic_pointer_cast<RUBEChainFixture>(body->fixture(i))) {
                RUBEPolygonFixturePtr polygonFixture =
                    boost::make_shared<RUBEPolygonFixture>(body->fixture(i)->name());
                polygonFixture->fixtureDef() = body->fixture(i)->fixtureDef();
                polygonFixture->fixtureDef().shape = &polygonFixture->shape();
                polygonFixture->shape().Set(&chainFixture->shape().m_vertices[0],
                    chainFixture->shape().m_count - 1);
                newBody->addFixture(polygonFixture);
            }
        }

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(newBody->bodyDef());

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        PuzzleHintAreaComponentPtr thinkComponent =
            boost::make_shared<PuzzleHintAreaComponent>();

        obj->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createWeaponStation(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createWeaponStation(
            body->enumProp("weaponType", WeaponTypeGG),
            body->floatProp("minAmmo"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBarrel1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBarrel1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBarrel2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBarrel2();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createTurret1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createTurret1();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createPyrobot(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPyrobot();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createKeyl(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createKeyl();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createLaserBlocker(Scene* scene, const RUBEBodyPtr& body)
    {
        UInt32 mask = collisionBitPlayer |
            collisionBitEnemy | collisionBitAlly | collisionBitGizmo |
            collisionBitRock | collisionBitEnemyBuilding;

        if (!body->boolProp("blockPlayer", true)) {
            mask &= ~collisionBitPlayer;
        }

        if (!body->boolProp("blockAlly", true)) {
            mask &= ~collisionBitAlly;
        }

        if (!body->boolProp("blockEnemy", true)) {
            mask &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        }

        if (!body->boolProp("blockRock", true)) {
            mask &= ~collisionBitRock;
        }

        SceneObjectPtr obj = sceneObjectFactory.createLaserBlocker(
            body->colorProp("color", Color(1.0f, 0.0f, 0.0f)),
            body->floatProp("width"), body->floatProp("length"),
            body->intProp("zorder"), mask);

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerupInventory(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupInventory(
            body->enumProp("inventoryItem", InventoryItemDynomite));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createPlaceholder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->addComponent(sceneObjectFactory.createPlaceholder(
            body->enumProp("inventoryItem", InventoryItemDynomite),
            body->floatProp("height"),
            body->floatProp("radius"),
            body->intProp("num_sectors"),
            body->intProp("image_zorder"),
            body->intProp("circle_zorder"),
            body->stringProp("image"),
            body->boolProp("image_flip"),
            body->floatProp("alpha")));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        obj->findComponent<PlaceholderComponent>()->setVisible(body->boolProp("visible", true));
        obj->findComponent<PlaceholderComponent>()->setActive(body->bodyDef().active);

        return obj;
    }

    SceneObjectPtr SceneAsset::createRockPlaceholder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->addComponent(sceneObjectFactory.createRockPlaceholder(
            body->floatProp("height"),
            body->floatProp("radius"),
            body->intProp("num_sectors"),
            body->intProp("image_zorder"),
            body->intProp("circle_zorder"),
            body->stringProp("image"),
            body->boolProp("image_flip"),
            body->floatProp("alpha")));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        obj->findComponent<PlaceholderComponent>()->setVisible(body->boolProp("visible", true));
        obj->findComponent<PlaceholderComponent>()->setActive(body->bodyDef().active);

        return obj;
    }

    SceneObjectPtr SceneAsset::createVehiclePlaceholder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->addComponent(sceneObjectFactory.createVehiclePlaceholder(
            body->floatProp("radius"),
            body->intProp("num_sectors"),
            body->intProp("zorder"),
            body->floatProp("alpha")));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        obj->findComponent<PlaceholderComponent>()->setVisible(body->boolProp("visible", true));
        obj->findComponent<PlaceholderComponent>()->setActive(body->bodyDef().active);

        return obj;
    }

    SceneObjectPtr SceneAsset::createAllyPlaceholder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->addComponent(sceneObjectFactory.createAllyPlaceholder(
            body->enumProp("inventoryItem", InventoryItemDynomite),
            body->floatProp("height"),
            body->floatProp("radius"),
            body->intProp("num_sectors"),
            body->intProp("image_zorder"),
            body->intProp("circle_zorder"),
            body->stringProp("image"),
            body->boolProp("image_flip"),
            body->floatProp("alpha")));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        obj->findComponent<PlaceholderComponent>()->setVisible(body->boolProp("visible", true));
        obj->findComponent<PlaceholderComponent>()->setActive(body->bodyDef().active);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossSkull(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossSkull();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSpider(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSpider();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createInstance(Scene* scene, const RUBEBodyPtr& body)
    {
        if ((xf_.p != b2Vec2_zero) || (xf_.q.s != 0.0f) || (xf_.q.c != 1.0f)) {
            instances_.insert(std::make_pair(body->name(),
                scene->script_instanciateByTransform(body->stringProp("assetPath"),
                b2Mul(xf_, b2Transform(body->bodyDef().position, b2Rot(body->bodyDef().angle))))));
        } else {
            instances_.insert(std::make_pair(body->name(),
                scene->script_instanciate(body->stringProp("assetPath"),
                body->bodyDef().position, body->bodyDef().angle)));
        }

        return SceneObjectPtr();
    }

    SceneObjectPtr SceneAsset::createPowerupHelmet(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupHelmet();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBabySpider1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBabySpider1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBabySpider2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBabySpider2();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createSpiderNest(Scene* scene, const RUBEBodyPtr& body)
    {
        float probabilities[1] = { body->floatProp("probability1", 0.5f) };

        SceneObjectPtr obj = sceneObjectFactory.createSpiderNest(probabilities,
            body->boolProp("dead"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createWeaponTrap(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createWeaponTrap(
            body->enumProp("weaponType", WeaponTypeGG),
            body->enumProp("missileType", SceneObjectTypeNeutralMissile));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossQueen(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossQueen();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createEnforcer1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createEnforcer1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createEnforcer2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createEnforcer2();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createEnforcer3(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createEnforcer3();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSentry1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSentry1(body->boolProp("folded"));

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSentry2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSentry2(body->boolProp("folded"));

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createGorger1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createGorger1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerupAmmo(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupAmmo(body->enumProp("weaponType", WeaponTypeProxMine));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createRoboArm(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createRoboArm();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createCrate(Scene* scene, const RUBEBodyPtr& body, int i)
    {
        float powerupProbabilities[PowerupTypeMax + 1];
        float ammoProbabilities[WeaponTypeMax + 1];

        for (int j = 0; j < PowerupTypeMax + 1; ++j) {
            std::ostringstream os;
            os << "prob_" << EnumTraits<PowerupType>::strValues[j];
            powerupProbabilities[j] = body->floatProp(os.str());
        }
        for (int j = 0; j < WeaponTypeMax + 1; ++j) {
            std::ostringstream os;
            os << "prob_ammo_" << EnumTraits<WeaponType>::strValues[j];
            ammoProbabilities[j] = body->floatProp(os.str());
        }

        SceneObjectPtr obj = sceneObjectFactory.createCrate(i, powerupProbabilities, ammoProbabilities);

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createCrate1(Scene* scene, const RUBEBodyPtr& body)
    {
        return createCrate(scene, body, 1);
    }

    SceneObjectPtr SceneAsset::createCrate2(Scene* scene, const RUBEBodyPtr& body)
    {
        return createCrate(scene, body, 2);
    }

    SceneObjectPtr SceneAsset::createCrate3(Scene* scene, const RUBEBodyPtr& body)
    {
        return createCrate(scene, body, 3);
    }

    SceneObjectPtr SceneAsset::createCrate4(Scene* scene, const RUBEBodyPtr& body)
    {
        return createCrate(scene, body, 4);
    }

    SceneObjectPtr SceneAsset::createPowerupWeapon(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupWeapon(
            body->enumProp("weaponType", WeaponTypeBlaster),
            body->floatProp("minAmmo"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createOrbo(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createOrbo();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createWarder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createWarder();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createKeeper(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createKeeper(body->boolProp("folded"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerupBackpack(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupBackpack();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerupEarpiece(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerupEarpiece();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createHealthStation(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createHealthStation(body->floatProp("amount"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createRodBundle(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createRodBundle();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossCoreProtector(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossCoreProtector();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBarrel3(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBarrel3();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createAcidWorm1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createAcidWorm1(body->boolProp("folded"),
            body->boolProp("foldable"), body->floatProp("unfoldRadius"));

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createCentipede1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createCentipede1();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBeetle1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBeetle1(body->boolProp("shoot"), body->boolProp("split"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBeetle2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBeetle2();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBoat(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBoat();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerGen(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerGen(body->boolProp("rusted"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createPowerGen2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createPowerGen2(body->boolProp("rusted"));

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossSquid(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossSquid();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createMech(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createMech();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createHomer(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createHomer();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createScientist(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createScientist();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossChopper(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossChopper();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createCreature(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createCreature(body->intProp("i", 1));

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createOctopus(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createOctopus();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createWalker(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createWalker();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSnake(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSnake();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossBuddy(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossBuddy();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossNatan(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossNatan();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createGuardian(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createGuardian();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createGuardian2(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createGuardian2();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createMilitary1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createMilitary1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createSarge1(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSarge1();

        obj->setPos(body->bodyDef().position);
        obj->setAngle(body->bodyDef().angle);

        return obj;
    }

    SceneObjectPtr SceneAsset::createEnemyBuilding(Scene* scene, const RUBEBodyPtr& body)
    {
        RUBEBodyPtr newBody =
            boost::make_shared<RUBEBody>(body->name(), body->bodyDef());

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeEnemyBuilding);

        obj->setBodyDef(newBody->bodyDef());

        processFixtures(body, newBody, obj.get(), false);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(newBody);

            obj->addComponent(component);
        }

        processImages(body, obj.get());

        return obj;
    }

    SceneObjectPtr SceneAsset::createSawer(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createSawer(body->floatProp("length", 5.0f), body->boolProp("fixed", true));

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        if (body->boolProp("autoTarget")) {
            obj->findComponent<TargetableComponent>()->setAutoTarget(true);
        }

        return obj;
    }

    SceneObjectPtr SceneAsset::createBossBeholder(Scene* scene, const RUBEBodyPtr& body)
    {
        SceneObjectPtr obj = sceneObjectFactory.createBossBeholder();

        obj->setPosRecursive(body->bodyDef().position);
        obj->setAngleRecursive(body->bodyDef().angle);

        return obj;
    }
}
