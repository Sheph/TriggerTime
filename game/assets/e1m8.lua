local music = nil;
local ambientMusic = "";
local isAmbient = false;
local subwayCars = {};
local airlocks = {};
local doors = {};
local timers = {};
local levers = {};
local timersDone = false;

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

function makePlatform(platform, parts, enterFn, leaveFn, ...)
    platform.deadbodyAware = true;

    local function enterFunc(other, args)
        if platform.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            platform.objs[other.cookie] = { count = 1, obj = other };
            if table.size(platform.objs) == 1 then
                platform.lastPos = platform.pos;
                platform.lastAngle = platform.angle;
                platform.timer = addTimeout0(function(cookie, dt)
                    local pa = platform.angle;
                    local pp = platform.pos;
                    local a = pa - platform.lastAngle;
                    for _, v in pairs(platform.objs) do
                        v.obj:setPosSmoothed(pp + (v.obj.pos - platform.lastPos):rotated(a));
                        v.obj:setAngleSmoothed(v.obj.angle + a);
                    end
                    platform.lastPos = pp;
                    platform.lastAngle = pa;
                end);
            end
            if enterFn ~= nil then
                enterFn(other, unpack2(args));
            end
        else
            platform.objs[other.cookie].count = platform.objs[other.cookie].count + 1;
        end
    end;

    local c = CollisionSensorComponent();
    platform:addComponent(c);
    platform.objs = {};
    platform.timer = nil;

    if parts ~= nil then
        for _, p in pairs(parts) do
            enterFunc(p);
        end
    end

    c.listener = createSensorListener(enterFunc, function(other, args)
        platform.objs[other.cookie].count = platform.objs[other.cookie].count - 1;
        if platform.objs[other.cookie].count == 0 then
            platform.objs[other.cookie] = nil;
            other.activeDeadbody = false;
            if table.size(platform.objs) == 0 then
                cancelTimeout(platform.timer);
                platform.timer = nil;
            end
            if leaveFn ~= nil then
                leaveFn(other, unpack2(args));
            end
        end
    end, pack2(...));
end

function disablePlatform(platform)
    platform:findCollisionSensorComponent():removeFromParent();
    if platform.timer ~= nil then
        cancelTimeout(platform.timer);
    end
    platform.objs = nil;
end

function makeSubwayCar(inst, pathName, enterFn, leaveFn, ...)
    inst.myCar = findObject(inst.objects, "subway_car");
    inst.myMover = findObject(inst.objects, "subway_car_mover");
    local p = scene:getObjects(pathName)[1];
    inst.myMover.roamBehavior:reset();
    inst.myMover.roamBehavior.linearVelocity = 60.0;
    inst.myMover.roamBehavior.linearDamping = 3.0;
    inst.myMover.roamBehavior.loop = true;
    inst.myMover.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    subwayCars[inst.myCar.cookie] = inst;
    makePlatform(inst.myCar, nil, enterFn, leaveFn, ...);
end

function startSubwayCar(inst)
    inst.myMover.roamBehavior:start();
end

function stopSubwayCar(inst)
    inst.myMover.roamBehavior:reset();
    inst.myMover.linearVelocity = vec2(0, 0);
    inst.myMover.angularVelocity = 0;
    inst.myCar.sleepingAllowed = true;
end

function makeSubwayStop(sensorName, timeout, enterFn, leaveFn, ...)
    setSensorListener(sensorName, function(other, objs, args)
        if objs[other.cookie] == nil then
            objs[other.cookie] = { count = 1, obj = other };
            if other.name == "subway_car" then
                local inst = subwayCars[other.cookie];
                local mover = inst.myMover;
                mover.roamBehavior:damp();
                if enterFn ~= nil then
                    enterFn(inst, unpack2(args));
                end
                addTimeoutOnce(timeout, function()
                    if leaveFn ~= nil then
                        leaveFn(inst, unpack2(args));
                    end
                    mover.roamBehavior.dampDistance = 0;
                    mover.roamBehavior.linearDamping = 1.5;
                    addTimeoutOnce(3.0, function()
                        mover.roamBehavior.linearDamping = 3.0;
                    end);
                end);
            end
        else
            objs[other.cookie].count = objs[other.cookie].count + 1;
        end
    end, function (other, objs, args)
        objs[other.cookie].count = objs[other.cookie].count - 1;
        if objs[other.cookie].count == 0 then
            objs[other.cookie] = nil;
        end
    end, {}, pack2(...));
end

