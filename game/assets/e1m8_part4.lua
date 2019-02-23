local pod5Timer = nil;
local pod5GGTimer = nil;
local pod5Spawn = scene:getObjects("pod5_spawn")[1];
local pod5 = {};
local pod6Timer = nil;
local pod6Spawn = scene:getObjects("pod6_spawn")[1];
local pod6 = {};
local playerBlocker1 = scene:getObjects("player_blocker1")[1];
local playerBlocker2 = scene:getObjects("player_blocker2")[1];
local playerBlocker3 = scene:getObjects("player_blocker3")[1];
local pod5SpiderSpawn =  scene:getObjects("pod5_spider_spawn");
local scientist1 = scene:getObjects("scientist1")[1];
local respawnTimer = nil;
local boss = scene:getObjects("boss")[1];
local display1 = nil;
local display2 = nil;

function startFinalPods()
    local rideTurn = 0;
    local function spawnPod(cookie)
        local inst = scene:instanciate("e1m8_pod.json", pod5Spawn:getTransform());
        makePod(inst, "pod5_path", -90.0, true, function(other)
            if other.type == const.SceneObjectTypePlayer then
                playerBlocker1.active = false;
                scene.camera:findCameraComponent():zoomTo(50, const.EaseInOutQuad, 1.0);
            end
        end, function(other)
            if other.type == const.SceneObjectTypePlayer then
                playerBlocker1.active = true;
                scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 1.0);
            end
        end);
        startPod(inst);
        pod5[inst.myPod.cookie] = inst;
        inst.mySpiders = {};
        if rideTurn ~= 0 then
            rideTurn = rideTurn - 1;
            if math.random(1, 3) <= 2 then
                for _, obj in pairs(pod5SpiderSpawn) do
                    local e;
                    if math.random(1, 2) == 1 then
                        e = factory:createBabySpider1();
                    else
                        e = factory:createBabySpider2();
                    end
                    addSpawnedEnemyFreezable(e, obj);
                    table.insert(inst.mySpiders, e);
                end
            end
        else
            makePodUsable(inst);
            rideTurn = math.random(1, 3);
        end
    end
    spawnPod(0);
    pod5Timer = addTimeout(3.3, spawnPod);
    pod5GGTimer = addTimeout0(function(cookie)
        local ho = scene.player:findPlayerComponent().altWeapon.heldObject;
        if ho == nil then
            return;
        end
        for _, pod in pairs(pod5) do
            if (pod.myHatchJoint ~= nil) and (pod.myHatch == ho) then
                pod.myHatchJoint:remove();
                pod.myHatchJoint = nil;
                pod.myHatch:addComponent(FadeOutComponent(1.0));
                break;
            end
        end
    end);
    addTimeoutOnce(1.8, function()
        pod6Timer = addTimeout(3.3, function(cookie)
            local inst = scene:instanciate("e1m8_pod.json", pod6Spawn:getTransform());
            makePod(inst, "pod6_path", 90.0, true);
            startPod(inst);
            pod6[inst.myPod.cookie] = inst;
            inst.myBeetles = {};
            if math.random(1, 3) <= 2 then
                inst.myBeetles = spawnBeetleProb("pod6_beetle_spawn", 0.5, 0.5, 0.5, 0.5, vec2(0, 0));
                for _, obj in pairs(inst.myBeetles) do
                    obj:findBeetleComponent():setTrapped();
                    obj:findBeetleComponent().autoTarget = true;
                end
            end
        end);
    end);
end

local function stopFinalPods()
    cancelTimeout(pod5Timer);
    cancelTimeout(pod5GGTimer);
    local cookies = {};
    for cookie, inst in pairs(pod5) do
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
        pod5[cookie] = nil;
    end
    cancelTimeout(pod6Timer);
    local cookies = {};
    for cookie, inst in pairs(pod6) do
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
        pod6[cookie] = nil;
    end
end

local function setupDisplay(name)
    local rc = scene:getObjects("terrain5")[1]:findRenderProjComponents(name)[1];
    local display = AnimationComponent(rc.drawable);
    display:addAnimation(const.AnimationDefault, "tv_def", 1);
    display:addAnimation(const.AnimationDefault + 1, "tv_natan", 1);
    display:startAnimation(const.AnimationDefault);
    scene:getObjects("terrain5")[1]:addComponent(display);

    return display;
