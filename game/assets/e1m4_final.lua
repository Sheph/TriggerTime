local redOpened = false;
local yellowOpened = false;
local display2 = nil;
local enemies = {};
local gorgers = {};
local nextWave = nil;
local checkTimer = nil;

local function waveSetup(name, next)
    nextWave = next;
    local tmp = scene:getObjects(name.."_gorger");
    for _, obj in pairs(tmp) do
        local e = factory:createGorger1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        table.insert(enemies, e);
        table.insert(gorgers, e);
    end
    tmp = scene:getObjects(name.."_sentry");
    for _, obj in pairs(tmp) do
        local e = factory:createSentry1(false);
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        table.insert(enemies, e);
    end
    tmp = scene:getObjects(name.."_enforcer");
    for _, obj in pairs(tmp) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        table.insert(enemies, e);
    end
end

local function finalSequence()
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local tmp = scene:getObjects("end_enforcer");
    for _, obj in pairs(tmp) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findEnforcerComponent();
        ec.patrol = true;
        ec.target = scene.player;
        ec.targetDistance = 15.0;
        scene:addObject(e);
    end
    local tmp = scene:getObjects("end_sentry");
    for _, obj in pairs(tmp) do
        local e = factory:createSentry1(false);
        e:setTransform(obj:getTransform());
        local ec = e:findSentryComponent();
        ec.patrol = true;
        ec.target = scene.player;
        ec.targetDistance = 15.0;
        scene:addObject(e);
    end
    addTimeoutOnce(8.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("display2_target")[1], const.EaseOutQuad, 2.0);
        addTimeoutOnce(2.5, function()
            display2:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.0, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog38.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog38.str2},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog38.str3},
                }, function ()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 2.0);
                    completeLevel(3.0, tr.str28, tr.str29, "e1m5");
                end);
            end);
        end);
    end);
end

local function wavesDone()
    print("waves done");
    cancelTimeout(checkTimer);
    addTimeout(0.5, function(cookie)
        for _, obj in pairs(enemies) do
            if obj:alive() then
                return;
            end
        end
        cancelTimeout(cookie);
        print("enemies dead, final sequence");
        addTimeoutOnce(1.0, function()
            finalSequence();
        end);
    end);
end

local function wave4()
    print("wave4");
    waveSetup("boss4", wavesDone);
end

local function wave3()
    print("wave3");
    waveSetup("boss3", wave4);
end

local function wave2()
    print("wave2");
    waveSetup("boss2", wave3);
end

local function wave1()
    print("wave1");
    nextWave = wave2;
    gorgers = scene:getObjects("boss1_gorger");
    for _, obj in pairs(gorgers) do
        obj:findTargetableComponent().autoTarget = true;
        table.insert(enemies, obj);
    end
    addTimeoutOnce(1.6, function()
        local objs = scene:getObjects("boss1_other");
        for _, obj in pairs(objs) do
            obj:findTargetableComponent().autoTarget = true;
            table.insert(enemies, obj);
        end
    end);
    addTimeoutOnce(2.0, function()
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
        scene.cutscene = false;
        addTimeoutOnce(15.0, function()
            display2:startAnimation(const.AnimationDefault);
        end);
    end);
    checkTimer = addTimeout(0.5, function(cookie)
        local tmp = {};
        for _, obj in pairs(gorgers) do
            if obj:alive() then
                table.insert(tmp, obj);
            end
        end
        if #tmp <= 1 then
            gorgers = tmp;
            nextWave();
        end
    end);
end

-- main

makeAirlock("door30", false);

makeKeyDoor("red_door", "red_key_ph", function()
    redOpened = true;
    if yellowOpened then
        openAirlock("door30", true);
    end
end);

makeKeyDoor("yellow_door", "yellow_key_ph", function()
    yellowOpened = true;
    if redOpened then
        openAirlock("door30", true);
    end
end);

setSensorEnterListener("pre_lab_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog39.str1},
        {"player", tr.dialog39.str2},
    }, function ()
        scene.cutscene = false;
    end);
end);

makeAirlock("door31", true);

display2 = setupDisplay("display2");

setSensorEnterListener("lab_cp", true, function(other)
    stopAllConveyors = true;
    closeAirlock("door31", true);
    scene.respawnPoint = scene:getObjects("lab_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    startMusic("action7.ogg", true);
    addTimeoutOnce(2.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("display2_target")[1], const.EaseOutQuad, 3.0);
        addTimeoutOnce(4.5, function()
            display2:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.0, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog40.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog40.str2},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog40.str3},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog40.str4},
                }, function ()
                    wave1();
                end);
            end);
        end);
    end);
end);
