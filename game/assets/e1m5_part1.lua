local gun1 = scene:getInstances("gun1")[1];
local gun2 = scene:getInstances("gun2")[1];
local gun3 = scene:getInstances("gun3")[1];
local gun4 = scene:getInstances("gun4")[1];
local gun5 = scene:getInstances("gun5")[1];
local gun6 = scene:getInstances("gun6")[1];
local gun7 = scene:getInstances("gun7")[1];
local gun8 = scene:getInstances("gun8")[1];
local gun9 = scene:getObjects("gun9")[1];
local gun9helpers = {};
local gun9spawn = scene:getObjects("gun9_spawn")[1];
local gun9spawnTimer = nil;
local barrel1 = scene:getObjects("barrel1")[1];
local barrel2 = scene:getObjects("barrel2")[1];
local gun10 = scene:getInstances("gun10")[1];
local gun11 = scene:getInstances("gun11")[1];
local gun12 = scene:getInstances("gun12")[1];
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();

local function makeGun(inst, damage, spreadAngle, turnInterval, turnDuration, loopDelay)
    local obj = findObject(inst.objects, "gun1_root");
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponMachineGunComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.useFilter = true;
    inst.weapon.useFreeze = false;
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.damage = damage;
    inst.weapon.velocity = 100.0;
    inst.weapon.spreadAngle = spreadAngle;
    inst.weapon.turnInterval = turnInterval;
    inst.weapon.turnDuration = turnDuration;
    inst.weapon.loopDelay = loopDelay;
    obj:addComponent(inst.weapon);
end

local function makeGun2(inst, explosionImpulse, explosionDamage, interval)
    local obj = findObject(inst.objects, "gun2_root");
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponRLauncherComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.useFilter = true;
    inst.weapon.useFreeze = false;
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.explosionImpulse = explosionImpulse;
    inst.weapon.explosionDamage = explosionDamage;
    inst.weapon.velocity = 30.0;
    inst.weapon.interval = interval;
    obj:addComponent(inst.weapon);
end

local function makeGun123(inst)
    makeGun(inst, 4.0, math.rad(4.0), 0, 0, 0.05);
end

local function makeGun9()
    local dummy = gun9:findDummyComponent("missile");
    gun9.weapon = WeaponMachineGunComponent(const.SceneObjectTypeEnemyMissile);
    gun9.weapon.useFilter = true;
    gun9.weapon.useFreeze = false;
    gun9.weapon.pos = dummy.pos;
    gun9.weapon.angle = dummy.angle;
    gun9.weapon.damage = 4.0;
    gun9.weapon.velocity = 100.0;
    gun9.weapon.spreadAngle = math.rad(20.0);
    gun9.weapon.turnInterval = 0.1;
    gun9.weapon.turnDuration = 1.5;
    gun9.weapon.loopDelay = 3.0;
    gun9:addComponent(gun9.weapon);

    gun9:addComponent(CollisionSensorComponent());
    gun9.objs = {};
    gun9.lastAngle = gun9.angle;
    gun9.startAngle = gun9.angle;

    setSensorListener("gun9", function(other)
        if gun9.objs[other.cookie] == nil then
            gun9.objs[other.cookie] = { count = 1, obj = other };
        else
            gun9.objs[other.cookie].count = gun9.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        if gun9.objs[other.cookie] == nil then
            return;
        end
        gun9.objs[other.cookie].count = gun9.objs[other.cookie].count - 1;
        if gun9.objs[other.cookie].count == 0 then
            gun9.objs[other.cookie] = nil;
        end
    end);

    gun9.timer = addTimeout0(function(cookie, dt)
        for _, v in pairs(gun9.objs) do
            local a = gun9.angle - gun9.lastAngle;
            v.obj:setPosSmoothed(gun9.pos + (v.obj.pos - gun9.pos):rotated(a));
            v.obj:setAngleSmoothed(v.obj.angle + a);
        end
        gun9.lastAngle = gun9.angle;
    end);

    gun9.active = false;
    gun9.active = true;

    gun9.seekBehavior:reset();
    gun9.seekBehavior.angularVelocity = math.rad(30);
    gun9.seekBehavior.loop = true;
    gun9.seekBehavior:start();

    local function gun9spawnFunc(cookie)
        for k, obj in pairs(gun9helpers) do
            if obj:dead() then
                gun9helpers[k] = nil;
            end
        end
        if table.size(gun9helpers) >= 1 then
            return;
        end
        local e = factory:createScorp2();
        e:setTransform(gun9spawn:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
        table.insert(gun9helpers, e);
    end;

    setSensorListener("gun9_cp", function(other)
        gun9.seekBehavior.target = other;
        gun9.weapon:reload();
        gun9.weapon:trigger(true);
        gun9spawnTimer = addTimeout(1.0, gun9spawnFunc);
    end, function (other)
        gun9.seekBehavior.target = nil;
        gun9.angularVelocity = 0;
        gun9.weapon:trigger(false);
        cancelTimeout(gun9spawnTimer);
    end);
end

-- main

setSensorEnterListener("guns1_intro_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        local dummy = factory:createDummy();
        dummy.pos = scene.player.pos;
        scene:addObject(dummy);
        scene.camera:findCameraComponent().target = dummy;
        local p = scene:getObjects("guns1_path")[1];
        dummy.roamBehavior:reset();
        dummy.roamBehavior.linearVelocity = 80.0;
        dummy.roamBehavior.linearDamping = 4.0;
        dummy.roamBehavior.dampDistance = 5.0;
        dummy.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        dummy.roamBehavior:start();
        addTimeoutOnce(7.5, function()
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
            dummy:removeFromParent();
            addTimeoutOnce(1.5, function()
                scene.cutscene = false;
            end);
        end);
    end);
end);

