local music = nil;
local ambientMusic = "";
local mazeContent = {};
local mazeSpecialContent = {};
local mazeContentIndex = {};
local mazeSpecialContentIndex = {};
local guns2 = {};
isAmbient = false;
playerInDoor = false;
preBossPos = nil;
bossDoor = nil;
bossInst = nil;

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

function openMazeAirlock(inst, haveSound)
    local snd = nil;
    local wheel = true;
    if inst.my ~= nil then
        snd = inst.my.snd;
        wheel = inst.my.wheel;
        cancelTimeout(inst.my.cookie);
        inst.my = nil;
    end
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
    inst.my = {};
    if (snd == nil) and haveSound then
        inst.my.snd = audio:createSound("door_move.ogg");
        inst.my.snd.loop = true;
        inst.my.snd:play();
    else
        inst.my.snd = snd;
    end
    inst.my.wheel = wheel;

    wj.motorSpeed = math.abs(wj.motorSpeed);

    if (not wheel) then
        j.motorSpeed = math.abs(j.motorSpeed);
        inst.my.cookie = addTimeout0(function(cookie, dt)
            if (j:getJointTranslation() >= j.upperLimit) then
                cancelTimeout(cookie);
                if inst.my.snd ~= nil then
                    inst.my.snd:stop();
                end
                inst.my = nil;
            end
        end);
    else
        inst.my.cookie = addTimeout0(function(cookie, dt)
            if (wj.jointAngle >= wj.upperLimit) then
                if inst.my.snd ~= nil then
                    inst.my.snd:stop();
                    inst.my.snd = nil;
                end
                if haveSound then
                    inst.my.snd = audio:createSound("servo_move.ogg");
                    inst.my.snd.loop = true;
                    inst.my.snd:play();
                end
                inst.my.wheel = false;
                cancelTimeout(cookie);
                j.motorSpeed = math.abs(j.motorSpeed);
                inst.my.cookie = addTimeout0(function(cookie, dt)
                    if (j:getJointTranslation() >= j.upperLimit) then
                        cancelTimeout(cookie);
                        if inst.my.snd ~= nil then
                            inst.my.snd:stop();
                        end
                        inst.my = nil;
                    end
                end);
            end
        end);
    end
end

function closeMazeAirlock(inst, haveSound)
    local snd = nil;
    local wheel = false;
    if inst.my ~= nil then
        snd = inst.my.snd;
        wheel = inst.my.wheel;
        cancelTimeout(inst.my.cookie);
        inst.my = nil;
    end
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
    inst.my = {};
    if (snd == nil) and haveSound then
        inst.my.snd = audio:createSound("servo_move.ogg");
        inst.my.snd.loop = true;
        inst.my.snd:play();
    else
        inst.my.snd = snd;
    end
    inst.my.wheel = wheel;

    j.motorSpeed = -math.abs(j.motorSpeed);

    if (wheel) then
        wj.motorSpeed = -math.abs(wj.motorSpeed);
        inst.my.cookie = addTimeout0(function(cookie, dt)
            if (wj.jointAngle <= wj.lowerLimit) then
                cancelTimeout(cookie);
                if inst.my.snd ~= nil then
                    inst.my.snd:stop();
                end
                inst.my = nil;
            end
        end);
    else
        inst.my.cookie = addTimeout0(function(cookie, dt)
            if (j:getJointTranslation() <= j.lowerLimit) then
                if inst.my.snd ~= nil then
                    inst.my.snd:stop();
                    inst.my.snd = nil;
                end
                if haveSound then
                    inst.my.snd = audio:createSound("door_move.ogg");
                    inst.my.snd.loop = true;
                    inst.my.snd:play();
                end
                inst.my.wheel = true;
                cancelTimeout(cookie);
                wj.motorSpeed = -math.abs(wj.motorSpeed);
                inst.my.cookie = addTimeout0(function(cookie, dt)
                    if (wj.jointAngle <= wj.lowerLimit) then
                        cancelTimeout(cookie);
                        if inst.my.snd ~= nil then
                            inst.my.snd:stop();
                        end
                        inst.my = nil;
                    end
                end);
            end
        end);
    end
end

function makeMazeAirlock(pos, angle)
    local inst = scene:instanciate("e1m12_airlock1.json", pos, angle);
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
    findObject(inst.objects, "airlock_cp"):findCollisionSensorComponent().listener = createSensorListener(function(other, self)
        if (other.type == const.SceneObjectTypePlayer) then
            if (inst.myC[1] ~= nil) and (inst.myC[1].myActivateFirst == nil) then
                inst.myC[1].myActivateFirst = 1;
                inst.myC[1].myActivateFn(inst.myC[1]);
            end
            if (inst.myC[2] ~= nil) and (inst.myC[2].myActivateFirst == nil) then
                inst.myC[2].myActivateFirst = 1;
                inst.myC[2].myActivateFn(inst.myC[2]);
            end
            local wasAct = self.act;
            self.act = true;
            if (self.num > 0) and (not wasAct) then
                openMazeAirlock(inst, true);
            end
            playerInDoor = true;
        end
        if self.num == 0 then
            if self.act then
                openMazeAirlock(inst, (other.type == const.SceneObjectTypePlayer));
            end
        end
        self.num = self.num + 1;
    end, function (other, self)
        if (other.type == const.SceneObjectTypePlayer) then
            playerInDoor = false;
        end
        self.num = self.num - 1;
        if self.num == 0 then
            if self.act then
                closeMazeAirlock(inst, (other.type == const.SceneObjectTypePlayer));
            end
        end
    end, { num = 0, act = false });
    return inst;
