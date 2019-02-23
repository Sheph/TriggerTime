local boss = scene:getObjects("skull1")[1];
local bossAmplified = false;
local bp = scene:getObjects("boss_path");
local bossFastSpeed = settings.bossSkull.fastWalkSpeed;
local bossSlowSpeed = settings.bossSkull.slowWalkSpeed;
local barrel = nil;
local barrelTurn = math.random(0, 1);

local function bossDefeated()
    scene.camera:findCameraComponent():zoomTo(35, const.EaseInQuad, 2.0);
    moveDoor(true, "door14");
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog19.str1},
        {"player", tr.dialog19.str2},
    }, function ()
        scene.cutscene = false;
    end);
end

local function bossCheck(cookie, dt)
    if boss:dead() then
        boss.roamBehavior.linearDamping = 2.0;
        boss.roamBehavior:damp();
        cancelTimeout(cookie);
        addTimeoutOnce(2.0, startAmbientMusic, true);
        addTimeoutOnce(5.0, bossDefeated);
        return;
    end
    if not bossAmplified and (boss:lifePercent() <= 0.3) then
        boss:findBossSkullComponent():setAmplified();
        bossFastSpeed = bossFastSpeed * 1.5;
        if bossFastSpeed > 35 then
            bossFastSpeed = 35;
        end
        bossSlowSpeed = bossSlowSpeed * 1.5;
        bossAmplified = true;
    end
    if not boss.roamBehavior.finished then
        if (scene.player.pos - boss.pos):len() < 20.0 then
            boss.roamBehavior.linearVelocity = bossSlowSpeed;
        else
            boss.roamBehavior.linearVelocity = bossFastSpeed;
        end
        return;
    end
    local p = bp[math.random(1, #bp)];
    boss.roamBehavior:reset();
    boss.roamBehavior.linearVelocity = bossFastSpeed;
    boss.roamBehavior.linearDamping = 0.0;
    boss.roamBehavior.dampDistance = 0.0;
    boss.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    boss.roamBehavior:start();
end

local function barrelSpawnCheck(cookie, self)
    if self.turn ~= barrelTurn then
        return;
    end
    if boss:dead() then
        cancelTimeout(cookie);
        if self.launchTimer ~= nil then
            cancelTimeout(self.launchTimer);
        end
        return;
    end
    if (barrel == nil) or barrel:dead() then
        barrel = factory:createBarrel1();
        self.launchTimer = addTimeoutOnce(3.0, function()
            self.launchTimer = nil;
            barrelTurn = 1 - barrelTurn;
            barrel.pos = self.spawn.pos;
            barrel.angle = math.random() * math.pi * 2.0;
            scene:addObject(barrel);
            barrel:findPhysicsBodyComponent():setFilterGroupIndex(-800);
            self.j.motorSpeed = -self.j.motorSpeed;
            addTimeout0(function(cookie, dt)
                if (self.j:getJointTranslation() >= self.j.upperLimit) then
                    audio:playSound("servo_stop.ogg");
                    cancelTimeout(cookie);
                    self.j.motorSpeed = -self.j.motorSpeed;
                    addTimeout0(function(cookie, dt)
                        if (self.j:getJointTranslation() <= self.j.lowerLimit) then
                            cancelTimeout(cookie);
                        end
                    end);
                end
            end);
        end);
    end
end

-- main

makeDoor("door13", true);
makeDoor("door14", false);

setSensorEnterListener("arena_cp", true, function()
    scene.respawnPoint = scene:getObjects("arena_cp")[1]:getTransform();
    moveDoor(false, "door13");
    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 1.0);
    scene.camera:findCameraComponent():follow(scene:getObjects("skull1_target")[1], const.EaseLinear, 1.0);
    startMusic("action3.ogg", true);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.player.angle = (boss.pos - scene.player.pos):angle();
    showLowerDialog(
    {
        {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog20.str1},
        {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog20.str2},
    }, function ()
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
        scene.cutscene = false;
        addTimeoutOnce(1.0, function()
            boss:findTargetableComponent().autoTarget = true;
            local p = bp[math.random(1, #bp)];
            boss.roamBehavior:reset();
            boss.roamBehavior.linearVelocity = bossFastSpeed;
            boss.roamBehavior.linearDamping = 1.0;
            boss.roamBehavior.dampDistance = 0.0;
            boss.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            boss.roamBehavior:start();
            addTimeout0(bossCheck);
            addTimeout(0.5, barrelSpawnCheck, { j = scene:getJoints("press17_joint")[1], spawn = scene:getObjects("spawn17")[1], launchTimer = nil, turn = 0 });
            addTimeout(0.5, barrelSpawnCheck, { j = scene:getJoints("press18_joint")[1], spawn = scene:getObjects("spawn18")[1], launchTimer = nil, turn = 1 });
        end);
    end);
end);

setSensorEnterListener("exit_cp", true, function()
    moveDoor(false, "door14");
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(2.5, function()
        audio:playSound("servo_move.ogg");
        local arena = scene:getObjects("arena0")[1];
        local grate = arena:findRenderQuadComponents("grate")[1];
        local pcs = scene.player:findRenderQuadComponents();
        local gsz = grate.height;
        local psz = {}
        for _, pc in pairs(pcs) do
            psz[pc] = pc.height;
        end
        local p1 = arena:getWorldPoint(grate.pos);
        local p2 = scene.player.pos;
        addTimeout0(function(cookie, dt)
            if grate.height <= 4.5 then
                cancelTimeout(cookie);
                return;
            end
            grate.height = grate.height - dt * 4.0;
            for _, pc in pairs(pcs) do
                pc.height = psz[pc] * (grate.height / gsz);
            end
            scene.player.pos = p1 + (p2 - p1) * (grate.height / gsz);
        end);
        completeLevel(3.0, tr.str26, tr.str27, "e1m3");
    end);
end);
