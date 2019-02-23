local control0 = scene:getObjects("control0_ph")[1]:findPlaceholderComponent();
local control1 = scene:getObjects("control1_ph")[1]:findPlaceholderComponent();

local function enforcerSpawn(other, name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end
end

local function otherSpawn(other, name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local e = nil;
        if math.random(1, 3) == 3 then
            e = factory:createScorp2();
        else
            e = factory:createTetrocrab2();
        end
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end
end

function setupPart0WayBack()
    setSensorEnterListener("enemy5_spawn", true, function(other)
        enforcerSpawn(other, "enemy5_enforcer");
        otherSpawn(other, "enemy5_other");
    end);
    setSensorEnterListener("wayback3_cp", true, function(other)
        fixedSpawn("wayback3");
        spawnBots("wayback3", 0.5);
    end);
    setSensorEnterListener("wayback4_cp", true, function(other)
        fixedSpawn("wayback4");
    end);
end

-- main

makeAirlock("door1", false);
makeAirlock("door2", false);
makeAirlock("door3", false);
makeAirlock("door4", true);
makeAirlock("door5", false);
makeAirlock("door6", false);
makeAirlock("door7", false);
makeAirlockTrigger("door3_cp", "door3");

scene:getObjects("comm_enforcer")[1].life = 1;

setSensorEnterListener("comm_cp1", true, function(other)
    scene.respawnPoint = scene:getObjects("comm_cp1")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("comm_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 8.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("comm_cp2", true, function(other)
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    addTimeoutOnce(1.5, function()
        scene.player:findPlayerComponent().weapon:trigger(true);
        addTimeoutOnce(0.5, function()
            scene.player:findPlayerComponent().weapon:trigger(false);
            addTimeoutOnce(1.0, function()
                scene.player.roamBehavior.linearVelocity = 8.0;
                scene.player.roamBehavior.linearDamping = 3.0;
                scene.player.roamBehavior.dampDistance = 0.0;
            end);
        end);
    end);
end);

control0.listener = createSensorEnterListener(true, function(other)
    scene.player.roamBehavior:damp();
    control0.active = false;
    control0.visible = false;
    addTimeoutOnce(1.0, function()
        scene.player.roamBehavior:reset();
        showLowerDialog(
        {
            {"player", tr.dialog41.str1},
            {"player", tr.dialog41.str2},
            {"player", tr.dialog41.str3},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog41.str4},
            {"player", tr.dialog41.str5},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog41.str6},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog41.str7},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog41.str8},
            {"player", tr.dialog41.str9},
        }, function ()
            scene.cutscene = false;
            addTimeoutOnce(1.0, function()
                openAirlock("door5");
                openAirlock("door6");
                closeAirlock("door4");
                local objs = scene:getObjects("enforcer1");
                for _, obj in pairs(objs) do
                    obj:findTargetableComponent().autoTarget = true;
                end
                alarmLightOn("alarm1", false);
            end);
        end);
    end);
end);

setSensorEnterListener("enforcer2_spawn", true, enforcerSpawn, "enforcer2");
setSensorEnterListener("enemy3_spawn", true, function(other)
    enforcerSpawn(other, "enemy3_enforcer");
    otherSpawn(other, "enemy3_other");
end);
setSensorEnterListener("enemy4_spawn", true, enforcerSpawn, "enemy4_enforcer");

makeGasPipe("pipe1");
makeGasPipe("pipe2");

setSensorEnterListener("enforcer3_activate", true, function(other)
    local objs = scene:getObjects("enforcer3");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("ws1")[1].pos);
scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("control1_ph")[1].pos);

local weapon1Cnt = 0;

setSensorEnterListener("weapon1_cp", true, function(other)
    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("ws1")[1].pos);
    weapon1Cnt = weapon1Cnt + 1;
    if weapon1Cnt == 2 then
        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
    end
end);

control1.listener = createSensorEnterListener(true, function(other)
    control1.active = false;
    control1.visible = false;
    openAirlock("door1");
    openAirlock("door4");
    alarmLightOff("alarm1", false);
    local objs = scene:getObjects("laser1");
    for _, obj in pairs(objs) do
        obj.active = false;
        obj.visible = false;
    end
    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("control1_ph")[1].pos);
    weapon1Cnt = weapon1Cnt + 1;
    if weapon1Cnt == 2 then
        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
    end
    enforcerSpawn(other, "enforcer4");
    setupPart0WayBack();
end);
