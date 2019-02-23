--- Light render component class.
-- Used for lights rendering and controlling.
-- This class is derived from @{RenderComponent}.
-- @classmod RenderLightComponent

--- Ambient light.
-- Color used for global illumination.
-- @tfield tab ambientLight

--- Number of blur passes to make.
-- Number of gaussian blur passes to make, 0 - none.
-- @tfield int numBlur

--- Is blur enabled.
-- i.e. is @{numBlur} > 0.
-- @tfield bool blur

--- Is gamma correction enabled.
-- @tfield bool gammaCorrection
