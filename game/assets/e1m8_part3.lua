local beetle1 = scene:getObjects("beetle1");
local beetle1Target = scene:getObjects("beetle1_target");
local beetle1Timer = nil;
local control6 = scene:getObjects("control6_ph")[1]:findPlaceholderComponent();
local flamer1 = scene:getInstances("flamer1")[1];
local control7 = scene:getObjects("control7_ph")[1]:findPlaceholderComponent();
local control8 = scene:getObjects("control8_ph")[1]:findPlaceholderComponent();
local pod3Timer = nil;
local pod3Timeout = 0;
local pod3Spawn = scene:getObjects("pod3_spawn")[1];
local pod3 = {};
local pod4Timer = nil;
local pod4Timeout = 0;
local pod4Spawn = scene:getObjects("pod4_spawn")[1];
local pod4 = {};
local thinkTwice = true;
local bugsSpawned = false;
local flamerUsed = false;

local function spawnCentipedes()
    local spawn = scene:getObjects("swarm_spawn_centipede");
    for _, obj in pairs(spawn) do
        local enemy = factory:createCentipede1();
        enemy:findCentipedeComponent().spawnPowerup = false;
        addSpawnedEnemyFreezable(enemy, obj);
    end
end

local function makeFlamer(inst)
    local obj = findObject(inst.objects, "flamer_tower");
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponFlamethrowerComponent(const.SceneObjectTypeNeutralMissile);
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.damage = 400.0;
    obj:addComponent(inst.weapon);
    inst.turret = GovernedTurretComponent();
    inst.turret.turnSpeed = math.rad(180);
    inst.turret.weapon = inst.weapon;
    obj:addComponent(inst.turret);
    inst.tower = obj;
end

local function makeFlamerControl(ph, inst)
    local function controlEnter(other, self)
        thinkTwice = false;
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
                scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                flamerUsed = true;
            else
                scene.inputTurret:setShootImage("subway1/flamer_icon.png");
                scene.inputPlayer.active = false;
                scene.inputTurret.active = true;
                inst.turret.active = true;
                scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
                scene.camera:findCameraComponent():follow(inst.tower, const.EaseOutQuad, 1.5);
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
            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
            if scene.player:alive() then
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
            end
        end
        if self.timer ~= nil then
            cancelTimeout(self.timer);
            self.timer = nil;
        end
    end, { choice = nil, timer = nil });
end

local function startPods34()
    pod3Timer = addTimeout0(function(cookie, dt)
        pod3Timeout = pod3Timeout - dt;
        if pod3Timeout < 0 then
            local inst = scene:instanciate("e1m8_pod.json", pod3Spawn:getTransform());
            makePod(inst, "pod3_path", 90.0);
            startPod(inst);
            pod3[inst.myPod.cookie] = inst;
            pod3Timeout = 3.0;
        end
    end);
    pod4Timer = addTimeout0(function(cookie, dt)
        pod4Timeout = pod4Timeout - dt;
        if pod4Timeout < 0 then
            local inst = scene:instanciate("e1m8_pod.json", pod4Spawn:getTransform());
            inst.myBeetles = spawnBeetleProb("pod4_beetle_spawn", 0.5, 0.5, 0.5, 0.5, vec2(0, 0));
            for _, obj in pairs(inst.myBeetles) do
                obj:findBeetleComponent():setTrapped();
                obj:findBeetleComponent().autoTarget = true;
            end
            makePod(inst, "pod4_path", 90.0);
            startPod(inst);
            pod4[inst.myPod.cookie] = inst;
            pod4Timeout = 3.0;
        end
    end);
end

local function stopPods34()
    cancelTimeout(pod3Timer);
    local cookies = {};
    for cookie, inst in pairs(pod3) do
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        table.insert(cookies, cookie);
    end
    for _, cookie in pairs(cookies) do
        pod3[cookie] = nil;
    end
    pod3Timeout = 0;
    cancelTimeout(pod4Timer);
    cookies = {};
    for cookie, inst in pairs(pod4) do
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.myBeetles) do
            obj:removeFromParent();
        end
        table.insert(cookies, cookie);
    end
    for _, cookie in pairs(cookies) do
        pod4[cookie] = nil;
    end
    pod4Timeout = 0;
