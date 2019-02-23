local platform0 = scene:getObjects("platform0")[1];
local platform1 = scene:getObjects("platform1")[1];
local platform2 = scene:getObjects("platform2")[1];
local gun1 = scene:getInstances("gun1")[1];
local gun2 = scene:getInstances("gun2")[1];
local gun3 = scene:getInstances("gun3")[1];
local gun4 = scene:getInstances("gun4")[1];
local gun5 = scene:getInstances("gun5")[1];
local gun6 = scene:getInstances("gun6")[1];
local conveyor0Spawn = scene:getObjects("conveyor0_spawn");
local conveyor1Spawn = scene:getObjects("conveyor1_spawn");
local conveyor1LeverSpawn = scene:getObjects("conveyor1_spawn_lever")[1];
local conveyor1BarrelSpawn = scene:getObjects("conveyor1_spawn_barrel")[1];
local stone1 = {};
local lever1 = {};
local barrel1 = {};
local tentacle3 = scene:getObjects("tentacle3")[1];
local gun7 = scene:getInstances("gun7")[1];
local gun8 = scene:getInstances("gun8")[1];
local barrelSpawn1 = scene:getObjects("barrel_spawn1");
local vessel0 = scene:getObjects("vessel0")[1];
local lever2ph = scene:getObjects("lever2_ph")[1]:findPlaceholderComponent();
local stone1Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("stone1_light")[1];
local leverInPlace = false;
local stone1InPlace = false;
local stopConveyorSpawn = false;

function openVessel0()
    audio:playSound("drain1.ogg");
    vessel0:addComponent(FadeOutComponent(4.0));
    addTimeoutOnce(4.5, function()
        openDoor("door2", true);
    end);
end

function stopPlatforms0()
    scene:getJoints("platform0_joint")[1].motorEnabled = false;
    scene:getJoints("platform1_joint")[1].motorEnabled = false;
    scene:getJoints("platform2_joint")[1].motorEnabled = false;
    stopConveyorSpawn = true;
end

local function makeTentacleSwing1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.5 + math.random() * 1.0;
        local f = 7000 + math.random() * 3000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentaclePulse2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local timeF = 0.2 + math.random() * 0.2;
        local phaseF = 3.0 + math.random() * 2.0;
        local a1F = 0.1 + math.random() * 0.2;
        local a2F = 0.1 + math.random() * 0.2;
        obj:addComponent(TentaclePulseComponent(15, timeF, phaseF, a1F, a2F));
    end
end

