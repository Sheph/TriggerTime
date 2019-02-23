--- Scene class.
-- See @{02-object-model.md|Object and component model} for more details.
--
-- @classmod Scene

--- Add an object to scene.
-- @tparam SceneObject obj Object to add
-- @see Component:onRegister
function Scene:addObject(obj)
end

--- Remove an object from scene.
-- An object that was added and then removed from the scene must not be added
-- again, i.e. it's illegal to call @{Scene:addObject} for it again.
-- @tparam SceneObject obj Object to remove
-- @see Component:onUnregister
function Scene:removeObject(obj)
end

--- Get all objects.
-- @treturn {SceneObject} Array of objects
function Scene:getObjects()
end

--- Get all objects with specified name.
-- @string name
-- @treturn {SceneObject} Array of objects
function Scene:getObjects(name)
end

--- Find object of specified type.
-- @tparam int type Object type, e.g. @{global|const.SceneObjectTypeRock}
-- @treturn SceneObject
function Scene:findObject(type)
end

--- Add a gear joint to the scene.
-- @tparam SceneObject objectA
-- @tparam SceneObject objectB
-- @tparam JointProxy joint1
-- @tparam JointProxy joint2
-- @number ratio
-- @bool collideConnected
-- @treturn JointProxy
function Scene:addGearJoint(objectA, objectB, joint1, joint2, ratio, collideConnected)
end

--- Add a weld joint to the scene.
-- @tparam SceneObject objectA
-- @tparam SceneObject objectB
-- @tparam vec2 worldAnchor
-- @bool collideConnected
-- @treturn JointProxy
function Scene:addWeldJoint(objectA, objectB, worldAnchor, collideConnected)
end

--- Get all joints with specific name.
-- @string name
-- @treturn {JointProxy} Array of joints
function Scene:getJoints(name)
end

--- Removes a joint from the scene.
-- @tparam JointProxy joint
-- @see JointProxy:remove
function Scene:removeJoint(joint)
end

--- Add a timer to the scene.
-- @tparam func|tab cb Timer callback, could be one of these:
--
-- * A function that takes a single argument - time delta (dt)
-- * Any entity that has an "update" member that's a function that takes a single argument - time delta (dt)
--
-- @treturn int Timer cookie that can be used to remove the timer
function Scene:addTimer(cb)
end

--- Removes a timer from the scene.
-- @tparam int cookie Timer cookie returned by @{addTimer}
function Scene:removeTimer(cookie)
end

--- Load another level.
-- Once the new level is loaded current scene and script will be replace
-- by new scene and script.
-- @string scriptPath Path to new level's script
-- @string[opt] assetPath Path to new level's \*.json
function Scene:setNextLevel(scriptPath, assetPath)
end

--- Camera object.
-- @tfield SceneObject camera

--- Player object.
-- @tfield ?SceneObject player

--- Respawn point.
-- Actually this is a @{transform} because we also need initial player angle.
-- @tfield transform respawnPoint

--- Is cutscene running.
-- @tfield bool cutscene

--- Game is quitting.
-- @tfield bool quit

--- Game width in units.
-- @tfield number gameWidth

--- Game height in units.
-- @tfield number gameHeight

--- Lighting management component.
-- @tfield RenderLightComponent lighting
