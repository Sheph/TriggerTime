local spawned = {};
local spawnersProcessed = false;
local keeper1 = scene:getObjects("keeper1")[1];
local bossSpawned = { keeper1 };
local keyl = scene:getObjects("keyl")[1];
local keylHack2 = false;

local function setupSpawn(name)
    setSensorEnterListener(name.."_cp", true, function(other)
        local objs = scene:getObjects(name.."_ef");
        for _, obj in pairs(objs) do
            local e = factory:createEnforcer1();
            e:setTransform(obj:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
            table.insert(spawned, e);
        end
        objs = scene:getObjects(name.."_sy");
        for _, obj in pairs(objs) do
            local e = factory:createSentry1(false);
            e:setTransformRecursive(obj:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
            table.insert(spawned, e);
        end
        objs = scene:getObjects(name.."_gr");
        for _, obj in pairs(objs) do
            local e = factory:createGorger1();
            e:setTransformRecursive(obj:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
            table.insert(spawned, e);
        end
        if name == "spawn5" then
            spawnersProcessed = true;
        end
    end);
end

function finalSequence()
    setAmbientMusic("ambient7.ogg");
    startAmbientMusic(true);
    scene:getObjects("keyl8_cp")[1].active = true;
    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal3")[1].pos);
    local p = scene:getObjects("keyl4_path")[1];
    keyl.pos = p.pos;
    keyl.roamBehavior:reset();
    keyl.roamBehavior.linearVelocity = 13.0;
    keyl.roamBehavior.linearDamping = 4.0;
    keyl.roamBehavior.dampDistance = 3.0;
    keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    keyl.roamBehavior:start();
end

-- main

setupSpawn("spawn1");
setupSpawn("spawn2");
setupSpawn("spawn3");
setupSpawn("spawn4");
setupSpawn("spawn5");

addTimeout(0.5, function(cookie)
    for k, obj in pairs(spawned) do
        if obj:dead() then
            spawned[k] = nil;
        end
    end
    if spawnersProcessed and (table.size(spawned) == 0) then
        cancelTimeout(cookie);
        local laser13 = scene:getObjects("laser13");
        for _, obj in pairs(laser13) do
            obj:addComponent(FadeOutComponent(1.0));
        end
    end
end);

setSensorEnterListener("exit3_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit3_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;

    local p = scene:getObjects("player1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 10.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("exit4_cp", true, function(other)
    addTimeoutOnce(0.5, function()
        scene.player.roamBehavior:reset();
        scene:getObjects("laser14")[1].active = true;
        scene:getObjects("laser14")[1].visible = true;

        keeper1:findKeeperComponent().autoTarget = true;
        addTimeoutOnce(0.5, function()
            openHatch("hatch1");
            addTimeoutOnce(0.5, function()
                keeper1:findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch("hatch1");
                    scene.cutscene = false;
                end);
            end);
        end);
    end);
end);

addTimeout(0.5, function(cookie)
    if keeper1:lifePercent() > 0.7 then
        return;
    end
    cancelTimeout(cookie);
    local obj = scene:getObjects("boss_spawn1_keeper")[1];
    local e = factory:createKeeper(false);
    e:setTransform(obj:getTransform());
    local ec = e:findTargetableComponent();
    ec.autoTarget = true;
    scene:addObject(e);
    table.insert(bossSpawned, e);
    local objs = scene:getObjects("boss_spawn1_enforcer");
    for _, obj in pairs(objs) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        table.insert(bossSpawned, e);
    end
end);

addTimeout(0.5, function(cookie)
    for k, obj in pairs(bossSpawned) do
        if obj:dead() then
            bossSpawned[k] = nil;
        end
    end
    if (table.size(bossSpawned) == 0) then
        cancelTimeout(cookie);
        addTimeoutOnce(4.0, function()
            finalSequence();
        end);
    end
end);

local function keylHack2Func()
    scene:getObjects("ga2")[1]:findGoalAreaComponent():removeAllGoals();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showUpperDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog54.str1},
        {"player", tr.dialog54.str2},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog54.str3},
    }, function ()
        keyl.angle = (scene:getObjects("control2")[1].pos - keyl.pos):angle();
        addTimeoutOnce(2.0, function()
            audio:playSound("alarm_off.ogg");
            local laser17 = scene:getObjects("laser17");
            for _, obj in pairs(laser17) do
                obj:addComponent(FadeOutComponent(1.0));
            end
            addTimeoutOnce(1.0, function()
                keyl.angle = (scene.player.pos - keyl.pos):angle();
                showUpperDialog(
                {
                    {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog55.str1}
                }, function ()
                    local p = scene:getObjects("keyl5_path")[1];
                    keyl.roamBehavior:reset();
                    keyl.roamBehavior.linearVelocity = 13.0;
                    keyl.roamBehavior.linearDamping = 4.0;
                    keyl.roamBehavior.dampDistance = 3.0;
                    keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                    keyl.roamBehavior:start();

                    local p = scene:getObjects("player2_path")[1];
                    scene.player.roamBehavior:reset();
                    scene.player.roamBehavior.linearVelocity = 15.0;
                    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                    scene.player.roamBehavior:start();
                end);
            end);
        end);
    end);
end

setSensorEnterListener("keyl7_cp", true, function(other)
    if keylHack2 then
        keylHack2Func();
    end
    keylHack2 = true;
end);

setSensorListener("keyl8_cp", function(other)
    if keylHack2 then
        keylHack2Func();
    end
    keylHack2 = true;
end, function (other)
    keylHack2 = false;
end);

setSensorEnterListener("keyl9_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        showUpperDialog(
        {
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog56.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog56.str2},
        }, function ()
            addTimeoutOnce(1.0, function()
                audio:playSound("alarm_off.ogg");
                local laser16 = scene:getObjects("laser16");
                for _, obj in pairs(laser16) do
                    obj:addComponent(FadeOutComponent(1.0));
                end
                addTimeoutOnce(1.0, function()
                    keyl.angle = (scene.player.pos - keyl.pos):angle();
                    showUpperDialog(
                    {
                        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog57.str1},
                        {"player", tr.dialog57.str2},
                    }, function ()
                        local p = scene:getObjects("keyl6_path")[1];
                        keyl.roamBehavior:reset();
                        keyl.roamBehavior.linearVelocity = 13.0;
                        keyl.roamBehavior.linearDamping = 4.0;
                        keyl.roamBehavior.dampDistance = 3.0;
                        keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        keyl.roamBehavior:start();

                        local p = scene:getObjects("player3_path")[1];
                        scene.player.roamBehavior:reset();
                        scene.player.roamBehavior.linearVelocity = 11.0;
                        scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        scene.player.roamBehavior:start();
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("exit5_cp", true, function(other)
    completeLevel(1.5, tr.str29, tr.str30, "e1m6");
end);
