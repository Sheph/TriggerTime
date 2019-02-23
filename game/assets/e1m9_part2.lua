local jars = {};
jars[22] = scene:getInstances("jar22")[1];
jars[25] = scene:getInstances("jar25")[1];
jars[28] = scene:getInstances("jar28")[1];
jars[29] = scene:getInstances("jar29")[1];
jars[30] = scene:getInstances("jar30")[1];
jars[31] = scene:getInstances("jar31")[1];
local display4 = setupDisplay("display4");
local socket1 = findObject(scene:getInstances("socket1")[1].objects, "socket1");
local socket2 = findObject(scene:getInstances("socket2")[1].objects, "socket1");
local socket3 = findObject(scene:getInstances("socket3")[1].objects, "socket1");
local socket4 = findObject(scene:getInstances("socket4")[1].objects, "socket1");
local cord4 = {scene:getObjects("cord4_front")[1], scene:getObjects("cord4_back")[1]};
local cord5 = {scene:getObjects("cord5_front")[1], scene:getObjects("cord5_back")[1]};
local cord6 = {scene:getObjects("cord6_front")[1], nil};
local gasroomLight1 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("gasroom_light1")[1];
local gasroomLight2 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("gasroom_light2")[1];
local octopuses = {
    scene:getObjects("octopus1")[1],
    scene:getObjects("octopus2")[1],
    scene:getObjects("octopus3")[1],
    scene:getObjects("octopus4")[1]
};
local gasCloud = nil;
local control1 = scene:getObjects("control1_ph")[1]:findPlaceholderComponent();

local function updateGasroomExit()
    if (socket1.myPlug ~= nil) and (socket1.myPlug.myOther ~= nil) and (socket1.myPlug.myOther.mySocket == socket4) then
        gasroomLight1.visible = true;
    else
        gasroomLight1.visible = false;
    end
    if (socket2.myPlug ~= nil) and (socket2.myPlug.myOther ~= nil) and (socket2.myPlug.myOther.mySocket == socket3) then
        gasroomLight2.visible = true;
    else
        gasroomLight2.visible = false;
    end
    if gasroomLight1.visible and gasroomLight2.visible then
        disableSocket(socket1);
        disableSocket(socket2);
        disableSocket(socket3);
        disableSocket(socket4);
        openAirlock("door23", true);
    end
end

local function octopusCharge(octopus, fn1, fn2, ...)
    local dir = octopus:getDirection(1.0);
    addTimeout0(function(cookie, dt, self)
        octopus:applyForceToCenter(dir * -2000.0, true);
        self.t = self.t - dt;
        if self.t <= 0 then
            cancelTimeout(cookie);
            if fn1 ~= nil then
                fn1(unpack2(self.args));
            end
            addTimeout0(function(cookie, dt, self)
                octopus:applyForceToCenter(dir * 10000.0, true);
                self.t = self.t - dt;
                if self.t <= 0 then
                    cancelTimeout(cookie);
                    scene.camera:findCameraComponent():roll(math.rad(5));
                    audio:playSound("grate_hit.ogg");
                    fn2(unpack2(self.args));
                end
            end, { t = 0.3, args = self.args });
        end
    end, { t = 0.7, args = pack2(...) });
end

local function unleashOctopus(jar)
    local octopus = jar.myOctopus;
    local cs = jar.myRoot:findFloatComponents();
    for _, c in pairs(cs) do
        c:removeFromParent();
    end
    audio:playSound("octopus_charge"..math.random(1, 2)..".ogg");
    octopusCharge(octopus, nil, function()
        addTimeoutOnce(0.5, octopusCharge, octopus, nil, function()
            addTimeoutOnce(0.5, octopusCharge, octopus, function()
                for _, joint in pairs(jar.joints) do
                    joint:remove();
                end
            end,
            function()
                for _, obj in pairs(jar.myParts) do
                    obj:addComponent(FadeOutComponent(1.5));
                end
                addTimeoutOnce(0.5, function()
                    local hb = RenderHealthbarComponent(vec2(0.0, 4.0), 0, 8.0, 1.2, const.zOrderMarker + 1);
                    hb.color = { 1.0, 1.0, 1.0, 0.6 };
                    octopus:addComponent(hb);
                    octopus:findTargetableComponent().autoTarget = true;
                end);
            end);
        end);
    end);
