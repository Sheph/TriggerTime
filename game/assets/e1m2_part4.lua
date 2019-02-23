local saws = {};
local sawTimer = nil;
local saw3SliderJoint = scene:getJoints("saw3_slider_joint")[1];
local saw6SliderJoint = scene:getJoints("saw6_slider_joint")[1];
local spike1Joint = scene:getJoints("spike1_joint")[1];
local spike4Joint = scene:getJoints("spike4_joint")[1];
local spike6Joint = scene:getJoints("spike6_joint")[1];
local saw7Slider = nil;
local doorMoves = {};

local function stoneDoorMove(jointName, upper, tremor)
    local j = scene:getJoints(jointName)[1];
    local snd = nil;
    if doorMoves[jointName] ~= nil then
        snd = doorMoves[jointName].snd;
        cancelTimeout(doorMoves[jointName].cookie);
        doorMoves[jointName] = nil;
    end
    if (upper and j:getJointTranslation() >= j.upperLimit) or
       (not upper and j:getJointTranslation() <= j.lowerLimit) then
        if snd ~= nil then
            snd:stop();
            snd = nil;
            scene.camera:findCameraComponent():tremor(false);
        end
        return;
    end
    doorMoves[jointName] = {};
    if (snd == nil) and tremor then
        doorMoves[jointName].snd = audio:createSound("stone_moving.ogg");
        doorMoves[jointName].snd:play();
        scene.camera:findCameraComponent():tremorStart(0.3);
    else
        if (not tremor) and (snd ~= nil) then
            snd:stop();
            snd = nil;
            scene.camera:findCameraComponent():tremor(false);
        end
        doorMoves[jointName].snd = snd;
    end
    if upper then
        j.motorSpeed = math.abs(j.motorSpeed);
    else
        j.motorSpeed = -math.abs(j.motorSpeed);
    end
    doorMoves[jointName].cookie = addTimeout0(function(cookie, dt, args)
        if (upper and j:getJointTranslation() >= j.upperLimit) or
           (not upper and j:getJointTranslation() <= j.lowerLimit) then
           cancelTimeout(cookie);
            if tremor then
                doorMoves[jointName].cookie = addTimeoutOnce(0.2, function()
                    doorMoves[jointName].snd:stop();
                    doorMoves[jointName] = nil;
                    scene.camera:findCameraComponent():tremor(false);
                end);
            else
                doorMoves[jointName] = nil;
            end
        end
    end);
end

local function loopPrismatic(j)
    if (j:getJointTranslation() >= j.upperLimit) then
        j.motorSpeed = -math.abs(j.motorSpeed);
    elseif (j:getJointTranslation() <= j.lowerLimit) then
        j.motorSpeed = math.abs(j.motorSpeed);
    end
end

local function loopSpike(j, direct)
    if j.myMotorSpeed == nil then
        j.myMotorSpeed = math.abs(j.motorSpeed);
    end
    if (j:getJointTranslation() >= j.upperLimit) then
        if direct then
            j.motorSpeed = -j.myMotorSpeed / 4;
        else
            j.motorSpeed = -j.myMotorSpeed;
        end
    elseif (j:getJointTranslation() <= j.lowerLimit) then
        if direct then
            j.motorSpeed = j.myMotorSpeed;
        else
            j.motorSpeed = j.myMotorSpeed / 4;
        end
    end
end

local function enableSaws()
    for _, obj in pairs(saws) do
        obj.active = true;
    end
    sawTimer = addTimeout0(function(cookie, dt)
        loopPrismatic(saw3SliderJoint);
        loopPrismatic(saw6SliderJoint);
        loopSpike(spike1Joint, true);
        loopSpike(spike4Joint, true);
        loopSpike(spike6Joint, false);
    end);
end

local function disableSaws()
    for _, obj in pairs(saws) do
        obj.active = false;
    end
    cancelTimeout(sawTimer);
end

-- main

for i = 1, 6, 1 do
    local saw = scene:getObjects("saw"..i)[1];
    saw.type = const.SceneObjectTypeEnemyBuilding;
    table.insert(saws, saw);
end
for i = 8, 11, 1 do
    local saw = scene:getObjects("saw"..i)[1];
    saw.type = const.SceneObjectTypeEnemyBuilding;
    table.insert(saws, saw);
end
for i = 1, 6, 1 do
    local spike = scene:getObjects("spike"..i)[1];
    spike.type = const.SceneObjectTypeEnemyBuilding;
    table.insert(saws, spike);
