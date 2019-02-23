local lantern1 = scene:getObjects("lantern1")[1];
local lanternRespawnTimer = nil;
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();
local control3 = scene:getObjects("control3_ph")[1]:findPlaceholderComponent();
local control4 = scene:getObjects("control4_ph")[1]:findPlaceholderComponent();
local control5 = scene:getObjects("control5_ph")[1]:findPlaceholderComponent();
local powergen1 = scene:getObjects("powergen1")[1];
local powergen2 = scene:getObjects("powergen2")[1];
local powergen1Light = scene:getObjects("terrain2")[1]:findLightComponent():getLights("powergen1_light")[1];
local powergen2Light = scene:getObjects("terrain2")[1]:findLightComponent():getLights("powergen2_light")[1];
local lantern1Reached1 = false;
local lantern1Reached2 = false;
local queen = scene:getObjects("queen")[1];
local pod1Timer = nil;
local pod1Timeout = 0;
local pod1Spawn = scene:getObjects("pod1_spawn")[1];
local pod1 = {};
local pod2Timer = nil;
local pod2Timeout = 0;
local pod2Spawn = scene:getObjects("pod2_spawn")[1];
local pod2 = {};

local function brakeGrate(name, quiet)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        obj.active = false;
        obj:findRenderQuadComponent().drawable:setImage("common2/grate5.png");
    end
    if not quiet then
        scene.camera:findCameraComponent():roll(math.rad(5));
        audio:playSound("grate_hit.ogg");
    end
end

local function spawnBabySpiderProb(name, normal, red)
    local spawn = scene:getObjects(name);
    local probs = {normal, red};
    local res = 1;
    local enemies = {};
    for _, obj in pairs(spawn) do
        local val = math.random();
        local prob = 0;
        for i = 1, 2, 1 do
            prob = prob + probs[i];
            if val < prob then
                res = i;
                break;
            end
        end
        local e;
        if res == 1 then
            e = factory:createBabySpider1();
        else
            e = factory:createBabySpider2();
        end
        addSpawnedEnemyFreezable(e, obj);
        table.insert(enemies, e);
    end
    return enemies;
end

local function startPods12()
    pod1Timer = addTimeout0(function(cookie, dt)
        pod1Timeout = pod1Timeout - dt;
        if pod1Timeout < 0 then
            local inst = scene:instanciate("e1m8_pod.json", pod1Spawn:getTransform());
            makePod(inst, "pod1_path", -90.0);
            startPod(inst);
            pod1[inst.myPod.cookie] = inst;
            pod1Timeout = 3.0;
        end
    end);
    pod2Timer = addTimeout0(function(cookie, dt)
        pod2Timeout = pod2Timeout - dt;
        if pod2Timeout < 0 then
            local inst = scene:instanciate("e1m8_pod.json", pod2Spawn:getTransform());
            inst.mySpiders = spawnBabySpiderProb("pod2_spider_spawn", 0.5, 0.5);
            makePod(inst, "pod2_path", -90.0);
            startPod(inst);
            pod2[inst.myPod.cookie] = inst;
            pod2Timeout = 3.0;
        end
    end);
end

local function stopPods12()
    cancelTimeout(pod1Timer);
    local cookies = {};
    for cookie, inst in pairs(pod1) do
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        table.insert(cookies, cookie);
    end
    for _, cookie in pairs(cookies) do
        pod1[cookie] = nil;
    end
    pod1Timeout = 0;
    cancelTimeout(pod2Timer);
    cookies = {};
    for cookie, inst in pairs(pod2) do
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.mySpiders) do
            obj:removeFromParent();
        end
        table.insert(cookies, cookie);
    end
    for _, cookie in pairs(cookies) do
        pod2[cookie] = nil;
    end
    pod2Timeout = 0;
end

function stairs45Down()
    scene.lighting.ambientLight = {0.15, 0.15, 0.15, 1.0};
    startAmbientMusic(true);
    stopPods12();
    cancelTimeout(lanternRespawnTimer);
end

function stairs45Up()
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    startMusic("ambient3.ogg", true);
    startPods12();
    lanternRespawnTimer = addTimeout(0.25, function(cookie)
        scene.respawnPoint = lantern1:getTransform();
    end);
end

-- main