end

function makeMazeKeyAirlock(pos, angle, color)
    local inst = scene:instanciate("e1m12_airlock1_"..color..".json", pos, angle);
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
    findObject(inst.objects, "key_door").angle = 0;
    findObject(inst.objects, "key_ph").angle = math.pi / 2;

    local doorObj = findObject(inst.objects, "key_door");
    local phObj = findObject(inst.objects, "key_ph");
    local phC = phObj:findPlaceholderComponent();

    phC.listener = createSensorEnterListener(false, function(other, fn, args)
        if not scene.player:findPlayerComponent().inventory:have(phC.item) then
            return;
        end
        scene.player:findPlayerComponent().inventory:take(phC.item);
        phObj:removeFromParent();
        doorObj:findRenderQuadComponents("key")[1].visible = true;
        audio:playSound("key.ogg");

        if (inst.myC[1] ~= nil) and (inst.myC[1].myActivateFirst == nil) then
            inst.myC[1].myActivateFirst = 1;
            inst.myC[1].myActivateFn(inst.myC[1]);
        end
        if (inst.myC[2] ~= nil) and (inst.myC[2].myActivateFirst == nil) then
            inst.myC[2].myActivateFirst = 1;
            inst.myC[2].myActivateFn(inst.myC[2]);
        end

        openMazeAirlock(inst, false);
    end);

    return inst;
end

function makeAirlock(inst, opened)
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
    if opened then
        openMazeAirlock(inst, false);
    end
end

function mazeRegisterContent(blocks, content, createFn, activateFn)
    local cnt = { nil };
    for _, v in pairs(blocks) do
        if mazeContent[v] == nil then
            mazeContent[v] = {};
        end
        local f1 = createFn;
        local f2 = activateFn;
        if f1 == nil then
            f1 = function() end;
        end
        if f2 == nil then
            f2 = function() end;
        end
        table.insert(mazeContent[v], { content, f1, f2 });
        mazeContentIndex[v] = cnt;
    end
end

function mazeRegisterSpecialContent(blocks, content, createFn, activateFn)
    local cnt = { nil };
    for _, v in pairs(blocks) do
        if mazeSpecialContent[v] == nil then
            mazeSpecialContent[v] = {};
        end
        local f1 = createFn;
        local f2 = activateFn;
        if f1 == nil then
            f1 = function() end;
        end
        if f2 == nil then
            f2 = function() end;
        end
        table.insert(mazeSpecialContent[v], { content, f1, f2 });
        mazeSpecialContentIndex[v] = cnt;
    end
end

