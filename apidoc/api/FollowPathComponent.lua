--- Path following component class.
-- This class provides means to move an object along a @{Path}.
--
-- @classmod FollowPathComponent

--- FollowPathComponent constructor.
-- @function FollowPathComponent
function FollowPathComponent:__init()
end

--- Object reached the end of path.
-- @tfield bool finished

--- Current time value.
-- @tfield number time

--- Path to follow.
-- @tfield Path path

--- Transform to apply to path vertices.
-- @tfield transform pathTransform

--- Tweening to use.
-- @tfield Tweening tweening