function openSubwayCarDoor(inst)
    if inst.door ~= nil then
        cancelTimeout(inst.door.cookie);
        inst.door = nil;
    end
    local j = findJoint(inst.joints, "subway_car_door_joint");
    if j:getJointTranslation() >= j.upperLimit then
        j.motorSpeed = math.abs(j.motorSpeed);
        return;
    end
    inst.door = {};
    j.motorSpeed = math.abs(j.motorSpeed);
    inst.door.cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            cancelTimeout(cookie);
            inst.door = nil;
        end
    end);
end

function closeSubwayCarDoor(inst)
    if inst.door ~= nil then
        cancelTimeout(inst.door.cookie);
        inst.door = nil;
    end
    local j = findJoint(inst.joints, "subway_car_door_joint");
    if j:getJointTranslation() <= j.lowerLimit then
        j.motorSpeed = -math.abs(j.motorSpeed);
        return;
    end
    inst.door = {};
    j.motorSpeed = -math.abs(j.motorSpeed);
    inst.door.cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() <= j.lowerLimit) then
            cancelTimeout(cookie);
            inst.door = nil;
        end
    end);
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

function openDoor(name, haveSound)
    local snd = nil;
    if doors[name] ~= nil then
        snd = doors[name].snd;
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_left_joint");
    if j:getJointTranslation() >= j.upperLimit then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    if (snd == nil) and haveSound then
        doors[name].snd = audio:createSound("servo_move.ogg");
        doors[name].snd.loop = true;
        doors[name].snd:play();
    else
        doors[name].snd = snd;
    end
    j.motorSpeed = math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            cancelTimeout(cookie);
            if haveSound then
                doors[name].snd:stop();
            end
            doors[name] = nil;
        end
    end);
end

function closeDoor(name, haveSound)
    local snd = nil;
    if doors[name] ~= nil then
        snd = doors[name].snd;
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_left_joint");
    if j:getJointTranslation() <= j.lowerLimit then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = -math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    if (snd == nil) and haveSound then
        doors[name].snd = audio:createSound("servo_move.ogg");
        doors[name].snd.loop = true;
        doors[name].snd:play();
    else
        doors[name].snd = snd;
    end
    j.motorSpeed = -math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() <= j.lowerLimit) then
            cancelTimeout(cookie);
            if haveSound then
                doors[name].snd:stop();
            end
            doors[name] = nil;
        end
    end);
end

function makeDoor(name, opened)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "door1_left"),
        findObject(inst.objects, "door1_right"),
        findJoint(inst.joints, "door1_left_joint"),
        findJoint(inst.joints, "door1_right_joint"),
        -1, false);
    if opened then
        openDoor(name, false);
    end
end

function makeDoorTrigger(sensorName, name)
    setSensorListener(sensorName, function(other, self)
        if self.num == 0 then
            openDoor(name, true);
        end
        self.num = self.num + 1;
    end, function (other, self)
        self.num = self.num - 1;
        if self.num == 0 then
            closeDoor(name, true);
        end
    end, { num = 0 });
end

function makeStairs(name, downFn, upFn, ...)
    local stairsDown = scene:getObjects(name.."_down")[1];
    local stairsUp = scene:getObjects(name.."_up")[1];
    local curObj = nil;
    local args = pack2(...);
    setSensorEnterListener(stairsDown.name, false, function(other)
        if curObj ~= nil then
            return;
        end
        curObj = other;
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
            if curObj:alive() then
                curObj:findPlayerComponent().weapon:cancel();
                curObj:findPlayerComponent().altWeapon:cancel();
                curObj.pos = stairsUp.pos;
                scene.respawnPoint = stairsUp:getTransform();
                if upFn ~= nil then
                    upFn(unpack2(args));
                end
            end
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                if curObj:alive() then
                    curObj.active = false;
                    curObj.active = true;
                end
                curObj = nil;
            end);
        end);
    end);
    setSensorEnterListener(stairsUp.name, false, function(other)
        if curObj ~= nil then
            return;
        end
        curObj = other;
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
            if curObj:alive() then
                curObj:findPlayerComponent().weapon:cancel();
                curObj:findPlayerComponent().altWeapon:cancel();
                curObj.pos = stairsDown.pos;
                scene.respawnPoint = stairsDown:getTransform();
                if downFn ~= nil then
                    downFn(unpack2(args));
                end
            end
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                if curObj:alive() then
                    curObj.active = false;
                    curObj.active = true;
                end
                curObj = nil;
            end);
        end);
    end);
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

