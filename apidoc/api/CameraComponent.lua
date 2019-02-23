--- Camera component class.
-- Camera defines a region in a @{Scene} that's visible to the user. Camera
-- can be attached to a @{SceneObject}, so that it follows it wherever it goes.
-- @classmod CameraComponent

--- Attaches the camera to some @{SceneObject}.
-- @tparam SceneObject obj
function CameraComponent:setTarget(obj)
end

--- Detaches the camera from @{SceneObject}.
function CameraComponent:resetTarget()
end