end

function stairs3Down()
    scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};
    cancelTimeout(beetle1Timer);
    stopPods34();
    addTimeoutOnce(2.0, function()
        if bugsSpawned and (not flamerUsed) then
            gameShell:setAchieved(const.AchievementBugged);
        end
    end);
end

function stairs3Up()
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    beetle1Timer = addTimeout(3.0, function()
        for _, obj in pairs(beetle1) do
            local c = obj:findTargetableComponent();
            c.target = beetle1Target[math.random(1, #beetle1Target)];
        end
    end);
    startPods34();
end

-- main

for _, obj in pairs(beetle1) do
    local c = obj:findBeetleComponent();
    if c ~= nil then
        c:setTrapped();
    end
    c = obj:findCentipedeComponent();
    if c ~= nil then
        c:setTrapped();
    end
end

makeStairs("stairs3", stairs3Down, stairs3Up);

makePodStop("pod3_stop", 2.0, pod3, function()
    pod3Timeout = pod3Timeout + 2.0;
end);

setSensorEnterListener("pod3_kill", false, function(other)
    local inst = pod3[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        pod3[other.cookie] = nil;
    end
end);

makePodStop("pod4_stop", 2.0, pod4, function()
    pod4Timeout = pod4Timeout + 2.0;
end);

setSensorEnterListener("pod4_kill", false, function(other)
    local inst = pod4[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.myBeetles) do
            obj:removeFromParent();
        end
        pod4[other.cookie] = nil;
    end
end);

makeAirlock("door15", false);
makeAirlock("door16", false);
makeAirlock("door17", false);
makeAirlock("door18", false);
makeAirlock("door19", false);
makeAirlock("door20", false);
makeAirlock("door21", true);
makeAirlock("door22", true);
makeAirlock("door23", false);
makeAirlockTrigger("door23_cp", "door23");
makeAirlock("door24", false);
makeAirlockTrigger("door24_cp", "door24");

setSensorEnterListener("pods3_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog104.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("pods4_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog105.str1},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("incubator1_cp", true, function(other)
    fixedSpawn("incubator1");
end);

setSensorEnterListener("incubator2_cp", true, function(other)
    fixedSpawn("incubator2");
end);

setSensorEnterListener("swarm1_cp", false, function(other)
    scene.player.linearVelocity = scene:getObjects("swarm1_cp")[1]:getDirection(5.0);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog106.str1},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("swarm2_cp", false, function(other)
    scene.player.linearVelocity = scene:getObjects("swarm2_cp")[1]:getDirection(5.0);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog107.str1},
    }, function ()
        scene.cutscene = false;
    end);
end);

