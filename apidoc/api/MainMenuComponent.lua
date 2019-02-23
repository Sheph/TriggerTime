--- Main menu component class.
-- This is the base class for main menu. If you want to
-- create a main menu derive from this class, create an instance and
-- add it to @{SceneObject}. Example:
--
--    class 'MyMainMenu' (MainMenuComponent)
--
--    function MyMainMenu:__init()
--        MainMenuComponent.__init(self, self);
--    end
--
--    function MyMainMenu:onRegister()
--    end
--
--    function MyMainMenu:onUnregister()
--    end
--
--    function MyMainMenu:onPressed(i)
--        if i == 0 then
--            print("play");
--        else
--            print("score");
--        end
--    end
--
--    local obj = SceneObject();
--    local c = MyMainMenu();
--
--    c:addItem("common1/play.png");
--    c:addItem("common1/score.png");
--
--    obj:addComponent(c);
--    scene:addObject(obj);
--
-- @classmod MainMenuComponent

--- MainMenuComponent constructor.
-- @param self
-- @param self
-- @function MainMenuComponent
function MainMenuComponent:__init(self, self)
end

--- Add menu item.
-- @string image Image path
function MainMenuComponent:addItem(image)
end

--- Called when i'th item is pressed.
-- @int i Menu item pressed
function MainMenuComponent:onPressed(i)
end
