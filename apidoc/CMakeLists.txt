set(SOURCES
    config.ld
    manual/01-introduction.md
    manual/02-object-model.md
    manual/03-level-handling.md
    api/Scene.lua
    api/SceneObject.lua
    api/Component.lua
    api/CameraComponent.lua
    api/CollisionSensorComponent.lua
    api/PathComponent.lua
    api/RenderBackgroundComponent.lua
    api/ShroomGuardianComponent.lua
    api/CollisionFilter.lua
    api/SceneObjectFactory.lua
    api/Settings.lua
    api/SensorListener.lua
    api/Path.lua
    api/PathIterator.lua
    api/JointProxy.lua
    api/RevoluteJointProxy.lua
    api/PrismaticJointProxy.lua
    api/DistanceJointProxy.lua
    api/PulleyJointProxy.lua
    api/WheelJointProxy.lua
    api/WeldJointProxy.lua
    api/FrictionJointProxy.lua
    api/RopeJointProxy.lua
    api/GearJointProxy.lua
    api/Tweening.lua
    api/SingleTweening.lua
    api/SequentialTweening.lua
    api/FollowPathComponent.lua
    api/CollisionCookieFilter.lua
    api/DialogComponent.lua
    api/MainMenuComponent.lua
    api/PickerComponent.lua
    api/PhysicsBodyComponent.lua
    api/TetrobotComponent.lua
    api/TetrocrabComponent.lua
    api/RenderComponent.lua
    api/RenderLightComponent.lua
    api/LightComponent.lua
    api/AudioManager.lua
    api/AudioSource.lua
    api/global.lua
    ${AIRFORCE_SOURCE_DIR}/game/assets/modules/rot.lua
    ${AIRFORCE_SOURCE_DIR}/game/assets/modules/transform.lua
    ${AIRFORCE_SOURCE_DIR}/game/assets/modules/vec2.lua
    ${AIRFORCE_SOURCE_DIR}/game/assets/modules/utils.lua
)

add_custom_command(
    OUTPUT ${AIRFORCE_OUT_DIR}/doc/index.html
    COMMAND ldoc -d ${AIRFORCE_OUT_DIR}/doc .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${SOURCES}
)

add_custom_target(
    apidoc ALL
    DEPENDS ${AIRFORCE_OUT_DIR}/doc/index.html
)

get_directory_property(CLEAN_FILES ADDITIONAL_MAKE_CLEAN_FILES)
set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES
    "${CLEAN_FILES};${AIRFORCE_OUT_DIR}/doc"
)
