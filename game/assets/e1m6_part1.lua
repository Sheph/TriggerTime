local spawn1 = scene:getObjects("spawn1");
local wavesDone = false;

local function waveN2(timeout)
    if wavesDone then
        return;
    end
    table.shuffle(spawn1);
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            e.name = "p1";
            e:setTransform(spawn1[i]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
    addTimeoutOnce(timeout - 0.5, waveN2, timeout - 0.5);
end

local function spawnAmmo1(timeout, n)
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local obj = factory:createPowerupAmmo(const.WeaponTypeMachineGun);
            obj.name = "p1";
            obj.pos = spawn1[i].pos + vec2(-1.5, 1.5);
            scene:addObject(obj);
            obj = factory:createPowerupAmmo(const.WeaponTypeMachineGun);
            obj.name = "p1";
            obj.pos = spawn1[i].pos + vec2(1.5, 1.5);
            scene:addObject(obj);
            obj = factory:createPowerupAmmo(const.WeaponTypeMachineGun);
            obj.name = "p1";
            obj.pos = spawn1[i].pos + vec2(0, -1.5);
            scene:addObject(obj);
        end);
    end
    addTimeoutOnce(10.0, waveN2, 15.0);
end

local function waveN(timeout, n)
    table.shuffle(spawn1);
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            e.name = "p1";
            e:setTransform(spawn1[i]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
    if n == 0 then
        addTimeoutOnce(timeout, spawnAmmo1);
    else
        addTimeoutOnce(timeout - 0.5, waveN, timeout - 0.5, n - 1);
    end
end

local function wave2()
    table.shuffle(spawn1);
    for i = 1, (#spawn1 - 1), 1 do
        local s = spawn1[i];
        summon1(s, i == 1, function()
            local e = factory:createEnforcer1();
            e.name = "p1";
            e:setTransform(s:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
    local s = spawn1[#spawn1];
    summon1(s, false, function()
        local e = factory:createWarder();
        e.name = "p1";
        e:setTransform(s:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end);
    addTimeoutOnce(10.0, waveN, 10.0, 4);
end

local function wave1()
    for i = 1, #spawn1, 1 do
        summon1(spawn1[i], i == 1, function()
            local e = factory:createEnforcer1();
            e.name = "p1";
            e:setTransform(spawn1[i]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
    addTimeoutOnce(10.0, wave2);
end

local function bridge1Done()
    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
    showUpperMsg(3.0, {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog67.str1}, nil);
end

-- main

setSensorEnterListenerWithAlly("core1_cp", function()
    scene.respawnPoint = scene:getObjects("core1_cp")[1]:getTransform();
    bridgeClose(bridge0, 10.0, false, function()
        addTimeoutOnce(2.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog68.str1},
                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog68.str2},
            }, function ()
                bridgeOpen(bridge1, 0.5, true, bridge1Done);
                scene.camera:findCameraComponent():follow(scene:getObjects("target1")[1], const.EaseOutQuad, 1.5);
                addTimeoutOnce(3.5, function()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                    scene.cutscene = false;
                    addTimeoutOnce(1.5, function()
                        startMusic("action8.ogg", true);
                        addTimeoutOnce(2.0, function()
                            wave1();
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("bridge1_cp", true, function(other)
    scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal1")[1].pos);
    wavesDone = true;
    scene.respawnPoint = scene:getObjects("bridge1_cp")[1]:getTransform();
    startAmbientMusic(true);
    bridgeClose(bridge1, 10.0, true, function()
        local objs = scene:queryObjects(scene:getObjects("arena0")[1].pos, 80.0, 80.0);
        for _, obj in pairs(objs) do
            if (obj.name == "p1") or (obj.type == const.SceneObjectTypeAlly) then
                obj:removeFromParent();
            end
        end
    end);
end);