end

function postGasroom()
    scene:getObjects("gasroom5_cp")[1].active = true;
end

-- main

makeAirlock("door21", true);
makeAirlock("door22", true);
makeAirlock("door23", false);
makeAirlock("door24", true);
makeAirlock("door25", false);

for i = 0, 21, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    if (i == 8) or (i == 16) or (i == 21) then
        makeJar1Empty(jars[i]);
    else
        makeJar1WithCreature(jars[i], "cr"..i);
    end
end

for i = 23, 24, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    makeJar1WithCreature(jars[i], "cr"..i);
end

for i = 26, 27, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    makeJar1WithCreature(jars[i], "cr"..i);
end

makeJar1(jars[22], {{"cord4_front", 1.5}, {"cord4_back", 1.5}, {"cord4_1", 1.5}, {"cord4_2", 1.5}});
makeJar1(jars[25], {{"cord5_front", 1.5}, {"cord5_back", 1.5}, {"cord5_1", 1.5}, {"cord5_2", 1.5}});

local octopusFloats = {};

local octopusJarsDestroyed = 0;

local door25Timer = nil;

for i = 1,4,1 do
    local octopusFloats = {{"octopus"..i, 100.0}};
    for j = 1,8,1 do
        octopusFloats[j + 1] = {"octopus"..i..".tentacle"..j, 8.0};
    end
    makeJar2(jars[27 + i], octopusFloats);
    jars[27 + i].myOctopus = octopuses[i];
    jars[27 + i].myDieCb = function()
        scene:getObjects("terrain0")[1]:findLightComponent():getLights("jar"..(27 + i).."_light")[1].visible = false;
        octopusJarsDestroyed = octopusJarsDestroyed + 1;
        if octopusJarsDestroyed == 4 then
            addTimeoutOnce(5.0, function()
                scene:getObjects("gasroom2_cp")[1].active = true;
                door25Timer = addTimeoutOnce(0.5, function()
                    door25Timer = nil;
                    openAirlock("door25", true);
                end);
            end);
        end
    end;
end

makeSocket(socket1);
makeSocket(socket2);
makeSocket(socket3);
makeSocket(socket4);

makePlugs(cord4, function(plug, socket)
    updateGasroomExit();
end, function(plug, socket)
    updateGasroomExit();
end);

makePlugs(cord5, function(plug, socket)
    updateGasroomExit();
end, function(plug, socket)
    updateGasroomExit();
end);

makePlugs(cord6, function(plug, socket)
    updateGasroomExit();
end, function(plug, socket)
    updateGasroomExit();
end);

findObject(scene:getInstances("tube1")[1].objects, "tube"):findLightComponent():getLights("light")[1].visible = true;
findObject(scene:getInstances("tube2")[1].objects, "tube"):findLightComponent():getLights("light")[1].visible = true;

scene:addWeldJoint(jars[18].myRoot, scene:getObjects("cord6_base")[1], scene:getObjects("cord6_base")[1]:getWorldPoint(vec2(0, 0)), false);

setSensorEnterListener("gasroom3_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("gasroom3_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("gasroom3");
    end
end, { first = true });

