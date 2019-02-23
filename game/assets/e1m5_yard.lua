local platform0 = scene:getObjects("platform0")[1];
local platform0Gun = nil;
local platform0timer = nil;
local gun0helpers = {};
local gun0spawn = scene:getObjects("gun0_spawn");
local gun0spawnTimer = nil;

local function makeE1m4Airlock(name)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
end

local function makePlatform0()
    local dummy = platform0:findDummyComponent("missile");
    platform0Gun = WeaponMachineGunComponent(const.SceneObjectTypeEnemyMissile);
    platform0Gun.useFilter = true;
    platform0Gun.useFreeze = false;
    platform0Gun.pos = dummy.pos;
    platform0Gun.angle = dummy.angle;
    platform0Gun.damage = 4.0;
    platform0Gun.velocity = 100.0;
    platform0Gun.spreadAngle = math.rad(30.0);
    platform0Gun.turnInterval = 0.04;
    platform0Gun.turnDuration = 1.5;
    platform0Gun.loopDelay = 1.6;
    platform0:addComponent(platform0Gun);

    platform0:addComponent(CollisionSensorComponent());
    platform0.objs = {};
    platform0.lastAngle = platform0.angle;
    platform0.startAngle = platform0.angle;

    setSensorListener("platform0", function(other)
        if platform0.objs[other.cookie] == nil then
            if other.type == const.SceneObjectTypePlayer and platform0timer ~= nil then
                scene:getObjects("ga1")[1]:findGoalAreaComponent():removeAllGoals();
                scene:getObjects("gun0_cp")[1].active = false;
                cancelTimeout(platform0timer);
                platform0timer = nil;
                platform0.interceptBehavior:reset();
                local enforcer3 = scene:getObjects("enforcer3")[1];
                enforcer3:findEnforcerComponent().detour = false;
                enforcer3:findEnforcerComponent().autoTarget = true;
                addTimeoutOnce(3.0, function()
                    platform0.angularVelocity = 0.0;
                    platform0.bodyType = const.BodyKinematic;
                end);
                addTimeout0(function(cookie, dt)
                    if enforcer3:alive() then
                        return;
                    end
                    cancelTimeout(cookie);
                    local k = factory:createPowerupInventory(const.InventoryItemBlueKey);
                    k.pos = enforcer3.pos;
                    scene:addObject(k);
                end);
            end
            platform0.objs[other.cookie] = { count = 1, obj = other };
        else
            platform0.objs[other.cookie].count = platform0.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        if platform0.objs[other.cookie] == nil then
            return;
        end
        platform0.objs[other.cookie].count = platform0.objs[other.cookie].count - 1;
        if platform0.objs[other.cookie].count == 0 then
            platform0.objs[other.cookie] = nil;
        end
    end);
    platform0timer = addTimeout0(function(cookie, dt)
        for _, v in pairs(platform0.objs) do
            local a = platform0.angle - platform0.lastAngle;
            v.obj:setPosSmoothed(platform0.pos + (v.obj.pos - platform0.pos):rotated(a));
            v.obj:setAngleSmoothed(v.obj.angle + a);
        end
        platform0.lastAngle = platform0.angle;
    end);

    platform0.active = false;
    platform0.active = true;

    platform0.interceptBehavior:reset();
    platform0.interceptBehavior.parentLinearVelocity = 80.0;
    platform0.interceptBehavior.angularVelocity = math.rad(90);
    platform0.interceptBehavior:start();

    scene:getObjects("gun0_cp")[1].active = true;

    local function gun0spawnFunc(cookie)
        for k, obj in pairs(gun0helpers) do
            if obj:dead() then
                gun0helpers[k] = nil;
            end
        end
        if table.size(gun0helpers) >= 3 then
            return;
        end
        local numEnforcers = 0;
        for _, obj in pairs(gun0helpers) do
            if obj:findEnforcerComponent() ~= nil then
                numEnforcers = numEnforcers + 1;
            end
        end
        local sp = {};
        local cc = scene.camera:findCameraComponent();
        for _, obj in pairs(gun0spawn) do
            if not cc:rectVisible(obj.pos, 5.0, 5.0) then
                table.insert(sp, {(obj.pos - scene.player.pos):lenSq(), obj});
            end
        end
        table.sort(sp, function(a, b) return a[1] < b[1] end);
        local n = math.min(3 - table.size(gun0helpers), #sp);
        for i = 1, n, 1 do
            local e;
            if (numEnforcers <= 0) then
                e = factory:createEnforcer1();
                numEnforcers = numEnforcers + 1;
            else
                e = factory:createScorp2();
            end
            e:setTransform(sp[i][2]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
            table.insert(gun0helpers, e);
        end
    end;

    setSensorListener("gun0_cp", function(other)
        scene.respawnPoint = scene:getObjects("yard_cp")[1]:getTransform();
        platform0.interceptBehavior.target = other;
        platform0Gun:reload();
        platform0Gun:trigger(true);
        scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
        gun0spawnTimer = addTimeout(5.0, gun0spawnFunc);
    end, function (other, args)
        platform0.interceptBehavior.target = nil;
        platform0Gun:trigger(false);
        if other:alive() or (scene.respawnPoint ~= scene:getObjects("yard_cp")[1]:getTransform()) then
            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
        end
        cancelTimeout(gun0spawnTimer);
    end);
end

-- main

scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal2")[1].pos);

makeDoor("door2", true);
makeE1m4Airlock("door3");

setSensorEnterListener("yard_cp", true, function(other)
    closeDoor("door2", true);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 2.0);
        scene.camera:findCameraComponent():follow(scene:getObjects("enforcer3")[1], const.EaseOutQuad, 2.0);
        addTimeoutOnce(2.5, function()
            showLowerDialog(
            {
                {"enemy", tr.str7, "prison1/portrait_guard.png", tr.dialog58.str1}
            }, function ()
                local objs = scene:getObjects("enforcer4");
                for _, obj in pairs(objs) do
                    obj:findTargetableComponent().autoTarget = true;
                end
                gun0helpers = objs;
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                addTimeoutOnce(1.5, function()
                    scene.cutscene = false;
                    makePlatform0();
                    scene:getObjects("ga1")[1].active = true;
                end);
            end);
        end);
    end);
end);