end
table.insert(saws, scene:getObjects("saw3_slider")[1]);
table.insert(saws, scene:getObjects("saw4_slider")[1]);
table.insert(saws, scene:getObjects("saw5_slider")[1]);
table.insert(saws, scene:getObjects("saw4_holder")[1]);
table.insert(saws, scene:getObjects("saw6_slider")[1]);
table.insert(saws, scene:getObjects("saw9_slider")[1]);

setSensorListener("saw_activate_cp", function(other, self)
    if self.timer ~= nil then
        cancelTimeout(self.timer);
        self.timer = nil;
    else
        enableSaws();
    end
end, function (other, self)
    if other:scene() ~= nil then
        scene.respawnPoint = scene:getObjects("saw0_cp")[1]:getTransform();
    end
    self.timer = addTimeoutOnce(5.0, function()
        disableSaws();
        self.timer = nil;
    end);
end, { timer = nil });

setSensorEnterListener("saw0_cp", false, function(other)
    if not isAmbient then
        startAmbientMusic(true);
    end
end);

setSensorEnterListener("saw1_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("saw1_cp")[1]:getTransform();
    setupBgMetal();
    if isAmbient then
        startMusic("action4.ogg", true);
    end
end);

setSensorEnterListener("saw2_cp", false, function(other)
    setupBgAir();
end);

setSensorEnterListener("saw3_cp", false, function(other, self)
    if self.obj == nil then
        scene.respawnPoint = scene:getObjects("saw3_cp")[1]:getTransform();
        stoneDoorMove("door10_joint", true, true);
    end
    if self.obj ~= other then
        if self.obj ~= nil then
            setupBgAir();
        end
        if self.sawObjs ~= nil then
            for _, obj in pairs(self.sawObjs) do
                local c = obj:findPhysicsBodyComponent();
                if c ~= nil then
                    c:removeFromParent();
                end
                obj:addComponent(FadeOutComponent(1.0));
            end
        end

        local sawObjs = scene:instanciate("e1m2_saw7.json",
            scene:getObjects("saw7_ph")[1]:getTransform()).objects;

        for _, obj in pairs(sawObjs) do
            obj.type = const.SceneObjectTypeEnemyBuilding;
            if obj.name == "saw7_slider" then
                saw7Slider = obj;
            end
        end

        local saw7path = scene:getObjects("saw7_path")[1];
        saw7Slider.roamBehavior:reset();
        saw7Slider.roamBehavior.linearVelocity = 5.0;
        saw7Slider.roamBehavior.linearDamping = 3.0;
        saw7Slider.roamBehavior.dampDistance = 5.0;
        saw7Slider.roamBehavior.changeAngle = false;
        saw7Slider.roamBehavior:changePath(saw7path:findPathComponent().path, saw7path:getTransform());
        saw7Slider.roamBehavior:start();

        self.sawObjs = sawObjs;
    end
    self.obj = other;
end, { obj = nil, sawObjs = nil });

setSensorListener("saw4_cp", function(other, self)
    stoneDoorMove("door11_joint", true, true);
end, function (other, self)
    stoneDoorMove("door11_joint", false, false);
end);

makeGear("spike1", "spike1_joint", "spike2", "spike2_joint", -1);
makeGear("spike1", "spike1_joint", "spike3", "spike3_joint", 1);

setSensorListener("saw5_cp", function(other, self)
    stoneDoorMove("door12_joint", true, true);
end, function (other, self)
    stoneDoorMove("door12_joint", false, false);
end);

makeGear("spike4", "spike4_joint", "spike5", "spike5_joint", 1);

setSensorListener("saw6_cp", function(other, self)
    stoneDoorMove("saw9_slider_joint", true, false);
    saw7Slider.roamBehavior.linearVelocity = 10.0;
end, function (other, self)
    stoneDoorMove("saw9_slider_joint", false, false);
end);

setSensorEnterListener("saw7_cp", false, function(other)
    setupBgMetal();
end);

setSensorEnterListener("saw8_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("saw8_cp")[1]:getTransform();
    stoneDoorMove("door9_joint", true, true);
end);

setSensorEnterListener("saw9_cp", true, function(other)
    scene:getObjects("ga4")[1]:findGoalAreaComponent():addGoal(scene:getObjects("red_door")[1].pos);
end);
