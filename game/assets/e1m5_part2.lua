local lever5def = (math.random(0, 1) == 0);
local lever6def = (math.random(0, 1) == 0);
local lever7def = (math.random(0, 1) == 0);
local doorComb = {
    {{"door10", "door11"}, {"door12", "door13"}},
    {{"door12", "door13"}, {"door10", "door11"}},
    {{"door10", "door12"}, {"door11", "door13"}},
    {{"door11", "door13"}, {"door10", "door12"}},
    {{"door10", "door13"}, {"door11", "door12"}},
    {{"door11", "door12"}, {"door10", "door13"}}
};
table.shuffle(doorComb);
local lever5doors = doorComb[1][1];
local lever6doors = doorComb[1][2];
local lever7doors = {doorComb[1][1][math.random(1, 2)], doorComb[1][2][math.random(1, 2)]};
local leverDoors = {lever5doors, lever6doors, lever7doors};
table.shuffle(leverDoors);
lever5doors = leverDoors[1];
lever6doors = leverDoors[2];
lever7doors = leverDoors[3];

local press1 = scene:getJoints("press1_joint")[1];
local press2 = scene:getJoints("press2_joint")[1];
local press3 = scene:getJoints("press3_joint")[1];
local press4 = scene:getJoints("press4_joint")[1];
local press5 = scene:getJoints("press5_joint")[1];
local press5Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("press5_light")[1];

local control3 = scene:getObjects("control3_ph")[1]:findPlaceholderComponent();

-- main

scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent().timeout = 30.0;
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHintTrigger(scene:getObjects("ha1_cp")[1].pos, 1.0);
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha1_cp")[1].pos);

if settings.skill <= const.SkillEasy then
    scene:getObjects("ha1")[1].active = true;
end

setSensorEnterListener("ha1_cp", true, function(other)
    scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():removeAllHints();
end);

scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent().timeout = 30.0;
scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():addHintTrigger(scene:getObjects("ha2_cp")[1].pos, 1.0);
scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha2_cp")[1].pos);

if settings.skill <= const.SkillEasy then
    scene:getObjects("ha2")[1].active = true;
end

setSensorEnterListener("ha2_cp", true, function(other)
    scene:getObjects("ha2")[1]:findPuzzleHintAreaComponent():removeAllHints();
end);

makeDoor("door8", false);
makeDoorTrigger("door8_cp", "door8");

makeDoor("door9", false);
makeDoorTrigger("door9_cp", "door9");

makeDoor("door10", false);
makeDoor("door11", false);
makeDoor("door12", false);
makeDoor("door13", false);

local function door1Fn(doors)
    if doorOpened(doors[1]) then
        closeDoor(doors[1], true);
    else
        openDoor(doors[1], true);
    end
    if doorOpened(doors[2]) then
        closeDoor(doors[2], false);
    else
        openDoor(doors[2], false);
    end
end;

makeLever("lever5", lever5def, door1Fn, door1Fn, lever5doors);
makeLever("lever6", lever6def, door1Fn, door1Fn, lever6doors);
makeLever("lever7", lever7def, door1Fn, door1Fn, lever7doors);

makeDoor("door14", false);

makeLever("lever8", false, function()
    disableLever("lever8");
    disableLever("lever11");
    press4.motorSpeed = -math.abs(press4.motorSpeed);
    local objs = scene:getObjects("laser10");
    for _, obj in pairs(objs) do
        obj:addComponent(FadeOutComponent(1.0));
    end
    audio:playSound("alarm_off.ogg");
    local inst = scene:getInstances("lever8")[1];
    for _, obj in pairs(inst.objects) do
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

makeLever("lever9", false, function()
    press1.motorSpeed = math.abs(press1.motorSpeed);
    audio:playSound("lever_pull.ogg");
end, function()
    press1.motorSpeed = -math.abs(press1.motorSpeed);
    audio:playSound("lever_pull.ogg");
end);

makeLever("lever10", false, function()
    press2.motorSpeed = math.abs(press2.motorSpeed);
    press3.motorSpeed = math.abs(press3.motorSpeed);
    audio:playSound("lever_pull.ogg");
end,
function()
    press2.motorSpeed = -math.abs(press2.motorSpeed);
    press3.motorSpeed = -math.abs(press3.motorSpeed);
    audio:playSound("lever_pull.ogg");
end);

makeLever("lever11", false, function()
    press4.motorSpeed = math.abs(press4.motorSpeed);
    audio:playSound("lever_pull.ogg");
end, function()
    press4.motorSpeed = -math.abs(press4.motorSpeed);
    audio:playSound("lever_pull.ogg");
end);

makeGear("press5", "press5_joint", "press6", "press6_joint", -1);

setSensorListener("press5_cp", function(other, self)
    if self.count == 0 then
        press5.motorSpeed = math.abs(press5.motorSpeed);
        audio:playSound("lever_pull.ogg");
        press5Light.visible = true;
    end
    self.count = self.count + 1;
end, function (other, self)
    self.count = self.count - 1;
    if self.count == 0 then
        press5.motorSpeed = -math.abs(press5.motorSpeed);
        audio:playSound("lever_pull.ogg");
        press5Light.visible = false;
    end
end, { count = 0 });

setSensorEnterListener("press5_pre_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("press5_pre_cp")[1]:getTransform();
    press4.motorSpeed = math.abs(press4.motorSpeed);
end);

control3.listener = createSensorEnterListener(true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
            scene.camera:findCameraComponent().target = scene:getObjects("enforcer3")[1];
            scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                addTimeoutOnce(1.0, function()
                    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal1")[1].pos);
                    local laser5 = scene:getObjects("laser5");
                    if #laser5 == 0 then
                        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("platform0")[1].pos);
                    end
                    audio:playSound("alarm_off.ogg");
                    local laser6 = scene:getObjects("laser6");
                    for _, obj in pairs(laser6) do
                        obj:addComponent(FadeOutComponent(1.0));
                    end
                    control3.active = false;
                    control3.visible = false;
                    press2.motorSpeed = -math.abs(press2.motorSpeed);
                    press4.motorSpeed = -math.abs(press4.motorSpeed);
                    disableLever("lever9");
                    disableLever("lever10");
                    disableLever("lever11");
                    scene:getObjects("press5_cp")[1].active = false;
                    addTimeoutOnce(2.0, function()
                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                            scene.camera:findCameraComponent().target = scene.player;
                            scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);
                            scene.cutscene = false;
                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);
