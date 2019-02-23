local car0 = scene:getInstances("car0")[1];
local car1 = scene:getInstances("car1")[1];
local control9 = scene:getObjects("control9_ph")[1]:findPlaceholderComponent();
local elevator1Lights = {
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("elevator1_light1"),
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("elevator1_light2"),
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("elevator1_light3")
};
local inTrain = false;

local function makeOpenDoorStop(name, openTimeout, stayTimeout, timerAmount)
    if timerAmount ~= nil then
        makeTimer(name.."_timer", timerAmount);
    end
    makeSubwayStop(name, openTimeout + stayTimeout, function(car)
        car.myCar.collisionImpulseMultiplier = 1.0;
        addTimeoutOnce(openTimeout, function()
            for _, v in pairs(car.myCar.objs) do
                if v.obj == scene.player then
                    scene.respawnPoint = scene:getObjects(name)[1]:getTransform();
                    inTrain = false;
                end
                v.obj.invulnerable = false;
            end
            scene:getObjects(name.."_blocker")[1].active = false;
            openSubwayCarDoor(car);
        end);
        addTimeoutOnce(stayTimeout, function()
            scene:getObjects(name.."_blocker")[1].active = true;
            closeSubwayCarDoor(car);
            if timerAmount ~= nil then
                resetTimer(name.."_timer");
            end
        end);
    end, function(car)
        car.myCar.collisionImpulseMultiplier = 4.0;
        for _, v in pairs(car.myCar.objs) do
            v.obj.invulnerable = true;
            if v.obj == scene.player then
                inTrain = true;
            end
        end
    end);
end

function stopTrains()
    stopSubwayCar(car0);
    stopSubwayCar(car1);
    stopTimers();
end

-- main

local insts = scene:getInstances("flicker_lamp_off");
for _, inst in pairs(insts) do
    local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
    for _, light in pairs(lights) do
        light.visible = false;
    end
end

makeFlickerLamps("flicker_lamp");
makeFlickerLamps("flicker_lamp_off");

makeAirlock("door0", false);
makeDoor("door1", false);
makeDoor("door25", false);

makeSubwayCar(car0, "car0_path", function(other)
    if other.type == const.SceneObjectTypePlayer then
        scene.camera:findCameraComponent():zoomTo(45, const.EaseInOutQuad, 1.0);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
    end
end, function(other)
    if other.type == const.SceneObjectTypePlayer then
        scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 1.0);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
    end
end);

makeSubwayCar(car1, "car1_path", function(other)
    if other.type == const.SceneObjectTypePlayer then
        scene.camera:findCameraComponent():zoomTo(45, const.EaseInOutQuad, 1.0);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
    end
end, function(other)
    if other.type == const.SceneObjectTypePlayer then
        scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 1.0);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
    end
end);

local function setTimers()
    setTimer("car0_cp0_timer", 5.0);
    setTimer("car1_cp0_timer", 26.0);
    setTimer("car0_cp2_timer", 48.0);
    setTimer("car1_cp2_timer", 45.0);
    setTimer("car0_cp1_timer", 27.0);
    setTimer("car1_cp1_timer", 6.0);
end

makeOpenDoorStop("car0_cp0", 1.0, 4.0, 60.0);
makeOpenDoorStop("car0_cp1", 1.0, 4.0, 60.0);
makeOpenDoorStop("car0_cp2", 1.0, 4.0, 60.0);
makeOpenDoorStop("car1_cp0", 1.0, 4.0, 53.0);
makeOpenDoorStop("car1_cp1", 1.0, 4.0, 53.0);
makeOpenDoorStop("car1_cp2", 1.0, 4.0, 53.0);

if settings.developer == 0 then
    scene.cutscene = true;
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
    scene.camera:findCameraComponent().target = scene:getObjects("intro_target1")[1];
    addTimeoutOnce(1.5, function()
        introCutscene(function()
            scene.cutscene = false;
            scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};
            scene.camera:findCameraComponent().target = scene.player;
            setTimers();
            startSubwayCar(car0);
            startSubwayCar(car1);
        end);
    end);
else
    setTimers();
    addTimeoutOnce(0.0, function()
        startSubwayCar(car0);
        startSubwayCar(car1);
    end);
