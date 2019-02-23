--- Audio management class.
-- Allows playing sound effects and music.
--
-- @classmod AudioManager

--- Create sound audio source.
-- @string name Path to audio file
-- @treturn AudioSource Audio source object
function AudioManager:createSound(name)
end

--- Play sound file once.
-- Same as @{createSound} + @{AudioSource.play}
-- @string name Path to audio file
function AudioManager:playSound(name)
end

--- Create streaming audio source.
-- Unlike sound audio sources streaming audio sources are meant
-- to be played during long period of time, i.e. this is for
-- things such as background music.
-- @string name Path to audio file
-- @treturn AudioSource Audio source object
function AudioManager:createStream(name)
end

--- Master volume for the game.
-- @tfield float volume
