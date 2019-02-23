local spawn1 = scene:getObjects("spawn1");
local spawned = {};
local boss = scene:getObjects("boss")[1];
local rodBundles = scene:getObjects("rod_bundle1");
local bossDefeated = false;
local keyl = scene:getObjects("keyl")[1];

local function coreAlert()
    audio:playSound("alarm2.ogg");
    stainedGlass({0.9, 0, 0, 0.4}, {0.0, 0, 0, 0.7}, const.EaseOutQuad, 0.9, function()
        stainedGlass({0.0, 0, 0, 0.7}, {0.0, 0, 0, 0.7}, const.EaseLinear, 0.5, function()
            stainedGlass({0.0, 0, 0, 0.7}, {0.9, 0, 0, 0.4}, const.EaseInQuad, 0.9, coreAlert);
        end);
    end);
end

local function checkSpawnedDead(cookie)
    local c = boss:findBossCoreProtectorComponent();
    if c ~= nil then
        return;
    end
    for _, obj in pairs(spawned) do
        if obj:alive() then
            return;
        end
    end
    cancelTimeout(cookie);
    addTimeoutOnce(1.0, function()
        startMusic("action9.ogg", true);
        scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 3.0);
        if not scene.player:findPlayerComponent().flagDamagedByCoreProtectorMelee then
            gameShell:setAchieved(const.AchievementFisticuffs);
        end
        stainedGlass({0.0, 0, 0, 0.0}, {0.9, 0, 0, 0.4}, const.EaseInQuad, 1.0, coreAlert);
        addTimeoutOnce(5.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog75.str1},
            }, function ()
                scene.cutscene = false;
                addTimeoutOnce(2.0, function()
                    scene.player.linearDamping = 6.0;
                    scene.cutscene = true;
                    showLowerDialog(
                    {
                        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog76.str1},
                        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog76.str2},
                    }, function ()
                        scene.cutscene = false;
                        addTimeoutOnce(1.0, function()
                            bridgeOpen(bridge2, 10.0, false);
                            addTimeoutOnce(1.0, function()
                                showUpperMsg(5.0, {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog77.str1}, nil);
                                scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal4")[1].pos);
                                scene:getObjects("door9_cp")[1].active = false;
                                closeDoor("door9", false);
                                closeDoor("door15", false);
                                scene:getObjects("toxic1_cp")[1].active = true;
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end

local function spawnFunc(cookie)
    if bossDefeated then
        cancelTimeout(cookie);
        return;
    end
    for i = 1, 2, 1 do
        if (spawned[i] == nil) or spawned[i]:dead() then
            table.shuffle(spawn1);
        end
    end
    for i = 1, 2, 1 do
        if (spawned[i] == nil) or spawned[i]:dead() then
            local s = spawn1[i];
            if math.random(1, 4) == 4 then
                spawned[i] = factory:createWarder();
            else
                spawned[i] = factory:createEnforcer1();
            end
            addTimeoutOnce(10.0, function()
                if bossDefeated then
                    spawned[i] = nil;
                    return;
                end
                summon1(s, true, function()
                    addSpawnedEnemy(spawned[i], s);
                end);
            end);
        end
    end
end

local function checkRodBundle(cookie, dt, bundle)
    if bundle:alive() then
        return;
    end
    bundle:findRenderHealthbarComponent():removeFromParent();
    cancelTimeout(cookie);
    for i = 1, 4, 1 do
        addTimeoutOnce((i - 1) * 0.4, function()
            local obj = factory:createExplosion1(const.zOrderExplosion);
            obj.pos = bundle.pos + vec2(math.random() * 6.0 - 3.0, math.random() * 6.0 - 3.0);
            scene:addObject(obj);
        end);
    end
    addTimeoutOnce(1.0, function()
        bundle:findLightComponent():removeFromParent();
    end);
    if bossDefeated then
        return;
    end
    for _, obj in pairs(rodBundles) do
        if obj:alive() then
            return;
        end
    end
    bossDefeated = true;
    addTimeoutOnce(1.5, function()
        boss:findBossCoreProtectorComponent():setDie();
        addTimeout(0.5, checkSpawnedDead);
    end);
end

-- main

setSensorEnterListener("boss2_cp", true, function()
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.respawnPoint = scene:getObjects("boss2_cp")[1]:getTransform();
    bridgeClose(bridge1, 15.0, false, nil);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog78.str1},
            {"player", tr.dialog78.str2},
        }, function ()
            scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.0);
            boss:setActiveRecursive(true);
            boss:setVisibleRecursive(true);
            local c = boss:findBossCoreProtectorComponent();
            c:setArena(scene:getObjects("arena0")[1].pos, scene:getObjects("boss_start")[1].pos, scene:getObjects("boss_die")[1].pos);
            startMusic("action8.ogg", true);
            addTimeoutOnce(3.0, function()
                c:startAngry();
                scene.cutscene = false;
                for _, obj in pairs(rodBundles) do
                    obj.life = obj.maxLife;
                    local hb = RenderHealthbarComponent(vec2(0.0, 5.5), 0, 10.0, 1.5, const.zOrderMarker + 1);
                    hb.color = { 1.0, 1.0, 1.0, 0.6 };
                    obj:addComponent(hb);
                    addTimeout0(checkRodBundle, obj);
                end
                addTimeoutOnce(3.0, function()
                    c.autoTarget = true;
                    addTimeout(0.5, spawnFunc);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("toxic1_cp", true, function()
    scene:getObjects("keyl1_cp")[1].active = false;
    keyl:setTransform(scene:getObjects("keyl_final_pos")[1]:getTransform());
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(keyl, const.EaseOutQuad, 1.5);
        addTimeoutOnce(2.0, function()
            showLowerDialog(
            {
                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog79.str1},
            }, function ()
                openDoor("door14", true);
                addTimeoutOnce(1.5, function()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                    addTimeoutOnce(3.0, function()
                        scene.camera:findCameraComponent():follow(keyl, const.EaseOutQuad, 1.5);
                        addTimeoutOnce(2.0, function()
                            showLowerDialog(
                            {
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog80.str1},
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog80.str2},
                                {"player", tr.dialog80.str3},
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog80.str4},
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog80.str5},
                            }, function ()
                                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                                scene.cutscene = false;
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("toxic2_cp", true, function()
    completeLevel(1.5, tr.str30, tr.str31, "e1m7");
end);
