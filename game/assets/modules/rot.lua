--- 2D rotation class.
--
-- @classmod rot

rot = {}
rot.__index = rot

--- Are 2 rotations equal.
-- Note that rot(a) and rot(a + 2 * math.pi) are equal.
-- @tparam rot a
-- @tparam rot b
-- @treturn bool
function rot.__eq(a, b)
    return a.s == b.s and a.c == b.c
end

--- Convert rot to "(s, c)" string.
-- @tparam rot a
-- @treturn string
function rot.__tostring(a)
    return "(" .. a.s .. ", " .. a.c .. ")"
end

--- rot constructor.
-- @number s Angle, in radians
-- @number[opt] c Cosine of angle. If it's specified, then 's' is treated as sine of angle
-- @usage local r = rot(math.pi);
-- @usage local r = rot(math.sin(math.pi), math.cos(math.pi));
-- @function rot
function rot.new(s, c)
    if c ~= nil then
        return setmetatable({ s = s, c = c }, rot)
    else
        return setmetatable({ s = math.sin(s), c = math.cos(s) }, rot)
    end
end

--- Get rot angle.
-- @treturn number Angle in range [-math.pi, math.pi]
function rot:getAngle()
    return math.atan2(self.s, self.c);
end

--- Get x-axis for this rotation.
-- @treturn vec2
function rot:getXAxis()
    return vec2(self.c, self.s);
end

--- Get y-axis for this rotation.
-- @treturn vec2
function rot:getYAxis()
    return vec2(-self.s, self.c);
end

--- Clone the rot.
-- @treturn rot
function rot:clone()
    return rot.new(self.s, self.c)
end

--- Unpack the rot.
-- @treturn number
-- @treturn number
function rot:unpack()
    return self.s, self.c
end

--- Angle sine.
-- @tfield number s

--- Angle cosine.
-- @tfield number c

setmetatable(rot, { __call = function(_, ...) return rot.new(...) end })
