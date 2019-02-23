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

#include "SceneObjectFactory.h"
#include "Settings.h"
#include "AssetManager.h"
#include "PlayerComponent.h"
#include "ShroomCannonComponent.h"
#include "ShroomGuardianComponent.h"
#include "MissileComponent.h"
#include "ExplosionComponent.h"
#include "BlasterShotComponent.h"
#include "CollisionDieComponent.h"
#include "CollisionSparksComponent.h"
#include "CollisionDamageComponent.h"
#include "CollisionSensorComponent.h"
#include "RenderQuadComponent.h"
#include "RenderTerrainComponent.h"
#include "RenderBackgroundComponent.h"
#include "RenderDottedCircleComponent.h"
#include "RenderPolygonsComponent.h"
#include "AnimationComponent.h"
#include "ShroomSnakeComponent.h"
#include "TetrobotComponent.h"
#include "RenderBeamComponent.h"
#include "LaserComponent.h"
#include "TetrocrabComponent.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsRopeComponent.h"
#include "RenderHealthbarComponent.h"
#include "SpawnerComponent.h"
#include "FireballComponent.h"
#include "ScorpComponent.h"
#include "TeleportComponent.h"
#include "BloodComponent.h"
#include "InvulnerabilityComponent.h"
#include "WeaponStationComponent.h"
#include "RenderTrailComponent.h"
#include "BuckshotComponent.h"
#include "BarrelComponent.h"
#include "ConeLight.h"
#include "TurretComponent.h"
#include "GoalComponent.h"
#include "PyrobotComponent.h"
#include "PlaceholderComponent.h"
#include "TimebombComponent.h"
#include "BossSkullComponent.h"
#include "SpiderComponent.h"
#include "BabySpiderComponent.h"
#include "SpiderNestComponent.h"
#include "WeaponGGComponent.h"
#include "WeaponShotgunComponent.h"
#include "WeaponPistolComponent.h"
#include "WeaponMachineGunComponent.h"
#include "WeaponRLauncherComponent.h"
#include "WeaponLGunComponent.h"
#include "WeaponPlasmaGunComponent.h"
#include "WeaponSeekerComponent.h"
#include "BossQueenComponent.h"
#include "CollisionCancelComponent.h"
#include "BulletComponent.h"
#include "EnforcerComponent.h"
#include "SentryComponent.h"
#include "GorgerComponent.h"
#include "RocketComponent.h"
#include "ProxMineComponent.h"
#include "LaserSightComponent.h"
#include "RoboArmComponent.h"
#include "CrateComponent.h"
#include "OrboComponent.h"
#include "WarderComponent.h"
#include "PlasmaComponent.h"
#include "KeeperComponent.h"
#include "DudeComponent.h"
#include "HealthStationComponent.h"
#include "SummonComponent.h"
#include "BossCoreProtectorComponent.h"
#include "ToxicSplashComponent.h"
#include "ToxicCloudComponent.h"
#include "AcidWormComponent.h"
#include "WeaponAcidballComponent.h"
#include "AcidballComponent.h"
#include "CentipedeComponent.h"
#include "WeaponSpitterComponent.h"
#include "SpitComponent.h"
#include "BeetleComponent.h"
#include "BoatComponent.h"
#include "PowerGenComponent.h"
#include "TentaclePulseComponent.h"
#include "TentacleSwingComponent.h"
#include "TentacleSquidComponent.h"
#include "BossSquidComponent.h"
#include "MechComponent.h"
#include "HomerComponent.h"
#include "SeekerComponent.h"
#include "BossChopperComponent.h"
#include "CreatureComponent.h"
#include "GasCloudComponent.h"
#include "OctopusComponent.h"
#include "WalkerComponent.h"
#include "SnakeComponent.h"
#include "BossBuddyComponent.h"
#include "QuadPulseComponent.h"
#include "BuddySideComponent.h"
#include "BuddyTongueComponent.h"
#include "NapalmComponent.h"
#include "BossNatanComponent.h"
#include "PowerBeamComponent.h"
#include "Tetrobot2Component.h"
#include "WeaponHeaterComponent.h"
#include "WeaponAimComponent.h"
#include "GuardianComponent.h"
#include "SawerComponent.h"
#include "CollisionBuzzSawComponent.h"
#include "PuzzleHintComponent.h"
#include "BossBeholderComponent.h"
#include "WeaponEShieldComponent.h"
#include "Const.h"
#include "Utils.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    SceneObjectFactory sceneObjectFactory;

    template <>
    Single<SceneObjectFactory>* Single<SceneObjectFactory>::single = NULL;

    SceneObjectFactory::SceneObjectFactory()
    {
    }

    SceneObjectFactory::~SceneObjectFactory()
    {
    }

    bool SceneObjectFactory::init()
    {
        return true;
    }

    void SceneObjectFactory::shutdown()
    {
    }

    void SceneObjectFactory::setupGarbage(const SceneObjectPtr& obj, bool cancelCollisions)
    {
        if (cancelCollisions) {
            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
            c->setFilter(types);
            obj->addComponent(c);
        }
    }

    SceneObjectPtr SceneObjectFactory::createDummy()
    {
        b2BodyDef bodyDef;

        bodyDef.type = b2_kinematicBody;

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", bodyDef);

        ComponentPtr component =
            boost::make_shared<PhysicsBodyComponent>(body);

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(body->bodyDef());

        obj->addComponent(component);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createPlayer()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("player.json");

        asset->root()->setType(SceneObjectTypePlayer);

        /*
         * We control the damping manually for the player.
         */
        asset->root()->setLinearDamping(0.0f);
        asset->root()->setAngularDamping(0.0f);

        asset->root()->setMaxLife(settings.player.maxLife);
        asset->root()->setLife(settings.player.maxLife);

        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");
        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());
        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());

        torsoAc->addAnimation(AnimationDefault, "player_torso_def");
        torsoAc->addAnimation(PlayerComponent::AnimationIdle, "player_torso_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationWalk, "player_torso_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationHelmetDefault, "player_torso_helmet_def");
        torsoAc->addAnimation(PlayerComponent::AnimationHelmetIdle, "player_torso_helmet_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationHelmetWalk, "player_torso_helmet_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationNogunDefault, "player_torso_nogun_def");
        torsoAc->addAnimation(PlayerComponent::AnimationNogunIdle, "player_torso_nogun_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationNogunWalk, "player_torso_nogun_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackDefault, "player_torso_nobp_def");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackIdle, "player_torso_nobp_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackWalk, "player_torso_nobp_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackChainsawDefault, "player_torso_nobp_chainsaw_def");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackChainsawIdle, "player_torso_nobp_chainsaw_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackChainsawWalk, "player_torso_nobp_chainsaw_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationNobackpackChainsawAttack, "player_torso_nobp_chainsaw_attack");
        torsoAc->addAnimation(PlayerComponent::AnimationChainsawDefault, "player_torso_chainsaw_def");
        torsoAc->addAnimation(PlayerComponent::AnimationChainsawIdle, "player_torso_chainsaw_idle");
        torsoAc->addAnimation(PlayerComponent::AnimationChainsawWalk, "player_torso_chainsaw_walk");
        torsoAc->addAnimation(PlayerComponent::AnimationChainsawAttack, "player_torso_chainsaw_attack");

        legsAc->addAnimation(AnimationDefault, "player_legs_def");
        legsAc->addAnimation(PlayerComponent::AnimationWalk, "player_legs_walk");

        torsoAc->startAnimation(PlayerComponent::AnimationIdle);
        legsAc->startAnimation(AnimationDefault);

        asset->root()->addComponent(torsoAc);
        asset->root()->addComponent(legsAc);

        PlayerComponentPtr pc(new PlayerComponent(
                asset->world()->image("missile")->pos(),
                normalizeAngle(asset->world()->image("missile")->angle() + b2_pi / 2.0f),
                asset->world()->image("missile")->height() / 2.0f,
                asset->world()->image("exhaust1")->pos(),
                normalizeAngle(asset->world()->image("exhaust1")->angle()),
                asset->world()->image("exhaust2")->pos(),
                normalizeAngle(asset->world()->image("exhaust2")->angle()),
                torsoRc, torsoAc, legsRc, legsAc,
                asset->root()->findComponent<LightComponent>()->getLight<Light>("flashlight")));

        pc->setMaxStamina(settings.player.maxStamina);
        pc->setStamina(settings.player.maxStamina);

        asset->root()->addComponent(pc);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.player.damageImpulseThreshold);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            component->setDamageSound(audio.createSound("player_hit.ogg"));

            asset->root()->addComponent(component);
        }

        if (settings.player.laserSight) {
            RenderTrailComponentPtr rtc =
                boost::make_shared<RenderTrailComponent>(asset->world()->image("sight")->pos(),
                    normalizeAngle(asset->world()->image("sight")->angle() + b2_pi / 2.0f),
                    assetManager.getDrawable("trail1.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
                    zOrderMarker);

            rtc->setColor(Color(1.0f, 0.0f, 0.0f, 0.4f));
            rtc->setColor2(Color(1.0f, 0.0f, 0.0f, 0.05f));

            rtc->setWidth1(0.3f);
            rtc->setWidth2(0.3f);

            rtc->setLength(0.0f);

            ComponentPtr component =
                boost::make_shared<LaserSightComponent>(pc, rtc);

            asset->root()->addComponent(rtc);
            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBackground(const std::string& image,
                                                        float width,
                                                        float height,
                                                        const b2Vec2& velocity,
                                                        int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setLinearVelocity(velocity);

        {
            ComponentPtr component =
                boost::make_shared<RenderBackgroundComponent>(
                    assetManager.getDrawable(image),
                    width, height, zOrder);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createShroomCannonRed1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("shroom_cannon_red1.json");

        asset->root()->setType(SceneObjectTypeEnemy);

        asset->root()->setLife(settings.shroomCannon.life);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "shroom_cannon_red1_def");
        ac->addAnimation(ShroomCannonComponent::AnimationAttack, "shroom_cannon_red1_attack");
        ac->addAnimation(ShroomCannonComponent::AnimationReload, "shroom_cannon_red1_reload");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        ShroomCannonComponentPtr thinkComponent =
            boost::make_shared<ShroomCannonComponent>(ac,
                asset->world()->image("missile")->pos());

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setFilter(SceneObjectTypeRock);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createShroomCannonRed1Missile(float damage)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("shroom_cannon_red1_missile.json");

        asset->root()->setType(SceneObjectTypeEnemyMissile);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<MissileComponent>(cdc, damage);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createExplosion1(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion1.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec, 15.0f, 128);

            component->setLight(25.0f, 1.0f, false, 0.9f);
            component->setShake(2.0f);
            component->setSound(audio.createSound("explosion1.ogg"));
            component->setColor(Color(1.0f, 0.7f, 0.7f));

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createExplosion2(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion2.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec);

            component->setLight(0.6f, 0.3f, true, 1.0f);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createExplosion3(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion3.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec, 9.0f, 64);

            component->setLight(10.0f, 1.0f, false, 0.7f);
            component->setShake(0.5f);
            component->setSound(audio.createSound("explosion3.ogg"));

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createExplosion4(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion4.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec, 7.0f, 64);

            component->setLight(10.0f, 1.0f, true, 0.8f);
            component->setShake(0.5f);
            component->setSound(audio.createSound("explosion4.ogg"));
            component->setColor(Color(0.0f, 1.0f, 0.0f));

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createExplosion5(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion5.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec, 7.5f, 64);

            component->setLight(10.0f, 1.0f, true, 0.75f);
            component->setShake(0.5f);
            component->setSound(audio.createSound("explosion3.ogg"));
            component->setColor(Color(1.0f, 1.0f, 1.0f));

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createExplosion6(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("explosion6.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ExplosionComponentPtr component =
                boost::make_shared<ExplosionComponent>(pec, 11.0f, 96);

            component->setLight(12.0f, 1.0f, false, 1.0f);
            component->setShake(1.5f);
            component->setSound(audio.createSound("explosion5.ogg"));
            component->setColor(Color(0.0f, 1.0f, 1.0f));

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBlasterShot(SceneObjectType type, float damage, bool twisted, float velocity)
    {
        static std::vector<float> tmp;

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("bullet1.json");

        Color* colors = &settings.blasterShot.enemy[0];

        switch (type) {
        case SceneObjectTypePlayerMissile:
            colors = &settings.blasterShot.player[0];
            break;
        case SceneObjectTypeAllyMissile:
            colors = &settings.blasterShot.ally[0];
            break;
        case SceneObjectTypeEnemyMissile:
        case SceneObjectTypeNeutralMissile:
            break;
        default:
            assert(false);
            break;
        }

        asset->root()->findComponent<RenderQuadComponent>()->setColor(colors[0]);
        asset->root()->findComponent<LightComponent>()->lights().front()->setColor(colors[1]);

        ParticleEffectComponentPtr pec = asset->root()->findComponent<ParticleEffectComponent>();

        pec->setTimeFactor(2);

        for (ParticleEffectComponent::Emmiters::iterator it = pec->emmiters().begin();
             it != pec->emmiters().end(); ++it) {
            tmp.push_back(0.0f);
            tmp.push_back(1.0f);

            (*it)->entry().tint.setTimeline(tmp);

            tmp.clear();

            tmp.push_back(colors[2].rgba[0]);
            tmp.push_back(colors[2].rgba[1]);
            tmp.push_back(colors[2].rgba[2]);
            tmp.push_back(colors[3].rgba[0]);
            tmp.push_back(colors[3].rgba[1]);
            tmp.push_back(colors[3].rgba[2]);

            (*it)->entry().tint.setColors(tmp);

            tmp.clear();

            (*it)->entry().duration.setLow((*it)->entry().duration.lowMin() * (20.0f / velocity));
            (*it)->entry().emission.setHigh((*it)->entry().emission.highMin() * (velocity / 20.0f));
            (*it)->entry().life.setHigh((*it)->entry().life.highMin() * (20.0f / velocity));
        }

        pec->resetEmit();

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<BlasterShotComponent>(cdc,
                pec,
                damage, twisted);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createShroomGuardianRed1()
    {
        return createShroomGuardianRed(settings.shroomGuardian.life,
            settings.shroomGuardian.turnSpeed, Color(1.0f, 1.0f, 1.0f, 1.0f));
    }

    SceneObjectPtr SceneObjectFactory::createShroomGuardianRed2()
    {
        return createShroomGuardianRed(settings.shroomGuardian2.life,
            settings.shroomGuardian2.turnSpeed, Color(1.0f, 0.0f, 0.0f, 1.0f));
    }

    SceneObjectPtr SceneObjectFactory::createJake()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("jake.json");

        asset->root()->setType(SceneObjectTypeAlly);

        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");

        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());

        legsAc->addAnimation(AnimationDefault, "player_def");
        legsAc->addAnimation(DudeComponent::AnimationWalk, "player_walk");

        legsAc->startAnimation(AnimationDefault);

        asset->root()->addComponent(legsAc);

        ComponentPtr component = boost::make_shared<DudeComponent>(legsRc, legsAc);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createShroomSnake()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("shroom_snake.json");

        asset->root()->setLife(settings.shroomSnake.life);

        std::vector<SInt32> cookies;

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);

            cookies.push_back(asset->object(i)->cookie());
        }

        for (int i = 0; i < asset->numObjects(); ++i) {
            CollisionCookieFilterPtr collisionFilter =
                boost::make_shared<CollisionCookieFilter>();

            for (size_t j = 0; j < cookies.size(); ++j) {
                if (cookies[j] != asset->object(i)->cookie()) {
                    collisionFilter->add(cookies[j]);
                }
            }

            asset->object(i)->setCollisionFilter(collisionFilter);
        }

        AnimationComponentPtr ac;

        Animation attack;

        {
            DrawablePtr drawable =
                asset->root()->findComponent<RenderQuadComponent>()->drawable();

            ac = boost::make_shared<AnimationComponent>(drawable);

            ac->addAnimation(AnimationDefault, "shroom_snake_def");
            ac->addAnimation(ShroomSnakeComponent::AnimationBite, "shroom_snake_bite");
            ac->addAnimation(ShroomSnakeComponent::AnimationAttack, "shroom_snake_attack");

            ac->startAnimation(AnimationDefault);

            asset->root()->addComponent(ac);
        }

        ShroomSnakeComponentPtr thinkComponent;

        {
            assert(settings.shroomSnake.attackAngles.size() == static_cast<size_t>(attack.numFrames()));

            thinkComponent =
                boost::make_shared<ShroomSnakeComponent>(
                    asset->object("upper_lip"),
                    asset->object("upper_lip")->pos() - asset->root()->pos(),
                    asset->object("lower_lip"),
                    asset->object("lower_lip")->pos() - asset->root()->pos(),
                    ac,
                    settings.shroomSnake.defAngle,
                    settings.shroomSnake.attackAngles);

            asset->root()->addComponent(thinkComponent);
        }

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createRope(float length)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("rope_hook.json");

        asset->root()->setType(SceneObjectTypeDeadbody);

        float segmentSize = 1.0f;

        SceneObjectPtr root = asset->root();

        int numSegments = std::ceil(length / segmentSize);

        std::vector<SceneObject*> objects;

        objects.reserve(numSegments + 1);

        objects.push_back(root.get());

        if (numSegments > 1) {
            for (int i = 1; i < numSegments; ++i) {
                b2Vec2 pos(-segmentSize * i, 0.0f);

                asset = assetManager.getSceneObjectAsset(((i % 2) == 0) ? "rope_segment1.json" : "rope_segment2.json");

                asset->root()->setType(SceneObjectTypeDeadbody);

                asset->root()->setPos(pos);

                root->addObject(asset->root());

                objects.push_back(asset->root().get());
            }
        }

        PhysicsRopeComponentPtr component =
            boost::make_shared<PhysicsRopeComponent>(objects, segmentSize);

        root->addComponent(component);

        return root;
    }

    SceneObjectPtr SceneObjectFactory::createBlocker(float width, float height)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeBlocker);

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", b2BodyDef());

        RUBEPolygonFixturePtr fixture = boost::make_shared<RUBEPolygonFixture>("");

        fixture->fixtureDef().friction = 0.0f;
        fixture->fixtureDef().filter.categoryBits = collisionBitGeneral;
        fixture->fixtureDef().filter.maskBits = collisionBitPlayer;

        fixture->shape().SetAsBox(width / 2.0f, height / 2.0f);

        body->addFixture(fixture);

        ComponentPtr component =
            boost::make_shared<PhysicsBodyComponent>(body);

        obj->addComponent(component);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createTetrobot()
    {
        return createTetrobot2();
    }

    SceneObjectPtr SceneObjectFactory::createTetrobotGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("tetrobot_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createTetrobot2()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("tetrobot2.json");

        asset->root()->setType(SceneObjectTypeEnemy);

        asset->root()->setMaxLife(settings.tetrobot2.life);
        asset->root()->setLife(settings.tetrobot2.life);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "tetrobot2_def");
        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        Tetrobot2ComponentPtr component =
            boost::make_shared<Tetrobot2Component>();

        WeaponHeaterComponentPtr weapon =
            boost::make_shared<WeaponHeaterComponent>(SceneObjectTypeEnemyMissile);

        weapon->setDamage(settings.tetrobot2.shootDamage);
        weapon->setLength(settings.tetrobot2.shootLength);
        weapon->setImpulse(settings.tetrobot2.shootImpulse);
        weapon->setDuration(settings.tetrobot2.shootDuration);
        weapon->setInterval(0.0f);
        weapon->setPos(asset->world()->image("missile")->pos());
        weapon->setAngle(asset->world()->image("missile")->angle());

        component->setWeapon(weapon);

        asset->root()->addComponent(weapon);

        WeaponAimComponentPtr aimWeapon =
            boost::make_shared<WeaponAimComponent>(SceneObjectTypeEnemyMissile);

        aimWeapon->setLength(settings.tetrobot2.shootLength);
        aimWeapon->setPos(asset->world()->image("missile")->pos());
        aimWeapon->setAngle(asset->world()->image("missile")->angle());

        component->setAimWeapon(aimWeapon);

        asset->root()->addComponent(aimWeapon);

        asset->root()->addComponent(component);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.tetrobot2.damageImpulseThreshold);
            component->setMultiplier(settings.tetrobot2.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createTetrobot2Gibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("tetrobot2_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
        }

        return asset->root();
    }

    ComponentPtr SceneObjectFactory::createLaser(const b2Vec2& pos,
        float angle, float length, float damage, int zOrder)
    {
        RenderBeamComponentPtr rbc = boost::make_shared<RenderBeamComponent>(pos, angle, 0.5f,
            assetManager.getDrawable("healthbar_life.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
            zOrder);

        return boost::make_shared<LaserComponent>(pos, angle, length, damage, rbc);
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrabInternal(const std::string& assetPath, const Color& color, int idx, float life, float walkSpeed, float walkAccel, float turnSpeed)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(assetPath);

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setLife(life);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        std::ostringstream os;
        os << "tetrocrab" << idx;

        ac->addAnimation(AnimationDefault, os.str() + "_def", 5.0f / walkSpeed);
        ac->addAnimation(TetrocrabComponent::AnimationBite, os.str() + "_bite", 5.0f / walkSpeed);
        ac->startAnimation(AnimationDefault);

        ac->setPaused(true);

        asset->root()->addComponent(ac);

        TetrocrabComponentPtr thinkComponent =
            boost::make_shared<TetrocrabComponent>(idx, ac, walkSpeed, walkAccel, turnSpeed);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.tetrocrab.damageImpulseThreshold);
            component->setMultiplier(settings.tetrocrab.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrab()
    {
        return createTetrocrab3();
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrab2()
    {
        return createTetrocrab4();
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrab3()
    {
        return createTetrocrabInternal("tetrocrab3.json",
            Color(1.0f, 1.0f, 0.0f, 1.0f),
            1,
            settings.tetrocrab3.life,
            settings.tetrocrab3.walkSpeed, 2.0f,
            settings.tetrocrab3.turnSpeed);
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrab4()
    {
        return createTetrocrabInternal("tetrocrab3.json",
            Color(0.0f, 1.0f, 0.0f, 1.0f),
            2,
            settings.tetrocrab4.life,
            settings.tetrocrab3.walkSpeed, 2.0f,
            settings.tetrocrab3.turnSpeed);
    }

    SceneObjectPtr SceneObjectFactory::createTetrocrabGibbed(int i)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset((i == 1) ? "tetrocrab1_gibbed.json" : "tetrocrab2_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setMaterial(MaterialFlesh);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSpawner1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("spawner1.json");

        asset->root()->setType(SceneObjectTypeEnemyBuilding);

        asset->root()->setMaxLife(1);
        asset->root()->setLife(1);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 7.0f), 0, 10.0f, 1.5f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        {
            SpawnerComponentPtr component =
                boost::make_shared<SpawnerComponent>(b2Vec2_zero, 8.0f);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    AnimationComponentPtr SceneObjectFactory::createLightningAnimation()
    {
        AnimationComponentPtr component =
            boost::make_shared<AnimationComponent>(
                assetManager.getDrawable("lightning/0.png", Texture::WrapModeRepeat, Texture::WrapModeClamp));

        component->addAnimation(AnimationDefault, "lightning");

        return component;
    }

    SceneObjectPtr SceneObjectFactory::createScorpInternal(const Color& color, const std::string& nameBase, float life, float walkSpeed)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("scorp.json");

        asset->root()->setType(SceneObjectTypeEnemy);

        asset->root()->setLife(life);

        asset->root()->findComponent<RenderQuadComponent>()->setColor(color);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, nameBase + "_def");
        ac->addAnimation(ScorpComponent::AnimationWalk, nameBase + "_walk", (5.0f / walkSpeed));
        ac->addAnimation(ScorpComponent::AnimationAttack, nameBase + "_attack");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        ScorpComponentPtr thinkComponent =
            boost::make_shared<ScorpComponent>(ac, walkSpeed);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.scorp.damageImpulseThreshold);
            component->setMultiplier(settings.scorp.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createScorp()
    {
        return createScorp3();
    }

    SceneObjectPtr SceneObjectFactory::createScorp2()
    {
        return createScorp4();
    }

    SceneObjectPtr SceneObjectFactory::createScorp3()
    {
        return createScorpInternal(Color(1.0f, 1.0f, 1.0f, 1.0f), "scorp", settings.scorp3.life, settings.scorp3.walkSpeed);
    }

    SceneObjectPtr SceneObjectFactory::createScorp4()
    {
        return createScorpInternal(Color(1.0f, 1.0f, 1.0f, 1.0f), "scorp2", settings.scorp4.life, settings.scorp3.walkSpeed);
    }

    SceneObjectPtr SceneObjectFactory::createFireball(SceneObjectType type, float damage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("fireball.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "fireball_def");
        ac->addAnimation(FireballComponent::AnimationBoom, "fireball_boom");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        {
            ComponentPtr component = boost::make_shared<FireballComponent>(cdc, ac, damage);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createTeleport1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("teleport1.json");

        {
            ComponentPtr component = boost::make_shared<TeleportComponent>(asset->object("ring"));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupHealth(float timeout, float amount)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_health.json");

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupHealthComponent>(cdc, timeout,
                    settings.powerupHealth.radius, settings.powerupHealth.color,
                    amount);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupGem(float timeout)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_gem.json");

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupGemComponent>(cdc, timeout,
                    settings.powerupGem.radius, settings.powerupGem.color);

            asset->root()->addComponent(component);
        }

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "powerup_gem_def");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupInventory(InventoryItem item)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(Inventory::getAssetPath(item));

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupInventoryComponent>(item, cdc,
                    settings.powerupInventory.radius, settings.powerupInventory.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupHelmet()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_helmet.json");

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupHelmetComponent>(cdc,
                    settings.powerupHelmet.radius, settings.powerupHelmet.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupAmmo(WeaponType type)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(WeaponComponent::getAmmoAssetPath(type));

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        float amount = 0;

        switch (type) {
        case WeaponTypeProxMine:
            amount = settings.powerupAmmo.proxmineAmount[0];
            break;
        case WeaponTypeRLauncher:
            amount = settings.powerupAmmo.rlauncherAmount[0];
            break;
        case WeaponTypeMachineGun:
            amount = settings.powerupAmmo.mgunAmount[0];
            break;
        case WeaponTypePlasmaGun:
            amount = settings.powerupAmmo.plasmagunAmount[0];
            break;
        case WeaponTypeShotgun:
            amount = settings.powerupAmmo.shotgunAmount[0];
            break;
        case WeaponTypeEShield:
            amount = settings.powerupAmmo.eshieldAmount[0];
            break;
        default:
            assert(false);
            break;
        }

        {
            ComponentPtr component =
                boost::make_shared<PowerupAmmoComponent>(type, amount, cdc,
                    settings.powerupAmmo.radius, settings.powerupAmmo.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupWeapon(WeaponType type, float amount)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_weapon.json");

        asset->root()->findComponentByName<RenderQuadComponent>("weapon")->drawable()->setImage(
            WeaponComponent::getImage(type));

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupWeaponComponent>(type, amount, cdc,
                    settings.powerupWeapon.radius, settings.powerupWeapon.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupBackpack()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_backpack.json");

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupBackpackComponent>(cdc,
                    settings.powerupBackpack.radius, settings.powerupBackpack.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerupEarpiece()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powerup_earpiece.json");

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        cdc->setAllowSensor(true);

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component =
                boost::make_shared<PowerupEarpieceComponent>(cdc,
                    settings.powerupEarpiece.radius, settings.powerupEarpiece.color);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    AnimationComponentPtr SceneObjectFactory::createArrowAnimation()
    {
        AnimationComponentPtr component =
            boost::make_shared<AnimationComponent>(
                assetManager.getDrawable("common2/arrow_0.png"));

        component->addAnimation(AnimationDefault, "arrow");

        return component;
    }

    SceneObjectPtr SceneObjectFactory::createBlood1(float height, int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        DrawablePtr drawable = assetManager.getDrawable("common2/blood_0.png");

        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, zOrder);

        obj->addComponent(rc);

        AnimationComponentPtr ac =
            boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "blood");

        ac->startAnimation(AnimationDefault);

        obj->addComponent(ac);

        BloodComponentPtr bc = boost::make_shared<BloodComponent>(ac);

        obj->addComponent(bc);

        return obj;
    }

    ComponentPtr SceneObjectFactory::createInvulnerability(float radius, float duration, int zOrder)
    {
        DrawablePtr drawable = assetManager.getDrawable("factory4/shield_0.png");

        float height = radius * 2.0f;
        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, zOrder);

        rc->setColor(Color(1.0f, 1.0f, 1.0f, 0.5f));

        AnimationComponentPtr ac =
            boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "shield");

        ac->startAnimation(AnimationDefault);

        return boost::make_shared<InvulnerabilityComponent>(rc, ac, duration);
    }

    SceneObjectPtr SceneObjectFactory::createBloodStain1(float height, int zOrder, bool deadbody)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        if (deadbody) {
            makeDeadbody(obj, height / 8.0f);
        } else {
            obj->setFreezable(true);
            obj->setFreezeRadius(height);
        }

        std::ostringstream os;

        os << "common2/blood_" << (rand() % 6) << ".png";

        DrawablePtr drawable = assetManager.getDrawable(os.str());

        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, zOrder);

        obj->addComponent(rc);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBloodStain2(float height, int zOrder, bool deadbody)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        if (deadbody) {
            makeDeadbody(obj, height / 8.0f);
        } else {
            obj->setFreezable(true);
            obj->setFreezeRadius(height);
        }

        DrawablePtr drawable = assetManager.getDrawable("factory1/blood2.png");

        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, zOrder);

        obj->addComponent(rc);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createWeaponStation(WeaponType weaponType, float minAmmo)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("weapon_station.json");

        asset->root()->findComponentByName<RenderQuadComponent>("weapon")->drawable()->setImage(
            WeaponComponent::getImage(weaponType));

        WeaponStationComponentPtr thinkComponent = boost::make_shared<WeaponStationComponent>(weaponType, minAmmo);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBuckshot(SceneObjectType type, float damage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("buckshot.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        ConeLightPtr light =
            asset->root()->findComponent<LightComponent>()->getLight<ConeLight>("cone_light");

        RenderQuadComponentPtr rc = asset->root()->findComponentByName<RenderQuadComponent>("root");

        RenderTrailComponentPtr rtc =
            boost::make_shared<RenderTrailComponent>(b2Vec2_zero, b2_pi,
                assetManager.getDrawable("fire1.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
                rc->zOrder() - 1);

        rtc->setColor(Color(1.0f, 1.0f, 1.0f, rc->color().rgba[3]));
        rtc->setColor2(Color(1.0f, 1.0f, 1.0f, 0.0f));

        asset->root()->addComponent(rtc);

        {
            ComponentPtr component = boost::make_shared<BuckshotComponent>(cdc,
                rc, rtc, light, damage, 0.3f, 0.8f, 10.0f);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel1.json");

        asset->root()->setType(SceneObjectTypeGizmo);
        asset->root()->setLife(settings.barrel1.life);
        asset->root()->setMaxLife(settings.barrel1.life);

        asset->root()->setBlastDamageMultiplier(settings.barrel1.blastDamageMultiplier);

        {
            BarrelComponentPtr component = boost::make_shared<BarrelComponent>();

            component->setExplosive(settings.barrel1.explosionImpulse,
                settings.barrel1.explosionDamage);

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.barrel1.damageImpulseThreshold);
            component->setMultiplier(settings.barrel1.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);

            component->setFilter(types);

            component->setDamageSound(audio.createSound("metal_hit1.ogg"));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel1Gibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel1_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeGizmo);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel2()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel2.json");

        asset->root()->setType(SceneObjectTypeGizmo);
        asset->root()->setLife(settings.barrel2.life);
        asset->root()->setMaxLife(settings.barrel2.life);

        {
            BarrelComponentPtr component = boost::make_shared<BarrelComponent>();

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.barrel2.damageImpulseThreshold);
            component->setMultiplier(settings.barrel2.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);

            component->setFilter(types);

            component->setDamageSound(audio.createSound("metal_hit1.ogg"));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel2Gibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel2_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeGizmo);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel3()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel3.json");

        asset->root()->setType(SceneObjectTypeGizmo);
        asset->root()->setLife(settings.barrel3.life);
        asset->root()->setMaxLife(settings.barrel3.life);

        {
            BarrelComponentPtr component = boost::make_shared<BarrelComponent>();

            component->setToxic(settings.barrel3.toxicDamage, settings.barrel3.toxicDamageTimeout);

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.barrel3.damageImpulseThreshold);
            component->setMultiplier(settings.barrel3.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);

            component->setFilter(types);

            component->setDamageSound(audio.createSound("metal_hit1.ogg"));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBarrel3Gibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("barrel3_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeGizmo);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createTurret1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("turret1.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
        }

        asset->root()->setLife(settings.turret1.life);
        asset->root()->setMaxLife(settings.turret1.life);

        SceneObjectPtr tower;
        std::vector<b2Transform> shotPoses;

        {
            tower = asset->object("tower");

            for (int i = 0; i < asset->world()->numImages(); ++i) {
                const RUBEImagePtr& image = asset->world()->image(i);
                if (image->name() == "missile") {
                    shotPoses.push_back(b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f)));
                }
            }

            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.turret1.damageImpulseThreshold);
            component->setMultiplier(settings.turret1.damageMultiplier);

            component->setFilter(SceneObjectTypes(SceneObjectTypeGizmo) | SceneObjectTypeRock);

            tower->addComponent(component);
        }

        {
            ComponentPtr component = boost::make_shared<TurretComponent>(tower, shotPoses);

            asset->root()->addComponent(component);
        }

        tower->setBlastDamageMultiplier(settings.turret1.blastDamageMultiplier);

        return asset->root();
    }

    ComponentPtr SceneObjectFactory::createGoal(const SceneObjectPtr& forObject)
    {
        PhysicsBodyComponentPtr c = forObject->findComponent<PhysicsBodyComponent>();

        b2Vec2 ext = c->computeAABB(b2Transform(b2Vec2_zero, b2Rot(0))).GetExtents();

        float radius = b2Max(ext.x, ext.y) + 0.3f;

        return createGoal(radius, b2Vec2_zero);
    }

    ComponentPtr SceneObjectFactory::createGoal(float radius, const b2Vec2& offset)
    {
        RenderDottedCircleComponentPtr dcc =
            boost::make_shared<RenderDottedCircleComponent>(offset, 0.3f,
                assetManager.getDrawable("common2/dot.png"), zOrderMarker);

        dcc->setColor(Color(0.17f, 0.7f, 0.17f, 0.8f));
        dcc->setRadius(radius);
        dcc->setDotVelocity(5.0f, radius);
        dcc->setDotDistance(0.5f, radius);

        return boost::make_shared<GoalComponent>(dcc);
    }

    SceneObjectPtr SceneObjectFactory::createShroomGuardianRed(float life, float turnSpeed, const Color& color)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("shroom_guardian_red1.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->findComponent<RenderQuadComponent>()->setColor(color);
        }

        asset->root()->setLife(life);
        asset->root()->setMaxLife(life);

        {
            ShroomGuardianComponentPtr component = boost::make_shared<ShroomGuardianComponent>();

            component->setTurnSpeed(turnSpeed);

            asset->root()->addComponent(component);
        }

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.0f), 0, 3.0f, 0.5f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPyrobot()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("pyrobot.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialBulletProof);

        asset->root()->setLife(settings.pyrobot.life);
        asset->root()->setMaxLife(settings.pyrobot.life);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "pyrobot_def");
        ac->addAnimation(PyrobotComponent::AnimationWalk, "pyrobot_walk");
        ac->addAnimation(PyrobotComponent::AnimationAttack, "pyrobot_attack");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        std::vector<b2Transform> weaponXfs;

        for (int i = 0; i < asset->world()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->world()->image(i);
            if (image->name() == "missile") {
                weaponXfs.push_back(b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f)));
            }
        }

        PyrobotComponentPtr thinkComponent =
            boost::make_shared<PyrobotComponent>(ac, weaponXfs[0], weaponXfs[1]);

        asset->root()->addComponent(thinkComponent);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.0f), 0, 5.0f, 1.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createKeyl()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("keyl.json");

        asset->root()->setType(SceneObjectTypeAlly);

        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");
        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");

        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());
        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());

        legsAc->addAnimation(AnimationDefault, "kyle_legs_def");
        legsAc->addAnimation(DudeComponent::AnimationWalk, "kyle_legs_walk");

        torsoAc->addAnimation(AnimationDefault, "kyle_torso_def");
        torsoAc->addAnimation(DudeComponent::AnimationWalk, "kyle_torso_walk");

        legsAc->startAnimation(AnimationDefault);
        torsoAc->startAnimationRandomTime(AnimationDefault);

        asset->root()->addComponent(legsAc);
        asset->root()->addComponent(torsoAc);

        DudeComponentPtr component = boost::make_shared<DudeComponent>(legsRc, legsAc, torsoAc);

        component->setLegsAngle(-b2_pi / 2);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createLaserBlocker(const Color& color, float width, float length, int zOrder, UInt32 maskBits)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setType(SceneObjectTypeBlocker);

        b2BodyDef bodyDef;

        bodyDef.type = b2_kinematicBody;

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", bodyDef);

        RUBEPolygonFixturePtr fixture = boost::make_shared<RUBEPolygonFixture>("");

        fixture->fixtureDef().friction = 0.0f;
        fixture->fixtureDef().filter.categoryBits = collisionBitGeneral;
        fixture->fixtureDef().filter.maskBits = maskBits;

        fixture->shape().SetAsBox(length / 2.0f, width / 2.0f, b2Vec2(length / 2.0f, 0.0f), 0.0f);

        body->addFixture(fixture);

        ComponentPtr component =
            boost::make_shared<PhysicsBodyComponent>(body);

        obj->addComponent(component);

        {
            RenderBeamComponentPtr component = boost::make_shared<RenderBeamComponent>(b2Vec2_zero, 0.0f, width,
                assetManager.getDrawable("laser1.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
                zOrder);

            component->setLength(length);

            Color tmp = color;
            tmp.rgba[3] = 0.4f;

            component->setColor(tmp);

            obj->addComponent(component);
        }

        {
            Color tmp = color;
            tmp.rgba[3] = 1.0f;

            LineLightPtr light = boost::make_shared<LineLight>();
            light->setAngle(b2_pi / 2);
            light->setColor(tmp);
            light->setXray(true);
            light->setDistance(width);
            light->setBothWays(true);
            light->setPos(b2Vec2(length / 2, 0.0f));
            light->setLength(length / 2);

            LightComponentPtr component = boost::make_shared<LightComponent>();

            component->attachLight(light);

            obj->addComponent(component);
        }

        obj->setBodyDef(body->bodyDef());

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createCircleSensor(float radius, UInt32 maskBits)
    {
        b2BodyDef bodyDef;

        bodyDef.type = b2_kinematicBody;

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", bodyDef);

        RUBECircleFixturePtr fixture = boost::make_shared<RUBECircleFixture>("");

        fixture->fixtureDef().isSensor = true;
        fixture->fixtureDef().filter.categoryBits = collisionBitGeneral;
        fixture->fixtureDef().filter.maskBits = maskBits;

        fixture->shape().m_p = b2Vec2_zero;
        fixture->shape().m_radius = radius;

        body->addFixture(fixture);

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

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

        obj->setBodyDef(body->bodyDef());

        return obj;
    }

    ComponentPtr SceneObjectFactory::createCircle(float radius, int numSectors, const Color& color, int zOrder)
    {
        std::vector<Points> polygons;

        polygons.push_back(Points());

        polygons.back().resize(numSectors);

        for (int i = 0; i < numSectors; ++i) {
            float a = i * 2 * b2_pi / numSectors;
            polygons.back()[i] = radius * b2Vec2(cosf(a), sinf(a));
        }

        RenderPolygonsComponentPtr component =
            boost::make_shared<RenderPolygonsComponent>(polygons,
                assetManager.getImage("dialog_bg.png"),
                1.0f, 1.0f, zOrder);

        component->setColor(color);

        return component;
    }

    ComponentPtr SceneObjectFactory::createPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder)
    {
        return createPlaceholder(item, height, radius, numSectors, imageZOrder, circleZOrder, "", false, 0.0f);
    }

    ComponentPtr SceneObjectFactory::createPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha)
    {
        return boost::make_shared<PlaceholderComponent>(item,
            createPlaceholderSensor((image.empty() ? Inventory::getPlaceholderImageName(item) : image),
                (image.empty() ? false : imageFlip),
                height, radius, numSectors, imageZOrder, circleZOrder, collisionBitPlayer | collisionBitVehicle, alpha));
    }

    ComponentPtr SceneObjectFactory::createRockPlaceholder(float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha)
    {
        return boost::make_shared<PlaceholderComponent>(InventoryItemDynomite,
            createPlaceholderSensor(image, imageFlip,
                height, radius, numSectors, imageZOrder, circleZOrder, collisionBitRock, alpha));
    }

    ComponentPtr SceneObjectFactory::createVehiclePlaceholder(float radius, int numSectors, int zOrder, float alpha)
    {
        Color color = settings.placeholder.color;
        if (alpha > 0.0f) {
            color.rgba[3] = alpha;
        }

        SceneObjectPtr sensor = createCircleSensor(radius, collisionBitPlayer | collisionBitVehicle);

        sensor->addComponent(createCircle(radius, numSectors, color, zOrder));

        RenderDottedCircleComponentPtr dcc =
            boost::make_shared<RenderDottedCircleComponent>(b2Vec2_zero, 0.3f,
                assetManager.getDrawable("common2/dot.png"), zOrder);

        dcc->setColor(color);
        dcc->setRadius(radius);
        dcc->setDotVelocity(5.0f, radius);
        dcc->setDotDistance(0.5f, radius);

        sensor->addComponent(dcc);

        return boost::make_shared<PlaceholderComponent>(InventoryItemDynomite, sensor);
    }

    ComponentPtr SceneObjectFactory::createAllyPlaceholder(InventoryItem item, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, const std::string& image, bool imageFlip, float alpha)
    {
        SceneObjectPtr sensor = createPlaceholderSensor((image.empty() ? Inventory::getPlaceholderImageName(item) : image),
            (image.empty() ? false : imageFlip),
            height, radius, numSectors, imageZOrder, circleZOrder, collisionBitAlly, alpha);
        sensor->script_setBodyType(b2_dynamicBody);
        sensor->setSleepingAllowed(false);

        return boost::make_shared<PlaceholderComponent>(item, sensor);
    }

    SceneObjectPtr SceneObjectFactory::createPlaceholderSensor(const std::string& image, bool imageFlip, float height, float radius, int numSectors, int imageZOrder, int circleZOrder, UInt32 maskBits, float alpha)
    {
        Color color = settings.placeholder.color;
        if (alpha > 0.0f) {
            color.rgba[3] = alpha;
        }

        SceneObjectPtr sensor = createCircleSensor(radius, maskBits);

        sensor->addComponent(createCircle(radius, numSectors, color, circleZOrder));

        DrawablePtr drawable = assetManager.getDrawable(image);

        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, imageZOrder);

        rc->setColor(color);
        rc->setFlip(imageFlip);

        sensor->addComponent(rc);

        RenderDottedCircleComponentPtr dcc =
            boost::make_shared<RenderDottedCircleComponent>(b2Vec2_zero, 0.3f,
                assetManager.getDrawable("common2/dot.png"), circleZOrder);

        dcc->setColor(color);
        dcc->setRadius(radius);
        dcc->setDotVelocity(5.0f, radius);
        dcc->setDotDistance(0.5f, radius);

        sensor->addComponent(dcc);

        return sensor;
    }

    ComponentPtr SceneObjectFactory::createTimebomb(InventoryItem item, float height, int imageZOrder, int circleZOrder)
    {
        static const float radius = 13.0f;
        static const float numSectors = 30.0f;

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->addComponent(createCircle(radius, numSectors,
            settings.timebomb.color, circleZOrder));

        DrawablePtr drawable = assetManager.getDrawable(Inventory::getImageName(item));

        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2_zero, -b2_pi / 2.0f, width, height,
                drawable, imageZOrder);

        obj->addComponent(rc);

        RenderDottedCircleComponentPtr dcc =
            boost::make_shared<RenderDottedCircleComponent>(b2Vec2_zero, 0.3f,
                assetManager.getDrawable("common2/dot.png"), circleZOrder);

        dcc->setColor(settings.timebomb.color);
        dcc->setRadius(radius);
        dcc->setDotVelocity(5.0f, radius);
        dcc->setDotDistance(0.5f, radius);

        obj->addComponent(dcc);

        return boost::make_shared<TimebombComponent>(obj, height);
    }

    SceneObjectPtr SceneObjectFactory::createBossSkull()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_skull.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setPropagateDamage(false);
        }

        asset->root()->setLife(settings.bossSkull.life);
        asset->root()->setMaxLife(settings.bossSkull.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 3.0f), 0, 8.0f, 2.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        std::vector<BossSkullGunComponentPtr> guns;

        /*
         * Setup front guns.
         */

        const char* frontGunNames[] = { "gun_front_left", "gun_front_right" };
        const char* frontGunJointNames[] = { "gun_front_left_joint", "gun_front_right_joint" };

        for (size_t i = 0; i < sizeof(frontGunNames)/sizeof(frontGunNames[0]); ++i) {
            SceneObjectPtr obj = asset->object(frontGunNames[i]);

            std::vector<WeaponComponentPtr> weapons;

            for (int j = 0; j < asset->world()->body(frontGunNames[i])->numImages(); ++j) {
                const RUBEImagePtr& image = asset->world()->body(frontGunNames[i])->image(j);

                if (image->name() != "missile") {
                    continue;
                }

                WeaponBlasterComponentPtr weapon = boost::make_shared<WeaponBlasterComponent>(true, SceneObjectTypeEnemyMissile);

                weapon->setTransform(b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f)));
                weapon->setDamage(settings.bossSkull.gunFrontShootDamage);
                weapon->setVelocity(settings.bossSkull.gunFrontShootVelocity);
                weapon->setTurns(1);
                weapon->setShotsPerTurn(1);
                weapon->setLoopDelay(settings.bossSkull.gunFrontShootDelay);
                weapon->setHaveSound(false);

                if (weapons.empty()) {
                    weapon->setHaveSound(true);
                }

                weapons.push_back(weapon);
            }

            BossSkullGunComponentPtr c = boost::make_shared<BossSkullGunComponent>(weapons,
                settings.bossSkull.gunFrontTurnSpeed,
                settings.bossSkull.gunFrontShootAngle,
                settings.bossSkull.gunFrontShootRadius,
                frontGunJointNames[i]);

            obj->addComponent(c);

            guns.push_back(c);
        }

        /*
         * Setup back guns.
         */

        const char* backGunNames[] = { "gun_back_left", "gun_back_right" };
        const char* backGunJointNames[] = { "gun_back_left_joint", "gun_back_right_joint" };

        for (size_t i = 0; i < sizeof(backGunNames)/sizeof(backGunNames[0]); ++i) {
            SceneObjectPtr obj = asset->object(backGunNames[i]);

            std::vector<WeaponComponentPtr> weapons;

            for (int j = 0; j < asset->world()->body(backGunNames[i])->numImages(); ++j) {
                const RUBEImagePtr& image = asset->world()->body(backGunNames[i])->image(j);

                if (image->name() != "missile") {
                    continue;
                }

                WeaponBlasterComponentPtr weapon = boost::make_shared<WeaponBlasterComponent>(false, SceneObjectTypeEnemyMissile);

                weapon->setTransform(b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f)));
                weapon->setDamage(settings.bossSkull.gunBackShootDamage);
                weapon->setVelocity(settings.bossSkull.gunBackShootVelocity);
                weapon->setTurns(1);
                weapon->setShotsPerTurn(1);
                weapon->setLoopDelay(settings.bossSkull.gunBackShootDelay);
                weapon->setHaveSound(false);

                if (weapons.empty()) {
                    weapon->setHaveSound(true);
                }

                weapons.push_back(weapon);
            }

            BossSkullGunComponentPtr c = boost::make_shared<BossSkullGunComponent>(weapons,
                settings.bossSkull.gunBackTurnSpeed,
                settings.bossSkull.gunBackShootAngle,
                settings.bossSkull.gunBackShootRadius,
                backGunJointNames[i]);

            obj->addComponent(c);

            guns.push_back(c);
        }

        /*
         * Setup mid guns.
         */

        std::vector<BossSkullGunComponentPtr> flamethrowers;

        const char* midGunNames[] = { "gun_mid_left", "gun_mid_right" };
        const char* midGunJointNames[] = { "gun_mid_left_joint", "gun_mid_right_joint" };

        for (size_t i = 0; i < sizeof(midGunNames)/sizeof(midGunNames[0]); ++i) {
            SceneObjectPtr obj = asset->object(midGunNames[i]);

            std::vector<WeaponComponentPtr> weapons;

            for (int j = 0; j < asset->world()->body(midGunNames[i])->numImages(); ++j) {
                const RUBEImagePtr& image = asset->world()->body(midGunNames[i])->image(j);

                if (image->name() != "missile") {
                    continue;
                }

                WeaponFlamethrowerComponentPtr weapon = boost::make_shared<WeaponFlamethrowerComponent>(SceneObjectTypeEnemyMissile);

                weapon->setTransform(b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f)));
                weapon->setDamage(settings.bossSkull.gunMidDamage);
                weapon->setHaveSound(false);

                if (weapons.empty()) {
                    weapon->setHaveSound(true);
                }

                weapons.push_back(weapon);
            }

            BossSkullGunComponentPtr c = boost::make_shared<BossSkullGunComponent>(weapons,
                settings.bossSkull.gunMidTurnSpeed,
                settings.bossSkull.gunMidShootAngle,
                settings.bossSkull.gunMidShootRadius,
                midGunJointNames[i]);

            obj->addComponent(c);

            guns.push_back(c);
            flamethrowers.push_back(c);
        }

        BossSkullComponentPtr c = boost::make_shared<BossSkullComponent>(guns, flamethrowers);

        asset->root()->addComponent(c);

        asset->root()->setBlastDamageMultiplier(settings.bossSkull.blastDamageMultiplier);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSpider()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("spider.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
        }

        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());

        torsoAc->addAnimation(AnimationDefault, "bigspider_def");
        torsoAc->addAnimation(SpiderComponent::AnimationAttack, "bigspider_attack");

        torsoAc->startAnimation(AnimationDefault);

        asset->root()->addComponent(torsoAc);

        SceneObjectPtr legs[] = {
            asset->object("leg_front1_top"),
            asset->object("leg_front1_bottom"),
            asset->object("leg_front2_top"),
            asset->object("leg_front2_bottom"),
            asset->object("leg_back1_top"),
            asset->object("leg_back1_bottom"),
            asset->object("leg_back2_top"),
            asset->object("leg_back2_bottom")
        };

        b2Vec2 legTips[] = {
            asset->world()->body("leg_front1_top")->image("tip")->pos(),
            asset->world()->body("leg_front1_bottom")->image("tip")->pos(),
            asset->world()->body("leg_front2_top")->image("tip")->pos(),
            asset->world()->body("leg_front2_bottom")->image("tip")->pos(),
            asset->world()->body("leg_back1_top")->image("tip")->pos(),
            asset->world()->body("leg_back1_bottom")->image("tip")->pos(),
            asset->world()->body("leg_back2_top")->image("tip")->pos(),
            asset->world()->body("leg_back2_bottom")->image("tip")->pos()
        };

        SpiderComponentPtr c = boost::make_shared<SpiderComponent>(legs, legTips,
            asset->rootBody()->image("tip")->pos(), torsoAc);

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBabySpider1()
    {
        return createBabySpider(1.0f,
            settings.babySpider1.life, settings.babySpider1.walkSpeed,
            settings.babySpider1.biteDamage, false);
    }

    SceneObjectPtr SceneObjectFactory::createBabySpider2()
    {
        return createBabySpider(0.7f,
            settings.babySpider2.life, settings.babySpider2.walkSpeed,
            settings.babySpider2.biteDamage, true);
    }

    SceneObjectPtr SceneObjectFactory::createBabySpider(float scale, float life, float walkSpeed, float biteDamage, bool isSmall)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("baby_spider.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);
        asset->root()->setLife(life);
        asset->root()->setFireDamageMultiplier(settings.babySpider.fireDamageMultiplier);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        rc->setHeight(rc->height() * scale);

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        if (isSmall) {
            ac->addAnimation(AnimationDefault, "spider2_def", 10.0f / walkSpeed);
            ac->addAnimation(BabySpiderComponent::AnimationDie, "spider2_die");
        } else {
            ac->addAnimation(AnimationDefault, "spider_def", 10.0f / walkSpeed);
            ac->addAnimation(BabySpiderComponent::AnimationDie, "spider_die");
        }

        ac->startAnimation(AnimationDefault);

        ac->setPaused(true);

        asset->root()->addComponent(ac);

        BabySpiderComponentPtr thinkComponent =
            boost::make_shared<BabySpiderComponent>(ac, rc, walkSpeed, biteDamage);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.babySpider.damageImpulseThreshold);
            component->setMultiplier(settings.babySpider.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSpiderNest(float probabilities[1], bool dead)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("spider_nest.json");

        asset->root()->setType(SceneObjectTypeEnemyBuilding);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setLife(settings.spiderNest.life);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "spidernest_def");
        ac->addAnimation(SpiderNestComponent::AnimationDie, "spidernest_die");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        b2Transform spawn[3];
        int j = 0;

        for (int i = 0; i < asset->rootBody()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->rootBody()->image(i);

            if (image->name() != "spawn") {
                continue;
            }

            if (j < 3) {
                spawn[j++] = b2Transform(image->pos(), b2Rot(image->angle() + b2_pi / 2.0f));
            }
        }

        SpiderNestComponentPtr thinkComponent =
            boost::make_shared<SpiderNestComponent>(ac, spawn,
                settings.spiderNest.radius, probabilities);

        if (dead) {
            thinkComponent->setDead();
        }

        asset->root()->addComponent(thinkComponent);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.spiderNest.damageImpulseThreshold);
            component->setMultiplier(settings.spiderNest.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createWeaponTrap(WeaponType weaponType, SceneObjectType objectType)
    {
        b2BodyDef bodyDef;

        bodyDef.type = b2_kinematicBody;

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", bodyDef);

        ComponentPtr component =
            boost::make_shared<PhysicsBodyComponent>(body);

        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setBodyDef(body->bodyDef());

        obj->addComponent(component);

        WeaponComponentPtr weapon;

        switch (weaponType) {
        case WeaponTypeGG:
            weapon = boost::make_shared<WeaponGGComponent>(objectType);
            break;
        case WeaponTypeBlaster:
            weapon = boost::make_shared<WeaponBlasterComponent>(false, objectType);
            break;
        case WeaponTypeShotgun:
            weapon = boost::make_shared<WeaponShotgunComponent>(false, objectType);
            break;
        case WeaponTypeRLauncher:
            weapon = boost::make_shared<WeaponRLauncherComponent>(objectType);
            break;
        case WeaponTypeFireball:
            weapon = boost::make_shared<WeaponFireballComponent>(objectType);
            break;
        case WeaponTypeFlamethrower:
            weapon = boost::make_shared<WeaponFlamethrowerComponent>(objectType);
            break;
        case WeaponTypePistol:
            weapon = boost::make_shared<WeaponPistolComponent>(objectType);
            break;
        case WeaponTypeLGun:
            weapon = boost::make_shared<WeaponLGunComponent>(objectType);
            break;
        default:
            assert(false);
            break;
        }

        obj->addComponent(weapon);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBossQueen()
    {
        SceneObjectTypes types;

        types.set(SceneObjectTypePlayer);

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_queen.json");

        asset->root()->setLife(settings.bossQueen.life);
        asset->root()->setMaxLife(settings.bossQueen.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 1.0f), 0, 12.0f, 2.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        asset->root()->setMaterial(MaterialFlesh);

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setCollisionImpulseMultiplier(0.0f);
            asset->object(i)->setPropagateDamage(false);

            if (asset->object(i)->name() == "front_leg") {
                CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
                c->setFilter(types);
                c->setDamage(settings.bossQueen.legDamage);
                asset->object(i)->addComponent(c);
            }
        }

        RenderQuadComponentPtr headRc = asset->root()->findComponentByName<RenderQuadComponent>("head");

        AnimationComponentPtr headAc = boost::make_shared<AnimationComponent>(headRc->drawable());

        headAc->addAnimation(AnimationDefault, "queen_def");
        headAc->addAnimation(BossQueenComponent::AnimationAttack, "queen_attack");

        headAc->startAnimation(AnimationDefault);

        asset->root()->addComponent(headAc);

        SceneObjectPtr legs[] = {
            asset->object("leg1_top"),
            asset->object("leg1_bottom"),
            asset->object("leg2_top"),
            asset->object("leg2_bottom"),
            asset->object("leg3_top"),
            asset->object("leg3_bottom"),
            asset->object("leg4_top"),
            asset->object("leg4_bottom")
        };

        for (size_t i = 0; i < 4; ++i) {
            CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
            c->setFilter(types);
            c->setDamage(settings.bossQueen.legDamage);
            legs[i]->addComponent(c);
        }

        b2Vec2 legTips[] = {
            asset->world()->body("leg1_top")->image("tip")->pos(),
            asset->world()->body("leg1_bottom")->image("tip")->pos(),
            asset->world()->body("leg2_top")->image("tip")->pos(),
            asset->world()->body("leg2_bottom")->image("tip")->pos(),
            asset->world()->body("leg3_top")->image("tip")->pos(),
            asset->world()->body("leg3_bottom")->image("tip")->pos(),
            asset->world()->body("leg4_top")->image("tip")->pos(),
            asset->world()->body("leg4_bottom")->image("tip")->pos()
        };

        b2Vec2 cover[] = {
            asset->rootBody()->image("leg1_top_cover")->pos(),
            asset->rootBody()->image("leg1_bottom_cover")->pos(),
            asset->rootBody()->image("leg2_top_cover")->pos(),
            asset->rootBody()->image("leg2_bottom_cover")->pos()
        };

        Points spawn;

        for (int i = 0; i < asset->rootBody()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->rootBody()->image(i);
            if (image->name() == "spawn") {
                spawn.push_back(image->pos());
            }
        }

        b2Vec2 dead[] = {
            asset->rootBody()->image("leg1_top_dead")->pos(),
            asset->rootBody()->image("leg1_bottom_dead")->pos(),
            asset->rootBody()->image("leg2_top_dead")->pos(),
            asset->rootBody()->image("leg2_bottom_dead")->pos()
        };

        BossQueenComponentPtr c = boost::make_shared<BossQueenComponent>(legs, legTips,
            asset->rootBody()->image("tip")->pos(), cover, spawn, dead, headAc);

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createEnforcer(float life, float shootInterval, const Color& color,
        b2Vec2& missilePos, float& missileAngle, float& missileFlashDistance)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("enforcer.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setLife(life);
        asset->root()->setMaxLife(life);

        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");
        RenderQuadComponentPtr torsoMaskRc = asset->root()->findComponentByName<RenderQuadComponent>("torso_mask");
        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");

        torsoMaskRc->setColor(color);

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());
        AnimationComponentPtr torsoMaskAc = boost::make_shared<AnimationComponent>(torsoMaskRc->drawable());
        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());

        torsoAc->addAnimation(AnimationDefault, "enforcer_torso_def");
        torsoMaskAc->addAnimation(AnimationDefault, "enforcer_torso_mask_def");
        legsAc->addAnimation(AnimationDefault, "enforcer_legs_def");
        torsoAc->addAnimation(EnforcerComponent::AnimationDie, "enforcer_torso_die");
        torsoMaskAc->addAnimation(EnforcerComponent::AnimationDie, "enforcer_torso_mask_die");

        torsoAc->startAnimation(AnimationDefault);
        torsoMaskAc->startAnimation(AnimationDefault);
        legsAc->startAnimation(AnimationDefault);

        torsoAc->setPaused(true);
        torsoMaskAc->setPaused(true);
        legsAc->setPaused(true);

        asset->root()->addComponent(torsoAc);
        asset->root()->addComponent(torsoMaskAc);
        asset->root()->addComponent(legsAc);

        {
            EnforcerComponentPtr component = boost::make_shared<EnforcerComponent>(torsoRc, torsoMaskRc, legsRc, torsoAc, torsoMaskAc, legsAc, shootInterval);

            missilePos = asset->rootBody()->image("missile")->pos();
            missileAngle = normalizeAngle(asset->rootBody()->image("missile")->angle() + b2_pi / 2.0f);
            missileFlashDistance = asset->rootBody()->image("missile")->height() / 2.0f;

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.enforcer.damageImpulseThreshold);
            component->setMultiplier(settings.enforcer.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createEnforcer1()
    {
        b2Vec2 missilePos;
        float missileAngle;
        float missileFlashDistance;

        SceneObjectPtr obj = createEnforcer(settings.enforcer1.life,
            settings.enforcer1.shootInterval, settings.enforcer1.color,
            missilePos, missileAngle, missileFlashDistance);

        EnforcerComponentPtr component =
            obj->findComponent<EnforcerComponent>();

        WeaponPistolComponentPtr pistol =
            boost::make_shared<WeaponPistolComponent>(SceneObjectTypeEnemyMissile);

        pistol->setDamage(settings.enforcer1.shootDamage);
        pistol->setVelocity(settings.enforcer1.shootVelocity);
        pistol->setInterval(settings.enforcer1.shootInterval);
        pistol->setPos(missilePos);
        pistol->setAngle(missileAngle);
        pistol->setFlashDistance(missileFlashDistance);

        component->setWeapon(pistol);

        obj->addComponent(pistol);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createEnforcer2()
    {
        b2Vec2 missilePos;
        float missileAngle;
        float missileFlashDistance;

        SceneObjectPtr obj = createEnforcer(settings.enforcer2.life,
            settings.enforcer2.shootDelay, settings.enforcer2.color,
            missilePos, missileAngle, missileFlashDistance);

        EnforcerComponentPtr component =
            obj->findComponent<EnforcerComponent>();

        WeaponBlasterComponentPtr blaster =
            boost::make_shared<WeaponBlasterComponent>(true, SceneObjectTypeEnemyMissile);

        blaster->setDamage(settings.enforcer2.shootDamage);
        blaster->setVelocity(settings.enforcer2.shootVelocity);
        blaster->setTurns(3);
        blaster->setShotsPerTurn(3);
        blaster->setTurnInterval(settings.enforcer2.shootTurnInterval);
        blaster->setLoopDelay(settings.enforcer2.shootDelay);
        blaster->setPos(missilePos + angle2vec(missileAngle, missileFlashDistance));
        blaster->setAngle(missileAngle);
        blaster->setTweakPos(true);

        component->setWeapon(blaster);

        obj->addComponent(blaster);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createEnforcer3()
    {
        b2Vec2 missilePos;
        float missileAngle;
        float missileFlashDistance;

        SceneObjectPtr obj = createEnforcer(settings.enforcer1.life,
            settings.enforcer1.shootInterval, settings.enforcer3.color,
            missilePos, missileAngle, missileFlashDistance);

        EnforcerComponentPtr component =
            obj->findComponent<EnforcerComponent>();

        WeaponPistolComponentPtr pistol =
            boost::make_shared<WeaponPistolComponent>(SceneObjectTypeEnemyMissile);

        pistol->setDamage(settings.enforcer1.shootDamage);
        pistol->setVelocity(settings.enforcer1.shootVelocity);
        pistol->setInterval(settings.enforcer1.shootInterval);
        pistol->setPos(missilePos);
        pistol->setAngle(missileAngle);
        pistol->setFlashDistance(missileFlashDistance);

        component->setWeapon(pistol);

        obj->addComponent(pistol);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBullet(SceneObjectType type, float height, float damage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("bullet2.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        LineLightPtr light =
            asset->root()->findComponent<LightComponent>()->getLight<LineLight>("light");

        RenderTrailComponentPtr rtc =
            boost::make_shared<RenderTrailComponent>(b2Vec2_zero, b2_pi,
                assetManager.getDrawable("trail1.png", Texture::WrapModeRepeat, Texture::WrapModeClamp),
                asset->rootBody()->image("light")->zOrder());

        switch (type) {
        case SceneObjectTypePlayerMissile:
            light->setColor(settings.bullet.player[0]);
            rtc->setColor2(settings.bullet.player[1]);
            break;
        case SceneObjectTypeAllyMissile:
            light->setColor(settings.bullet.ally[0]);
            rtc->setColor2(settings.bullet.ally[1]);
            break;
        case SceneObjectTypeEnemyMissile:
        case SceneObjectTypeNeutralMissile:
            light->setColor(settings.bullet.enemy[0]);
            rtc->setColor2(settings.bullet.enemy[1]);
            break;
        default:
            assert(false);
            break;
        }

        rtc->setColor(light->color());
        rtc->setWidth1(height);
        rtc->setWidth2(height);

        asset->root()->addComponent(rtc);

        {
            ComponentPtr component = boost::make_shared<BulletComponent>(cdc,
                rtc, light, damage);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSentry1(bool folded)
    {
        SceneObjectPtr obj = createSentry(true, folded,
            settings.sentry1.life,
            settings.sentry1.shootDamage,
            settings.sentry1.shootTurnDuration,
            settings.sentry1.shootLoopDelay,
            "sentry1");

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createSentry2(bool folded)
    {
        SceneObjectPtr obj = createSentry(false, folded,
            settings.sentry2.life,
            settings.sentry2.shootDamage,
            settings.sentry2.shootTurnDuration,
            settings.sentry2.shootLoopDelay,
            "sentry2");

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createSentryGibbed(int i)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset((i == 1) ? "sentry_gibbed.json" : "sentry2_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createGorger1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("gorger.json");

        asset->root()->setType(SceneObjectTypeEnemyBuilding);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setLife(settings.gorger.life);
        asset->root()->setMaxLife(settings.gorger.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.0f), 0, 6.0f, 1.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        ac->addAnimation(AnimationDefault, "gorger_def");
        ac->addAnimation(GorgerComponent::AnimationAngry, "gorger_angry");
        ac->addAnimation(GorgerComponent::AnimationWalk, "gorger_walk");
        ac->addAnimation(GorgerComponent::AnimationMelee, "gorger_melee");
        ac->addAnimation(GorgerComponent::AnimationPreShoot, "gorger_preshoot");
        ac->addAnimation(GorgerComponent::AnimationPostShoot, "gorger_postshoot");
        ac->addAnimation(GorgerComponent::AnimationShoot, "gorger_shoot");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        {
            GorgerComponentPtr component = boost::make_shared<GorgerComponent>(ac,
                asset->rootBody()->image("melee")->pos(),
                asset->rootBody()->image("melee")->height() / 2.0f);

            for (int i = 0; i < 2; ++i) {
                std::ostringstream os;

                os << "missile" << (i + 1);

                WeaponRLauncherComponentPtr rlauncher =
                    boost::make_shared<WeaponRLauncherComponent>(SceneObjectTypeEnemyMissile);

                rlauncher->setExplosionImpulse(settings.gorger.shootExplosionImpulse);
                rlauncher->setExplosionDamage(settings.gorger.shootExplosionDamage);
                rlauncher->setVelocity(settings.gorger.shootVelocity);
                rlauncher->setInterval(0.1f);
                rlauncher->setPos(asset->rootBody()->image(os.str())->pos());
                rlauncher->setAngle(normalizeAngle(asset->rootBody()->image(os.str())->angle() + b2_pi / 2.0f));

                component->setWeapon(i, rlauncher);
                asset->root()->addComponent(rlauncher);
            }

            asset->root()->addComponent(component);
        }

        asset->root()->setBlastDamageMultiplier(settings.gorger.blastDamageMultiplier);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createRocket(SceneObjectType type, float explosionImpulse, float explosionDamage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("rocket.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        SceneObjectTypes damageTypes;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        } else {
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<RocketComponent>(cdc,
                asset->root()->findComponent<ParticleEffectComponent>(),
                explosionImpulse, explosionDamage, damageTypes);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createGorgerGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("gorger_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createProxMine(SceneObjectType type)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("proxmine.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture<RUBEFixture>("sensor")->fixtureDef().filter;

        SceneObjectTypes damageTypes;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits = collisionBitPlayer | collisionBitAlly;
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits = collisionBitEnemy | collisionBitEnemyBuilding | collisionBitEnemyRock;
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        } else {
            filter.maskBits = collisionBitPlayer | collisionBitAlly |
                collisionBitEnemy | collisionBitEnemyBuilding | collisionBitEnemyRock;
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        ProxMineComponentPtr thinkComponent = boost::make_shared<ProxMineComponent>(damageTypes);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        asset->root()->becomeDeadbody();

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createRoboArm()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("roboarm.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeRock);
        }

        RoboArmComponentPtr c = boost::make_shared<RoboArmComponent>(asset->object("tip"),
            asset->object("clamp_high"),
            asset->object("clamp_low"),
            "joint_high", "joint_low",
            asset->rootBody()->image("min")->pos().Length());

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createCrate(int i, float powerupProbabilities[PowerupTypeMax + 1], float ammoProbabilities[WeaponTypeMax + 1])
    {
        std::ostringstream os;

        os << "crate" << i << ".json";

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(os.str());

        asset->root()->setType(SceneObjectTypeGizmo);
        asset->root()->setLife(settings.crate1.life);
        asset->root()->setMaxLife(settings.crate1.life);

        {
            CrateComponentPtr component = boost::make_shared<CrateComponent>(i, powerupProbabilities, ammoProbabilities);

            asset->root()->addComponent(component);
        }

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.crate1.damageImpulseThreshold);
            component->setMultiplier(settings.crate1.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);
            types.set(SceneObjectTypeAlly);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            component->setDamageSound(audio.createSound("wood_hit1.ogg"));

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createCrateGibbed(int i)
    {
        std::ostringstream os;

        os << "crate" << i << "_gibbed.json";

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(os.str());

        for (int j = 0; j < asset->numObjects(); ++j) {
            asset->object(j)->setType(SceneObjectTypeGizmo);

            makeDebris(asset->object(j));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSentry(bool enemy,
        bool folded, float life,
        float shootDamage,
        float shootTurnDuration,
        float shootLoopDelay,
        const std::string& base)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sentry.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            if (enemy) {
                asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            } else {
                asset->object(i)->setType(SceneObjectTypeAlly);
            }
        }

        asset->root()->setLife(life);
        asset->root()->setMaxLife(life);

        RenderQuadComponentPtr torsoRc = asset->object("torso")->findComponentByName<RenderQuadComponent>("torso");
        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());
        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());

        torsoAc->addAnimation(AnimationDefault, base + "_torso_def");
        legsAc->addAnimation(AnimationDefault, base + "_legs_def");
        torsoAc->addAnimation(SentryComponent::AnimationUnfold, base + "_torso_unfold");
        legsAc->addAnimation(SentryComponent::AnimationUnfold, base + "_legs_unfold");
        legsAc->addAnimation(SentryComponent::AnimationWalk, base + "_legs_walk");

        asset->root()->addComponent(torsoAc);
        asset->root()->addComponent(legsAc);

        {
            SentryComponentPtr component = boost::make_shared<SentryComponent>(asset->object("torso"), torsoRc, legsRc, torsoAc, legsAc, folded);

            component->setShootDelay(shootLoopDelay * 1.5f);

            WeaponMachineGunComponentPtr mgun =
                boost::make_shared<WeaponMachineGunComponent>(enemy ? SceneObjectTypeEnemyMissile : SceneObjectTypeAllyMissile);

            mgun->setDamage(shootDamage);
            mgun->setVelocity(settings.sentry.shootVelocity);
            mgun->setSpreadAngle(settings.sentry.shootSpreadAngle);
            mgun->setTurnInterval(settings.sentry.shootTurnInterval);
            mgun->setTurnDuration(shootTurnDuration);
            mgun->setLoopDelay(shootLoopDelay);
            mgun->setPos(asset->world()->body("torso")->image("missile")->pos());
            mgun->setAngle(normalizeAngle(asset->world()->body("torso")->image("missile")->angle() + b2_pi / 2.0f));
            mgun->setFlashDistance(asset->world()->body("torso")->image("missile")->height() / 2.0f);
            mgun->setTweakPos(true);

            component->setWeapon(mgun);

            asset->root()->addComponent(component);
            asset->object("torso")->addComponent(mgun);
        }

        if (!enemy) {
            b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

            filter.categoryBits = collisionBitAlly;

            asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createOrbo()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("orbo.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setLife(settings.orbo.life);
        asset->root()->setMaxLife(settings.orbo.life);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "orbo_def");
        ac->addAnimation(OrboComponent::AnimationExtend, "orbo_extend");
        ac->addAnimation(OrboComponent::AnimationRetract, "orbo_retract");

        ac->startAnimation(AnimationDefault);

        ac->setPaused(true);

        asset->root()->addComponent(ac);

        OrboComponentPtr thinkComponent =
            boost::make_shared<OrboComponent>(ac);

        for (int i = 0; i < 2; ++i) {
            std::ostringstream os;

            os << "missile" << (i + 1);

            WeaponMachineGunComponentPtr mgun =
                boost::make_shared<WeaponMachineGunComponent>(SceneObjectTypeEnemyMissile);

            mgun->setDamage(settings.orbo.shootDamage);
            mgun->setVelocity(settings.orbo.shootVelocity);
            mgun->setSpreadAngle(settings.orbo.shootSpreadAngle);
            mgun->setTurnInterval(settings.orbo.shootTurnInterval);
            mgun->setTurnDuration(settings.orbo.shootTurnDuration);
            mgun->setLoopDelay(settings.orbo.shootLoopDelay);
            mgun->setPos(asset->rootBody()->image(os.str())->pos());
            mgun->setAngle(normalizeAngle(asset->rootBody()->image(os.str())->angle() + b2_pi / 2.0f));
            mgun->setFlashDistance(asset->rootBody()->image(os.str())->height() / 2.0f);

            if (i != 0) {
                mgun->setHaveSound(false);
            }

            thinkComponent->setWeapon(i, mgun);
            asset->root()->addComponent(mgun);
        }

        asset->root()->addComponent(thinkComponent);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createOrboGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("orbo_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createWarder()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("warder.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);
        asset->root()->setLife(settings.warder.life);
        asset->root()->setMaxLife(settings.warder.life);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "warder_def");
        ac->addAnimation(WarderComponent::AnimationWalk, "warder_walk");
        ac->addAnimation(WarderComponent::AnimationMelee1, "warder_melee1");
        ac->addAnimation(WarderComponent::AnimationMelee2, "warder_melee2");
        ac->addAnimation(WarderComponent::AnimationMelee3, "warder_melee3");
        ac->addAnimation(WarderComponent::AnimationShoot, "warder_shoot");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        b2Vec2 meleePos[3] = {
            asset->rootBody()->image("melee1")->pos(),
            asset->rootBody()->image("melee2")->pos(),
            asset->rootBody()->image("melee3")->pos()
        };

        float meleeRadius[3] = {
            asset->rootBody()->image("melee1")->height() / 2.0f,
            asset->rootBody()->image("melee2")->height() / 2.0f,
            asset->rootBody()->image("melee3")->height() / 2.0f
        };

        WarderComponentPtr thinkComponent =
            boost::make_shared<WarderComponent>(ac, meleePos, meleeRadius);

        WeaponLGunComponentPtr lgun =
            boost::make_shared<WeaponLGunComponent>(SceneObjectTypeEnemyMissile);

        lgun->setDamage(settings.warder.shootDamage);
        lgun->setLength(settings.warder.shootLength);
        lgun->setImpulse(settings.warder.shootImpulse);
        lgun->setPos(asset->rootBody()->image("missile")->pos());
        lgun->setAngle(normalizeAngle(asset->rootBody()->image("missile")->angle() + b2_pi / 2.0f));
        lgun->setFlashDistance(asset->rootBody()->image("missile")->height() / 2.0f);

        thinkComponent->setWeapon(lgun);
        asset->root()->addComponent(lgun);

        asset->root()->addComponent(thinkComponent);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createWarderGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("warder_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPlasma(SceneObjectType type, float damage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("plasma.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<PlasmaComponent>(cdc,
                asset->root()->findComponent<ParticleEffectComponent>(),
                damage);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createKeeper(bool folded)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("keeper.json");

        asset->root()->setType(SceneObjectTypeEnemyBuilding);

        asset->root()->setLife(settings.keeper.life);
        asset->root()->setMaxLife(settings.keeper.life);

        DrawablePtr drawable =
            asset->root()->findComponent<RenderQuadComponent>()->drawable();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(drawable);

        {
            std::string pathBase = settings.keeper.badImages ? "badkeeper" : "keeper";

            ac->addAnimation(AnimationDefault, pathBase + "_def");
            ac->addAnimation(KeeperComponent::AnimationAngry, pathBase + "_angry");
            ac->addAnimation(KeeperComponent::AnimationWalk, pathBase + "_walk");
            ac->addAnimation(KeeperComponent::AnimationMelee, pathBase + "_melee");
            ac->addAnimation(KeeperComponent::AnimationGun, pathBase + "_gun");
            ac->addAnimation(KeeperComponent::AnimationPrePlasma, pathBase + "_preplasma");
            ac->addAnimation(KeeperComponent::AnimationPostPlasma, pathBase + "_postplasma");
            ac->addAnimation(KeeperComponent::AnimationPlasma, pathBase + "_plasma");
            ac->addAnimation(KeeperComponent::AnimationMissile, pathBase + "_missile");
            ac->addAnimation(KeeperComponent::AnimationCrawlOut, pathBase + "_crawlout");
            ac->addAnimation(KeeperComponent::AnimationDeath, pathBase + "_death");

            ac->startAnimation(AnimationDefault);

            asset->root()->addComponent(ac);
        }

        {
            KeeperComponentPtr component = boost::make_shared<KeeperComponent>(ac,
                asset->rootBody()->image("melee")->pos(),
                asset->rootBody()->image("melee")->height() / 2.0f, folded);

            WeaponMachineGunComponentPtr mgun =
                boost::make_shared<WeaponMachineGunComponent>(SceneObjectTypeEnemyMissile);

            mgun->setDamage(settings.keeper.gunDamage);
            mgun->setVelocity(settings.keeper.gunVelocity);
            mgun->setSpreadAngle(settings.keeper.gunAngle);
            mgun->setLoopDelay(settings.keeper.gunInterval);
            mgun->setPos(asset->rootBody()->image("gun")->pos());
            mgun->setAngle(normalizeAngle(asset->rootBody()->image("gun")->angle() + b2_pi / 2.0f));
            mgun->setTweakPos(true);
            mgun->setFlashQuad(false);

            component->setWeaponGun(mgun);
            asset->root()->addComponent(mgun);

            WeaponPlasmaGunComponentPtr pgun =
                boost::make_shared<WeaponPlasmaGunComponent>(SceneObjectTypeEnemyMissile);

            pgun->setDamage(settings.keeper.plasmaDamage);
            pgun->setVelocity(settings.keeper.plasmaVelocity);
            pgun->setSpreadAngle(settings.keeper.plasmaAngle);
            pgun->setNumShots(5);
            pgun->setInterval(0.0f);
            pgun->setPos(asset->rootBody()->image("plasma")->pos());
            pgun->setAngle(normalizeAngle(asset->rootBody()->image("plasma")->angle() + b2_pi / 2.0f));

            component->setWeaponPlasma(pgun);
            asset->root()->addComponent(pgun);

            WeaponRLauncherComponentPtr rlauncher =
                boost::make_shared<WeaponRLauncherComponent>(SceneObjectTypeEnemyMissile);

            rlauncher->setExplosionImpulse(settings.keeper.missileImpulse);
            rlauncher->setExplosionDamage(settings.keeper.missileDamage);
            rlauncher->setVelocity(settings.keeper.missileVelocity);
            rlauncher->setInterval(0.1f);
            rlauncher->setPos(asset->rootBody()->image("missile")->pos());
            rlauncher->setAngle(normalizeAngle(asset->rootBody()->image("missile")->angle() + b2_pi / 2.0f));

            component->setWeaponMissile(rlauncher);
            asset->root()->addComponent(rlauncher);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createHealthStation(float amount)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("health_station.json");

        amount *= settings.player.maxLife / settings.healthStation.life;

        asset->root()->setLife(amount);
        asset->root()->setMaxLife(amount);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.5f), 0, 7.0f, 1.25f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        HealthStationComponentPtr thinkComponent = boost::make_shared<HealthStationComponent>();

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createRodBundle()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("rod_bundle.json");

        asset->root()->setMaxLife(settings.rodBundle.life);

        asset->root()->setType(SceneObjectTypeEnemyBuilding);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSummon1(bool haveSound, int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("teleport1.p", b2Vec2_zero, 0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            SummonComponentPtr component =
                boost::make_shared<SummonComponent>(pec,
                    Color(0.33f, 0.66f, 1.0f, 1.0f), 11.0f, 2.0f, 2.0f, haveSound);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createSummon2(bool haveSound, int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("teleport2.p", b2Vec2_zero, 0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            SummonComponentPtr component =
                boost::make_shared<SummonComponent>(pec,
                    Color(0.33f, 0.66f, 1.0f, 1.0f), 15.0f, 2.0f, 2.0f, haveSound);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBossCoreProtector()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_coreprotector.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setCollisionImpulseMultiplier(0.0f);

            SceneObjectTypes types;

            types.set(SceneObjectTypePlayer);

            CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
            c->setFilter(types);
            c->setDamage(settings.bossCoreProtector.meleeDamage);
            c->setDamageSound(audio.createSound("player_hit.ogg"));
            c->setRoll(settings.bossCoreProtector.meleeRollAngle);
            c->setCoreProtectorMelee();
            asset->object(i)->addComponent(c);
        }

        SceneObjectPtr arm[] = {
            asset->object("arm3_upper"),
            asset->object("arm3_lower"),
        };

        b2Vec2 armTip[] = {
            asset->world()->body("arm3_upper")->image("tip")->pos(),
            asset->world()->body("arm3_lower")->image("tip")->pos(),
        };

        b2Vec2 armElbow[] = {
            asset->world()->body("arm3_upper")->image("elbow")->pos(),
            asset->world()->body("arm3_lower")->image("elbow")->pos(),
        };

        b2Vec2 armInitPos[] = {
            asset->rootBody()->image("init_upper")->pos(),
            asset->rootBody()->image("init_lower")->pos()
        };

        b2Vec2 armAimPos[] = {
            asset->rootBody()->image("aim_upper")->pos(),
            asset->rootBody()->image("aim_lower")->pos()
        };

        b2Vec2 armWeaponPos[] = {
            asset->rootBody()->image("weapon_upper")->pos(),
            asset->rootBody()->image("weapon_lower")->pos()
        };

        b2Vec2 armAngryPos[][2] = {
            {
                asset->rootBody()->image("angry_upper1")->pos(),
                asset->rootBody()->image("angry_lower1")->pos()
            },
            {
                asset->rootBody()->image("angry_upper2")->pos(),
                asset->rootBody()->image("angry_lower2")->pos()
            }
        };

        b2Vec2 armDiePos[][2] = {
            {
                asset->rootBody()->image("die_upper1")->pos(),
                asset->rootBody()->image("die_lower1")->pos()
            },
            {
                asset->rootBody()->image("die_upper2")->pos(),
                asset->rootBody()->image("die_lower2")->pos()
            }
        };

        WeaponLGunComponentPtr lgun =
            boost::make_shared<WeaponLGunComponent>(SceneObjectTypeEnemyMissile);

        lgun->setDamage(settings.bossCoreProtector.shootDamage);
        lgun->setLength(settings.bossCoreProtector.shootLength);
        lgun->setImpulse(settings.bossCoreProtector.shootImpulse);
        lgun->setPos(asset->world()->body("head")->image("missile")->pos());
        lgun->setAngle(normalizeAngle(asset->world()->body("head")->image("missile")->angle() + b2_pi / 2.0f));
        lgun->setFlashDistance(asset->world()->body("head")->image("missile")->height() / 2.0f);

        asset->object("head")->addComponent(lgun);

        BossCoreProtectorComponentPtr c =
            boost::make_shared<BossCoreProtectorComponent>(asset->object("head"),
                arm, armTip, armElbow, armInitPos, armAimPos, armWeaponPos,
                armAngryPos, armDiePos);

        c->setWeapon(lgun);

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createToxicSplash1(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("toxic1.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ToxicSplashComponentPtr component =
                boost::make_shared<ToxicSplashComponent>(pec);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createToxicSplash2(int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("toxic3.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ToxicSplashComponentPtr component =
                boost::make_shared<ToxicSplashComponent>(pec);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createToxicCloud1(float damage, float damageTimeout, int zOrder)
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("toxic2.p",
                                           b2Vec2(0.0f, 0.0f),
                                           0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            ToxicCloudComponentPtr component =
                boost::make_shared<ToxicCloudComponent>(pec, 12.0f, 4.0f, 4.0f, damage, damageTimeout, 8.0f);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createMissileHit1(const SceneObjectPtr& obj, const b2Vec2& pos)
    {
        return createMissileHit1(obj->material(), pos);
    }

    SceneObjectPtr SceneObjectFactory::createMissileHit1(Material material, const b2Vec2& pos)
    {
        SceneObjectPtr hit;

        if (material == MaterialFlesh) {
            hit = sceneObjectFactory.createBlood1(1.5f, zOrderExplosion);

            hit->setPos(pos);
            hit->setAngle(getRandom(0.0f, 2.0f * b2_pi));
        } else {
            hit = sceneObjectFactory.createExplosion2(zOrderExplosion);

            hit->setPos(pos);
        }

        return hit;
    }

    SceneObjectPtr SceneObjectFactory::createAcidWorm1(bool folded, bool foldable, float unfoldRadius)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("acidworm.json");

        asset->root()->setLife(settings.acidworm.life);
        asset->root()->setMaxLife(settings.acidworm.life);

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setMaterial(MaterialFlesh);
        }

        RenderQuadComponentPtr headRc = asset->object("head")->findComponentByName<RenderQuadComponent>("head");
        AnimationComponentPtr headAc = boost::make_shared<AnimationComponent>(headRc->drawable());

        headAc->addAnimation(AnimationDefault, "acidworm_def");
        headAc->addAnimation(AcidWormComponent::AnimationAttack, "acidworm_attack");

        headAc->startAnimation(AnimationDefault);

        asset->object("head")->addComponent(headAc);

        {
            WeaponAcidballComponentPtr weapon =
                boost::make_shared<WeaponAcidballComponent>(SceneObjectTypeEnemyMissile);

            weapon->setExplosionTimeout(settings.acidworm.explosionTimeout);
            weapon->setExplosionImpulse(settings.acidworm.explosionImpulse);
            weapon->setExplosionDamage(settings.acidworm.explosionDamage);
            weapon->setToxicDamage(settings.acidworm.toxicDamage);
            weapon->setToxicDamageTimeout(settings.acidworm.toxicDamageTimeout);
            weapon->setVelocity(settings.acidworm.shootVelocity);
            weapon->setPos(asset->world()->body("head")->image("missile")->pos());
            weapon->setAngle(normalizeAngle(asset->world()->body("head")->image("missile")->angle() + b2_pi / 2.0f));
            weapon->setAcidWormOwner();

            asset->object("head")->addComponent(weapon);

            AcidWormComponentPtr component =
                boost::make_shared<AcidWormComponent>(asset->object("head"), asset->object("part5"), folded, foldable, unfoldRadius, headAc);

            component->setWeapon(weapon);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createAcidball(SceneObjectType type)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("acidball.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        SceneObjectTypes damageTypes;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        } else {
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            AcidballComponentPtr component = boost::make_shared<AcidballComponent>(cdc);

            component->setExplosionDamageTypes(damageTypes);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createCentipede1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("centipede.json");

        asset->root()->setLife(settings.centipede.life);
        asset->root()->setMaxLife(settings.centipede.life);

        asset->root()->setBlastDamageMultiplier(1.0f / 5.0f);

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setMaterial(MaterialFlesh);
            asset->object(i)->setBlastDamageMultiplier(1.0f / 5.0f);
        }

        std::vector<AnimationComponentPtr> acs;

        for (int i = 1; i <= 9; ++i) {
            std::ostringstream os;
            os << "part" << i;

            SceneObjectPtr part = asset->object(os.str());

            {
                RenderQuadComponentPtr rc = part->findComponentByName<RenderQuadComponent>("upperleg");
                AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

                acs.push_back(ac);

                if ((i % 2) == 0) {
                    ac->addAnimation(AnimationDefault, "centipede_upperleg1");
                } else {
                    ac->addAnimation(AnimationDefault, "centipede_upperleg2");
                }

                ac->startAnimation(AnimationDefault);
                ac->setPaused(true);

                part->addComponent(ac);
            }

            {
                RenderQuadComponentPtr rc = part->findComponentByName<RenderQuadComponent>("lowerleg");
                AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

                acs.push_back(ac);

                if ((i % 2) != 0) {
                    ac->addAnimation(AnimationDefault, "centipede_lowerleg1");
                } else {
                    ac->addAnimation(AnimationDefault, "centipede_lowerleg2");
                }

                ac->startAnimation(AnimationDefault);
                ac->setPaused(true);

                part->addComponent(ac);
            }
        }

        RenderQuadComponentPtr headRc = asset->root()->findComponentByName<RenderQuadComponent>("part");
        AnimationComponentPtr headAc = boost::make_shared<AnimationComponent>(headRc->drawable());

        headAc->addAnimation(AnimationDefault, "centipede_def");
        headAc->addAnimation(CentipedeComponent::AnimationAttack, "centipede_attack");

        headAc->startAnimation(AnimationDefault);

        asset->root()->addComponent(headAc);

        {
            WeaponSpitterComponentPtr weapon =
                boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.centipede.spitDamage);
            weapon->setVelocity(settings.centipede.spitVelocity);
            weapon->setSpreadAngle(settings.centipede.spitAngle);
            weapon->setMinShots(settings.centipede.spitMinShots);
            weapon->setMaxShots(settings.centipede.spitMaxShots);
            weapon->setTotalShots(settings.centipede.spitTotalShots);
            weapon->setShotInterval(settings.centipede.spitInterval);
            weapon->setInterval(0.0f);
            weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 0.5f));
            weapon->setMaxColor(Color(0.5f, 1.0f, 0.0f, 0.7f));
            weapon->setMinHeight(0.5f);
            weapon->setMaxHeight(1.0f);
            weapon->setPos(asset->rootBody()->image("missile")->pos());
            weapon->setAngle(normalizeAngle(asset->rootBody()->image("missile")->angle() + b2_pi / 2.0f));

            asset->root()->addComponent(weapon);

            CentipedeComponentPtr component = boost::make_shared<CentipedeComponent>(acs, headAc);

            component->setWeapon(weapon);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSpit(int i, SceneObjectType type, float damage, float height, const Color& color)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        std::ostringstream os;
        os << "spit" << i << ".json";

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(os.str());

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        Color tmpColor = color;
        tmpColor.rgba[3] = 1.0f;

        int j = 1;

        LightComponentPtr lc = asset->root()->findComponent<LightComponent>();

        float factor = 0.0f;

        while (true) {
            os.str("");
            os << "image" << j;

            RenderQuadComponentPtr rc = asset->root()->findComponentByName<RenderQuadComponent>(os.str());

            if (!rc) {
                break;
            }

            rc->setColor(tmpColor);

            if (j == 1) {
                factor = height / rc->height();
            }

            rc->setHeight(factor * rc->height());
            rc->setPos(factor * rc->pos());

            os.str("");
            os << "light" << j++;

            PointLightPtr light = lc->getLight<PointLight>(os.str());

            light->setColor(color);

            light->setPos(factor * light->pos());
            light->setDistance(factor * light->distance());
        }

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<SpitComponent>(cdc,
                damage);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBeetle1(bool shoot, bool split)
    {
        b2Vec2 missilePos;
        float missileAngle;

        Color color;
        float scale;

        if (shoot && split) {
            color = Color(0.0f, 1.0f, 0.0f);
            scale = 1.2f;
        } else if (shoot) {
            color = Color(0.19f, 0.68f, 0.37f);
            scale = 1.0f;
        } else if (split) {
            color = Color(1.0f, 1.0f, 0.0f);
            scale = 1.2f;
        } else {
            color = Color(0.68f, 0.68f, 0.37f);
            scale = 1.0f;
        }

        SceneObjectPtr obj = createBeetle(scale, settings.beetle1.walkSpeed, color, missilePos, missileAngle);

        obj->setLife(settings.beetle1.life);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.beetle1.damageImpulseThreshold);
            component->setMultiplier(settings.beetle1.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            obj->addComponent(component);
        }

        BeetleComponentPtr c = obj->findComponent<BeetleComponent>();

        if (shoot) {
            WeaponSpitterComponentPtr weapon =
                boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.beetle1.spitDamage);
            weapon->setVelocity(settings.beetle1.spitVelocity);
            weapon->setSpreadAngle(settings.beetle1.spitAngle);
            weapon->setMinShots(3);
            weapon->setMaxShots(3);
            weapon->setTotalShots(3);
            weapon->setShotInterval(0.0f);
            weapon->setInterval(0.0f);
            weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
            weapon->setMaxColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
            weapon->setMinHeight(0.9f);
            weapon->setMaxHeight(0.9f);
            weapon->setPos(missilePos);
            weapon->setAngle(normalizeAngle(missileAngle + b2_pi / 2.0f));
            weapon->setSpreadRandom(false);
            weapon->setI(2);

            obj->addComponent(weapon);

            c->setWeapon(weapon);
        }

        c->setSplit(split);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBeetle2()
    {
        b2Vec2 missilePos;
        float missileAngle;

        SceneObjectPtr obj = createBeetle(0.7f, settings.beetle2.walkSpeed, Color(1.0f, 0.0f, 0.0f), missilePos, missileAngle);

        obj->setLife(settings.beetle2.life);

        {
            CollisionDamageComponentPtr component =
                boost::make_shared<CollisionDamageComponent>();

            component->setImpulseThreshold(settings.beetle2.damageImpulseThreshold);
            component->setMultiplier(settings.beetle2.damageMultiplier);

            SceneObjectTypes types;

            types.set(SceneObjectTypeTerrain);
            types.set(SceneObjectTypeRock);
            types.set(SceneObjectTypeEnemy);
            types.set(SceneObjectTypeEnemyBuilding);
            types.set(SceneObjectTypeGizmo);
            types.set(SceneObjectTypeVehicle);

            component->setFilter(types);

            obj->addComponent(component);
        }

        BeetleComponentPtr c = obj->findComponent<BeetleComponent>();

        c->setKamikaze(true);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBeetle(float scale, float walkSpeed, const Color& color, b2Vec2& missilePos, float& missileAngle)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("beetle.json");

        missilePos = asset->rootBody()->image("missile")->pos();
        missileAngle = asset->rootBody()->image("missile")->angle();

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);

        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");
        RenderQuadComponentPtr torsoMaskRc = asset->root()->findComponentByName<RenderQuadComponent>("torso_mask");

        torsoRc->setHeight(torsoRc->height() * scale);
        torsoMaskRc->setHeight(torsoMaskRc->height() * scale);
        torsoMaskRc->setColor(color);

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());
        AnimationComponentPtr torsoMaskAc = boost::make_shared<AnimationComponent>(torsoMaskRc->drawable());

        torsoAc->addAnimation(AnimationDefault, "beetle_def", 10.0f / walkSpeed);
        torsoMaskAc->addAnimation(AnimationDefault, "beetle_mask_def", 10.0f / walkSpeed);

        torsoAc->addAnimation(BeetleComponent::AnimationDie, "beetle_die");
        torsoMaskAc->addAnimation(BeetleComponent::AnimationDie, "beetle_mask_die");

        torsoAc->startAnimation(AnimationDefault);
        torsoMaskAc->startAnimation(AnimationDefault);

        torsoAc->setPaused(true);
        torsoMaskAc->setPaused(true);

        asset->root()->addComponent(torsoAc);
        asset->root()->addComponent(torsoMaskAc);

        BeetleComponentPtr thinkComponent =
            boost::make_shared<BeetleComponent>(torsoAc, torsoMaskAc,
                torsoRc, torsoMaskRc);

        asset->root()->addComponent(thinkComponent);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBeetle1Gibbed(bool shoot, const Color& color)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("beetle_gibbed.json");

        float factor = shoot ? 1.2f : 1.0f;

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setMaterial(MaterialFlesh);

            RenderQuadComponentPtr rc = asset->object(i)->findComponent<RenderQuadComponent>();

            rc->setHeight(factor * rc->height());
            rc->setPos(factor * rc->pos());

            if ((asset->object(i) == asset->root()) || (asset->object(i)->name() == "tint_gib")) {
                rc->setColor(color);
            }
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBoat()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boat.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeVehicle);
        }

        BoatComponentPtr c = boost::make_shared<BoatComponent>(asset->object("gun"), "gun_joint");

        WeaponLGunComponentPtr lgun =
            boost::make_shared<WeaponLGunComponent>(SceneObjectTypePlayerMissile);

        lgun->setDamage(settings.boat.shootDamage);
        lgun->setLength(settings.boat.shootLength);
        lgun->setImpulse(settings.boat.shootImpulse);
        lgun->setPos(asset->world()->body("gun")->image("missile")->pos());
        lgun->setAngle(normalizeAngle(asset->world()->body("gun")->image("missile")->angle() + b2_pi / 2.0f));
        lgun->setFlashDistance(asset->world()->body("gun")->image("missile")->height() / 2.0f);
        lgun->setI(1);

        c->setWeapon(lgun);
        asset->object("gun")->addComponent(lgun);

        asset->root()->addComponent(c);

        return asset->root();
    }

    ComponentPtr SceneObjectFactory::script_createQuad(const std::string& image, float height, int zOrder)
    {
        DrawablePtr drawable = assetManager.getDrawable(image);

        float width = height * drawable->image().aspect();

        return boost::make_shared<RenderQuadComponent>(b2Vec2_zero, 0.0f, width, height,
            drawable, zOrder);
    }

    SceneObjectPtr SceneObjectFactory::createPowerGen(bool rusted)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("powergen.json");

        asset->root()->setMaxLife(settings.powerGen1.life);
        asset->root()->setLife(settings.powerGen1.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 3.5f), 0, 8.0f, 1.2f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        asset->root()->setType(SceneObjectTypeEnemyBuilding);

        RenderQuadComponentPtr rc = asset->root()->findComponentByName<RenderQuadComponent>("root");

        rc->drawable()->setImage(assetManager.getImage(rusted ? "prison1/powergen1.png" : "prison1/powergen2.png"));

        RenderQuadComponentPtr lampRc = asset->root()->findComponentByName<RenderQuadComponent>("lamp");

        LightComponentPtr lc = asset->root()->findComponent<LightComponent>();

        LightPtr fireLight[] = {
            lc->getLight<Light>("light1"),
            lc->getLight<Light>("light2"),
            lc->getLight<Light>("light3")
        };

        PowerGenComponentPtr c =
            boost::make_shared<PowerGenComponent>(rc, lampRc, lc->getLight<Light>("lamp_light"), fireLight);

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerGen2(bool rusted)
    {
        SceneObjectPtr obj = createPowerGen(rusted);

        obj->setMaxLife(settings.powerGen2.life);
        obj->setLife(settings.powerGen2.life);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBossSquid()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_squid.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setMaterial(MaterialFlesh);
            asset->object(i)->setPropagateDamage(false);
        }

        asset->object("blocker")->setType(SceneObjectTypeBlocker);

        asset->root()->setLife(settings.bossSquid.life);
        asset->root()->setMaxLife(settings.bossSquid.life);

        SceneObjectPtr mouth[] = {
            asset->object("mouth_top_left"),
            asset->object("mouth_top_right"),
            asset->object("mouth_bottom_left"),
            asset->object("mouth_bottom_right")
        };

        SceneObjectPtr leg[] = {
            asset->object("leg_top_right"),
            asset->object("leg_bottom_right"),
            asset->object("leg_top_left"),
            asset->object("leg_bottom_left")
        };

        SceneObjectPtr tentacle[] = {
            asset->object("tentacle_right1"),
            asset->object("tentacle_right2"),
            asset->object("tentacle_top1"),
            asset->object("tentacle_top2"),
            asset->object("tentacle_left1"),
            asset->object("tentacle_left2"),
            asset->object("tentacle_bottom1"),
            asset->object("tentacle_bottom2")
        };

        for (int i = 0; i < 4; ++i) {
            mouth[i]->addComponent(boost::make_shared<TentaclePulseComponent>(
                15, getRandom(0.2f, 0.4f), getRandom(3.0f, 3.5f),
                getRandom(0.2f, 0.3f), getRandom(0.2f, 0.3f)));

            float t = getRandom(1.0f, 1.5f);
            float f = getRandom(800.0f, 1000.0f);

            TentacleSwingComponentPtr c = boost::make_shared<TentacleSwingComponent>(
                t, EaseInOutQuad, f,
                t, EaseInOutQuad, -f);
            c->setBone(0);
            mouth[i]->addComponent(c);
        }

        for (int i = 0; i < 4; ++i) {
            leg[i]->addComponent(boost::make_shared<TentaclePulseComponent>(
                15, getRandom(0.2f, 0.4f), getRandom(4.0f, 4.5f),
                getRandom(0.2f, 0.3f), getRandom(0.2f, 0.3f)));

            float t = getRandom(1.2f, 1.7f);
            float f = getRandom(4000.0f, 6000.0f);

            TentacleSwingComponentPtr c = boost::make_shared<TentacleSwingComponent>(
                t, EaseInOutQuad, f,
                t, EaseInOutQuad, -f);
            c->setBone(0);
            leg[i]->addComponent(c);
        }

        for (int i = 0; i < 8; ++i) {
            float t = getRandom(1.0f, 1.5f);
            float f = getRandom(20000.0f, 30000.0f);

            TentacleSwingComponentPtr c = boost::make_shared<TentacleSwingComponent>(
                t, EaseInOutQuad, f,
                t, EaseInOutQuad, -f);
            c->setBone(0);
            tentacle[i]->addComponent(c);

            {
                RUBEBodyPtr tipBody = asset->world()->body(tentacle[i]->name() + "_tip");

                WeaponSpitterComponentPtr weapon =
                    boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

                if ((i % 2) == 0) {
                    weapon->setDamage(settings.bossSquid.attack1Damage);
                    weapon->setVelocity(settings.bossSquid.attack1Velocity);
                    weapon->setSpreadAngle(settings.bossSquid.attack1Angle);
                    weapon->setMinShots(settings.bossSquid.attack1MinShots);
                    weapon->setMaxShots(settings.bossSquid.attack1MaxShots);
                    weapon->setTotalShots(settings.bossSquid.attack1TotalShots);
                    weapon->setShotInterval(settings.bossSquid.attack1Interval);
                    weapon->setInterval(0.0f);
                    weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 0.5f));
                    weapon->setMaxColor(Color(0.5f, 1.0f, 0.0f, 0.7f));
                    weapon->setMinHeight(0.5f);
                    weapon->setMaxHeight(1.0f);
                    weapon->setSound(audio.createSound("squid_attack1.ogg"));
                } else {
                    weapon->setDamage(settings.bossSquid.attack2Damage);
                    weapon->setVelocity(settings.bossSquid.attack2Velocity);
                    weapon->setSpreadAngle(settings.bossSquid.attack2Angle);
                    weapon->setMinShots(3);
                    weapon->setMaxShots(3);
                    weapon->setTotalShots(3);
                    weapon->setShotInterval(0.0f);
                    weapon->setInterval(0.0f);
                    weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
                    weapon->setMaxColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
                    weapon->setMinHeight(0.9f);
                    weapon->setMaxHeight(0.9f);
                    weapon->setSpreadRandom(false);
                    weapon->setI(2);
                    weapon->setSound(audio.createSound("beetle_attack.ogg"));
                }

                weapon->setPos(tipBody->image("missile")->pos());
                weapon->setAngle(tipBody->image("missile")->angle());
                weapon->setUseFreeze(false);

                asset->object(tentacle[i]->name() + "_tip")->addComponent(weapon);

                TentacleSquidComponentPtr component = boost::make_shared<TentacleSquidComponent>((i % 2) == 0);

                component->setWeapon(weapon);

                tentacle[i]->addComponent(component);
            }
        }

        SceneObjectPtr legControl[] = {
            asset->object("leg_top_right_control"),
            asset->object("leg_bottom_right_control"),
            asset->object("leg_top_left_control"),
            asset->object("leg_bottom_left_control")
        };

        SceneObjectPtr eyeControl[] = {
            asset->object("eye_right_control"),
            asset->object("eye_top_control"),
            asset->object("eye_left_control"),
            asset->object("eye_bottom_control")
        };

        SceneObjectPtr mouthControl[] = {
            asset->object("mouth_top_right_control"),
            asset->object("mouth_bottom_right_control"),
            asset->object("mouth_top_left_control"),
            asset->object("mouth_bottom_left_control")
        };

        Points explosionPos;

        for (int i = 0; i < asset->rootBody()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->rootBody()->image(i);

            if (image->name() != "explosion") {
                continue;
            }

            explosionPos.push_back(image->pos());
        }

        BossSquidComponentPtr c =
            boost::make_shared<BossSquidComponent>(tentacle, legControl,
                eyeControl, mouthControl, leg, mouth, explosionPos);

        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBossSquidEye()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_squid_eye.json");

        RenderTentacleComponentPtr tc =
            asset->root()->findComponent<RenderTentacleComponent>();
        tc->objects().back()->setLife(settings.bossSquid.eyeLife);
        tc->objects().back()->setMaxLife(settings.bossSquid.eyeLife);

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setMaterial(MaterialFlesh);
            asset->object(i)->setPropagateDamage(false);
        }

        float t = getRandom(1.2f, 1.5f);
        float f = getRandom(7000.0f, 8000.0f);
        float duration = getRandom(0.0f, t * 2.0f);

        TentacleSwingComponentPtr c = boost::make_shared<TentacleSwingComponent>(
            t, EaseInOutQuad, f,
            t, EaseInOutQuad, -f);
        c->setBone(3);
        c->setTweenTime(duration);
        asset->root()->addComponent(c);

        c = boost::make_shared<TentacleSwingComponent>(
            t, EaseInOutQuad, -f * 3,
            t, EaseInOutQuad, f * 3);
        c->setBone(1);
        c->setTweenTime(duration);
        asset->root()->addComponent(c);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createMech()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("mech.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeAlly);
        }

        asset->root()->setLife(settings.mech.life);
        asset->root()->setMaxLife(settings.mech.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 4.0f), 0, 6.0f, 1.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        filter.categoryBits = collisionBitAlly;

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        RenderQuadComponentPtr torsoRc = asset->object("torso")->findComponentByName<RenderQuadComponent>("torso");
        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");

        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());
        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());

        torsoAc->addAnimation(AnimationDefault, "mech_torso_def");
        legsAc->addAnimation(AnimationDefault, "mech_legs_def", 10.0f / settings.mech.walkSpeed);
        torsoAc->addAnimation(MechComponent::AnimationMelee2, "mech_torso_melee2");
        torsoAc->addAnimation(MechComponent::AnimationPreGun, "mech_torso_pregun");
        torsoAc->addAnimation(MechComponent::AnimationGun, "mech_torso_gun");
        torsoAc->addAnimation(MechComponent::AnimationPostGun, "mech_torso_postgun");

        torsoAc->startAnimation(AnimationDefault);
        legsAc->startAnimation(AnimationDefault);
        legsAc->setPaused(true);

        asset->root()->addComponent(torsoAc);
        asset->root()->addComponent(legsAc);

        Points melee2Points;

        std::ostringstream os;

        while (true) {
            os.str("");
            os << "melee2_" << melee2Points.size();

            RUBEImagePtr image = asset->world()->image(os.str());

            if (!image) {
                break;
            }

            melee2Points.push_back(image->pos());
        }

        {
            float melee2Duration = assetManager.getAnimation("mech_torso_melee2").duration() / assetManager.getAnimation("mech_torso_melee2").numFrames();

            MechComponentPtr component =
                boost::make_shared<MechComponent>(asset->object("torso"),
                    torsoRc, legsRc, torsoAc, legsAc,
                    melee2Points, melee2Duration,
                    asset->object("torso")->findComponent<LightComponent>()->getLights("fire"));

            WeaponMachineGunComponentPtr mgun =
                boost::make_shared<WeaponMachineGunComponent>(SceneObjectTypePlayerMissile);

            mgun->setDamage(settings.mech.attackDamage);
            mgun->setVelocity(settings.mech.attackVelocity);
            mgun->setSpreadAngle(settings.mech.attackSpreadAngle);
            mgun->setLoopDelay(settings.mech.attackInterval);
            mgun->setPos(asset->world()->image("gun")->pos());
            mgun->setAngle(asset->world()->image("gun")->angle());
            mgun->setTweakPos(true);
            mgun->setBulletHeight(0.2f);
            mgun->setTriggerCrosshair(true);
            mgun->setFlashQuad(false);

            component->setWeapon(mgun);
            asset->object("torso")->addComponent(mgun);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createHomer()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("homer.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setLife(settings.homer.life);
        asset->root()->setMaxLife(settings.homer.life);

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "homer_def");
        ac->addAnimation(HomerComponent::AnimationPreGun, "homer_pregun");
        ac->addAnimation(HomerComponent::AnimationPostGun, "homer_postgun");
        ac->addAnimation(HomerComponent::AnimationGun, "homer_gun");
        ac->addAnimation(HomerComponent::AnimationPreMissile, "homer_premissile");
        ac->addAnimation(HomerComponent::AnimationPostMissile, "homer_postmissile");
        ac->addAnimation(HomerComponent::AnimationMissile, "homer_missile");
        ac->addAnimation(HomerComponent::AnimationMelee, "homer_melee");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        HomerComponentPtr component =
            boost::make_shared<HomerComponent>(ac, asset->rootBody()->image("melee")->pos(),
                asset->rootBody()->image("melee")->height() / 2.0f);

        WeaponPlasmaGunComponentPtr pgun =
            boost::make_shared<WeaponPlasmaGunComponent>(SceneObjectTypeEnemyMissile);

        pgun->setDamage(settings.homer.gunDamage);
        pgun->setVelocity(settings.homer.gunVelocity);
        pgun->setSpreadAngle(settings.homer.gunAngle);
        pgun->setNumShots(1);
        pgun->setInterval(settings.homer.gunInterval);
        pgun->setPos(asset->world()->image("gun")->pos());
        pgun->setAngle(asset->world()->image("gun")->angle());

        component->setWeaponGun(pgun);
        asset->root()->addComponent(pgun);

        WeaponSeekerComponentPtr seeker =
            boost::make_shared<WeaponSeekerComponent>(SceneObjectTypeEnemyMissile);

        seeker->setExplosionImpulse(settings.homer.missileImpulse);
        seeker->setExplosionDamage(settings.homer.missileDamage);
        seeker->setVelocity(settings.homer.missileVelocity);
        seeker->setInterval(0.1f);
        seeker->setSeekVelocity(settings.homer.missileSeekVelocity);
        seeker->setPos(asset->world()->image("missile")->pos());
        seeker->setAngle(asset->world()->image("missile")->angle());

        component->setWeaponMissile(seeker);
        asset->root()->addComponent(seeker);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSeeker(SceneObjectType type, float explosionImpulse, float explosionDamage)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("seeker.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        SceneObjectTypes damageTypes;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        } else {
            damageTypes.set(SceneObjectTypePlayer);
            damageTypes.set(SceneObjectTypeAlly);
            damageTypes.set(SceneObjectTypeEnemy);
            damageTypes.set(SceneObjectTypeEnemyBuilding);
        }

        asset->root()->findComponent<PhysicsBodyComponent>()->setFilterOverride(filter);

        CollisionDieComponentPtr cdc = boost::make_shared<CollisionDieComponent>();

        asset->root()->addComponent(cdc);

        {
            ComponentPtr component = boost::make_shared<SeekerComponent>(cdc,
                asset->root()->findComponent<ParticleEffectComponent>(),
                explosionImpulse, explosionDamage, damageTypes);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createHomerGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("homer_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);

            makeDebris(asset->object(i));
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createEShield(SceneObjectType type)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("eshield.json");

        asset->root()->setType(SceneObjectTypeShield);
        asset->root()->setGlassy(true);

        SceneObjectTypes types;

        if (type == SceneObjectTypeEnemyMissile) {
            types.set(SceneObjectTypePlayerMissile);
            types.set(SceneObjectTypeAllyMissile);
            types.set(SceneObjectTypeNeutralMissile);
        } else if (type != SceneObjectTypeNeutralMissile) {
            types.set(SceneObjectTypeEnemyMissile);
            types.set(SceneObjectTypeNeutralMissile);
        } else {
            types.set(SceneObjectTypePlayerMissile);
            types.set(SceneObjectTypeAllyMissile);
            types.set(SceneObjectTypeEnemyMissile);
        }

        CollisionTypeFilterPtr cf = boost::make_shared<CollisionTypeFilter>();

        cf->setTypes(types);

        asset->root()->setCollisionFilter(cf);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createFakeMissile(SceneObjectType type)
    {
        assert((type == SceneObjectTypePlayerMissile) ||
               (type == SceneObjectTypeEnemyMissile) ||
               (type == SceneObjectTypeAllyMissile) ||
               (type == SceneObjectTypeNeutralMissile));

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("fake_missile.json");

        asset->root()->setType(type);

        b2Filter filter = asset->rootBody()->fixture(0)->fixtureDef().filter;

        if (type == SceneObjectTypeEnemyMissile) {
            filter.maskBits &= ~(collisionBitEnemy | collisionBitEnemyBuilding);
        } else if (type != SceneObjectTypeNeutralMissile) {
            filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
        }

        PhysicsBodyComponentPtr c = asset->root()->findComponent<PhysicsBodyComponent>();

        c->setFilterOverride(filter);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createScientist()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("scientist.json");

        asset->root()->setType(SceneObjectTypeAlly);

        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");
        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");

        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());
        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());

        legsAc->addAnimation(AnimationDefault, "scientist_legs_def");
        legsAc->addAnimation(DudeComponent::AnimationWalk, "scientist_legs_walk");

        torsoAc->addAnimation(AnimationDefault, "scientist_torso_def");
        torsoAc->addAnimation(DudeComponent::AnimationWalk, "scientist_torso_walk");

        legsAc->startAnimation(AnimationDefault);
        torsoAc->startAnimationRandomTime(AnimationDefault);

        asset->root()->addComponent(legsAc);
        asset->root()->addComponent(torsoAc);

        DudeComponentPtr component = boost::make_shared<DudeComponent>(legsRc, legsAc, torsoAc);

        component->setLegsAngle(-b2_pi / 2);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBossChopper()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_chopper.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
        }

        asset->root()->setLife(settings.bossChopper.life);
        asset->root()->setMaxLife(settings.bossChopper.life);

        asset->root()->setBlastDamageMultiplier(settings.bossChopper.blastDamageMultiplier);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 5.0f), 0, 12.0f, 2.0f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        std::vector<WeaponComponentPtr> guns;

        for (int i = 0; i < asset->rootBody()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->rootBody()->image(i);

            if (image->name() != "gun") {
                continue;
            }

            WeaponMachineGunComponentPtr weapon = boost::make_shared<WeaponMachineGunComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.bossChopper.gunDamage);
            weapon->setVelocity(settings.bossChopper.gunVelocity);
            weapon->setSpreadAngle(settings.bossChopper.gunAngle);
            weapon->setLoopDelay(settings.bossChopper.gunInterval);
            weapon->setPos(image->pos());
            weapon->setAngle(image->angle());
            weapon->setHaveSound(false);
            weapon->setBulletHeight(0.2f);

            if (guns.empty()) {
                weapon->setHaveSound(true);
            }

            guns.push_back(weapon);

            asset->root()->addComponent(weapon);
        }

        Points explosionPos;

        for (int i = 0; i < asset->rootBody()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->rootBody()->image(i);

            if (image->name() != "boom") {
                continue;
            }

            explosionPos.push_back(image->pos());
        }

        BossChopperComponentPtr component = boost::make_shared<BossChopperComponent>(explosionPos);

        component->setWeaponGuns(guns);

        WeaponSeekerComponentPtr seeker =
            boost::make_shared<WeaponSeekerComponent>(SceneObjectTypeEnemyMissile);

        seeker->setExplosionImpulse(settings.bossChopper.missileImpulse);
        seeker->setExplosionDamage(settings.bossChopper.missileDamage);
        seeker->setVelocity(settings.bossChopper.missileVelocity);
        seeker->setInterval(0.1f);
        seeker->setSeekVelocity(settings.bossChopper.missileSeekVelocity);
        seeker->setPos(asset->world()->image("missile")->pos());
        seeker->setAngle(asset->world()->image("missile")->angle());

        component->setWeaponMissile(seeker);
        asset->root()->addComponent(seeker);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createCreature(int i)
    {
        assert((i >= 1) && (i <= static_cast<int>(settings.creature.size())));

        std::ostringstream os;

        os << "creature" << i << ".json";

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset(os.str());

        asset->root()->setLife(settings.creature[i - 1].life);
        asset->root()->setMaxLife(settings.creature[i - 1].life);

        for (int j = 0; j < asset->numObjects(); ++j) {
            asset->object(j)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(j)->setMaterial(MaterialFlesh);

            {
                CollisionDamageComponentPtr component =
                    boost::make_shared<CollisionDamageComponent>();

                component->setImpulseThreshold(settings.creature[i - 1].damageImpulseThreshold);
                component->setMultiplier(settings.creature[i - 1].damageMultiplier);

                SceneObjectTypes types;

                types.set(SceneObjectTypePlayer);
                types.set(SceneObjectTypeAlly);
                types.set(SceneObjectTypeEnemy);
                types.set(SceneObjectTypeEnemyBuilding);
                types.set(SceneObjectTypeTerrain);
                types.set(SceneObjectTypeRock);
                types.set(SceneObjectTypeGizmo);
                types.set(SceneObjectTypeVehicle);

                component->setFilter(types);

                asset->object(j)->addComponent(component);
            }
        }

        {
            CreatureComponentPtr component = boost::make_shared<CreatureComponent>(i);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createGasCloud1(float delay, int zOrder)
    {
        return createGasCloud1Inner(settings.gasCloud1.propagationDelay, settings.gasCloud1.propagationDelay - delay, zOrder);
    }

    SceneObjectPtr SceneObjectFactory::createGasCloud1Inner(float delay, float offset, int zOrder)
    {
        SceneObjectPtr obj = createCircleSensor(5.0f, collisionBitPlayer);

        ParticleEffectComponentPtr pec =
            assetManager.getParticleEffect("gas1.p",
                b2Vec2(0.0f, 0.0f), 0.0f);

        pec->setZOrder(zOrder);

        pec->resetEmit();

        obj->addComponent(pec);

        {
            GasCloudComponentPtr thinkComponent =
                boost::make_shared<GasCloudComponent>(pec, delay, offset);

            obj->addComponent(thinkComponent);

            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            obj->addComponent(component);
        }

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createOctopus()
    {
        SceneObjectTypes types;

        types.set(SceneObjectTypePlayer);

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("octopus.json");

        asset->root()->setLife(settings.octopus.life);
        asset->root()->setMaxLife(settings.octopus.life);

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setMaterial(MaterialFlesh);
            asset->object(i)->setPropagateDamage(false);

            CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
            c->setFilter(types);
            if (asset->object(i) == asset->root()) {
                c->setDamage(settings.octopus.hitDamage);
                c->setDamageSound(audio.createSound("player_hit.ogg"));
                c->setRoll(deg2rad(2.0f));
            }
            asset->object(i)->addComponent(c);
        }

        std::vector<WeaponComponentPtr> weapons;

        for (int i = 0; i < asset->world()->numImages(); ++i) {
            const RUBEImagePtr& image = asset->world()->image(i);
            if (image->name() == "missile") {
                WeaponSpitterComponentPtr weapon =
                    boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

                weapon->setDamage(settings.octopus.spitDamage);
                weapon->setVelocity(settings.octopus.spitVelocity);
                weapon->setSpreadAngle(0.0f);
                weapon->setMinShots(1);
                weapon->setMaxShots(1);
                weapon->setTotalShots(1);
                weapon->setShotInterval(0.0f);
                weapon->setInterval(0.0f);
                weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
                weapon->setMaxColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
                weapon->setMinHeight(1.1f);
                weapon->setMaxHeight(1.1f);
                weapon->setSpreadRandom(false);
                weapon->setI(2);
                weapon->setSound(audio.createSound("beetle_attack.ogg"));
                weapon->setPos(image->pos());
                weapon->setAngle(image->angle());
                weapon->setUseFreeze(false);
                weapon->setHaveSound(weapons.empty());

                asset->root()->addComponent(weapon);

                weapons.push_back(weapon);
            }
        }

        {
            OctopusComponentPtr component = boost::make_shared<OctopusComponent>();

            component->setWeapons(weapons);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createWalker()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("walker.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setPropagateDamage(false);
        }

        asset->object("blocker")->setType(SceneObjectTypeBlocker);

        asset->object("heart")->setMaxLife(settings.walker.life);
        asset->object("heart")->setLife(settings.walker.life);

        asset->object("heart")->setMaterial(MaterialFlesh);

        SceneObjectPtr legs[] = {
            asset->object("leg1"),
            asset->object("leg2"),
            asset->object("leg3"),
            asset->object("leg4"),
        };

        b2Vec2 legTips[] = {
            asset->world()->body("leg1")->image("tip")->pos(),
            asset->world()->body("leg2")->image("tip")->pos(),
            asset->world()->body("leg3")->image("tip")->pos(),
            asset->world()->body("leg4")->image("tip")->pos(),
        };


        WalkerComponentPtr component = boost::make_shared<WalkerComponent>(asset->object("heart"),
            legs, legTips);

        for (int i = 0; i < 4; ++i) {
            std::ostringstream os;

            os << "missile" << (i + 1);

            const RUBEImagePtr& image = asset->world()->image(os.str());

            WeaponSeekerComponentPtr seeker =
                boost::make_shared<WeaponSeekerComponent>(SceneObjectTypeEnemyMissile);

            seeker->setExplosionImpulse(settings.walker.missileImpulse);
            seeker->setExplosionDamage(settings.walker.missileDamage);
            seeker->setVelocity(settings.walker.missileVelocity);
            seeker->setInterval(0.1f);
            seeker->setSeekVelocity(settings.walker.missileSeekVelocity);
            seeker->setTweakPos(false);
            seeker->setUseFilter(true);
            seeker->setPos(image->pos());
            seeker->setAngle(image->angle());

            component->setWeapon(i, seeker);

            asset->root()->addComponent(seeker);

            WeaponSpitterComponentPtr weapon =
                boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.walker.spitDamage);
            weapon->setVelocity(settings.walker.spitVelocity);
            weapon->setSpreadAngle(settings.walker.spitAngle);
            weapon->setMinShots(settings.walker.spitMinShots);
            weapon->setMaxShots(settings.walker.spitMaxShots);
            weapon->setTotalShots(settings.walker.spitTotalShots);
            weapon->setShotInterval(settings.walker.spitInterval);
            weapon->setInterval(settings.walker.spitDelay);
            weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 0.5f));
            weapon->setMaxColor(Color(0.5f, 1.0f, 0.0f, 0.7f));
            weapon->setMinHeight(0.7f);
            weapon->setMaxHeight(1.2f);
            weapon->setPos(image->pos());
            weapon->setAngle(image->angle());
            weapon->setUseFilter(true);
            weapon->setUseFreeze(false);
            weapon->setTweakPos(false);

            component->setExposedWeapon(i, weapon);

            asset->root()->addComponent(weapon);
        }

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSnake()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("snake.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setPropagateDamage(false);
        }

        asset->object("eye")->setMaterial(MaterialFlesh);
        asset->object("eye")->setMaxLife(settings.snake.life);
        asset->object("eye")->setLife(settings.snake.life);
        asset->object("eye")->setBlastDamageMultiplier(2.0f);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 3.0f), 0, 8.0f, 1.5f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->object("eye")->addComponent(component);
        }

        {
            SnakeComponentPtr thinkComponent =
                boost::make_shared<SnakeComponent>(asset->object("eye"),
                    asset->object("tentacle_base")->findComponent<RenderTentacleComponent>());

            asset->root()->addComponent(thinkComponent);

            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            WeaponAcidballComponentPtr missileWeapon =
                boost::make_shared<WeaponAcidballComponent>(SceneObjectTypeEnemyMissile);

            missileWeapon->setExplosionTimeout(settings.snake.missileExplosionTimeout);
            missileWeapon->setExplosionImpulse(settings.snake.missileExplosionImpulse);
            missileWeapon->setExplosionDamage(settings.snake.missileExplosionDamage);
            missileWeapon->setToxicDamage(settings.snake.missileToxicDamage);
            missileWeapon->setToxicDamageTimeout(settings.snake.missileToxicDamageTimeout);
            missileWeapon->setVelocity(settings.snake.missileVelocity);
            missileWeapon->setInterval(settings.snake.missileInterval);
            missileWeapon->setPos(asset->world()->image("missile")->pos());
            missileWeapon->setAngle(asset->world()->image("missile")->angle());
            missileWeapon->setUseFilter(true);
            missileWeapon->setUseFreeze(false);

            asset->root()->addComponent(missileWeapon);

            std::vector<SceneObjectPtr> tmp = asset->root()->getObjects("fang");
            for (std::vector<SceneObjectPtr>::const_iterator it = tmp.begin();
                it != tmp.end(); ++it) {
                missileWeapon->addToFilter((*it)->cookie());
            }

            thinkComponent->setWeaponMissile(missileWeapon);

            WeaponLGunComponentPtr gun =
                boost::make_shared<WeaponLGunComponent>(SceneObjectTypeEnemyMissile);

            gun->setDamage(settings.snake.gunDamage);
            gun->setLength(settings.snake.gunLength);
            gun->setImpulse(settings.snake.gunImpulse);
            gun->setPos(asset->world()->image("gun")->pos());
            gun->setAngle(asset->world()->image("gun")->angle());

            thinkComponent->setWeaponGun(gun);

            asset->root()->addComponent(gun);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBossBuddy()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_buddy.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setPropagateDamage(false);
            asset->object(i)->setCollisionImpulseMultiplier(0.0f);
            asset->object(i)->setMaterial(MaterialFlesh);
        }

        asset->object("brain")->setMaxLife(settings.bossBuddy.life);
        asset->object("brain")->setLife(settings.bossBuddy.life);

        BuddySideComponent::Layer layers[3][8] = {
        {
            BuddySideComponent::Layer(asset->object("head1"),
                asset->object("upperElbow1"), asset->object("lowerElbow1"), asset->object("upperArm1"), asset->object("lowerArm1")),
            BuddySideComponent::Layer(asset->object("head1_1"),
                asset->object("upperElbow1_1"), asset->object("lowerElbow1_1"), asset->object("upperArm1_1"), asset->object("lowerArm1_1")),
            BuddySideComponent::Layer(asset->object("head1_2"),
                asset->object("upperElbow1_2"), asset->object("lowerElbow1_2"), asset->object("upperArm1_2"), asset->object("lowerArm1_2")),
            BuddySideComponent::Layer(asset->object("head1_3"),
                asset->object("upperElbow1_3"), asset->object("lowerElbow1_3"), asset->object("upperArm1_3"), asset->object("lowerArm1_3")),
            BuddySideComponent::Layer(asset->object("head1_4"),
                asset->object("upperElbow1_4"), asset->object("lowerElbow1_4"), asset->object("upperArm1_4"), asset->object("lowerArm1_4")),
            BuddySideComponent::Layer(asset->object("head1_5"),
                asset->object("upperElbow1_5"), asset->object("lowerElbow1_5"), asset->object("upperArm1_5"), asset->object("lowerArm1_5"),
                asset->object("upperElbow1_5s"), asset->object("lowerElbow1_5s"), asset->object("upperArm1_5s"), asset->object("lowerArm1_5s")),
            BuddySideComponent::Layer(asset->object("head1_6"),
                asset->object("upperElbow1_6"), asset->object("lowerElbow1_6"), asset->object("upperArm1_6"), asset->object("lowerArm1_6"),
                asset->object("upperElbow1_6s"), asset->object("lowerElbow1_6s"), asset->object("upperArm1_6s"), asset->object("lowerArm1_6s")),
            BuddySideComponent::Layer(asset->object("head1_7"),
                asset->object("upperElbow1_7"), asset->object("lowerElbow1_7"), asset->object("upperArm1_7"), asset->object("lowerArm1_7"),
                asset->object("upperElbow1_7s"), asset->object("lowerElbow1_7s"), asset->object("upperArm1_7s"), asset->object("lowerArm1_7s")),
        },
        {
            BuddySideComponent::Layer(asset->object("head2"),
                asset->object("upperElbow2"), asset->object("lowerElbow2"), asset->object("upperArm2"), asset->object("lowerArm2")),
            BuddySideComponent::Layer(asset->object("head2_1"),
                asset->object("upperElbow2_1"), asset->object("lowerElbow2_1"), asset->object("upperArm2_1"), asset->object("lowerArm2_1")),
            BuddySideComponent::Layer(asset->object("head2_2"),
                asset->object("upperElbow2_2"), asset->object("lowerElbow2_2"), asset->object("upperArm2_2"), asset->object("lowerArm2_2")),
            BuddySideComponent::Layer(asset->object("head2_3"),
                asset->object("upperElbow2_3"), asset->object("lowerElbow2_3"), asset->object("upperArm2_3"), asset->object("lowerArm2_3")),
            BuddySideComponent::Layer(asset->object("head2_4"),
                asset->object("upperElbow2_4"), asset->object("lowerElbow2_4"), asset->object("upperArm2_4"), asset->object("lowerArm2_4")),
            BuddySideComponent::Layer(asset->object("head2_5"),
                asset->object("upperElbow2_5"), asset->object("lowerElbow2_5"), asset->object("upperArm2_5"), asset->object("lowerArm2_5"),
                asset->object("upperElbow2_5s"), asset->object("lowerElbow2_5s"), asset->object("upperArm2_5s"), asset->object("lowerArm2_5s")),
            BuddySideComponent::Layer(asset->object("head2_6"),
                asset->object("upperElbow2_6"), asset->object("lowerElbow2_6"), asset->object("upperArm2_6"), asset->object("lowerArm2_6"),
                asset->object("upperElbow2_6s"), asset->object("lowerElbow2_6s"), asset->object("upperArm2_6s"), asset->object("lowerArm2_6s")),
            BuddySideComponent::Layer(asset->object("head2_7"),
                asset->object("upperElbow2_7"), asset->object("lowerElbow2_7"), asset->object("upperArm2_7"), asset->object("lowerArm2_7"),
                asset->object("upperElbow2_7s"), asset->object("lowerElbow2_7s"), asset->object("upperArm2_7s"), asset->object("lowerArm2_7s")),
        },
        {
            BuddySideComponent::Layer(asset->object("head3"),
                asset->object("upperElbow3"), asset->object("lowerElbow3"), asset->object("upperArm3"), asset->object("lowerArm3")),
            BuddySideComponent::Layer(asset->object("head3_1"),
                asset->object("upperElbow3_1"), asset->object("lowerElbow3_1"), asset->object("upperArm3_1"), asset->object("lowerArm3_1")),
            BuddySideComponent::Layer(asset->object("head3_2"),
                asset->object("upperElbow3_2"), asset->object("lowerElbow3_2"), asset->object("upperArm3_2"), asset->object("lowerArm3_2")),
            BuddySideComponent::Layer(asset->object("head3_3"),
                asset->object("upperElbow3_3"), asset->object("lowerElbow3_3"), asset->object("upperArm3_3"), asset->object("lowerArm3_3")),
            BuddySideComponent::Layer(asset->object("head3_4"),
                asset->object("upperElbow3_4"), asset->object("lowerElbow3_4"), asset->object("upperArm3_4"), asset->object("lowerArm3_4")),
            BuddySideComponent::Layer(asset->object("head3_5"),
                asset->object("upperElbow3_5"), asset->object("lowerElbow3_5"), asset->object("upperArm3_5"), asset->object("lowerArm3_5"),
                asset->object("upperElbow3_5s"), asset->object("lowerElbow3_5s"), asset->object("upperArm3_5s"), asset->object("lowerArm3_5s")),
            BuddySideComponent::Layer(asset->object("head3_6"),
                asset->object("upperElbow3_6"), asset->object("lowerElbow3_6"), asset->object("upperArm3_6"), asset->object("lowerArm3_6"),
                asset->object("upperElbow3_6s"), asset->object("lowerElbow3_6s"), asset->object("upperArm3_6s"), asset->object("lowerArm3_6s")),
            BuddySideComponent::Layer(asset->object("head3_7"),
                asset->object("upperElbow3_7"), asset->object("lowerElbow3_7"), asset->object("upperArm3_7"), asset->object("lowerArm3_7"),
                asset->object("upperElbow3_7s"), asset->object("lowerElbow3_7s"), asset->object("upperArm3_7s"), asset->object("lowerArm3_7s")),
        }};

        for (int n = 0; n < 3; ++n) {
            for (int i = 5; i <= 7; ++i) {
                SceneObjectTypes types;

                types.set(SceneObjectTypePlayer);

                for (int j = 0; j < 2; ++j) {
                    CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
                    c->setFilter(types);
                    c->setDamage(settings.bossBuddy.sideMeleeDamage);
                    c->setDamageSound(audio.createSound("buddy_hit.ogg"));
                    c->setRoll(deg2rad(2.0f));
                    layers[n][i].elbowS[j]->addComponent(c);

                    c = boost::make_shared<CollisionCancelComponent>();
                    c->setFilter(types);
                    c->setDamage(settings.bossBuddy.sideMeleeDamage);
                    c->setDamageSound(audio.createSound("buddy_hit.ogg"));
                    c->setRoll(deg2rad(2.0f));
                    layers[n][i].armS[j]->addComponent(c);
                }
            }

            for (int i = 0; i < 8; ++i) {
                RenderQuadComponentPtr rc = layers[n][i].head->findComponent<RenderQuadComponent>();
                QuadPulseComponentPtr pc = boost::make_shared<QuadPulseComponent>(rc, b2Vec2_zero, 1.0f, EaseInOutQuad, 0.95f,
                        1.0f, EaseInOutQuad, 1.05f);
                layers[n][i].head->addComponent(pc);
            }
        }

        SceneObjectPtr center[5] = {
            asset->object("root"),
            asset->object("root_1"),
            asset->object("root_2"),
            asset->object("root_3"),
            asset->object("root_4")
        };

        for (int i = 0; i < 5; ++i) {
            std::vector<RenderQuadComponentPtr> rcs = center[i]->findComponents<RenderQuadComponent>("body");
            for (std::vector<RenderQuadComponentPtr>::const_iterator it = rcs.begin(); it != rcs.end(); ++it) {
                QuadPulseComponentPtr pc = boost::make_shared<QuadPulseComponent>(*it, (*it)->pos(), 1.0f, EaseInOutQuad, 0.95f,
                        1.0f, EaseInOutQuad, 1.05f);
                center[i]->addComponent(pc);
            }
        }

        SceneObjectPtr tentacles[3] = {
            asset->object("tentacle_base1"),
            asset->object("tentacle_base2"),
            asset->object("tentacle_base3")
        };

        for (int i = 0; i < 3; ++i) {
            tentacles[i]->addComponent(boost::make_shared<TentaclePulseComponent>(
                15, getRandom(0.15f, 0.3f), getRandom(6.0f, 7.5f),
                getRandom(0.6f, 0.8f), getRandom(0.6f, 0.8f)));
        }

        BuddySideComponentPtr c1 = boost::make_shared<BuddySideComponent>(1, layers[0]);

        asset->object("head1")->addComponent(c1);

        BuddySideComponentPtr c2 = boost::make_shared<BuddySideComponent>(2, layers[1]);

        asset->object("head2")->addComponent(c2);

        BuddySideComponentPtr c3 = boost::make_shared<BuddySideComponent>(3, layers[2]);

        asset->object("head3")->addComponent(c3);

        BuddySideComponentPtr sides[3] = {c1, c2, c3};

        SceneObjectPtr pipes[2] = {
            asset->object("pipe0"),
            asset->object("pipe1")
        };

        b2Transform eyeletXf[2] = {
            b2Transform(asset->world()->body("pipe0")->image("eyelet")->pos(), b2Rot(asset->world()->body("pipe0")->image("eyelet")->angle())),
            b2Transform(asset->world()->body("pipe1")->image("eyelet")->pos(), b2Rot(asset->world()->body("pipe1")->image("eyelet")->angle()))
        };

        SceneObjectPtr jaws[2] = {
            asset->object("jaw0"),
            asset->object("jaw1")
        };

        SceneObjectPtr skull[2] = {
            asset->object("skull0"),
            asset->object("skull1")
        };

        BossBuddyComponentPtr thinkComponent(new BossBuddyComponent(sides,
            asset->object("head0"), asset->object("head0s"),
            b2Transform(asset->world()->image("tongue")->pos(), b2Rot(asset->world()->image("tongue")->angle())),
            pipes, eyeletXf, asset->object("brain"), asset->object("root_3"), asset->object("root3s"),
            b2Transform(asset->world()->image("napalm")->pos(), b2Rot(asset->world()->image("napalm")->angle())),
            jaws, skull, asset->object("gun0")));

        for (int i = 0; i < 2; ++i) {
            std::ostringstream os;

            os << "gun" << (i + 1);

            const RUBEImagePtr& image = asset->world()->body(os.str())->image("missile");

            WeaponPlasmaGunComponentPtr weapon =
                boost::make_shared<WeaponPlasmaGunComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.bossBuddy.plasmaDamage);
            weapon->setVelocity(settings.bossBuddy.plasmaVelocity);
            weapon->setSpreadAngle(settings.bossBuddy.plasmaAngle);
            weapon->setNumShots(3);
            weapon->setInterval(settings.bossBuddy.plasmaInterval);
            weapon->setPos(image->pos());
            weapon->setAngle(image->angle());
            weapon->setUseFreeze(false);
            weapon->setHaveSound(i == 0);

            thinkComponent->setSideWeapon(i, weapon);

            asset->object(os.str())->addComponent(weapon);

            WeaponRLauncherComponentPtr altWeapon =
                boost::make_shared<WeaponRLauncherComponent>(SceneObjectTypeEnemyMissile);

            altWeapon->setExplosionImpulse(settings.bossBuddy.missileExplosionImpulse);
            altWeapon->setExplosionDamage(settings.bossBuddy.missileExplosionDamage);
            altWeapon->setVelocity(settings.bossBuddy.missileVelocity);
            altWeapon->setInterval(0.1f);
            altWeapon->setPos(image->pos());
            altWeapon->setAngle(image->angle());
            altWeapon->setUseFreeze(false);

            thinkComponent->setSideAltWeapon(i, altWeapon);

            asset->object(os.str())->addComponent(altWeapon);
        }

        WeaponSeekerComponentPtr seeker =
            boost::make_shared<WeaponSeekerComponent>(SceneObjectTypeEnemyMissile);

        seeker->setExplosionImpulse(settings.bossBuddy.seekerImpulse);
        seeker->setExplosionDamage(settings.bossBuddy.seekerDamage);
        seeker->setVelocity(settings.bossBuddy.seekerVelocity);
        seeker->setInterval(0.1f);
        seeker->setSeekVelocity(settings.bossBuddy.seekerSeekVelocity);
        seeker->setPos(asset->world()->body("gun0")->image("missile")->pos());
        seeker->setAngle(asset->world()->body("gun0")->image("missile")->angle());

        thinkComponent->setFrontWeapon(seeker);

        asset->object("gun0")->addComponent(seeker);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->object("pipe0")->addComponent(component);
        }

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->object("pipe1")->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBuddyTongue()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_buddy_tongue.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeDeadbody);
            asset->object(i)->setPropagateDamage(false);
            asset->object(i)->setCollisionImpulseMultiplier(0.0f);
            asset->object(i)->setMaterial(MaterialFlesh);
        }

        asset->root()->setType(SceneObjectTypeEnemy);

        asset->root()->setMaxLife(settings.bossBuddy.tongueLife);
        asset->root()->setLife(settings.bossBuddy.tongueLife);

        BuddyTongueComponentPtr thinkComponent = boost::make_shared<BuddyTongueComponent>(
            b2Transform(asset->world()->image("eyelet")->pos(), b2Rot(asset->world()->image("eyelet")->angle())));

        WeaponSpitterComponentPtr weapon =
            boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

        weapon->setDamage(settings.bossBuddy.tongueAttackDamage);
        weapon->setVelocity(settings.bossBuddy.tongueAttackVelocity);
        weapon->setSpreadAngle(settings.bossBuddy.tongueAttackAngle);
        weapon->setMinShots(settings.bossBuddy.tongueAttackMinShots);
        weapon->setMaxShots(settings.bossBuddy.tongueAttackMaxShots);
        weapon->setTotalShots(settings.bossBuddy.tongueAttackTotalShots);
        weapon->setShotInterval(0.04f);
        weapon->setInterval(settings.bossBuddy.tongueAttackInterval);
        weapon->setMinColor(Color(0.0f, 1.0f, 0.0f, 0.5f));
        weapon->setMaxColor(Color(0.5f, 1.0f, 0.0f, 0.7f));
        weapon->setMinHeight(0.7f);
        weapon->setMaxHeight(1.2f);
        weapon->setPos(asset->world()->image("missile")->pos());
        weapon->setAngle(asset->world()->image("missile")->angle());
        weapon->setUseFreeze(false);
        weapon->setTweakPos(false);
        weapon->setSound(audio.createSound("squid_attack1.ogg"));

        thinkComponent->setWeapon(weapon);

        asset->root()->addComponent(weapon);

        asset->root()->addComponent(thinkComponent);

        {
            CollisionSensorComponentPtr component =
                boost::make_shared<CollisionSensorComponent>();

            component->setListener(thinkComponent);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createNapalm(float damage, bool haveSound)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("napalm.json");

        {
            NapalmComponentPtr component = boost::make_shared<NapalmComponent>(
                asset->root()->findComponent<ParticleEffectComponent>(),
                damage, haveSound);

            component->setAutoTarget(true);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createNapalm2(float damage, bool haveSound)
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("napalm2.json");

        {
            NapalmComponentPtr component = boost::make_shared<NapalmComponent>(
                asset->root()->findComponent<ParticleEffectComponent>(),
                damage, haveSound);

            component->setAutoTarget(true);

            asset->root()->addComponent(component);
        }

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createBossNatan()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_natan.json");

        asset->root()->setType(SceneObjectTypeEnemy);
        asset->root()->setMaterial(MaterialFlesh);

        asset->root()->setMaxLife(settings.bossNatan.life);
        asset->root()->setLife(asset->root()->maxLife() * (2.0f / 3.0f));

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        AnimationComponentPtr ac = boost::make_shared<AnimationComponent>(rc->drawable());

        ac->addAnimation(AnimationDefault, "natan_def");
        ac->addAnimation(BossNatanComponent::AnimationMelee, "natan_melee");
        ac->addAnimation(BossNatanComponent::AnimationPreShoot, "natan_preshoot");
        ac->addAnimation(BossNatanComponent::AnimationPreShoot2, "natan_preshoot2");
        ac->addAnimation(BossNatanComponent::AnimationPostShoot, "natan_postshoot");
        ac->addAnimation(BossNatanComponent::AnimationShoot, "natan_shoot");
        ac->addAnimation(BossNatanComponent::AnimationSyringe, "natan_syringe");
        ac->addAnimation(BossNatanComponent::AnimationRam, "natan_ram");
        ac->addAnimation(BossNatanComponent::AnimationPowerup, "natan_powerup");
        ac->addAnimation(BossNatanComponent::AnimationDie, "natan_die");
        ac->addAnimation(BossNatanComponent::AnimationDead, "natan_dead");

        ac->startAnimation(AnimationDefault);

        asset->root()->addComponent(ac);

        std::vector<ParticleEffectComponentPtr> cs = asset->root()->findComponents<ParticleEffectComponent>();

        assert(cs.size() == 2);

        ParticleEffectComponentPtr pec[] = { cs[0], cs[1] };

        BossNatanComponentPtr component =
            boost::make_shared<BossNatanComponent>(ac, pec, asset->rootBody()->image("melee")->pos(),
                asset->rootBody()->image("melee")->height() / 2.0f);

        WeaponPlasmaGunComponentPtr weapon =
            boost::make_shared<WeaponPlasmaGunComponent>(SceneObjectTypeEnemyMissile);

        weapon->setDamage(settings.bossNatan.plasmaDamage);
        weapon->setVelocity(settings.bossNatan.plasmaVelocity);
        weapon->setSpreadAngle(settings.bossNatan.plasmaAngle);
        weapon->setNumShots(3);
        weapon->setInterval(settings.bossNatan.plasmaInterval);
        weapon->setPos(asset->world()->image("missile")->pos());
        weapon->setAngle(asset->world()->image("missile")->angle());
        weapon->setUseFreeze(false);
        weapon->setHaveSound(true);

        component->setWeapon(0, weapon);

        asset->root()->addComponent(weapon);

        WeaponSpitterComponentPtr weapon2 =
            boost::make_shared<WeaponSpitterComponent>(SceneObjectTypeEnemyMissile);

        weapon2->setDamage(settings.bossNatan.spitDamage);
        weapon2->setVelocity(settings.bossNatan.spitVelocity);
        weapon2->setSpreadAngle(settings.bossNatan.spitAngle);
        weapon2->setMinShots(3);
        weapon2->setMaxShots(3);
        weapon2->setTotalShots(3);
        weapon2->setShotInterval(0.0f);
        weapon2->setInterval(settings.bossNatan.spitInterval);
        weapon2->setMinColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
        weapon2->setMaxColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
        weapon2->setMinHeight(1.1f);
        weapon2->setMaxHeight(1.1f);
        weapon2->setSpreadRandom(false);
        weapon2->setI(2);
        weapon2->setSound(audio.createSound("beetle_attack.ogg"));
        weapon2->setPos(asset->world()->image("missile")->pos());
        weapon2->setAngle(asset->world()->image("missile")->angle());
        weapon2->setUseFreeze(false);
        weapon2->setHaveSound(true);

        component->setWeapon(1, weapon2);

        asset->root()->addComponent(weapon2);

        WeaponBlasterComponentPtr weapon3 =
            boost::make_shared<WeaponBlasterComponent>(true, SceneObjectTypeEnemyMissile);

        weapon3->setDamage(settings.bossNatan.blasterDamage);
        weapon3->setVelocity(settings.bossNatan.blasterVelocity);
        weapon3->setTurns(1);
        weapon3->setShotsPerTurn(3);
        weapon3->setLoopDelay(settings.bossNatan.blasterInterval);
        weapon3->setPos(asset->world()->image("missile")->pos());
        weapon3->setAngle(asset->world()->image("missile")->angle());
        weapon3->setUseFreeze(false);

        component->setWeapon(2, weapon3);

        asset->root()->addComponent(weapon3);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createPowerBeam(float radius, float length)
    {
        SceneObjectPtr obj = createDummy();

        PowerBeamComponentPtr component = boost::make_shared<PowerBeamComponent>(radius, length);

        obj->addComponent(component);

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createGuardian()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("guardian.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setPropagateDamage(false);
        }

        asset->root()->setMaxLife(settings.guardian.life);
        asset->root()->setLife(settings.guardian.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.5f), 0, 4.0f, 0.8f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        GuardianComponentPtr component = boost::make_shared<GuardianComponent>(settings.guardian.jumpVelocity,
            settings.guardian.shootMinTimeout, settings.guardian.shootMaxTimeout, settings.guardian.jumpTimeout);

        WeaponBlasterComponentPtr blaster =
            boost::make_shared<WeaponBlasterComponent>(false, SceneObjectTypeEnemyMissile);

        blaster->setDamage(settings.guardian.shootDamage);
        blaster->setVelocity(settings.guardian.shootVelocity);
        blaster->setTurns(4);
        blaster->setShotsPerTurn(1);
        blaster->setTurnInterval(0.1f);
        blaster->setLoopDelay(0.5f);
        blaster->setPos(asset->world()->image("missile")->pos());
        blaster->setAngle(asset->world()->image("missile")->angle());

        component->setWeapon(blaster);

        asset->root()->addComponent(blaster);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createGuardian2()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("guardian.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
            asset->object(i)->setPropagateDamage(false);

            if (asset->object(i)->name() == "bone1") {
                asset->object(i)->findComponent<RenderQuadComponent>()->drawable()->setImage(
                    assetManager.getImage("common1/guardian2_bone1.png"));
            } else if (asset->object(i)->name() == "bone2") {
                asset->object(i)->findComponent<RenderQuadComponent>()->drawable()->setImage(
                    assetManager.getImage("common1/guardian2_bone2.png"));
            }
        }

        asset->root()->findComponent<RenderQuadComponent>()->drawable()->setImage(
            assetManager.getImage("common1/guardian2_head.png"));

        asset->root()->setMaxLife(settings.guardian2.life);
        asset->root()->setLife(settings.guardian2.life);

        {
            RenderHealthbarComponentPtr component =
                boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 2.5f), 0, 4.0f, 0.8f, zOrderMarker + 1);

            component->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

            asset->root()->addComponent(component);
        }

        GuardianComponentPtr component = boost::make_shared<GuardianComponent>(settings.guardian2.jumpVelocity,
            settings.guardian2.shootMinTimeout, settings.guardian2.shootMaxTimeout, settings.guardian2.jumpTimeout);

        WeaponBlasterComponentPtr blaster =
            boost::make_shared<WeaponBlasterComponent>(true, SceneObjectTypeEnemyMissile);

        blaster->setDamage(settings.guardian.shootDamage);
        blaster->setVelocity(settings.guardian.shootVelocity);
        blaster->setTurns(4);
        blaster->setShotsPerTurn(2);
        blaster->setTurnInterval(0.1f);
        blaster->setLoopDelay(0.5f);
        blaster->setPos(asset->world()->image("missile")->pos());
        blaster->setAngle(asset->world()->image("missile")->angle());

        component->setWeapon(blaster);

        asset->root()->addComponent(blaster);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createMilitary1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("military1.json");

        asset->root()->setType(SceneObjectTypeAlly);

        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");
        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");

        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());
        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());

        legsAc->addAnimation(AnimationDefault, "military_legs_def");
        legsAc->addAnimation(DudeComponent::AnimationWalk, "military_legs_walk");

        torsoAc->addAnimation(AnimationDefault, "military_torso_nogun_def");
        torsoAc->addAnimation(DudeComponent::AnimationWalk, "military_torso_nogun_walk");
        torsoAc->addAnimation(DudeComponent::AnimationGunDefault, "military_torso_def");
        torsoAc->addAnimation(DudeComponent::AnimationGunWalk, "military_torso_walk");

        legsAc->startAnimation(AnimationDefault);
        torsoAc->startAnimationRandomTime(DudeComponent::AnimationGunDefault);

        asset->root()->addComponent(legsAc);
        asset->root()->addComponent(torsoAc);

        DudeComponentPtr component = boost::make_shared<DudeComponent>(legsRc, legsAc, torsoAc);

        component->setLegsAngle(-b2_pi / 2);
        component->setHaveGun(true);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSarge1()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sarge1.json");

        asset->root()->setType(SceneObjectTypeAlly);

        RenderQuadComponentPtr legsRc = asset->root()->findComponentByName<RenderQuadComponent>("legs");
        RenderQuadComponentPtr torsoRc = asset->root()->findComponentByName<RenderQuadComponent>("torso");

        AnimationComponentPtr legsAc = boost::make_shared<AnimationComponent>(legsRc->drawable());
        AnimationComponentPtr torsoAc = boost::make_shared<AnimationComponent>(torsoRc->drawable());

        legsAc->addAnimation(AnimationDefault, "sarge_legs_def");
        legsAc->addAnimation(DudeComponent::AnimationWalk, "sarge_legs_walk");

        torsoAc->addAnimation(AnimationDefault, "sarge_torso_def");
        torsoAc->addAnimation(DudeComponent::AnimationWalk, "sarge_torso_walk");

        legsAc->startAnimation(AnimationDefault);
        torsoAc->startAnimationRandomTime(AnimationDefault);

        asset->root()->addComponent(legsAc);
        asset->root()->addComponent(torsoAc);

        DudeComponentPtr component = boost::make_shared<DudeComponent>(legsRc, legsAc, torsoAc);

        component->setLegsAngle(-b2_pi / 2);

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSawer(float length, bool fixed)
    {
        float segmentSize = 1.0f;

        SceneObjectPtr root;

        int numSegments = std::ceil(length / segmentSize);

        std::vector<SceneObjectPtr> objs;
        std::vector<RUBECircleFixturePtr> fixtures;

        for (int i = 0; i < numSegments; ++i) {
            b2Vec2 pos(segmentSize * i, 0.0f);

            SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sawer_segment.json");

            asset->root()->setType(SceneObjectTypeDeadbody);

            asset->root()->setPos(pos);

            if (root) {
                root->addObject(asset->root());
            } else {
                root = asset->root();
            }

            objs.push_back(asset->root());
            fixtures.push_back(asset->rootBody()->fixture<RUBECircleFixture>("root"));
        }

        if (fixed) {
            root->script_setBodyType(b2_kinematicBody);
        }

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sawer_head.json");

        asset->root()->setType(SceneObjectTypeDeadbody);

        asset->root()->setPos(b2Vec2(segmentSize * numSegments, 0.0f));

        root->addObject(asset->root());

        RenderQuadComponentPtr rc = asset->root()->findComponent<RenderQuadComponent>();

        RenderTentacleComponentPtr tc = boost::make_shared<RenderTentacleComponent>(objs, fixtures,
            assetManager.getDrawable("pipe9.png",
                Texture::WrapModeRepeat,
                Texture::WrapModeClamp),
            rc->zOrder() + 1, 25, 0.0f, 0.2f);

        tc->resetTimeline1(2);
        tc->resetTimeline2(2);
        tc->set1At(0, 0.0f, 0.8f);
        tc->set1At(1, 1.0f, 0.8f);
        tc->set2At(0, 0.0f, 0.8f);
        tc->set2At(1, 1.0f, 0.8f);

        root->addComponent(tc);

        {
            SawerComponentPtr component = boost::make_shared<SawerComponent>(tc, asset->root());

            root->addComponent(component);
        }

        return root;
    }

    SceneObjectPtr SceneObjectFactory::createSawerSaw()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sawer_saw.json");

        asset->root()->setType(SceneObjectTypeEnemy);

        CollisionBuzzSawComponentPtr component = boost::make_shared<CollisionBuzzSawComponent>();

        asset->root()->addComponent(component);

        return asset->root();
    }

    SceneObjectPtr SceneObjectFactory::createSawerSawGibbed()
    {
        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("sawer_saw_gibbed.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemy);
        }

        return asset->root();
    }

    void SceneObjectFactory::makeDeadbody(const SceneObjectPtr& obj, float radius)
    {
        b2BodyDef bodyDef;

        bodyDef.type = b2_dynamicBody;
        bodyDef.linearDamping = 3.0f;
        bodyDef.angularDamping = 3.0f;

        RUBEBodyPtr body = boost::make_shared<RUBEBody>("", bodyDef);

        RUBECircleFixturePtr fixture = boost::make_shared<RUBECircleFixture>("");

        fixture->fixtureDef().filter.categoryBits = collisionBitEnemy;
        fixture->fixtureDef().density = 1.0f;

        fixture->shape().m_p = b2Vec2_zero;
        fixture->shape().m_radius = radius;

        body->addFixture(fixture);

        {
            ComponentPtr component =
                boost::make_shared<PhysicsBodyComponent>(body);

            obj->addComponent(component);
        }

        obj->setBodyDef(body->bodyDef());

        obj->becomeDeadbody();
    }

    bool SceneObjectFactory::makeDebris(const SceneObjectPtr& obj)
    {
        PhysicsBodyComponentPtr pc = obj->findComponent<PhysicsBodyComponent>();

        if (!pc || (pc->rubeBody()->numFixtures() < 1) ||
            pc->rubeBody()->fixture(0)->fixtureDef().isSensor) {
            return false;
        }

        obj->setType(SceneObjectTypeGarbage);

        obj->setCollisionImpulseMultiplier(0.0f);

        if (obj->body()) {
            pc->setFilterCategoryBits(collisionBitGarbage);
            pc->setFilterMaskBits(0xFFFF);
            pc->setFilterGroupIndex(collisionGroupMissile);
        } else {
            b2Filter filter;

            filter.categoryBits = collisionBitGarbage;
            filter.groupIndex = collisionGroupMissile;

            pc->setFilterOverride(filter);
        }

        SceneObjectTypes types;

        types.set(SceneObjectTypePlayer);
        types.set(SceneObjectTypeEnemy);
        types.set(SceneObjectTypeEnemyBuilding);
        types.set(SceneObjectTypeAlly);
        types.set(SceneObjectTypeGizmo);
        types.set(SceneObjectTypeVehicle);
        types.set(SceneObjectTypeRock);

        CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
        c->setFilter(types);
        c->setNoStatic(true);
        obj->addComponent(c);

        std::vector<RenderQuadComponentPtr> tmp = obj->findComponents<RenderQuadComponent>();

        for (std::vector<RenderQuadComponentPtr>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
            (*it)->setZOrder(-92);
        }

        return true;
    }

    SceneObjectPtr SceneObjectFactory::createPuzzleHint()
    {
        SceneObjectPtr obj = boost::make_shared<SceneObject>();

        obj->setFreezable(true);
        obj->setFreezeRadius(settings.puzzleHint.radius * 2.0f);

/*        RenderDottedCircleComponentPtr dcc =
            boost::make_shared<RenderDottedCircleComponent>(b2Vec2_zero, 0.3f,
                assetManager.getDrawable("common2/dot.png"), zOrderMarker + 1);

        dcc->setColor(settings.puzzleHint.color);
        dcc->setRadius(settings.puzzleHint.radius);
        dcc->setDotVelocity(5.0f, settings.puzzleHint.radius);
        dcc->setDotDistance(0.5f, settings.puzzleHint.radius);

        obj->addComponent(boost::make_shared<GoalComponent>(dcc));*/

        DrawablePtr drawable = assetManager.getDrawable("common1/hint_arrow.png");

        float height = settings.puzzleHint.height;
        float width = height * drawable->image().aspect();

        RenderQuadComponentPtr rc =
            boost::make_shared<RenderQuadComponent>(b2Vec2(0.0f, height * 0.5f), 0.0f, width, height,
                drawable, zOrderMarker + 2);

        obj->addComponent(rc);

        obj->addComponent(boost::make_shared<PuzzleHintComponent>(rc));

        return obj;
    }

    SceneObjectPtr SceneObjectFactory::createBossBeholder()
    {
        SceneObjectTypes types;

        types.set(SceneObjectTypePlayer);

        SceneObjectAssetPtr asset = assetManager.getSceneObjectAsset("boss_beholder.json");

        for (int i = 0; i < asset->numObjects(); ++i) {
            asset->object(i)->setType(SceneObjectTypeEnemyBuilding);
            asset->object(i)->setMaterial(MaterialFlesh);
            asset->object(i)->setPropagateDamage(false);

            if (asset->object(i)->name() == "leg") {
                CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
                c->setFilter(types);
                c->setDamage(settings.bossBeholder.legDamage);
                c->setDamageSound(audio.createSound("player_hit.ogg"));
                c->setRoll(deg2rad(2.0f));
                asset->object(i)->addComponent(c);
            }
        }

        asset->root()->setMaxLife(settings.bossBeholder.life);
        asset->root()->setLife(settings.bossBeholder.life);

        RenderQuadComponentPtr rc = asset->root()->findComponentByName<RenderQuadComponent>("root");
        QuadPulseComponentPtr pc = boost::make_shared<QuadPulseComponent>(rc, b2Vec2_zero,
            0.5f, EaseInOutQuad, 0.98f,
            0.5f, EaseInOutQuad, 1.02f);
        pc->setT(0.0f);
        asset->root()->addComponent(pc);

        LightComponentPtr lc = asset->root()->findComponent<LightComponent>();

        LightPtr fireLight[] = {
            lc->getLight<Light>("light1"),
            lc->getLight<Light>("light2"),
            lc->getLight<Light>("light3"),
            lc->getLight<Light>("light4"),
            lc->getLight<Light>("light5"),
            lc->getLight<Light>("light6")
        };

        BossBeholderComponentPtr component = boost::make_shared<BossBeholderComponent>(asset->object("legs"),
            asset->object("guns"),
            lc->getLight<Light>("light"),
            fireLight);

        for (int i = 0; i < 8; ++i) {
            std::ostringstream os;

            os << "gun" << (i + 1);

            const RUBEImagePtr& image = asset->world()->body(os.str())->image("missile");

            WeaponFireballComponentPtr weapon =
                boost::make_shared<WeaponFireballComponent>(SceneObjectTypeEnemyMissile);

            weapon->setDamage(settings.bossBeholder.fireballDamage);
            weapon->setVelocity(settings.bossBeholder.fireballVelocity);
            weapon->setInterval(0.0f);
            weapon->setPos(image->pos());
            weapon->setAngle(image->angle());
            weapon->setUseFreeze(false);
            weapon->setHaveSound(i == 0);
            weapon->setHitHaveSound(false);

            component->setGunWeapon(i, weapon);

            asset->object(os.str())->addComponent(weapon);

            WeaponHeaterComponentPtr heaterWeapon =
                boost::make_shared<WeaponHeaterComponent>(SceneObjectTypeEnemyMissile);

            heaterWeapon->setDamage(settings.bossBeholder.laserDamage);
            heaterWeapon->setLength(100.0f);
            heaterWeapon->setImpulse(settings.bossBeholder.laserImpulse);
            heaterWeapon->setDuration(0.8f);
            heaterWeapon->setInterval(0.0f);
            heaterWeapon->setMinWidth(0.8f);
            heaterWeapon->setMaxWidth(1.8f);
            heaterWeapon->setHaveSound(false);
            heaterWeapon->setPos(asset->world()->image("missile")->pos());
            heaterWeapon->setAngle(asset->world()->image("missile")->angle());

            component->setLaserWeapon(i, heaterWeapon);

            asset->object(os.str())->addComponent(heaterWeapon);

            WeaponAimComponentPtr aimWeapon =
                boost::make_shared<WeaponAimComponent>(SceneObjectTypeEnemyMissile);

            aimWeapon->setLength(100.0f);
            aimWeapon->setPos(asset->world()->image("missile")->pos());
            aimWeapon->setAngle(asset->world()->image("missile")->angle());

            component->setAimWeapon(i, aimWeapon);

            asset->object(os.str())->addComponent(aimWeapon);
        }

        WeaponEShieldComponentPtr shield0 =
            boost::make_shared<WeaponEShieldComponent>(SceneObjectTypeEnemyMissile);

        shield0->setAngle(0.0f);
        shield0->setHaveSound(true);

        component->setShieldWeapon(0, shield0);
        asset->root()->addComponent(shield0);

        WeaponEShieldComponentPtr shield1 =
            boost::make_shared<WeaponEShieldComponent>(SceneObjectTypeEnemyMissile);

        shield1->setAngle(b2_pi * 2.0f / 3.0f);
        shield1->setHaveSound(false);

        component->setShieldWeapon(1, shield1);
        asset->root()->addComponent(shield1);

        WeaponEShieldComponentPtr shield2 =
            boost::make_shared<WeaponEShieldComponent>(SceneObjectTypeEnemyMissile);

        shield2->setAngle(b2_pi * 4.0f / 3.0f);
        shield2->setHaveSound(false);

        component->setShieldWeapon(2, shield2);
        asset->root()->addComponent(shield2);

        shield0->setDistance(2.5f);
        shield1->setDistance(2.5f);
        shield2->setDistance(2.5f);

        asset->root()->addComponent(component);

        return asset->root();
    }
}
