local shuttle = scene:getObjects("shuttle1_mover")[1];
local door2Timer = nil;
local stonesTaken = 0;

local function intro1()
    scene.cutscene = true;
    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
    scene.camera:findCameraComponent().target = shuttle;
    local p = scene:getObjects("intro1_path")[1];
    shuttle.roamBehavior:reset();
    shuttle.roamBehavior.linearVelocity = 60.0;
    shuttle.roamBehavior.linearDamping = 4.0;
    shuttle.roamBehavior.dampDistance = 28.0;
    shuttle.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    shuttle.roamBehavior:start();

    local tmp = scene:getObjects("shuttle1")[1];
    local rc = tmp:findRenderQuadComponent();
    tmp:addComponent(QuadPulseComponent(rc, vec2(0, 0), 0.4, const.EaseInOutQuad, 0.98,
        0.4, const.EaseInOutQuad, 1.02));
end

local function rockFall(other, cp, boom, stone)
    scene.respawnPoint = scene:getObjects(cp)[1]:getTransform();
    local objs = scene:getObjects(boom);
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        scene:addObject(e);
    end
    objs = scene:getObjects(stone);
    for _, obj in pairs(objs) do
        obj.active = true;
    end
    addTimeoutOnce(0.5, function()
        local objs = scene:getObjects(stone);
        for _, obj in pairs(objs) do
            obj.visible = true;
        end
    end);
end

local function openJacketTurret(jacket, turret)
    openJacket(jacket);
    addTimeoutOnce(1.0, function()
        local hb = RenderHealthbarComponent(vec2(0.0, 2.0), 0, 4.0, 0.8, const.zOrderMarker + 1);
        hb.color = { 1.0, 1.0, 1.0, 0.6 };
        scene:getObjects(turret)[1]:addComponent(hb);
        scene:getObjects(turret)[1]:findTargetableComponent().autoTarget = true;
    end);
end

local function pressSpawn(joint, spawn, fn, ...)
    local e = factory:createTetrocrab3();
    addSpawnedEnemy(e, scene:getObjects(spawn)[1]);
    e:findPhysicsBodyComponent():setFilterGroupIndex(-800);
    local j = scene:getJoints(joint)[1];
    j.motorSpeed = -j.motorSpeed;
    local args = pack2(...);
    addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            audio:playSound("servo_stop.ogg");
            cancelTimeout(cookie);
            j.motorSpeed = -j.motorSpeed;
            addTimeout0(function(cookie, dt)
                if (j:getJointTranslation() <= j.lowerLimit) then
                    cancelTimeout(cookie);
                    fn(unpack2(args));
                end
            end);
        end
    end);
end

local function pressSpawn2(joint, spawn, barrel, snd, spawned, fn, ...)
    local e;
    if barrel then
        if math.random(1, 3) ~= 1 then
            e = factory:createBarrel1();
        else
            e = factory:createBarrel2();
        end
        e.pos = scene:getObjects(spawn)[1].pos;
        e.angle = math.random() * math.pi * 2.0;
        scene:addObject(e);
    else
        e = factory:createTetrocrab3();
        addSpawnedEnemy(e, scene:getObjects(spawn)[1]);
        table.insert(spawned, e);
    end
    e:findPhysicsBodyComponent():setFilterGroupIndex(-800);
    local j = scene:getJoints(joint)[1];
    j.motorSpeed = -j.motorSpeed;
    local args = pack2(...);
    addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            if snd then
                audio:playSound("servo_stop.ogg");
            end
            cancelTimeout(cookie);
            j.motorSpeed = -j.motorSpeed;
            addTimeout0(function(cookie, dt)
                if (j:getJointTranslation() <= j.lowerLimit) then
                    cancelTimeout(cookie);
                    fn(unpack2(args));
                end
            end);
        end
    end);
end

local function pressSpawnNum(joint, spawn, i)
    local func;
    func = function(j)
        if j == 0 then
            return;
        end
        pressSpawn(joint, spawn, func, j - 1);
    end
    func(i);
end

local function pressSpawnNum2(joint, spawn, i, snd, spawned, fn)
    local func;
    local barrel1 = math.random(1, i);
    func = function(j)
        if j == 0 then
            if fn ~= nil then
                fn();
            end
            return;
        end
        pressSpawn2(joint, spawn, (j == barrel1), snd, spawned, func, j - 1);
    end
    func(i);
