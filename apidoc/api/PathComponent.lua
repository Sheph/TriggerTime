--- Path component class.
-- This class serves as a container for @{Path}. It's usually created from
-- RUBE level data.
-- @classmod PathComponent

--- Stored @{Path} object.
-- Note that this path is in object-local coordinates, i.e. if you want to
-- obtain world coordinates for this path you need to apply self.parent
-- @{transform} to it.
-- @tfield Path path
