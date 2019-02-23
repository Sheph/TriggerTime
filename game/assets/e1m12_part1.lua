local function createEnforcers(inst, name, e2Prob)
    local res = {};
    local spawn = findObjects(inst.objects, name);
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 1 / e2Prob) == 1 then
            e = factory:createEnforcer2();
        else
            e = factory:createEnforcer1();
        end

        e.freezable = obj.freezable;
        e.freezeRadius = obj.freezeRadius;
        e:setTransformRecursive(obj:getTransform());
        scene:addObject(e);

        table.insert(res, e);
    end
    return res;
end

local function activateEnemies(enemies)
    for _, obj in pairs(enemies) do
        obj:findTargetableComponent().autoTarget = true;
    end
end

-- main

-- NORMAL BLOCK

local normalBlocks = {"e1m12_block1.json", "e1m12_block2.json", "e1m12_block3.json", "e1m12_block4.json"};

mazeRegisterContent(normalBlocks, "e1m12_c1.json");

mazeRegisterContent(normalBlocks, "e1m12_c2.json", function(inst, key)
        makeFlickerLamps(inst, "flicker_lamp");
        inst.myEnemies = {};
        local spawn = findObjects(inst.objects, "spawn1");
        table.shuffle(spawn);
        local num = math.random(4, 6);
        for i = 1, num, 1 do
            local e;
            local r = math.random(1, 3);
            if r == 1 then
                e = factory:createEnforcer1();
            elseif r == 2 then
                e = factory:createEnforcer2();
            else
                e = factory:createWarder();
            end

            e.freezable = spawn[i].freezable;
            e.freezeRadius = spawn[i].freezeRadius;
            e:setTransformRecursive(spawn[i]:getTransform());
            scene:addObject(e);

            table.insert(inst.myEnemies, e);
        end
    end, function(inst)
        activateEnemies(inst.myEnemies);
    end);

mazeRegisterContent(normalBlocks, "e1m12_c3.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(3 + math.random() * 2, function()
            local spawn = findObjects(inst.objects, "spawn1");
            table.shuffle(spawn);
            local summonSound = true;
            local enemies = {};
            for i = 1, #spawn, 1 do
                if math.random(1, 2) == 1 then
                    enemies[i] = factory:createEnforcer1();
                else
                    enemies[i] = factory:createEnforcer2();
                end
                summon1(spawn[i], summonSound, function()
                    addSpawnedEnemyFreezable(enemies[i], spawn[i]);
                end);
                summonSound = false;
            end
            addTimeout(0.25, function(cookie)
                for _, obj in pairs(enemies) do
                    if obj:alive() then
                        return;
                    end
                end
                cancelTimeout(cookie);
                addTimeoutOnce(1.0, function()
                    audio:playSound("alarm_off.ogg");
                    local laser1 = findObjects(inst.objects, "laser1");
                    for _, obj in pairs(laser1) do
                        obj:addComponent(FadeOutComponent(1.0));
                    end
                end);
            end);
        end);
    end);

-- BLOCK 5

mazeRegisterContent({"e1m12_block5.json"},
    "e1m12_block5_c1.json", function(inst, key)
        inst.myEnemies = createEnforcers(inst, "enforcer1", 0.5);
    end, function(inst)
        activateEnemies(inst.myEnemies);
    end);

mazeRegisterContent({"e1m12_block5.json"},
    "e1m12_block5_c2.json", function(inst, key)
        local c = findObject(inst.objects, "content1");
        local bug1 = findObjects(inst.objects, "bug1");
        for _, obj in pairs(bug1) do
            obj:findTargetableComponent().target = c;
        end
    end, function(inst)
        local bug1 = findObjects(inst.objects, "bug1");
        for _, obj in pairs(bug1) do
            obj:findTargetableComponent().target = nil;
            obj:findTargetableComponent().autoTarget = true;
        end
    end);

-- BLOCK 6

