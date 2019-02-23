--- Path iterator class.
-- This class is used to traverse @{Path} objects. Example:
--
--    local it = path:first();
--    local dend = path:dend();
--    local dist = 0;
--
--    while it:less(dend) do
--        print("distance = "..dist..", pos = "..tostring(it.current));
--        it:step(0.5);
--        dist = dist + 0.5;
--    end
--
-- @classmod PathIterator

--- Get an iterator that's at exactly the same position as this one.
-- @treturn PathIterator Iterator object
function PathIterator:clone()
end

--- Is this iterator positioned before another.
-- @tparam PathIterator other The other iterator
-- @treturn bool
function PathIterator:less(other)
end

--- Is this iterator positioned exactly as another.
-- @tparam PathIterator other The other iterator
-- @treturn bool
function PathIterator:eq(other)
end

--- Is this iterator positioned before or exactly as another.
-- @tparam PathIterator other The other iterator
-- @treturn bool
function PathIterator:lessEq(other)
end

--- Is this iterator positioned after another.
-- @tparam PathIterator other The other iterator
-- @treturn bool
function PathIterator:gt(other)
end

--- Is this iterator positioned after or exactly as another.
-- @tparam PathIterator other The other iterator
-- @treturn bool
function PathIterator:gtEq(other)
end

--- Move the iterator along the path by some value.
-- @number length Move distance, if < 0 then move is in reverse
function PathIterator:step(length)
end

--- Current position.
-- @tfield vec2 current

--- Should this iterator loop.
-- When true, iterator never reaches @{Path:dend}, it jumps to @{Path:first}
-- once it reaches @{Path:dend}.
-- @tfield bool loop
