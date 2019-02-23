## Level handling

### Level loading

In Air Force level loading is done in 4 phases:

* New `Scene` object is created. The newly created `Scene` doesn't contain
any objects, joints, etc.
* Level \*.json file (exported from RUBE editor) is processed and `Scene`
is populated with objects, joints, etc.
* Level script runs. The script can see all the objects, joints, etc. It can
setup things such as `SensorListener`s, call `Scene:addTimer` to setup initial
timers, etc.
* Game enters `Scene` update loop, i.e. all objects and components are
continuously updated
as described in [object and component model](02-object-model.md.html#Component_phases).

### Simple level logic example

Suppose you've created a level in RUBE that contains:

* A sensor object named "my_sensor".
* A player respawn point that's near (but not inside) the sensor.
* A rock object named "my_rock".

Now let's write a simple script for this level. We're goint to
implement the following logic:

* Initially rock is not joined with anything on level, thus, it'll float
when being shot at or collided with something.
* When player enters the sensor rock is welded in place.
* When player exits the sensor rock weld is removed and it can float again.

First, lets declare sensor listener class and write a constructor:

    class 'MyListener' (SensorListener)

    function MyListener:__init(sensor, rock)
        SensorListener.__init(self, self);
        self.sensor = sensor;
        self.rock = rock;
        self.j = nil;
    end

The class is derived from `SensorListener`, which is the base class for all
sensor listeners. It also has 3 fields: sensor, rock and j. We're going to use
them to store references to the sensor object, rock object and the joint we're
goint to create later.

Now, let's lookup the sensor:

    local obj = scene:getObjects("my_sensor")[1];

Since we only have one sensor named "my_sensor" on the level we need to
take the first and only object in the returned array. Now, we need to lookup
`CollisionSensorComponent`:

    local c = obj:findCollisionSensorComponent();

This component is responsible for handling sensors. Next, we're going to
create an instance of MyListener class and pass it to `CollisionSensorComponent`:

    c.listener = MyListener(obj, scene:getObjects("my_rock")[1]);

We also pass the sensor object and the rock object as arguments. Now, let's
implement sensor listener's enter handler:

    function MyListener:sensorEnter(obj)
        if obj.type == const.SceneObjectTypePlayer then
            self.j = scene:addWeldJoint(self.rock, self.sensor, self.rock.worldCenter, false);
        end
    end

First, we check that the object entering the sensor is actually a player, if
it's so we create a weld joint between the rock and the sensor using
`Scene:addWeldJoint` method. We're using the sensor object in this case for
simplicity, it could have been any other object. And since we want to weld the
rock in-place we're using rock's center of mass as an anchor. We also save
the returned joint proxy as self.j.

Now let's write sensor listener's exit handler:

    function MyListener:sensorExit(obj)
        if obj.type == const.SceneObjectTypePlayer then
            self.j:remove();
        end
    end

Here we also check that the object exiting the sensor is a player, if it's so
we remove the joint created earlier.

If you run this level now, you'll see that when you shoot the rock it moves
because of the bullet impact, but once you enter the sensor the rock stops
moving. If you exit the sensor and shoot the rock again it moves again, etc.
