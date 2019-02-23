local arenaCenter = scene:getObjects("boss1_cp")[1].pos;
local constraint = vec2(100.0, 100.0 * (scene.gameHeight / scene.gameWidth));
local hookgun1 = scene:getInstances("hookgun1")[1];
local hookgun2 = scene:getInstances("hookgun2")[1];
local hookgun3 = scene:getInstances("hookgun3")[1];
local hookguns = { hookgun1, hookgun2, hookgun3 };
local eshield1 = scene:getObjects("eshield1");
local tentacle1 = scene:getObjects("tentacle1")[1];
local tentacle1weld = weldTentacleBones(tentacle1, {0, 97, 5, 92},
    {scene:getObjects("weldA")[1], scene:getObjects("weldB")[1], scene:getObjects("weldA2")[1], scene:getObjects("weldB2")[1]},
    6, 0.7);
local pulse1 = scene:getObjects("pulse1");
local swallowTimer = nil;
local jar1 = scene:getInstances("jar1")[1];
local cr0 = scene:getObjects("cr0")[1];
local tentacle2 = scene:getObjects("tentacle2")[1];
local tentacle2weld = weldTentacleBones2(tentacle2, {1, 2, 3, 6}, 3.0, 1.0);
local tentacle3 = scene:getObjects("tentacle3")[1];
local tentacle3weld = weldTentacleBones2(tentacle3, {1, 2, 3, 6}, 3.0, 1.0);
local tentacle4 = scene:getObjects("tentacle4")[1];
local tentacle4weld = weldTentacleBones2(tentacle4, {1, 2, 3, 6}, 3.0, 1.0);
local shakeStream = nil;
local swallowReturnXf = scene:getObjects("target3")[1]:getTransform();

local function makeHookGun(inst)
    local obj = findObject(inst.objects, "hookgun_tower");
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponRopeComponent(const.SceneObjectTypeNeutralMissile);
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.useFilter = true;
    inst.weapon.length = 12.0;
    inst.weapon.launchSpeed = 60.0;
    inst.weapon.launchDistance = 28.0;
    inst.weapon.pullSpeed = 8.0;
    inst.weapon.pullMaxForce = 500.0;
    inst.weapon.interval = 0.6;
    obj:addComponent(inst.weapon);
    inst.turret = GovernedTurretComponent();
    inst.turret.turnSpeed = math.rad(720);
    inst.turret.weapon = inst.weapon;
    inst.turret.haveSound = false;
    obj:addComponent(inst.turret);
    inst.tower = obj;
    local ph = findObject(inst.objects, "hookgun_ph"):findPlaceholderComponent();
    local function controlEnter(other, self)
        local items = {};
        local a = -90;
        if scene.inputTurret.active then
            a = 90;
        end
        table.insert(items, { pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(a), height = 6, image = "common2/arrow_button.png" });
        self.choice = showChoice(items, function(i)
            if scene.inputTurret.active then
                scene.inputPlayer.active = true;
                scene.inputTurret.active = false;
                inst.turret.active = false;
                scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.0);
            else
                scene.inputTurret:setShootImage("subway1/hook_icon.png");
                scene.inputPlayer.active = false;
                scene.inputTurret.active = true;
                inst.turret.active = true;
                scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.0);
            end
            self.timer = addTimeoutOnce(1.0, function()
                self.timer = nil;
                controlEnter(other, self);
            end);
        end);
    end
    ph.listener = createSensorListener(controlEnter, function(other, self)
        if self.choice ~= nil then
            self.choice:removeFromParent();
        end
        if scene.inputTurret.active then
            scene.inputPlayer.active = true;
            scene.inputTurret.active = false;
            inst.turret.active = false;
            scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.0);
        end
        if self.timer ~= nil then
            cancelTimeout(self.timer);
            self.timer = nil;
        end
    end, { choice = nil, timer = nil });
    inst.myPh = ph;
end

