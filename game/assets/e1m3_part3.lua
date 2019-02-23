local spiders = {};
local spider9cp = scene:getObjects("spider9_cp")[1];
local spider10cp = scene:getObjects("spider10_cp")[1];
local spider11cp = scene:getObjects("spider11_cp")[1];
local spider12cp = scene:getObjects("spider12_cp")[1];
local spider13cp = scene:getObjects("spider13_cp")[1];
local spider14cp = scene:getObjects("spider14_cp")[1];
local spider15cp = scene:getObjects("spider15_cp")[1];
local retreats = {};
local musicTimer = nil;
local respawnPosTimer = nil;
local control1 = scene:getObjects("control1_ph")[1]:findPlaceholderComponent();
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();
local control3 = scene:getObjects("control3_ph")[1]:findPlaceholderComponent();
local control4 = scene:getObjects("control4_ph")[1]:findPlaceholderComponent();
local dynomitePh = scene:getObjects("dynomite_ph")[1]:findPlaceholderComponent();
local dynomitePh2 = scene:getObjects("dynomite_ph2")[1]:findPlaceholderComponent();
local stop1 = true;
local stop4 = true;
local stop5 = true;
local stop6 = true;
local dynomite2 = scene:getObjects("dynomite2")[1];
local barrelSpawn1 = scene:getObjects("barrel_spawn1");
local spike1 = scene:getObjects("spike1")[1];
local spike2 = scene:getObjects("spike2")[1];
local babyTarget = scene:getObjects("baby_target");
local babyTimer = nil;
local babySpawned = {};
local queenDeadDialogTimer = nil;

local function spiderSpawnCp(other, self, spawn, retreat)
    if self.mySpawned then
        return;
    end
    if table.size(spiders) == 0 then
        musicTimer = addTimeoutOnce(0.5, function()
            startMusic("action5.ogg", false);
            musicTimer = nil;
        end);
    end
    local obj = factory:createSpider();
    obj:setTransformRecursive(scene:getObjects(spawn)[1]:getTransform());
    local s = obj:findSpiderComponent();
    s.autoTarget = true;
    s.canRotate = false;
    scene:addObject(obj);
    spiders[obj.cookie] = {obj, self};
    retreats[obj.cookie] = retreat;
    self.mySpawned = true;
end

local function spiderScaredAngryCp(other)
    if other.parent ~= nil then
        other = other.parent;
    end
    local s = other:findSpiderComponent();
    if s == nil then
        return;
    end
    if not s.autoTarget then
        return;
    end
    s.target = nil;
    s.autoTarget = false;
    addTimeoutOnce(1.0, function()
        s:angry();
        addTimeoutOnce(3, function()
            s:rotate();
            s.target = scene:getObjects(retreats[other.cookie])[1];
            retreats[other.cookie] = nil;
            s.legVelocity = 200.0;
            s.patrol = true;
            addTimeoutOnce(5, function()
                other:removeFromParent();
            end);
        end);
    end);
end;

local function spiderScaredCp(other)
    if other.parent ~= nil then
        other = other.parent;
    end
    local s = other:findSpiderComponent();
    if s == nil then
        return;
    end
    if not s.autoTarget then
        return;
    end
    s.target = nil;
    s.autoTarget = false;
    s:rotate();
    s.target = scene:getObjects(retreats[other.cookie])[1];
    spiders[other.cookie][2].mySpawned = false;
    spiders[other.cookie] = nil;
    retreats[other.cookie] = nil;
    s.legVelocity = 200.0;
    s.patrol = true;
    if table.size(spiders) == 0 then
        if musicTimer ~= nil then
            cancelTimeout(musicTimer);
            musicTimer = nil;
        else
            startAmbientMusic(true);
        end
    end
    addTimeoutOnce(5, function()
        other:removeFromParent();
    end);
end

local function spiderCleanup()
    local cc = scene.camera:findCameraComponent();
    for _, spider in pairs(spiders) do
        spider[2].mySpawned = false;
        if not cc:rectVisible(spider[1].pos, 12.0, 12.0) then
            retreats[spider[1].cookie] = nil;
            spider[1]:removeFromParent();
        end
    end
    if table.size(spiders) > 0 then
        if musicTimer ~= nil then
            cancelTimeout(musicTimer);
            musicTimer = nil;
        else
            startAmbientMusic(true);
        end
    end
    spiders = {};
