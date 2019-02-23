--- Scene object class.
-- See @{02-object-model.md|Object and component model} for more details.
--
-- @classmod SceneObject

--- SceneObject constructor.
-- This creates an object with (0, 0) position, 0 velocity, etc. One must make
-- appropriate method calls to set these up.
-- @usage local obj = SceneObject();
-- @function SceneObject
function SceneObject:__init()
end

--- Get the scene this object belongs to.
-- @treturn ?Scene The scene
function SceneObject:scene()
end

--- Get all nested objects.
-- @treturn {SceneObject} Array of objects
function SceneObject:getObjects()
end

--- Get all nested objects with specified name.
-- @string name
-- @treturn {SceneObject} Array of objects
function SceneObject:getObjects(name)
end

--- Add a component to the object.
-- @tparam Component c Component to add
-- @see Component:onRegister
function SceneObject:addComponent(c)
end

--- Remove a component from the object.
-- @tparam Component c Component to remove
-- @see Component:onUnregister
function SceneObject:removeComponent(c)
end

--- Remove this object from parent object.
-- If this object is not a nested object, i.e. it was added directly to
-- the @{Scene} then it's removed from the @{Scene}.
-- @see Scene:removeObject
-- @see Component:onUnregister
function SceneObject:removeFromParent()
end

--- Get object transform.
-- @treturn transform Object transform
function SceneObject:getTransform()
end

--- Set object transform.
-- @tparam transform t Object transform
function SceneObject:setTransform(t)
end

--- Set object transform.
-- @tparam vec2 pos Object position
-- @number angle Object angle
function SceneObject:setTransform(pos, angle)
end

--- Set object transform recursively.
-- Sets transform for this object and for all nested objects.
-- @tparam transform t Object transform
function SceneObject:setTransformRecursive(t)
end

--- Set object transform recursively.
-- Sets transform for this object and for all nested objects.
-- @tparam vec2 pos Object position
-- @number angle Object angle
function SceneObject:setTransformRecursive(pos, angle)
end

--- Set object position recursively.
-- Sets position for this object and for all nested objects.
-- @tparam vec2 pos Object position
function SceneObject:setPosRecursive(pos)
end

--- Set object angle recursively.
-- Sets angle for this object and for all nested objects.
-- @number angle Object angle
function SceneObject:setAngleRecursive(angle)
end

--- Rotate this object around some point.
-- @tparam vec2 pos Rotate the object around this point
-- @number angle Amount of rotation
function SceneObject:rotateAroundBy(pos, angle)
end

--- Rotate this object around some point recursively.
-- Same as @{rotateAroundBy}, but for all nested objects as well.
-- @tparam vec2 pos Rotate the object around this point
-- @number angle Amount of rotation
function SceneObject:rotateAroundByRecursive(pos, angle)
end

--- Apply force to this object.
-- @tparam vec2 force Force vector
-- @tparam vec2 point Point of application
-- @bool wake Wake up object, should be true most of the times
function SceneObject:applyForce(force, point, wake)
end

--- Apply force to center of this object.
-- @tparam vec2 force Force vector
-- @bool wake Wake up object, should be true most of the times
function SceneObject:applyForceToCenter(force, wake)
end

--- Apply torque to this object.
-- @number torque Torque to apply
-- @bool wake Wake up object, should be true most of the times
function SceneObject:applyTorque(torque, wake)
end

--- Apply linear impulse to this object.
-- @tparam vec2 impulse Impulse vector
-- @tparam vec2 point Point of application
-- @bool wake Wake up object, should be true most of the times
function SceneObject:applyLinearImpulse(impulse, point, wake)
end

--- Apply angular impulse to this object.
-- @number impulse Angular impulse to apply
-- @bool wake Wake up object, should be true most of the times
function SceneObject:applyAngularImpulse(impulse, wake)
end