end

function introCutscene(fn)
    display1:startAnimation(const.AnimationDefault + 1);
    addTimeoutOnce(1.5, function()
        showUpperDialog(
        {
            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog111.str1},
            {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog111.str2},
        }, function ()
            scene.camera:findCameraComponent():follow(scene:getObjects("intro_target2")[1], const.EaseOutQuad, 1.0);
            addTimeoutOnce(2.0, function()
                showUpperDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog112.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog112.str2},
                }, function ()
                    openAirlock("door32", true);
                    addTimeoutOnce(1.0, function()
                        local lab = scene:getObjects("intro_lab");
                        local p = scene:getObjects("intro_lab_path")[1];
                        for _, obj in pairs(lab) do
                            obj.roamBehavior:reset();
                            obj.roamBehavior.linearVelocity = 10.0;
                            obj.roamBehavior.linearDamping = 6.0;
                            obj.roamBehavior.dampDistance = 2.0;
                            obj.roamBehavior:changePath(p:findPathComponent().path, obj:getTransform());
                            obj.roamBehavior:start();
                        end
                    end);
                    addTimeoutOnce(2.0, function()
                        local skull = scene:getObjects("intro_skull")[1];
                        local p = scene:getObjects("intro_skull_path")[1];
                        skull.roamBehavior:reset();
                        skull.roamBehavior.linearVelocity = 10.0;
                        skull.roamBehavior.linearDamping = 6.0;
                        skull.roamBehavior.dampDistance = 2.0;
                        skull.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        skull.roamBehavior:start();
                    end);
                    addTimeoutOnce(5.0, function()
                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 1.0, function()
                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 1.0);
                            local lab = scene:getObjects("intro_lab");
                            for _, obj in pairs(lab) do
                                obj:removeFromParent();
                            end;
                            scene:getObjects("intro_skull")[1]:removeFromParent();
                            display1:startAnimation(const.AnimationDefault);
                            fn();
                        end);
                    end);
                end);
            end);
        end);
    end);
end

-- main

makeLever("lever2", false);

addTimeoutOnce(1.0, function()
    disableLever("lever2");
end);

setSensorEnterListener("pod5_a", false, function(other)
    local inst = pod5[other.cookie];
    if (inst ~= nil) and (inst.myBlocker ~= nil) then
        inst.myBlocker.active = false;
        inst.myBlockerJoint:remove();
    end
end);

setSensorEnterListener("pod5_b", false, function(other)
    local inst = pod5[other.cookie];
    if (inst ~= nil) and (inst.myBlocker ~= nil) and (not inst.myBlocker.active) then
        inst.myBlocker.active = true;
        inst.myBlocker:setTransform(inst.myPod:getTransform());
        inst.myBlockerJoint = scene:addWeldJoint(inst.myPod, inst.myBlocker, inst.myPod:getWorldPoint(vec2(0, 0)), false);
        if playerBlocker1.active == false then
            addTimeoutOnce(0.3, function()
                for _, v in pairs(inst.myPod.objs) do
                    if v.obj == scene.player then
                        playerBlocker2.active = false;
                        respawnTimer = addTimeout0(function(cookie, dt)
                            scene.respawnPoint = inst.myPod:getTransform();
                        end);
                        break;
                    end
                end
            end);
        end
    end
end);

setSensorEnterListener("pod5_kill", false, function(other)
    local inst = pod5[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.mySpiders) do
            obj:removeFromParent();
        end
        pod5[other.cookie] = nil;
    end
end);

setSensorEnterListener("pod6_kill", false, function(other)
    local inst = pod6[other.cookie];
    if inst ~= nil then
        disablePod(inst);
        for _, obj in pairs(inst.objects) do
            obj:removeFromParent();
        end
        for _, obj in pairs(inst.myBeetles) do
            obj:removeFromParent();
        end
        pod6[other.cookie] = nil;
    end
end);

