local airlocks = {};
local music = nil;
local ambientMusic = "";
local isAmbient = false;
local alarmLights = {};
stopAllConveyors = false;

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

function makeGasPipe(name)
    local rc = scene:getObjects(name)[1]:findRenderStripeComponents("gas")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "gas1", 1);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects(name)[1]:addComponent(ac);
end

function makeLavaPipe(name)
    local rc = scene:getObjects(name)[1]:findRenderStripeComponents("lava")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "goo1", 1);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects(name)[1]:addComponent(ac);
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
        if stopAllConveyors then
            cancelTimeout(cookie);
            return;
        end
        local dir = conveyor:getDirection(velocity * dt);
        for _, v in pairs(conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
    end);
end

function makeConveyor(name, velocity, fn, ...)
    makeConveyor2(name, velocity, nil, fn, ...);
end

function alarmLightOn(name, silent)
    if alarmLights[name] ~= nil then
        return;
    end;
    local lights = {};
    local insts = scene:getInstances(name);
    for _, inst in pairs(insts) do
        local light = findObject(inst.objects, "lamp"):findLightComponent():getLights()[1];
        table.insert(lights, light);
    end
    local tweening = SequentialTweening(true);
    tweening:addTweening(SingleTweening(0.4, const.EaseOutQuad, 0.2, 1.0, false));
    tweening:addTweening(SingleTweening(0.4, const.EaseInQuad, 1.0, 0.2, false));
    alarmLights[name] = addTimeout0(function(cookie, dt, self)
        for _, light in pairs(lights) do
            local c = light.color;
            c[4] = tweening:getValue(self.t);
            light.color = c;
        end
        self.t = self.t + dt;
    end, { t = 0 });
    if not silent then
        audio:playSound("alarm1.ogg");
    end
end

function alarmLightOff(name, silent)
    if alarmLights[name] == nil then
        return;
    end
    local lights = {};
    local insts = scene:getInstances(name);
    for _, inst in pairs(insts) do
        local light = findObject(inst.objects, "lamp"):findLightComponent():getLights()[1];
        table.insert(lights, light);
    end
    local tweening = SingleTweening(0.4, const.EaseOutQuad, lights[1].color[4], 0.0, false);
    addTimeout0(function(cookie, dt, self)
        for _, light in pairs(lights) do
            local c = light.color;
            c[4] = tweening:getValue(self.t);
            light.color = c;
        end
        if tweening:finished(self.t) then
            cancelTimeout(cookie);
        end
        self.t = self.t + dt;
    end, { t = 0 });
    cancelTimeout(alarmLights[name]);
    alarmLights[name] = nil;
    if not silent then
        audio:playSound("alarm_off.ogg");
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

function makeLavaDamage(name, damage)
    local sensor = scene:getObjects(name)[1];
    sensor.objs = {};
    setSensorListener(name, function(other, args)
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
    addTimeout0(function(cookie, dt)
        for _, v in pairs(sensor.objs) do
            v.obj:changeLife(-damage * dt);
        end
    end);
end

function setupDisplay(name)
    local rc = scene:getObjects("terrain1")[1]:findRenderProjComponents(name)[1];
    local display = AnimationComponent(rc.drawable);
    display:addAnimation(const.AnimationDefault, "tv_def", 1);
    display:addAnimation(const.AnimationDefault + 1, "tv_natan", 1);
    display:startAnimation(const.AnimationDefault);
    scene:getObjects("terrain1")[1]:addComponent(display);

    return display;
end

function fixedSpawn(name)
    local spawn = scene:getObjects(name.."_tetrobot2");
    for _, obj in pairs(spawn) do
        local e = factory:createTetrobot2();
        addSpawnedEnemyFreezable(e, obj);
        e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
    end
    local spawn = scene:getObjects(name.."_enforcer1");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createEnforcer1(), obj);
    end
end

function spawnBots(name, scorpProb)
    local spawn = scene:getObjects(name.."_bots1");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 1 / scorpProb) == 1 then
            e = factory:createScorp3();
        else
            e = factory:createTetrocrab3();
        end
        addSpawnedEnemyFreezable(e, obj);
    end
    spawn = scene:getObjects(name.."_bots2");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 1 / scorpProb) == 1 then
            e = factory:createScorp4();
        else
            e = factory:createTetrocrab4();
        end
        addSpawnedEnemyFreezable(e, obj);
    end
end

-- main

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};

math.randomseed(os.time());

scene:addObject(factory:createBackground("metal5.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground));

setAmbientMusic("ambient6.ogg");
startAmbientMusic(false);

require("e1m4_part0");
require("e1m4_part1");
require("e1m4_part2");
require("e1m4_part3");
require("e1m4_part4");
require("e1m4_final");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
end

if settings.developer >= 2 then
    scene:getObjects("comm_cp1")[1]:removeFromParent();
    scene:getObjects("comm_cp2")[1]:removeFromParent();
    scene:getObjects("control0_ph")[1]:removeFromParent();
    scene:getObjects("comm_enforcer")[1]:removeFromParent();
    openAirlock("door5");
    openAirlock("door6");
    closeAirlock("door4");
    alarmLightOn("alarm1", true);
end

if settings.developer >= 5 then
    openAirlock("door1");
    openAirlock("door4");
    setupPart0WayBack();
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperBlaster);
    alarmLightOff("alarm1", true);
end

if settings.developer >= 7 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeProxMine);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeProxMine, 12);
end

if (settings.developer >= 10) and (settings.developer <= 16) then
    setupRod1Done(true);
end

if settings.developer >= 11 then
    openAirlock("door15", true);
    scene:getObjects("display1_cp")[1]:removeFromParent();
    scene:getObjects("display1_cp2")[1]:removeFromParent();
    scene:getObjects("sentry1_cp")[1].active = true;
    scene:getObjects("sentry2_cp")[1].active = true;
end

if settings.developer >= 12 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 15);
    openAirlock("door10", false);
    openAirlock("door29", false);
    alarmLightOff("alarm2", true);
end

if settings.developer >= 17 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
end

if settings.developer >= 18 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemYellowKey);
end
