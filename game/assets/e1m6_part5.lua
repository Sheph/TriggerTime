local spawn1 = scene:getObjects("spawn1");
local wavesDone = false;

local function wave5()
    if wavesDone then
        return;
    end
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e = factory:createEnforcer1();
            e.name = "p1";
            addSpawnedEnemy(e, spawn1[i]);
        end);
    end
    addTimeoutOnce(6.0, wave5);
end

local function wave4()
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e = factory:createWarder();
            e.name = "p1";
            addSpawnedEnemy(e, spawn1[i]);
        end);
    end
    addTimeoutOnce(8.0, wave5);
end

local function wave3()
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e = factory:createOrbo();
            e.name = "p1";
            addSpawnedEnemy(e, spawn1[i]);
        end);
    end
    addTimeoutOnce(8.0, wave4);
end

local function spawnAmmo1(timeout, n)
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local obj = factory:createPowerupAmmo(const.WeaponTypePlasmaGun);
            obj.name = "p1";
            obj.pos = spawn1[i].pos;
            scene:addObject(obj);
        end);
    end
    addTimeoutOnce(10.0, wave3);
end

local function wave1(num, n)
    table.shuffle(spawn1);
    for i = 1, num, 1 do
        local s = spawn1[i];
        summon2(s, i == 1, function()
            local e = factory:createGorger1();
            e.name = "p1";
            addSpawnedEnemy(e, s);
        end);
    end
    if n == 1 then
        addTimeoutOnce(10.0, spawnAmmo1);
    else
        addTimeoutOnce(15.0, wave1, num + 1, n - 1);
    end
end

local function bridge2Done()
    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal4")[1].pos);
    showUpperMsg(3.0, {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog71.str1}, nil);
end

-- main

setSensorEnterListener("core2_cp", true, function()
    scene.respawnPoint = scene:getObjects("core2_cp")[1]:getTransform();
    bridgeClose(bridge1, 10.0, false, function()
        addTimeoutOnce(2.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog72.str1},
            }, function ()
                bridgeOpen(bridge2, 0.9, true, bridge2Done);
                scene.camera:findCameraComponent():follow(scene:getObjects("target2")[1], const.EaseOutQuad, 1.5);
                addTimeoutOnce(3.5, function()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                    scene.cutscene = false;
                    startMusic("action8.ogg", true);
                    addTimeoutOnce(2.0, function()
                        wave1(2, 2);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("bridge2_cp", true, function(other)
    scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal4")[1].pos);
    wavesDone = true;
    scene.respawnPoint = scene:getObjects("bridge2_cp")[1]:getTransform();
    startAmbientMusic(true);
    bridgeClose(bridge2, 10.0, true, function()
        local objs = scene:queryObjects(scene:getObjects("arena0")[1].pos, 80.0, 80.0);
        for _, obj in pairs(objs) do
            if (obj.name == "p1") or (obj.type == const.SceneObjectTypeAlly) then
                obj:removeFromParent();
            end
        end
    end);
end);
