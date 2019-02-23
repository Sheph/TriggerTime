local music = nil;
local ambientMusic = "";
local isAmbient = false;
local doors = {};
local floor = scene:getObjects("floor")[1];
local planks = scene:getObjects("planks")[1];
local levers = {};
local airlocks = {};
local hooks = {};

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

function makeToxic1(sensor, damage)
    local c = CollisionSensorComponent();
    sensor:addComponent(c);
    sensor.objs = {};
    sensor.timer = nil;
    c.listener = createSensorListener(function(other, args)
        if sensor.objs[other.cookie] == nil then
            sensor.objs[other.cookie] = { count = 1, obj = other, t = 0.5, eff = false };
            if table.size(sensor.objs) == 1 then
                sensor.timer = addTimeout0(function(cookie, dt)
                    for cie, v in pairs(sensor.objs) do
                        if (planks.objs[cie] == nil) then
                            v.t = v.t + dt;
                            if (v.t >= 0.5) and (not v.eff) then
                                v.eff = true;
                                local s = factory:createToxicSplash1(-90);
                                s.pos = v.obj.pos;
                                scene:addObject(s);
                            end
                            if v.t >= 0.75 then
                                v.t = 0.0;
                                v.obj:changeLife(-damage);
                                v.eff = false;
                                audio:playSound("toxic1.ogg");
                            end
                        else
                            v.t = 0.5;
                            v.eff = false;
                        end
                    end
                end);
            end
        else
            sensor.objs[other.cookie].count = sensor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        sensor.objs[other.cookie].count = sensor.objs[other.cookie].count - 1;
        if sensor.objs[other.cookie].count == 0 then
            sensor.objs[other.cookie] = nil;
            if table.size(sensor.objs) == 0 then
                cancelTimeout(sensor.timer);
                sensor.timer = nil;
            end
        end
    end);
end

function makeToxic2(obj, rc)
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic2", 1);
    ac:startAnimation(const.AnimationDefault);
    obj:addComponent(ac);
end

