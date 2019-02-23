local tentacle12 = scene:getObjects("tentacle12");
local tentacle12weld = weldTentacleBones(tentacle12, {3, 6, 9}, 1.5, 0.5);
local park4 = scene:getObjects("park4_ph")[1]:findPlaceholderComponent();
local winch0 = scene:getObjects("winch0")[1];
local winch1 = scene:getObjects("winch1")[1];
local winch2 = scene:getObjects("winch2")[1];
local winch3 = scene:getObjects("winch3")[1];
local boulder0 = scene:getObjects("boulder0")[1];
local boulder1 = scene:getObjects("boulder1")[1];
local boulder2 = scene:getObjects("boulder2")[1];
local boulder3 = scene:getObjects("boulder3")[1];
local chain0Hooks = {scene:getObjects("chain0_hook_front")[1], scene:getObjects("chain0_hook_back")[1]};
local chain0 = table.copy(scene:getObjects("chain0"), chain0Hooks);
local chain1Hooks = {scene:getObjects("chain1_hook_front")[1], scene:getObjects("chain1_hook_back")[1]};
local chain1 = table.copy(scene:getObjects("chain1"), chain1Hooks);
local tentacle13 = scene:getObjects("tentacle13");
local tentacle13weld = weldTentacleBones(tentacle13, {3, 6, 9}, 1.5, 0.4);
local dynomitePh = scene:getObjects("dynomite_ph")[1]:findPlaceholderComponent();

local function makeChain(name, chain, hooks)
    makeHooks(hooks, function(hook, eyelet, self)
        hook.myHooked = true;
        for _, obj in pairs(hooks) do
            if not obj.myHooked then
                return;
            end
        end
        for _, obj in pairs(chain) do
            obj.ggMassOverride = obj.mass;
            obj.ggInertiaOverride = obj.inertia;
            local c = obj:findPhysicsBodyComponent();
            obj.myDensity = c.density;
            c.density = 100.0;
            obj:resetMassData();
            obj.linearVelocity = vec2(0, 0);
            obj.angularVelocity = 0;
            c = obj:findCollisionCancelComponent();
            c.immediate = true;
        end
    end, function(hook, eyelet, self)
        for _, obj in pairs(hooks) do
            if not obj.myHooked then
                hook.myHooked = false;
                return;
            end
        end
        hook.myHooked = false;
        for _, obj in pairs(chain) do
            local c = obj:findPhysicsBodyComponent();
            c.density = obj.myDensity;
            obj:resetMassData();
            obj.linearVelocity = vec2(0, 0);
            obj.angularVelocity = 0;
            c = obj:findCollisionCancelComponent();
            c.immediate = false;
        end
        local joints = scene:getJoints(name.."_joint");
        for _, j in pairs(joints) do
            j:clone();
            j:remove();
        end
        for _, obj in pairs(hooks) do
            if obj.myHooked then
                local j = obj.myEyelet.j:clone();
                obj.myEyelet.j:remove();
                obj.myEyelet.j = j;
            end
        end
    end);
end

local function makeBoulder(boulder)
    makeEyeletHolder(boulder);
    scene:addFrictionJoint(scene:getObjects("terrain0")[1], boulder, vec2(0, 0), vec2(0, 0), 10000, 10000, true);
end

local function makeWinchLever(leverName, winchName, rev)
    makeLever(leverName, not rev, function()
        audio:playSound("lever_pull.ogg");
        scene:getJoints(winchName.."_joint")[1].motorEnabled = rev;
    end, function()
        audio:playSound("lever_pull.ogg");
        scene:getJoints(winchName.."_joint")[1].motorEnabled = not rev;
    end);
end

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
        local t = 1.0 + math.random() * 0.5;
        local f = 6000 + math.random() * 4000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 1.0 + math.random() * 0.5;
        local f = 50000 + math.random() * 40000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentaclePulse2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local timeF = 0.1 + math.random() * 0.1;
        local phaseF = 5.0 + math.random() * 2.0;
        local a1F = 0.3 + math.random() * 0.4;
        local a2F = 0.3 + math.random() * 0.4;
        obj:addComponent(TentaclePulseComponent(15, timeF, phaseF, a1F, a2F));
    end
end

