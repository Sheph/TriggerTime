--- Wheel joint proxy class.
-- This class is derived from @{JointProxy}.
--
-- @classmod WheelJointProxy

--- Get joint translation.
-- @treturn number
function WheelJointProxy:getJointTranslation()
end

--- Get joint speed.
-- @treturn number
function WheelJointProxy:getJointSpeed()
end

--- Get motor torque.
-- @number inv_dt
-- @treturn number
function WheelJointProxy:getMotorTorque(inv_dt)
end

--- Local anchor A.
-- @tfield vec2 localAnchorA

--- Local anchor B.
-- @tfield vec2 localAnchorB

--- Local axis A.
-- @tfield vec2 localAxisA

--- Motor enabled.
-- @tfield bool motorEnabled

--- Motor speed.
-- @tfield number motorSpeed

--- Max motor torque.
-- @tfield number maxMotorTorque

--- Spring frequency HZ.
-- @tfield number springFrequencyHz

--- Spring damping ratio.
-- @tfield number springDampingRatio