end

local function openStoneSlow(joint, fn)
    local j = scene:getJoints(joint)[1];
    j.motorSpeed = -j.motorSpeed;
    return addTimeout0(function(cookie, dt)
        if j:getJointTranslation() >= j.upperLimit then
            cancelTimeout(cookie);
            if fn ~= nil then
                fn();
            end
        end
    end);
end

local function startGGCheck1()
    local rocks = {};
    for i = 1, 7, 1 do
        table.insert(rocks, scene:getObjects("rock"..i)[1]);
    end
    addTimeout0(function(cookie, dt, self)
        local ho = scene.player:findPlayerComponent().altWeapon.heldObject
        if ho == nil then
            return;
        end
        local have = 0;
        for k, obj in pairs(rocks) do
            have = 1;
            if ho == obj then
                rocks[k] = nil;
                scene:getJoints(obj.name.."_joint")[1]:remove();
                stonesTaken = stonesTaken + 1;
                break;
            end
        end
        if stonesTaken == 3 then
            stonesTaken = 100;
            addTimeoutOnce(1.0, function()
                pressSpawnNum("press15_joint", "spawn15", 3);
            end);
            addTimeoutOnce(1.5, function()
                pressSpawnNum("press16_joint", "spawn16", 3);
            end);
        end
        if not have then
            cancelTimeout(cookie);
        end
    end);
end

function showMoveFireInputHint()
    addTimeoutOnce(1.0, function()
        local obj = SceneObject();
        local c = nil;
        local c1 = nil;
        local c2 = nil;
        local c3 = nil;

        if input.usingGamepad then
            obj.pos = vec2(18, scene.gameHeight - 5);
            c = InputHintComponent(-2);
            c:addGamepadStick(true);
            c:setDescription(tr.str50);
            obj:addComponent(c);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(18, scene.gameHeight - 9);
            c1 = InputHintComponent(-2);
            c1:addGamepadStick(false);
            c1:setDescription(tr.str51);
            obj:addComponent(c1);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(18, scene.gameHeight - 13);
            c2 = InputHintComponent(-2);
            addGamepadBindingHint(c2, const.ActionGamepadIdPrimaryFire);
            c2:setDescription(tr.str20);
            obj:addComponent(c2);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(18, scene.gameHeight - 17);
            c3 = InputHintComponent(-2);
            addGamepadBindingHint(c3, const.ActionGamepadIdRun);
            c3:setDescription(tr.str56);
            obj:addComponent(c3);
        else
            obj.pos = vec2(14, scene.gameHeight - 5);
            c = InputHintComponent(-2);
            addBindingHint(c, const.ActionIdMoveUp);
            addBindingHint(c, const.ActionIdMoveDown);
            c:setDescription(tr.str18);
            obj:addComponent(c);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(14, scene.gameHeight - 9);
            c1 = InputHintComponent(-2);
            addBindingHint(c1, const.ActionIdMoveLeft);
            addBindingHint(c1, const.ActionIdMoveRight);
            c1:setDescription(tr.str19);
            obj:addComponent(c1);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(14, scene.gameHeight - 13);
            c2 = InputHintComponent(-2);
            addBindingHint(c2, const.ActionIdPrimaryFire);
            c2:setDescription(tr.str20);
            obj:addComponent(c2);
            scene:addObject(obj);

            obj = SceneObject();
            obj.pos = vec2(14, scene.gameHeight - 17);
            c3 = InputHintComponent(-2);
            addBindingHint(c3, const.ActionIdRun);
            c3:setDescription(tr.str56);
            obj:addComponent(c3);
        end

        scene:addObject(obj);

        addTimeoutOnce(6.0, function()
            c:setFade(1.0);
            c1:setFade(1.0);
            c2:setFade(1.0);
            c3:setFade(1.0);
        end);
    end);
end

