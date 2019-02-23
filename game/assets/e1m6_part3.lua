local masterSocket1 = scene:getObjects("socket_master1")[1];
local masterSocket2 = scene:getObjects("socket_master2")[1];
local plug1Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("plug1_light")[1];
local plug2Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("plug2_light")[1];
local plug17Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("plug17_light")[1];
local plug18Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("plug18_light")[1];
local sockets = {};
local plugs = {};
local lights = {};
local tube3Broken = false;
local halwayDone = false;

local function spawnPlug9()
    local spawn = scene:getObjects("spawn_plug9");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            addSpawnedEnemy(e, spawn[i]);
        end);
    end
end

local function spawnPlug13()
    local spawn = scene:getObjects("spawn_plug13");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            addSpawnedEnemy(e, spawn[i]);
        end);
    end
end

local function spawnPlugHalfway()
    local spawn = scene:getObjects("spawn_plug_halfway");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
    local spawn = scene:getObjects("spawn_plug_halfway_mini");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], false, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            addSpawnedEnemy(e, spawn[i]);
        end);
    end
end

local function spawnPlugDone()
    local spawn = scene:getObjects("spawn_plug_done");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
end

local plugSpawnRules = {
    {9, 5, 0, spawnPlug9},
    {13, 11, 0, spawnPlug13}
};

local function updateLightsSingle(s)
    if s.myPlug == nil then
        return;
    end
    local i = s.myPlug.myI;
    if tube3Broken and (i == 3) then
        return;
    end
    if lights[i] ~= nil then
        lights[i].visible = true;
    end
    local s2 = sockets[i];
    if s2 == nil then
        return;
    end
    updateLightsSingle(s2);
end

local function updateLights()
    for i = 3, 16, 1 do
        if lights[i] ~= nil then
            lights[i].visible = false;
        end
    end
    updateLightsSingle(masterSocket1);
    updateLightsSingle(masterSocket2);
    plug1Light.visible = false;
    plug2Light.visible = false;
    plug17Light.visible = false;
    plug18Light.visible = false;
    if (plugs[1].mySocket ~= nil) and (plugs[0].mySocket ~= nil) and lights[plugs[0].mySocket.myI].visible then
        plug1Light.visible = true;
    end
    if (plugs[2].mySocket ~= nil) then
        plug2Light.visible = true;
    end
    if (plugs[17].mySocket ~= nil) and lights[plugs[17].mySocket.myI].visible then
        plug17Light.visible = true;
    end
    if (plugs[18].mySocket ~= nil) and lights[plugs[18].mySocket.myI].visible then
        plug18Light.visible = true;
    end
    if plug1Light.visible and plug2Light.visible then
        openDoor("door5", true);
        disableSocket(sockets[1]);
        disableSocket(sockets[2]);
        disableSocket(sockets[3]);
    else
        closeDoor("door5", true);
    end
    if plug17Light.visible and plug18Light.visible then
        openDoor("door7", true);
        disableSocket(masterSocket1);
        disableSocket(masterSocket2);
        for i = 4, 16, 1 do
            if sockets[i] ~= nil then
                disableSocket(sockets[i]);
            end
        end
        spawnPlugDone();
    else
        closeDoor("door7", true);
    end
    for _, v in pairs(plugSpawnRules) do
        if (v[3] == 0) and (plugs[v[1]].mySocket == sockets[v[2]]) then
            v[3] = 1;
            addTimeoutOnce(math.random(1, 3), v[4]);
        end
    end
    if (not halwayDone) and (plug17Light.visible or plug18Light.visible) then
        halwayDone = true;
        startMusic("action10.ogg", false);
        spawnPlugHalfway();
    end
end

local function explodeTube(fire)
    local obj = factory:createExplosion1(86);
    obj.pos = fire.pos;
    scene:addObject(obj);
    fire.visible = true;
end

-- main

makeDoor("door5", false);
makeDoor("door7", false);

makeSocket(masterSocket1);
makeSocket(masterSocket2);

for i = 0, 18, 1 do
    sockets[i] = scene:getObjects("socket"..i)[1];
    plugs[i] = scene:getObjects("plug"..i)[1];
    if sockets[i] ~= nil then
        makeSocket(sockets[i]);
        sockets[i].myI = i;
    end
    if plugs[i] ~= nil then
        makePlug(plugs[i], updateLights, updateLights);
        plugs[i].myI = i;
    end
    local inst = scene:getInstances("tube"..i)[1];
    if inst ~= nil then
        local obj = findObject(inst.objects, "tube");
        lights[i] = obj:findLightComponent():getLights("light")[1];
    end
end

setSensorEnterListener("gen1_cp", true, function(other)
    startMusic("action9.ogg", true);
    local tube = findObject(scene:getInstances("tube3")[1].objects, "tube");
    tube.glassy = true;
    scene.respawnPoint = scene:getObjects("gen1_cp")[1]:getTransform();
    addTimeoutOnce(0.25, function()
        tube3Broken = true;
        updateLights();
    end);
    local objs = scene:getObjects("fire3");
    for i = 1, #objs, 1 do
        addTimeoutOnce(0.5 * (i - 1), explodeTube, objs[i]);
    end
    addTimeoutOnce((#objs - 1) * 0.5 + 2.0, function()
        tube.glassy = false;
    end);
    local spawn = scene:getObjects("spawn5");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createOrbo(), obj);
    end
end);

setSensorEnterListener("gen2_cp", true, function(other)
    local spawn = scene:getObjects("spawn6_orbo");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createOrbo(), spawn[i]);
        end);
    end
    local spawn = scene:getObjects("spawn6_oth");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], false, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            addSpawnedEnemy(e, spawn[i]);
        end);
    end
end);

setSensorEnterListener("gen3_cp", true, function(other)
    local spawn = scene:getObjects("spawn7");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createWarder(), spawn[i]);
        end);
    end
    local spawn = scene:getObjects("spawn7_mini");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 3) == 3 then
            e = factory:createScorp2();
        else
            e = factory:createTetrocrab2();
        end
        addSpawnedEnemy(e, obj);
    end
end);

setSensorEnterListener("cool1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("cool1_cp")[1]:getTransform();
    startAmbientMusic(true);
end);