mazeRegisterContent({"e1m12_block6.json"},
    "e1m12_block6_c1.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(math.random() * 2, function()
            if math.random(1, 2) == 1 then
                local spawn = findObject(inst.objects, "spawn1");
                summon2(spawn, true, function()
                    addSpawnedEnemyFreezable(factory:createGorger1(), spawn);
                end);
            else
                local spawn = findObjects(inst.objects, "spawn2");
                local summonSound = true;
                for i = 1, #spawn, 1 do
                    summon1(spawn[i], summonSound, function()
                        if math.random(1, 2) == 1 then
                            addSpawnedEnemyFreezable(factory:createOrbo(), spawn[i]);
                        else
                            addSpawnedEnemyFreezable(factory:createWarder(), spawn[i]);
                        end
                    end);
                    summonSound = false;
                end
            end
        end);
    end);

mazeRegisterContent({"e1m12_block6.json"},
    "e1m12_block6_c2.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(math.random() * 3, function()
            local sentry1 = findObjects(inst.objects, "sentry1");
            for _, obj in pairs(sentry1) do
                obj:findSentryComponent().autoTarget = true;
                obj:findSentryComponent():unfold();
            end
        end);
    end);

-- BLOCK 7

mazeRegisterContent({"e1m12_block7.json"}, "e1m12_block7_c1.json");

mazeRegisterContent({"e1m12_block7.json"}, "e1m12_block7_c2.json");

-- BLOCK 8

mazeRegisterContent({"e1m12_block8.json"}, "e1m12_block8_c1.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(math.random() * 1, function()
            local spawn = findObjects(inst.objects, "spawn1");
            local summonSound = true;
            for i = 1, #spawn, 1 do
                summon1(spawn[i], summonSound, function()
                    if math.random(1, 2) == 1 then
                        local tb = factory:createTetrobot();
                        addSpawnedEnemyFreezable(tb, spawn[i]);
                        tb:findPhysicsBodyComponent():setFilterGroupIndex(-300);
                    else
                        addSpawnedEnemyFreezable(factory:createWarder(), spawn[i]);
                    end
                end);
                summonSound = false;
            end
        end);
    end);

-- BLOCK 9

mazeRegisterContent({"e1m12_block9.json"}, "e1m12_block9_c1.json", function(inst, key)
        inst.myEnemies = {};
        local spawn = findObjects(inst.objects, "spawn1");
        for _, obj in pairs(spawn) do
            local e;
            if math.random(1, 2) == 1 then
                e = factory:createSentry1(true);
            else
                e = factory:createEnforcer1();
            end

            e.freezable = obj.freezable;
            e.freezeRadius = obj.freezeRadius;
            e:setTransformRecursive(obj:getTransform());
            scene:addObject(e);

            table.insert(inst.myEnemies, e);
        end
    end, function(inst)
        activateEnemies(inst.myEnemies);
        addTimeoutOnce(math.random() * 3, function()
            for _, obj in pairs(inst.myEnemies) do
                local c = obj:findSentryComponent();
                if c ~= nil then
                    c:unfold();
                end
            end
        end);
    end);

mazeRegisterContent({"e1m12_block9.json"},
    "e1m12_block9_c2.json", function(inst, key)
        makeFlickerLamps(inst, "flicker_lamp");
    end, function(inst)
        addTimeoutOnce(1 + math.random() * 1, function()
            local spawn = findObjects(inst.objects, "spawn1");
            local summonSound = true;
            for i = 1, #spawn, 1 do
                summon1(spawn[i], summonSound, function()
                    if math.random(1, 3) == 1 then
                        addSpawnedEnemyFreezable(factory:createHomer(), spawn[i]);
                    else
                        addSpawnedEnemyFreezable(factory:createEnforcer1(), spawn[i]);
                    end
                end);
                summonSound = false;
            end
        end);
    end);

-- BLOCK 10

mazeRegisterContent({"e1m12_block10.json"}, "e1m12_block10_c1.json", function(inst, key)
        makeFlickerLamps(inst, "flicker_lamp");
    end, function(inst)
    end);