setSensorEnterListener("gasroom4_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("gasroom4_cp")[1]:getTransform();
    closeAirlock("door21", true);
    closeAirlock("door22", false);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("cam_target3")[1], const.EaseOutQuad, 1.0);
        addTimeoutOnce(2.0, function()
            display4:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.5, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog126.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog126.str2},
                }, function ()
                    scene.camera:findCameraComponent():follow(scene:getObjects("gas1")[1], const.EaseOutQuad, 1.0);
                    gasCloud = factory:createGasCloud1(3.0, 85);
                    gasCloud.pos = scene:getObjects("gas1")[1].pos;
                    scene:addObject(gasCloud);
                    audio:playSound("gas_in.ogg");
                    addTimeoutOnce(5.0, function()
                        display4:startAnimation(const.AnimationDefault);
                        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                        addTimeoutOnce(1.5, function()
                            local dialog;
                            local dialogTimer;
                            dialogTimer = addTimeoutOnce(4.0, function()
                                dialogTimer = nil;
                                dialog:removeFromParent();
                                scene.cutscene = false;
                            end);
                            dialog = showLowerDialog(
                            {
                                {"player", tr.dialog127.str1},
                            }, function ()
                                scene.cutscene = false;
                                if dialogTimer ~= nil then
                                    cancelTimeout(dialogTimer);
                                    dialogTimer = nil;
                                end
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorListener("gasroom1_cp", function(other, self)
    scene.respawnPoint = scene:getObjects("gasroom1_cp")[1]:getTransform();
    scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 1.0);
    other:findCollisionDamageComponent().enabled = false;
    if self.first then
        self.first = false;
        audio:playSound("lights_on.ogg");
        scene:getObjects("terrain0")[1]:findLightComponent():getLights("gasroom_light3")[1].visible = true;
        if not scene.player:findPlayerComponent().flagDamagedByGas then
            gameShell:setAchieved(const.AchievementFreshAir);
        end
    end
end, function(other)
    scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.0);
    other:findCollisionDamageComponent().enabled = true;
end, { first = true });

setSensorEnterListener("gasroom2_cp", true, function(other)
    startMusic("action9.ogg", true);
    if door25Timer ~= nil then
        cancelTimeout(door25Timer);
        door25Timer = nil;
        closeAirlock("door24", true);
    else
        addTimeoutOnce(0.1, function()
            closeAirlock("door25", true);
        end);
        closeAirlock("door24", false);
    end
    addTimeoutOnce(2.0, function()
        unleashOctopus(jars[29]);
        addTimeoutOnce(5.0, function()
            startMusic("action10.ogg", false);
            unleashOctopus(jars[30]);
            addTimeoutOnce(1.0, function()
                unleashOctopus(jars[28]);
                addTimeoutOnce(1.0, function()
                    unleashOctopus(jars[31]);
                end);
            end);
        end);
    end);
    addTimeout(0.25, function(cookie)
        for _, obj in pairs(octopuses) do
            if obj:alive() then
                return;
            end
        end
        cancelTimeout(cookie);
        addTimeoutOnce(1.5, function()
            startAmbientMusic(true);
            openAirlock("door24", true);
            openAirlock("door25", false);
        end);
    end);
end);

control1.listener = createSensorEnterListener(true, function(other)
    control1.active = false;
    control1.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog128.str1},
        }, function ()
            scene.camera:findCameraComponent():follow(scene:getObjects("gas1")[1], const.EaseOutQuad, 1.5);
            addTimeoutOnce(2.0, function()
                audio:playSound("gas_out.ogg");
                if gasCloud ~= nil then
                    local objs = gasCloud:getObjects();
                    table.insert(objs, gasCloud);
                    for _, obj in pairs(objs) do
                        obj:findParticleEffectComponent():allowCompletion();
                        obj:addComponent(FadeOutComponent(9.0));
                    end
                    gasCloud = nil;
                end
                addTimeoutOnce(7.0, function()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                    scene.cutscene = false;
                    postGasroom();
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("gasroom5_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("gasroom5_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("gasroom5");
        local spawn = scene:getObjects("gasroom5b_keeper");
        local keepers = {};
        for i = 1, #spawn, 1 do
            keepers[i] = factory:createKeeper(false);
            summon2(spawn[i], false, function()
                addSpawnedEnemyFreezable(keepers[i], spawn[i]);
            end);
        end
        addTimeout(0.25, function(cookie)
            for _, obj in pairs(keepers) do
                if obj:alive() then
                    return;
                end
            end
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                openAirlock("door21", true);
                openAirlock("door22", false);
                bossDone(1);
            end);
        end);
    end
end, { first = true });
