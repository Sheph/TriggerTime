--- Dialog component class.
-- This is the base class for all in-game dialogs. If you want to
-- create an in-game dialog derive from this class, create an instance and
-- add it to @{SceneObject}. Example:
--
--    class 'MyDialog' (DialogComponent)
--
--    function MyDialog:__init()
--        DialogComponent.__init(self, self, vec2(scene.gameWidth / 2 - 15, 1), 30, 8);
--        self.i = 0;
--    end
--
--    function MyDialog:onRegister()
--        self:setTitlePlayer();
--        self:setMessage("Hi, Jake");
--    end
--
--    function MyDialog:onUnregister()
--    end
--
--    function MyDialog:okPressed()
--        if self.i == 0 then
--            self:setTitleAlly("Jake", "common1/portrait_jake.png");
--            self:setMessage("Hi, player");
--        elseif self.i == 1 then
--            self:setTitlePlayer();
--            self:setMessage("Bye");
--        else
--            self:endDialog();
--        end
--        self.i = self.i + 1;
--    end
--
--    local dialog = SceneObject();
--    dialog:addComponent(MyDialog());
--    scene:addObject(dialog);
--
-- @classmod DialogComponent

--- DialogComponent constructor.
-- @param self
-- @param self
-- @tparam vec2 pos Dialog center position
-- @number width Dialog width, in units
-- @number height Dialog height, in units
-- @function DialogComponent
function DialogComponent:__init(self, self, pos, width, height)
end

--- Change dialog title and avatar to player.
function DialogComponent:setTitlePlayer()
end

--- Change dialog title and avatar to an ally.
-- @string name Ally name
-- @string image Ally avatar path
function DialogComponent:setTitleAlly(name, image)
end

--- Change dialog title and avatar to an enemy.
-- @string name Enemy name
-- @string image Enemy avatar path
function DialogComponent:setTitleEnemy(name, image)
end

--- Set dialog message.
-- @string message
function DialogComponent:setMessage(message)
end

--- End the dialog.
-- Shorthand for:
--    self.parent:removeFromParent();
function DialogComponent:endDialog()
end

--- Called when user presses "ok" in the dialog.
function DialogComponent:okPressed()
end

--- Allow fast completions.
-- When true the user doesn't have to wait until each dialog message is
-- typed, he can press "ok" and skip ahead.
-- @tfield[opt=true] bool fastComplete
