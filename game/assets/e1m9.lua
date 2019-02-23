local music = nil;
local ambientMusic = "";
local isAmbient = false;
local airlocks = {};
local plugs = {};
local jars = {};
local countdown = nil;
local numJars = 0;
local totalJars = 0;
local bossStatus = { false, false, false };
local jarsDone = false;

function startMusic(name, crossfade)
    local newMusic = audio:createStream(name);
    newMusic.loop = true;
    newMusic.isMusic = true;
    if crossfade then
        audio:crossfade(music, newMusic, 0.5, 0.5, 0.3);
    else
        if music ~= nil then
            music:stop();
        end
        newMusic:play();
    end
    music = newMusic;
    isAmbient = false;
end

function stopMusic(crossfade)
    if music ~= nil then
        if crossfade then
            audio:crossfade(music, nil, 0.5, 0.5, 0.3);
        else
            music:stop();
        end
    end
    music = nil;
end

function setAmbientMusic(name)
    if ambientMusic ~= name then
        ambientMusic = name;
        isAmbient = false;
    end
end

function startAmbientMusic(crossfade)
    if not isAmbient then
        startMusic(ambientMusic, crossfade);
        isAmbient = true;
    end
end

local function jarsAndBossesDone()
    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("boss1_cp")[1].pos);
    scene:getObjects("boss1_cp")[1].active = true;
end

function updateCountdown()
    if countdown == nil then
        countdown = showCountdown(10.5, 1.0);
        countdown:setTextColor({1.0, 0.0, 0.0, 0.6});
    end
    countdown:setText(tr.str54..": "..(totalJars - numJars).."/"..totalJars);
    if numJars == 0 then
        addTimeoutOnce(2.0, function()
            local timer = addTimeout(0.05, function(cookie, self)
                if self.red then
                    countdown:setTextColor({1.0, 0.0, 0.0, 0.6});
                else
                    countdown:setTextColor({1.0, 1.0, 1.0, 0.6});
                end
                self.red = not self.red;
            end, { red = false });
            addTimeoutOnce(3.0, function()
                cancelTimeout(timer);
                countdown.parent:removeFromParent();
                countdown = nil;
                jarsDone = true;
                for i = 1,3,1 do
                    if not bossStatus[i] then
                        return;
                    end
                end
                jarsAndBossesDone();
            end);
        end);
    end
end

function bossDone(i)
    bossStatus[i] = true;
    for i = 1,3,1 do
        if not bossStatus[i] then
            return;
        end
    end
    if jarsDone then
        jarsAndBossesDone();
    end
end

function openAirlock(name, haveSound)
    local snd = nil;
    local wheel = true;
    if airlocks[name] ~= nil then
        snd = airlocks[name].snd;
        wheel = airlocks[name].wheel;
        cancelTimeout(airlocks[name].cookie);
        airlocks[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "airlock_door1_joint");
    local wj = findJoint(inst.joints, "airlock_wheel_joint");
    if ((j:getJointTranslation() >= j.upperLimit) and (wj.jointAngle >= wj.upperLimit)) then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = math.abs(j.motorSpeed);
        wj.motorSpeed = math.abs(wj.motorSpeed);
        return;
    end
    airlocks[name] = {};
    if (snd == nil) and haveSound then
        airlocks[name].snd = audio:createSound("door_move.ogg");
        airlocks[name].snd.loop = true;
        airlocks[name].snd:play();
    else
        airlocks[name].snd = snd;
    end
    airlocks[name].wheel = wheel;

    wj.motorSpeed = math.abs(wj.motorSpeed);

    if (not wheel) then
        j.motorSpeed = math.abs(j.motorSpeed);
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (j:getJointTranslation() >= j.upperLimit) then
                cancelTimeout(cookie);
                if haveSound then
                    airlocks[name].snd:stop();
                end
                airlocks[name] = nil;
            end
        end);
    else
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (wj.jointAngle >= wj.upperLimit) then
                if haveSound then
                    airlocks[name].snd:stop();
                    airlocks[name].snd = audio:createSound("servo_move.ogg");
                    airlocks[name].snd.loop = true;
                    airlocks[name].snd:play();
                end
                airlocks[name].wheel = false;
                cancelTimeout(cookie);
                j.motorSpeed = math.abs(j.motorSpeed);
                airlocks[name].cookie = addTimeout0(function(cookie, dt)
                    if (j:getJointTranslation() >= j.upperLimit) then
                        cancelTimeout(cookie);
                        if haveSound then
                            airlocks[name].snd:stop();
                        end
                        airlocks[name] = nil;
                    end
                end);
            end
        end);
    end
