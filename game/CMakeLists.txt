set(SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/config-easy.ini.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/config-hard.ini.cpp
    tt.rc
    Platform.cpp
    Utils.cpp
    Game.cpp
    OGL.cpp
    InputManager.cpp
    InputKeyboard.cpp
    InputTouchScreen.cpp
    InputMouse.cpp
    Scene.cpp
    SceneObject.cpp
    TextureManager.cpp
    Logger.cpp
    Renderer.cpp
    AssetManager.cpp
    SceneObjectAsset.cpp
    SceneAsset.cpp
    Image.cpp
    SceneObjectFactory.cpp
    Script.cpp
    Script2.cpp
    SceneObjectManager.cpp
    Component.cpp
    PhasedComponentManager.cpp
    PhysicsComponentManager.cpp
    CollisionComponentManager.cpp
    RenderComponentManager.cpp
    CameraComponent.cpp
    PlayerComponent.cpp
    ShroomCannonComponent.cpp
    MissileComponent.cpp
    ExplosionComponent.cpp
    BlasterShotComponent.cpp
    PhysicsBodyComponent.cpp
    PhysicsJointComponent.cpp
    PhysicsRopeComponent.cpp
    CollisionFilter.cpp
    CollisionDieComponent.cpp
    CollisionSparksComponent.cpp
    CollisionDamageComponent.cpp
    CollisionSensorComponent.cpp
    CollisionRopeComponent.cpp
    RenderComponent.cpp
    RenderPolygonsComponent.cpp
    RenderQuadComponent.cpp
    RenderTerrainComponent.cpp
    RenderBackgroundComponent.cpp
    RenderStripeComponent.cpp
    RenderDamageComponent.cpp
    ParticleEffectComponent.cpp
    ParticleEmmiter.cpp
    Drawable.cpp
    Animation.cpp
    AnimationComponent.cpp
    Const.cpp
    HealthBarComponent.cpp
    ScriptSensorListener.cpp
    JointProxy.cpp
    ScriptComponent.cpp
    ShroomGuardianComponent.cpp
    TextArea.cpp
    DialogComponent.cpp
    ScriptDialogComponent.cpp
    Settings.cpp
    HermitePath.cpp
    SingleTweening.cpp
    SequentialTweening.cpp
    PathComponent.cpp
    FollowPathComponent.cpp
    DebugDraw.cpp
    ShroomSnakeComponent.cpp
    KnobComponent.cpp
    Level.cpp
    MainMenuComponent.cpp
    ScriptMainMenuComponent.cpp
    Frame.cpp
    PickerComponent.cpp
    ScriptPickerComponent.cpp
    UIComponentManager.cpp
    PauseMenuComponent.cpp
    RopeLaunchComponent.cpp
    RopePullComponent.cpp
    RenderBatch.cpp
    RenderQueue.cpp
    TetrobotComponent.cpp
    TetrocrabComponent.cpp
    LaserComponent.cpp
    RenderBeamComponent.cpp
    Light.cpp
    RenderLightComponent.cpp
    PointLight.cpp
    LightComponent.cpp
    ConeLight.cpp
    LineLight.cpp
    AudioManager.cpp
    RenderDottedLineComponent.cpp
    RenderDottedCircleComponent.cpp
    GravityGunAimComponent.cpp
    GravityGunComponent.cpp
    ScriptCollisionComponent.cpp
    FadeOutComponent.cpp
    TargetableComponent.cpp
    BehaviorRoamComponent.cpp
    BehaviorComponent.cpp
    BehaviorSeekComponent.cpp
    BehaviorAvoidComponent.cpp
    BehaviorDetourComponent.cpp
    BehaviorInterceptComponent.cpp
    AudioSourceDummy.cpp
    RenderHealthbarComponent.cpp
    SpawnerComponent.cpp
    ScorpComponent.cpp
    FireballComponent.cpp
    TeleportComponent.cpp
    StainedGlassComponent.cpp
    LevelCompletedComponent.cpp
    GameOverComponent.cpp
    NumericIndicatorComponent.cpp
    PowerupComponent.cpp
    GoalIndicatorComponent.cpp
    GoalAreaComponent.cpp
    BloodComponent.cpp
    StunnedComponent.cpp
    InvulnerabilityComponent.cpp
    RenderChainComponent.cpp
    ChoiceComponent.cpp
    ScriptChoiceComponent.cpp
    WeaponComponent.cpp
    WeaponBlasterComponent.cpp
    WeaponGGComponent.cpp
    WeaponFireballComponent.cpp
    WeaponStationComponent.cpp
    WeaponShotgunComponent.cpp
    BuckshotComponent.cpp
    RenderTrailComponent.cpp
    BarrelComponent.cpp
    TurretComponent.cpp
    GoalComponent.cpp
    PyrobotComponent.cpp
    WeaponFlamethrowerComponent.cpp
    MeasurementComponent.cpp
    Inventory.cpp
    InventoryComponent.cpp
    PlaceholderComponent.cpp
    TimebombComponent.cpp
    BossSkullComponent.cpp
    BossSkullGunComponent.cpp
    SpiderComponent.cpp
    SceneInstance.cpp
    BabySpiderComponent.cpp
    SpiderNestComponent.cpp
    BossQueenComponent.cpp
    CollisionCancelComponent.cpp
    WeaponPistolComponent.cpp
    BulletComponent.cpp
    EnforcerComponent.cpp
    SentryComponent.cpp
    WeaponMachineGunComponent.cpp
    GorgerComponent.cpp
    WeaponRLauncherComponent.cpp
    RocketComponent.cpp
    WeaponProxMineComponent.cpp
    ProxMineComponent.cpp
    InputContext.cpp
    InputController.cpp
    InputControllerPC.cpp
    InputContextCutscenePC.cpp
    InputContextPlayerPC.cpp
    InputContextUIPC.cpp
    InputControllerTS.cpp
    InputContextCutsceneTS.cpp
    InputContextPlayerTS.cpp
    InputContextUITS.cpp
    LaserSightComponent.cpp
    RoboArmComponent.cpp
    InputContextRoboArmPC.cpp
    InputContextRoboArmTS.cpp
    RenderProjComponent.cpp
    CrateComponent.cpp
    WeaponChainsawComponent.cpp
    OrboComponent.cpp
    RenderLBoltComponent.cpp
    WeaponLGunComponent.cpp
    WarderComponent.cpp
    WeaponPlasmaGunComponent.cpp
    PlasmaComponent.cpp
    KeeperComponent.cpp
    DummyComponent.cpp
    DudeComponent.cpp
    CollisionModifyComponent.cpp
    RingLight.cpp
    HealthStationComponent.cpp
    SummonComponent.cpp
    BossCoreProtectorComponent.cpp
    ToxicSplashComponent.cpp
    ToxicCloudComponent.cpp
    AcidWormComponent.cpp
    WeaponAcidballComponent.cpp
    AcidballComponent.cpp
    CentipedeComponent.cpp
    WeaponSpitterComponent.cpp
    SpitComponent.cpp
    BeetleComponent.cpp
    RenderTentacleComponent.cpp
    TentaclePulseComponent.cpp
    TentacleSwingComponent.cpp
    TentacleAttractComponent.cpp
    InputContextBoatPC.cpp
    BoatComponent.cpp
    InputContextBoatTS.cpp
    PowerGenComponent.cpp
    TentacleSquidComponent.cpp
    BossSquidComponent.cpp
    InputContextMechPC.cpp
    MechComponent.cpp
    InputContextMechTS.cpp
    HomerComponent.cpp
    SeekerComponent.cpp
    WeaponSeekerComponent.cpp
    WeaponEShieldComponent.cpp
    InputContextTurret.cpp
    InputContextTurretPC.cpp
    InputContextTurretTS.cpp
    GovernedTurretComponent.cpp
    BossChopperComponent.cpp
    FloatComponent.cpp
    CreatureComponent.cpp
    GasCloudComponent.cpp
    OctopusComponent.cpp
    WalkerComponent.cpp
    SnakeComponent.cpp
    CountdownComponent.cpp
    BossBuddyComponent.cpp
    QuadPulseComponent.cpp
    BuddySideComponent.cpp
    WeaponRopeComponent.cpp
    BuddyTongueComponent.cpp
    NapalmComponent.cpp
    BossNatanComponent.cpp
    DizzyComponent.cpp
    PowerBeamComponent.cpp
    Tetrobot2Component.cpp
    WeaponHeaterComponent.cpp
    WeaponAimComponent.cpp
    GuardianComponent.cpp
    RenderSaturationComponent.cpp
    FootageTrackComponent.cpp
    RenderTextComponent.cpp
    LetterBoxComponent.cpp
    InputHintComponent.cpp
    CrosshairComponent.cpp
    SawerComponent.cpp
    TentacleInflateComponent.cpp
    BuzzSawComponent.cpp
    CollisionBuzzSawComponent.cpp
    CollisionBuzzSawMissileComponent.cpp
    LRFileInterface.cpp
    LRRenderInterface.cpp
    LRSystemInterface.cpp
    LRManager.cpp
    LRComponent.cpp
    Texture.cpp
    UITimerComponent.cpp
    ScriptUITimerComponent.cpp
    InputBinding.cpp
    UserData.cpp
    UpgradeManager.cpp
    UpgradeDisplayComponent.cpp
    GameLogAppender.cpp
    Statistics.cpp
    PuzzleHintAreaComponent.cpp
    PuzzleHintIndicatorComponent.cpp
    PuzzleHintComponent.cpp
    InputGamepad.cpp
    InputGamepadBinding.cpp
    BossBeholderComponent.cpp
    DamageFlashComponent.cpp
    StaminaBarComponent.cpp
    GameShell.cpp
    DummyShell.cpp
)

