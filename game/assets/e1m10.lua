local music = nil;
local ambientMusic = "";
local isAmbient = false;
local airlocks = {};
boss1 = scene:getObjects("boss1")[1];
boss2 = scene:getObjects("boss2")[1];
bg = nil;

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

function weldTentacleBones(tentacle, bones, targets, freq, damping)
    local cookies = {};
    for i, bone in pairs(bones) do
        local objs = tentacle:findRenderTentacleComponent().objects;

        local ac = TentacleAttractComponent(freq, damping);

        ac.bone = bone;
        ac.target = targets[i];

        tentacle:addComponent(ac);

        table.insert(cookies, ac);
    end
    return cookies;
end

function unweldTentacleBones(cookies)
    for _, cookie in pairs(cookies) do
        cookie:removeFromParent();
    end
end

function weldTentacleBones2(tentacle, bones, freq, damping)
    local cookies = {};
    local objs = tentacle:findRenderTentacleComponent().objects;
    for _, bone in pairs(bones) do
        local ac = TentacleAttractComponent(freq, damping);

        local dummy = factory:createDummy();
        ac.bone = bone;
        dummy.pos = objs[ac.bone + 1].pos;
        scene:addObject(dummy);

        ac.target = dummy;

        tentacle:addComponent(ac);

        table.insert(cookies, ac);
    end
    return cookies;
end

function unweldTentacleBones2(cookies)
    for _, cookie in pairs(cookies) do
        cookie.target:removeFromParent();
        cookie:removeFromParent();
    end
end

-- main

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};

math.randomseed(os.time());

bg = factory:createBackground("ground3.png", 9.0, 9.0, vec2(0.8, 0.8), const.zOrderBackground);
bg:findRenderBackgroundComponent().color = {0.58, 0.58, 0.58, 1.0};
scene:addObject(bg);

setAmbientMusic("ambient13.ogg");
startAmbientMusic(false);

require("e1m10_part0");
require("e1m10_part1");
require("e1m10_part2");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
    scene:getObjects("boss0_cp")[1].active = false;
    scene:getObjects("boss0b_cp")[1].active = false;
    closeAirlock("door2", false);
    closeAirlock("door3", false);
    boss1:findTargetableComponent().autoTarget = true;
    boss2:setTransform(scene:getObjects("target4")[1]:getTransform());
end

if settings.developer == 1 then
    startMusic("action15.ogg", false);
    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
end

if settings.developer == 2 then
    startMusic("action15.ogg", false);
    setSensorListener("boss1_cp", function(other) end, function(other) end);
    boss1:findBossBuddyComponent():setDead();
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 10);
    scene.camera:findCameraComponent():zoomTo(45, const.EaseLinear, 0);
    swallowSequence();
end

if settings.developer >= 3 then
    setSensorListener("boss1_cp", function(other) end, function(other) end);
    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
    boss1:removeFromParent();
    boss1 = nil;
end

if settings.developer == 3 then
    addTimeoutOnce(1.0, function()
        startChamber(false);
    end);
end

if (settings.developer == 4) or (settings.developer == 5) then
    startMusic("action15.ogg", false);
    startChamber(true);
    scene.cutscene = true;
    startNatan();
end

if settings.developer == 6 then
    stopMusic(false);
    scene.lighting.ambientLight = {0.3, 0.3, 0.3, 1.0};
    startChamber(true);
    scene.cutscene = true;
    addTimeoutOnce(0.25, function()
        local stream = audio:createStream("queen_shake.ogg");
        stream.loop = true;
        stream:play();
        startCarl(stream, false);
    end);
end

if settings.developer >= 7 then
    stopMusic(false);
    scene.lighting.ambientLight = {0.3, 0.3, 0.3, 1.0};
    startChamber(true);
    local stream = audio:createStream("queen_shake.ogg");
    stream.loop = true;
    stream:play();
    startCarl(stream, true);
end

if settings.developer >= 8 then
    bg:removeFromParent();
    bg = factory:createBackground("metal5.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground);
    scene:addObject(bg);
end