local function makeTentaclePulse(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local timeF = 0.2;
        local phaseF = 5.0;
        local a1F = 0.6;
        local a2F = 0.0;
        obj:addComponent(TentaclePulseComponent(15, timeF, phaseF, a1F, a2F));
    end
end

local function makeTentacleSwing(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 1.25;
        local f = 40000;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeJar1()
    jar1.myRoot = findObject(jar1.objects, "jar_root");
    jar1.myPart = findObject(jar1.objects, "jar_part");

    local rc = jar1.myPart:findRenderTerrainComponents()[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic4", 1);
    ac:startAnimation(const.AnimationDefault);
    jar1.myPart:addComponent(ac);
    jar1.myRoot.type = const.SceneObjectTypeEnemyBuilding;

    local c = FloatComponent(1.0, 0.5, 3.0);
    c.target = cr0;
    jar1.myPart:addComponent(c);
end

local function extendTentacle(tentacle, weld)
    tentacle.linearVelocity = tentacle:getDirection(15.0);
    for _, cookie in pairs(weld) do
        cookie.target.linearVelocity = tentacle:getDirection(15.0);
    end
    addTimeoutOnce(0.9, function()
        tentacle.linearVelocity = vec2(0, 0);
        for _, cookie in pairs(weld) do
            cookie.target.linearVelocity = vec2(0, 0);
        end
    end);
    addTimeoutOnce(1.9, function()
        unweldTentacleBones2(weld);
    end);
    addTimeoutOnce(2.9, function()
        local c = tentacle:findRenderTentacleComponent();
        local last = c.objects[#c.objects];
        local dummy = last:findDummyComponent("missile");
        tentacle.myWeapon = WeaponAcidballComponent(const.SceneObjectTypeEnemyMissile);
        tentacle.myWeapon.useFilter = true;
        tentacle.myWeapon.useFreeze = false;
        tentacle.myWeapon.pos = dummy.pos;
        tentacle.myWeapon.angle = dummy.angle;
        tentacle.myWeapon.velocity = 35.0;
        tentacle.myWeapon.interval = 0.1;
        tentacle.myWeapon.explosionTimeout = 1.0;
        tentacle.myWeapon.explosionImpulse = 40.0;
        tentacle.myWeapon.explosionDamage = 25.0;
        tentacle.myWeapon.toxicDamage = 10.0;
        tentacle.myWeapon.toxicDamageTimeout = 0.8;
        last:addComponent(tentacle.myWeapon);
        local function shootTimeout()
            if jar1.myRoot:alive() then
                tentacle.myWeapon:triggerOnce();
                addTimeoutOnce(1.0 + math.random() * 1.5, shootTimeout);
            end
        end;
        addTimeoutOnce(math.random() * 2.0, shootTimeout);
    end);
end

local function retractTentacle(tentacle)
    tentacle.linearVelocity = tentacle:getDirection(-25.0);
    addTimeoutOnce(1.0, function()
        tentacle.linearVelocity = vec2(0, 0);
    end);
end

function swallowSequence()
    local objs = scene:getObjects("ammo1");
    for i = 1, #objs, 1 do
        local s = nil;
        if scene.player:findPlayerComponent():haveWeapon(const.WeaponTypeRLauncher) then
            s = factory:createPowerupAmmo(const.WeaponTypeRLauncher);
        elseif scene.player:findPlayerComponent():haveWeapon(const.WeaponTypeLGun) then
            s = factory:createPowerupAmmo(const.WeaponTypePlasmaGun);
        end
        if s ~= nil then
            s.pos = objs[i].pos;
            scene:addObject(s);
        end
    end
    local v1 = scene:getObjects("swallow0_cp")[1].pos;
    swallowTimer = addTimeout0(function(cookie, dt)
        scene.player:applyForceToCenter((v1 - scene.player.pos):normalized() * 1000.0, true);
    end);
end

-- main

makeJar1();

for _, obj in pairs(pulse1) do
    obj.material = const.MaterialFlesh;
    local rc = obj:findRenderQuadComponent();
    obj:addComponent(QuadPulseComponent(rc, vec2(0, 0), 1.0, const.EaseInOutQuad, 0.95,
        1.0, const.EaseInOutQuad, 1.05));
end

for _ , obj in pairs(eshield1) do
    obj.myTimeout = 0;
end

addTimeout(0.25, function(cookie)
    if (boss1 == nil) or (boss1:findBossBuddyComponent().brainDead) then
        cancelTimeout(cookie);
        return;
    end
    for _ , obj in pairs(eshield1) do
        if (obj.mySpawned == nil) or (obj.mySpawned:scene() == nil) then
            obj.myTimeout = obj.myTimeout - 0.25;
            if (obj.myTimeout <= 0) and (not scene.camera:findCameraComponent():rectVisible(obj.pos, 5.0, 5.0)) then
                obj.mySpawned = factory:createPowerupAmmo(const.WeaponTypeEShield);
                obj.mySpawned:setTransform(obj:getTransform());
                scene:addObject(obj.mySpawned);
                obj.myTimeout = 30.0;
            end
        end
    end
end);

addTimeout(0.25, function(cookie)
    if boss1 == nil then
        cancelTimeout(cookie);
        return;
    end
    local c = boss1:findBossBuddyComponent();
    if c.brainDead then
        cancelTimeout(cookie);
        addTimeoutOnce(1.0, function()
            scene:getObjects("boss2_cp")[1].active = true;
            scene.cutscene = true;
            scene.player.linearDamping = 6.0;
            local p = scene:getObjects("boss1_path")[1];
            scene.player.roamBehavior:reset();
            scene.player.roamBehavior.linearVelocity = 10.0;
            scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            scene.player.roamBehavior:start();
        end);
    end
end);

setSensorEnterListener("boss2_cp", true, function(other)
    scene.player.roamBehavior:reset();
    local c = boss1:findBossBuddyComponent();
    c.canSwallow = true;
    addTimeout0(function(cookie, dt)
        if c.deathFinished then
            cancelTimeout(cookie);
            stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                swallowReturnXf = scene.player:getTransform();
                setSensorListener("boss1_cp", function(other) end, function(other) end);
                scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
                scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
                scene.camera:findCameraComponent():zoomTo(45, const.EaseLinear, 0);
                scene.player.pos = scene:getObjects("target1")[1].pos;
                scene.respawnPoint = scene:getObjects("target1")[1]:getTransform();
                scene.cutscene = false;
                swallowSequence();
                stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5);
            end);
        end
    end);
end);

makeAirlock("door1", false);
makeAirlock("door2", true);
makeAirlock("door3", true);
makeAirlock("door4", false);
makeAirlock("door5", false);

setSensorEnterListener("boss0_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("boss0_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("boss0_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 10.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("boss0b_cp", true, function(other)
    closeAirlock("door2", true);
    closeAirlock("door3", false);
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog130.str1},
            {"player", tr.dialog130.str2},
            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog130.str3},
            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog130.str4},
        }, function ()
            startMusic("action15.ogg", true);
            local obj = factory:createSummon2(true, const.zOrderExplosion);
            obj:setTransform(boss2:getTransform());
            scene:addObject(obj);
            addTimeoutOnce(2.0, function()
                boss2:setTransform(scene:getObjects("target4")[1]:getTransform());
                scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
                scene.player.roamBehavior.linearVelocity = 15.0;
                scene.player.roamBehavior.dampDistance = 1.0;
                addTimeoutOnce(2.9, function()
                    scene.player.roamBehavior:reset();
                    scene.cutscene = false;
                end);
                addTimeoutOnce(2.0, function()
                    boss1:findTargetableComponent().autoTarget = true;
                end);
            end);
        end);
    end);