makePodStop("pod1_stop", 2.0, pod1, function()
    pod1Timeout = pod1Timeout + 2.0;
end);

setSensorEnterListener("pod1_kill", false, function(other)
    local inst = pod1[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        pod1[other.cookie] = nil;
    end
end);

makePodStop("pod2_stop", 2.0, pod2, function()
    pod2Timeout = pod2Timeout + 2.0;
end);

setSensorEnterListener("pod2_kill", false, function(other)
    local inst = pod2[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.mySpiders) do
            obj:removeFromParent();
        end
        pod2[other.cookie] = nil;
    end
end);

local spiders = table.copy(scene:getObjects("spider1"),
    scene:getObjects("spider2"), scene:getObjects("spider3"),
    scene:getObjects("spider4"));
for _, obj in pairs(spiders) do
    obj:findSpiderComponent():trapped();
end

scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("lantern1")[1], false);

makeStairs("stairs4", stairs45Down, stairs45Up);
makeStairs("stairs5", stairs45Down, stairs45Up);

makeAirlock("door7", false);

setSensorEnterListener("scary1_cp", false, function(other, self)
    if other.name == "lantern1" then
        self.lanternIn = true;
    elseif other.type == const.SceneObjectTypePlayer then
        if self.lanternIn then
            scene:getObjects("scary1_cp")[1].active = false;
        else
            scene.player.linearVelocity = scene:getObjects("scary1_cp")[1]:getDirection(5.0);
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog98.str1},
            }, function ()
                scene.cutscene = false;
            end);
        end
    end
end, { lanternIn = false });

setSensorEnterListener("scary2_cp", true, function(other)
    scene:getObjects("spider2")[1]:findSpiderComponent():angry();
end);

setSensorEnterListener("scary3_cp", true, function(other)
    scene:getObjects("spider3")[1]:findSpiderComponent():angry();
end);

control2.listener = createSensorEnterListener(true, function(other)
    control2.active = false;
    control2.visible = false;
    control3.active = false;
    control3.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog99.str1},
            {"player", tr.dialog99.str2},
            {"player", tr.dialog99.str3},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);
control3.listener = control2.listener;

makeAirlock("door8", true);

setSensorListener("lantern1_cp", function(other, self)
    if other.name == "lantern1" then
        lantern1Reached1 = true;
    end
end, function (other, self)
    if other.name == "lantern1" then
        lantern1Reached1 = false;
    end
end);

setSensorEnterListener("scary4_cp", true, function(other)
    if not lantern1Reached1 then
        lantern1.pos = scene:getObjects("scary4_cp")[1].pos;
    end
    closeAirlock("door8", true);
end);

