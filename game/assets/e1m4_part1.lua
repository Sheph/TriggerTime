local conveyor1Spawn = scene:getObjects("conveyor1_spawn");
local conveyor6Spawn = scene:getObjects("conveyor6_spawn");
local proxmine1 = scene:getObjects("proxmine1");
local proxmine1Spawned = {};
local tb1 = scene:getObjects("tb1");
local guns2 = {};
local gun1 = scene:getInstances("gun1")[1];
local rod1 = scene:getObjects("door32_rod")[1];
local doorLight1 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door32_light")[1];
local ha1Targets = scene:getObjects("ha1_target");

local function showWeaponSwitchInputHint()
    local obj = SceneObject();
    local c = nil;

    if input.usingGamepad then
        obj.pos = vec2(10, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addGamepadBindingHint(c, const.ActionGamepadIdSecondarySwitch);
        c:setDescription(tr.str24);
        obj:addComponent(c);
    else
        obj.pos = vec2(12, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addBindingHint(c, const.ActionIdSecondarySwitch);
        c:setDescription(tr.str24);
        obj:addComponent(c);
    end

    scene:addObject(obj);

    addTimeout0(function(cookie, dt)
        if not scene.inputPlayer.secondarySwitchShowOff then
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                c:setFade(1.0);
            end);
        end
    end);
end

function makeGun2(inst, dieCb, ...)
    inst.myObj = findObject(inst.objects, "gun2_root");
    inst.myTrunk = findObject(inst.objects, "gun2_trunk");
    inst.myFire = findObjects(inst.objects, "gun2_fire1");
    inst.myDamaged = false;

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
        if (not inst.myDamaged) and (inst.myObj.life < inst.myObj.maxLife) then
            inst.myDamaged = true;
            scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():removeAllHints();
        end
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

local function activateLight(light)
    light.visible = true;
    light.myColor = light.color[4];
    local tweening = SequentialTweening(true);
    tweening:addTweening(SingleTweening(0.4, const.EaseOutQuad, light.color[4], 1.0, false));
    tweening:addTweening(SingleTweening(0.4, const.EaseInQuad, 1.0, light.color[4], false));
    light.myTimer = addTimeout0(function(cookie, dt, self)
        local c = light.color;
        c[4] = tweening:getValue(self.t);
        light.color = c;
        self.t = self.t + dt;
    end, { t = 0 });
end

local function deactivateLight(light)
    if light.myTimer ~= nil then
        cancelTimeout(light.myTimer);
        light.myTimer = nil;
        local c = light.color;
        c[4] = light.myColor;
        light.color = c;
        light.visible = false;
    end
end

local function coolRod(rod, fn, ...)
    local rc = rod:findRenderQuadComponent();
    local t = 0;
    addTimeout0(function(cookie, dt, args)
        t = t + dt;
        if t > 2 then
            t = 2;
        end
        rc.color = {1, t / 2, t / 2, 1};
        if t == 2 then
            cancelTimeout(cookie);
            fn(unpack2(args));
        end
    end, pack2(...));
end

-- main

scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent().timeout = 60.0;
for _, obj in pairs(ha1Targets) do
    scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHintTrigger(obj.pos, 1.0);
end
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha1_hint")[1].pos);

if settings.skill <= const.SkillNormal then
    scene:getObjects("ha1")[1].active = true;
end

makeConveyor("conveyor1", 9.0, function(obj)
    if obj.name == "mineral" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

makeConveyor("conveyor6", 9.0, function(obj)
    if obj.name == "mineral" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(1.0, function(cookie)
    if stopAllConveyors then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor1Spawn[math.random(1, #conveyor1Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    local inst = scene:instanciate("e1m4_mineral"..(math.random(1, 4))..".json", xf);

    spawn = conveyor6Spawn[math.random(1, #conveyor6Spawn)];
    xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    inst = scene:instanciate("e1m4_mineral"..(math.random(1, 4))..".json", xf);
end);

makeAirlock("door8", false);
makeAirlockTrigger("door8_cp", "door8");
makeAirlock("door9", false);

setSensorEnterListener("conveyor1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("conveyor1_cp")[1]:getTransform();
    local objs = scene:getObjects("enforcer6");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("proxmine1_cp", false, function(other)
    for i, obj in pairs(proxmine1Spawned) do
        if obj:scene() == nil then
            proxmine1Spawned[i] = nil;
        end
    end
    local numProxmines = #proxmine1 - math.ceil(other:findPlayerComponent():ammo(const.WeaponTypeProxMine) / 3);
    for i = 1,#proxmine1,1 do
        if proxmine1Spawned[i] ~= nil then
            numProxmines = numProxmines - 1;
        end
    end
    for i = 1,#proxmine1,1 do
        if numProxmines <= 0 then
            break;
        end
        if proxmine1Spawned[i] == nil then
            local obj = factory:createPowerupAmmo(const.WeaponTypeProxMine);
            obj:setTransform(proxmine1[i]:getTransform());
            scene:addObject(obj);
            proxmine1Spawned[i] = obj;
            numProxmines = numProxmines - 1;
        end
    end
end);

setSensorEnterListener("proxmine2_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog42.str1},
    }, function ()
        scene.cutscene = false;
        scene.inputPlayer.secondarySwitchShowOff = true;
        showWeaponSwitchInputHint();
    end);
end);

setSensorEnterListener("catwalk2_cp", false, function(other)
    local numAlive = 0;
    for _, obj in pairs(tb1) do
        if (obj.mySpawned ~= nil) and obj.mySpawned:alive() then
            numAlive = numAlive + 1;
        else
            obj.mySpawned = nil;
        end
    end
    if numAlive >= (#tb1 / 2) then
        return;
    end
    table.shuffle(tb1);
    for i = 1, #tb1, 1 do
        if tb1[i].mySpawned == nil then
            local e = factory:createTetrobot();
            e:setTransform(tb1[i]:getTransform());
            e:findTargetableComponent().autoTarget = true;
            scene:addObject(e);
            e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
            tb1[i].mySpawned = e;
            numAlive = numAlive + 1;
        end
        if numAlive >= (#tb1 / 2) then
            break;
        end
    end
end);

makeKeyDoor("blue_door", "blue_key_ph", function()
    openAirlock("door2");
end);

makeAirlock("door32", false);

activateLight(doorLight1);

makeGun2(gun1, function()
    coolRod(rod1, function()
        audio:playSound("alarm_off.ogg");
        deactivateLight(doorLight1);
        openAirlock("door32", true);
    end);
end);

scene:getObjects("sawer1")[1].linearVelocity = vec2(10, 0);
scene:getObjects("sawer2")[1].linearVelocity = vec2(-10, 0);
scene:getObjects("sawer3")[1].linearVelocity = vec2(10, 0);
scene:getObjects("sawer4")[1].linearVelocity = vec2(-10, 0);

setSensorEnterListener("sawer_cp", false, function(other)
    if other.bodyType == const.BodyKinematic then
        other.linearVelocity = vec2(0, 0) - other.linearVelocity;
    end
end);