function makeTentacleFlesh(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local c = obj:findRenderTentacleComponent();
        local objs2 = c.objects;
        for _, obj2 in pairs(objs2) do
            obj2.material = const.MaterialFlesh;
        end
    end
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
    local rj = findJoint(inst.joints, "door1_right_joint");
    if rj ~= nil then
        scene:addGearJoint(findObject(inst.objects, "door1_left"),
            findObject(inst.objects, "door1_right"),
            findJoint(inst.joints, "door1_left_joint"),
            findJoint(inst.joints, "door1_right_joint"),
            -1, false);
    end
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

function makeRipples1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local rcs = obj:findRenderQuadComponents("ripple1");
        for _, rc in pairs(rcs) do
            local ac = AnimationComponent(rc.drawable);
            ac:addAnimation(const.AnimationDefault, "ripple1", 1);
            ac:startAnimation(const.AnimationDefault);
            obj:addComponent(ac);
        end
    end
end

function addRipples1(name, height)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local c = factory:createQuad("common2/particle_basic.png", height, -150);
        c.name = "ripple1";
        c.fixedPos = true;
        c.color = {0.0, 1.0, 0.0, 1.0};
        obj:addComponent(c);
    end
    makeRipples1(name);
end

function makeToxicFlow(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local j = 1;
        while (true) do
            local rc = obj:findRenderStripeComponents("toxic"..j)[1];
            if rc == nil then
                break;
            end
            local ac = AnimationComponent(rc.drawable);
            ac:addAnimation(const.AnimationDefault, "toxic3", 1);
            ac:startAnimation(const.AnimationDefault);
            obj:addComponent(ac);
            j = j + 1;
        end
    end
end

function makePlatform(platform, parts)
    platform.deadbodyAware = true;

    local function enterFunc(other)
        if platform.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            platform.objs[other.cookie] = { count = 1, obj = other };
            if table.size(platform.objs) == 1 then
                platform.lastAngle = platform.angle;
                platform.timer = addTimeout0(function(cookie, dt)
                    for _, v in pairs(platform.objs) do
                        local a = platform.angle - platform.lastAngle;
                        v.obj:setPosSmoothed(platform.pos + (v.obj.pos - platform.pos):rotated(a));
                        v.obj:setAngleSmoothed(v.obj.angle + a);
                    end
                    platform.lastAngle = platform.angle;
                end);
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

    c.listener = createSensorListener(enterFunc, function (other)
        platform.objs[other.cookie].count = platform.objs[other.cookie].count - 1;
        if platform.objs[other.cookie].count == 0 then
            other.activeDeadbody = false;
            platform.objs[other.cookie] = nil;
            if table.size(platform.objs) == 0 then
                cancelTimeout(platform.timer);
                platform.timer = nil;
            end
        end
    end);
end

function makeConveyor2(name, velocity, fnEnt, fnExit, ...)
    local conveyor = scene:getObjects(name)[1];
    conveyor.deadbodyAware = true;

    local rc = conveyor:findRenderStripeComponents("belt")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "belt1", 16 / velocity);
    ac:startAnimation(const.AnimationDefault);
    conveyor:addComponent(ac);

    conveyor:addComponent(CollisionSensorComponent());
    conveyor.objs = {};
    setSensorListener(name, function(other, args)
        if conveyor.objs[other.cookie] == nil then
            local doAdd = true;
            if fnEnt ~= nil then
                doAdd = fnEnt(other, unpack2(args));
            end
            if doAdd then
                other.activeDeadbody = true;
                conveyor.objs[other.cookie] = { count = 1, obj = other };
            end
        else
            conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        if conveyor.objs[other.cookie] == nil then
            return;
        end
        conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count - 1;
        if conveyor.objs[other.cookie].count == 0 then
            local obj = conveyor.objs[other.cookie].obj;
            conveyor.objs[other.cookie] = nil;
            obj.activeDeadbody = false;
            if fnExit ~= nil then
                fnExit(obj, unpack2(args));
            end
        end
    end, pack2(...));
    addTimeout0(function(cookie, dt)
        local dir = conveyor:getDirection(velocity * dt);
        for _, v in pairs(conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
    end);
end

function makeConveyor(name, velocity, fn, ...)
    makeConveyor2(name, velocity, nil, fn, ...);
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

function hideLeverHandle(name)
    local inst = scene:getInstances(name)[1];
    local obj = findObject(inst.objects, "lever");
    obj.active = false;
    obj.visible = false;
end

function showLeverHandle(name)
    local inst = scene:getInstances(name)[1];
    local obj = findObject(inst.objects, "lever");
    obj.active = true;
    obj.visible = true;
end

function fixedSpawn(name)
    local spawn = scene:getObjects(name.."_warder");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createWarder(), obj);
    end
    local spawn = scene:getObjects(name.."_enforcer1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createEnforcer1(), obj);
    end
    local spawn = scene:getObjects(name.."_orbo");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createOrbo(), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createBeetle1(false, false), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1_shoot");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createBeetle1(true, false), obj);
    end
    local spawn = scene:getObjects(name.."_beetle1_split");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createBeetle1(false, true), obj);
    end
    local spawn = scene:getObjects(name.."_scorp2");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createScorp2(), obj);
    end
    local spawn = scene:getObjects(name.."_centipede1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createCentipede1(), obj);
    end
    local spawn = scene:getObjects(name.."_s_warder");
    local summonSound = true;
    for i = 1, #spawn, 1 do
        summon1(spawn[i], summonSound, function()
            addSpawnedEnemy(factory:createWarder(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_gorger1");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], summonSound, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
        summonSound = false;
    end
    local spawn = scene:getObjects(name.."_s_orbo");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], summonSound, function()
            addSpawnedEnemy(factory:createOrbo(), spawn[i]);
        end);
        summonSound = false;
    end
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

function makeBoatPark(ph, exitName, enterFn, exitFn, ...)
    ph.listener = createSensorListener(function(other, self)
        if scene.inputBoat.active then
            if other.type == const.SceneObjectTypeVehicle then
                self.choice = showChoice({{ pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(90), height = 6, image = "common2/arrow_button.png"}}, function(i)
                    ph.myBoat = other;
                    other.bodyType = const.BodyKinematic;
                    other.linearVelocity = vec2(0, 0);
                    other.angularVelocity = 0;
                    ph.myBoat:findBoatComponent().active = false;
                    scene.inputPlayer.active = true;
                    scene.inputBoat.active = false;
                    scene.player:findPlayerComponent().standing = false;
                    scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 1.5);
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                    scene.respawnPoint = scene:getObjects(exitName)[1]:getTransform();
                    scene.player.pos = scene.respawnPoint.p;
                    self.choice = nil;
                    ph.active = false;
                    addTimeoutOnce(1.0, function()
                        ph.active = true;
                    end);
                    if exitFn ~= nil then
                        exitFn(ph.myBoat, unpack2(self.args));
                    end
                end);
            end
        else
            if other.type == const.SceneObjectTypeVehicle then
                ph.myBoat = other;
                other.bodyType = const.BodyKinematic;
                other.linearVelocity = vec2(0, 0);
                other.angularVelocity = 0;
            else
                self.choice = showChoice({{ pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(-90), height = 6, image = "common2/arrow_button.png"}}, function(i)
                    ph.myBoat.bodyType = const.BodyDynamic;
                    ph.myBoat:findBoatComponent().active = true;
                    scene.inputPlayer.active = false;
                    scene.inputBoat.active = true;
                    scene.player:findPlayerComponent().standing = true;
                    scene.camera:findCameraComponent():zoomTo(55, const.EaseInOutQuad, 1.5);
                    scene.camera:findCameraComponent():follow(ph.myBoat, const.EaseOutQuad, 1.0);
                    self.choice = nil;
                    ph.active = false;
                    addTimeoutOnce(1.0, function()
                        ph.active = true;
                    end);
                    if enterFn ~= nil then
                        local boat = ph.myBoat;
                        ph.myBoat = nil;
                        enterFn(boat, unpack2(self.args));
                    end
                end);
            end
        end
    end, function(other, self)
        if scene.inputBoat.active and (other.type ~= const.SceneObjectTypeVehicle) then
            return;
        end
        if self.choice ~= nil then
            self.choice:removeFromParent();
            self.choice = nil;
        end
    end, { choice = nil, args = pack2(...) });
end

function openAirlock(name, haveSound, fast)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "airlock_door1_joint");
    local wj = findJoint(inst.joints, "airlock_wheel_joint");
    local snd = nil;
    local wheel = true;
    if airlocks[name] ~= nil then
        if wj.motorSpeed >= 0 then
            wj.motorSpeed = airlocks[name].wheelMotorSpeed;
        else
            wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
        end
        if j.motorSpeed >= 0 then
            j.motorSpeed = airlocks[name].doorMotorSpeed;
        else
            j.motorSpeed = -airlocks[name].doorMotorSpeed;
        end
        snd = airlocks[name].snd;
        wheel = airlocks[name].wheel;
        cancelTimeout(airlocks[name].cookie);
        airlocks[name] = nil;
    end
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

    airlocks[name].wheelMotorSpeed = math.abs(wj.motorSpeed);
    airlocks[name].doorMotorSpeed = math.abs(j.motorSpeed);

    if fast then
        wj.motorSpeed = wj.motorSpeed * 4.0;
        j.motorSpeed = j.motorSpeed * 4.0;
    end

    if (not wheel) then
        j.motorSpeed = math.abs(j.motorSpeed);
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (j:getJointTranslation() >= j.upperLimit) then
                cancelTimeout(cookie);
                if haveSound then
                    airlocks[name].snd:stop();
                end
                if wj.motorSpeed >= 0 then
                    wj.motorSpeed = airlocks[name].wheelMotorSpeed;
                else
                    wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
                end
                if j.motorSpeed >= 0 then
                    j.motorSpeed = airlocks[name].doorMotorSpeed;
                else
                    j.motorSpeed = -airlocks[name].doorMotorSpeed;
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
                        if wj.motorSpeed >= 0 then
                            wj.motorSpeed = airlocks[name].wheelMotorSpeed;
                        else
                            wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
                        end
                        if j.motorSpeed >= 0 then
                            j.motorSpeed = airlocks[name].doorMotorSpeed;
                        else
                            j.motorSpeed = -airlocks[name].doorMotorSpeed;
                        end
                        airlocks[name] = nil;
                    end
                end);
            end
        end);
    end
