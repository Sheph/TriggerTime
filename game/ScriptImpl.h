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

#ifndef _SCRIPT_IMPL_H_
#define _SCRIPT_IMPL_H_

#include "Script.h"
#include "Logger.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Path.h"
#include "SingleTweening.h"
#include "SequentialTweening.h"
#include "ScriptSensorListener.h"
#include "CollisionSensorComponent.h"
#include "CollisionDamageComponent.h"
#include "CollisionCancelComponent.h"
#include "ScriptCollisionComponent.h"
#include "ScriptComponent.h"
#include "ScriptDialogComponent.h"
#include "ScriptMainMenuComponent.h"
#include "ScriptPickerComponent.h"
#include "ScriptUITimerComponent.h"
#include "ShroomGuardianComponent.h"
#include "PlayerComponent.h"
#include "CameraComponent.h"
#include "RenderBackgroundComponent.h"
#include "RenderQuadComponent.h"
#include "RenderStripeComponent.h"
#include "RenderTerrainComponent.h"
#include "PathComponent.h"
#include "FollowPathComponent.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "TetrobotComponent.h"
#include "TetrocrabComponent.h"
#include "LightComponent.h"
#include "FadeOutComponent.h"
#include "AnimationComponent.h"
#include "SpawnerComponent.h"
#include "ScorpComponent.h"
#include "TeleportComponent.h"
#include "LevelCompletedComponent.h"
#include "GameOverComponent.h"
#include "GoalIndicatorComponent.h"
#include "GoalAreaComponent.h"
#include "ScriptChoiceComponent.h"
#include "WeaponGGComponent.h"
#include "WeaponFireballComponent.h"
#include "WeaponBlasterComponent.h"
#include "WeaponPistolComponent.h"
#include "WeaponRLauncherComponent.h"
#include "WeaponMachineGunComponent.h"
#include "WeaponPlasmaGunComponent.h"
#include "WeaponRopeComponent.h"
#include "WeaponAcidballComponent.h"
#include "PyrobotComponent.h"
#include "MeasurementComponent.h"
#include "PlaceholderComponent.h"
#include "TimebombComponent.h"
#include "SpiderComponent.h"
#include "ExplosionComponent.h"
#include "StainedGlassComponent.h"
#include "BossSkullComponent.h"
#include "BossQueenComponent.h"
#include "SentryComponent.h"
#include "EnforcerComponent.h"
#include "RoboArmComponent.h"
#include "RenderProjComponent.h"
#include "InvulnerabilityComponent.h"
#include "WeaponLGunComponent.h"
#include "KeeperComponent.h"
#include "DummyComponent.h"
#include "CollisionFilter.h"
#include "CollisionModifyComponent.h"
#include "ParticleEffectComponent.h"
#include "BossCoreProtectorComponent.h"
#include "RenderHealthbarComponent.h"
#include "RenderTentacleComponent.h"
#include "TentaclePulseComponent.h"
#include "TentacleSwingComponent.h"
#include "TentacleAttractComponent.h"
#include "BoatComponent.h"
#include "BossSquidComponent.h"
#include "MechComponent.h"
#include "BeetleComponent.h"
#include "CentipedeComponent.h"
#include "GovernedTurretComponent.h"
#include "BossChopperComponent.h"
#include "FloatComponent.h"
#include "CreatureComponent.h"
#include "WalkerComponent.h"
#include "SnakeComponent.h"
#include "CountdownComponent.h"
#include "BossBuddyComponent.h"
#include "QuadPulseComponent.h"
#include "BossNatanComponent.h"
#include "PowerBeamComponent.h"
#include "WeaponHeaterComponent.h"
#include "GorgerComponent.h"
#include "RenderSaturationComponent.h"
#include "FootageTrackComponent.h"
#include "RenderTextComponent.h"
#include "LetterBoxComponent.h"
#include "InputHintComponent.h"
#include "InputManager.h"
#include "UserData.h"
#include "UpgradeManager.h"
#include "Const.h"
#include "Settings.h"
#include "Platform.h"
#include "AudioManager.h"
#include "Utils.h"
#include "HermitePath.h"
#include "PuzzleHintAreaComponent.h"
#include "BossBeholderComponent.h"
#include "BabySpiderComponent.h"
#include "DudeComponent.h"
#include "GameShell.h"
#include "af/Utils.h"
#include "luainc.h"
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <boost/make_shared.hpp>
#include <log4cplus/ndc.h>
#include "ScriptConverters.h"

namespace
{
    static void updateCallWrapper(const luabind::object& obj, bool member, float dt)
    {
        try
        {
            if (member) {
                luabind::call_member<void>(obj, "update", dt);
            } else {
                luabind::call_function<void>(obj, dt);
            }
        }
        catch (const luabind::error& e)
        {
            ::lua_pop(e.state(), 1);
        }
        catch (const std::exception& e)
        {
            LOG4CPLUS_ERROR(af::logger(), e.what());
        }
    }

    struct GlobalConstHolder {};
}

namespace luabind
{
    template <>
    struct default_converter<af::Scene::TimerFn> : native_converter_base<af::Scene::TimerFn>
    {
        static int compute_score(lua_State* L, int index)
        {
            int type = ::lua_type(L, index);
            return ((type == LUA_TFUNCTION) || (type == LUA_TUSERDATA) || (type == LUA_TTABLE)) ? 0 : -1;
        }

        af::Scene::TimerFn from(lua_State* L, int index)
        {
            int type = ::lua_type(L, index);

            luabind::object tmp(luabind::from_stack(L, index));

            return boost::bind(&updateCallWrapper, tmp, (type != LUA_TFUNCTION), _1);
        }

        void to(lua_State* L, const af::Scene::TimerFn& value)
        {
            throw luabind::cast_failed(L, typeid(value));
        }
    };

    template <>
    struct default_converter<const af::Scene::TimerFn&> : default_converter<af::Scene::TimerFn>
    {};
}

namespace af
{
    /*
     * Operator hacks for luabind. The problem with luabind is that
     * it doesn't handle ==, !=, etc. for shared_ptr. Even though shared_ptr
     * has these functions it's impossible to register them via luabind
     * operator registration, so the only way is to pretend our shared_ptr'ed
     * class has these operators. We don't want to expose these to everyone
     * else of course, so we just make them static here.
     * @{
     */

    static bool operator==(const Component& lhs, const Component& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const SceneObject& lhs, const SceneObject& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const SensorListener& lhs, const SensorListener& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const JointProxy& lhs, const JointProxy& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const Light& lhs, const Light& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const ActionBinding& lhs, const ActionBinding& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const InputBinding& lhs, const InputBinding& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const ActionGamepadBinding& lhs, const ActionGamepadBinding& rhs)
    {
        return &lhs == &rhs;
    }

    static bool operator==(const InputGamepadBinding& lhs, const InputGamepadBinding& rhs)
    {
        return &lhs == &rhs;
    }

    /*
     * @}
     */

    class Script::Impl
    {
    public:
        Impl(const std::string& path,
             Scene* scene);
        ~Impl();

        void bind();

        void bind2();

        static int loadPackage(lua_State* L);

        void setPackageLoaders();

        void require(const std::string& name);

        void createGlobals();

        void loadFile();

        static int print(lua_State* L);

        std::string path_;
        Scene* scene_;

        lua_State* L_;
    };
}

#endif
