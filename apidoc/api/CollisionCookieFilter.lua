--- Cookie-based collision filter class.
-- This class is derived from @{CollisionFilter}. It filters the collisions
-- based on set of @{SceneObject.cookie} values. Collision will happen only
-- if colliding object's cookie is not in that set.
--
-- @classmod CollisionCookieFilter

--- CollisionCookieFilter constructor.
-- @function CollisionCookieFilter
function CollisionCookieFilter:__init()
end

--- Add a @{SceneObject.cookie} to the set.
-- @int cookie @{SceneObject.cookie} value
function CollisionCookieFilter:add(cookie)
end