end

function closeAirlock(name, haveSound)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "airlock_door1_joint");
    local wj = findJoint(inst.joints, "airlock_wheel_joint");
    local snd = nil;
    local wheel = false;
    if airlocks[name] ~= nil then
        if wj.motorSpeed >= 0 then
            wj.motorSpeed = airlocks[name].wheelMotorSpeed;
        else
            wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
        end
        if j.motorSpeed >= 0 then
            j.motorSpeed = airlocks[name].doorMotorSpeed;
        else
            j.motorSpeed = -airlocks[name].doorMotorSpeed;
        end
        snd = airlocks[name].snd;
        wheel = airlocks[name].wheel;
        cancelTimeout(airlocks[name].cookie);
        airlocks[name] = nil;
    end
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

    airlocks[name].wheelMotorSpeed = math.abs(wj.motorSpeed);
    airlocks[name].doorMotorSpeed = math.abs(j.motorSpeed);

    wj.motorSpeed = wj.motorSpeed * 2.0;
    j.motorSpeed = j.motorSpeed * 3.0;

    if (wheel) then
        wj.motorSpeed = -math.abs(wj.motorSpeed);
        airlocks[name].cookie = addTimeout0(function(cookie, dt)
            if (wj.jointAngle <= wj.lowerLimit) then
                cancelTimeout(cookie);
                if haveSound then
                    airlocks[name].snd:stop();
                end
                if wj.motorSpeed >= 0 then
                    wj.motorSpeed = airlocks[name].wheelMotorSpeed;
                else
                    wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
                end
                if j.motorSpeed >= 0 then
                    j.motorSpeed = airlocks[name].doorMotorSpeed;
                else
                    j.motorSpeed = -airlocks[name].doorMotorSpeed;
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
                        if wj.motorSpeed >= 0 then
                            wj.motorSpeed = airlocks[name].wheelMotorSpeed;
                        else
                            wj.motorSpeed = -airlocks[name].wheelMotorSpeed;
                        end
                        if j.motorSpeed >= 0 then
                            j.motorSpeed = airlocks[name].doorMotorSpeed;
                        else
                            j.motorSpeed = -airlocks[name].doorMotorSpeed;
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
        openAirlock(name, false, true);
    end
end

function makeSeaCrate(name)
    local crate = scene:getObjects(name);
    for _, obj in pairs(crate) do
        obj:findPhysicsBodyComponent():setFilterCategoryBits(const.collisionBitCustom2);
    end
    addRipples1(name, 7.0);
end

function weldTentacleBones(tentacles, bones, freq, damping)
    local cookies = {};
    for _, tentacle in pairs(tentacles) do
        for _, bone in pairs(bones) do
            local objs = tentacle:findRenderTentacleComponent().objects;

            local ac = TentacleAttractComponent(freq, damping);

            local dummy = factory:createDummy();
            ac.bone = bone;
            dummy.pos = objs[ac.bone + 1].pos;
            scene:addObject(dummy);

            ac.target = dummy;

            tentacle:addComponent(ac);

            table.insert(cookies, ac);
        end
    end
    return cookies;
end

function unweldTentacleBones(cookies)
    for _, cookie in pairs(cookies) do
        cookie.target:removeFromParent();
        cookie:removeFromParent();
    end
end

function makeHooks(objs, hookFn, unhookFn, ...)
    for i = 1, 2, 1 do
        objs[i].myEyelet = nil;
        objs[i].myHookFn = hookFn;
        objs[i].myUnhookFn = unhookFn;
        objs[i].myArgs = pack2(...);
        objs[i].myBack = i > 1;
        hooks[objs[i].cookie] = objs[i];
    end
end

function makeEyeletHolder(eyeletHolder)
    eyeletHolder.myEyelets = {};
    local eyelets = eyeletHolder:findDummyComponents("eyelet");
    for i = 1, #eyelets, 1 do
        eyeletHolder.myEyelets[i] = {};
        eyeletHolder.myEyelets[i].holder = eyeletHolder;
        eyeletHolder.myEyelets[i].dummy = eyelets[i];
        eyeletHolder.myEyelets[i].hook = nil;
        eyeletHolder.myEyelets[i].j = nil;
        eyeletHolder.myEyelets[i].skip = false;
    end
    eyeletHolder:addComponent(CollisionSensorComponent());
    setSensorEnterListener(eyeletHolder.name, false, function(other)
        local minDist = 100000;
        local eyelet = nil;
        for _, tmp in pairs(eyeletHolder.myEyelets) do
            local dist = (other.pos - tmp.dummy.worldPos):lenSq();
            if dist < minDist then
                minDist = dist;
                eyelet = tmp;
            end
        end
        if eyelet.skip then
            eyelet.skip = false;
            return;
        end
        if eyelet.hook ~= nil then
            return;
        end
        if hooks[other.cookie] == nil then
            return;
        end
        other = hooks[other.cookie];
        if other.myEyelet ~= nil then
            return;
        end
        local aw = scene.player:findPlayerComponent().altWeapon;
        if aw.heldObject == other then
            aw:cancel();
        end
        other.myEyelet = eyelet;
        other:setTransform(eyelet.dummy.worldTransform);
        if other.myBack then
            other.angle = other.angle - math.pi;
        end
        eyelet.j = scene:addWeldJoint(other, eyeletHolder, eyelet.dummy.worldPos, false);
        eyelet.hook = other;
        if other.myHookFn ~= nil then
            other.myHookFn(other, eyelet, unpack2(other.myArgs));
        end
    end);
end

addTimeout(0.1, function()
    local obj = scene.player:findPlayerComponent().altWeapon.heldObject;
    if (obj == nil) or (hooks[obj.cookie] == nil) then
        return;
    end
    obj = hooks[obj.cookie];
    local eyelet = obj.myEyelet;
    if eyelet == nil then
        return;
    end
    eyelet.j:remove();
    obj.active = false;
    obj.active = true;
    eyelet.skip = true;
    eyelet.j = nil;
    eyelet.hook = nil;
    obj.myEyelet = nil;
    if obj.myUnhookFn ~= nil then
        obj.myUnhookFn(obj, eyelet, unpack2(obj.myArgs));
    end
end);

local function makeFloor(sensor)
    sensor.objs = {};
    sensor:findCollisionSensorComponent().listener = createSensorListener(function(other, args)
        if sensor.objs[other.cookie] == nil then
            sensor.objs[other.cookie] = { count = 1, obj = other };
        else
            sensor.objs[other.cookie].count = sensor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        sensor.objs[other.cookie].count = sensor.objs[other.cookie].count - 1;
        if sensor.objs[other.cookie].count == 0 then
            sensor.objs[other.cookie] = nil;
        end
    end);
end

local function makeToxicSea(sensor)
    sensor.objs = {};
    sensor:findCollisionSensorComponent().listener = createSensorListener(function(other, args)
        if sensor.objs[other.cookie] == nil then
            sensor.objs[other.cookie] = { count = 1, obj = other, fade = false };
        else
            sensor.objs[other.cookie].count = sensor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        sensor.objs[other.cookie].count = sensor.objs[other.cookie].count - 1;
        if sensor.objs[other.cookie].count == 0 then
            sensor.objs[other.cookie] = nil;
        end
    end);
    addTimeout0(function(cookie, dt)
        local aw = scene.player:findPlayerComponent().altWeapon;
        for c, v in pairs(sensor.objs) do
            if (floor.objs[c] == nil) and (not v.fade) and (aw.heldObject ~= v.obj) and (v.obj.linearVelocity:lenSq() < 10.0 * 10.0) then
                v.fade = true;
                v.obj:addComponent(FadeOutComponent(0.5));
            end
        end
    end);
end

local function makePlanks(sensor)
    sensor.objs = {};
    sensor:findCollisionSensorComponent().listener = createSensorListener(function(other, args)
        if sensor.objs[other.cookie] == nil then
            sensor.objs[other.cookie] = { count = 1, obj = other };
        else
            sensor.objs[other.cookie].count = sensor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        sensor.objs[other.cookie].count = sensor.objs[other.cookie].count - 1;
        if sensor.objs[other.cookie].count == 0 then
            sensor.objs[other.cookie] = nil;
        end
    end);
end

-- main

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.2, 0.2, 0.2, 1.0};

