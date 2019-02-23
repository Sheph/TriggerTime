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
    void Script::Impl::bind2()
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
            luabind::class_<Tweening, TweeningPtr>("Tweening")
                .property("duration", &Tweening::duration)
                .def("getValue", &Tweening::getValue)
                .def("finished", &Tweening::finished)
                .property("loop", &Tweening::loop, &Tweening::setLoop),

            luabind::class_<SingleTweening, Tweening, TweeningPtr>("SingleTweening")
                .def(luabind::constructor<float, Easing, bool>())
                .def(luabind::constructor<float, Easing, float, float, bool>())
                .def("start", &SingleTweening::start)
                .def("tend", &SingleTweening::end),

            luabind::class_<SequentialTweening, Tweening, TweeningPtr>("SequentialTweening")
                .def(luabind::constructor<bool>())
                .def("addTweening", &SequentialTweening::addTweening),

            luabind::class_<PathComponent, PhasedComponent, ComponentPtr>("PathComponent")
                .property("path", &PathComponent::path, &PathComponent::setPath),

            luabind::class_<DummyComponent, PhasedComponent, ComponentPtr>("DummyComponent")
                .property("pos", &DummyComponent::pos)
                .property("angle", &DummyComponent::angle)
                .property("transform", &DummyComponent::transform)
                .property("zOrder", &DummyComponent::zOrder)
                .property("width", &DummyComponent::width)
                .property("length", &DummyComponent::length)
                .property("worldPos", &DummyComponent::worldPos)
                .property("worldTransform", &DummyComponent::worldTransform),

            luabind::class_<FollowPathComponent, PhasedComponent, ComponentPtr>("FollowPathComponent")
                .def(luabind::constructor<>())
                .property("finished", &FollowPathComponent::finished)
                .property("time", &FollowPathComponent::time, &FollowPathComponent::setTime)
                .property("pathTransform", &FollowPathComponent::pathTransform, &FollowPathComponent::setPathTransform)
                .property("path", &FollowPathComponent::path, &FollowPathComponent::setPath)
                .property("tweening", &FollowPathComponent::tweening, &FollowPathComponent::setTweening),

            luabind::class_<BehaviorComponent, PhasedComponent, ComponentPtr>("BehaviorComponent")
                .def("start", &BehaviorComponent::start)
                .def("reset", &BehaviorComponent::reset)
                .property("finished", &BehaviorComponent::finished)
                .property("loop", &BehaviorComponent::loop, &BehaviorComponent::setLoop),

            luabind::class_<BehaviorRoamComponent, BehaviorComponent, ComponentPtr>("BehaviorRoamComponent")
                .def("changePath", &BehaviorRoamComponent::changePath)
                .def("damp", &BehaviorRoamComponent::damp)
                .property("linearVelocity", &BehaviorRoamComponent::linearVelocity, &BehaviorRoamComponent::setLinearVelocity)
                .property("linearDamping", &BehaviorRoamComponent::linearDamping, &BehaviorRoamComponent::setLinearDamping)
                .property("dampDistance", &BehaviorRoamComponent::dampDistance, &BehaviorRoamComponent::setDampDistance)
                .property("changeAngle", &BehaviorRoamComponent::changeAngle, &BehaviorRoamComponent::setChangeAngle)
                .property("angleOffset", &BehaviorRoamComponent::angleOffset, &BehaviorRoamComponent::setAngleOffset)
                .property("paused", &BehaviorRoamComponent::paused, &BehaviorRoamComponent::setPaused),

            luabind::class_<BehaviorSeekComponent, BehaviorComponent, ComponentPtr>("BehaviorSeekComponent")
                .property("angularVelocity", &BehaviorSeekComponent::angularVelocity, &BehaviorSeekComponent::setAngularVelocity)
                .property("target", &BehaviorSeekComponent::target, &BehaviorSeekComponent::setTarget)
                .property("useTorque", &BehaviorSeekComponent::useTorque, &BehaviorSeekComponent::setUseTorque),

            luabind::class_<BehaviorInterceptComponent, BehaviorComponent, ComponentPtr>("BehaviorInterceptComponent")
                .property("angularVelocity", &BehaviorInterceptComponent::angularVelocity, &BehaviorInterceptComponent::setAngularVelocity)
                .property("parentLinearVelocity", &BehaviorInterceptComponent::parentLinearVelocity, &BehaviorInterceptComponent::setParentLinearVelocity)
                .property("target", &BehaviorInterceptComponent::target, &BehaviorInterceptComponent::setTarget),

            luabind::class_<CollisionFilter, CollisionFilterPtr>("CollisionFilter"),

            luabind::class_<CollisionCookieFilter, CollisionFilter, CollisionFilterPtr>("CollisionCookieFilter")
                .def(luabind::constructor<>())
                .def("add", &CollisionCookieFilter::add),

            luabind::class_<PhysicsBodyComponent, PhysicsComponent, ComponentPtr>("PhysicsBodyComponent")
                .def("computeAABB", &PhysicsBodyComponent::computeAABB)
                .def("enableFixture", &PhysicsBodyComponent::enableFixture)
                .def("disableFixture", &PhysicsBodyComponent::disableFixture)
                .def("enableAllFixtures", &PhysicsBodyComponent::enableAllFixtures)
                .def("disableAllFixtures", &PhysicsBodyComponent::disableAllFixtures)
                .def("setFilterGroupIndex", &PhysicsBodyComponent::setFilterGroupIndex)
                .def("setFilterCategoryBits", &PhysicsBodyComponent::setFilterCategoryBits)
                .property("filterMaskBits", &PhysicsBodyComponent::filterMaskBits, &PhysicsBodyComponent::setFilterMaskBits)
                .property("density", &PhysicsBodyComponent::density, &PhysicsBodyComponent::setDensity)
                .property("filterGroupIndex", &PhysicsBodyComponent::filterGroupIndex, &PhysicsBodyComponent::setFilterGroupIndex),

            luabind::class_<PhysicsJointComponent, PhysicsComponent, ComponentPtr>("PhysicsJointComponent")
                .def("getJoints", (std::vector<JointProxyPtr> (PhysicsJointComponent::*)() const)&PhysicsJointComponent::script_getJoints),

            luabind::class_<TetrobotComponent, TargetableComponent, ComponentPtr>("TetrobotComponent"),

            luabind::class_<TetrocrabComponent, TargetableComponent, ComponentPtr>("TetrocrabComponent"),

            luabind::class_<SpiderComponent, TargetableComponent, ComponentPtr>("SpiderComponent")
                .def("rotate", &SpiderComponent::rotate)
                .def("angry", &SpiderComponent::angry)
                .def("trapped", &SpiderComponent::trapped)
                .property("legVelocity", &SpiderComponent::legVelocity, &SpiderComponent::setLegVelocity)
                .property("patrol", &SpiderComponent::patrol, &SpiderComponent::setPatrol)
                .property("canRotate", &SpiderComponent::canRotate, &SpiderComponent::setCanRotate),

            luabind::class_<BossSkullComponent, TargetableComponent, ComponentPtr>("BossSkullComponent")
                .def("setAmplified", &BossSkullComponent::setAmplified),

            luabind::class_<BossQueenComponent, TargetableComponent, ComponentPtr>("BossQueenComponent")
                .def("trapped", &BossQueenComponent::trapped)
                .def("angry", &BossQueenComponent::angry)
                .property("deathStarted", &BossQueenComponent::deathStarted)
                .property("deathFinished", &BossQueenComponent::deathFinished),

            luabind::class_<BossCoreProtectorComponent, TargetableComponent, ComponentPtr>("BossCoreProtectorComponent")
                .def("setArena", &BossCoreProtectorComponent::setArena)
                .def("startAngry", &BossCoreProtectorComponent::startAngry)
                .def("setDie", &BossCoreProtectorComponent::setDie),

            luabind::class_<BossSquidComponent, TargetableComponent, ComponentPtr>("BossSquidComponent")
                .property("deathFinished", &BossSquidComponent::deathFinished),

            luabind::class_<BossChopperComponent, TargetableComponent, ComponentPtr>("BossChopperComponent")
                .property("patrol", &BossChopperComponent::patrol, &BossChopperComponent::setPatrol)
                .property("canDie", &BossChopperComponent::canDie, &BossChopperComponent::setCanDie),

            luabind::class_<BossBeholderComponent, TargetableComponent, ComponentPtr>("BossBeholderComponent")
                .property("patrol", &BossBeholderComponent::patrol, &BossBeholderComponent::setPatrol)
                .property("deathFinished", &BossBeholderComponent::deathFinished),

            luabind::class_<GorgerComponent, TargetableComponent, ComponentPtr>("GorgerComponent")
                .def("setWalk", &GorgerComponent::setWalk),

            luabind::class_<RenderLightComponent, RenderComponent, ComponentPtr>("RenderLightComponent")
                .property("ambientLight", &RenderLightComponent::ambientLight, &RenderLightComponent::setAmbientLight)
                .property("numBlur", &RenderLightComponent::numBlur, &RenderLightComponent::setNumBlur)
                .property("blur", &RenderLightComponent::blur)
                .property("gammaCorrection", &RenderLightComponent::gammaCorrection, &RenderLightComponent::setGammaCorrection)
                .def("addLight", &RenderLightComponent::addLight)
                .def("removeLight", &RenderLightComponent::removeLight),

            luabind::class_<Light, LightPtr>("Light")
                .def(luabind::const_self == luabind::const_self)
                .property("visible", &Light::visible, &Light::setVisible)
                .property("color", &Light::color, &Light::setColor)
                .property("nearOffset", &Light::nearOffset, &Light::setNearOffset)
                .property("intensity", &Light::intensity, &Light::setIntensity)
                .property("diffuse", &Light::diffuse, &Light::setDiffuse)
                .property("pos", &Light::pos, &Light::setPos)
                .property("angle", &Light::angle, &Light::setAngle)
                .property("worldTransform", &Light::worldTransform, &Light::setWorldTransform)
                .property("numRays", &Light::numRays, &Light::setNumRays)
                .property("xray", &Light::xray, &Light::setXray)
                .property("dynamic", &Light::dynamic, &Light::setDynamic)
                .def("setDirty", &Light::setDirty)
                .def("remove", &Light::remove),

            luabind::class_<LightComponent, PhasedComponent, ComponentPtr>("LightComponent")
                .def("setDirty", &LightComponent::setDirty)
                .def("getLights", &LightComponent::getLights)
                .def("getLights", &LightComponent::lights),

            luabind::class_<RenderQuadComponent, RenderComponent, ComponentPtr>("RenderQuadComponent")
                .property("drawable", &RenderQuadComponent::drawable)
                .property("height", &RenderQuadComponent::height, &RenderQuadComponent::script_setHeight)
                .property("pos", &RenderQuadComponent::pos, &RenderQuadComponent::setPos)
                .property("angle", &RenderQuadComponent::angle, &RenderQuadComponent::setAngle)
                .property("fixedPos", &RenderQuadComponent::fixedPos, &RenderQuadComponent::setFixedPos)
                .def("setSizes", &RenderQuadComponent::setSizes),

            luabind::class_<RenderProjComponent, RenderComponent, ComponentPtr>("RenderProjComponent")
                .property("drawable", &RenderProjComponent::drawable),

            luabind::class_<RenderStripeComponent, RenderComponent, ComponentPtr>("RenderStripeComponent")
                .property("drawable", &RenderStripeComponent::drawable),

            luabind::class_<RenderTerrainComponent, RenderComponent, ComponentPtr>("RenderTerrainComponent")
                .property("drawable", &RenderTerrainComponent::drawable),

            luabind::class_<RenderHealthbarComponent, RenderComponent, ComponentPtr>("RenderHealthbarComponent")
                .def(luabind::constructor<const b2Vec2&, float, float, float, int>()),

            luabind::class_<RenderTentacleComponent, RenderComponent, ComponentPtr>("RenderTentacleComponent")
                .property("flip", &RenderTentacleComponent::flip, &RenderTentacleComponent::setFlip)
                .property("timeline1Size", &RenderTentacleComponent::timeline1Size)
                .property("timeline2Size", &RenderTentacleComponent::timeline2Size)
                .property("objects", &RenderTentacleComponent::objects)
                .def("resetTimeline1", &RenderTentacleComponent::resetTimeline1)
                .def("resetTimeline2", &RenderTentacleComponent::resetTimeline2)
                .def("time1At", &RenderTentacleComponent::time1At)
                .def("width1At", &RenderTentacleComponent::width1At)
                .def("set1At", &RenderTentacleComponent::set1At)
                .def("time2At", &RenderTentacleComponent::time2At)
                .def("width2At", &RenderTentacleComponent::width2At)
                .def("set2At", &RenderTentacleComponent::set2At),

            luabind::class_<RenderSaturationComponent, RenderComponent, ComponentPtr>("RenderSaturationComponent")
                .def(luabind::constructor<float, int>()),

            luabind::class_<ParticleEffectComponent, RenderComponent, ComponentPtr>("ParticleEffectComponent")
                .def("resetEmit", &ParticleEffectComponent::resetEmit)
                .def("allowCompletion", &ParticleEffectComponent::allowCompletion),

            luabind::class_<Drawable, DrawablePtr>("Drawable")
                .def("setImage", (void (Drawable::*)(const std::string&))&Drawable::script_setImage)
                .def("setImage", (void (Drawable::*)(const std::string&, Texture::WrapMode, Texture::WrapMode))&Drawable::script_setImage),

            luabind::class_<FadeOutComponent, PhasedComponent, ComponentPtr>("FadeOutComponent")
                .def(luabind::constructor<float>()),

            luabind::class_<InvulnerabilityComponent, PhasedComponent, ComponentPtr>("InvulnerabilityComponent")
                .property("duration", &InvulnerabilityComponent::duration, &InvulnerabilityComponent::setDuration),

            luabind::class_<AnimationComponent, PhasedComponent, ComponentPtr>("AnimationComponent")
                .def(luabind::constructor<const DrawablePtr&>())
                .def("addAnimation", &AnimationComponent::addAnimation)
                .def("startAnimation", &AnimationComponent::startAnimation)
                .def("addAnimationForceLoop", &AnimationComponent::script_addAnimationForceLoop)
                .property("currentAnimation", &AnimationComponent::currentAnimation)
                .property("animationFinished", &AnimationComponent::animationFinished)
                .property("animationFrameIndex", &AnimationComponent::animationFrameIndex)
                .property("drawable", &AnimationComponent::drawable, &AnimationComponent::setDrawable)
                .property("paused", &AnimationComponent::paused, &AnimationComponent::setPaused),

            luabind::class_<SpawnerComponent, PhasedComponent, ComponentPtr>("SpawnerComponent")
                .def("startSpawn", &SpawnerComponent::startSpawn)
                .def("finishSpawn", &SpawnerComponent::finishSpawn),

            luabind::class_<ScorpComponent, TargetableComponent, ComponentPtr>("ScorpComponent")
                .property("weapon", &ScorpComponent::weapon),

            luabind::class_<SentryComponent, TargetableComponent, ComponentPtr>("SentryComponent")
                .property("patrol", &SentryComponent::patrol, &SentryComponent::setPatrol)
                .property("targetDistance", &SentryComponent::targetDistance, &SentryComponent::setTargetDistance)
                .def("unfold", &SentryComponent::unfold),

            luabind::class_<KeeperComponent, TargetableComponent, ComponentPtr>("KeeperComponent")
                .def("crawlOut", &KeeperComponent::crawlOut),

            luabind::class_<EnforcerComponent, TargetableComponent, ComponentPtr>("EnforcerComponent")
                .property("patrol", &EnforcerComponent::patrol, &EnforcerComponent::setPatrol)
                .property("detour", &EnforcerComponent::detour, &EnforcerComponent::setDetour)
                .property("targetDistance", &EnforcerComponent::targetDistance, &EnforcerComponent::setTargetDistance),

            luabind::class_<TeleportComponent, PhasedComponent, ComponentPtr>("TeleportComponent")
                .def("start", &TeleportComponent::start)
                .property("finished", &TeleportComponent::finished),

            luabind::class_<RoboArmComponent, PhasedComponent, ComponentPtr>("RoboArmComponent")
                .property("active", &RoboArmComponent::active, &RoboArmComponent::setActive),

            luabind::class_<GoalAreaComponent, PhasedComponent, ComponentPtr>("GoalAreaComponent")
                .def(luabind::constructor<>())
                .def("addGoal", (void (GoalAreaComponent::*)(const b2Vec2&))&GoalAreaComponent::addGoal)
                .def("removeGoal", (void (GoalAreaComponent::*)(const b2Vec2&))&GoalAreaComponent::removeGoal)
                .def("addGoal", (void (GoalAreaComponent::*)(const SceneObjectPtr&, bool))&GoalAreaComponent::addGoal)
                .def("removeGoal", (void (GoalAreaComponent::*)(const SceneObjectPtr&))&GoalAreaComponent::removeGoal)
                .def("removeAllGoals", &GoalAreaComponent::removeAllGoals)
                .property("object", &GoalAreaComponent::object, &GoalAreaComponent::setObject),

            luabind::class_<PuzzleHintAreaComponent, PhasedComponent, ComponentPtr>("PuzzleHintAreaComponent")
                .def("addHintTrigger", &PuzzleHintAreaComponent::addHintTrigger)
                .def("addHint", &PuzzleHintAreaComponent::addHint)
                .def("removeHint", &PuzzleHintAreaComponent::removeHint)
                .def("removeAllHints", &PuzzleHintAreaComponent::removeAllHints)
                .property("object", &PuzzleHintAreaComponent::object, &PuzzleHintAreaComponent::setObject)
                .property("timeout", &PuzzleHintAreaComponent::timeout, &PuzzleHintAreaComponent::setTimeout),

            luabind::class_<WeaponComponent, PhasedComponent, ComponentPtr>("WeaponComponent")
                .def("trigger", &WeaponComponent::trigger)
                .def("triggerOnce", &WeaponComponent::triggerOnce)
                .def("reload", &WeaponComponent::reload)
                .def("cancel", &WeaponComponent::cancel)
                .property("pos", &WeaponComponent::pos, &WeaponComponent::setPos)
                .property("angle", &WeaponComponent::angle, &WeaponComponent::setAngle)
                .property("haveSound", &WeaponComponent::haveSound, &WeaponComponent::setHaveSound)
                .property("triggerHeld", &WeaponComponent::triggerHeld)
                .property("flashDistance", &WeaponComponent::flashDistance, &WeaponComponent::setFlashDistance)
                .property("useFilter", &WeaponComponent::useFilter, &WeaponComponent::setUseFilter)
                .property("useFreeze", &WeaponComponent::useFreeze, &WeaponComponent::setUseFreeze),

            luabind::class_<WeaponGGComponent, WeaponComponent, ComponentPtr>("WeaponGGComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("heldObject", &WeaponGGComponent::heldObject),

            luabind::class_<WeaponFireballComponent, WeaponComponent, ComponentPtr>("WeaponFireballComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("damage", &WeaponFireballComponent::damage, &WeaponFireballComponent::setDamage)
                .property("velocity", &WeaponFireballComponent::velocity, &WeaponFireballComponent::setVelocity)
                .property("interval", &WeaponFireballComponent::interval, &WeaponFireballComponent::setInterval),

            luabind::class_<WeaponBlasterComponent, WeaponComponent, ComponentPtr>("WeaponBlasterComponent")
                .def(luabind::constructor<bool, SceneObjectType>())
                .property("damage", &WeaponBlasterComponent::damage, &WeaponBlasterComponent::setDamage)
                .property("velocity", &WeaponBlasterComponent::velocity, &WeaponBlasterComponent::setVelocity)
                .property("turns", &WeaponBlasterComponent::turns, &WeaponBlasterComponent::setTurns)
                .property("shotsPerTurn", &WeaponBlasterComponent::shotsPerTurn, &WeaponBlasterComponent::setShotsPerTurn)
                .property("turnInterval", &WeaponBlasterComponent::turnInterval, &WeaponBlasterComponent::setTurnInterval)
                .property("loopDelay", &WeaponBlasterComponent::loopDelay, &WeaponBlasterComponent::setLoopDelay),

            luabind::class_<WeaponFlamethrowerComponent, WeaponComponent, ComponentPtr>("WeaponFlamethrowerComponent")
                .def(luabind::constructor<SceneObjectType>())
                .def("setScale", &WeaponFlamethrowerComponent::setScale)
                .def("setColors", &WeaponFlamethrowerComponent::setColors)
                .property("damage", &WeaponFlamethrowerComponent::damage, &WeaponFlamethrowerComponent::setDamage)
                .property("haveLight", &WeaponFlamethrowerComponent::haveLight, &WeaponFlamethrowerComponent::setHaveLight)
                .property("scale", &WeaponFlamethrowerComponent::scale),

            luabind::class_<WeaponPistolComponent, WeaponComponent, ComponentPtr>("WeaponPistolComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("damage", &WeaponPistolComponent::damage, &WeaponPistolComponent::setDamage)
                .property("velocity", &WeaponPistolComponent::velocity, &WeaponPistolComponent::setVelocity)
                .property("interval", &WeaponPistolComponent::interval, &WeaponPistolComponent::setInterval),

            luabind::class_<WeaponMachineGunComponent, WeaponComponent, ComponentPtr>("WeaponMachineGunComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("damage", &WeaponMachineGunComponent::damage, &WeaponMachineGunComponent::setDamage)
                .property("velocity", &WeaponMachineGunComponent::velocity, &WeaponMachineGunComponent::setVelocity)
                .property("spreadAngle", &WeaponMachineGunComponent::spreadAngle, &WeaponMachineGunComponent::setSpreadAngle)
                .property("turnInterval", &WeaponMachineGunComponent::turnInterval, &WeaponMachineGunComponent::setTurnInterval)
                .property("turnDuration", &WeaponMachineGunComponent::turnDuration, &WeaponMachineGunComponent::setTurnDuration)
                .property("loopDelay", &WeaponMachineGunComponent::loopDelay, &WeaponMachineGunComponent::setLoopDelay)
                .property("tweakPos", &WeaponMachineGunComponent::tweakPos, &WeaponMachineGunComponent::setTweakPos),

            luabind::class_<WeaponRLauncherComponent, WeaponComponent, ComponentPtr>("WeaponRLauncherComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("explosionImpulse", &WeaponRLauncherComponent::explosionImpulse, &WeaponRLauncherComponent::setExplosionImpulse)
                .property("explosionDamage", &WeaponRLauncherComponent::explosionDamage, &WeaponRLauncherComponent::setExplosionDamage)
                .property("velocity", &WeaponRLauncherComponent::velocity, &WeaponRLauncherComponent::setVelocity)
                .property("interval", &WeaponRLauncherComponent::interval, &WeaponRLauncherComponent::setInterval),

            luabind::class_<WeaponLGunComponent, WeaponComponent, ComponentPtr>("WeaponLGunComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("length", &WeaponLGunComponent::length, &WeaponLGunComponent::setLength)
                .property("damage", &WeaponLGunComponent::damage, &WeaponLGunComponent::setDamage)
                .property("impulse", &WeaponLGunComponent::impulse, &WeaponLGunComponent::setImpulse),

            luabind::class_<WeaponPlasmaGunComponent, WeaponComponent, ComponentPtr>("WeaponPlasmaGunComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("damage", &WeaponPlasmaGunComponent::damage, &WeaponPlasmaGunComponent::setDamage)
                .property("velocity", &WeaponPlasmaGunComponent::velocity, &WeaponPlasmaGunComponent::setVelocity)
                .property("spreadAngle", &WeaponPlasmaGunComponent::spreadAngle, &WeaponPlasmaGunComponent::setSpreadAngle)
                .property("numShots", &WeaponPlasmaGunComponent::numShots, &WeaponPlasmaGunComponent::setNumShots)
                .property("interval", &WeaponPlasmaGunComponent::interval, &WeaponPlasmaGunComponent::setInterval),

            luabind::class_<WeaponRopeComponent, WeaponComponent, ComponentPtr>("WeaponRopeComponent")
                .def(luabind::constructor<SceneObjectType>())
                .def("tear", &WeaponRopeComponent::tear)
                .property("length", &WeaponRopeComponent::length, &WeaponRopeComponent::setLength)
                .property("launchSpeed", &WeaponRopeComponent::launchSpeed, &WeaponRopeComponent::setLaunchSpeed)
                .property("launchDistance", &WeaponRopeComponent::launchDistance, &WeaponRopeComponent::setLaunchDistance)
                .property("pullSpeed", &WeaponRopeComponent::pullSpeed, &WeaponRopeComponent::setPullSpeed)
                .property("pullMaxForce", &WeaponRopeComponent::pullMaxForce, &WeaponRopeComponent::setPullMaxForce)
                .property("interval", &WeaponRopeComponent::interval, &WeaponRopeComponent::setInterval)
                .property("hit", &WeaponRopeComponent::hit),

            luabind::class_<WeaponAcidballComponent, WeaponComponent, ComponentPtr>("WeaponAcidballComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("velocity", &WeaponAcidballComponent::velocity, &WeaponAcidballComponent::setVelocity)
                .property("interval", &WeaponAcidballComponent::interval, &WeaponAcidballComponent::setInterval)
                .property("explosionTimeout", &WeaponAcidballComponent::explosionTimeout, &WeaponAcidballComponent::setExplosionTimeout)
                .property("explosionImpulse", &WeaponAcidballComponent::explosionImpulse, &WeaponAcidballComponent::setExplosionImpulse)
                .property("explosionDamage", &WeaponAcidballComponent::explosionDamage, &WeaponAcidballComponent::setExplosionDamage)
                .property("toxicDamage", &WeaponAcidballComponent::toxicDamage, &WeaponAcidballComponent::setToxicDamage)
                .property("toxicDamageTimeout", &WeaponAcidballComponent::toxicDamageTimeout, &WeaponAcidballComponent::setToxicDamageTimeout),

            luabind::class_<WeaponHeaterComponent, WeaponComponent, ComponentPtr>("WeaponHeaterComponent")
                .def(luabind::constructor<SceneObjectType>())
                .property("damage", &WeaponHeaterComponent::damage, &WeaponHeaterComponent::setDamage)
                .property("length", &WeaponHeaterComponent::length, &WeaponHeaterComponent::setLength)
                .property("impulse", &WeaponHeaterComponent::impulse, &WeaponHeaterComponent::setImpulse)
                .property("duration", &WeaponHeaterComponent::duration, &WeaponHeaterComponent::setDuration)
                .property("interval", &WeaponHeaterComponent::interval, &WeaponHeaterComponent::setInterval)
                .property("minWidth", &WeaponHeaterComponent::minWidth, &WeaponHeaterComponent::setMinWidth)
                .property("maxWidth", &WeaponHeaterComponent::maxWidth, &WeaponHeaterComponent::setMaxWidth)
                .property("measure", &WeaponHeaterComponent::measure, &WeaponHeaterComponent::setMeasure),

            luabind::class_<MeasurementComponent, PhasedComponent, ComponentPtr>("MeasurementComponent")
                .def(luabind::constructor<>())
                .property("bulletDamageMissed", &MeasurementComponent::bulletDamageMissed)
                .property("heaterDamageReceived", &MeasurementComponent::heaterDamageReceived),

            luabind::class_<Inventory, InventoryPtr>("Inventory")
                .def("have", &Inventory::have)
                .def("count", &Inventory::count)
                .def("give", &Inventory::give)
                .def("take", &Inventory::take),

            luabind::class_<PlaceholderComponent, PhasedComponent, ComponentPtr>("PlaceholderComponent")
                .property("transform", &PlaceholderComponent::transform, &PlaceholderComponent::setTransform)
                .property("listener", &PlaceholderComponent::listener, &PlaceholderComponent::setListener)
                .property("visible", &PlaceholderComponent::visible, &PlaceholderComponent::setVisible)
                .property("active", &PlaceholderComponent::active, &PlaceholderComponent::setActive)
                .property("item", &PlaceholderComponent::item),

            luabind::class_<TimebombComponent, PhasedComponent, ComponentPtr>("TimebombComponent")
                .property("transform", &TimebombComponent::transform, &TimebombComponent::setTransform)
                .property("timeoutFn", &TimebombComponent::timeoutFn, &TimebombComponent::setTimeoutFn)
                .def("setBlast", &TimebombComponent::setBlast),

            luabind::class_<SceneInstance, SceneInstancePtr>("SceneInstance")
                .property("pos", &SceneInstance::pos)
                .property("angle", &SceneInstance::angle)
                .property("objects", (const std::vector<SceneObjectPtr>& (SceneInstance::*)() const)&SceneInstance::objects)
                .property("joints", (const std::vector<JointProxyPtr>& (SceneInstance::*)() const)&SceneInstance::joints)
                .def("getInstances", &SceneInstance::script_getInstances),

            luabind::class_<ExplosionComponent, PhasedComponent, ComponentPtr>("ExplosionComponent")
                .def("setBlast", &ExplosionComponent::setBlast)
                .def("setSound", &ExplosionComponent::setSound),

            luabind::class_<TentaclePulseComponent, PhasedComponent, ComponentPtr>("TentaclePulseComponent")
                .def(luabind::constructor<int, float, float, float, float>()),

            luabind::class_<TentacleSwingComponent, PhasedComponent, ComponentPtr>("TentacleSwingComponent")
                .def(luabind::constructor<>())
                .def(luabind::constructor<float, Easing, float, float, Easing, float>())
                .def("setLoop", &TentacleSwingComponent::setLoop)
                .def("setSingle", &TentacleSwingComponent::setSingle)
                .property("bone", &TentacleSwingComponent::bone, &TentacleSwingComponent::setBone),

            luabind::class_<TentacleAttractComponent, TargetableComponent, ComponentPtr>("TentacleAttractComponent")
                .def(luabind::constructor<float, float>())
                .property("bone", &TentacleAttractComponent::bone, &TentacleAttractComponent::setBone),

            luabind::class_<BoatComponent, PhasedComponent, ComponentPtr>("BoatComponent")
                .property("active", &BoatComponent::active, &BoatComponent::setActive),

            luabind::class_<MechComponent, PhasedComponent, ComponentPtr>("MechComponent")
                .property("active", &MechComponent::active, &MechComponent::setActive),

            luabind::class_<BeetleComponent, TargetableComponent, ComponentPtr>("BeetleComponent")
                .property("spawnPowerup", &BeetleComponent::spawnPowerup, &BeetleComponent::setSpawnPowerup)
                .def("setTrapped", &BeetleComponent::setTrapped),

            luabind::class_<BabySpiderComponent, TargetableComponent, ComponentPtr>("BabySpiderComponent")
                .property("spawnPowerup", &BabySpiderComponent::spawnPowerup, &BabySpiderComponent::setSpawnPowerup)
                .property("haveSound", &BabySpiderComponent::haveSound, &BabySpiderComponent::setHaveSound)
                .def("setTrapped", &BabySpiderComponent::setTrapped),

            luabind::class_<CentipedeComponent, TargetableComponent, ComponentPtr>("CentipedeComponent")
                .property("spawnPowerup", &CentipedeComponent::spawnPowerup, &CentipedeComponent::setSpawnPowerup)
                .def("setTrapped", &CentipedeComponent::setTrapped),

            luabind::class_<DudeComponent, TargetableComponent, ComponentPtr>("DudeComponent")
                .property("haveGun", &DudeComponent::haveGun, &DudeComponent::setHaveGun),

            luabind::class_<GovernedTurretComponent, PhasedComponent, ComponentPtr>("GovernedTurretComponent")
                .def(luabind::constructor<>())
                .property("active", &GovernedTurretComponent::active, &GovernedTurretComponent::setActive)
                .property("turnSpeed", &GovernedTurretComponent::turnSpeed, &GovernedTurretComponent::setTurnSpeed)
                .property("haveSound", &GovernedTurretComponent::haveSound, &GovernedTurretComponent::setHaveSound)
                .property("weapon", &GovernedTurretComponent::weapon, &GovernedTurretComponent::script_setWeapon),

            luabind::class_<PointLight, Light, LightPtr>("PointLight")
                .def(luabind::constructor<const std::string&>())
                .property("distance", &PointLight::distance, &PointLight::setDistance),

            luabind::class_<FloatComponent, TargetableComponent, ComponentPtr>("FloatComponent")
                .def(luabind::constructor<float, float, float>())
                .def("stop", &FloatComponent::stop)
                .property("force", &FloatComponent::force, &FloatComponent::setForce),

            luabind::class_<CreatureComponent, PhasedComponent, ComponentPtr>("CreatureComponent")
                .property("I", &CreatureComponent::I),

            luabind::class_<WalkerComponent, TargetableComponent, ComponentPtr>("WalkerComponent")
                .def("expose", &WalkerComponent::expose)
                .def("angry", &WalkerComponent::angry)
                .property("deathFinished", &WalkerComponent::deathFinished),

            luabind::class_<SnakeComponent, TargetableComponent, ComponentPtr>("SnakeComponent")
                .def("setIdle", &SnakeComponent::setIdle)
                .def("setWalk", &SnakeComponent::setWalk)
                .def("setMegaShoot", &SnakeComponent::setMegaShoot)
                .def("setAngry", &SnakeComponent::setAngry)
                .def("setDie", &SnakeComponent::setDie)
                .property("deathFinished", &SnakeComponent::deathFinished),

            luabind::class_<CountdownComponent, UIComponent, ComponentPtr>("CountdownComponent")
                .def(luabind::constructor<float, float>())
                .def("setText", &CountdownComponent::setText)
                .def("setTextColor", &CountdownComponent::setTextColor),

            luabind::class_<BossBuddyComponent, TargetableComponent, ComponentPtr>("BossBuddyComponent")
                .def("setDead", &BossBuddyComponent::setDead)
                .property("brainDead", &BossBuddyComponent::brainDead)
                .property("canSwallow", &BossBuddyComponent::canSwallow, &BossBuddyComponent::setCanSwallow)
                .property("deathFinished", &BossBuddyComponent::deathFinished),

            luabind::class_<QuadPulseComponent, PhasedComponent, ComponentPtr>("QuadPulseComponent")
                .def(luabind::constructor<const RenderQuadComponentPtr&, const b2Vec2&, float, Easing, float, float, Easing, float>())
                .property("t", &QuadPulseComponent::t, &QuadPulseComponent::setT),

            luabind::class_<BossNatanComponent, TargetableComponent, ComponentPtr>("BossNatanComponent")
                .def("setupInitial", &BossNatanComponent::setupInitial)
                .def("startNapalm", &BossNatanComponent::startNapalm)
                .def("finishNapalm", &BossNatanComponent::finishNapalm)
                .def("finishPowerup", &BossNatanComponent::finishPowerup)
                .def("startPowerupAnimation", &BossNatanComponent::startPowerupAnimation)
                .def("finishPowerupAnimation", &BossNatanComponent::finishPowerupAnimation)
                .property("teleportDests", &BossNatanComponent::teleportDests, &BossNatanComponent::setTeleportDests)
                .property("powerupObj", &BossNatanComponent::powerupObj, &BossNatanComponent::setPowerupObj)
                .property("needPowerup", &BossNatanComponent::needPowerup)
                .property("deathFinished", &BossNatanComponent::deathFinished),

            luabind::class_<PowerBeamComponent, PhasedComponent, ComponentPtr>("PowerBeamComponent")
                .def("finish", &PowerBeamComponent::finish),

            luabind::class_<FootageTrackComponent, PhasedComponent, ComponentPtr>("FootageTrackComponent")
                .def(luabind::constructor<const SceneObjectPtr&, const b2Vec2&, const std::string&, float, const std::string&, float, int>())
                .property("aimMinScale", &FootageTrackComponent::aimMinScale, &FootageTrackComponent::setAimMinScale)
                .property("aimScale", &FootageTrackComponent::aimScale, &FootageTrackComponent::setAimScale)
                .property("aimTime", &FootageTrackComponent::aimTime, &FootageTrackComponent::setAimTime)
                .property("trackTime", &FootageTrackComponent::trackTime, &FootageTrackComponent::setTrackTime)
                .property("transitionTime", &FootageTrackComponent::transitionTime, &FootageTrackComponent::setTransitionTime)
                .property("imageTime", &FootageTrackComponent::imageTime, &FootageTrackComponent::setImageTime)
                .property("textTimeout", &FootageTrackComponent::textTimeout, &FootageTrackComponent::setTextTimeout)
                .property("keepTime", &FootageTrackComponent::keepTime, &FootageTrackComponent::setKeepTime)
                .property("fadeTime", &FootageTrackComponent::fadeTime, &FootageTrackComponent::setFadeTime)
                .property("trackColor", &FootageTrackComponent::trackColor, &FootageTrackComponent::setTrackColor)
                .property("imageColor", &FootageTrackComponent::imageColor, &FootageTrackComponent::setImageColor)
                .property("textColor", &FootageTrackComponent::textColor, &FootageTrackComponent::setTextColor),

            luabind::class_<RenderTextComponent, RenderComponent, ComponentPtr>("RenderTextComponent")
                .def(luabind::constructor<const b2Vec2&, float, float, const std::string&, int>())
                .property("pos", &RenderTextComponent::pos, &RenderTextComponent::setPos)
                .property("angle", &RenderTextComponent::angle, &RenderTextComponent::setAngle)
                .property("width", &RenderTextComponent::width, &RenderTextComponent::setWidth)
                .property("charSize", &RenderTextComponent::charSize, &RenderTextComponent::setCharSize)
                .property("text", &RenderTextComponent::text, &RenderTextComponent::setText)
                .property("maxLetters", &RenderTextComponent::maxLetters)
                .property("numLetters", &RenderTextComponent::numLetters, &RenderTextComponent::setNumLetters),

            luabind::class_<LetterBoxComponent, UIComponent, ComponentPtr>("LetterBoxComponent")
                .def(luabind::constructor<float, float, int>())
                .def("setActive", &LetterBoxComponent::setActive),

            luabind::class_<InputHintComponent, UIComponent, ComponentPtr>("InputHintComponent")
                .def(luabind::constructor<int>())
                .def("addKbNormal", &InputHintComponent::addKbNormal)
                .def("addKbLong", &InputHintComponent::addKbLong)
                .def("addText", &InputHintComponent::addText)
                .def("addMb", &InputHintComponent::addMb)
                .def("addGamepadStick", &InputHintComponent::addGamepadStick)
                .def("addGamepadButton", &InputHintComponent::addGamepadButton)
                .def("setDescription", &InputHintComponent::setDescription)
                .def("setFade", &InputHintComponent::setFade),

            luabind::class_<Platform>("Platform")
                .property("desktopVideoModes", &Platform::desktopVideoModes)
                .property("winVideoModes", &Platform::winVideoModes)
                .property("defaultVideoMode", &Platform::defaultVideoMode)
                .property("desktopVideoMode", &Platform::desktopVideoMode)
                .property("msaaModes", &Platform::msaaModes)
                .property("vsyncSupported", &Platform::vsyncSupported)
                .def("changeVideoMode", &Platform::changeVideoMode),

            luabind::class_<GameShell>("GameShell")
                .def("getAchievementImageName", &GameShell::script_getAchievementImageName)
                .def("getAchievementName", &GameShell::getAchievementName)
                .def("getAchievementDescription", &GameShell::getAchievementDescription)
                .def("isAchieved", &GameShell::isAchieved)
                .def("setAchieved", &GameShell::setAchieved)
                .def("getAchievementProgress", &GameShell::getAchievementProgress)
                .def("incAchievementProgress", &GameShell::incAchievementProgress)
                .def("getLanguage", &GameShell::getLanguage),

            luabind::class_<InputBinding, InputBindingPtr>("InputBinding")
                .def(luabind::const_self == luabind::const_self)
                .def("isKey", &InputBinding::isKey)
                .def("isLongKey", (bool (InputBinding::*)() const)&InputBinding::isLongKey)
                .def("isMb", &InputBinding::isMb)
                .def("setKey", &InputBinding::setKey)
                .def("setMb", &InputBinding::setMb)
                .def("equal", &InputBinding::equal)
                .def("assign", &InputBinding::assign)
                .def("clear", &InputBinding::clear)
                .def("empty", &InputBinding::empty)
                .scope
                [
                    luabind::def("keyToString", &InputBinding::keyToString)
                ]
                .property("ki", &InputBinding::ki)
                .property("str", &InputBinding::str),

            luabind::class_<ActionBinding, ActionBindingPtr>("ActionBinding")
                .def(luabind::const_self == luabind::const_self)
                .def("ib", &ActionBinding::ib),

            luabind::class_<InputGamepadBinding, InputGamepadBindingPtr>("InputGamepadBinding")
                .def(luabind::const_self == luabind::const_self)
                .def("isLongButton", (bool (InputGamepadBinding::*)() const)&InputGamepadBinding::isLongButton)
                .def("setButton", &InputGamepadBinding::setButton)
                .def("equal", &InputGamepadBinding::equal)
                .def("assign", &InputGamepadBinding::assign)
                .def("clear", &InputGamepadBinding::clear)
                .def("empty", &InputGamepadBinding::empty)
                .scope
                [
                    luabind::def("buttonToString", &InputGamepadBinding::buttonToString)
                ]
                .property("button", &InputGamepadBinding::button)
                .property("str", &InputGamepadBinding::str),

            luabind::class_<ActionGamepadBinding, ActionGamepadBindingPtr>("ActionGamepadBinding")
                .def(luabind::const_self == luabind::const_self)
                .def("ib", &ActionGamepadBinding::ib),

            luabind::class_<InputGamepad>("InputGamepad")
                .property("stickDeadzone", &InputGamepad::stickDeadzone, &InputGamepad::setStickDeadzone)
                .property("triggerDeadzone", &InputGamepad::triggerDeadzone, &InputGamepad::setTriggerDeadzone)
                .def("triggered", &InputGamepad::triggered),

            luabind::class_<InputManager>("InputManager")
                .def("binding", &InputManager::binding)
                .def("gamepadBinding", &InputManager::gamepadBinding)
                .property("gamepad", &InputManager::gamepad)
                .property("usingGamepad", &InputManager::usingGamepad),

            luabind::class_<UserData>("UserData")
                .property("numGems", &UserData::numGems)
                .def("giveGems", &UserData::giveGems)
                .def("levelAccessible", &UserData::levelAccessible)
                .def("setLevelAccessible", &UserData::setLevelAccessible),

            luabind::class_<UpgradeManager>("UpgradeManager")
                .scope
                [
                    luabind::def("getImageName", &UpgradeManager::getImageName),
                    luabind::def("getCost", &UpgradeManager::getCost)
                ]
                .def("give", &UpgradeManager::give)
                .def("clear", &UpgradeManager::clear),

            luabind::class_<Statistics, StatisticsPtr>("Statistics")
                .property("enemiesSpawned", &Statistics::enemiesSpawned)
                .property("enemiesKilled", &Statistics::enemiesKilled)
                .property("accuracy", &Statistics::accuracy)
                .property("playTime", &Statistics::playTime)
                .property("numAnimals", &Statistics::numAnimals)
                .property("numKilledAnimals", &Statistics::numKilledAnimals)
        ];
#endif
    }
}