local function makeTentacleSwing3(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 1.2 + math.random() * 0.5;
        local f = 150000 + math.random() * 70000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function spawnBeetleProb(name, normal, shootOnly, splitOnly, shootAndSplit)
    local spawn = scene:getObjects(name);
    local probs = {normal, shootOnly, splitOnly, shootAndSplit};
    local args = {{false, false}, {true, false}, {false, true}, {true, true}};
    local cargs = args[1];
    local lastSpawned = nil;
    for _, obj in pairs(spawn) do
        local val = math.random();
        local prob = 0;
        for i = 1, 4, 1 do
            prob = prob + probs[i];
            if val < prob then
                cargs = args[i];
                break;
            end
        end
        lastSpawned = factory:createBeetle1(unpack(cargs));
        addSpawnedEnemy(lastSpawned, obj);
    end
    return lastSpawned;
end

local function spawnCaveHoleCentipedes(indices)
    for _, i in pairs(indices) do
        local centipede = factory:createCentipede1();
        addSpawnedEnemy(centipede, scene:getObjects("centipede"..i)[1]);
        centipede:findPhysicsBodyComponent():setFilterGroupIndex(-800);
        local objs = centipede:getObjects();
        for _, obj in pairs(objs) do
            obj:findPhysicsBodyComponent():setFilterGroupIndex(-800);
        end
    end
end

local function spawnCaveHoleBeetles(indices, normal, shootOnly, splitOnly, shootAndSplit)
    for _, i in pairs(indices) do
        local obj = spawnBeetleProb("centipede"..i, normal, shootOnly, splitOnly, shootAndSplit);
        obj:findPhysicsBodyComponent():setFilterGroupIndex(-800);
    end
end

function stopPlatforms2()
    scene:getJoints("winch0_joint")[1].motorEnabled = false;
    scene:getJoints("winch1_joint")[1].motorEnabled = false;
    scene:getJoints("winch2_joint")[1].motorEnabled = false;
    scene:getJoints("winch3_joint")[1].motorEnabled = false;
end

-- main

makeTentacleFlesh("tentacle11");
makeTentacleSwing1("tentacle11");
makeTentaclePulse1("tentacle11");
makeTentacleFlesh("tentacle12");
makeTentacleSwing2("tentacle12");

makeAirlock("door12", true);

setSensorEnterListener("boat7_cp", true, function(other)
    closeAirlock("door12", true);
    startAmbientMusic(true);
    stopPlatforms1();
    addTimeoutOnce(1.0, function()
        if not scene.player:findPlayerComponent().flagDamagedByAcidWormOnBoat then
            gameShell:setAchieved(const.AchievementWormFeeling);
        end
    end);
end);

makeBoatPark(park4, "park4_exit");

setSensorEnterListener("boat8_cp", true, function(other)
    local ld = other.linearDamping;
    local ad = other.angularDamping;
    other.linearDamping = 6.0;
    other.angularDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.5, function()
        showLowerDialog(
        {
            {"player", tr.dialog84.str1},
        }, function ()
            scene.cutscene = false;
            other.linearDamping = ld;
            other.angularDamping = ad;
        end);
    end);
end);

makeDoor("door13", false);

makeEyeletHolder(winch0);
makeEyeletHolder(winch1);
makeEyeletHolder(winch2);
makeEyeletHolder(winch3);
makeBoulder(boulder0);
makeBoulder(boulder1);
makeBoulder(boulder2);
makeBoulder(boulder3);

makeChain("chain0", chain0, chain0Hooks);
makeChain("chain1", chain1, chain1Hooks);

makeTentacleFlesh("tentacle13");
makeTentacleSwing3("tentacle13");
makeTentaclePulse2("tentacle13");

makeWinchLever("lever3", "winch1", false);
makeWinchLever("lever4", "winch0", false);
makeWinchLever("lever5", "winch2", true);
makeWinchLever("lever6", "winch3", false);

setSensorListener("winch2_cp", function(other)
    scene.respawnPoint = scene:getObjects("winch2_cp")[1]:getTransform();
    scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
end, function(other)
    scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
end);

setSensorListener("cave_hole_cp", function(other)
end, function(other)
    local c = other:findPhysicsBodyComponent();
    if c ~= nil then
        c:setFilterGroupIndex(0);
    end
end);

setSensorEnterListener("cave1_cp", true, function(other)
    spawnBeetleProb("cave1_beetle", 0.5, 0.3, 0.1, 0.1);
end);

setSensorEnterListener("cave2_cp", true, function(other)
    spawnBeetleProb("cave2_beetle", 0.5, 0.2, 0.2, 0.1);
end);

setSensorEnterListener("winch1_cp", true, function(other)
    audio:playSound("squid_alert1.ogg");
    scene.respawnPoint = scene:getObjects("winch1_cp")[1]:getTransform();
    unweldTentacleBones(tentacle13weld);
    spawnCaveHoleCentipedes({1, 2, 3, 6});
end);

setSensorEnterListener("cave3_cp", true, function(other)
    local t = 0;
    for i = 1, 3, 1 do
        addTimeoutOnce(t, function()
            spawnCaveHoleBeetles({4, 5, 6}, 0.5, 0.3, 0.1, 0.1);
        end);
        t = t + 0.3;
    end
end);

setSensorEnterListener("cave4_cp", false, function(other)
    if other.name ~= "boulder0" then
        return;
    end
    spawnCaveHoleCentipedes({4});
    local t = 0;
    for i = 1, 2, 1 do
        addTimeoutOnce(t, function()
            spawnCaveHoleCentipedes({2, 3});
        end);
        t = t + 1.3;
    end
    scene:getObjects("cave4_cp")[1].active = false;
end);

setSensorEnterListener("cave5_cp", false, function(other)
    if other.name ~= "boulder1" then
        return;
    end
    spawnBeetleProb("cave5_beetle", 0.5, 0.2, 0.2, 0.1);
    fixedSpawn("cave5");
    scene:getObjects("cave5_cp")[1].active = false;
end);