makeDoor("door4", false);
makeDoorTrigger("door4_cp", "door4");

makeDoor("door5", false);
makeDoorTrigger("door5_cp", "door5");

makeGun123(gun1);
makeGun123(gun2);
makeGun123(gun3);

local function gun1Func(cookie, dt, self)
    self.t = self.t - dt;
    if self.t >= 0 then
        return;
    end
    if self.pause then
        self.guns[self.i].weapon:trigger(true);
        self.t = 0.5;
        self.pause = false;
    else
        self.guns[self.i].weapon:trigger(false);
        self.i = self.i + 1;
        if self.i > 3 then
            self.i = 1;
        end
        self.t = 0.5;
        self.pause = true;
    end
end

setSensorListener("gun1_cp", function(other, self)
    gun1.weapon:reload();
    gun2.weapon:reload();
    gun3.weapon:reload();
    self.timer = addTimeout0(gun1Func, { t = 0.5, i = 1, guns = {gun1, gun2, gun3}, pause = true });
end, function (other, self)
    gun1.weapon:trigger(false);
    gun2.weapon:trigger(false);
    gun3.weapon:trigger(false);
    cancelTimeout(self.timer);
    self.timer = nil;
end, { timer = nil });

setSensorEnterListener("gun1_end_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("gun1_end_cp")[1]:getTransform();
    scene:getObjects("gun1_cp")[1].active = false;
    local objs = scene:getObjects("enforcer5");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

makeGun(gun4, 5.0, math.rad(40.0), 0.02, 2.0, 1.0);
makeGun(gun5, 5.0, math.rad(15.0), 0.07, 1.5, 1.2);
makeGun(gun6, 5.0, math.rad(15.0), 0.08, 1.7, 1.5);
makeGun(gun7, 5.0, math.rad(15.0), 0.09, 1.6, 1.3);
makeGun(gun8, 5.0, math.rad(15.0), 0.07, 1.5, 1.4);

setSensorListener("gun2_cp", function(other, self)
    gun4.weapon:reload();
    gun5.weapon:reload();
    gun6.weapon:reload();
    gun7.weapon:reload();
    gun8.weapon:reload();
    gun4.weapon:trigger(true);
    gun5.weapon:trigger(true);
    gun6.weapon:trigger(true);
    gun7.weapon:trigger(true);
    gun8.weapon:trigger(true);
end, function (other, self)
    gun4.weapon:trigger(false);
    gun5.weapon:trigger(false);
    gun6.weapon:trigger(false);
    gun7.weapon:trigger(false);
    gun8.weapon:trigger(false);
end, { timer = nil });

makeGun9();

makeDoor("door6", true);

setSensorEnterListener("gun9_pre_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("gun9_pre_cp")[1]:getTransform();
    scene:getObjects("gun9_cp")[1].active = true;
    closeDoor("door6", true);

    local function openDoor7()
        openDoor("door6", false);
        openDoor("door7", true);
    end

    addTimeout0(function(cookie, dt)
        if barrel1:alive() then
            return;
        end
        cancelTimeout(cookie);
        local objs = scene:getObjects("fire2");
        for _, obj in pairs(objs) do
            obj.visible = true;
        end
        if barrel2:dead() then
            addTimeoutOnce(1.0, openDoor7);
        end
    end);
    addTimeout0(function(cookie, dt)
        if barrel2:alive() then
            return;
        end
        cancelTimeout(cookie);
        local objs = scene:getObjects("fire3");
        for _, obj in pairs(objs) do
            obj.visible = true;
        end
        if barrel1:dead() then
            addTimeoutOnce(1.0, openDoor7);
        end
    end);
end);

makeDoor("door7", false);

setSensorEnterListener("gun10_pre_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("gun10_pre_cp")[1]:getTransform();
end);

makeGun2(gun10, 100.0, 50.0, 0.0);
makeGun2(gun11, 100.0, 50.0, 0.0);
makeGun2(gun12, 100.0, 50.0, 0.0);

local function gun10Func(cookie, dt, self)
    self.t = self.t - dt;
    if self.t >= 0 then
        return;
    end
    self.guns[self.i].weapon:triggerOnce();
    self.t = 3.0;
    self.i = math.random(1, 3);
end

setSensorListener("gun10_cp", function(other, self)
    self.timer = addTimeout0(gun10Func, { t = 1.0, i = math.random(1, 3), guns = {gun10, gun11, gun12} });
end, function (other, self)
    cancelTimeout(self.timer);
    self.timer = nil;
end, { timer = nil });

setSensorEnterListener("gun10_end_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("gun10_end_cp")[1]:getTransform();
    scene:getObjects("gun10_cp")[1].active = false;
    local objs = scene:getObjects("enforcer12");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

control2.listener = createSensorEnterListener(true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
            scene.camera:findCameraComponent().target = scene:getObjects("enforcer3")[1];
            scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                addTimeoutOnce(1.0, function()
                    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal2")[1].pos);
                    local laser6 = scene:getObjects("laser6");
                    if #laser6 == 0 then
                        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("platform0")[1].pos);
                    end
                    audio:playSound("alarm_off.ogg");
                    local laser5 = scene:getObjects("laser5");
                    for _, obj in pairs(laser5) do
                        obj:addComponent(FadeOutComponent(1.0));
                    end
                    control2.active = false;
                    control2.visible = false;
                    local efs = scene:getObjects("enforcer6");
                    for _, obj in pairs(efs) do
                        obj:removeFromParent();
                    end
                    efs = scene:getObjects("enforcer7");
                    for _, obj in pairs(efs) do
                        obj:removeFromParent();
                    end
                    scene:getObjects("gun9_cp")[1].active = false;
                    scene:getObjects("gun2_cp")[1].active = false;

                    local objs = scene:getObjects("gun10_spawn");
                    for _, obj in pairs(objs) do
                        local e = factory:createEnforcer1();
                        e:setTransform(obj:getTransform());
                        local ec = e:findTargetableComponent();
                        ec.autoTarget = true;
                        scene:addObject(e);
                    end
                    addTimeoutOnce(2.0, function()
                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                            scene.camera:findCameraComponent().target = scene.player;
                            scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);
                            scene.cutscene = false;
                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);