function mazeCreateBlock(block, pos, angle, key)
    scene:instanciate(block, pos, angle);
    local tmpc = mazeContent[block];
    if mazeContentIndex[block][1] == nil then
        mazeContentIndex[block][1] = math.random(1, #tmpc);
    end
    mazeContentIndex[block][1] = mazeContentIndex[block][1] + 1;
    if (mazeContentIndex[block][1] > #tmpc) then
        mazeContentIndex[block][1] = 1;
    end
    tmpc = tmpc[mazeContentIndex[block][1]];
    local inst = scene:instanciate(tmpc[1], pos, angle);
    inst.myActivateFn = tmpc[3];
    inst.mySpecial = false;

    local objs = findObjects(inst.objects, "ammo");
    objs = table.copy(objs, findObjects(inst.objects, "ws"));
    for _ , obj in pairs(objs) do
        obj.angle = 0;
    end

    tmpc[2](inst, key);

    if key ~= -1 then
        local obj = findObject(inst.objects, "key1");

        if obj ~= nil then
            local k = factory:createPowerupInventory(key);
            k.pos = obj.pos;
            scene:addObject(k);
        end
    end

    return inst;
end

function mazeCreateSpecialBlock(block, pos, angle)
    scene:instanciate(block, pos, angle);
    local tmpc = mazeSpecialContent[block];
    if mazeSpecialContentIndex[block][1] == nil then
        mazeSpecialContentIndex[block][1] = math.random(1, #tmpc);
    end
    mazeSpecialContentIndex[block][1] = mazeSpecialContentIndex[block][1] + 1;
    if (mazeSpecialContentIndex[block][1] > #tmpc) then
        mazeSpecialContentIndex[block][1] = 1;
    end
    tmpc = tmpc[mazeSpecialContentIndex[block][1]];
    local inst = scene:instanciate(tmpc[1], pos, angle);
    inst.myActivateFn = tmpc[3];
    inst.mySpecial = true;

    local objs = findObjects(inst.objects, "ammo");
    objs = table.copy(objs, findObjects(inst.objects, "ws"));
    for _ , obj in pairs(objs) do
        obj.angle = 0;
    end

    tmpc[2](inst);
    return inst;
end

function makeFlickerLamps(cinst, name)
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
        local t = 0.005 + math.random() * 0.015;
        self.t = self.t - t;
        addTimeoutOnce(t, flickerTimeout2, self);
    end

    local function flickerTimeout(self)
        self.t = 0.01 + math.random() * 0.25;
        flickerTimeout2(self);
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, self);
    end

    local insts = cinst:getInstances(name);
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, { lights = lights, t = 0, defaultOn = lights[1].visible });
    end
end

function openHatch(cinst, name)
    local inst = cinst:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = math.abs(j.motorSpeed);
end

function closeHatch(cinst, name)
    local inst = cinst:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = -math.abs(j.motorSpeed);
end

function makeGun2(inst, dieCb, ...)
    inst.myObj = findObject(inst.objects, "gun2_root");
    inst.myTrunk = findObject(inst.objects, "gun2_trunk");
    inst.myFire = findObjects(inst.objects, "gun2_fire1");

    inst.myObj.type = const.SceneObjectTypeEnemyBuilding;
    inst.myObj.maxLife = settings.heater1.life;
    inst.myObj.life = settings.heater1.life;

    local hb = RenderHealthbarComponent(vec2(0.0, 4.0), 0, 6.0, 1.2, const.zOrderMarker + 1);
    hb.color = { 1.0, 1.0, 1.0, 0.6 };
    inst.myObj:addComponent(hb);

    local dummy = inst.myTrunk:findDummyComponent("missile");
    inst.weapon = WeaponHeaterComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.damage = 60.0;
    inst.weapon.length = 100.0;
    inst.weapon.impulse = 50.0;
    inst.weapon.duration = 0.8;
    inst.weapon.interval = 0.0;
    inst.weapon.minWidth = 0.8;
    inst.weapon.maxWidth = 1.8;
    inst.weapon.haveSound = false;
    inst.myTrunk:addComponent(inst.weapon);
    inst.weapon:trigger(true);

    inst.myDieCb = dieCb;
    inst.myArgs = pack2(...);

    local light = inst.myObj:findLightComponent():getLights()[1];

    light.myColor = light.color[4];
    local tweening = SequentialTweening(true);
    tweening:addTweening(SingleTweening(0.4, const.EaseOutQuad, light.color[4], 1.0, false));
    tweening:addTweening(SingleTweening(0.4, const.EaseInQuad, 1.0, light.color[4], false));
    inst.myTimer = addTimeout0(function(cookie, dt, self)
        local c = light.color;
        c[4] = tweening:getValue(self.t);
        light.color = c;
        self.t = self.t + dt;
    end, { t = 0 });

    guns2[inst.myObj.cookie] = inst;
end

addTimeout0(function(cookie, dt)
    for _, inst in pairs(guns2) do
        if inst.myObj:dead() then
            guns2[inst.myObj.cookie] = nil;
            cancelTimeout(inst.myTimer);
            inst.myObj:findRenderHealthbarComponent():removeFromParent();
            inst.myObj:findLightComponent():removeFromParent();
            inst.weapon:trigger(false);
            for i = 1, #inst.myFire, 1 do
                addTimeoutOnce(0.5 * (i - 1), function()
                    local obj = factory:createExplosion1(106);
                    obj.pos = inst.myFire[i].pos;
                    scene:addObject(obj);
                    inst.myFire[i].angle = 0;
                    inst.myFire[i].visible = true;
                end);
            end
            inst.myDieCb(unpack2(inst.myArgs));
        end
    end
end);

-- main

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

if settings.developer == 0 then
    scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);
else
    scene.camera:findCameraComponent():zoomTo(40, const.EaseLinear, 0);
end

scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};

math.randomseed(os.time());

setAmbientMusic("ambient14.ogg");
startAmbientMusic(false);

require("e1m12_part1");
require("e1m12_part0");
require("e1m12_part2");
require("e1m12_part3");

if settings.developer == 1 then
    scene.respawnPoint = scene:getObjects("player_1")[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
elseif settings.developer >= 2 then
    scene.respawnPoint = transform(preBossPos, rot(0));
    scene.player:setTransform(scene.respawnPoint);
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemBlueKey);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypePlasmaGun);
    --scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeLGun);
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeProxMine);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 50);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 50);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypePlasmaGun, 250);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeProxMine, 10);
end