function makePod(inst, pathName, angleOffset, strong, enterFn, leaveFn, ...)
    inst.myPod = findObject(inst.objects, "pod");
    inst.myMover = findObject(inst.objects, "pod_mover");
    if strong then
        findJoint(inst.joints, "pod_joint").maxForce = 11000.0;
    end
    local p = scene:getObjects(pathName)[1];
    inst.myMover.roamBehavior:reset();
    inst.myMover.roamBehavior.linearVelocity = 10.0;
    inst.myMover.roamBehavior.linearDamping = 3.0;
    inst.myMover.roamBehavior.angleOffset = math.rad(angleOffset);
    inst.myMover.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    makePlatform(inst.myPod, nil, enterFn, leaveFn, ...);
end

function startPod(inst)
    inst.myMover.roamBehavior:start();
end

function disablePod(inst)
    disablePlatform(inst.myPod);
end

function makePodStop(sensorName, timeout, pods, enterFn, leaveFn, ...)
    setSensorListener(sensorName, function(other, objs, args)
        if objs[other.cookie] == nil then
            objs[other.cookie] = { count = 1, obj = other };
            if other.name == "pod" then
                for _, inst in pairs(pods) do
                    inst.myMover.roamBehavior:damp();
                end
                if enterFn ~= nil then
                    enterFn(unpack2(args));
                end
                addTimeoutOnce(timeout, function()
                    if leaveFn ~= nil then
                        leaveFn(unpack2(args));
                    end
                    for _, inst in pairs(pods) do
                        inst.myMover.roamBehavior.dampDistance = 0;
                    end
                end);
            end
        else
            objs[other.cookie].count = objs[other.cookie].count + 1;
        end
    end, function (other, objs, args)
        objs[other.cookie].count = objs[other.cookie].count - 1;
        if objs[other.cookie].count == 0 then
            objs[other.cookie] = nil;
        end
    end, {}, pack2(...));
end

function makePodUsable(inst)
    inst.myHatch = findObject(inst.objects, "pod_hatch");
    inst.myHatchJoint = findJoint(inst.joints, "pod_hatch_joint");
    inst.myHatch.gravityGunAware = true;
    local lights = inst.myHatch:findLightComponent():getLights();
    for _, light in pairs(lights) do
        light.visible = true;
    end
    inst.myBlocker = findObject(inst.objects, "pod_blocker");
    inst.myBlockerJoint = findJoint(inst.joints, "pod_blocker_joint");
    inst.myBlocker.active = true;
end

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

function makeTimer(name, amount)
    timers[name] = {};
    timers[name].amount = amount;
    timers[name].cur = amount;
    timers[name].d0 = scene:getObjects("terrain0")[1]:findRenderQuadComponents(name.."_0")[1].drawable;
    timers[name].d1 = scene:getObjects("terrain0")[1]:findRenderQuadComponents(name.."_1")[1].drawable;
    timers[name].d0:setImage("common1/font_"..math.floor(amount / 10)..".png");
    timers[name].d1:setImage("common1/font_"..math.floor(amount % 10)..".png");
    timers[name].resetFlag = false;
end

function resetTimer(name)
    timers[name].cur = timers[name].amount;
    timers[name].resetFlag = true;
end

function setTimer(name, amount)
    timers[name].cur = amount;
    timers[name].d0:setImage("common1/font_"..math.floor(amount / 10)..".png");
    timers[name].d1:setImage("common1/font_"..math.floor(amount % 10)..".png");
end

function stopTimers()
    timersDone = true;
end

local function leverCheck(cookie, j)
    if j.jointAngle < 0 then
        j.motorSpeed = -math.abs(j.motorSpeed);
        if (not j.myLowered) and (j.jointAngle <= j.lowerLimit) then
            if j.myLowerFn ~= nil then
                j.myLowerFn(unpack2(j.myArgs));
            end
            j.myLowered = true;
        end
    elseif j.jointAngle >= 0 then
        j.motorSpeed = math.abs(j.motorSpeed);
        if j.myLowered and (j.jointAngle >= j.upperLimit) then
            if j.myUpperFn ~= nil then
               j.myUpperFn(unpack2(j.myArgs));
            end
            j.myLowered = false;
        end
    end
end;

function makeLever(name, lowered, lowerFn, upperFn, ...)
    local inst = scene:getInstances(name)[1];
    local obj = findObject(inst.objects, "lever");
    local j = findJoint(inst.joints, "lever_joint");
    if lowered then
        j.motorSpeed = -math.abs(j.motorSpeed);
        obj.angle = obj.angle + j.lowerLimit;
    else
        j.motorSpeed = math.abs(j.motorSpeed);
        obj.angle = obj.angle + j.upperLimit;
    end
    levers[name] = j;
    j.motorEnabled = true;
    j.myLowered = lowered;
    j.myLimit = math.abs(j.lowerLimit);
    j.myLowerFn = lowerFn;
    j.myUpperFn = upperFn;
    j.myArgs = pack2(...);
    j.myTimer = addTimeout(0.3, leverCheck, j);
