local function makeConveyor(name, velocity)
    local conveyor = scene:getObjects(name)[1];
    conveyor.deadbodyAware = true;

    local rc = conveyor:findRenderStripeComponents("belt")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "belt1", 12.5 / velocity);
    ac:startAnimation(const.AnimationDefault);
    conveyor:addComponent(ac);

    conveyor:addComponent(CollisionSensorComponent());
    conveyor.objs = {};
    setSensorListener(name, function(other, args)
        if conveyor.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            conveyor.objs[other.cookie] = { count = 1, obj = other };
        else
            conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        if conveyor.objs[other.cookie] == nil then
            return;
        end
        conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count - 1;
        if conveyor.objs[other.cookie].count == 0 then
            local obj = conveyor.objs[other.cookie].obj;
            conveyor.objs[other.cookie] = nil;
            obj.activeDeadbody = false;
        end
    end);
    addTimeout0(function(cookie, dt)
        local dir = conveyor:getDirection(velocity * dt);
        for _, v in pairs(conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
    end);
end

local data = {
    [3] = {"e1m1", 0},
    [4] = {"e1m2", 0},
    [5] = {"e1m3", 0},
    [6] = {"e1m3", 3},
    [7] = {"e1m3", 8},
    [8] = {"e1m4", 0},
    [9] = {"e1m5", 0},
    [10] = {"e1m5", 16},
    [11] = {"e1m6", 0},
    [12] = {"e1m7", 0},
    [13] = {"e1m8", 0},
    [14] = {"e1m8", 10},
    [15] = {"e1m10", 0},
    [16] = {"e1m2", 20},
    [17] = {"e1m2", 14},
    [18] = {"e1m3", 4},
    [19] = {"e1m3", 15},
    [20] = {"e1m4", 18},
    [21] = {"e1m6", 3},
    [22] = {"e1m6", 5},
    [23] = {"e1m6", 6},
    [24] = {"e1m7", 11},
    [25] = {"e1m7", 22},
    [26] = {"e1m8", 12},
    [27] = {"e1m9", 8},
    [28] = {"e1m10", 1},
    [29] = {"e1m10", 4},
};

-- main

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.5, 0.5, 0.5, 1.0};

scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1")[1].pos, scene:getObjects("constraint2")[1].pos);

makeConveyor("conveyor0", 60.0);

for idx, idxData in pairs(data) do
    setSensorEnterListener(idx.."_cp", true, function(other)
        scene.cutscene = true;
        settings:setDeveloper(idxData[2]);
        stainedGlass({0, 0, 0, 0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 0.25, function()
            stainedGlass({0, 0, 0, 1.0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 100.0);
            scene:setNextLevel(idxData[1]..".lua", idxData[1]..".json");
        end);
    end);
end