end

function closeAirlock(name, haveSound)
    local snd = nil;
    local wheel = false;
    if airlocks[name] ~= nil then
        snd = airlocks[name].snd;
        wheel = airlocks[name].wheel;
        cancelTimeout(airlocks[name].cookie);
        airlocks[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "airlock_door1_joint");
    local wj = findJoint(inst.joints, "airlock_wheel_joint");
    if ((j:getJointTranslation() <= j.lowerLimit) and (wj.jointAngle <= wj.lowerLimit)) then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = -math.abs(j.motorSpeed);
        wj.motorSpeed = -math.abs(wj.motorSpeed);
        return;
    end
    airlocks[name] = {};
    if (snd == nil) and haveSound then
        airlocks[name].snd = audio:createSound("servo_move.ogg");
        airlocks[name].snd.loop = true;
        airlocks[name].snd:play();
    else
        airlocks[name].snd = snd;
    end
    airlocks[name].wheel = wheel;

    j.motorSpeed = -math.abs(j.motorSpeed);

    if (wheel) then
        wj.motorSpeed = -math.abs(wj.motorSpeed);
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (wj.jointAngle <= wj.lowerLimit) then
                cancelTimeout(cookie);
                if haveSound then
                    airlocks[name].snd:stop();
                end
                airlocks[name] = nil;
            end
        end);
    else
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (j:getJointTranslation() <= j.lowerLimit) then
                if haveSound then
                    airlocks[name].snd:stop();
                    airlocks[name].snd = audio:createSound("door_move.ogg");
                    airlocks[name].snd.loop = true;
                    airlocks[name].snd:play();
                end
                airlocks[name].wheel = true;
                cancelTimeout(cookie);
                wj.motorSpeed = -math.abs(wj.motorSpeed);
                airlocks[name].cookie = addTimeout0(function(cookie, dt)
                    if (wj.jointAngle <= wj.lowerLimit) then
                        cancelTimeout(cookie);
                        if haveSound then
                            airlocks[name].snd:stop();
                        end
                        airlocks[name] = nil;
                    end
                end);
            end
        end);
    end
end

function makeAirlock(name, opened)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
    if opened then
        openAirlock(name, false);
    end
end

function makeAirlockTrigger(sensorName, name)
    setSensorListener(sensorName, function(other, self)
        if self.num == 0 then
            openAirlock(name, true);
        end
        self.num = self.num + 1;
    end, function (other, self)
        self.num = self.num - 1;
        if self.num == 0 then
            closeAirlock(name, true);
        end
    end, { num = 0 });
end

function setupDisplay(name)
    local rc = scene:getObjects("terrain0")[1]:findRenderProjComponents(name)[1];
    local display = AnimationComponent(rc.drawable);
    display:addAnimation(const.AnimationDefault, "tv_def", 1);
    display:addAnimation(const.AnimationDefault + 1, "tv_natan", 1);
    display:startAnimation(const.AnimationDefault);
    scene:getObjects("terrain0")[1]:addComponent(display);

    return display;
end

function makePlugs(objs, plugFn, unplugFn, ...)
    for i = 1, 2, 1 do
        if objs[i] == nil then
            break;
        end
        objs[i].mySocket = nil;
        objs[i].myPlugFn = plugFn;
        objs[i].myUnplugFn = unplugFn;
        objs[i].myArgs = pack2(...);
        plugs[objs[i].cookie] = objs[i];
    end
    if objs[2] == nil then
        return;
    end
    objs[1].myOther = objs[2];
    objs[2].myOther = objs[1];
end

