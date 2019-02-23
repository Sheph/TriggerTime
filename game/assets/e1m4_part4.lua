local conveyor4Spawn = scene:getObjects("conveyor4_spawn");
local conveyor5Spawn = scene:getObjects("conveyor5_spawn");
local press9j = scene:getJoints("press9_joint")[1];
local press9cp = scene:getObjects("press9_cp")[1];
local laser5 = scene:getObjects("laser5");
local blueStone = scene:getObjects("blue_stone")[1];
local blueStoneObj = findObject(scene:instanciate("e1m4_blue_stone.json", blueStone:getTransform()).objects, "blue_stone");
local blueStoneLight = scene:getObjects("terrain1")[1]:findLightComponent():getLights("blue_stone_light")[1];
local press9Light = scene:getObjects("terrain1")[1]:findLightComponent():getLights("press9_light")[1];

-- main

scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent().timeout = 30.0;
scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():addHintTrigger(scene:getObjects("ha2_target")[1].pos, 1.0);
scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha2_hint")[1].pos);

if settings.skill <= const.SkillEasy then
    scene:getObjects("ha2")[1].active = true;
end

makeAirlock("door29", false);

setSensorEnterListener("conveyor4_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("conveyor4_cp")[1]:getTransform();
end);

makeConveyor2("conveyor4", 9.5, function(obj)
    if obj.name == "robopart" then
        obj.name = "robopart2";
    end
    return true;
end, function(obj)
    if obj.name == "robopart2" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(1.25, function(cookie)
    if stopAllConveyors then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor4Spawn[math.random(1, #conveyor4Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    scene:instanciate("e1m4_robopart"..math.random(9, 14)..".json", xf);
end);

makeConveyor("conveyor5", 7.0, function(obj)
    if obj.name == "robopart2" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(2.0, function(cookie)
    if stopAllConveyors then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor5Spawn[math.random(1, #conveyor5Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    scene:instanciate("e1m4_robopart"..math.random(9, 14)..".json", xf);
end);

local laser4 = scene:getObjects("laser4");
for _, obj in pairs(laser4) do
    -- very dirty :(
    obj:findPhysicsBodyComponent():setFilterCategoryBits(const.collisionBitCustom2);
end

press9cp.objs = {};
press9j.myMotorSpeed = math.abs(press9j.motorSpeed);
setSensorListener("press9_cp", function(other, args)
    if press9cp.objs[other.cookie] == nil then
        press9cp.objs[other.cookie] = { count = 1, obj = other };
        if table.size(press9cp.objs) > 0 then
            press9j.motorSpeed = -press9j.myMotorSpeed;
            press9Light.visible = true;
        end
    else
        press9cp.objs[other.cookie].count = press9cp.objs[other.cookie].count + 1;
    end
end, function (other, args)
    press9cp.objs[other.cookie].count = press9cp.objs[other.cookie].count - 1;
    if press9cp.objs[other.cookie].count == 0 then
        press9cp.objs[other.cookie] = nil;
        if table.size(press9cp.objs) <= 0 then
            press9j.motorSpeed = press9j.myMotorSpeed / 3;
            press9Light.visible = false;
        end
    end
end);

setSensorEnterListener("blue_stone_die_cp", false, function(other)
    if other == blueStoneObj then
        other:addComponent(FadeOutComponent(1.0));
        blueStoneObj = findObject(scene:instanciate("e1m4_blue_stone.json", blueStone:getTransform()).objects, "blue_stone");
    end
end);

setSensorEnterListener("blue_stone_cp", false, function(other)
    if other ~= blueStoneObj then
        return;
    end
    blueStoneLight.visible = true;
    other:addComponent(FadeOutComponent(1.0));
    for _, obj in pairs(laser5) do
        obj:addComponent(FadeOutComponent(2.0));
    end
    audio:playSound("alarm_off.ogg");
    openAirlock("door13", false);
    scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():removeAllHints();
end);