setSensorEnterListener("ride1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("ride1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 10.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("ride2_cp", true, function(other)
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    addTimeoutOnce(1.0, function()
        scene.player.roamBehavior:reset();
        scientist1.angle = (scene.player.pos - scientist1.pos):angle();
        addTimeoutOnce(1.0, function()
            showLowerDialog(
            {
                {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog113.str1},
                {"player", tr.dialog113.str2},
                {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog113.str3},
                {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog113.str4},
                {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog113.str5},
                {"player", tr.dialog113.str6},
            }, function ()
                local obj = factory:createSummon1(true, const.zOrderExplosion);
                obj:setTransform(scientist1:getTransform());
                scene:addObject(obj);
                addTimeoutOnce(2.0, function()
                    scientist1:removeFromParent();
                end);
                addTimeoutOnce(4.0, function()
                    showLowerDialog(
                    {
                        {"player", tr.dialog114.str1},
                        {"player", tr.dialog114.str2},
                        {"player", tr.dialog114.str3},
                    }, function ()
                        scene.cutscene = false;
                    end);
                end);
            end);
        end);
    end);
end);

makeAirlock("door26", false);
makeAirlock("door27", false);
makeAirlock("door28", false);
makeAirlock("door29", false);
makeAirlock("door30", false);
makeAirlock("door31", false);

setSensorEnterListener("ride3_cp", true, function(other)
    local enforcer3 = scene:getObjects("enforcer3")[1];
    local p = scene:getObjects("enforcer3_path")[1];
    enforcer3.roamBehavior:reset();
    enforcer3.roamBehavior.linearVelocity = 10.0;
    enforcer3.roamBehavior.linearDamping = 6.0;
    enforcer3.roamBehavior.dampDistance = 2.0;
    enforcer3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    enforcer3.roamBehavior:start();
end);

setSensorEnterListener("ride4_cp", true, function(other)
    local sentries = scene:getObjects("ride_sentry1");
    for _, obj in pairs(sentries) do
        obj:findSentryComponent().autoTarget = true;
        obj:findSentryComponent():unfold();
    end
end);

local podHatch = scene:getInstances("pod_hatch");
for _, inst in pairs(podHatch) do
    findObject(inst.objects, "pod_mover"):removeFromParent();
    local pod = findObject(inst.objects, "pod");
    local podHatch = findObject(inst.objects, "pod_hatch");
    pod.sleepingAllowed = true;
    podHatch.sleepingAllowed = true;
    pod:findPhysicsBodyComponent():setFilterGroupIndex(0);
    podHatch:findPhysicsBodyComponent():setFilterGroupIndex(0);
end

local podNoHatch = scene:getInstances("pod_no_hatch");
for _, inst in pairs(podNoHatch) do
    findObject(inst.objects, "pod_mover"):removeFromParent();
    findObject(inst.objects, "pod_hatch"):removeFromParent();
    local pod = findObject(inst.objects, "pod");
    pod.sleepingAllowed = true;
    pod:findPhysicsBodyComponent():setFilterGroupIndex(0);
end

setSensorEnterListener("ride5_cp", false, function(other)
    local inst = pod5[other.cookie];
    if (inst ~= nil) and inst.myBlocker and inst.myBlocker.active then
        for _, v in pairs(inst.myPod.objs) do
            if v.obj == scene.player then
                if respawnTimer ~= nil then
                    cancelTimeout(respawnTimer);
                    respawnTimer = nil;
                end
                scene.respawnPoint = scene:getObjects("ride6_cp")[1]:getTransform();
                inst.myBlocker.active = false;
                playerBlocker3.active = false;
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                local p = scene:getObjects("ride5_path")[1];
                scene.player.roamBehavior:reset();
                scene.player.roamBehavior.linearVelocity = settings.player.moveSpeed;
                scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                scene.player.roamBehavior:start();
                break;
            end
        end
    end
end);

setSensorEnterListener("ride6_cp", true, function(other)
    scene:getObjects("ride5_cp")[1].active = false;
    addTimeoutOnce(0.5, function()
        scene.player.roamBehavior:reset();
        scene.cutscene = false;
        playerBlocker3.active = true;
    end);
end);

makeKeyDoor("yellow_door2", "yellow_key_ph2", function()
    openAirlock("door31", true);
end);

setSensorEnterListener("boss1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("boss1_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local laser4 = scene:getObjects("laser4");
    for _, obj in pairs(laser4) do
        obj.active = true;
        obj.visible = true;
    end
    local laser5 = scene:getObjects("laser5");
    for _, obj in pairs(laser5) do
        obj.active = true;
        obj.visible = true;
    end
    stopFinalPods();
    local c = boss:findTargetableComponent();
    addTimeoutOnce(2.0, function()
        startMusic("action13.ogg", true);
        c.target = scene:getObjects("boss_target1")[1];
        c.patrol = true;
        scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
        scene.camera:findCameraComponent():zoomTo(50, const.EaseInOutQuad, 1.5);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.5);
        addTimeoutOnce(3.0, function()
            showLowerDialog(
            {
                {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog115.str1},
                {"player", tr.dialog115.str2},
                {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog115.str3},
                {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog115.str4},
            }, function ()
                scene.cutscene = false;
                addTimeout0(function(cookie, dt)
                    if boss:alive() then
                        return;
                    end
                    cancelTimeout(cookie);
                    c.autoTarget = false;
                    c.target = scene:getObjects("boss_target2")[1];
                    c.patrol = true;
                    scene:getObjects("boss2_cp")[1].active = true;
                end);
                addTimeoutOnce(1.0, function()
                    c.patrol = false;
                    c.autoTarget = true;
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("boss2_cp", true, function(other)
    boss:findTargetableComponent().target = scene:getObjects("boss_target3")[1];
    scene:getObjects("boss3_cp")[1].active = true;
end);

setSensorEnterListener("boss3_cp", true, function(other)
    boss.linearVelocity = vec2(0, 0);
    boss.angularVelocity = 0;
    boss:setTransform(scene:getObjects("boss_target5")[1]:getTransform());
    boss:findTargetableComponent().target = scene:getObjects("boss_target4")[1];
    boss.maxLife = boss.maxLife * 0.75;
    boss.life = boss.maxLife;
    boss:findBossChopperComponent().canDie = true;
    audio:playSound("alarm_off.ogg");
    local laser5 = scene:getObjects("laser5");
    for _, obj in pairs(laser5) do
        obj:addComponent(FadeOutComponent(1.0));
    end
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_3")[1].pos);
    scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.5);
    scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("boss_target4")[1].pos);
end);

setSensorEnterListener("boss4_cp", true, function(other)
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_4")[1].pos, scene:getObjects("constraint1_3")[1].pos);
    scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.5);
    scene.respawnPoint = scene:getObjects("boss4_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local laser6 = scene:getObjects("laser6");
    for _, obj in pairs(laser6) do
        obj.active = true;
        obj.visible = true;
    end
    local laser7 = scene:getObjects("laser7");
    for _, obj in pairs(laser7) do
        obj.active = true;
        obj.visible = true;
    end
    local c = boss:findTargetableComponent();
    addTimeoutOnce(3.0, function()
        scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("boss_target4")[1].pos);
        showLowerDialog(
        {
            {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog116.str1},
        }, function ()
            scene.cutscene = false;
            addTimeoutOnce(1.0, function()
                c.patrol = false;
                c.autoTarget = true;
            end);
            addTimeout0(function(cookie, dt)
                if boss:scene() ~= nil then
                    return;
                end
                cancelTimeout(cookie);
                addTimeoutOnce(1.0, function()
                    startMusic("ambient12.ogg", true);
                    scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
                    scene.camera:findCameraComponent():zoomTo(35.0, const.EaseInOutQuad, 1.5);
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.5);
                    gameShell:setAchieved(const.AchievementDieAlready);
                    addTimeoutOnce(2.0, function()
                        scene.player.linearDamping = 6.0;
                        scene.cutscene = true;
                        addTimeoutOnce(1.0, function()
                            showLowerDialog(
                            {
                                {"player", tr.dialog117.str1},
                            }, function ()
                                scene.cutscene = false;
                                scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("exit_target1")[1].pos);
                                audio:playSound("alarm_off.ogg");
                                local laser7 = scene:getObjects("laser7");
                                for _, obj in pairs(laser7) do
                                    obj:addComponent(FadeOutComponent(1.0));
                                end
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

display1 = setupDisplay("display1");
display2 = setupDisplay("display2");
makeAirlock("door32", false);

setSensorEnterListener("exit_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog118.str1},
        }, function ()
            completeLevel(1.0, tr.str32, tr.str33, "e1m9");
        end);
    end);
end);
