--- Game settings class.
-- Game settings are defined in game's config.ini file. They're read-only.
-- @classmod Settings

--- Player game settings.
-- @tfield number slowMoveSpeed Player speed when brake button is pressed
-- @tfield number fastMoveSpeed Player speed when accelerate button is pressed
-- @tfield number moveSpeed Player speed when brake and accelerate aren't pressed
-- @table Player

--- Developer mode enabled.
-- Developer mode is useful for debugging and during level design, e.g. you can
-- create a fake respawn point and place the player there only in developer
-- mode, thus, you won't have to start the level all over again each time you
-- change your level and want to give it a try.
-- Another use-case is skipping cutscenes, you can design you level in a way
-- that cutscenes are only displayed when developer mode is false.
-- @tfield bool developer

--- Player game settings.
-- @tfield Settings.Player player