mazeRegisterContent({"e1m12_block10.json"}, "e1m12_block10_c2.json", function(inst, key)
        local turret1 = findObjects(inst.objects, "turret1");
        for _, obj in pairs(turret1) do
            local hb = RenderHealthbarComponent(vec2(0.0, 2.0), 0, 4.0, 0.8, const.zOrderMarker + 1);
            hb.color = { 1.0, 1.0, 1.0, 0.6 };
            obj:addComponent(hb);
        end
    end, function(inst)
        local turret1 = findObjects(inst.objects, "turret1");
        for _, obj in pairs(turret1) do
            obj:findTargetableComponent().autoTarget = true;
        end
    end);

-- LONG BLOCK

local longBlocks = {"e1m12_longblock1.json", "e1m12_longblock2.json", "e1m12_longblock3.json", "e1m12_longblock4.json"};

mazeRegisterContent(longBlocks,
    "e1m12_long_c1.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(3 + math.random() * 5, function()
            local spawn = findObjects(inst.objects, "spawn1");
            table.shuffle(spawn);
            local summonSound = true;
            if math.random(1, 2) == 1 then
                for i = 1, 2, 1 do
                    summon2(spawn[i], summonSound, function()
                        addSpawnedEnemyFreezable(factory:createGorger1(), spawn[i]);
                    end);
                    summonSound = false;
                end
                addTimeoutOnce(5 + math.random() * 8, function()
                    local spawn2 = spawn;
                    table.shuffle(spawn2);
                    local summonSound = true;
                    for i = 1, 6, 1 do
                        summon1(spawn2[i], summonSound, function()
                            addSpawnedEnemyFreezable(factory:createEnforcer1(), spawn2[i]);
                        end);
                        summonSound = false;
                    end
                end);
            else
                for i = 1, 4, 1 do
                    summon1(spawn[i], summonSound, function()
                        addSpawnedEnemyFreezable(factory:createHomer(), spawn[i]);
                    end);
                    summonSound = false;
                end
                addTimeoutOnce(5 + math.random() * 8, function()
                    local spawn2 = spawn;
                    table.shuffle(spawn2);
                    local summonSound = true;
                    for i = 1, 4, 1 do
                        summon1(spawn2[i], summonSound, function()
                            addSpawnedEnemyFreezable(factory:createEnforcer2(), spawn2[i]);
                        end);
                        summonSound = false;
                    end
                end);
            end
        end);
    end);

mazeRegisterContent(longBlocks,
    "e1m12_long_c2.json", function(inst, key)
        local guns = inst:getInstances("gun1");
        inst.myGuns = {};
        local hs = true;
        for i = 1, #guns, 1 do
            local obj = findObject(guns[i].objects, "gun1_root");
            inst.myGuns[i] = WeaponFireballComponent(const.SceneObjectTypeEnemyMissile);
            inst.myGuns[i].useFilter = true;
            inst.myGuns[i].useFreeze = false;
            inst.myGuns[i].damage = 15.0;
            inst.myGuns[i].velocity = 13.0;
            inst.myGuns[i].interval = 1.0;
            inst.myGuns[i].haveSound = hs;
            obj:addComponent(inst.myGuns[i]);
            hs = false;
        end
        findObject(inst.objects, "gun1_cp"):findCollisionSensorComponent().listener = createSensorListener(function(other)
            for _, gun in pairs(inst.myGuns) do
                gun:reload();
                gun:trigger(true);
            end
        end, function (other)
            for _, gun in pairs(inst.myGuns) do
                gun:trigger(false);
            end
        end);
    end, function(inst)
    end);

