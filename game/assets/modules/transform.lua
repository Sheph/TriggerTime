--- 2D transform class.
-- Just a @{vec2} and @{rot} pair.
-- @classmod transform

transform = {}
transform.__index = transform

function transform.__eq(a, b)
    return a.p == b.p and a.q == b.q
end

--- Convert transform to "((x, y), angle)" string.
-- @tparam transform a
-- @treturn string
function transform.__tostring(a)
    return "(" .. tostring(a.p) .. ", " .. a.q:getAngle() .. ")"
end

--- transform constructor.
-- @tparam vec2 p Position
-- @tparam rot q Rotation
-- @usage local t = transform(vec2(0.1, 0.5), rot(math.pi));
-- @function transform
function transform.new(p, q)
    return setmetatable({ p = p, q = q }, transform)
end

--- Clone the transform.
-- @treturn transform
function transform:clone()
    return transform.new(self.p, self.q)
end

--- Unpack the transform.
-- @treturn vec2
-- @treturn rot
function transform:unpack()
    return self.p, self.q
end

--- Position.
-- @tfield vec2 p

--- Rotation.
-- @tfield rot q

setmetatable(transform, { __call = function(_, ...) return transform.new(...) end })
