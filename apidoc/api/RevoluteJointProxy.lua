--- Revolute joint proxy class.
-- This class is derived from @{JointProxy}.
--
-- @classmod RevoluteJointProxy

--- Set limits.
-- @number lower
-- @number upper
function RevoluteJointProxy:setLimits(lower, upper)
end

--- Get motor torque.
-- @number inv_dt
-- @treturn number
function RevoluteJointProxy:getMotorTorque(inv_dt)
end

--- Local anchor A.
-- @tfield vec2 localAnchorA

--- Local anchor B.
-- @tfield vec2 localAnchorB

--- Reference angle.
-- @tfield number referenceAngle

--- Joint angle.
-- @tfield number jointAngle

--- Joint speed.
-- @tfield number localAnchorA

--- Limit enabled.
-- @tfield bool limitEnabled

--- Lower limit.
-- @tfield number lowerLimit

--- Upper limit.
-- @tfield number upperLimit

--- Motor enabled.
-- @tfield bool motorEnabled

--- Motor speed.
-- @tfield number motorSpeed

--- Max motor torque.
-- @tfield number maxMotorTorque
