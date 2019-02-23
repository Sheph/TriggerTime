local eshield1 = scene:getObjects("eshield1");
local eshield1Spawned = {};
local gun1 = scene:getInstances("gun1");
local gun2 = scene:getInstances("gun2");
local gun3 = scene:getInstances("gun3");
local keeper1 = scene:getObjects("keeper1")[1];
local control1 = scene:getObjects("control1_ph")[1]:findPlaceholderComponent();

local function makeGun1(insts)
    local hs = true;
    for _, inst in pairs(insts) do
        local obj = findObject(inst.objects, "gun1_root");
        local dummy = obj:findDummyComponent("missile");
        inst.weapon = WeaponPlasmaGunComponent(const.SceneObjectTypeEnemyMissile);
        inst.weapon.useFilter = true;
        inst.weapon.useFreeze = false;
        inst.weapon.pos = dummy.pos;
        inst.weapon.angle = dummy.angle;
        inst.weapon.damage = 7.0;
        inst.weapon.velocity = 50.0;
        inst.weapon.spreadAngle = math.rad(3.0);
        inst.weapon.numShots = 1;
        inst.weapon.interval = 0.1;
        inst.weapon.haveSound = hs;
        hs = false;
        obj:addComponent(inst.weapon);
    end
end

local function makeGun2(insts)
    local hs = true;
    for _, inst in pairs(insts) do
        local obj = findObject(inst.objects, "gun2_root");
        local dummy = obj:findDummyComponent("missile");
        inst.weapon = WeaponMachineGunComponent(const.SceneObjectTypeEnemyMissile);
        inst.weapon.useFilter = true;
        inst.weapon.useFreeze = false;
        inst.weapon.pos = dummy.pos;
        inst.weapon.angle = dummy.angle;
        inst.weapon.damage = 7.0;
        inst.weapon.velocity = 100.0;
        inst.weapon.spreadAngle = math.rad(8.0);
        inst.weapon.turnInterval = 0;
        inst.weapon.turnDuration = 0;
        inst.weapon.loopDelay = 0.08;
        inst.weapon.haveSound = hs;
        hs = false;
        obj:addComponent(inst.weapon);
    end
end

function postStation3()
    openAirlock("door5", true);
    openAirlock("door6", false);
    fixedSpawn("station3_e4");
end

-- main

makeStairs("stairs1", function()
    scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};
end, function()
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
end);

makeStairs("stairs2", function()
    scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};
end, function()
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
end);

makeAirlock("door2", false);
makeAirlockTrigger("door2_cp", "door2");

setSensorEnterListener("eshield1_cp", false, function(other)
    for i, obj in pairs(eshield1Spawned) do
        if obj:scene() == nil then
            eshield1Spawned[i] = nil;
        end
    end
    local numEShields = #eshield1 - math.ceil(other:findPlayerComponent():ammo(const.WeaponTypeEShield) / 3);
    for i = 1,#eshield1,1 do
        if eshield1Spawned[i] ~= nil then
            numEShields = numEShields - 1;
        end
    end
    for i = 1,#eshield1,1 do
        if numEShields <= 0 then
            break;
        end
        if eshield1Spawned[i] == nil then
            local obj = factory:createPowerupAmmo(const.WeaponTypeEShield);
            obj:setTransform(eshield1[i]:getTransform());
            scene:addObject(obj);
            eshield1Spawned[i] = obj;
            numEShields = numEShields - 1;
        end
    end
end);

setSensorEnterListener("eshield2_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog96.str1},
        {"player", tr.dialog96.str2},
    }, function ()
        scene.cutscene = false;
        scene.inputPlayer.secondarySwitchShowOff = true;
    end);
end);

makeAirlock("door3", false);
makeAirlockTrigger("door3_cp", "door3");

makeGun1(gun1);

setSensorListener("gun1_cp", function(other, self)
    for _, inst in pairs(gun1) do
        inst.weapon:reload();
        inst.weapon:trigger(true);
    end
end, function (other, self)
    for _, inst in pairs(gun1) do
        inst.weapon:trigger(false);
    end
end);

makeGun2(gun2);
makeGun2(gun3);