control6.listener = createSensorEnterListener(true, function(other)
    scene.respawnPoint = scene:getObjects("swarm3_cp")[1]:getTransform();
    scene:getObjects("swarm1_cp")[1].active = false;
    scene:getObjects("swarm2_cp")[1].active = false;
    scene:getObjects("swarm3_cp")[1].active = true;
    scene:getObjects("swarm4_cp")[1].active = true;
    control6.active = false;
    control6.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog108.str1},
            {"player", tr.dialog108.str2},
        }, function ()
            addTimeoutOnce(1.0, function()
                startMusic("action11.ogg", true);
                audio:playSound("lights_on.ogg");
                local swarm = spawnBeetleProb("swarm_spawn_beetle", 0.8, 0.1, 0.05, 0.05, vec2(1, 1));
                swarm = table.copy(swarm, spawnBeetleProb("swarm_spawn_beetle", 0.95, 0.0, 0.05, 0.0, vec2(1, -1), 0.4));
                addTimeoutOnce(1.5, function()
                    audio:playSound("lights_on.ogg");
                    addTimeoutOnce(1.5, function()
                        closeAirlock("door21", false);
                        audio:playSound("lights_on.ogg");
                        scene.camera:findCameraComponent():zoomTo(55, const.EaseOutQuad, 1.0);
                        local lights = scene:getObjects("terrain1")[1]:findLightComponent():getLights("swarm_light");
                        for _, light in pairs(lights) do
                            light.visible = true;
                        end
                        bugsSpawned = true;
                        addTimeoutOnce(1.5, function()
                            showUpperMsg(4.0, {"player", tr.dialog109.str1}, nil);
                            addTimeoutOnce(4.0, function()
                                scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("swarm8_cp")[1].pos);
                                scene.cutscene = false;
                                scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.0);
                                addTimeoutOnce(1.0, function()
                                    for _, obj in pairs(swarm) do
                                        local c = obj:findTargetableComponent();
                                        if c ~= nil then
                                            c.autoTarget = true;
                                        end
                                    end
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("swarm3_cp", true, function(other)
    spawnCentipedes();
    local swarm = spawnBeetleProb("swarm_spawn_beetle", 0.8, 0.1, 0.05, 0.05, vec2(0, 0));
    for _, obj in pairs(swarm) do
        obj:findTargetableComponent().autoTarget = true;
    end
    scene.respawnPoint = scene:getObjects("swarm4_cp")[1]:getTransform();
end);

setSensorEnterListener("swarm4_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("swarm5_cp")[1]:getTransform();
end);

setSensorEnterListener("swarm5_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("swarm6_cp")[1]:getTransform();
end);

setSensorEnterListener("swarm6_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("swarm7_cp")[1]:getTransform();
end);

setSensorEnterListener("swarm7_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("swarm8_target")[1]:getTransform();
end);

setSensorEnterListener("swarm8_cp", true, function(other)
    scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("swarm8_cp")[1].pos);
    scene.respawnPoint = scene:getObjects("swarm8_cp")[1]:getTransform();
    scene.cutscene = true;
    local p = scene:getObjects("swarm8_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = settings.player.moveSpeed;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

control8.listener = createSensorListener(function(other, self)
    if self.i == 0 then
        self.i = 1;
        startAmbientMusic(true);
        closeAirlock("door22", true);
        scene.player.linearDamping = 6.0;
        scene.player.roamBehavior:damp();
        addTimeoutOnce(1.0, function()
            scene.player.roamBehavior:reset();
            scene.cutscene = false;
            scene.player.active = false;
            scene.player.active = true;
        end);
    else
        local items = {};
        local a = -90;
        if self.closed then
            a = 90;
        end
        table.insert(items, { pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(a), height = 6, image = "common2/arrow_button.png" });
        self.choice = showChoice(items, function(i)
            control8.active = false;
            control8.visible = false;
            if thinkTwice then
                thinkTwice = false;
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                showLowerDialog(
                {
                    {"player", tr.dialog110.str1}
                }, function ()
                    scene.cutscene = false;
                    addTimeoutOnce(0.5, function()
                        control8.active = true;
                        control8.visible = true;
                    end);
                end);
            else
                if self.closed then
                    openAirlock("door22", true);
                    openAirlock("door20", false);
                    openAirlock("door15", false);
                else
                    closeAirlock("door22", true);
                    closeAirlock("door20", false);
                    closeAirlock("door15", false);
                end
                self.closed = not self.closed;
                addTimeoutOnce(0.5, function()
                    control8.active = true;
                    control8.visible = true;
                end);
            end
        end);
    end
end, function(other, self)
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
end, { i = 0, closed = true, choice = nil });

makeFlamer(flamer1);
makeFlamerControl(control7, flamer1);

-- color puzzle start

local puzzleDoneTimeout = 0.7;
local puzzleDone = false;
local puzzleTimer = nil;
local puzzleState = {};
local puzzleRc = {};
local puzzleI = math.random(1, 3);
local puzzleColor;
local puzzleWinColor;
if puzzleI == 1 then
    puzzleColor = {
        {0.0, 1.0, 1.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0}
    };
    puzzleWinColor = {1.0, 0.0, 0.0, 1.0};
elseif puzzleI == 2 then
    puzzleColor = {
        {1.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0}
    };
    puzzleWinColor = {0.0, 1.0, 0.0, 1.0};
else
    puzzleColor = {
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 1.0}
    };
    puzzleWinColor = {0.0, 1.0, 1.0, 1.0};
end
for i = 1, 8, 1 do
    puzzleState[i] = {};
    puzzleRc[i] = scene:getObjects("terrain1")[1]:findRenderQuadComponents("puzzle_image"..i)[1];
    puzzleRc[i].myOn = false;
    puzzleRc[i].myTimer = nil;
end
local function puzzleCheck(cookie, dt)
    local ccCnt = 0;
    local aw = scene.player:findPlayerComponent().altWeapon;
    for i = 1, 8, 1 do
        local wasOn = puzzleRc[i].myOn;
        puzzleRc[i].myOn = false;
        for _, obj in pairs(puzzleState[i]) do
            if obj.type == const.SceneObjectTypePlayer then
                puzzleRc[i].myOn = true;
                break;
            elseif (aw.heldObject ~= obj) and (obj.linearVelocity:lenSq() < 8.0 * 8.0) then
                puzzleRc[i].myOn = true;
                break;
            end
        end
        if puzzleRc[i].myOn then
            puzzleRc[i].color = puzzleColor[i];
            if puzzleRc[i].myTimer ~= nil then
                cancelTimeout(puzzleRc[i].myTimer);
                puzzleRc[i].myTimer = nil;
            end
            if not wasOn then
                audio:playSound("lever_pull.ogg");
            end
            if ((puzzleColor[i][1] == puzzleWinColor[1]) and
                (puzzleColor[i][2] == puzzleWinColor[2]) and
                (puzzleColor[i][3] == puzzleWinColor[3])) then
                ccCnt = ccCnt + 1;
            else
                ccCnt = -100;
            end
        else
            if wasOn and puzzleRc[i].myTimer == nil then
                local tweening = SingleTweening(0.5, const.EaseOutQuad, 0.0, 1.0, false);
                puzzleRc[i].myTimer = addTimeout0(function(cookie, dt, self)
                    local v = tweening:getValue(self.t);
                    local c = {
                        (1 - v) * puzzleColor[i][1] + v,
                        (1 - v) * puzzleColor[i][2] + v,
                        (1 - v) * puzzleColor[i][3] + v,
                        1.0
                    };
                    puzzleRc[i].color = c;
                    self.t = self.t + dt;
                    if tweening:finished(self.t) then
                        cancelTimeout(cookie);
                        puzzleRc[i].myTimer = nil;
                    end
                end, { t = 0 });
            end
        end
    end
    if not puzzleDone then
        if (ccCnt == 3) then
            puzzleDoneTimeout = puzzleDoneTimeout - dt;
            if puzzleDoneTimeout < 0 then
                for i = 1, 8, 1 do
                    scene:getObjects("puzzle_cp"..i)[1].active = false;
                end
                audio:playSound("alarm_off.ogg");
                local laser2 = scene:getObjects("laser2");
                for _, obj in pairs(laser2) do
                    obj:addComponent(FadeOutComponent(1.0));
                end
                puzzleDone = true;
            end
        else
            puzzleDoneTimeout = 0.7;
        end
    end
end
for i = 1, 8, 1 do
    setSensorListener("puzzle_cp"..i, function(other, objs)
        if objs[other.cookie] == nil then
            objs[other.cookie] = { count = 1, obj = other };
            if (other.type == const.SceneObjectTypePlayer) or (other.name == "stone1") then
                puzzleState[i][other.cookie] = other;
            end
            if puzzleTimer == nil then
                puzzleTimer = addTimeout0(puzzleCheck);
            end
        else
            objs[other.cookie].count = objs[other.cookie].count + 1;
        end
    end, function(other, objs)
        objs[other.cookie].count = objs[other.cookie].count - 1;
        if objs[other.cookie].count == 0 then
            objs[other.cookie] = nil;
            if (other.type == const.SceneObjectTypePlayer) or (other.name == "stone1") then
                puzzleState[i][other.cookie] = nil;
                local done = true;
                for i = 1, 8, 1 do
                    if table.size(puzzleState[i]) > 0 then
                        done = false;
                        break;
                    end
                end
                if done then
                    cancelTimeout(puzzleTimer);
                    puzzleTimer = nil;
                    puzzleCheck(0, 0);
                end
            end
        end
    end, {});
end

-- color puzzle end

makeKeyDoor("yellow_door", "yellow_key_ph", function()
    control7.active = true;
    control7.visible = true;
end);

setSensorEnterListener("incubator3_cp", true, function(other)
    fixedSpawn("incubator3");
end);

setSensorEnterListener("incubator4_cp", true, function(other)
    fixedSpawn("incubator4");
end);
