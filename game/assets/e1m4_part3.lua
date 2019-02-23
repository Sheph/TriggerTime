local control5 = scene:getObjects("control5_ph")[1]:findPlaceholderComponent();
local helper1 = nil;
local enemies = {};
local deadbods = {};
local checkTimer = nil;
local helperTimer = nil;
local doorStates = {
    ["door18"] = false,
    ["door19"] = true,
    ["door20"] = false,
    ["door21"] = true,
    ["door22"] = false,
    ["door23"] = true,
    ["door24"] = false,
    ["door25"] = true,
    ["door26"] = false
};

local setupAll;

local function checkHelper(cookie)
    if helper1:alive() then
        return;
    end
    cancelTimeout(cookie);
    if checkTimer ~= nil then
        cancelTimeout(checkTimer);
    end
    addTimeoutOnce(1.5, function()
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 2.0, function()
            for _, obj in pairs(enemies) do
                obj:removeFromParent();
            end
            for _, obj in pairs(deadbods) do
                obj:removeFromParent();
            end
            deadbods = {};
            enemies = {};
            for k, state in pairs(doorStates) do
                if state then
                    openAirlock(k, false);
                else
                    closeAirlock(k, false);
                end
            end
            local objs = scene:getObjects("helper1_crate");
            for _, obj in pairs(objs) do
                obj:removeFromParent();
            end
            scene.respawnPoint = scene:getObjects("helper1_cp")[1]:getTransform();
            scene.player:setTransform(scene.respawnPoint);
            setupAll();
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 2.0);
        end);
    end);
end