math.randomseed(os.time());

do
    local bg = factory:createBackground("toxic2/0.png", 8, 8, vec2(1.0, 1.0), const.zOrderBackground);
    makeToxic2(bg, bg:findRenderBackgroundComponent());
    scene:addObject(bg);
end

makeFloor(floor);
makePlanks(planks);
makeToxicSea(scene:getObjects("toxic_sea")[1]);

setAmbientMusic("ambient10.ogg");
startAmbientMusic(false);

require("e1m7_part0");
require("e1m7_part1");
require("e1m7_part2");
require("e1m7_part3");
require("e1m7_part4");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
end

if (settings.developer >= 2) and (settings.developer <= 15) then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 100);
end

if settings.developer == 7 then
    scene:getObjects("fspawn13_cp")[1].active = false;
    scene:getObjects("fspawn14_cp")[1].active = false;
end

if settings.developer == 8 then
    scene:getObjects("fspawn1_cp")[1].active = false;
    openVessel0();
end

if settings.developer == 9 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
end

if settings.developer >= 12 then
    stopPlatforms0();
end

if (settings.developer >= 12) and (settings.developer <= 15) then
    addTimeoutOnce(0.0, function()
        startMusic("action11.ogg", true);
    end);
end

if ((settings.developer >= 12) and (settings.developer <= 15)) or (settings.developer == 21) then
    addTimeoutOnce(0.0, function()
        local boat1 = scene:getObjects("boat1")[1];
        boat1:setTransformRecursive(scene.player:getTransform());
        boat1.bodyType = const.BodyDynamic;
        boat1:findBoatComponent().active = true;
        scene.inputPlayer.active = false;
        scene.inputBoat.active = true;
        scene.player:findPlayerComponent().standing = true;
        scene.camera:findCameraComponent():zoomTo(55, const.EaseLinear, 0.0);
        scene.camera:findCameraComponent().target = boat1;
    end);
end

if settings.developer >= 16 then
    local boat1 = scene:getObjects("boat1")[1];
    boat1:setTransformRecursive(scene:getObjects("player_16_boat")[1]:getTransform());
end

if settings.developer >= 17 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypePlasmaGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 30);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypePlasmaGun, 50);
end

if settings.developer == 19 then
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 15);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypePlasmaGun, 150);
end

if settings.developer == 20 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemDynomite);
    scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("dynomite_ph")[1].pos);
end

if settings.developer >= 22 then
    local boat1 = scene:getObjects("boat1")[1];
    boat1:setTransformRecursive(scene:getObjects("player_22_boat")[1]:getTransform());
end
