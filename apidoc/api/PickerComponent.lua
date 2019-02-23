--- Picker UI component class.
-- This is the base class for picker UI. If you want to
-- create a picker UI derive from this class, create an instance and
-- add it to @{SceneObject}. Example:
--
--    class 'MyPicker' (PickerComponent)
--
--    function MyPicker:__init()
--        PickerComponent.__init(self, self);
--    end
--
--    function MyPicker:onRegister()
--    end
--
--    function MyPicker:onUnregister()
--    end
--
--    function MyPicker:onPressed(i)
--        if i < 0 then
--            print("back");
--        elseif i == 0 then
--            print("level1");
--        else
--            print("level2");
--        end
--    end
--
--    local obj = SceneObject();
--    local c = MyPicker();
--
--    c:setHeader("common1/choose_level.png");
--    c:addItem("Level1", "common1/l1.png", "best score: 100");
--    c:addItem("Level2", "common1/l2.png", "best score: 200");
--
--    obj:addComponent(c);
--    scene:addObject(obj);
--
-- @classmod PickerComponent

--- PickerComponent constructor.
-- @param self
-- @param self
-- @function PickerComponent
function PickerComponent:__init(self, self)
end

--- Set picker header image.
-- @string image Image path
function PickerComponent:setHeader(image)
end

--- Add picker item.
-- @string title Item title text
-- @string image Item image path
-- @string details Item details text
function PickerComponent:addItem(title, image, details)
end

--- Called when i'th item is pressed
-- @int i Item pressed or < 0 if "back" was pressed
function PickerComponent:onPressed(i)
end