function makeSocket(socket)
    local c = CollisionSensorComponent();
    socket.myPlug = nil;
    socket:addComponent(c);
    c.listener = createSensorEnterListener(false, function(other)
        if socket.myDisabled then
            return;
        end
        if socket.myPlug ~= nil then
            return;
        end
        if plugs[other.cookie] == nil then
            return;
        end
        other = plugs[other.cookie];
        local aw = scene.player:findPlayerComponent().altWeapon;
        if aw.heldObject == other then
            aw:cancel();
        end
        other.mySocket = socket;
        other:setTransform(socket:getTransform());
        other:findRenderQuadComponents("nopins")[1].visible = true;
        other:findRenderQuadComponents("pins")[1].visible = false;
        socket.myJ = scene:addWeldJoint(other, socket, socket:getWorldPoint(vec2(0, 0)), true);
        socket.myPlug = other;
        if other.myPlugFn ~= nil then
            other.myPlugFn(other, socket, unpack2(other.myArgs));
        end
    end);
end

function disableSocket(socket)
    socket:findLightComponent():getLights("light0")[1].visible = false;
    if socket.myPlug ~= nil then
        socket.myPlug.gravityGunAware = false;
        socket.myPlug:findLightComponent():getLights("light0")[1].visible = false;
    end
    socket.myDisabled = true;
end

function enableSocket(socket)
    socket:findLightComponent():getLights("light0")[1].visible = true;
    if socket.myPlug ~= nil then
        socket.myPlug.gravityGunAware = true;
        socket.myPlug:findLightComponent():getLights("light0")[1].visible = true;
    end
    socket.myDisabled = false;
end

addTimeout(0.1, function()
    local obj = scene.player:findPlayerComponent().altWeapon.heldObject;
    if (obj == nil) or (plugs[obj.cookie] == nil) then
        return;
    end
    obj = plugs[obj.cookie];
    local socket = obj.mySocket;
    if socket == nil then
        return;
    end
    socket.myJ:remove();
    socket.myJ = nil;
    socket.myPlug = nil;
    obj.mySocket = nil;
    obj:findRenderQuadComponents("nopins")[1].visible = false;
    obj:findRenderQuadComponents("pins")[1].visible = true;
    if obj.myUnplugFn ~= nil then
        obj.myUnplugFn(obj, socket, unpack2(obj.myArgs));
    end
end);

function makeKeyDoor(doorName, phName, fn, ...)
    local doorObj = scene:getObjects(doorName)[1];
    local phObj = scene:getObjects(phName)[1];
    local phC = phObj:findPlaceholderComponent();

    phC.listener = createSensorEnterListener(false, function(other, fn, args)
        if not scene.player:findPlayerComponent().inventory:have(phC.item) then
            return;
        end
        scene.player:findPlayerComponent().inventory:take(phC.item);
        phObj:removeFromParent();
        doorObj:findRenderQuadComponents("key")[1].visible = true;
        audio:playSound("key.ogg");
        fn(unpack2(args));
    end, fn, pack2(...));
end

