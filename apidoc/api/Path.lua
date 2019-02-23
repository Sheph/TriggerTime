--- Path class.
-- This class represents a curved path, e.g.
-- [cubic hermite path](https://en.wikipedia.org/wiki/Cubic_Hermite_spline#/media/File:Finite_difference_spline_example.png). Path always
-- has first point and last point. First point can be equal to last point, in that case path is looped.
-- @{PathIterator} can be used to traverse the path.
--
-- @classmod Path

--- Get an iterator that's positioned at first point.
-- @treturn PathIterator Iterator object
function Path:first()
end

--- Get an iterator that's positioned at last point.
-- @treturn PathIterator Iterator object
function Path:last()
end

--- Get an iterator that's positioned right past the last point.
-- @treturn PathIterator Iterator object
function Path:dend()
end

--- Get an iterator that's positioned right before the first point.
-- @treturn PathIterator Iterator object
function Path:rend()
end

--- Get an iterator that's position somewhere on the path.
-- @number pos Specifies the distance from first point.
--
-- * If pos is < 0 - @{Path:rend} iterator is returned
-- * If pos is greater or equal to @{Path.length} - @{Path:dend} iterator is returned
-- @treturn PathIterator Itertor object
function Path:find(pos)
end

--- Path length.
-- @tfield number length

--- Array of path's control points.
-- @tfield {vec2} points