end

function enableLever(name)
    local j = levers[name];
    if (j == nil) or (j.myTimer ~= nil) then
        return;
    end
    j:setLimits(-j.myLimit, j.myLimit);
    j.motorEnabled = true;
    j.myTimer = addTimeout(0.3, leverCheck, j);
end

function disableLever(name)
    local j = levers[name];
    if (j == nil) or (j.myTimer == nil) then
        return;
    end
    j:setLimits(j.jointAngle, j.jointAngle);
    j.motorEnabled = false;
    cancelTimeout(j.myTimer);
    j.myTimer = nil;
end

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

function spawnBeetleProb(name, normal, shootOnly, splitOnly, shootAndSplit, offset, percent)
    local spawn = scene:getObjects(name);
    local probs = {normal, shootOnly, splitOnly, shootAndSplit};
    local args = {{false, false}, {true, false}, {false, true}, {true, true}};
    local cargs = args[1];
    local res = {};
    local i = 0;
    local maxNum = #spawn;
    if percent ~= nil then
        maxNum = maxNum * percent;
    end
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
        local enemy = factory:createBeetle1(unpack(cargs));
        enemy:findBeetleComponent().spawnPowerup = false;
        enemy.freezable = obj.freezable;
        enemy.freezeRadius = obj.freezeRadius;
        enemy.pos = obj.pos + offset;
        enemy.angle = obj.angle;
        scene:addObject(enemy);
        table.insert(res, enemy);
        i = i + 1;
        if i >= maxNum then
            break;
        end
    end
    return res;
end

-- main

addTimeout(1.0, function(cookie)
    if timersDone then
        cancelTimeout(cookie);
        return;
    end
    for _, timer in pairs(timers) do
        if timer.resetFlag then
            timer.resetFlag = false;
        else
            timer.cur = timer.cur - 1;
            if timer.cur < 0 then
                timer.cur = 0;
            end
        end
        timer.d0:setImage("common1/font_"..math.floor(timer.cur / 10)..".png");
        timer.d1:setImage("common1/font_"..math.floor(timer.cur % 10)..".png");
    end
end);

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};

math.randomseed(os.time());

scene:addObject(factory:createBackground("metal5.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground));

setAmbientMusic("ambient11.ogg");
startAmbientMusic(false);

require("e1m8_part0");
require("e1m8_part1");
require("e1m8_part2");
require("e1m8_part3");
require("e1m8_part4");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperBlaster);
    openAirlock("door32", false);
end

if (settings.developer >= 1) and (settings.developer <= 3) then
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
end

if (settings.developer >= 1) then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 80);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeProxMine);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeProxMine, 3);
end

if (settings.developer >= 2) and (settings.developer <= 11) then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 35);
end

if (settings.developer == 3) then
    scene:getObjects("station3_e1_cp")[1].active = false;
    scene:getObjects("station3_e2_cp")[1].active = false;
    scene:getObjects("station3_e3_cp")[1].active = false;
    postStation3();
end

if (settings.developer >= 4) and (settings.developer <= 5) then
    scene:getObjects("lantern1")[1].pos = scene.player.pos;
    stairs45Up();
    openAirlock("door7", false);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeMachineGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 350);
end

if (settings.developer >= 6) and (settings.developer <= 7) then
    stairs3Up();
    openAirlock("door17", false);
end

if (settings.developer == 7) then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeLGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeLGun, 50);
end

if (settings.developer == 8) then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemBlueKey);
end

if (settings.developer == 9) then
    scene:getObjects("elevator1_cp1")[1].active = false;
    addTimeoutOnce(1.5, function()
        enableLever("lever1");
    end);
end

if settings.developer >= 10 then
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
    addTimeoutOnce(1.0, function()
        stopTrains();
    end);
    startFinalPods();
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeLGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeLGun, 100);
    startMusic("ambient12.ogg", false);
end

if (settings.developer >= 10) and (settings.developer <= 11) then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeMachineGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 350);
end

if settings.developer == 11 then
    scene:getObjects("ride1_cp")[1].active = false;
    scene:getObjects("ride2_cp")[1].active = false;
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeEShield);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeEShield, 12);
end

if settings.developer == 12 then
    scene:getObjects("ride5_cp")[1].active = false;
    scene:getObjects("ride6_cp")[1].active = false;
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemYellowKey);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeEShield);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeEShield, 6);
end
