local cooler0Socket = scene:getObjects("cooler0_socket")[1];
local cooler1Socket = scene:getObjects("cooler1_socket")[1];
local cooler0Plug = scene:getObjects("cooler0_plug")[1];
local cooler1Plug = scene:getObjects("cooler1_plug")[1];
local press1Joint = scene:getJoints("press1_joint")[1];

-- main

makeDoor("door8", false);

makeWater("terrain0", "water1");

makeWaterPipe("pipe6");

makeSocket(cooler0Socket);
makeSocket(cooler1Socket);
makePlug(cooler0Plug);

findObject(scene:getInstances("cooler0_tube")[1].objects, "tube"):findLightComponent():getLights("light")[1].visible = true;
findObject(scene:getInstances("cooler1_tube")[1].objects, "tube"):findLightComponent():getLights("light")[1].visible = true;

addTimeoutOnce(0.5, function()
    disableSocket(cooler0Socket);
end)

makePlug(cooler1Plug, function(socket)
    local lights = scene:getObjects("cooler1")[1]:findLightComponent():getLights("light");
    for _, light in pairs(lights) do
        light.visible = true;
    end
    disableSocket(cooler1Socket);
    addTimeoutOnce(2.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog70.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog70.str2},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog70.str3},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog70.str4}
        }, function ()
            scene.cutscene = false;
            openDoor("door8", true);
            openDoor("door4", false);
            scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal3")[1].pos);
            scene:getObjects("east3_cp")[1].active = true;
            scene:getObjects("core2_cp")[1].active = true;
        end);
    end);
end);

addTimeout0(function(cookie, dt)
    if (press1Joint:getJointTranslation() <= press1Joint.lowerLimit) then
        press1Joint.motorSpeed = math.abs(press1Joint.motorSpeed);
    elseif (press1Joint:getJointTranslation() >= press1Joint.upperLimit) then
        press1Joint.motorSpeed = -math.abs(press1Joint.motorSpeed);
    end
end);

makeGear("press1", "press1_joint", "press2", "press2_joint", 1);
makeGear("press1", "press1_joint", "press3", "press3_joint", -2);

setSensorEnterListener("east3_cp", true, function(other)
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal3")[1].pos);
    bridgeOpen(bridge1, 5.0, false);
end);

setSensorEnterListener("pump1_cp", true, function(other)
    local spawn = scene:getObjects("spawn8_orbo");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createOrbo(), obj);
    end
    local spawn = scene:getObjects("spawn8_oth");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 2) == 2 then
            e = factory:createWarder();
        else
            e = factory:createEnforcer1();
        end
        addSpawnedEnemy(e, obj);
    end
    local spawn = scene:getObjects("spawn8_gorger");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
end);
