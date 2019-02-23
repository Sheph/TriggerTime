--- Tweening class.
-- This class serves as a base class for all types of tweenings. Tweenings are
-- used to interpolate between set of numeric values during some period of time.
--
-- @classmod Tweening

--- Get interpolated value for given time.
-- @number timeVal Time, in seconds
-- @treturn number Interpolated value
function Tweening:getValue(timeVal)
end

--- Is tweening finished for given time.
-- @number timeVal Time, in seconds
-- @treturn bool Is tweening finished
function Tweening:finished(timeVal)
end

--- Tweening duration, in seconds.
-- @tfield number duration

--- Is loop tweening.
-- If true then @{finished} never returns true and @{getValue} returns
-- values in a loop.
-- @tfield bool loop
