local music = nil;
local ambientMusic = "";
local isAmbient = false;
local levers = {};
local doors = {};

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

function doorOpened(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_left_joint");
    return j:getJointTranslation() >= j.upperLimit;
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

-- main

scene.player:findPlayerComponent().haveBackpack = false;

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.2, 0.2, 0.2, 1.0};

math.randomseed(os.time());

setAmbientMusic("ambient7.ogg");
startAmbientMusic(false);

require("e1m5_part0");
require("e1m5_yard");
require("e1m5_part1");
require("e1m5_part2");
require("e1m5_part3");
require("e1m5_part4");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
end

if settings.developer >= 2 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeChainsaw);
end

if settings.developer >= 6 then
    openDoor("door7", false);
    scene:getObjects("gun9_cp")[1].active = true;
    scene:getObjects("gun9_pre_cp")[1].active = false;
end

if settings.developer >= 9 then
    openDoor("door10", false);
    openDoor("door11", false);
    openDoor("door12", false);
    openDoor("door13", false);
end

if settings.developer >= 12 then
    local laser5 = scene:getObjects("laser5");
    for _, obj in pairs(laser5) do
        obj:removeFromParent();
    end
    local laser6 = scene:getObjects("laser6");
    for _, obj in pairs(laser6) do
        obj:removeFromParent();
    end
end

if settings.developer >= 13 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemBlueKey);
end

if settings.developer >= 14 then
    scene.player:findPlayerComponent().haveBackpack = true;
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);
end

if settings.developer >= 15 then
    openDoor("door18", false);
end

if settings.developer >= 16 then
    setAmbientMusic("ambient8.ogg");
    startAmbientMusic(true);
    scene:getObjects("exit1_cp")[1].active = false;
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeMachineGun);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeProxMine);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 200);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeProxMine, 12);
end

if settings.developer >= 19 then
    local laser13 = scene:getObjects("laser13");
    for _, obj in pairs(laser13) do
        obj:removeFromParent();
    end
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 20);
end

if settings.developer >= 20 then
    finalSequence();
end
