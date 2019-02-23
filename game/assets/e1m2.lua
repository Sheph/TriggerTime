local music = nil;
local ambientMusic = "";
local doors = {};
local guns2 = {};
isAmbient = false;
bg = {};

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

function openJacket(name)
    local stream = audio:createSound("servo_move.ogg");
    stream:play();
    addTimeoutOnce(1.25, function()
        stream:stop();
    end);
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "jacket1_joint");
    j.motorSpeed = -j.motorSpeed;
end

function openStone(joint)
    local j = scene:getJoints(joint)[1];
    j.motorSpeed = -j.motorSpeed;
    scene.camera:findCameraComponent():tremorStart(0.3);
    local s = audio:createSound("stone_moving.ogg");
    s:play();
    addTimeout0(function(cookie, dt)
        if j:getJointTranslation() <= j.lowerLimit then
            cancelTimeout(cookie);
            addTimeoutOnce(0.2, function()
                scene.camera:findCameraComponent():tremor(false);
                s:stop();
            end);
        end
    end);
end

function closeStone(joint)
    local j = scene:getJoints(joint)[1];
    j.motorSpeed = -j.motorSpeed;
    scene.camera:findCameraComponent():tremorStart(0.3);
    local s = audio:createSound("stone_moving.ogg");
    s:play();
    addTimeout0(function(cookie, dt)
        if j:getJointTranslation() >= j.upperLimit then
            cancelTimeout(cookie);
            addTimeoutOnce(0.2, function()
                scene.camera:findCameraComponent():tremor(false);
                s:stop();
            end);
        end
    end);
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
        local t = 0.005 + math.random() * 0.015;
        self.t = self.t - t;
        addTimeoutOnce(t, flickerTimeout2, self);
    end

    local function flickerTimeout(self)
        self.t = 0.01 + math.random() * 0.25;
        flickerTimeout2(self);
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, self);
    end

    local insts = scene:getInstances(name);
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, { lights = lights, t = 0, defaultOn = lights[1].visible });
    end
end

local scl = 5.0;

function setupBgAir()
    if bg[1] ~= nil then
        bg[1]:removeFromParent();
    end
    if bg[2] ~= nil then
        bg[2]:removeFromParent();
    end
    if bg[3] ~= nil then
        bg[3]:removeFromParent();
    end

    bg[1] = factory:createBackground("ground1.png", 10, 10, vec2(0.2, 0.2), const.zOrderBackground);
    bg[1]:findRenderBackgroundComponent().color = {0.6, 0.6, 0.6, 1.0};
    scene:addObject(bg[1]);

    bg[2] = factory:createBackground("fog.png", 544 / scl / 2, 416 / scl / 2, vec2(10.0, 1.0), const.zOrderBackground + 1)
    bg[2]:findRenderBackgroundComponent().unbound = true;
    bg[2]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.6};
    scene:addObject(bg[2]);

    bg[3] = factory:createBackground("fog.png", 544 / scl / 1.5, 416 / scl / 1.5, vec2(15.0, 1.0), const.zOrderBackground + 2)
    bg[3]:findRenderBackgroundComponent().unbound = true;
    bg[3]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.7};
    bg[3]:findRenderBackgroundComponent().offset = vec2(0, 416 / scl / 1.5 / 2);
    scene:addObject(bg[3]);
end

function setupBgMetal()
    if bg[1] ~= nil then
        bg[1]:removeFromParent();
    end
    if bg[2] ~= nil then
        bg[2]:removeFromParent();
        bg[2] = nil;
    end
    if bg[3] ~= nil then
        bg[3]:removeFromParent();
        bg[3] = nil;
    end
    bg[1] = factory:createBackground("metal1.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground);
    scene:addObject(bg[1]);
end

function moveDoor(open, name, quiet)
    local snd = nil;
    if doors[name] ~= nil then
        snd = doors[name].snd;
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_bar_joint");
    if (open and j:getJointTranslation() >= j.upperLimit) or
       (not open and j:getJointTranslation() <= j.lowerLimit) then
        if snd ~= nil then
            snd:stop();
        end
        return;
    end
    doors[name] = {};
    if (snd == nil) and not quiet then
        doors[name].snd = audio:createSound("door_move.ogg");
        doors[name].snd.loop = true;
        doors[name].snd:play();
    else
        doors[name].snd = snd;
    end
    if open then
        j.motorSpeed = math.abs(j.motorSpeed);
    else
        j.motorSpeed = -math.abs(j.motorSpeed);
    end
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (open and j:getJointTranslation() >= j.upperLimit) or
           (not open and j:getJointTranslation() <= j.lowerLimit) then
            if not quiet then
                doors[name].snd:stop();
            end
            doors[name] = nil;
            cancelTimeout(cookie);
        end
    end);
end

function makeDoor(name, opened)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "door1_bar"),
        findObject(inst.objects, "door1_wheel"),
        findJoint(inst.joints, "door1_bar_joint"),
        findJoint(inst.joints, "door1_wheel_joint"),
        1, false);
    if opened then
        moveDoor(true, name, true);
    end
end

function fixedSpawn(name)
    local spawn = scene:getObjects(name.."_tetrobot2");
    for _, obj in pairs(spawn) do
        local e = factory:createTetrobot2();
        addSpawnedEnemyFreezable(e, obj);
        e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
    end
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
    inst.weapon.length = 140.0;
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

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.3, 0.3, 0.3, 1.0};

math.randomseed(os.time());

setupBgAir();

bg[4] = factory:createBackground("fog.png", 544 / scl, 416 / scl, vec2(30.0, 1.0), 110)
bg[4]:findRenderBackgroundComponent().unbound = true;
bg[4]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.8};
scene:addObject(bg[4]);

setAmbientMusic("ambient2.ogg");
startAmbientMusic(false);

require("e1m2_part0");
require("e1m2_part1");
require("e1m2_part2");
require("e1m2_part3");
require("e1m2_part4");
require("e1m2_part5");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
    bg[4]:removeFromParent();
    bg[4] = nil;
end

if settings.developer == 1 then
    showMoveFireInputHint();
end

if settings.developer >= 6 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
end

if settings.developer == 6 then
    for i = 1, 7, 1 do
        scene:getJoints("rock"..i.."_joint")[1]:remove();
    end
end

if settings.developer >= 7 then
    setupBgMetal();
end

if (settings.developer >= 8) and (settings.developer <= 12) then
    setupTrain(2);
end

if settings.developer >= 9 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeSuperShotgun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeShotgun, 20);
end

if settings.developer == 12 then
    hallway1Comeback();
end

if (settings.developer >= 13) and (settings.developer <= 18) then
    setupTrain(3);
end

if (settings.developer >= 14) and (settings.developer <= 16) then
    startMusic("action4.ogg", false);
end

if (settings.developer >= 15) and (settings.developer <= 16) then
    setupBgAir();
end

if settings.developer == 17 then
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("door6_light1")[1].visible = false;
    scene:getObjects("terrain0")[1]:findLightComponent():getLights("door6_light2")[1].visible = false;
    scene:getJoints("door9_joint")[1].motorSpeed = -scene:getJoints("door9_joint")[1].motorSpeed;
end

if settings.developer == 18 then
    moveDoor(true, "door6", true);
end

if settings.developer == 19 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
end