function fixedSpawn(name)
    local spawn = scene:getObjects(name.."_warder");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createWarder(), obj);
    end
    local spawn = scene:getObjects(name.."_enforcer1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createEnforcer1(), obj);
    end
    local spawn = scene:getObjects(name.."_enforcer2");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createEnforcer2(), obj);
    end
    local spawn = scene:getObjects(name.."_orbo");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createOrbo(), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createBeetle1(false, false), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1_shoot");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createBeetle1(true, false), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1_split");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createBeetle1(false, true), obj);
    end
    local spawn = scene:getObjects(name.."_scorp2");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createScorp2(), obj);
    end
    local spawn = scene:getObjects(name.."_centipede1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createCentipede1(), obj);
    end
    local spawn = scene:getObjects(name.."_homer");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createHomer(), obj);
    end
    local spawn = scene:getObjects(name.."_gorger1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemyFreezable(factory:createGorger1(), obj);
    end
    local spawn = scene:getObjects(name.."_s_warder");
    local summonSound = true;
    for i = 1, #spawn, 1 do
        summon1(spawn[i], summonSound, function()
            addSpawnedEnemyFreezable(factory:createWarder(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_gorger1");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], summonSound, function()
            addSpawnedEnemyFreezable(factory:createGorger1(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_orbo");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], summonSound, function()
            addSpawnedEnemyFreezable(factory:createOrbo(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_homer");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], summonSound, function()
            addSpawnedEnemyFreezable(factory:createHomer(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_keeper");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], summonSound, function()
            addSpawnedEnemyFreezable(factory:createKeeper(false), spawn[i]);
        end);
        summonSound = false;
    end
end

function openHatch(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = math.abs(j.motorSpeed);
end

function closeHatch(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = -math.abs(j.motorSpeed);
end

function makeJar1(jar, floats)
    jar.myRoot = findObject(jar.objects, "jar_root");
    jar.myPart = findObject(jar.objects, "jar_part");

    local rc = jar.myPart:findRenderTerrainComponents()[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic2", 1);
    ac:startAnimation(const.AnimationDefault);
    jar.myPart:addComponent(ac);

    for _, obj in pairs(jar.objects) do
        obj.type = const.SceneObjectTypeEnemyBuilding;
    end

    jar.myRoot.maxLife = settings.jar1.life;
    jar.myRoot.life = settings.jar1.life;

    local hb = RenderHealthbarComponent(vec2(0.0, 7.0), 0, 10.0, 1.5, const.zOrderMarker + 1);
    hb.color = { 1.0, 1.0, 1.0, 0.6 };
    jar.myRoot:addComponent(hb);

    if floats ~= nil then
        for _, f in pairs(floats) do
            local name = f[1];
            local obj = scene;
            local i = string.find(f[1], ".", 1, true);
            if i ~= nil then
                obj = scene:getObjects(string.sub(f[1], 1, i - 1))[1];
                name = string.sub(f[1], i + 1);
            end
            obj = obj:getObjects(name)[1];
            local c = FloatComponent(1.0, 0.5, f[2]);
            c.target = obj;
            jar.myPart:addComponent(c);
        end
    end

    jars[jar.myRoot.cookie] = jar;

    jar.myDoneCb = function()
        addTimeoutOnce(0.5, function()
            jar.myPart:addComponent(FadeOutComponent(1.5));
        end);
    end;
end

function makeJar1WithCreature(jar, name)
    numJars = numJars + 1;
    jar.myCreature = scene:getObjects(name)[1];
    jar.myCreature:findPhysicsBodyComponent():disableAllFixtures();
    local objs = jar.myCreature:getObjects();
    for _, obj in pairs(objs) do
        obj:findPhysicsBodyComponent():disableAllFixtures();
    end
    local i = scene:getObjects(name)[1]:findCreatureComponent().I;
    if i == 1 then
        makeJar1(jar, {{name, 20.0}, {name..".tail", 20.0}});
    elseif i == 2 then
        makeJar1(jar, {{name, 30.0}});
    elseif i == 3 then
        makeJar1(jar, {{name, 50.0}, {name..".leg1", 9.0}, {name..".leg2", 9.0}, {name..".leg3", 9.0}, {name..".leg4", 9.0}});
    elseif i == 4 then
        makeJar1(jar, {{name, 100.0}});
    elseif i == 5 then
        makeJar1(jar, {{name, 30.0}, {name..".tail1", 15.0}, {name..".tail2", 3.0}});
    elseif i == 6 then
        makeJar1(jar, {{name, 30.0}, {name..".leg1", 3.0}, {name..".leg2", 3.0}, {name..".leg3", 3.0}, {name..".leg4", 3.0}, {name..".leg5", 3.0}, {name..".leg6", 3.0}});
    else
        assert(false);
    end
    jar.myCounterCb = function()
        numJars = numJars - 1;
        updateCountdown();
    end;
end

function makeJar1Empty(jar)
    makeJar1(jar);
    jars[jar.myRoot.cookie] = nil;
    jar.myRoot.freezable = false;
    local hbc = jar.myRoot:findRenderHealthbarComponent();
    if hbc ~= nil then
        hbc:removeFromParent();
    end
    jar.myDoneCb();
end

function makeJar2(jar, floats)
    numJars = numJars + 1;

    jar.myRoot = findObject(jar.objects, "jar_root");
    jar.myParts = { findObject(jar.objects, "jar_part1"), findObject(jar.objects, "jar_part2"), findObject(jar.objects, "jar_part3") };
    jar.myLiquid = findObject(jar.objects, "jar_liquid");

    local rc = jar.myLiquid:findRenderTerrainComponents()[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic2", 1);
    ac:startAnimation(const.AnimationDefault);
    jar.myLiquid:addComponent(ac);

    for _, obj in pairs(jar.objects) do
        obj.type = const.SceneObjectTypeEnemyBuilding;
    end

    jar.myRoot.maxLife = settings.jar2.life;
    jar.myRoot.life = settings.jar2.life;

    local hb = RenderHealthbarComponent(vec2(0.0, 13.5), 0, 15.0, 2.0, const.zOrderMarker + 1);
    hb.color = { 1.0, 1.0, 1.0, 0.6 };
    jar.myRoot:addComponent(hb);

    if floats ~= nil then
        for _, f in pairs(floats) do
            local name = f[1];
            local obj = scene;
            local i = string.find(f[1], ".", 1, true);
            if i ~= nil then
                obj = scene:getObjects(string.sub(f[1], 1, i - 1))[1];
                name = string.sub(f[1], i + 1);
            end
            obj = obj:getObjects(name)[1];
            local c = FloatComponent(1.0, 0.5, f[2]);
            c.target = obj;
            jar.myRoot:addComponent(c);
        end
    end

    jars[jar.myRoot.cookie] = jar;

    jar.myDoneCb = function()
        addTimeoutOnce(0.5, function()
            jar.myLiquid:addComponent(FadeOutComponent(1.5));
            addTimeoutOnce(1.0, function()
                local cs = jar.myRoot:findFloatComponents();
                for _, c in pairs(cs) do
                    c:stop();
                end
            end);
        end);
    end;

    jar.myCounterCb = function()
        numJars = numJars - 1;
        updateCountdown();
    end;
end

function makeJar3(jar, floats)
    jar.myRoot = findObject(jar.objects, "jar_root");
    jar.myPart = findObject(jar.objects, "jar_part");

    local rc = jar.myPart:findRenderTerrainComponents()[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic2", 1);
    ac:startAnimation(const.AnimationDefault);
    jar.myPart:addComponent(ac);

    for _, obj in pairs(jar.objects) do
        obj.type = const.SceneObjectTypeEnemyBuilding;
    end

    jar.myRoot.maxLife = settings.jar3.life;
    jar.myRoot.life = settings.jar3.life;

    local hb = RenderHealthbarComponent(vec2(0.0, 6.0), 0, 10.0, 1.5, const.zOrderMarker + 1);
    hb.color = { 1.0, 1.0, 1.0, 0.6 };
    jar.myRoot:addComponent(hb);

    if floats ~= nil then
        for _, f in pairs(floats) do
            local name = f[1];
            local obj = scene;
            local i = string.find(f[1], ".", 1, true);
            if i ~= nil then
                obj = scene:getObjects(string.sub(f[1], 1, i - 1))[1];
                name = string.sub(f[1], i + 1);
            end
            obj = obj:getObjects(name)[1];
            local c = FloatComponent(1.0, 0.5, f[2]);
            c.target = obj;
            jar.myPart:addComponent(c);
        end
    end

    jars[jar.myRoot.cookie] = jar;

    jar.myDoneCb = function()
        addTimeoutOnce(0.5, function()
            jar.myPart:addComponent(FadeOutComponent(1.5));
        end);
    end;
end

function makeJar3WithCreature(jar, name)
    numJars = numJars + 1;
    jar.myCreature = scene:getObjects(name)[1];
    jar.myCreature:findPhysicsBodyComponent():disableAllFixtures();
    local objs = jar.myCreature:getObjects();
    for _, obj in pairs(objs) do
        obj:findPhysicsBodyComponent():disableAllFixtures();
    end
    local i = scene:getObjects(name)[1]:findCreatureComponent().I;
    if i == 7 then
        makeJar3(jar, {{name, 30.0}, {name..".tail1", 15.0}, {name..".tail2", 3.0}});
    elseif i == 8 then
        makeJar3(jar, {{name, 20.0}, {name..".tail", 20.0}});
    elseif i == 9 then
        makeJar3(jar, {{name, 30.0}, {name..".tentacle1", 20.0}});
    else
        assert(false);
    end
    jar.myCounterCb = function()
        numJars = numJars - 1;
        updateCountdown();
    end;
end

addTimeout0(function(cookie, dt)
    for _, jar in pairs(jars) do
        if jar.myRoot:dead() then
            jars[jar.myRoot.cookie] = nil;
            jar.myRoot.freezable = false;
            local hbc = jar.myRoot:findRenderHealthbarComponent();
            if hbc ~= nil then
                hbc:removeFromParent();
            end
            if jar.myCreature ~= nil then
                jar.myCreature:findPhysicsBodyComponent():enableAllFixtures();
                local objs = jar.myCreature:getObjects();
                for _, obj in pairs(objs) do
                    obj:findPhysicsBodyComponent():enableAllFixtures();
                end
            end
            local booms = jar.myRoot:findDummyComponents("boom");
            local to = 0;
            for _, boom in pairs(booms) do
                addTimeoutOnce(to, function()
                    local exp = factory:createExplosion1(const.zOrderExplosion);
                    exp.pos = jar.myRoot:getWorldPoint(boom.pos);
                    scene:addObject(exp);
                end);
                to = to + 0.38;
            end
            jar.myDoneCb();
            if jar.myCounterCb ~= nil then
                jar.myCounterCb();
            end
            if jar.myDieCb ~= nil then
                jar.myDieCb();
            end
        end
    end
end);

function makeFlickerLamps(name)
    local function flickerTimeout2(self)
        if self.t <= 0 then
            for _, light in pairs(self.lights) do
                light.visible = self.defaultOn;
            end
            return;
        end
        for _, light in pairs(self.lights) do
            light.visible = not light.visible;
        end
        local t = 0.005 + math.random() * 0.005;
        self.t = self.t - t;
        addTimeoutOnce(t, flickerTimeout2, self);
    end

    local function flickerTimeout(self)
        self.t = 0.01 + math.random() * 0.15;
        flickerTimeout2(self);
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, self);
    end

    local insts = scene:getInstances(name);
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, { lights = lights, t = 0, defaultOn = lights[1].visible });
    end
end

-- main

local insts = scene:getInstances("flicker_lamp_off");
for _, inst in pairs(insts) do
    local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
    for _, light in pairs(lights) do
        light.visible = false;
    end
end

makeFlickerLamps("flicker_lamp");
makeFlickerLamps("flicker_lamp_off");

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};

math.randomseed(os.time());

scene:addObject(factory:createBackground("metal5.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground));

setAmbientMusic("action7.ogg");
startAmbientMusic(false);

require("e1m9_part0");
require("e1m9_part1");
require("e1m9_part2");
require("e1m9_part3");
require("e1m9_part4");

totalJars = numJars;

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperBlaster);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeMachineGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 300);
    scene:getObjects("intro3_cp")[1].active = false;
    scene:getObjects("intro5_cp")[1].active = false;
end

if settings.developer >= 2 then
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    local lights = scene:getObjects("terrain0")[1]:findLightComponent():getLights("go_in_light1");
    for _, light in pairs(lights) do
        light.visible = true;
    end
end

if settings.developer == 2 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemBlueKey);
end

if settings.developer >= 3 then
    enableHallwayDoors();
end

if settings.developer >= 4 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 5);
end

if settings.developer == 6 then
    closeAirlock("door21", false);
    closeAirlock("door22", false);
    openAirlock("door23", false);
    scene:getObjects("gasroom4_cp")[1].active = false;
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypePlasmaGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypePlasmaGun, 250);
    postGasroom();
end

if settings.developer == 9 then
    bossStatus[1] = true;
    bossStatus[2] = true;
    bossStatus[3] = true;
    numJars = 1;
    updateCountdown();
end