mazeRegisterContent(longBlocks,
    "e1m12_long_c3.json", function(inst, key)
        local guns = inst:getInstances("gun2");
        for _, gun in pairs(guns) do
            makeGun2(gun, function()
            end);
        end
        local guns = inst:getInstances("gun3");
        for _, gun in pairs(guns) do
            makeGun2(gun, function()
                gun.myObj.angularVelocity = 0;
                gun.myTrunk.angularVelocity = 0;
            end);
            gun.myObj.angularVelocity = math.rad(30);
            gun.myTrunk.angularVelocity = math.rad(30);
        end
        findObject(inst.objects, "spawn_cp"):findCollisionSensorComponent().listener = createSensorEnterListener(true, function(other)
            addTimeoutOnce(math.random() * 2, function()
                local spawn = findObjects(inst.objects, "spawn1");
                table.shuffle(spawn);
                local summonSound = true;
                for i = 1, 5, 1 do
                    summon1(spawn[i], summonSound, function()
                        local tb = factory:createTetrobot();
                        addSpawnedEnemyFreezable(tb, spawn[i]);
                        tb:findPhysicsBodyComponent():setFilterGroupIndex(-300);
                    end);
                    summonSound = false;
                end
                addTimeoutOnce(5 + math.random() * 4, function()
                    local spawn = findObjects(inst.objects, "spawn1");
                    table.shuffle(spawn);
                    local summonSound = true;
                    for i = 1, 6, 1 do
                        summon1(spawn[i], summonSound, function()
                            local tb = factory:createTetrobot();
                            addSpawnedEnemyFreezable(tb, spawn[i]);
                            tb:findPhysicsBodyComponent():setFilterGroupIndex(-300);
                        end);
                        summonSound = false;
                    end
                end);
            end);
        end);
    end, function(inst)
    end);

-- BIG BLOCK

local bigBlocks = {"e1m12_bigblock1.json", "e1m12_bigblock2.json", "e1m12_bigblock3.json", "e1m12_bigblock4.json"};

mazeRegisterContent(bigBlocks,
    "e1m12_big_c1.json", function(inst, key)
    end, function(inst)
        addTimeoutOnce(5 + math.random() * 5, function()
            local spawn = findObjects(inst.objects, "spawn1");
            table.shuffle(spawn);
            local summonSound = true;

            for i = 1, 2, 1 do
                summon2(spawn[i], summonSound, function()
                    addSpawnedEnemyFreezable(factory:createGorger1(), spawn[i]);
                end);
                summonSound = false;
            end
        end);
        addTimeoutOnce(8.0 + math.random() * 10, function()
            findObject(inst.objects, "keeper1"):findKeeperComponent().autoTarget = true;
            openHatch(inst, "hatch1");
            addTimeoutOnce(0.5, function()
                findObject(inst.objects, "keeper1"):findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch(inst, "hatch1");
                end);
            end);
        end);
    end);

mazeRegisterContent(bigBlocks,
    "e1m12_big_c2.json", function(inst, key)
        local rc = findObject(inst.objects, "content1"):findRenderTerrainComponents("toxic1")[1];
        local ac = AnimationComponent(rc.drawable);
        ac:addAnimation(const.AnimationDefault, "toxic2", 1);
        ac:startAnimation(const.AnimationDefault);
        findObject(inst.objects, "content1"):addComponent(ac);
        findObject(inst.objects, "spawn_cp"):findCollisionSensorComponent().listener = createSensorEnterListener(true, function(other)
            local worm1 = findObjects(inst.objects, "worm1");
            table.shuffle(worm1);
            local t = 1 + math.random() * 2;
            for _, obj in pairs(worm1) do
                addTimeoutOnce(t, function()
                    obj:findTargetableComponent().autoTarget = true;
                end);
                t = t + 0.5;
            end
            local spawn = findObjects(inst.objects, "spawn1");
            table.shuffle(spawn);
            addTimeoutOnce(4 + math.random() * 4, function()
                summon2(spawn[1], true, function()
                    addSpawnedEnemyFreezable(factory:createGorger1(), spawn[1]);
                end);
                addTimeoutOnce(3 + math.random() * 4, function()
                    summon2(spawn[2], true, function()
                        addSpawnedEnemyFreezable(factory:createGorger1(), spawn[2]);
                    end);
                end);
            end);
        end);
    end, function(inst)
    end);

-- SPECIAL BLOCK

mazeRegisterSpecialContent({"e1m12_block1.json"},
    "e1m12_block1_s1.json", function(inst)
    end, function(inst)
    end);

mazeRegisterSpecialContent({"e1m12_block8.json"},
    "e1m12_block8_s1.json", function(inst)
    end, function(inst)
    end);

-- OTHER

addTimeout(15.0, function(cookie)
    if (not playerInDoor) and scene.player:alive() then
        scene.respawnPoint = scene.player:getTransform();
    end
end);
