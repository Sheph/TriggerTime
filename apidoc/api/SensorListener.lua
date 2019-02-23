--- Sensor listener class.
-- This is the base class for all sensor listeners. If you want to react on
-- sensor "enter" and "exit" events derive your own class from this one, create
-- an instance of it and assign it to @{CollisionSensorComponent.listener}.
-- Example:
--
--    class 'MyListener' (SensorListener)
--
--    function MyListener:__init()
--        SensorListener.__init(self, self);
--    end
--
--    function MyListener:sensorEnter(obj)
--        print("enter");
--    end
--
--    function MyListener:sensorExit(obj)
--        print("exit");
--    end
--
--    -- Get "my_sensor" object that I've created in RUBE
--    local obj = scene:getObjects("my_sensor")[1];
--
--    -- Get the sensor component
--    local c = obj:findCollisionSensorComponent();
--
--    -- Instanciate and assign my listener
--    c.listener = MyListener();
--
-- @classmod SensorListener

--- SensorListener constructor.
-- @param self
-- @param self
-- @function SensorListener
function SensorListener:__init(self, self)
end

--- Called when some @{SceneObject} enters the sensor.
-- @tparam SceneObject obj The object that entered
function SensorListener:sensorEnter(obj)
end

--- Called when some @{SceneObject} exits the sensor.
-- If an object that entered the sensor is removed from the scene this
-- function is also called.
-- @tparam SceneObject obj The object that exited
function SensorListener:sensorExit(obj)
end