local function showSecondaryFireInputHint()
    local obj = SceneObject();
    local c = nil;

    if input.usingGamepad then
        obj.pos = vec2(16, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addGamepadBindingHint(c, const.ActionGamepadIdSecondaryFire);
        c:setDescription(tr.str21);
        obj:addComponent(c);
    else
        obj.pos = vec2(16, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addBindingHint(c, const.ActionIdSecondaryFire);
        c:setDescription(tr.str21);
        obj:addComponent(c);
    end

    scene:addObject(obj);

    addTimeout0(function(cookie, dt)
        if not scene.inputPlayer.secondaryShowOff then
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                c:setFade(1.0);
            end);
        end
    end);
end

-- main

makeFlickerLamps("flicker_lamp");

local turrets = scene:getObjects("turret");
for _, obj in pairs(turrets) do
    local hb = RenderHealthbarComponent(vec2(0.0, 2.0), 0, 4.0, 0.8, const.zOrderMarker + 1);
    hb.color = { 1.0, 1.0, 1.0, 0.6 };
    obj:addComponent(hb);
end

if settings.developer == 0 then
    intro1();
    setSensorEnterListener("intro1_cp", true, function(other)
        if other.name ~= "shuttle1" then
            return;
        end
        addTimeoutOnce(1.5, function()
            scene.respawnPoint = scene:getObjects("intro1_cp")[1]:getTransform();
            scene.player:setTransform(scene:getObjects("intro1_cp")[1]:getTransform());
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
            local p = scene:getObjects("intro2_path")[1];
            scene.player.roamBehavior:reset();
            scene.player.roamBehavior.linearVelocity = 15.0;
            scene.player.roamBehavior.linearDamping = 5.0;
            scene.player.roamBehavior.dampDistance = 3.0;
            scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            scene.player.roamBehavior:start();
            scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 3.0);
            bg[4]:addComponent(FadeOutComponent(4.0));
            bg[4] = nil;
            addTimeoutOnce(1.0, function()
                scene.player.roamBehavior:reset();
                local p = scene:getObjects("intro3_path")[1];
                shuttle.roamBehavior:reset();
                shuttle.roamBehavior.linearVelocity = 60.0;
                shuttle.roamBehavior.linearDamping = 4.0;
                shuttle.roamBehavior.dampDistance = 28.0;
                shuttle.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                shuttle.roamBehavior:start();
            end);
            addTimeoutOnce(2.0, function()
                scene.cutscene = false;
                showMoveFireInputHint();
            end);
        end);
    end);
end

setSensorEnterListener("intro3_cp", true, rockFall, "intro3_cp", "boom3", "stone3");
setSensorEnterListener("intro4_cp", true, rockFall, "intro4_cp", "boom4", "stone4");
setSensorEnterListener("intro5_cp", true, rockFall, "intro5_cp", "boom5", "stone5");
setSensorEnterListener("intro6_cp", true, rockFall, "intro6_cp", "boom6", "stone6");
setSensorEnterListener("intro7_cp", true, rockFall, "intro7_cp", "boom7", "stone7");
setSensorEnterListener("intro8_cp", true, function(other)
    rockFall(other, "intro8_cp", "boom8", "stone8");
    openJacketTurret("jacket1", "turret1");
end);

setSensorEnterListener("turret2_cp", true, function(other)
    openJacketTurret("jacket2", "turret2");
end);

setSensorListener("hole1_cp", function(other, self)
    if self.objs[other.cookie] == nil then
        self.objs[other.cookie] = { count = 1, obj = other };
    else
        self.objs[other.cookie].count = self.objs[other.cookie].count + 1;
    end
end, function(other, self)
    self.objs[other.cookie].count = self.objs[other.cookie].count - 1;
    if self.objs[other.cookie].count == 0 then
        self.objs[other.cookie] = nil;
        local c = other:findPhysicsBodyComponent();
        if (c ~= nil) and (c.filterGroupIndex == -800) then
            c.filterGroupIndex = 0;
        end
    end
end, { objs = {} });

setSensorEnterListener("press1_cp", true, function(other)
    addTimeoutOnce(0.5, function()
        openJacketTurret("jacket3", "turret3");
    end);
    pressSpawnNum("press1_joint", "spawn1", 3);
    pressSpawnNum("press4_joint", "spawn4", 3);
end);

setSensorEnterListener("press2_cp", true, function(other)
    pressSpawnNum("press2_joint", "spawn2", 3);
    addTimeoutOnce(0.4, function()
        pressSpawnNum("press3_joint", "spawn3", 3);
    end);
    addTimeoutOnce(0.4, function()
        pressSpawnNum("press5_joint", "spawn5", 3);
    end);
    addTimeoutOnce(0.8, function()
        pressSpawnNum("press6_joint", "spawn6", 3);
    end);
end);

makeGear("press7", "press7_joint", "press8", "press8_joint", 1);
makeGear("press9", "press9_joint", "press10", "press10_joint", 1);
makeGear("press7", "press7_joint", "press9", "press9_joint", -1);

