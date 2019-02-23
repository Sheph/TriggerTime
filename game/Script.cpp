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

#include "ScriptImpl.h"

namespace af
{
    static int errorHandler(lua_State* L)
    {
        lua_Debug d;

        ::lua_getstack(L, 1, &d);
        ::lua_getinfo(L, "Sln", &d);

        std::string err = ::lua_tostring(L, -1);

        ::lua_pop(L, 1);

        std::ostringstream os;

        os << d.short_src << ": " << d.currentline;

        if (d.name != 0) {
            os << " (" << d.namewhat << " " << d.name << ")";
        }

        os << " - " << err;

        ::lua_pushstring(L, os.str().c_str());

        LOG4CPLUS_ERROR(logger(), os.str());

        return 1;
    }

    Script::Impl::Impl(const std::string& path,
         Scene* scene)
    : path_(path),
      scene_(scene),
      L_(NULL)
    {
    }

    Script::Impl::~Impl()
    {
        if (L_) {
            ::lua_close(L_);
        }
    }

    void Script::Impl::bind()
    {
        /*
         * Eclipse takes forever parsing this file
         * because of the bindings, so bypass eclipse indexer
         * using this define hack which is visible to compiler, but
         * not to eclipse.
         */
#ifdef BYPASS_ECLIPSE_INDEXER
        luabind::module(L_)
        [
            luabind::class_<GlobalConstHolder>("const")
                .enum_("constants")
                [
                    luabind::value("zOrderBackground", zOrderBackground),
                    luabind::value("zOrderBack", zOrderBack),
                    luabind::value("zOrderMain", zOrderMain),
                    luabind::value("zOrderExplosion", zOrderExplosion),
                    luabind::value("zOrderTerrain", zOrderTerrain),
                    luabind::value("zOrderFront", zOrderFront),
                    luabind::value("zOrderMarker", zOrderMarker),

                    luabind::value("SceneObjectTypeOther", SceneObjectTypeOther),
                    luabind::value("SceneObjectTypePlayer", SceneObjectTypePlayer),
                    luabind::value("SceneObjectTypeEnemy", SceneObjectTypeEnemy),
                    luabind::value("SceneObjectTypeEnemyBuilding", SceneObjectTypeEnemyBuilding),
                    luabind::value("SceneObjectTypePlayerMissile", SceneObjectTypePlayerMissile),
                    luabind::value("SceneObjectTypeEnemyMissile", SceneObjectTypeEnemyMissile),
                    luabind::value("SceneObjectTypeTerrain", SceneObjectTypeTerrain),
                    luabind::value("SceneObjectTypeRock", SceneObjectTypeRock),
                    luabind::value("SceneObjectTypeAlly", SceneObjectTypeAlly),
                    luabind::value("SceneObjectTypeAllyMissile", SceneObjectTypeAllyMissile),
                    luabind::value("SceneObjectTypeNeutralMissile", SceneObjectTypeNeutralMissile),
                    luabind::value("SceneObjectTypeGizmo", SceneObjectTypeGizmo),
                    luabind::value("SceneObjectTypeGarbage", SceneObjectTypeGarbage),
                    luabind::value("SceneObjectTypeBlocker", SceneObjectTypeBlocker),
                    luabind::value("SceneObjectTypeVehicle", SceneObjectTypeVehicle),
                    luabind::value("SceneObjectTypeDeadbody", SceneObjectTypeDeadbody),

                    luabind::value("MaterialOther", MaterialOther),
                    luabind::value("MaterialFlesh", MaterialFlesh),

                    luabind::value("EaseLinear", EaseLinear),
                    luabind::value("EaseInQuad", EaseInQuad),
                    luabind::value("EaseOutQuad", EaseOutQuad),
                    luabind::value("EaseInOutQuad", EaseInOutQuad),

                    luabind::value("BodyStatic", b2_staticBody),
                    luabind::value("BodyKinematic", b2_kinematicBody),
                    luabind::value("BodyDynamic", b2_dynamicBody),

                    luabind::value("AnimationNone", static_cast<int>(AnimationNone)),
                    luabind::value("AnimationDefault", static_cast<int>(AnimationDefault)),

                    luabind::value("WrapModeRepeat", Texture::WrapModeRepeat),
                    luabind::value("WrapModeClamp", Texture::WrapModeClamp),

                    luabind::value("WeaponTypeGG", WeaponTypeGG),
                    luabind::value("WeaponTypeBlaster", WeaponTypeBlaster),
                    luabind::value("WeaponTypeShotgun", WeaponTypeShotgun),
                    luabind::value("WeaponTypeRLauncher", WeaponTypeRLauncher),
                    luabind::value("WeaponTypeFireball", WeaponTypeFireball),
                    luabind::value("WeaponTypeFlamethrower", WeaponTypeFlamethrower),
                    luabind::value("WeaponTypeSuperBlaster", WeaponTypeSuperBlaster),
                    luabind::value("WeaponTypeProxMine", WeaponTypeProxMine),
                    luabind::value("WeaponTypeChainsaw", WeaponTypeChainsaw),
                    luabind::value("WeaponTypeLGun", WeaponTypeLGun),
                    luabind::value("WeaponTypePlasmaGun", WeaponTypePlasmaGun),
                    luabind::value("WeaponTypeMachineGun", WeaponTypeMachineGun),
                    luabind::value("WeaponTypeSuperShotgun", WeaponTypeSuperShotgun),
                    luabind::value("WeaponTypeEShield", WeaponTypeEShield),

                    luabind::value("InventoryItemDynomite", InventoryItemDynomite),
                    luabind::value("InventoryItemRedKey", InventoryItemRedKey),
                    luabind::value("InventoryItemBlueKey", InventoryItemBlueKey),
                    luabind::value("InventoryItemYellowKey", InventoryItemYellowKey),

                    luabind::value("collisionBitPlayer", collisionBitPlayer),
                    luabind::value("collisionBitEnemy", collisionBitEnemy),
                    luabind::value("collisionBitEnemyBuilding", collisionBitEnemyBuilding),
                    luabind::value("collisionBitRock", collisionBitRock),
                    luabind::value("collisionBitCustom2", collisionBitCustom2),

                    luabind::value("ActionIdMoveUp", ActionIdMoveUp),
                    luabind::value("ActionIdMoveDown", ActionIdMoveDown),
                    luabind::value("ActionIdMoveLeft", ActionIdMoveLeft),
                    luabind::value("ActionIdMoveRight", ActionIdMoveRight),
                    luabind::value("ActionIdPrimaryFire", ActionIdPrimaryFire),
                    luabind::value("ActionIdSecondaryFire", ActionIdSecondaryFire),
                    luabind::value("ActionIdPrimarySwitch", ActionIdPrimarySwitch),
                    luabind::value("ActionIdSecondarySwitch", ActionIdSecondarySwitch),
                    luabind::value("ActionIdInteract", ActionIdInteract),
                    luabind::value("ActionIdRun", ActionIdRun),
                    luabind::value("ActionIdMax", ActionIdMax),

                    luabind::value("ActionGamepadIdPrimaryFire", ActionGamepadIdPrimaryFire),
                    luabind::value("ActionGamepadIdSecondaryFire", ActionGamepadIdSecondaryFire),
                    luabind::value("ActionGamepadIdPrimarySwitch", ActionGamepadIdPrimarySwitch),
                    luabind::value("ActionGamepadIdSecondarySwitch", ActionGamepadIdSecondarySwitch),
                    luabind::value("ActionGamepadIdInteract", ActionGamepadIdInteract),
                    luabind::value("ActionGamepadIdRun", ActionGamepadIdRun),
                    luabind::value("ActionGamepadIdMax", ActionGamepadIdMax),

                    luabind::value("UpgradeIdExtraLife", UpgradeIdExtraLife),
                    luabind::value("UpgradeIdMoreAmmo", UpgradeIdMoreAmmo),
                    luabind::value("UpgradeIdArmor", UpgradeIdArmor),
                    luabind::value("UpgradeIdSuperBlaster", UpgradeIdSuperBlaster),
                    luabind::value("UpgradeIdMax", UpgradeIdMax),

                    luabind::value("SkillEasy", SkillEasy),
                    luabind::value("SkillNormal", SkillNormal),
                    luabind::value("SkillHard", SkillHard),

                    luabind::value("LanguageEnglish", LanguageEnglish),
                    luabind::value("LanguageRussian", LanguageRussian),

                    luabind::value("GamepadUnknown", GamepadUnknown),
                    luabind::value("GamepadDPADUp", GamepadDPADUp),
                    luabind::value("GamepadDPADDown", GamepadDPADDown),
                    luabind::value("GamepadDPADLeft", GamepadDPADLeft),
                    luabind::value("GamepadDPADRight", GamepadDPADRight),
                    luabind::value("GamepadStart", GamepadStart),
                    luabind::value("GamepadBack", GamepadBack),
                    luabind::value("GamepadLeftStick", GamepadLeftStick),
                    luabind::value("GamepadRightStick", GamepadRightStick),
                    luabind::value("GamepadLeftBumper", GamepadLeftBumper),
                    luabind::value("GamepadRightBumper", GamepadRightBumper),
                    luabind::value("GamepadLeftTrigger", GamepadLeftTrigger),
                    luabind::value("GamepadRightTrigger", GamepadRightTrigger),
                    luabind::value("GamepadA", GamepadA),
                    luabind::value("GamepadB", GamepadB),
                    luabind::value("GamepadX", GamepadX),
                    luabind::value("GamepadY", GamepadY),
                    luabind::value("GamepadButtonMax", GamepadButtonMax),

                    luabind::value("AchievementGravityGunExpert", AchievementGravityGunExpert),
                    luabind::value("AchievementRoasted", AchievementRoasted),
                    luabind::value("AchievementArachnophobia", AchievementArachnophobia),
                    luabind::value("AchievementImpresses", AchievementImpresses),
                    luabind::value("AchievementAimBot", AchievementAimBot),
                    luabind::value("AchievementFreedom", AchievementFreedom),
                    luabind::value("AchievementHeartbroken", AchievementHeartbroken),
                    luabind::value("AchievementThatWasClose", AchievementThatWasClose),
                    luabind::value("AchievementNotAScratch", AchievementNotAScratch),
                    luabind::value("AchievementEatThat", AchievementEatThat),
                    luabind::value("AchievementFisticuffs", AchievementFisticuffs),
                    luabind::value("AchievementBugged", AchievementBugged),
                    luabind::value("AchievementWormFeeling", AchievementWormFeeling),
                    luabind::value("AchievementDopefishLives", AchievementDopefishLives),
                    luabind::value("AchievementTurnBack", AchievementTurnBack),
                    luabind::value("AchievementDieAlready", AchievementDieAlready),
                    luabind::value("AchievementFreshAir", AchievementFreshAir),
                    luabind::value("AchievementSaveTheAnimals", AchievementSaveTheAnimals),
                    luabind::value("AchievementKillTheAnimals", AchievementKillTheAnimals),
                    luabind::value("AchievementImSorryBuddy", AchievementImSorryBuddy),
                    luabind::value("AchievementTheEnd", AchievementTheEnd),
                    luabind::value("AchievementHardcore", AchievementHardcore),
                    luabind::value("AchievementChicken", AchievementChicken),
                    luabind::value("AchievementExplorer", AchievementExplorer),
                    luabind::value("AchievementMax", AchievementMax)
                ],

            luabind::class_<Settings>("Settings")
                .scope
                [
                    luabind::class_<Settings::Player>("Player")
                        .def_readonly("moveSpeed", &Settings::Player::moveSpeed),
                    luabind::class_<Settings::Jar>("Jar")
                        .def_readonly("life", &Settings::Jar::life),
                    luabind::class_<Settings::Heater>("Heater")
                        .def_readonly("life", &Settings::Heater::life),
                    luabind::class_<Settings::BossSkull>("BossSkull")
                        .def_readonly("slowWalkSpeed", &Settings::BossSkull::slowWalkSpeed)
                        .def_readonly("fastWalkSpeed", &Settings::BossSkull::fastWalkSpeed),
                    luabind::class_<Settings::Scorp>("Scorp")
                        .def_readonly("shootDamage", &Settings::Scorp::shootDamage)
                        .def_readonly("shootVelocity", &Settings::Scorp::shootVelocity)
                ]
                .def_readonly("developer", &Settings::developer)
                .def_readonly("player", &Settings::player)
                .def_readonly("bossSkull", &Settings::bossSkull)
                .def_readonly("scorp", &Settings::scorp)
                .def("setDeveloper", &Settings::setDeveloper)
                .def("setGamma", &Settings::setGamma)
                .def("setSkill", &Settings::setSkill)
                .def_readonly("jar1", &Settings::jar1)
                .def_readonly("jar2", &Settings::jar2)
                .def_readonly("jar3", &Settings::jar3)
                .def_readonly("jar4", &Settings::jar4)
                .def_readonly("heater1", &Settings::heater1)
                .def_readonly("videoMode", &Settings::videoMode)
                .def_readonly("msaaMode", &Settings::msaaMode)
                .def_readonly("vsync", &Settings::vsync)
                .def_readonly("fullscreen", &Settings::fullscreen)
                .def_readonly("trilinearFilter", &Settings::trilinearFilter)
                .def_readonly("gamma", &Settings::gamma)
                .def_readonly("skill", &Settings::skill),

            luabind::class_<Scene>("Scene")
                .def("addObject", &Scene::addObject)
                .def("getObjects", (std::vector<SceneObjectPtr> (Scene::*)() const)&Scene::getObjects)
                .def("getObjects", (std::vector<SceneObjectPtr> (Scene::*)(const std::string&) const)&Scene::getObjects)
                .def("findObject", &Scene::findObject)
                .def("addGearJoint", &Scene::addGearJoint)
                .def("addWeldJoint", &Scene::addWeldJoint)
                .def("addFrictionJoint", &Scene::addFrictionJoint)
                .def("addRevoluteJoint", &Scene::addRevoluteJoint)
                .def("addMotorJoint", &Scene::addMotorJoint)
                .def("getJoints", &Scene::getJoints)
                .def("removeJoint", &Scene::removeJoint)
                .def("addTimer", &Scene::addTimer)
                .def("removeTimer", &Scene::removeTimer)
                .def("chainToNextLevel", &Scene::chainToNextLevel)
                .def("setNextLevel", &Scene::setNextLevel)
                .def("instanciate", &Scene::script_instanciate)
                .def("instanciate", &Scene::script_instanciateByTransform)
                .def("getInstances", &Scene::script_getInstances)
                .def("addCombatAlly", &Scene::addCombatAlly)
                .def("removeCombatAlly", &Scene::removeCombatAlly)
                .def("queryObjects", &Scene::script_queryObjects)
                .property("combatAllies", &Scene::combatAllies)
                .property("camera", &Scene::camera)
                .property("player", &Scene::player)
                .property("respawnPoint", &Scene::respawnPoint, &Scene::setRespawnPoint)
                .property("cutscene", &Scene::cutscene, &Scene::setCutscene)
                .property("quit", &Scene::quit, &Scene::setQuit)
                .property("gameWidth", &Scene::gameWidth)
                .property("gameHeight", &Scene::gameHeight)
                .property("lighting", &Scene::lighting)
                .property("inputPlayer", &Scene::inputPlayer)
                .property("inputRoboArm", &Scene::inputRoboArm)
                .property("inputBoat", &Scene::inputBoat)
                .property("inputMech", &Scene::inputMech)
                .property("inputTurret", &Scene::inputTurret)
                .property("paused", &Scene::paused, &Scene::setPaused)
                .property("playable", &Scene::playable)
                .property("scriptPath", &Scene::scriptPath)
                .property("assetPath", &Scene::assetPath)
                .property("basePath", &Scene::basePath)
                .property("stats", &Scene::stats),

            luabind::class_<SceneObjectFactory>("SceneObjectFactory")
                .def("createDummy", &SceneObjectFactory::createDummy)
                .def("createBackground", &SceneObjectFactory::createBackground)
                .def("createExplosion1", &SceneObjectFactory::createExplosion1)
                .def("createBlocker", &SceneObjectFactory::createBlocker)
                .def("createTetrobot", &SceneObjectFactory::createTetrobot)
                .def("createTetrobot2", &SceneObjectFactory::createTetrobot2)
                .def("createTetrocrab", &SceneObjectFactory::createTetrocrab)
                .def("createTetrocrab2", &SceneObjectFactory::createTetrocrab2)
                .def("createTetrocrab3", &SceneObjectFactory::createTetrocrab3)
                .def("createTetrocrab4", &SceneObjectFactory::createTetrocrab4)
                .def("createScorp", &SceneObjectFactory::createScorp)
                .def("createScorp2", &SceneObjectFactory::createScorp2)
                .def("createScorp3", &SceneObjectFactory::createScorp3)
                .def("createScorp4", &SceneObjectFactory::createScorp4)
                .def("createPowerupHealth", &SceneObjectFactory::createPowerupHealth)
                .def("createPowerupGem", &SceneObjectFactory::createPowerupGem)
                .def("createPowerupInventory", &SceneObjectFactory::createPowerupInventory)
                .def("createPowerupAmmo", &SceneObjectFactory::createPowerupAmmo)
                .def("createShroomGuardianRed1", &SceneObjectFactory::createShroomGuardianRed1)
                .def("createShroomGuardianRed2", &SceneObjectFactory::createShroomGuardianRed2)
                .def("createGoal", (ComponentPtr (SceneObjectFactory::*)(const SceneObjectPtr&))&SceneObjectFactory::createGoal)
                .def("createPlaceholder", (ComponentPtr (SceneObjectFactory::*)(InventoryItem, float, float, int, int, int))&SceneObjectFactory::createPlaceholder)
                .def("createTimebomb", &SceneObjectFactory::createTimebomb)
                .def("createBarrel1", &SceneObjectFactory::createBarrel1)
                .def("createBarrel2", &SceneObjectFactory::createBarrel2)
                .def("createBarrel3", &SceneObjectFactory::createBarrel3)
                .def("createSpider", &SceneObjectFactory::createSpider)
                .def("createBabySpider1", &SceneObjectFactory::createBabySpider1)
                .def("createBabySpider2", &SceneObjectFactory::createBabySpider2)
                .def("createEnforcer1", &SceneObjectFactory::createEnforcer1)
                .def("createSentry1", &SceneObjectFactory::createSentry1)
                .def("createSentry2", &SceneObjectFactory::createSentry2)
                .def("createInvulnerability", &SceneObjectFactory::createInvulnerability)
                .def("createGorger1", &SceneObjectFactory::createGorger1)
                .def("createKeeper", &SceneObjectFactory::createKeeper)
                .def("createSummon1", &SceneObjectFactory::createSummon1)
                .def("createSummon2", &SceneObjectFactory::createSummon2)
                .def("createWarder", &SceneObjectFactory::createWarder)
                .def("createOrbo", &SceneObjectFactory::createOrbo)
                .def("createToxicSplash1", &SceneObjectFactory::createToxicSplash1)
                .def("createBeetle1", &SceneObjectFactory::createBeetle1)
                .def("createQuad", &SceneObjectFactory::script_createQuad)
                .def("createCentipede1", &SceneObjectFactory::createCentipede1)
                .def("createHomer", &SceneObjectFactory::createHomer)
                .def("createEnforcer2", &SceneObjectFactory::createEnforcer2)
                .def("createGasCloud1", &SceneObjectFactory::createGasCloud1)
                .def("createRope", &SceneObjectFactory::createRope)
                .def("createPowerBeam", &SceneObjectFactory::createPowerBeam)
                .def("createScientist", &SceneObjectFactory::createScientist)
                .def("createGuardian", &SceneObjectFactory::createGuardian)
                .def("createGuardian2", &SceneObjectFactory::createGuardian2)
                .def("createPlayer", &SceneObjectFactory::createPlayer)
                .def("createWeaponStation", &SceneObjectFactory::createWeaponStation),

            luabind::class_<AudioSource, AudioSourcePtr>("AudioSource")
                .def("play", &AudioSource::play)
                .def("pause", &AudioSource::pause)
                .def("stop", &AudioSource::stop)
                .property("loop", &AudioSource::loop, &AudioSource::setLoop)
                .property("isMusic", &AudioSource::isMusic, &AudioSource::setIsMusic),

            luabind::class_<AudioManager>("AudioManager")
                .property("volume", &AudioManager::volume, &AudioManager::setVolume)
                .property("soundVolume", &AudioManager::soundVolume, &AudioManager::setSoundVolume)
                .property("musicVolume", &AudioManager::musicVolume, &AudioManager::setMusicVolume)
                .def("createSound", &AudioManager::createSound)
                .def("playSound", &AudioManager::playSound)
                .def("createStream", &AudioManager::createStream)
                .def("crossfade", &AudioManager::crossfade)
                .def("stopAll", &AudioManager::stopAll),

            luabind::class_<InputContext>("InputContext")
                .property("active", &InputContext::active, &InputContext::setActive),

            luabind::class_<InputContextPlayer, InputContext>("InputContextPlayer")
                .property("secondaryShowOff", &InputContextPlayer::secondaryShowOff, &InputContextPlayer::setSecondaryShowOff)
                .property("primarySwitchShowOff", &InputContextPlayer::primarySwitchShowOff, &InputContextPlayer::setPrimarySwitchShowOff)
                .property("secondarySwitchShowOff", &InputContextPlayer::secondarySwitchShowOff, &InputContextPlayer::setSecondarySwitchShowOff),

            luabind::class_<InputContextRoboArm, InputContext>("InputContextRoboArm"),

            luabind::class_<InputContextBoat, InputContext>("InputContextBoat"),

            luabind::class_<InputContextMech, InputContext>("InputContextMech"),

            luabind::class_<InputContextTurret, InputContext>("InputContextTurret")
                .def("setShootImage", &InputContextTurret::script_setShootImage),

            luabind::class_<Component, ComponentPtr>("Component")
                .def(luabind::const_self == luabind::const_self)
                .property("parent", &Component::script_parent)
                .def("removeFromParent", &Component::removeFromParent),

            luabind::class_<PhasedComponent, Component, ScriptComponent, ComponentPtr>("PhasedComponent")
                .def(luabind::constructor<luabind::object>())
                .property("phases", &PhasedComponent::phases),

            luabind::class_<TargetableComponent, PhasedComponent, ComponentPtr>("TargetableComponent")
                .def("setTarget", &TargetableComponent::setTarget)
                .property("target", &TargetableComponent::target, &TargetableComponent::setTarget)
                .property("autoTarget", &TargetableComponent::autoTarget, &TargetableComponent::setAutoTarget)
                .property("followPlayer", &TargetableComponent::followPlayer, &TargetableComponent::setFollowPlayer),

            luabind::class_<CollisionComponent, Component, ScriptCollisionComponent, ComponentPtr>("CollisionComponent")
                .def(luabind::constructor<luabind::object>()),

            luabind::class_<RenderComponent, Component, ComponentPtr>("RenderComponent")
                .property("visible", &RenderComponent::visible, &RenderComponent::setVisible)
                .property("color", &RenderComponent::color, &RenderComponent::setColor)
                .property("zOrder", &RenderComponent::zOrder, &RenderComponent::setZOrder)
                .property("name", &RenderComponent::name, &RenderComponent::setName),

            luabind::class_<PhysicsComponent, Component, ComponentPtr>("PhysicsComponent"),

            luabind::class_<UIComponent, Component, ComponentPtr>("UIComponent")
                .property("zOrder", &UIComponent::zOrder),

            luabind::class_<CollisionSensorComponent, CollisionComponent, ComponentPtr>("CollisionSensorComponent")
                .def(luabind::constructor<>())
                .property("listener", &CollisionSensorComponent::listener, &CollisionSensorComponent::setListener)
                .property("allowSensor", &CollisionSensorComponent::allowSensor, &CollisionSensorComponent::setAllowSensor),

            luabind::class_<CollisionDamageComponent, CollisionComponent, ComponentPtr>("CollisionDamageComponent")
                .def(luabind::constructor<>())
                .property("impulseThreshold", &CollisionDamageComponent::impulseThreshold, &CollisionDamageComponent::setImpulseThreshold)
                .property("enabled", &CollisionDamageComponent::enabled, &CollisionDamageComponent::setEnabled)
                .def("addObjectFilter", &CollisionDamageComponent::script_addObjectFilter),

            luabind::class_<CollisionCancelComponent, CollisionComponent, ComponentPtr>("CollisionCancelComponent")
                .def(luabind::constructor<>())
                .property("damage", &CollisionCancelComponent::damage, &CollisionCancelComponent::setDamage)
                .property("damageSound", &CollisionCancelComponent::damageSound, &CollisionCancelComponent::setDamageSound)
                .property("roll", &CollisionCancelComponent::roll, &CollisionCancelComponent::setRoll)
                .property("immediate", &CollisionCancelComponent::immediate, &CollisionCancelComponent::setImmediate)
                .def("setFilter", &CollisionCancelComponent::setFilter)
                .def("addObjectFilter", &CollisionCancelComponent::script_addObjectFilter),

            luabind::class_<CollisionModifyComponent, CollisionComponent, ComponentPtr>("CollisionModifyComponent")
                .def(luabind::constructor<>())
                .def("addObjectFilter", &CollisionModifyComponent::script_addObjectFilter)
                .def("setFilter", &CollisionModifyComponent::setFilter)
                .def("modifyFriction", &CollisionModifyComponent::modifyFriction)
                .def("modifyRestitution", &CollisionModifyComponent::modifyRestitution),

            luabind::class_<ShroomGuardianComponent, TargetableComponent, ComponentPtr>("ShroomGuardianComponent"),

            luabind::class_<PyrobotComponent, TargetableComponent, ComponentPtr>("PyrobotComponent")
                .property("patrol", &PyrobotComponent::patrol, &PyrobotComponent::setPatrol),

            luabind::class_<RenderBackgroundComponent, RenderComponent, ComponentPtr>("RenderBackgroundComponent")
                .property("offset", &RenderBackgroundComponent::offset, &RenderBackgroundComponent::setOffset)
                .property("drawable", &RenderBackgroundComponent::drawable)
                .property("unbound", &RenderBackgroundComponent::unbound, &RenderBackgroundComponent::setUnbound),

            luabind::class_<CameraComponent, TargetableComponent, ComponentPtr>("CameraComponent")
                .def("shake", &CameraComponent::shake)
                .def("roll", &CameraComponent::roll)
                .def("moveTo", &CameraComponent::moveTo)
                .def("follow", &CameraComponent::follow)
                .def("zoomTo", &CameraComponent::zoomTo)
                .def("tremor", &CameraComponent::tremor)
                .def("tremorStart", &CameraComponent::tremorStart)
                .def("pointVisible", &CameraComponent::pointVisible)
                .def("rectVisible", &CameraComponent::rectVisible)
                .def("setConstraint", &CameraComponent::setConstraint)
                .property("constraint", &CameraComponent::constraint)
                .property("useTargetAngle", &CameraComponent::useTargetAngle, &CameraComponent::setUseTargetAngle),

            luabind::class_<PlayerComponent, PhasedComponent, ComponentPtr>("PlayerComponent")
                .property("weapon", &PlayerComponent::weapon)
                .property("altWeapon", &PlayerComponent::altWeapon)
                .def("giveWeapon", &PlayerComponent::giveWeapon)
                .def("haveWeapon", &PlayerComponent::haveWeapon)
                .def("ammo", &PlayerComponent::ammo)
                .def("changeAmmo", &PlayerComponent::changeAmmo)
                .def("changeLifeNoDamage", &PlayerComponent::changeLifeNoDamage)
                .property("inventory", &PlayerComponent::inventory)
                .property("jetpack", &PlayerComponent::jetpack, &PlayerComponent::setJetpack)
                .property("flashlight", &PlayerComponent::flashlight, &PlayerComponent::setFlashlight)
                .property("earpiece", &PlayerComponent::earpiece, &PlayerComponent::setEarpiece)
                .property("standing", &PlayerComponent::standing, &PlayerComponent::setStanding)
                .property("haveGun", &PlayerComponent::haveGun, &PlayerComponent::setHaveGun)
                .property("haveBackpack", &PlayerComponent::haveBackpack, &PlayerComponent::setHaveBackpack)
                .property("numGems", &PlayerComponent::numGems, &PlayerComponent::setNumGems)
                .property("numHearts", &PlayerComponent::numHearts)
                .property("flagLifeLost", &PlayerComponent::flagLifeLost)
                .property("flagDamaged", &PlayerComponent::flagDamaged)
                .property("flagDamagedByCoreProtectorMelee", &PlayerComponent::flagDamagedByCoreProtectorMelee)
                .property("flagDamagedByAcidWormOnBoat", &PlayerComponent::flagDamagedByAcidWormOnBoat)
                .property("flagDamagedByGas", &PlayerComponent::flagDamagedByGas),

            luabind::class_<DialogComponent, UIComponent, ScriptDialogComponent, ComponentPtr>("DialogComponent")
                .def(luabind::constructor<luabind::object, b2Vec2, float, float>())
                .def("setTitlePlayer", &DialogComponent::setTitlePlayer)
                .def("setTitleAlly", &DialogComponent::setTitleAlly)
                .def("setTitleEnemy", &DialogComponent::setTitleEnemy)
                .def("setMessage", &DialogComponent::setMessage)
                .def("endDialog", &DialogComponent::endDialog)
                .property("fastComplete", &DialogComponent::fastComplete, &DialogComponent::setFastComplete),

            luabind::class_<UITimerComponent, UIComponent, ScriptUITimerComponent, ComponentPtr>("UITimerComponent")
                .def(luabind::constructor<luabind::object, float, int>()),

            luabind::class_<MainMenuComponent, UIComponent, ScriptMainMenuComponent, ComponentPtr>("MainMenuComponent")
                .def(luabind::constructor<luabind::object>())
                .def("addItem", &MainMenuComponent::addItem),

            luabind::class_<PickerComponent, UIComponent, ScriptPickerComponent, ComponentPtr>("PickerComponent")
                .def(luabind::constructor<luabind::object>())
                .def("setHeader", &PickerComponent::setHeader)
                .def("addItem", &PickerComponent::addItem),

            luabind::class_<ChoiceComponent, UIComponent, ScriptChoiceComponent, ComponentPtr>("ChoiceComponent")
                .def(luabind::constructor<luabind::object, int>())
                .def("addItem", &ChoiceComponent::addItem),

            luabind::class_<StainedGlassComponent, UIComponent, ComponentPtr>("StainedGlassComponent")
                .def(luabind::constructor<int>())
                .property("color", &StainedGlassComponent::color)
                .def("setColor", &StainedGlassComponent::setColor),

            luabind::class_<LevelCompletedComponent, UIComponent, ComponentPtr>("LevelCompletedComponent")
                .def(luabind::constructor<const std::string&, const std::string&, const std::string&, const std::string&, int>()),

            luabind::class_<GameOverComponent, UIComponent, ComponentPtr>("GameOverComponent")
                .def(luabind::constructor<int>()),

            luabind::class_<GoalIndicatorComponent, UIComponent, ComponentPtr>("GoalIndicatorComponent")
                .def(luabind::constructor<float, int>())
                .property("padding", &GoalIndicatorComponent::padding, &GoalIndicatorComponent::setPadding)
                .property("color", &GoalIndicatorComponent::color, &GoalIndicatorComponent::setColor)
                .property("direction", &GoalIndicatorComponent::direction, &GoalIndicatorComponent::setDirection),

            luabind::class_<SceneObject, SceneObjectPtr>("SceneObject")
                .def(luabind::constructor<>())
                .def(luabind::const_self == luabind::const_self)
                .def("scene", &SceneObject::scene)
                .property("cookie", &SceneObject::cookie)
                .property("name", &SceneObject::name, &SceneObject::setName)
                .property("parent", &SceneObject::script_parentObject)
                .def("getObjects", (std::vector<SceneObjectPtr> (SceneObject::*)() const)&SceneObject::getObjects)
                .def("getObjects", (std::vector<SceneObjectPtr> (SceneObject::*)(const std::string&) const)&SceneObject::getObjects)
                .def("addComponent", &SceneObject::addComponent)
                .def("removeComponent", &SceneObject::removeComponent)
                .def("removeFromParent", &SceneObject::removeFromParent)
                .def("removeFromParentRecursive", &SceneObject::removeFromParentRecursive)
                .property("type", &SceneObject::type, &SceneObject::setType)
                .property("material", &SceneObject::material, &SceneObject::setMaterial)
                .def("getTransform", &SceneObject::getTransform)
                .def("setTransform", (void (SceneObject::*)(const b2Transform&))&SceneObject::setTransform)
                .def("setTransform", (void (SceneObject::*)(const b2Vec2&, float))&SceneObject::setTransform)
                .def("setTransformRecursive", (void (SceneObject::*)(const b2Transform&))&SceneObject::setTransformRecursive)
                .def("setTransformRecursive", (void (SceneObject::*)(const b2Vec2&, float))&SceneObject::setTransformRecursive)
                .property("pos", &SceneObject::pos, &SceneObject::setPos)
                .def("setPosRecursive", &SceneObject::setPosRecursive)
                .def("setPosSmoothed", &SceneObject::setPosSmoothed)
                .property("angle", &SceneObject::angle, &SceneObject::setAngle)
                .def("setAngleRecursive", &SceneObject::setAngleRecursive)
                .def("setAngleSmoothed", &SceneObject::setAngleSmoothed)
                .property("worldCenter", &SceneObject::worldCenter)
                .property("localCenter", &SceneObject::localCenter)
                .property("mass", &SceneObject::mass)
                .property("inertia", &SceneObject::inertia)
                .property("ggMassOverride", &SceneObject::ggMassOverride, &SceneObject::setGGMassOverride)
                .property("ggInertiaOverride", &SceneObject::ggInertiaOverride, &SceneObject::setGGInertiaOverride)
                .property("linearVelocity", &SceneObject::linearVelocity, &SceneObject::setLinearVelocity)
                .property("angularVelocity", &SceneObject::angularVelocity, &SceneObject::setAngularVelocity)
                .property("linearDamping", &SceneObject::linearDamping, &SceneObject::setLinearDamping)
                .property("angularDamping", &SceneObject::angularDamping, &SceneObject::setAngularDamping)
                .def("applyForce", &SceneObject::applyForce)
                .def("applyForceToCenter", &SceneObject::applyForceToCenter)
                .def("applyTorque", &SceneObject::applyTorque)
                .def("applyLinearImpulse", &SceneObject::applyLinearImpulse)
                .def("applyAngularImpulse", &SceneObject::applyAngularImpulse)
                .property("bullet", &SceneObject::bullet, &SceneObject::setBullet)
                .property("active", &SceneObject::active, &SceneObject::setActive)
                .property("sleepingAllowed", &SceneObject::sleepingAllowed, &SceneObject::setSleepingAllowed)
                .property("awake", &SceneObject::awake, &SceneObject::setAwake)
                .def("resetMassData", &SceneObject::resetMassData)
                .def("setActiveRecursive", &SceneObject::setActiveRecursive)
                .def("getWorldPoint", &SceneObject::getWorldPoint)
                .def("getLocalPoint", &SceneObject::getLocalPoint)
                .def("getDirection", &SceneObject::getDirection)
                .property("life", &SceneObject::life, &SceneObject::setLife)
                .def("changeLife", &SceneObject::changeLife)
                .property("maxLife", &SceneObject::maxLife, &SceneObject::setMaxLife)
                .def("alive", &SceneObject::alive)
                .def("dead", &SceneObject::dead)
                .def("lifePercent", &SceneObject::lifePercent)
                .property("collisionFilter", &SceneObject::collisionFilter, &SceneObject::setCollisionFilter)
                .property("visible", &SceneObject::visible, &SceneObject::setVisible)
                .property("color", &SceneObject::color, &SceneObject::setColor)
                .def("setColorRecursive", &SceneObject::setColorRecursive)
                .property("glassy", &SceneObject::glassy, &SceneObject::setGlassy)
                .def("changePosSmoothed", &SceneObject::script_changePosSmoothed)
                .def("setVisibleRecursive", &SceneObject::setVisibleRecursive)
                .def("findCameraComponent", &SceneObject::findComponent<CameraComponent>)
                .def("findRenderBackgroundComponent", &SceneObject::findComponent<RenderBackgroundComponent>)
                .def("findCollisionSensorComponent", &SceneObject::findComponent<CollisionSensorComponent>)
                .def("findCollisionCancelComponent", &SceneObject::findComponent<CollisionCancelComponent>)
                .def("findCollisionDamageComponent", &SceneObject::findComponent<CollisionDamageComponent>)
                .def("findShroomGuardianComponent", &SceneObject::findComponent<ShroomGuardianComponent>)
                .def("findPathComponent", &SceneObject::findComponent<PathComponent>)
                .def("findPhysicsBodyComponent", &SceneObject::findComponent<PhysicsBodyComponent>)
                .def("findPhysicsJointComponent", &SceneObject::findComponent<PhysicsJointComponent>)
                .def("findTetrobotComponent", &SceneObject::findComponent<TetrobotComponent>)
                .def("findTetrocrabComponent", &SceneObject::findComponent<TetrocrabComponent>)
                .def("findLightComponent", &SceneObject::findComponent<LightComponent>)
                .def("findRenderQuadComponent", &SceneObject::findComponent<RenderQuadComponent>)
                .def("findRenderQuadComponents", (std::vector<RenderQuadComponentPtr> (SceneObject::*)(const std::string&) const)&SceneObject::findComponents<RenderQuadComponent>)
                .def("findRenderQuadComponents", (std::vector<RenderQuadComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<RenderQuadComponent>)
                .def("findRenderStripeComponents", (std::vector<RenderStripeComponentPtr> (SceneObject::*)(const std::string&) const)&SceneObject::findComponents<RenderStripeComponent>)
                .def("findRenderStripeComponents", (std::vector<RenderStripeComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<RenderStripeComponent>)
                .def("findRenderTerrainComponents", (std::vector<RenderTerrainComponentPtr> (SceneObject::*)(const std::string&) const)&SceneObject::findComponents<RenderTerrainComponent>)
                .def("findRenderTerrainComponents", (std::vector<RenderTerrainComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<RenderTerrainComponent>)
                .def("findRenderProjComponent", &SceneObject::findComponent<RenderProjComponent>)
                .def("findRenderProjComponents", (std::vector<RenderProjComponentPtr> (SceneObject::*)(const std::string&) const)&SceneObject::findComponents<RenderProjComponent>)
                .def("findRenderProjComponents", (std::vector<RenderProjComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<RenderProjComponent>)
                .def("findPlayerComponent", &SceneObject::findComponent<PlayerComponent>)
                .def("findTargetableComponent", &SceneObject::findComponent<TargetableComponent>)
                .def("findSpawnerComponent", &SceneObject::findComponent<SpawnerComponent>)
                .def("findAnimationComponent", &SceneObject::findComponent<AnimationComponent>)
                .def("findAnimationComponents", (std::vector<AnimationComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<AnimationComponent>)
                .def("findScorpComponent", &SceneObject::findComponent<ScorpComponent>)
                .def("findTeleportComponent", &SceneObject::findComponent<TeleportComponent>)
                .def("findGoalAreaComponent", &SceneObject::findComponent<GoalAreaComponent>)
                .def("findPuzzleHintAreaComponent", &SceneObject::findComponent<PuzzleHintAreaComponent>)
                .def("findWeaponComponent", &SceneObject::findComponent<WeaponComponent>)
                .def("findWeaponFlamethrowerComponent", &SceneObject::findComponent<WeaponFlamethrowerComponent>)
                .def("findWeaponFireballComponent", &SceneObject::findComponent<WeaponFireballComponent>)
                .def("findWeaponPistolComponent", &SceneObject::findComponent<WeaponPistolComponent>)
                .def("findWeaponRLauncherComponent", &SceneObject::findComponent<WeaponRLauncherComponent>)
                .def("findWeaponLGunComponent", &SceneObject::findComponent<WeaponLGunComponent>)
                .def("findWeaponPlasmaGunComponent", &SceneObject::findComponent<WeaponPlasmaGunComponent>)
                .def("findPyrobotComponent", &SceneObject::findComponent<PyrobotComponent>)
                .def("findMeasurementComponent", &SceneObject::findComponent<MeasurementComponent>)
                .def("findPlaceholderComponent", &SceneObject::findComponent<PlaceholderComponent>)
                .def("findSpiderComponent", &SceneObject::findComponent<SpiderComponent>)
                .def("findExplosionComponent", &SceneObject::findComponent<ExplosionComponent>)
                .def("findBossSkullComponent", &SceneObject::findComponent<BossSkullComponent>)
                .def("findBossQueenComponent", &SceneObject::findComponent<BossQueenComponent>)
                .def("findSentryComponent", &SceneObject::findComponent<SentryComponent>)
                .def("findEnforcerComponent", &SceneObject::findComponent<EnforcerComponent>)
                .def("findRoboArmComponent", &SceneObject::findComponent<RoboArmComponent>)
                .def("findInvulnerabilityComponent", &SceneObject::findComponent<InvulnerabilityComponent>)
                .def("findKeeperComponent", &SceneObject::findComponent<KeeperComponent>)
                .def("findDummyComponents", (std::vector<DummyComponentPtr> (SceneObject::*)(const std::string&) const)&SceneObject::findComponents<DummyComponent>)
                .def("findDummyComponent", &SceneObject::findComponentByName<DummyComponent>)
                .def("findParticleEffectComponent", &SceneObject::findComponent<ParticleEffectComponent>)
                .def("findBossCoreProtectorComponent", &SceneObject::findComponent<BossCoreProtectorComponent>)
                .def("findRenderHealthbarComponent", &SceneObject::findComponent<RenderHealthbarComponent>)
                .def("findRenderTentacleComponent", &SceneObject::findComponent<RenderTentacleComponent>)
                .def("findTentaclePulseComponent", &SceneObject::findComponent<TentaclePulseComponent>)
                .def("findTentacleSwingComponent", &SceneObject::findComponent<TentacleSwingComponent>)
                .def("findTentacleAttractComponent", &SceneObject::findComponent<TentacleAttractComponent>)
                .def("findBoatComponent", &SceneObject::findComponent<BoatComponent>)
                .def("findBossSquidComponent", &SceneObject::findComponent<BossSquidComponent>)
                .def("findMechComponent", &SceneObject::findComponent<MechComponent>)
                .def("findBeetleComponent", &SceneObject::findComponent<BeetleComponent>)
                .def("findBabySpiderComponent", &SceneObject::findComponent<BabySpiderComponent>)
                .def("findCentipedeComponent", &SceneObject::findComponent<CentipedeComponent>)
                .def("findGovernedTurretComponent", &SceneObject::findComponent<GovernedTurretComponent>)
                .def("findBossChopperComponent", &SceneObject::findComponent<BossChopperComponent>)
                .def("findBossBeholderComponent", &SceneObject::findComponent<BossBeholderComponent>)
                .def("findCreatureComponent", &SceneObject::findComponent<CreatureComponent>)
                .def("findFloatComponents", (std::vector<FloatComponentPtr> (SceneObject::*)() const)&SceneObject::findComponents<FloatComponent>)
                .def("findWalkerComponent", &SceneObject::findComponent<WalkerComponent>)
                .def("findSnakeComponent", &SceneObject::findComponent<SnakeComponent>)
                .def("findCountdownComponent", &SceneObject::findComponent<CountdownComponent>)
                .def("findWeaponRopeComponent", &SceneObject::findComponent<WeaponRopeComponent>)
                .def("findBossBuddyComponent", &SceneObject::findComponent<BossBuddyComponent>)
                .def("findQuadPulseComponent", &SceneObject::findComponent<QuadPulseComponent>)
                .def("findWeaponAcidballComponent", &SceneObject::findComponent<WeaponAcidballComponent>)
                .def("findBossNatanComponent", &SceneObject::findComponent<BossNatanComponent>)
                .def("findPowerBeamComponent", &SceneObject::findComponent<PowerBeamComponent>)
                .def("findWeaponHeaterComponent", &SceneObject::findComponent<WeaponHeaterComponent>)
                .def("findGorgerComponent", &SceneObject::findComponent<GorgerComponent>)
                .def("findRenderSaturationComponent", &SceneObject::findComponent<RenderSaturationComponent>)
                .def("findRenderTextComponent", &SceneObject::findComponent<RenderTextComponent>)
                .def("findDudeComponent", &SceneObject::findComponent<DudeComponent>)
                .property("bodyType", &SceneObject::script_bodyType, &SceneObject::script_setBodyType)
                .property("gravityGunAware", &SceneObject::gravityGunAware, &SceneObject::setGravityGunAware)
                .property("invulnerable", &SceneObject::invulnerable, &SceneObject::setInvulnerable)
                .property("freezable", &SceneObject::freezable, &SceneObject::setFreezable)
                .property("freezeRadius", &SceneObject::freezeRadius, &SceneObject::setFreezeRadius)
                .property("freezePhysics", &SceneObject::freezePhysics, &SceneObject::setFreezePhysics)
                .property("roamBehavior", &SceneObject::roamBehavior)
                .property("seekBehavior", &SceneObject::seekBehavior)
                .property("interceptBehavior", &SceneObject::interceptBehavior)
                .property("collisionImpulseMultiplier", &SceneObject::collisionImpulseMultiplier, &SceneObject::setCollisionImpulseMultiplier)
                .property("activeDeadbody", &SceneObject::activeDeadbody, &SceneObject::setActiveDeadbody)
                .property("deadbodyAware", &SceneObject::deadbodyAware, &SceneObject::setDeadbodyAware),

            luabind::class_<SensorListener, ScriptSensorListener, SensorListenerPtr>("SensorListener")
                .def(luabind::const_self == luabind::const_self)
                .def(luabind::constructor<luabind::object>()),

            luabind::class_<JointProxy, JointProxyPtr>("JointProxy")
                .def(luabind::const_self == luabind::const_self)
                .def("valid", &JointProxy::valid)
                .property("name", &JointProxy::name)
                .def("remove", &JointProxy::remove)
                .def("clone", &JointProxy::clone)
                .property("objectA", &JointProxy::script_objectA)
                .property("objectB", &JointProxy::script_objectB)
                .def("getAnchorA", &JointProxy::getAnchorA)
                .def("getAnchorB", &JointProxy::getAnchorB)
                .def("getReactionForce", &JointProxy::getReactionForce)
                .def("getReactionTorque", &JointProxy::getReactionTorque)
                .def("active", &JointProxy::active)
                .def("collideConnected", &JointProxy::collideConnected),

            luabind::class_<RevoluteJointProxy, JointProxy, JointProxyPtr>("RevoluteJointProxy")
                .property("localAnchorA", &RevoluteJointProxy::localAnchorA)
                .property("localAnchorB", &RevoluteJointProxy::localAnchorB)
                .property("referenceAngle", &RevoluteJointProxy::referenceAngle)
                .property("jointAngle", &RevoluteJointProxy::jointAngle)
                .property("jointSpeed", &RevoluteJointProxy::jointSpeed)
                .property("limitEnabled", &RevoluteJointProxy::limitEnabled, &RevoluteJointProxy::enableLimit)
                .property("lowerLimit", &RevoluteJointProxy::lowerLimit)
                .property("upperLimit", &RevoluteJointProxy::upperLimit)
                .def("setLimits", &RevoluteJointProxy::setLimits)
                .property("motorEnabled", &RevoluteJointProxy::motorEnabled, &RevoluteJointProxy::enableMotor)
                .property("motorSpeed", &RevoluteJointProxy::motorSpeed, &RevoluteJointProxy::setMotorSpeed)
                .property("maxMotorTorque", &RevoluteJointProxy::maxMotorTorque, &RevoluteJointProxy::setMaxMotorTorque)
                .def("getMotorTorque", &RevoluteJointProxy::getMotorTorque),

            luabind::class_<PrismaticJointProxy, JointProxy, JointProxyPtr>("PrismaticJointProxy")
                .property("localAnchorA", &PrismaticJointProxy::localAnchorA)
                .property("localAnchorB", &PrismaticJointProxy::localAnchorB)
                .property("localAxisA", &PrismaticJointProxy::localAxisA)
                .property("referenceAngle", &PrismaticJointProxy::referenceAngle)
                .def("getJointTranslation", &PrismaticJointProxy::getJointTranslation)
                .def("getJointSpeed", &PrismaticJointProxy::getJointSpeed)
                .property("limitEnabled", &PrismaticJointProxy::limitEnabled, &PrismaticJointProxy::enableLimit)
                .property("lowerLimit", &PrismaticJointProxy::lowerLimit)
                .property("upperLimit", &PrismaticJointProxy::upperLimit)
                .def("setLimits", &PrismaticJointProxy::setLimits)
                .property("motorEnabled", &PrismaticJointProxy::motorEnabled, &PrismaticJointProxy::enableMotor)
                .property("motorSpeed", &PrismaticJointProxy::motorSpeed, &PrismaticJointProxy::setMotorSpeed)
                .property("maxMotorForce", &PrismaticJointProxy::maxMotorForce, &PrismaticJointProxy::setMaxMotorForce)
                .def("getMotorForce", &PrismaticJointProxy::getMotorForce),

            luabind::class_<DistanceJointProxy, JointProxy, JointProxyPtr>("DistanceJointProxy")
                .property("localAnchorA", &DistanceJointProxy::localAnchorA)
                .property("localAnchorB", &DistanceJointProxy::localAnchorB)
                .property("length", &DistanceJointProxy::length, &DistanceJointProxy::setLength)
                .property("frequency", &DistanceJointProxy::frequency, &DistanceJointProxy::setFrequency)
                .property("dampingRatio", &DistanceJointProxy::dampingRatio, &DistanceJointProxy::setDampingRatio),

            luabind::class_<PulleyJointProxy, JointProxy, JointProxyPtr>("PulleyJointProxy")
                .property("groundAnchorA", &PulleyJointProxy::groundAnchorA)
                .property("groundAnchorB", &PulleyJointProxy::groundAnchorB)
                .property("lengthA", &PulleyJointProxy::lengthA)
                .property("lengthB", &PulleyJointProxy::lengthB)
                .property("ratio", &PulleyJointProxy::ratio)
                .def("getCurrentLengthA", &PulleyJointProxy::getCurrentLengthA)
                .def("getCurrentLengthB", &PulleyJointProxy::getCurrentLengthB),

            luabind::class_<WheelJointProxy, JointProxy, JointProxyPtr>("WheelJointProxy")
                .property("localAnchorA", &WheelJointProxy::localAnchorA)
                .property("localAnchorB", &WheelJointProxy::localAnchorB)
                .property("localAxisA", &WheelJointProxy::localAxisA)
                .def("getJointTranslation", &WheelJointProxy::getJointTranslation)
                .def("getJointSpeed", &WheelJointProxy::getJointSpeed)
                .property("motorEnabled", &WheelJointProxy::motorEnabled, &WheelJointProxy::enableMotor)
                .property("motorSpeed", &WheelJointProxy::motorSpeed, &WheelJointProxy::setMotorSpeed)
                .property("maxMotorTorque", &WheelJointProxy::maxMotorTorque, &WheelJointProxy::setMaxMotorTorque)
                .def("getMotorTorque", &WheelJointProxy::getMotorTorque)
                .property("springFrequencyHz", &WheelJointProxy::springFrequencyHz, &WheelJointProxy::setSpringFrequencyHz)
                .property("springDampingRatio", &WheelJointProxy::springDampingRatio, &WheelJointProxy::setSpringDampingRatio),

            luabind::class_<MotorJointProxy, JointProxy, JointProxyPtr>("MotorJointProxy")
                .property("linearOffset", &MotorJointProxy::linearOffset, &MotorJointProxy::setLinearOffset)
                .property("angularOffset", &MotorJointProxy::angularOffset, &MotorJointProxy::setAngularOffset)
                .property("maxForce", &MotorJointProxy::maxForce, &MotorJointProxy::setMaxForce)
                .property("maxTorque", &MotorJointProxy::maxTorque, &MotorJointProxy::setMaxTorque)
                .property("correctionFactor", &MotorJointProxy::correctionFactor, &MotorJointProxy::setCorrectionFactor),

            luabind::class_<WeldJointProxy, JointProxy, JointProxyPtr>("WeldJointProxy")
                .property("localAnchorA", &WeldJointProxy::localAnchorA)
                .property("localAnchorB", &WeldJointProxy::localAnchorB)
                .property("referenceAngle", &WeldJointProxy::referenceAngle)
                .property("frequency", &WeldJointProxy::frequency, &WeldJointProxy::setFrequency)
                .property("dampingRatio", &WeldJointProxy::dampingRatio, &WeldJointProxy::setDampingRatio),

            luabind::class_<FrictionJointProxy, JointProxy, JointProxyPtr>("FrictionJointProxy")
                .property("localAnchorA", &FrictionJointProxy::localAnchorA)
                .property("localAnchorB", &FrictionJointProxy::localAnchorB)
                .property("maxForce", &FrictionJointProxy::maxForce, &FrictionJointProxy::setMaxForce)
                .property("maxTorque", &FrictionJointProxy::maxTorque, &FrictionJointProxy::setMaxTorque),

            luabind::class_<RopeJointProxy, JointProxy, JointProxyPtr>("RopeJointProxy")
                .property("localAnchorA", &RopeJointProxy::localAnchorA)
                .property("localAnchorB", &RopeJointProxy::localAnchorB)
                .property("maxLength", &RopeJointProxy::maxLength, &RopeJointProxy::setMaxLength)
                .property("limitState", &RopeJointProxy::limitState),

            luabind::class_<GearJointProxy, JointProxy, JointProxyPtr>("GearJointProxy")
                .property("joint1", &GearJointProxy::joint1)
                .property("joint2", &GearJointProxy::joint2)
                .property("ratio", &GearJointProxy::ratio, &GearJointProxy::setRatio),

            luabind::class_<PathIterator, PathIteratorPtr>("PathIterator")
                .def("clone", &PathIterator::clone)
                .property("current", &PathIterator::current)
                .def("less", &PathIterator::less)
                .def("eq", &PathIterator::eq)
                .def("lessEq", &PathIterator::lessEq)
                .def("gt", &PathIterator::gt)
                .def("gtEq", &PathIterator::gtEq)
                .def("step", &PathIterator::step)
                .property("loop", &PathIterator::loop, &PathIterator::setLoop),

            luabind::class_<Path, PathPtr>("Path")
                .property("points", &Path::points)
                .property("length", &Path::length)
                .def("first", &Path::first)
                .def("last", &Path::last)
                .def("find", &Path::find)
                .def("dend", &Path::end)
                .def("rend", &Path::rend)
                .def("add", (void (Path::*)(const b2Vec2&))&Path::add)
                .def("addFront", (void (Path::*)(const b2Vec2&))&Path::addFront),

            luabind::class_<HermitePath, Path, PathPtr>("HermitePath")
                .def(luabind::constructor<float>())
        ];
#endif
        bind2();
    }

    int Script::Impl::loadPackage(lua_State* L)
    {
        Impl* impl = reinterpret_cast<Impl*>(lua_touserdata(L, lua_upvalueindex(1)));

        (void)impl;

        const char *name = luaL_checkstring(L, 1);

        std::string path = std::string("modules/") + name + ".lua";

        std::string str;

        boost::shared_ptr<PlatformIFStream> is =
            boost::make_shared<PlatformIFStream>(path);

        if (!*is) {
            std::string path2 = std::string(name) + ".lua";

            is = boost::make_shared<PlatformIFStream>(path2);

            if (!*is) {
                luaL_error(L, "Cannot open file \"%s\" or \"%s\"",
                    path.c_str(), path2.c_str());
                return 1;
            }

            path = path2;
        }

        assetManager.assetTouched(path);

        if (!readStream(*is, str)) {
            luaL_error(L, "Error reading file \"%s\"", path.c_str());
            return 1;
        }

        if (::luaL_loadbuffer(L, &str[0], str.size(), path.c_str()) != 0) {
            const char* errText = lua_tostring(L, -1);

            std::string text;

            if (errText) {
                text = errText;
            } else {
                text = std::string("Cannot load lua chunk from file \"") +
                                    path + "\"";
            }

            lua_pop(L, 1);

            luaL_error(L, "%s", text.c_str());
        }

        return 1;
    }

    void Script::Impl::setPackageLoaders()
    {
        {
            luabind::object loaders = luabind::newtable(L_);

            luabind::globals(L_)["package"]["loaders"] = loaders;
        }

        lua_getfield(L_, LUA_GLOBALSINDEX, "package");
        lua_getfield(L_, -1, "loaders");
        lua_remove(L_, -2);

        lua_pushinteger(L_, 1);
        lua_pushlightuserdata(L_, this);
        lua_pushcclosure(L_, &Impl::loadPackage, 1);
        lua_rawset(L_, -3);
        lua_pop(L_, 1);
    }

    void Script::Impl::require(const std::string& name)
    {
        luabind::call_function<void>(L_, "require", name);
    }

    void Script::Impl::createGlobals()
    {
        luabind::globals(L_)["scene"] = scene_;
        luabind::globals(L_)["factory"] = &sceneObjectFactory;
        luabind::globals(L_)["settings"] = &settings;
        luabind::globals(L_)["audio"] = &audio;
        luabind::globals(L_)["platform"] = platform.get();
        luabind::globals(L_)["input"] = &inputManager;
        luabind::globals(L_)["userData"] = &userData;
        luabind::globals(L_)["upgradeManager"] = &upgradeManager;
        luabind::globals(L_)["gameShell"] = gameShell.get();
    }

    void Script::Impl::loadFile()
    {
        assetManager.assetTouched(path_);

        std::string str;

        PlatformIFStream is(path_);

        if (!is) {
            throw std::runtime_error("Cannot open file \"" + path_ + "\"");
        }

        if (!readStream(is, str)) {
            throw std::runtime_error("Error reading file \"" + path_ + "\"");
        }

        if (::luaL_loadbuffer(L_, &str[0], str.size(), path_.c_str()) != 0) {
            const char* errText = ::lua_tostring(L_, -1);

            std::string text;

            if (errText) {
                text = errText;
            } else {
                text = std::string("Cannot load lua chunk from file \"") +
                                    path_ + "\"";
            }

            ::lua_pop(L_, 1);

            throw std::runtime_error(text);
        }
    }

    int Script::Impl::print(lua_State* L)
    {
        Impl* impl = reinterpret_cast<Impl*>(lua_touserdata(L, lua_upvalueindex(1)));

        log4cplus::NDCContextCreator ndc(impl->path_);

        std::ostringstream os;

        int n = lua_gettop(L);
        int i;
        lua_getglobal(L, "tostring");
        for (i = 1; i <= n; ++i) {
            const char* s;
            lua_pushvalue(L, -1);
            lua_pushvalue(L, i);
            lua_call(L, 1, 1);
            s = lua_tostring(L, -1);
            if (s == NULL) {
                return luaL_error(L, LUA_QL("tostring") " must return a string to "
                    LUA_QL("print"));
            }
            if (i > 1) {
                os << "\t";
            }
            os << s;
            lua_pop(L, 1);
        }

        LOG4CPLUS_DEBUG(logger(), os.str());

        return 0;
    }

    Script::Script(const std::string& path,
                   Scene* scene)
    : impl_(new Impl(path, scene))
    {
    }

    Script::~Script()
    {
        delete impl_;
    }

    bool Script::init()
    {
        impl_->L_ = ::lua_open();

        if (!impl_->L_) {
            LOG4CPLUS_ERROR(logger(), "Unable to create lua_State");

            return false;
        }

        luaL_openlibs(impl_->L_);

        try
        {
            luabind::open(impl_->L_);

            luabind::set_pcall_callback(&errorHandler);

            impl_->bind();

            impl_->setPackageLoaders();

            impl_->require("strict");
            impl_->require("vec2");
            impl_->require("rot");
            impl_->require("transform");
            impl_->require("utils");
            impl_->require("ui_utils");

            Language language = gameShell->getLanguage();

            if (settings.language != LanguageEnglish) {
                language = settings.language;
            }

            switch (language) {
            case LanguageRussian:
                impl_->require("tr_ru");
                break;
            default:
                assert(0);
            case LanguageEnglish:
                impl_->require("tr");
                break;
            }

            impl_->createGlobals();

            lua_pushlightuserdata(impl_->L_, impl_);
            lua_pushcclosure(impl_->L_, &Impl::print, 1);
            lua_setglobal(impl_->L_, "print");
        }
        catch (const luabind::error& e)
        {
            ::lua_pop(e.state(), 1);

            return false;
        }
        catch (const std::exception& e)
        {
            LOG4CPLUS_ERROR(logger(), e.what());

            return false;
        }

        return true;
    }

    bool Script::run()
    {
        try {
            impl_->require("startup");

            impl_->loadFile();
        } catch (const luabind::error& e) {
            ::lua_pop(e.state(), 1);

            return false;
        } catch (const std::exception& e) {
            LOG4CPLUS_ERROR(logger(), e.what());

            return false;
        }

        int base = ::lua_gettop(impl_->L_);

        ::lua_pushcfunction(impl_->L_, &errorHandler);
        ::lua_insert(impl_->L_, base);

        int result = ::lua_pcall(impl_->L_, 0, 0, base);

        ::lua_remove(impl_->L_, base);

        if (result != 0) {
            ::lua_pop(impl_->L_, 1);
        }

        return (result == 0);
    }

    void Script::finalize()
    {
        luabind::object obj = luabind::globals(impl_->L_)["finalizer"];

        if (!obj || (luabind::type(obj) != LUA_TFUNCTION)) {
            return;
        }

        try {
            luabind::call_function<void>(impl_->L_, "finalizer");
        } catch (const luabind::error& e) {
            ::lua_pop(e.state(), 1);
        } catch (const std::exception& e) {
            LOG4CPLUS_ERROR(logger(), e.what());
        }
    }

    struct lua_State* Script::state()
    {
        return impl_->L_;
    }
}