local function makeTentacleSwing2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.5 + math.random() * 1.0;
        local f = 1000 + math.random() * 1000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing3(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.5 + math.random() * 1.0;
        local f = 8000 + math.random() * 4000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing4(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.8 + math.random() * 1.0;
        local f = 20000 + math.random() * 8000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeGun1(inst, damage, length, impulse)
    local obj = findObject(inst.objects, "gun1_root");
    obj.freezable = true;
    obj.freezePhysics = false;
    obj.freezeRadius = length + 10.0;
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponLGunComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.damage = damage;
    inst.weapon.length = length;
    inst.weapon.impulse = impulse;
    inst.weapon.haveSound = false;
    obj:addComponent(inst.weapon);
    inst.weapon:trigger(true);
end

local function makeGun2(inst, damage, length, impulse, velocity)
    local obj = findObject(inst.objects, "gun2_root");
    obj.angularVelocity = math.rad(velocity);
    obj.freezable = true;
    obj.freezePhysics = false;
    obj.freezeRadius = length + 15.0;
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponLGunComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.damage = damage;
    inst.weapon.length = length;
    inst.weapon.impulse = impulse;
    inst.weapon.haveSound = false;
    obj:addComponent(inst.weapon);
    inst.weapon:trigger(true);
end

-- main

scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent().timeout = 45.0;
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHintTrigger(scene:getObjects("ha1_target")[1].pos, 1.0);
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha1_target")[1].pos);

if settings.skill <= const.SkillNormal then
    scene:getObjects("ha1")[1].active = true;
end

addTimeout(0.25, function(cookie, self)
    if math.abs(platform1.angularVelocity) <= math.rad(1) then
        self.t = self.t + 0.25;
        if self.t > 1.0 then
            cancelTimeout(cookie);
            scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():removeAllHints();
        end
    end
end, { t = 0 });

makeRipples1("terrain0");
makeRipples1("sea_garbage");
makeToxicFlow("toxic_flow");
makeToxicFlow("pipe1");
makeSeaCrate("sea_wood_crate");

local toxic1 = scene:getObjects("toxic1");
for _, obj in pairs(toxic1) do
    makeToxic1(obj, 5.0);
end

makeDoor("door1", false);
makeDoorTrigger("door1_cp", "door1");

makeDoor("door2", false);

makeDoor("door3", false);
makeDoorTrigger("door3_cp", "door3");

makeDoor("door4", false);
makeDoorTrigger("door4_cp", "door4");

makeDoor("door5", false);

makeDoor("door6", false);
makeDoorTrigger("door6_cp", "door6");

makeTentacleFlesh("tentacle1");
makeTentacleSwing1("tentacle1");
makeTentacleFlesh("tentacle2");
makeTentacleSwing1("tentacle2");
makeTentacleFlesh("tentacle3");
makeTentacleFlesh("tentacle4");
makeTentacleSwing2("tentacle4");
makeTentaclePulse2("tentacle4");
makeTentacleFlesh("tentacle5");
makeTentacleSwing3("tentacle5");
makeTentacleFlesh("tentacle6");
makeTentacleSwing3("tentacle6");
makeTentacleFlesh("tentacle7");
makeTentacleSwing4("tentacle7");
makeTentaclePulse2("tentacle7");

makePlatform(platform0);
makePlatform(platform1);
makePlatform(platform2);

makeGear("platform0", "platform0_joint", "platform1", "platform1_joint", -1);

makeGun1(gun1, 20.0, 40.0, 100.0);
makeGun1(gun2, 20.0, 40.0, 100.0);
makeGun1(gun3, 20.0, 40.0, 100.0);
makeGun1(gun4, 20.0, 40.0, 100.0);
makeGun1(gun5, 20.0, 40.0, 100.0);
makeGun1(gun6, 20.0, 75.0, 100.0);

makeConveyor("conveyor0", 9.0, function(obj)
    if obj.name == "robopart2" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(2.0, function(cookie)
    if stopConveyorSpawn then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor0Spawn[math.random(1, #conveyor0Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));

    for _, obj in pairs(stone1) do
        if obj:scene() == nil then
            stone1[obj.cookie] = nil;
        end
    end

    if (math.random(1, 3) == 1) and (table.size(stone1) < 3) then
        local inst = scene:instanciate("e1m7_stone1.json", xf);
        local obj = findObject(inst.objects, "stone1");
        stone1[obj.cookie] = obj;
    else
        scene:instanciate("e1m4_robopart"..math.random(9, 14)..".json", xf);
    end
end);

setSensorEnterListener("stone1_die_cp", false, function(other)
    other:removeFromParent();
end);

setSensorEnterListener("stone1_gotcha_cp", false, function(other)
    if other.name ~= "stone1" then
        return;
    end

    scene.player:findCollisionDamageComponent().enabled = false;

    scene.respawnPoint = scene:getObjects("stone1_gotcha_cp")[1]:getTransform();

    tentacle3:addComponent(TentacleSwingComponent(1.0,
        const.EaseInOutQuad, 25000.0,
        1.0,
        const.EaseInOutQuad, -25000.0));

    local bones = tentacle3:findRenderTentacleComponent().objects;

    scene:getObjects("stone1_gotcha_cp")[1].active = false;
    local tentacles = scene:getObjects("tentacle1");
    tentacles = table.copy(tentacles, scene:getObjects("tentacle2"));
    for _, obj in pairs(tentacles) do
        obj.linearVelocity = vec2(0, 24 / 1.6);
    end
    tentacle3.linearVelocity = vec2(0, -44 / 1.6);
    audio:playSound("squid_alert1.ogg");
    scene.camera:findCameraComponent():tremorStart(0.3);
    local stream = audio:createStream("queen_shake.ogg");
    stream:play();
    addTimeoutOnce(1.2 * 1.6, function()
        for _, obj in pairs(tentacles) do
            obj.linearVelocity = vec2(0, 0);
        end
        tentacle3.linearVelocity = vec2(0, 0);
        scene.camera:findCameraComponent():tremor(false);
        stream:stop();
    end);
    addTimeoutOnce(2.8, function()
        bones[2].linearVelocity = vec2(0);
        bones[2].angularVelocity = 0;
        bones[2].bodyType = const.BodyKinematic;

        bones[3].linearVelocity = vec2(0);
        bones[3].angularVelocity = 0;
        bones[3].bodyType = const.BodyKinematic;

        local c1 = TentacleSwingComponent();
        c1.bone = 3;
        tentacle3:addComponent(c1);

        local c2 = TentacleSwingComponent();
        c2.bone = 4;
        tentacle3:addComponent(c2);

        local c3 = TentacleSwingComponent();
        c3.bone = 5;
        tentacle3:addComponent(c3);

        for _, bone in pairs(bones) do
            local cc = CollisionCancelComponent();
            cc:setFilter({const.SceneObjectTypePlayer,
                const.SceneObjectTypeRock,
                const.SceneObjectTypeGizmo,
                const.SceneObjectTypeEnemy,
                const.SceneObjectTypeEnemyBuilding,
                const.SceneObjectTypeAlly});
            cc.damage = 5.0;
            bone:addComponent(cc);
        end

        local function swingFunc(val, val2, cnt)
            if cnt == 0 then
                c1:removeFromParent();
                c2:removeFromParent();
                c3:removeFromParent();

                addTimeoutOnce(1.0, function()
                    audio:playSound("squid_alert2.ogg");
                end);

                addTimeoutOnce(3.5, function()
                    scene.player:findCollisionDamageComponent().enabled = true;

                    bones[2].bodyType = const.BodyDynamic;
                    bones[3].bodyType = const.BodyDynamic;

                    for _, obj in pairs(tentacles) do
                        obj.linearVelocity = vec2(0, -24 / 1.6);
                    end
                    tentacle3.linearVelocity = vec2(0, 44 / 1.6);
                    scene.camera:findCameraComponent():tremorStart(0.3);
                    local stream = audio:createStream("queen_shake.ogg");
                    stream:play();
                    addTimeoutOnce(1.2 * 1.6, function()
                        for _, obj in pairs(tentacles) do
                            obj.linearVelocity = vec2(0, 0);
                        end
                        removeTentacle(tentacle3);
                        scene.camera:findCameraComponent():tremor(false);
                        stream:stop();
                    end);
                end);

                return;
            end
            c1:setSingle(val, val2, 0.5, const.EaseInQuad);
            c2:setSingle(val, val2, 0.5, const.EaseInQuad);
            c3:setSingle(val, val2, 0.5, const.EaseInQuad);
            addTimeoutOnce(0.5, function()
                scene.camera:findCameraComponent():roll(math.rad(3));
                audio:playSound("grate_hit.ogg");
            end);
            addTimeoutOnce(0.5 * 1.2, swingFunc, -val, -val2, cnt - 1);
        end;
        audio:playSound("squid_alert3.ogg");
        swingFunc(35000.0, 65000.0, 5);
    end);
end);

makeConveyor("conveyor1", 6.5, function(obj)
    if obj.name == "robopart2" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(1.25, function(cookie, self)
    if stopConveyorSpawn then
        cancelTimeout(cookie);
        return;
    end
    for _, obj in pairs(lever1) do
        if obj:scene() == nil then
            lever1[obj.cookie] = nil;
        end
    end
    for _, obj in pairs(barrel1) do
        if obj:scene() == nil then
            barrel1[obj.cookie] = nil;
        end
    end
    local r = math.random(1, 7);
    if (r >= 1) and (r <= 3) and (table.size(lever1) < 7) and self.wasLever then
        r = math.random(4, 7);
    end
    if (r >= 4) and (r <= 6) and (table.size(barrel1) < 7) and self.wasBarrel then
        r = math.random(0, 3);
    end
    self.wasLever = false;
    self.wasBarrel = false;
    if (r >= 1) and (r <= 3) and (table.size(lever1) < 7) then
        local xf = conveyor1LeverSpawn:getTransform();
        xf.q = rot(math.random(0, math.pi * 2.0));

        local inst = scene:instanciate("e1m7_lever1.json", xf);
        local obj = findObject(inst.objects, "lever1");
        lever1[obj.cookie] = obj;

        self.wasLever = true;
    elseif (r >= 4) and (r <= 6) and (table.size(barrel1) < 7) then
        local xf = conveyor1BarrelSpawn:getTransform();
        xf.q = rot(math.random(0, math.pi * 2.0));

        local obj = factory:createBarrel1();
        obj:setTransform(xf);
        scene:addObject(obj);
        barrel1[obj.cookie] = obj;

        self.wasBarrel = true;
    else
        local spawn = conveyor1Spawn[math.random(1, #conveyor1Spawn)];
        local xf = spawn:getTransform();
        xf.q = rot(math.random(0, math.pi * 2.0));

        scene:instanciate("e1m4_robopart"..math.random(9, 14)..".json", xf);
    end
end, { wasLever = false, wasBarrel = false });

setSensorEnterListener("lever1_die_cp", false, function(other)
    other:removeFromParent();
end);

makeGun2(gun7, 30.0, 35.0, 100.0, 30.0);
makeGun2(gun8, 30.0, 30.0, 100.0, -30.0);

addTimeout(1.0, function(cookie)
    if stopConveyorSpawn then
        cancelTimeout(cookie);
        return;
    end
    local cc = scene.camera:findCameraComponent();
    for _, spawn in pairs(barrelSpawn1) do
        if (spawn.myObj == nil) or (spawn.myObj:scene() == nil) then
            if not cc:rectVisible(spawn.pos, 50.0, 50.0) then
                spawn.myObj = factory:createBarrel3();
                local xf = spawn:getTransform();
                xf.q = rot(math.random(0, math.pi * 2.0));
                spawn.myObj:setTransform(xf);
                scene:addObject(spawn.myObj);
            end
        end
    end
end);

makeToxic2(vessel0, vessel0:findRenderTerrainComponents()[1]);

local function checkVessel0()
    if (not leverInPlace) or (not stone1InPlace) then
        return;
    end
    addTimeoutOnce(1.0, function()
        openVessel0();
    end);
end

makeLever("lever2", true, nil, function()
    audio:playSound("lever_pull.ogg");
    disableLever("lever2");
    leverInPlace = true;
    checkVessel0();
end);
hideLeverHandle("lever2");

lever2ph.listener = createSensorEnterListener(false, function(other)
    if other.name ~= "lever1" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    addTimeoutOnce(0.0 , function()
        other:removeFromParent();
    end);
    showLeverHandle("lever2");
    lever2ph.active = false;
    lever2ph.visible = false;
    audio:playSound("lever_pull.ogg");
end);

setSensorEnterListener("stone1_cp", false, function(other)
    if other.name ~= "stone1" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    other.gravityGunAware = false;
    scene:getObjects("stone1_cp")[1].active = false;
    addTimeoutOnce(0.5, function()
        audio:playSound("crystal_activate.ogg");
        local tweening = SequentialTweening(true);
        tweening:addTweening(SingleTweening(0.6, const.EaseOutQuad, stone1Light.color[4], 1.0, false));
        tweening:addTweening(SingleTweening(0.6, const.EaseInQuad, 1.0, stone1Light.color[4], false));
        addTimeout0(function(cookie, dt, self)
            local c = stone1Light.color;
            c[4] = tweening:getValue(self.t);
            stone1Light.color = c;
            self.t = self.t + dt;
        end, { t = 0 });
        stone1InPlace = true;
        checkVessel0();
    end);
end);

if settings.developer == 0 then
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog81.str1},
    }, function ()
        scene.cutscene = false;
    end);
end

for i = 1, 1000, 1 do
    local name = "fspawn"..i.."_cp";
    local obj = scene:getObjects(name)[1];
    if obj then
        setSensorEnterListener(name, true, function(other)
            fixedSpawn("fspawn"..i);
        end);
    end
end

setSensorEnterListener("fspawn3_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("fspawn3_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("fspawn3");
    end
end, { first = true });

setSensorEnterListener("tentacle_encounter_cp", true, function(other)
    showUpperMsg(4.0, {"player", tr.dialog82.str1});
end);

setSensorEnterListener("conveyor0_cp", true, function(other)
    scene:getObjects("fspawn9_cp")[1].active = true;
end);

setSensorEnterListener("fspawn10_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("fspawn10_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("fspawn10");
    end
end, { first = true });

setSensorEnterListener("lever_spawn1_cp", false, function(other, self)
    if self.lastObj ~= other then
        fixedSpawn("lever_spawn1");
        self.lastObj = other;
    end
end, { lastObj = nil });

makeKeyDoor("red_door", "red_key_ph", function()
    openDoor("door5");
end);
