--- @{SceneObject} factory class.
-- Creates different kinds of in-game objects such as enemies, obstacles, etc.
-- It's strongly advised to use level editor for this, this class should only
-- be used when dynamic runtime creation of objects is required.
--
-- @classmod SceneObjectFactory

--- Create dummy kinematic object.
-- Useful when you want an object that obeys kinematic
-- rules (i.e. respects linear and angular velocities) but does not interact
-- with other objects.
-- @treturn SceneObject Dummy object
function SceneObjectFactory:createDummy()
end

--- Create background/parallax object.
-- It's used for rendering and controlling background/parallax.
-- @string image Background image path
-- @number width Image tile width, in units
-- @number height Image tile height, in units
-- @tparam vec2 velocity Parallax velocity relative to camera velocity
-- @int zOrder Render z-order
-- @treturn SceneObject Background object
function SceneObjectFactory:createBackground(image, width, height, velocity, zOrder)
end

--- Create explosion object using explosion1.p particle effect.
-- @int zOrder Render z-order
-- @treturn SceneObject Explosion object
function SceneObjectFactory:createExplosion1(zOrder)
end

--- Create blocker object.
-- @number width Blocker width
-- @number height Blocker height
-- @treturn SceneObject Blocker object
function SceneObjectFactory:createBlocker(width, height)
end

--- Create tetrobot enemy.
-- @treturn SceneObject Tetrobot object
function SceneObjectFactory:createTetrobot()
end

--- Create tetrocrab enemy.
-- @treturn SceneObject Tetrocrab object
function SceneObjectFactory:createTetrocrab()
end
