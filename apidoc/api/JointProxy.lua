--- Joint proxy class.
-- This is base class for all types of joint proxies. We say "proxy" because
-- this class does not represent the joint between objects itself, it only
-- provides means to control the joint, when it makes sense.
--
-- Once the joint proxy is obtained it can be used to manipulate the joint,
-- however, actual joint might get destroyed any time (e.g. one of the objects
-- taking part in the joint got removed from the scene), in that case all of
-- joint proxy methods / properties will become no-op, return 0, etc.
-- @{JointProxy:valid} can be used to find out if actual joint is
-- still alive.
--
-- @classmod JointProxy

--- Check if actual joint is still alive.
-- @treturn bool
function JointProxy:valid()
end

--- Remove the joint from the scene.
-- This also automatically marks the proxy as invalid.
function JointProxy:remove()
end

--- Get anchor A.
-- @treturn vec2
function JointProxy:getAnchorA()
end

--- Get anchor B.
-- @treturn vec2
function JointProxy:getAnchorB()
end

--- Get reaction force.
-- @treturn vec2
function JointProxy:getReactionForce()
end

--- Get reaction torque.
-- @treturn number
function JointProxy:getReactionTorque()
end

--- Is joint active.
-- Joint is active when both objects in the joint are active.
-- @treturn bool
function JointProxy:active()
end

--- Should the objects in this joint collide with each other.
-- @treturn bool
function JointProxy:collideConnected()
end

--- Joint name.
-- @tfield string name

--- First object in this joint.
-- @tfield SceneObject objectA

--- Second object in this joint.
-- @tfield SceneObject objectB
