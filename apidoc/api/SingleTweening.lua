--- Single tweening class.
-- This class is derived from @{Tweening}. Single tweening interpolates
-- between 2 values using specified [easing](http://easings.net).
--
-- @classmod SingleTweening

--- SingleTweening constructor.
-- @number duration Tweening duration, in seconds
-- @int easing Easing, e.g. @{global|const.EaseInQuad}
-- @number[opt=0] start Start value
-- @number[optchain=1] tend End value
-- @bool loop Is loop tweening
-- @usage local t = SingleTweening(3, const.EaseLinear, 0, 10, false);
-- @function SingleTweening
function SingleTweening:__init(duration, easing, start, tend, loop)
end

--- Start value.
-- @treturn number
function SingleTweening:start()
end

--- End value.
-- @treturn number
function SingleTweening:tend()
end