--- Convert a point in object-local coordinates to world coordinates.
-- @tparam vec2 localPoint object-local point
-- @treturn vec2 world point
function SceneObject:getWorldPoint(localPoint)
end

--- Convert a point in world coordinates to object-local coordinates.
-- @tparam vec2 worldPoint world point
-- @treturn vec2 object-local point
function SceneObject:getLocalPoint(worldPoint)
end

--- Get an object direction vector with custom length.
-- @number length Desired vector length
-- @treturn vec2 Object direction vector
function SceneObject:getDirection(length)
end

--- Change object life by value.
-- i.e.
--    self.life = self.life + value.
-- @number value Change life by this value, can be both positive and negative
function SceneObject:changeLife(value)
end

--- Check if object is alive.
-- i.e.
--    self.life > 0
-- @treturn bool Is alive
function SceneObject:alive()
end

--- Check if object is not alive.
-- @treturn bool Is not alive
function SceneObject:dead()
end

--- Get object life in percents.
-- @treturn number Object life in range [0, 100]
function SceneObject:lifePercent()
end

--- Get object's @{CameraComponent}.
-- @treturn ?CameraComponent
function SceneObject:findCameraComponent()
end

--- Get object's @{RenderBackgroundComponent}.
-- @treturn ?RenderBackgroundComponent
function SceneObject:findRenderBackgroundComponent()
end

--- Get object's @{CollisionSensorComponent}.
-- @treturn ?CollisionSensorComponent
function SceneObject:findCollisionSensorComponent()
end

--- Get object's @{ShroomGuardianComponent}.
-- @treturn ?ShroomGuardianComponent
function SceneObject:findShroomGuardianComponent()
end

--- Get object's @{PathComponent}.
-- @treturn ?PathComponent
function SceneObject:findPathComponent()
end

--- Get object's @{PhysicsBodyComponent}.
-- @treturn ?PhysicsBodyComponent
function SceneObject:findPhysicsBodyComponent()
end

--- Get object's @{TetrobotComponent}.
-- @treturn ?TetrobotComponent
function SceneObject:findTetrobotComponent()
end

--- Get object's @{TetrocrabComponent}.
-- @treturn ?TetrocrabComponent
function SceneObject:findTetrocrabComponent()
end

--- Get object's @{LightComponent}.
-- @treturn ?LightComponent
function SceneObject:findLightComponent()
end

--- Unique identifier.
-- Each object in the game has an unique identifier called cookie.
-- @tfield int cookie

--- Object Name.
-- May be non-unique.
-- @tfield string name

--- Object type.
-- e.g. @{global|const.SceneObjectTypeRock}
-- @tfield int type

--- Object position.
-- @tfield vec2 pos

--- Object angle, in radians.
-- @tfield number angle

--- Object center of mass in world coordinates.
-- @tfield vec2 worldCenter

--- Object center of mass in object-local coordinates.
-- @tfield vec2 localCenter

--- Object mass.
-- @tfield number mass

--- Object intertia.
-- @tfield number intertia

--- Object linear velocity.
-- @tfield vec2 linearVelocity

--- Object angular velocity.
-- @tfield number angularVelocity

--- Object linear damping.
-- @tfield number linearDamping

--- Object angular damping.
-- @tfield number angularDamping

--- Bullet mode enabled.
-- Bullet mode should be enabled for very fast moving objects.
-- @tfield bool bullet

--- Is object physically active.
-- @tfield bool active

--- Object life. This is just a value associated with an object, e.g. setting it
-- to <= 0 doesn't mean anything, i.e. object is not going to be removed from
-- the scene or anything like that.
-- @tfield number life

--- Object maximum allowed life.
-- Used in @{lifePercent} to calculate percentage.
-- @tfield number maxLife

--- Object collision filter.
-- @tfield ?CollisionFilter collisionFilter

--- Object is visible.
-- Simply goes through all @{RenderComponent}s and gets/sets visible property.
-- @tfield bool visible
