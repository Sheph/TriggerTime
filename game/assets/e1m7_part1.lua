local tentacle9 = scene:getObjects("tentacle9");
local worm1 = scene:getObjects("worm1");
local park1 = scene:getObjects("park1_ph")[1]:findPlaceholderComponent();
local park2 = scene:getObjects("park2_ph")[1]:findPlaceholderComponent();
local powergen = {
    scene:getObjects("powergen1")[1],
    scene:getObjects("powergen2")[1],
    scene:getObjects("powergen3")[1]
};
local powergenLight = {
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("powergen1_light")[1],
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("powergen2_light")[1],
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("powergen3_light")[1]
};

local function makeTentaclePulse1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local timeF = 0.2 + math.random() * 0.2;
        local phaseF = 3.0 + math.random() * 2.0;
        local a1F = 0.1 + math.random() * 0.2;
        local a2F = 0.1 + math.random() * 0.2;
        obj:addComponent(TentaclePulseComponent(15, timeF, phaseF, a1F, a2F));
    end
end

local function makeTentacleSwing1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.5 + math.random() * 1.0;
        local f = 1000 + math.random() * 1000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.8 + math.random() * 1.0;
        local f = 40000 + math.random() * 6000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing3(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.8 + math.random() * 1.0;
        local f = 20000 + math.random() * 6000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

-- main

makeTentacleFlesh("tentacle8");
makeTentacleSwing1("tentacle8");
makeTentaclePulse1("tentacle8");
makeTentacleFlesh("tentacle9");
makeTentacleSwing2("tentacle9");

makeDoor("door7", false);

setSensorEnterListener("boat1_cp", true, function(other, self)
    stopPlatforms0();
    audio:playSound("squid_alert1.ogg");
    scene.respawnPoint = scene:getObjects("boat1_cp")[1]:getTransform();
    unweldTentacleBones(self.w);
    addTimeoutOnce(2.0, function()
        weldTentacleBones(tentacle9, {3, 6, 9}, 1.5, 0.5);
    end);
end, { w = weldTentacleBones(tentacle9, {3, 6, 9}, 1.5, 0.5) });

setSensorEnterListener("boat2_cp", true, function(other)
    for _, obj in pairs(worm1) do
        obj:findTargetableComponent().autoTarget = true;
    end
    addTimeout(0.5, function(cookie)
        for _, obj in pairs(worm1) do
            if obj:alive() then
                return;
            end
        end
        cancelTimeout(cookie);
        addTimeoutOnce(2.0, function()
            openDoor("door7", true);
        end);
    end);
end);

makeBoatPark(park1, "park1_exit", function(boat, self)
    if self.first then
        self.first = false;
        startMusic("action11.ogg", true);
    end
end, nil, { first = true });

setSensorEnterListener("boat3_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showUpperDialog(
        {
            {"player", tr.dialog83.str1},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

makeAirlock("door8", true);

setSensorEnterListener("boat4_cp", true, function(other)
    closeAirlock("door8", true);
end);

makeAirlock("door9", false);

makeBoatPark(park2, "park2_exit");

makeTentacleFlesh("tentacle10");
makeTentacleSwing3("tentacle10");
makeTentaclePulse1("tentacle10");

setSensorEnterListener("power1_cp", true, function(other)
    addTimeoutOnce(3.0, function()
        fixedSpawn("power1");
    end);
end);

addTimeout(0.25, function(cookie)
    local done = true;
    for i = 1, 3, 1 do
        if powergen[i] ~= nil then
            if powergen[i]:dead() then
                powergenLight[i].visible = false;
                powergen[i] = nil;
            else
                done = false;
            end
        end
    end
    if done then
        cancelTimeout(cookie);
        openAirlock("door9", true);
    end
end);