setSensorListener("gun2_cp", function(other, self)
    local which = true;
    local function trigger(cookie)
        local a, b;
        if which then
            a = gun2;
            b = gun3;
        else
            a = gun3;
            b = gun2;
        end
        which = not which;
        for _, inst in pairs(a) do
            inst.weapon:trigger(false);
        end
        self.timer = addTimeoutOnce(0.2, function()
            for _, inst in pairs(b) do
                inst.weapon:reload();
                inst.weapon:trigger(true);
            end
            self.timer = addTimeoutOnce(1.0, trigger);
        end);
    end
    for _, inst in pairs(gun2) do
        inst.weapon:reload();
        inst.weapon:trigger(true);
    end
    self.timer = addTimeoutOnce(1.0, trigger);
end, function (other, self)
    cancelTimeout(self.timer);
    for _, inst in pairs(gun2) do
        inst.weapon:trigger(false);
    end
    for _, inst in pairs(gun3) do
        inst.weapon:trigger(false);
    end
end, { timer = nil });

makeAirlock("door4", true);

setSensorEnterListener("surprise1_cp", true, function(other)
    closeAirlock("door4", true);
    scene.respawnPoint = scene:getObjects("surprise1_cp")[1]:getTransform();
    local spawn = scene:getObjects("surprise1_s_gorger1");
    local enemies = {};
    for i = 1, #spawn, 1 do
        local e = factory:createGorger1();
        table.insert(enemies, e);
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemyFreezable(e, spawn[i]);
        end);
    end
    addTimeout(0.5, function(cookie)
        for _, obj in pairs(enemies) do
            if obj:alive() then
                return;
            end
        end
        cancelTimeout(cookie);
        addTimeoutOnce(1.0, function()
            audio:playSound("alarm_off.ogg");
            local laser1 = scene:getObjects("laser1");
            for _, obj in pairs(laser1) do
                obj:addComponent(FadeOutComponent(1.0));
            end
        end);
    end);
end);

makeAirlock("door5", false);
makeAirlock("door6", false);

control1.listener = createSensorEnterListener(true, function(other)
    control1.active = false;
    control1.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog97.str1},
            {"player", tr.dialog97.str2},
        }, function ()
            stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                scene.camera:findCameraComponent().target = scene:getObjects("cam_target1")[1];
                stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                    addTimeoutOnce(1.0, function()
                        openAirlock("door17", true);
                        addTimeoutOnce(2.0, function()
                            stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                                scene.camera:findCameraComponent().target = scene:getObjects("cam_target2")[1];
                                stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                                    addTimeoutOnce(1.0, function()
                                        openAirlock("door7", true);
                                        addTimeoutOnce(2.0, function()
                                            stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                                                scene.camera:findCameraComponent().target = scene.player;
                                                stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                                                    scene.cutscene = false;
                                                    addTimeoutOnce(1.0, function()
                                                        for i = 1, 3, 1 do
                                                            addTimeoutOnce((i - 1) * 1.0, function()
                                                                summon2(scene:getObjects("surprise2_homer"..i)[1], true, function()
                                                                    addSpawnedEnemyFreezable(factory:createHomer(), scene:getObjects("surprise2_homer"..i)[1]);
                                                                end);
                                                            end);
                                                        end
                                                        for i = 4, 6, 1 do
                                                            addTimeoutOnce(7.0 + (i - 4) * 1.0, function()
                                                                summon2(scene:getObjects("surprise2_homer"..i)[1], true, function()
                                                                    addSpawnedEnemyFreezable(factory:createHomer(), scene:getObjects("surprise2_homer"..i)[1]);
                                                                end);
                                                            end);
                                                        end
                                                        addTimeoutOnce(13.0, function()
                                                            keeper1:findKeeperComponent().autoTarget = true;
                                                            addTimeoutOnce(0.5, function()
                                                                openHatch("hatch1");
                                                                addTimeoutOnce(0.5, function()
                                                                    keeper1:findKeeperComponent():crawlOut();
                                                                    addTimeoutOnce(1.8, function()
                                                                        closeHatch("hatch1");
                                                                    end);
                                                                end);
                                                            end);
                                                            addTimeout(0.25, function(cookie)
                                                                if keeper1:lifePercent() > 0.5 then
                                                                    return;
                                                                end
                                                                cancelTimeout(cookie);
                                                                for i = 7, 9, 1 do
                                                                    addTimeoutOnce((i - 7) * 1.0, function()
                                                                        summon2(scene:getObjects("surprise2_homer"..i)[1], true, function()
                                                                            addSpawnedEnemyFreezable(factory:createHomer(), scene:getObjects("surprise2_homer"..i)[1]);
                                                                        end);
                                                                    end);
                                                                end
                                                            end);
                                                            addTimeout(0.25, function(cookie)
                                                                if keeper1:alive() then
                                                                    return;
                                                                end
                                                                cancelTimeout(cookie);
                                                                addTimeoutOnce(1.0, function()
                                                                    postStation3();
                                                                end);
                                                            end);
                                                        end);
                                                    end);
                                                end);
                                            end);
                                        end);
                                    end);
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);