end

setSensorEnterListener("station3_e1_cp", true, function(other)
    local objs = scene:getObjects("station3_e1");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
        local c = obj:findSentryComponent();
        if c ~= nil then
            c:unfold();
        end
    end
end);

setSensorEnterListener("station3_e2_cp", true, function(other)
    fixedSpawn("station3_e2");
end);

setSensorEnterListener("station3_e3_cp", true, function(other)
    fixedSpawn("station3_e3");
end);

setSensorEnterListener("station2_e1_cp", true, function(other)
    local objs = scene:getObjects("station2_e1");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("station2_e2_cp", true, function(other)
    fixedSpawn("station2_e2");
end);

makeKeyDoor("red_door", "red_key_ph", function()
    openDoor("door1", true);
end);

makeKeyDoor("blue_door", "blue_key_ph", function()
    openDoor("door25", true);
end);

makeLever("lever1", true, nil, function()
    startFinalPods();
    local obj = scene:getObjects("elevator1_target")[1];
    audio:playSound("lever_pull.ogg");
    disableLever("lever1");
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        audio:playSound("servo_move.ogg");
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 1.0, function()
            scene.player.pos = obj.pos;
            scene.respawnPoint = obj:getTransform();
            scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
                scene.cutscene = false;
                stopTrains();
                startMusic("ambient12.ogg", true);
            end);
        end);
    end);
end);

addTimeoutOnce(1.0, function()
    disableLever("lever1");
end);

setSensorEnterListener("elevator1_cp1", true, function(other)
    addTimeoutOnce(3.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog89.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

control9.listener = createSensorEnterListener(true, function(other)
    scene:getObjects("elevator1_cp1")[1].active = false;
    enableLever("lever1");
    control9.active = false;
    control9.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog90.str1},
            {"player", tr.dialog90.str2},
        }, function ()
            scene.camera:findCameraComponent():follow(scene:getObjects("cam_target3")[1], const.EaseOutQuad, 1.5);
            for i = 1, 3, 1 do
                addTimeoutOnce(2.5 + (i - 1) * 1.5, function()
                    for _, light in pairs(elevator1Lights[i]) do
                        light.visible = true;
                    end
                    audio:playSound("lights_on.ogg");
                end);
            end
            addTimeoutOnce(2.5 + 2 * 1.5 + 1.5, function()
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                addTimeoutOnce(1.5, function()
                    scene.cutscene = false;
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("railways1_cp", false, function(other)
    if inTrain then
        return;
    end
    scene:getObjects("railways1_cp")[1].active = false;
    addTimeoutOnce(2.0, function()
        showUpperMsg(6.0, {"player", tr.dialog91.str1});
    end);
end);

setSensorEnterListener("railways2_cp", false, function(other)
    if inTrain then
        return;
    end
    scene:getObjects("railways2_cp")[1].active = false;
    addTimeoutOnce(2.0, function()
        showUpperMsg(6.0, {"player", tr.dialog92.str1});
    end);
end);

setSensorEnterListener("railways3_cp", false, function(other)
    if inTrain then
        return;
    end
    scene:getObjects("railways3_cp")[1].active = false;
    addTimeoutOnce(1.0, function()
        showUpperMsg(8.0, {"player", tr.dialog93.str1});
        scene:getObjects("terrain0")[1]:findRenderQuadComponents("dopefish0")[1].color = {1.0, 1.0, 1.0, 1.0};
        gameShell:setAchieved(const.AchievementDopefishLives);
    end);
end);

setSensorEnterListener("railways4_cp", false, function(other)
    if inTrain then
        return;
    end
    scene:getObjects("railways4_cp")[1].active = false;
    addTimeoutOnce(1.0, function()
        showUpperMsg(6.0, {"player", tr.dialog94.str1});
        gameShell:setAchieved(const.AchievementTurnBack);
    end);
end);

setSensorEnterListener("railways5_cp", false, function(other)
    if inTrain then
        return;
    end
    scene:getObjects("railways5_cp")[1].active = false;
    addTimeoutOnce(1.0, function()
        showUpperMsg(6.0, {"player", tr.dialog95.str1});
    end);
end);
