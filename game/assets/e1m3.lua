local music = nil;
local airlocks = {};
local ambientMusic = "";
local isAmbient = false;

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

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.05, 0.05, 0.05, 1.0};

math.randomseed(os.time());

scene:addObject(factory:createBackground("ground2.png", 10.24, 10.24, vec2(1.0, 1.0), const.zOrderBackground));

if settings.developer < 3 then
    setAmbientMusic("ambient4.ogg");
    startAmbientMusic(false);
end

require("e1m3_part0");
require("e1m3_part1");
require("e1m3_part2");
require("e1m3_cart");
require("e1m3_part3");
require("e1m3_pipes");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
end

if settings.developer >= 2 then
    scene.player:findPlayerComponent().flashlight = true;
end

if settings.developer >= 3 then
    setAmbientMusic("ambient3.ogg");
    startAmbientMusic(true);
end

if settings.developer <= 14 then
    if settings.developer >= 7 then
        cartDevSetup(1);
        scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
        scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 50);
    end

    if settings.developer >= 8 then
        setAmbientMusic("ambient5.ogg");
        startAmbientMusic(true);
    end

    if settings.developer >= 9 then
        setNoStop14();
    end

    if settings.developer >= 13 then
        setNoStop5();
    end

    if settings.developer >= 14 then
        setNoStop6();
    end
end

if settings.developer >= 15 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemDynomite);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
end

if settings.developer == 17 then
    cartDevSetup(3);
end

if settings.developer >= 18 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemDynomite);
end