local function checkWave(fn, ...)
    checkTimer = addTimeout(0.5, function(cookie, args)
        for _, obj in pairs(enemies) do
            if obj:alive() then
                return;
            end
        end
        for _, obj in pairs(enemies) do
            table.insert(deadbods, obj);
        end
        enemies = {};
        cancelTimeout(cookie);
        checkTimer = nil;
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

local function spawnEnforcers(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        enemies[e.cookie] = e;
    end
end

local function spawnSentries(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local e = factory:createSentry1(false);
        e:setTransformRecursive(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        enemies[e.cookie] = e;
    end
end

local function setupRoom0()
    scene:instanciate("e1m4_helper0.json", scene:getObjects("helper1_0")[1]:getTransform());

    helper1 = factory:createSentry2(true);
    helper1:setTransformRecursive(scene:getObjects("helper1")[1]:getTransform());
    scene:addObject(helper1);
    helperTimer = addTimeout(0.5, checkHelper);

    control5.active = true;
    control5.visible = true;
    control5.listener = createSensorEnterListener(true, function(other)
        control5.active = false;
        control5.visible = false;
        scene.player.linearDamping = 6.0;
        scene.player.angle = (scene:getObjects("control5_ph")[1].pos - scene.player.pos):angle();
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog44.str1},
            {"player", tr.dialog44.str2},
            {"player", tr.dialog44.str3},
        }, function ()
            scene.player.angle = (scene:getObjects("helper1")[1].pos - scene.player.pos):angle();
            addTimeoutOnce(1.0, function()
                scene:addCombatAlly(helper1);
                helper1:findSentryComponent():unfold();
                helper1:findSentryComponent().patrol = true;
                helper1:findSentryComponent().target = scene:getObjects("ally1_cp")[1];
            end);
        end);
    end);
    scene:getObjects("ally1_cp")[1].active = false;
    scene:getObjects("ally1_cp")[1].active = true;
    setSensorEnterListener("ally1_cp", true, function(other)
        addTimeoutOnce(2.0, function()
            helper1:findSentryComponent().target = scene:getObjects("ally2_cp")[1];
        end);
    end);
    scene:getObjects("ally2_cp")[1].active = false;
    scene:getObjects("ally2_cp")[1].active = true;
    setSensorEnterListener("ally2_cp", true, function(other)
        scene.cutscene = false;
        addTimeoutOnce(2.0, function()
            openAirlock("door18", true);
            addTimeoutOnce(0.5, function()
                helper1:findSentryComponent().target = scene:getObjects("ally3_cp")[1];
            end);
        end);
    end);
end

local function setupRoom1()
    scene:instanciate("e1m4_helper1.json", scene:getObjects("ally3_cp")[1]:getTransform());

    local started = false;
    local allyInside = false;
    local playerInside = false;

    local function room1Done()
        helper1:findSentryComponent().autoTarget = false;
        helper1:findSentryComponent().patrol = true;
        helper1:findSentryComponent().target = scene:getObjects("ally4_cp")[1];
        scene:getObjects("ally4_cp")[1].active = false;
        scene:getObjects("ally4_cp")[1].active = true;
        setSensorEnterListener("ally4_cp", true, function(other)
            addTimeoutOnce(2.0, function()
                openAirlock("door20", true);
                addTimeoutOnce(0.5, function()
                    helper1:findSentryComponent().target = scene:getObjects("ally5_cp")[1];
                end);
            end);
        end);
    end

    local function wave2()
        spawnEnforcers("room1_1");
        spawnSentries("room1_2");
        checkWave(room1Done);
    end

    local function wave1()
        if started then
            return;
        end
        scene.respawnPoint = scene:getObjects("player3_cp")[1]:getTransform();
        started = true;
        closeAirlock("door19", true);
        addTimeoutOnce(1.0, function()
            helper1:findSentryComponent().patrol = false;
            helper1:findSentryComponent().autoTarget = true;
            spawnEnforcers("room1_1");
            checkWave(wave2);
        end);
    end

    scene:getObjects("ally3_cp")[1].active = false;
    scene:getObjects("ally3_cp")[1].active = true;
    setSensorEnterListener("ally3_cp", true, function(other)
        allyInside = true;
        if playerInside then
            wave1();
        end
    end);
    setSensorListener("player3_cp", function(other, self)
        playerInside = true;
        if allyInside then
            wave1();
        end
    end, function (other, self)
        playerInside = false;
    end);
end

local function setupRoom2()
    scene:instanciate("e1m4_helper2.json", scene:getObjects("ally5_cp")[1]:getTransform());

    local started = false;
    local allyInside = false;
    local playerInside = false;

    local function room2Done()
        helper1:findSentryComponent().autoTarget = false;
        helper1:findSentryComponent().patrol = true;
        helper1:findSentryComponent().target = scene:getObjects("ally6_cp")[1];
        scene:getObjects("ally6_cp")[1].active = false;
        scene:getObjects("ally6_cp")[1].active = true;
        setSensorEnterListener("ally6_cp", true, function(other)
            addTimeoutOnce(2.0, function()
                openAirlock("door22", true);
                addTimeoutOnce(0.5, function()
                    helper1:findSentryComponent().target = scene:getObjects("ally7_cp")[1];
                end);
            end);
        end);
    end

    local function wave3()
        spawnSentries("room2_1");
        spawnEnforcers("room2_2");
        spawnEnforcers("room2_3");
        checkWave(room2Done);
    end

    local function wave2()
        spawnEnforcers("room2_2");
        checkWave(wave3);
    end

    local function wave1()
        if started then
            return;
        end
        scene.respawnPoint = scene:getObjects("player4_cp")[1]:getTransform();
        started = true;
        closeAirlock("door21", true);
        addTimeoutOnce(2.0, function()
            helper1:findSentryComponent().patrol = false;
            helper1:findSentryComponent().autoTarget = true;
            spawnSentries("room2_1");
            checkWave(wave2);
        end);
    end

    scene:getObjects("ally5_cp")[1].active = false;
    scene:getObjects("ally5_cp")[1].active = true;
    setSensorEnterListener("ally5_cp", true, function(other)
        allyInside = true;
        if playerInside then
            wave1();
        end
    end);
    setSensorListener("player4_cp", function(other, self)
        playerInside = true;
        if allyInside then
            wave1();
        end
    end, function (other, self)
        playerInside = false;
    end);
end

local function setupRoom3()
    scene:instanciate("e1m4_helper3.json", scene:getObjects("ally7_cp")[1]:getTransform());

    local started = false;
    local allyInside = false;
    local playerInside = false;

    local function room3Done()
        helper1:findSentryComponent().autoTarget = false;
        helper1:findSentryComponent().patrol = true;
        helper1:findSentryComponent().target = scene:getObjects("ally8_cp")[1];
        scene:getObjects("ally8_cp")[1].active = false;
        scene:getObjects("ally8_cp")[1].active = true;
        setSensorEnterListener("ally8_cp", true, function(other)
            addTimeoutOnce(2.0, function()
                openAirlock("door24", true);
                addTimeoutOnce(0.5, function()
                    helper1:findSentryComponent().target = scene:getObjects("ally9_cp")[1];
                end);
            end);
        end);
    end

    local function wave2()
        spawnSentries("room3_3");
        checkWave(room3Done);
    end

    local function wave1()
        if started then
            return;
        end
        scene.respawnPoint = scene:getObjects("player5_cp")[1]:getTransform();
        started = true;
        closeAirlock("door23", true);
        addTimeoutOnce(1.0, function()
            helper1:findSentryComponent().patrol = false;
            helper1:findSentryComponent().autoTarget = true;
            spawnEnforcers("room3_1");
            spawnSentries("room3_2");
            checkWave(wave2);
        end);
    end

    scene:getObjects("ally7_cp")[1].active = false;
    scene:getObjects("ally7_cp")[1].active = true;
    setSensorEnterListener("ally7_cp", true, function(other)
        allyInside = true;
        if playerInside then
            wave1();
        end
    end);
    setSensorListener("player5_cp", function(other, self)
        playerInside = true;
        if allyInside then
            wave1();
        end
    end, function (other, self)
        playerInside = false;
    end);
end

local function setupRoom4()
    local started = false;
    local allyInside = false;
    local playerInside = false;

    local function wave1()
        if started then
            return;
        end
        scene.respawnPoint = scene:getObjects("player6_cp")[1]:getTransform();
        started = true;
        closeAirlock("door25", true);
        addTimeoutOnce(1.0, function()
            helper1:findSentryComponent().patrol = false;
            helper1:findSentryComponent().autoTarget = true;
            openAirlock("door26", true);
            local objs = scene:getObjects("room4_1");
            local ens = {};
            for _, obj in pairs(objs) do
                local e = factory:createSentry1(false);
                e:addComponent(factory:createInvulnerability(2.0, -1.0, 20));
                e:setTransformRecursive(obj:getTransform());
                local ec = e:findTargetableComponent();
                ec.target = helper1;
                scene:addObject(e);
                table.insert(ens, e);
            end
            cancelTimeout(helperTimer);
            addTimeout(0.5, function(cookie)
                if helper1:alive() then
                    return;
                end
                cancelTimeout(cookie);
                for _, obj in pairs(ens) do
                    obj:findTargetableComponent().target = nil;
                    obj:findTargetableComponent().autoTarget = true;
                    obj:findInvulnerabilityComponent().duration = 0.5;
                end
                addTimeout(0.5, function(cookie)
                    for _, obj in pairs(ens) do
                        if obj:alive() then
                            return;
                        end
                    end
                    cancelTimeout(cookie);
                    addTimeoutOnce(1.0, function()
                        openAirlock("door27", true);
                    end);
                end);
            end);
        end);
    end

    scene:getObjects("ally9_cp")[1].active = false;
    scene:getObjects("ally9_cp")[1].active = true;
    setSensorEnterListener("ally9_cp", true, function(other)
        allyInside = true;
        if playerInside then
            wave1();
        end
    end);
    setSensorListener("player6_cp", function(other, self)
        playerInside = true;
        if allyInside then
            wave1();
        end
    end, function (other, self)
        playerInside = false;
    end);
end

setupAll = function()
    setupRoom0();
    setupRoom1();
    setupRoom2();
    setupRoom3();
    setupRoom4();
end

-- main

makeAirlock("door17", true);
makeAirlock("door27", false);
for k, state in pairs(doorStates) do
    makeAirlock(k, state);
end

setSensorEnterListener("helper1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("helper1_cp")[1]:getTransform();
    closeAirlock("door17", true);
end);

setupAll();

setSensorEnterListener("player7_cp", true, function(other)
    closeAirlock("door27", true);
    openAirlock("door11", false);
    scene.respawnPoint = scene:getObjects("player7_cp")[1]:getTransform();
    for _, obj in pairs(enemies) do
        obj:removeFromParent();
    end
    for _, obj in pairs(deadbods) do
        obj:removeFromParent();
    end
    deadbods = {};
    enemies = {};
end);

makeAirlock("door28", false);
makeAirlockTrigger("door28_cp", "door28");

if (settings.developer >= 13) and (settings.developer <= 16) then
    helper1:findSentryComponent():unfold();
    helper1:findSentryComponent().patrol = true;
    scene:addCombatAlly(helper1);
end

if settings.developer == 13 then
    helper1:findSentryComponent().target = scene:getObjects("ally3_cp")[1];
    helper1:setTransformRecursive(scene:getObjects("player3_cp")[1]:getTransform());
elseif settings.developer == 14 then
    helper1:findSentryComponent().target = scene:getObjects("ally5_cp")[1];
    helper1:setTransformRecursive(scene:getObjects("player4_cp")[1]:getTransform());
elseif settings.developer == 15 then
    helper1:findSentryComponent().target = scene:getObjects("ally7_cp")[1];
    helper1:setTransformRecursive(scene:getObjects("player5_cp")[1]:getTransform());
elseif settings.developer == 16 then
    helper1:findSentryComponent().target = scene:getObjects("ally9_cp")[1];
    helper1:setTransformRecursive(scene:getObjects("player6_cp")[1]:getTransform());
end
