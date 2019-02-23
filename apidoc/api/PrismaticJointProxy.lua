--- Prismatic joint proxy class.
-- This class is derived from @{JointProxy}.
--
-- @classmod PrismaticJointProxy

--- Get joint translation.
-- @treturn number
function PrismaticJointProxy:getJointTranslation()
end

--- Get joint speed.
-- @treturn number
function PrismaticJointProxy:getJointSpeed()
end

--- Set limits.
-- @number lower
-- @number upper
function PrismaticJointProxy:setLimits(lower, upper)
end

--- Get motor force.
-- @number inv_dt
-- @treturn number
function PrismaticJointProxy:getMotorForce(inv_dt)
end

--- Local anchor A.
-- @tfield vec2 localAnchorA

--- Local anchor B.
-- @tfield vec2 localAnchorB

--- Local axis A.
-- @tfield vec2 localAxisA

--- Reference angle.
-- @tfield number referenceAngle

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

--- Max motor force.
-- @tfield number maxMotorForce