if (USE_STEAM_API)
    set(SOURCES ${SOURCES}
        SteamShell.cpp
    )
endif ()

file(READ "config.ini" _INI)
file(READ "config-easy.ini" _EASY_INI)
file(READ "config-hard.ini" _HARD_INI)

string(REGEX REPLACE ";" "\\\\;" _INI "${_INI}")
string(REGEX REPLACE "\n" ";" _INI "${_INI}")
string(REGEX REPLACE ";" "\\\\;" _EASY_INI "${_EASY_INI}")
string(REGEX REPLACE "\n" ";" _EASY_INI "${_EASY_INI}")
string(REGEX REPLACE ";" "\\\\;" _HARD_INI "${_HARD_INI}")
string(REGEX REPLACE "\n" ";" _HARD_INI "${_HARD_INI}")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp" "extern const char configIniStr[];\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp" "const char configIniStr[] =")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config-easy.ini.cpp" "extern const char configEasyIniStr[];\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-easy.ini.cpp" "const char configEasyIniStr[] =")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config-hard.ini.cpp" "extern const char configHardIniStr[];\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-hard.ini.cpp" "const char configHardIniStr[] =")

if (WIN32)
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp" "\n\"assets=./assets\\n\"")

    foreach (_CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
        add_custom_command(
            OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config.ini
            COMMAND ${CMAKE_COMMAND} -E echo assets=../../../../game/assets > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config.ini
            COMMAND type config.ini >> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config.ini
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS config.ini
            VERBATIM
        )
        set(SOURCES ${SOURCES}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config.ini
        )
        configure_file(${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/OpenAL32.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/OpenAL32.dll COPYONLY)
        configure_file(${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/wrap_oal.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/wrap_oal.dll COPYONLY)
        configure_file(config-easy.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config-easy.ini COPYONLY)
        configure_file(config-hard.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/config-hard.ini COPYONLY)
        if (USE_STEAM_API)
            configure_file(steam_appid.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/steam_appid.txt COPYONLY)
            configure_file(${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/steam_api${STEAM_API_SUFFIX}.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/steam_api${STEAM_API_SUFFIX}.dll COPYONLY)
        endif ()
    endforeach ()
    set(SOURCES ${SOURCES}
        PlatformWin32.cpp
        main_win32.cpp
    )
else ()
    set(SOURCES ${SOURCES}
        PlatformLinux.cpp
        main_x11.cpp
    )
endif ()

foreach (_ENTRY ${_INI})
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp" "\n\"${_ENTRY}\\n\"")
endforeach()
foreach (_ENTRY ${_EASY_INI})
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-easy.ini.cpp" "\n\"${_ENTRY}\\n\"")
endforeach()
foreach (_ENTRY ${_HARD_INI})
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-hard.ini.cpp" "\n\"${_ENTRY}\\n\"")
endforeach()

file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config.ini.cpp" ";\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-easy.ini.cpp" ";\n")
file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/config-hard.ini.cpp" ";\n")

add_executable(tt WIN32 ${SOURCES})

target_link_libraries(tt afutil Rocket poly2tri log4cplus
                               luabind
                               lua
                               Box2D)

if (NOT WIN32)
    target_link_libraries(tt ${X11_LIBRARIES} ${X11_Xxf86vm_LIB} rt dl)

    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
    configure_file(config.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config.ini COPYONLY)
    configure_file(config-easy.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config-easy.ini COPYONLY)
    configure_file(config-hard.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config-hard.ini COPYONLY)
    if (USE_STEAM_API)
        configure_file(steam_appid.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt COPYONLY)
    endif ()
endif ()

if (USE_STEAM_API)
    target_link_libraries(tt ${STEAM_API_LIBRARIES})
endif ()

install(
    TARGETS tt
    RUNTIME DESTINATION "${INSTALL_OUT_DIR}"
)

if (INSTALL_BETA)
    file(READ "noinstall_list_beta.txt" BETA_LIST)
else ()
    file(READ "noinstall_list.txt" BETA_LIST)
endif ()

string(REGEX REPLACE ";" "\\\\;" BETA_LIST "${BETA_LIST}")
string(REGEX REPLACE "\n" ";" BETA_LIST "${BETA_LIST}")

file(GLOB_RECURSE ALL_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/assets FALSE assets/*)

list(REMOVE_ITEM ALL_LIST ${BETA_LIST})

foreach (_ENTRY ${ALL_LIST})
    get_filename_component(_NAME ${_ENTRY} NAME)
    get_filename_component(_DIR ${_ENTRY} DIRECTORY)
    get_filename_component(_EXT ${_ENTRY} EXT)

    install(
        FILES
        "assets/${_ENTRY}"
        DESTINATION "${INSTALL_OUT_DIR}/assets/${_DIR}")
    if("${_EXT}" STREQUAL ".lua")
        if (WIN32)
            foreach (_CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
                if (USE_LUAJIT)
                    install(CODE "set(ENV{LUA_PATH} ${AIRFORCE_SOURCE_DIR}/LuaJIT-2.0.4/src/?.lua)\nexecute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/luac -bg ${_ENTRY} ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/${_ENTRY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)" CONFIGURATIONS ${_CONFIGURATION})
                else ()
                    install(CODE "execute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/luac -o ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/${_ENTRY} ${_ENTRY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)" CONFIGURATIONS ${_CONFIGURATION})
                endif ()
            endforeach()
        else()
            if (USE_LUAJIT)
                install(CODE "set(ENV{LUA_PATH} ${AIRFORCE_SOURCE_DIR}/LuaJIT-2.0.4/src/?.lua)\nexecute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luac -bg ${_ENTRY} ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/${_ENTRY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)")
            else ()
                install(CODE "execute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luac -o ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/${_ENTRY} ${_ENTRY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)")
            endif ()
        endif()
    endif()
endforeach ()

if (WIN32)
    if (INSTALL_BETA)
        foreach (_CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
            if (USE_LUAJIT)
                install(CODE "set(ENV{LUA_PATH} ${AIRFORCE_SOURCE_DIR}/LuaJIT-2.0.4/src/?.lua)\nexecute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/luac -bg modules/startup_beta.lua ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/modules/startup.lua WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)" CONFIGURATIONS ${_CONFIGURATION})
            else ()
                install(CODE "execute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_CONFIGURATION}/luac -o ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/modules/startup.lua modules/startup_beta.lua WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)" CONFIGURATIONS ${_CONFIGURATION})
            endif ()
        endforeach()
    endif ()
    install(
        FILES
        "${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/OpenAL32.dll"
        "${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/wrap_oal.dll"
        DESTINATION "${INSTALL_OUT_DIR}")
    if (USE_STEAM_API)
        install(
            FILES
            "${AIRFORCE_SOURCE_DIR}/lib/${PLATFORM_NAME}/steam_api${STEAM_API_SUFFIX}.dll"
            DESTINATION "${INSTALL_OUT_DIR}")
    endif ()
else()
    if (USE_STEAM_API)
        install(
            PROGRAMS
            "${STEAM_API_LIBRARIES}"
            DESTINATION "${INSTALL_OUT_DIR}")
    endif ()
    if (INSTALL_BETA)
        if (USE_LUAJIT)
            install(CODE "set(ENV{LUA_PATH} ${AIRFORCE_SOURCE_DIR}/LuaJIT-2.0.4/src/?.lua)\nexecute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luac -bg modules/startup_beta.lua ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/modules/startup.lua WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)")
        else ()
            install(CODE "execute_process(COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luac -o ${CMAKE_INSTALL_PREFIX}/${INSTALL_OUT_DIR}/assets/modules/startup.lua modules/startup_beta.lua WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)")
        endif ()
    endif ()
    install(
        FILES
        "tt.png"
        DESTINATION "${INSTALL_OUT_DIR}")
endif()