setSensorListener("press7_cp", function(other, self)
    local func;
    func = function()
        self.j.motorSpeed = -self.speed / 4.5;
        self.timer = addTimeout0(function(cookie, dt)
            if self.j:getJointTranslation() >= self.j.upperLimit then
                cancelTimeout(cookie);
                self.j.motorSpeed = self.speed;
                self.timer = addTimeout0(function(cookie, dt)
                    if self.j:getJointTranslation() <= self.j.lowerLimit then
                        scene.camera:findCameraComponent():roll(math.rad(3));
                        audio:playSound("stone_hit.ogg");
                        cancelTimeout(cookie);
                        func();
                    end
                end);
            end
        end);
    end;
    func();
end, function(other, self)
    cancelTimeout(self.timer);
    self.j.motorSpeed = self.speed;
end, { timer = nil, j = scene:getJoints("press7_joint")[1], speed = scene:getJoints("press7_joint")[1].motorSpeed });

setSensorEnterListener("door1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("door1_cp")[1]:getTransform();
    closeStone("door1_joint");
    scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 1.0);
    local spawned = {};
    local function wave2()
        addTimeout(0.25, function(cookie)
            local numAlive = 0;
            for _, obj in pairs(spawned) do
                if obj:alive() then
                    numAlive = numAlive + 1;
                end
            end
            if numAlive > (#spawned * (1 / 3)) then
                return;
            end
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
                addTimeoutOnce(1.0, function()
                    audio:playSound("stone_moving.ogg");
                end);
                openStoneSlow("door2_joint", function()
                    addTimeoutOnce(1.0, function()
                        audio:playSound("stone_moving.ogg");
                    end);
                    openStoneSlow("door3_joint", function()
                        addTimeoutOnce(1.0, function()
                            audio:playSound("stone_moving.ogg");
                        end);
                        door2Timer = openStoneSlow("door4_joint", function()
                            door2Timer = nil;
                        end);
                    end);
                end);
                spawned = {};
                for i = 11, 14, 1 do
                    addTimeoutOnce(1.0 + 0.5 * (i - 11), function()
                        pressSpawnNum2("press"..i.."_joint", "spawn"..i, 5, i == 11, spawned);
                    end);
                end
                for i = 1, 2, 1 do
                    local e = factory:createTetrobot2();
                    addSpawnedEnemy(e, scene:getObjects("tb_spawn"..i)[1]);
                    e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
                    table.insert(spawned, e);
                end
                addTimeoutOnce(2.0, function()
                    local e = factory:createTetrobot2();
                    addSpawnedEnemy(e, scene:getObjects("tb_spawn1")[1]);
                    e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
                    table.insert(spawned, e);
                end);
            end);
        end);
    end
    for i = 11, 14, 1 do
        addTimeoutOnce(1.0 + 0.5 * (i - 11), function()
            if i == 14 then
                pressSpawnNum2("press"..i.."_joint", "spawn"..i, 4, false, spawned, wave2);
            else
                pressSpawnNum2("press"..i.."_joint", "spawn"..i, 4, i == 11, spawned);
            end
        end);
    end
    for i = 1, 2, 1 do
        local e = factory:createTetrobot2();
        addSpawnedEnemy(e, scene:getObjects("tb_spawn"..i)[1]);
        e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
        table.insert(spawned, e);
    end
end);

setSensorEnterListener("door2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("door2_cp")[1]:getTransform();
    scene.camera:findCameraComponent():zoomTo(35, const.EaseInQuad, 1.0);
    if door2Timer ~= nil then
        cancelTimeout(door2Timer);
        door2Timer = nil;
    end
    local j = scene:getJoints("door4_joint")[1];
    j.motorSpeed = j.motorSpeed * 15.0;
    openStone("door4_joint");
    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal1")[1].pos);
end);

setSensorEnterListener("rock1_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        addTimeoutOnce(1.0, function()
            showLowerDialog(
            {
                {"player", tr.dialog12.str1},
                {"player", tr.dialog12.str2},
            }, function ()
                addTimeoutOnce(0.5, function()
                    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
                    scene.inputPlayer.secondaryShowOff = true;
                    startGGCheck1();
                    showSecondaryFireInputHint();
                end);
                scene.cutscene = false;
            end);
        end);
    end);
end);