end);

setSensorListener("boss1_cp", function(other)
    scene.camera:findCameraComponent():setConstraint(arenaCenter - constraint, arenaCenter + constraint);
    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
end, function(other)
    scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
end);

makeHookGun(hookgun1);
makeHookGun(hookgun2);
makeHookGun(hookgun3);

addTimeout(0.25, function(cookie)
    if boss1 == nil then
        cancelTimeout(cookie);
        return;
    end
    if boss1:findBossBuddyComponent().brainDead then
        for _, hookgun in pairs(hookguns) do
            if hookgun.weapon.hit then
                hookgun.weapon:tear();
            end
            hookgun.myPh.active = false;
            hookgun.myPh.visible = false;
        end
        cancelTimeout(cookie);
        return;
    end
    for _, hookgun in pairs(hookguns) do
        if hookgun.weapon.hit and (hookgun.timeout == nil) then
            local tout = 10;
            for _, hookgun2 in pairs(hookguns) do
                if (hookgun2.timeout == nil) and (hookgun2.weapon ~= hookgun.weapon) then
                    tout = 17;
                    break;
                end
            end
            for _, hookgun2 in pairs(hookguns) do
                if hookgun2.timeout ~= nil then
                    hookgun2.timeout = tout;
                end
            end
            hookgun.timeout = tout;
        end
    end
    for _, hookgun in pairs(hookguns) do
        if hookgun.timeout then
            hookgun.timeout = hookgun.timeout - 0.25;
            if hookgun.timeout <= 0 then
                hookgun.timeout = nil;
                hookgun.weapon:tear();
            end
        end
    end
end);

makeTentacleFlesh("tentacle1");
makeTentaclePulse("tentacle1");
makeTentacleFlesh("tentacle2");
makeTentacleSwing("tentacle2");
makeTentacleFlesh("tentacle3");
makeTentacleSwing("tentacle3");
makeTentacleFlesh("tentacle4");
makeTentacleSwing("tentacle4");