control4.listener = createSensorEnterListener(true, function(other)
    control4.active = false;
    control4.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog100.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("scary5_cp", true, function(other)
    control4.active = false;
    control4.visible = false;
    brakeGrate("grate1", false);
    spawnBabySpiderProb("scary5", 0.6, 0.4);
end);

makeAirlock("door9", false);

setSensorEnterListener("scary6_cp", true, function(other)
    scene:getObjects("spider4")[1]:findSpiderComponent():angry();
    spawnBabySpiderProb("scary6", 0.6, 0.4);
end);

setSensorEnterListener("scary7_cp", true, function(other)
    brakeGrate("grate2", false);
    spawnBabySpiderProb("scary7", 0.4, 0.6);
end);

setSensorEnterListener("scary8_cp", true, function(other)
    brakeGrate("grate3", false);
    spawnBabySpiderProb("scary8", 0.4, 0.6);
end);

setSensorEnterListener("scary9_cp", true, function(other)
    addTimeoutOnce(2.0, function()
        audio:playSound("queen_spawn.ogg");
    end);
end);

addTimeout(0.25, function(cookie)
    if powergen1:dead() then
        powergen1Light.visible = false;
    end

    if powergen2:dead() then
        powergen2Light.visible = false;
    end

    if (not powergen1Light.visible) and (not powergen2Light.visible) then
        cancelTimeout(cookie);
        addTimeoutOnce(3.0, function()
            openAirlock("door9", true);
        end);
    end
end);

setSensorEnterListener("scary10_cp", true, function(other)
    addTimeoutOnce(2.0, function()
        fixedSpawn("scary10");
    end);
end);

setSensorEnterListener("scary11_cp", true, function(other)
    spawnBabySpiderProb("scary11", 0.8, 0.2);
end);

setSensorEnterListener("scary12_cp", true, function(other)
    spawnBabySpiderProb("scary12", 0.5, 0.5);
end);

setSensorEnterListener("scary13_cp", true, function(other)
    addTimeoutOnce(1.5, function()
        local spawn = scene:getObjects("scary13_s_gorger1");
        for i = 1, #spawn, 1 do
            summon2(spawn[i], i == 1, function()
                local obj = factory:createGorger1();
                addSpawnedEnemyFreezable(obj, spawn[i]);
                obj:findPhysicsBodyComponent():setFilterGroupIndex(300);
            end);
        end
    end);
end);

addTimeout(0.5, function(cookie)
    if powergen2:alive() then
        return;
    end
    cancelTimeout(cookie);
    spawnBabySpiderProb("scary14", 0.5, 0.5);
end);

makeAirlock("door10", true);

setSensorListener("lantern2_cp", function(other, self)
    if other.name == "lantern1" then
        lantern1Reached2 = true;
    end
end, function (other, self)
    if other.name == "lantern1" then
        lantern1Reached2 = false;
    end
end);

local hbc = queen:findRenderHealthbarComponent();
if hbc ~= nil then
    hbc:removeFromParent();
end

queen:findBossQueenComponent():trapped();

setSensorEnterListener("queen1_cp", true, function(other)
    if not lantern1Reached2 then
        lantern1.pos = scene:getObjects("queen1_cp")[1].pos;
    end
    closeAirlock("door10", true);
    scene:getObjects("queen2_cp")[1].active = true;
end);

setSensorListener("queen2_cp", function(other)
    queen:findBossQueenComponent().autoTarget = true;
    scene.camera:findCameraComponent():zoomTo(55, const.EaseOutQuad, 1.5);
end, function(other)
    queen:findBossQueenComponent().autoTarget = false;
    queen:findBossQueenComponent().target = nil;
    scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
end);

makeAirlock("door11", false);
makeAirlock("door12", false);
makeAirlock("door13", false);
makeAirlock("door14", false);

control5.listener = createSensorEnterListener(true, function(other)
    control5.active = false;
    control5.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog101.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("queen3_cp", false, function(other)
    if not scene.player:findPlayerComponent().inventory:have(const.InventoryItemRedKey) then
        return;
    end
    scene:getObjects("queen3_cp")[1].active = false;
    addTimeoutOnce(1.5, function()
        queen:findBossQueenComponent():angry();
        control5.active = false;
        control5.visible = false;
        local queen3spawn = scene:getObjects("queen3_s_gorger1");
        local queen4spawn = scene:getObjects("queen4_s_gorger1");
        local queen3enemies = {};
        local queen4enemies = {};
        for i = 1, #queen3spawn, 1 do
            table.insert(queen3enemies, factory:createGorger1());
        end
        for i = 1, #queen4spawn, 1 do
            table.insert(queen4enemies, factory:createGorger1());
        end
        addTimeout(0.25, function(cookie)
            for _, obj in pairs(queen3enemies) do
                if obj:alive() then
                    return;
                end
            end
            for _, obj in pairs(queen4enemies) do
                if obj:alive() then
                    return;
                end
            end
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                openAirlock("door13", true);
                openAirlock("door12", false);
                openAirlock("door8", false);
                scene:getObjects("scary4_cp")[1].active = false;
            end);
        end);
        addTimeoutOnce(1.5, function()
            for i = 1, #queen3spawn, 1 do
                summon2(queen3spawn[i], i == 1, function()
                    addSpawnedEnemyFreezable(queen3enemies[i], queen3spawn[i]);
                end);
            end
            addTimeoutOnce(4.0, function()
                for i = 1, #queen4spawn, 1 do
                    summon2(queen4spawn[i], i == 1, function()
                        addSpawnedEnemyFreezable(queen4enemies[i], queen4spawn[i]);
                    end);
                end
            end);
        end);
    end);
end);

setSensorEnterListener("pods1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog102.str1}
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("queen5_cp", true, function(other)
    spawnBabySpiderProb("queen5", 0.5, 0.5);
end);

setSensorEnterListener("pods2_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog103.str1},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);
