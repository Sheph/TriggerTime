--- Sequential tweening class.
-- This class is derived from @{Tweening}. It combines several @{Tweening}
-- instances into a single tweening.
--
-- @classmod SequentialTweening

--- SequentialTweening constructor.
-- @bool loop Is loop tweening
-- @usage local t1 = SingleTweening(3, const.EaseLinear, 0, 10, false);
-- local t2 = SingleTweening(2, const.EaseInQuad, 10, 20, false);
-- local t = SequentialTweening(false);
-- t:addTweening(t1);
-- t:addTweening(t2);
-- @function SequentialTweening
function SequentialTweening:__init(loop)
end

--- Add another tweening to sequence.
-- @tparam Tweening tweening
function SequentialTweening:addTweening(tweening)
end