setSensorEnterListener("swallow0_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("swallow0_cp")[1]:getTransform();
    cancelTimeout(swallowTimer);
    unweldTentacleBones(tentacle1weld);
    tentacle1weld = weldTentacleBones(tentacle1, {7, 90, 1, 96},
        {scene:getObjects("weld1")[1], scene:getObjects("weld2")[1], scene:getObjects("weld3")[1], scene:getObjects("weld4")[1]},
        3, 0.7);
    addTimeoutOnce(1.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        addTimeoutOnce(1.0, function()
            showLowerDialog(
            {
                {"player", tr.dialog131.str1},
                {"player", tr.dialog131.str2},
            }, function ()
                jar1.myRoot.maxLife = settings.jar4.life;
                jar1.myRoot.life = settings.jar4.life;

                if scene.player:findPlayerComponent():haveWeapon(const.WeaponTypeLGun) then
                    jar1.myRoot.maxLife = jar1.myRoot.maxLife * 2.0;
                    jar1.myRoot.life = jar1.myRoot.life * 2.0;
                end

                local hb = RenderHealthbarComponent(vec2(0.0, 7.0), 0, 10.0, 1.5, const.zOrderMarker + 1);
                hb.color = { 1.0, 1.0, 1.0, 0.6 };
                jar1.myRoot:addComponent(hb);

                scene.cutscene = false;

                addTimeoutOnce(1.0, function()
                    extendTentacle(tentacle2, tentacle2weld);
                    extendTentacle(tentacle3, tentacle3weld);
                    extendTentacle(tentacle4, tentacle4weld);
                end);

                addTimeout0(function(cookie, dt)
                    if jar1.myRoot:dead() then
                        audio:playSound("buddy_die.ogg");
                        gameShell:setAchieved(const.AchievementImSorryBuddy);
                        cancelTimeout(cookie);
                        local hbc = jar1.myRoot:findRenderHealthbarComponent();
                        if hbc ~= nil then
                            hbc:removeFromParent();
                        end
                        local booms = jar1.myRoot:findDummyComponents("boom");
                        local to = 0;
                        for _, boom in pairs(booms) do
                            addTimeoutOnce(to, function()
                                local exp = factory:createExplosion1(const.zOrderExplosion);
                                exp.pos = jar1.myRoot:getWorldPoint(boom.pos);
                                scene:addObject(exp);
                            end);
                            to = to + 0.38;
                        end
                        addTimeoutOnce(0.5, function()
                            jar1.myPart:addComponent(FadeOutComponent(1.5));
                            cr0:changeLife(-cr0.life);
                            tentacle2:findTentacleSwingComponent():removeFromParent();
                            tentacle3:findTentacleSwingComponent():removeFromParent();
                            tentacle4:findTentacleSwingComponent():removeFromParent();
                            addTimeoutOnce(3.0, function()
                                retractTentacle(tentacle2);
                                retractTentacle(tentacle3);
                                retractTentacle(tentacle4);
                            end);
                            addTimeoutOnce(6.0, function()
                                startAmbientMusic(true);
                                scene.camera:findCameraComponent():tremorStart(0.5);
                                shakeStream = audio:createStream("queen_shake.ogg");
                                shakeStream:play();
                                addTimeoutOnce(2.0, function()
                                    scene:getObjects("swallow1_cp")[1].active = true;
                                    unweldTentacleBones(tentacle1weld);
                                    tentacle1weld = weldTentacleBones(tentacle1, {0, 97, 5, 92},
                                        {scene:getObjects("weldA")[1], scene:getObjects("weldB")[1], scene:getObjects("weldA2")[1], scene:getObjects("weldB2")[1]},
                                        6, 0.7);
                                    addTimeoutOnce(0.25, function()
                                        local v1 = scene:getObjects("target2")[1].pos;
                                        swallowTimer = addTimeout0(function(cookie, dt)
                                            scene.player:applyForceToCenter((v1 - scene.player.pos):normalized() * 1000.0, true);
                                        end);
                                    end);
                                end);
                            end);
                        end);
                    end
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("swallow1_cp", true, function(other)
    scene.respawnPoint = swallowReturnXf;
    audio:crossfade(shakeStream, nil, 0.5, 0.5, 0.3);
    stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
        cancelTimeout(swallowTimer);
        scene.player:findPlayerComponent().weapon:cancel();
        scene.player:findPlayerComponent().altWeapon:cancel();
        scene.camera:findCameraComponent():tremor(false);
        scene.player.linearVelocity = vec2(0, 0);
        scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
        scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
        scene.player.pos = swallowReturnXf.p;
        stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5);
    end);
    addTimeoutOnce(2.0, function()
        audio:playSound("buddy_vaporize.ogg");
        local objs = table.copy({boss1}, boss1:getObjects());
        for _, obj in pairs(objs) do
            obj:addComponent(FadeOutComponent(5.0));
        end
        boss1 = nil;
        addTimeoutOnce(6.0, function()
            startChamber(false);
        end);
    end);
end);
