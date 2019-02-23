--- 2D vector class.
--
-- @classmod vec2

vec2 = {}
vec2.__index = vec2

--- Add two vectors or add a scalar to a vector.
-- @tparam vec2|number a
-- @tparam vec2|number b
-- @treturn vec2
function vec2.__add(a, b)
    if getmetatable(a) == nil then
        return vec2.new(b.x + a, b.y + a)
    elseif getmetatable(b) == nil then
        return vec2.new(a.x + b, a.y + b)
    else
        return vec2.new(a.x + b.x, a.y + b.y)
    end
end

--- Subtract two vectors or subtract a scalar from a vector.
-- @tparam vec2|number a
-- @tparam vec2|number b
-- @treturn vec2
function vec2.__sub(a, b)
    if getmetatable(a) == nil then
        return vec2.new(b.x - a, b.y - a)
    elseif getmetatable(b) == nil then
        return vec2.new(a.x - b, a.y - b)
    else
        return vec2.new(a.x - b.x, a.y - b.y)
    end
end

--- Vector dot product or multiply vector by scalar.
-- @tparam vec2|number a
-- @tparam vec2|number b
-- @treturn vec2
function vec2.__mul(a, b)
    if getmetatable(a) == nil then
        return vec2.new(b.x * a, b.y * a)
    elseif getmetatable(b) == nil then
        return vec2.new(a.x * b, a.y * b)
    else
        return vec2.new(a.x * b.x, a.y * b.y)
    end
end

--- Divide two vectors or divide vector by a scalar.
-- @tparam vec2|number a
-- @tparam vec2|number b
-- @treturn vec2
function vec2.__div(a, b)
    if getmetatable(a) == nil then
        return vec2.new(b.x / a, b.y / a)
    elseif getmetatable(b) == nil then
        return vec2.new(a.x / b, a.y / b)
    else
        return vec2.new(a.x / b.x, a.y / b.y)
    end
end

function vec2.__eq(a, b)
    return a.x == b.x and a.y == b.y
end

function vec2.__lt(a, b)
    return a.x < b.x or (a.x == b.x and a.y < b.y)
end

function vec2.__le(a, b)
    return a.x <= b.x and a.y <= b.y
end

--- Convert vector to "(x, y)" string.
-- @tparam vec2 a
-- @treturn string
function vec2.__tostring(a)
    return "(" .. a.x .. ", " .. a.y .. ")"
end

--- vec2 constructor.
-- @number x
-- @number y
-- @usage local v = vec2(0.5, 0.1);
-- @function vec2
function vec2.new(x, y)
    return setmetatable({ x = x or 0, y = y or 0 }, vec2)
end

--- Get distance between 2 points.
-- @tparam vec2 a
-- @tparam vec2 b
-- @treturn number
function vec2.distance(a, b)
    return (b - a):len()
end

--- Clone the vector.
-- @treturn vec2
function vec2:clone()
    return vec2.new(self.x, self.y)
end

--- Unpack the vector.
-- @treturn number
-- @treturn number
function vec2:unpack()
    return self.x, self.y
end

--- Vector length.
-- @treturn number
function vec2:len()
    return math.sqrt(self.x * self.x + self.y * self.y)
end

--- Vector squared length.
-- @treturn number
function vec2:lenSq()
    return self.x * self.x + self.y * self.y
end

--- Normalize vector.
-- @treturn vec2 self
function vec2:normalize()
    local len = self:len()

    self.x = self.x / len
    self.y = self.y / len

    return self
end

--- Get normalized vector.
-- @treturn vec2 Normalized vector
function vec2:normalized()
    return self / self:len()
end

--- Rotate vector.
-- @number phi Angle, in radians
-- @treturn vec2 self
function vec2:rotate(phi)
    local c = math.cos(phi)
    local s = math.sin(phi)

    local tmp = self.x;

    self.x = c * self.x - s * self.y
    self.y = s * tmp + c * self.y

    return self
end

--- Get rotated vector.
-- @number phi Angle, in radians
-- @treturn vec2 Rotated vector
function vec2:rotated(phi)
    return self:clone():rotate(phi)
end

--- Get perpendicular vector.
-- @treturn vec2
function vec2:perpendicular()
    return vec2.new(-self.y, self.x)
end

--- Get vector cross product.
-- @treturn vec2
function vec2:cross(other)
    return self.x * other.y - self.y * other.x
end

--- Convert vector to angle.
-- @treturn number
function vec2:angle()
    return math.atan2(self.y, self.x);
end

--- X coordinate.
-- @tfield number x

--- Y coordinate.
-- @tfield number y

setmetatable(vec2, { __call = function(_, ...) return vec2.new(...) end })