setSensorEnterListener("cave6_cp", true, function(other)
    spawnCaveHoleCentipedes({7, 8});
end);

setSensorEnterListener("cave7_cp", true, function(other)
    spawnCaveHoleCentipedes({9, 10});
end);

setSensorEnterListener("cave8_cp", true, function(other)
    fixedSpawn("cave8");
end);

setSensorEnterListener("cave9_cp", true, function(other)
    spawnCaveHoleCentipedes({11, 12});
end);

setSensorEnterListener("cave10_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("cave10_cp")[1]:getTransform();
    spawnCaveHoleCentipedes({14, 15});
    spawnCaveHoleBeetles({13, 16, 17, 18, 19}, 0.5, 0.3, 0.1, 0.1);
end);

setSensorEnterListener("cave11_cp", true, function(other)
    spawnCaveHoleCentipedes({16, 18, 19, 20, 21});
end);

setSensorEnterListener("cave12_cp", true, function(other)
    spawnCaveHoleCentipedes({20, 21});
    local t = 0;
    for i = 1, 3, 1 do
        addTimeoutOnce(t, function()
            spawnCaveHoleBeetles({18, 20, 21}, 0.5, 0.3, 0.1, 0.1);
        end);
        t = t + 0.3;
    end
end);

setSensorEnterListener("cave13_cp", true, function(other)
    fixedSpawn("cave13");
end);

setSensorEnterListener("cave14_cp", true, function(other)
    spawnBeetleProb("cave14_beetle", 0.4, 0.2, 0.2, 0.2);
end);

makeDoor("door14", false);
makeDoor("door15", false);

makeLever("lever7", false, function()
    audio:playSound("lever_pull.ogg");
    openDoor("door15", true);
    openDoor("door14", false);
    disableLever("lever7");
end, function()
    audio:playSound("lever_pull.ogg");
end);

setSensorEnterListener("cave15_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("cave15_cp")[1]:getTransform();
    fixedSpawn("cave15");
end);

setSensorEnterListener("cave16_cp", true, function(other)
    spawnBeetleProb("cave16_beetle", 0.6, 0.2, 0.1, 0.1);
end);

setSensorEnterListener("cave17_cp", true, function(other)
    spawnBeetleProb("cave17_beetle", 0.6, 0.2, 0.1, 0.1);
end);

setSensorEnterListener("cave18_cp", true, function(other)
    fixedSpawn("cave18");
    spawnBeetleProb("cave18_beetle", 0.6, 0.2, 0.1, 0.1);
end);

setSensorEnterListener("cave19_cp", true, function(other)
    fixedSpawn("cave19");
    spawnBeetleProb("cave19_beetle", 0.4, 0.3, 0.2, 0.1);
end);

makeKeyDoor("blue_door", "blue_key_ph", function()
    openDoor("door13");
end);

setSensorEnterListener("cave20_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("cave20_cp")[1]:getTransform();
end);

setSensorEnterListener("cave21_cp", true, function(other)
    showUpperMsg(4.0, {"player", tr.dialog85.str1});
    scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("dynomite_ph")[1].pos);
end);

setSensorEnterListener("cave22_cp", true, function(other)
    scene:getObjects("cave_hole_blocker2")[1].active = false;
    local rocks = scene:getObjects("expl_boulder");
    local to = 0.5;
    local i = 1;
    while i <= #rocks do
        local ci = i;
        addTimeoutOnce(to, function()
            for j = ci, ci + 2, 1 do
                if j <= #rocks then
                    local exp = factory:createExplosion1(const.zOrderExplosion);
                    if j ~= ci then
                        exp:findExplosionComponent():setSound(nil);
                    end
                    exp.pos = rocks[j].pos;
                    scene:addObject(exp);
                    rocks[j]:removeFromParent();
                end
            end
        end);
        to = to + 0.2;
        i = i + 3;
    end
end);

dynomitePh.listener = createSensorEnterListener(false, function(other)
    if scene.player:findPlayerComponent().inventory:have(const.InventoryItemDynomite) then
        local obj = dynomitePh.parent;
        obj.freezable = false;

        scene.player:findPlayerComponent().inventory:take(const.InventoryItemDynomite);
        dynomitePh:removeFromParent();

        local tb = factory:createTimebomb(const.InventoryItemDynomite, 1.8, 87, -115);
        tb:setBlast(200.0, 200.0, { const.SceneObjectTypePlayer });
        local goalPos = scene:getObjects("dynomite_ph")[1].pos;
        tb.timeoutFn = function ()
            scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(goalPos);
            local booms = scene:getObjects("dynomite_boom");
            local to = 0.4;
            for _, boom in pairs(booms) do
                addTimeoutOnce(to, function()
                    local exp = factory:createExplosion1(const.zOrderExplosion);
                    exp.pos = boom.pos;
                    scene:addObject(exp);
                end);
                to = to + 0.4;
            end
            audio:playSound("squid_alert1.ogg");
            unweldTentacleBones(tentacle12weld);
        end
        obj:addComponent(tb);
    end
end);