end

function babySpawnCp(other, spawner, prob1)
    local enemies = scene:getObjects(spawner);
    for _, enemy in pairs(enemies) do
        local e = nil;
        if math.random() <= prob1 then
            e = factory:createBabySpider1();
        else
            e = factory:createBabySpider2();
        end
        e:setTransform(enemy:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end;
end

function setNoStop14()
    stop1 = false;
    stop4 = false;
    openAirlock("door5", false);
    openAirlock("door6", false);
end

function setNoStop5()
    stop5 = false;
end

function setNoStop6()
    stop6 = false;
    openAirlock("door7", false);
end

function setCartRespawn()
    if respawnPosTimer == nil then
        respawnPosTimer = addTimeout(0.1, function(cookie)
            scene.respawnPoint = transform(cart0.pos - cart0:getDirection(6), rot(cart0.angle));
        end);
    end
end

function resetCartRespawn()
    if respawnPosTimer ~= nil then
        cancelTimeout(respawnPosTimer);
        respawnPosTimer = nil;
    end
end

-- main

scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent().timeout = 30.0;
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHintTrigger(scene:getObjects("ha1_target")[1].pos, 1.0);
scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHint(scene:getObjects("ha1_target")[1].pos);

if settings.skill <= const.SkillEasy then
    scene:getObjects("ha1")[1].active = true;
end

makeAirlock("door4", true);

setSensorEnterListener("cart_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("cart_cp")[1]:getTransform();
    closeAirlock("door4", true);
    spiderCleanup();
    resetCartRespawn();
end);

cart0:addComponent(CollisionSensorComponent());

setSensorEnterListener(cart0.name, false, spiderScaredCp);

setSensorEnterListener("cart_stop1_cp", false, function(other)
    if stop1 then
        cart0Stop();
    end
end);

setSensorEnterListener("cart_stop3_cp", false, function(other)
    cart0Stop();
end);

makeAirlock("door5", false);

setSensorEnterListener(spider9cp.name, false, spiderSpawnCp, spider9cp, "spider9_spawn", "spider9_retreat");
setSensorEnterListener(spider10cp.name, false, function(other)
    spiderSpawnCp(other, spider10cp, "spider10_spawn", "spider9_retreat");
    setCartRespawn();
end);
setSensorEnterListener(spider11cp.name, false, spiderSpawnCp, spider11cp, "spider11_spawn", "spider9_retreat");
setSensorEnterListener(spider12cp.name, true, function(other)
    spiderSpawnCp(other, spider12cp, "spider12_spawn", "spider12_retreat");
    babySpawnCp(other, "baby4", 0.5);
end);

setSensorEnterListener("spider_scared_cp", false, spiderScaredAngryCp);
setSensorEnterListener("spider_fast_scared_cp", false, spiderScaredCp);

control1.listener = createSensorEnterListener(true, function(other)
    control1.visible = false;
    control1.active = false;
    openAirlock("door5", true);
    addTimeoutOnce(2.0, function()
        cart0Proceed();
    end);
end);

setSensorEnterListener("nest_encounter_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.player.angle = (scene:getObjects("nest_encounter_cp")[1].pos - scene.player.pos):angle();
    setAmbientMusic("ambient5.ogg");
    startAmbientMusic(true);
    showUpperDialog(
    {
        {"player", tr.dialog28.str1},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("baby1_cp", true, babySpawnCp, "baby1", 0.5);
setSensorEnterListener("baby2_cp", true, babySpawnCp, "baby2", 0.0);
setSensorEnterListener("baby3_cp", true, babySpawnCp, "baby3", 0.5);
setSensorEnterListener(spider13cp.name, false, spiderSpawnCp, spider13cp, "spider13_spawn", "spider13_retreat");

makeAirlock("door6", false);

setSensorEnterListener("cart_stop4_cp", true, function(other)
    if stop4 then
        cart0Stop();
        control2.visible = true;
        control2.active = true;
    end
end);

control2.listener = createSensorEnterListener(true, function(other)
    control2.visible = false;
    control2.active = false;
    openAirlock("door6", true);
    addTimeoutOnce(2.0, function()
        cart0Proceed();
    end);
end);

setSensorEnterListener("cart_stop5_cp", false, function(other)
    if stop5 then
        cart0Stop();
    end
end);

makeAirlock("door7", false);

setSensorEnterListener("cart_stop6_cp", false, function(other)
    if stop6 then
        cart0Stop();
        control3.visible = true;
        control3.active = true;
    end
end);

control3.listener = createSensorEnterListener(true, function(other)
    control3.visible = false;
    control3.active = false;
    openAirlock("door7", true);
    addTimeoutOnce(2.0, function()
        cart0Proceed();
    end);
end);

setSensorEnterListener("cart_stop7_cp", false, function(other)
    cart0Stop();
end);

addTimeout(1.0, function(cookie)
    if dynomite2:scene() == nil then
        control4.visible = true;
        control4.active = true;
        cancelTimeout(cookie);
    end
end);

control4.listener = createSensorEnterListener(true, function(other)
    control4.visible = false;
    control4.active = false;
    cart0Back();
    addTimeoutOnce(3.0, function()
        setAmbientMusic("ambient3.ogg");
        startAmbientMusic(true);
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 2.0, function()
            cart0Reset();
            scene.player:setTransform(transform(cart0.pos - cart0:getDirection(6), rot(cart0.angle)));
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 2.0);
        end);
    end);
end);

setSensorEnterListener(spider14cp.name, false, spiderSpawnCp, spider14cp, "spider14_spawn", "spider14_retreat");
setSensorEnterListener(spider15cp.name, false, spiderSpawnCp, spider15cp, "spider15_spawn", "spider14_retreat");

setSensorEnterListener("cart_stop2_cp", false, function(other)
    cart0Stop();
end);

addTimeout(1.0, function(cookie)
    local cc = scene.camera:findCameraComponent();
    for _, spawn in pairs(barrelSpawn1) do
        if (spawn.myObj == nil) or (spawn.myObj:scene() == nil) then
            if not cc:rectVisible(spawn.pos, 50.0, 50.0) then
                spawn.myObj = factory:createBarrel2();
                local xf = spawn:getTransform();
                xf.q = rot(math.random(0, math.pi * 2.0));
                spawn.myObj:setTransform(xf);
                scene:addObject(spawn.myObj);
            end
        end
    end
end);

makeGear("wheel1", "wheel1_joint", "wheel2", "wheel2_joint", 1);
makeGear("wheel2", "wheel2_joint", "wheel3", "wheel3_joint", 1);

addTimeout0(function(cookie, dt, self)
    self[3] = self[3] + dt;
    self[4] = self[4] + dt;
    if (self[2]:getJointTranslation() <= self[2].lowerLimit) then
        self[3] = 0;
        spike1.linearVelocity = vec2(0, 0) - spike1.linearVelocity;
        spike2.linearVelocity = vec2(0, 0) - spike2.linearVelocity;
        self[1]:remove();
        self[1] = makeGear("wheel1", "wheel1_joint", "spike1", "spike1_joint", -0.035)
    end
    if (self[2]:getJointTranslation() >= self[2].upperLimit) then
        self[4] = 0;
        spike1.linearVelocity = vec2(0, 0) - spike1.linearVelocity;
        spike2.linearVelocity = vec2(0, 0) - spike2.linearVelocity;
        self[1]:remove();
        self[1] = makeGear("wheel1", "wheel1_joint", "spike1", "spike1_joint", 0.035)
    end
    if (self[3] > 1.0) or (self[4] > 1.0) then
        scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():removeAllHints();
        self[3] = -100.0;
        self[4] = -100.0;
    end
end, { makeGear("wheel1", "wheel1_joint", "spike1", "spike1_joint", 0.035),
       scene:getJoints("spike1_joint")[1], 0, 0 });

makeGear("spike1", "spike1_joint", "spike2", "spike2_joint", -1);

makeAirlock("door8", false);
makeAirlockTrigger("door8_cp", "door8");

dynomitePh.listener = createSensorEnterListener(false, function(other)
    if other:findPlayerComponent().inventory:have(const.InventoryItemDynomite) then
        local obj = dynomitePh.parent;
        obj.freezable = false;

        other:findPlayerComponent().inventory:take(const.InventoryItemDynomite);
        dynomitePh:removeFromParent();

        local tb = factory:createTimebomb(const.InventoryItemDynomite, 1.8, 87, -104);
        tb:setBlast(200.0, 200.0, { const.SceneObjectTypePlayer });
        tb.timeoutFn = function ()
            local rocks = scene:getObjects("rock1_boom");
            local to = 0.2;
            for i = 1, #rocks, 1 do
                addTimeoutOnce(to, function()
                    local exp = factory:createExplosion1(const.zOrderExplosion);
                    exp.pos = rocks[i].pos;
                    scene:addObject(exp);
                    rocks[i]:removeFromParent();
                    if i == #rocks then
                        dynomitePh2.active = true;
                        dynomitePh2.visible = true;
                    end
                end);
                to = to + 0.2;
            end
        end
        obj:addComponent(tb);
    end
end);

dynomitePh2.listener = createSensorEnterListener(false, function(other)
    if other:findPlayerComponent().inventory:have(const.InventoryItemDynomite) then
        local obj = dynomitePh2.parent;
        obj.freezable = false;

        other:findPlayerComponent().inventory:take(const.InventoryItemDynomite);
        dynomitePh2:removeFromParent();

        local tb = factory:createTimebomb(const.InventoryItemDynomite, 1.8, 87, -104);
        tb:setBlast(200.0, 200.0, { const.SceneObjectTypePlayer });
        tb.timeoutFn = function ()
            local rocks = scene:getObjects("rock2_boom");
            local to = 0.2;
            for _, rock in pairs(rocks) do
                addTimeoutOnce(to, function()
                    local exp = factory:createExplosion1(const.zOrderExplosion);
                    exp.pos = rock.pos;
                    scene:addObject(exp);
                    rock:removeFromParent();
                end);
                to = to + 0.2;
            end
        end
        obj:addComponent(tb);
    end
end);

makeAirlock("door9", true);

local door9parts = scene:getInstances("door9")[1].objects;
for _, obj in pairs(door9parts) do
    obj.collisionImpulseMultiplier = 0;
end

scene:getObjects("terrain_boss")[1].collisionImpulseMultiplier = 0;

setSensorEnterListener("boss_cp", true, function(other)
    closeAirlock("door9", true);
    scene.respawnPoint = scene:getObjects("boss_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.player.angle = (scene:getObjects("boss")[1].pos - scene.player.pos):angle();
    scene.cutscene = true;
    scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint_1")[1].pos, scene:getObjects("constraint_2")[1].pos);
    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
    local bossObj = scene:getObjects("boss")[1];
    local boss = bossObj:findBossQueenComponent();
    addTimeoutOnce(2.0, function()
        showLowerMsg(2.5,
            {"player", tr.dialog29.str1},
        function()
            scene.cutscene = false;
        end);
        addTimeoutOnce(1.0, function()
            boss.autoTarget = true;
            startMusic("action6.ogg", true);
        end);
    end);
    addTimeout0(function(cookie, dt)
        if boss.deathStarted then
            cancelTimeout(cookie);
            addTimeoutOnce(0.1, function()
                local backObj = bossObj:getObjects("back")[1];
                for i = 1, 200, 1 do
                    local e = nil;
                    if math.random() <= 0.5 then
                        e = factory:createBabySpider1();
                    else
                        e = factory:createBabySpider2();
                    end
                    e:setTransform(transform(backObj.pos + vec2(math.random(0, 8), 0):rotated(math.random(0, math.pi * 2.0)),
                        rot(math.random(0, math.pi * 2.0))));
                    local ec = e:findBabySpiderComponent();
                    ec.target = babyTarget[math.random(1, #babyTarget)];
                    ec.spawnPowerup = false;
                    ec:setTrapped();
                    scene:addObject(e);
                    table.insert(babySpawned, e);
                end
                babyTimer = addTimeout(3.0, function()
                    for _, obj in pairs(babySpawned) do
                        local c = obj:findTargetableComponent();
                        if c ~= nil then
                            c.target = babyTarget[math.random(1, #babyTarget)];
                        end
                    end
                end);
            end);
        end
    end);
    addTimeout(0.5, function(cookie)
        if boss.deathFinished then
            cancelTimeout(cookie);
            startAmbientMusic(true);
            scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 3.0);
            gameShell:setAchieved(const.AchievementArachnophobia);
            addTimeoutOnce(1.9, function()
                cancelTimeout(babyTimer);
                local t = 0.0;
                addTimeout0(function(cookie, dt)
                    t = t + dt;
                    if t >= 0.15 then
                        audio:playSound("baby_spider_die.ogg");
                        t = math.random(0.0, 0.05);
                    end
                    local done = true;
                    local max = 6;
                    for k, obj in pairs(babySpawned) do
                        local c = obj:findBabySpiderComponent();
                        if c ~= nil then
                            c.haveSound = false;
                            obj:changeLife(-obj.life);
                            babySpawned[k] = nil;
                            done = false;
                            max = max - 1;
                            if max == 0 then
                                break;
                            end
                        end
                    end
                    if done then
                        cancelTimeout(cookie);
                    end
                end);
            end);
            queenDeadDialogTimer = addTimeoutOnce(5.0, function()
                queenDeadDialogTimer = nil;
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                showLowerDialog(
                {
                    {"player", tr.dialog30.str1},
                    {"player", tr.dialog30.str2},
                    {"player", tr.dialog30.str3},
                }, function ()
                    scene.cutscene = false;
                    scene:getObjects("exit_ga")[1]:findGoalAreaComponent():addGoal(scene:getObjects("exit_goal")[1].pos);
                end);
            end);
        end
    end);
end);

setSensorEnterListener("queen1_cp", true, function(other)
    audio:playSound("queen_hit.ogg");
    scene.camera:findCameraComponent():shake(2.0);
    addTimeoutOnce(1.0, function()
        audio:playSound("queen_hit.ogg");
        scene.camera:findCameraComponent():shake(2.0);
        addTimeoutOnce(2.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog31.str1}
            }, function ()
                scene.cutscene = false;
            end);
        end);
    end);
end);

setSensorEnterListener("queen2_cp", true, function(other)
    audio:playSound("queen_angry.ogg");
    scene.camera:findCameraComponent():tremor(true);
    local stream = audio:createStream("queen_shake.ogg");
    stream:play();
    addTimeoutOnce(2.0, function()
        scene.camera:findCameraComponent():tremor(false);
        stream:stop();
        addTimeoutOnce(2.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog32.str1}
            }, function ()
                scene.cutscene = false;
            end);
        end);
    end);
end);

setSensorEnterListener("queen3_cp", true, function(other)
    setSensorEnterListener("queen4_cp", true, function(other)
        audio:playSound("queen_angry.ogg");
        scene.camera:findCameraComponent():tremor(true);
        local stream = audio:createStream("queen_shake.ogg");
        stream:play();
        addTimeoutOnce(2.0, function()
            scene.camera:findCameraComponent():tremor(false);
            stream:stop();
            addTimeoutOnce(2.0, function()
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                showLowerDialog(
                {
                    {"player", tr.dialog33.str1}
                }, function ()
                    scene.cutscene = false;
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("queen5_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog34.str1},
        {"player", tr.dialog34.str2}
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("queen6_cp", true, function(other)
    audio:playSound("queen_spawn.ogg");
    addTimeoutOnce(1.5, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog35.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("pre_exit_cp", true, function(other)
    if queenDeadDialogTimer ~= nil then
        cancelTimeout(queenDeadDialogTimer);
    end
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog36.str1},
        {"player", tr.dialog36.str2},
        {"player", tr.dialog36.str3},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("exit_cp", true, function()
    completeLevel(1.5, tr.str27, tr.str28, "e1m4");
end);
