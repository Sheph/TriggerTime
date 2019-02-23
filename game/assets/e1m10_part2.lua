local carl = nil;
local carlOpenDoor = false;
local shuttle = scene:getObjects("shuttle1")[1];

local function redAlert()
    audio:playSound("alarm2.ogg");
    stainedGlass({0.9, 0, 0, 0.5}, {0.9, 0, 0, 0.5}, const.EaseLinear, 0.15, function()
        stainedGlass({0.9, 0, 0, 0.5}, {0.0, 0, 0, 0.0}, const.EaseOutQuad, 0.9, function()
            stainedGlass({0.0, 0, 0, 0.0}, {0.0, 0, 0, 0.0}, const.EaseLinear, 0.5, function()
                stainedGlass({0.0, 0, 0, 0.0}, {0.9, 0, 0, 0.5}, const.EaseInQuad, 0.9, redAlert);
            end);
        end);
    end);
end

local function makeExitDoor()
    local doorObj = scene:getObjects("red_door")[1];
    local phObj = scene:getObjects("red_key_ph")[1];
    local phC = phObj:findPlaceholderComponent();

    phC.listener = createSensorEnterListener(true, function(other)
        phObj:removeFromParent();
        doorObj:findRenderQuadComponents("key")[1].visible = true;
        audio:playSound("key.ogg");
        openAirlock("door5", true);
        scene:getObjects("carl2_cp")[1].active = true;
    end);
end

local function explodeTube(tubeName, holeName, laserName)
    local tubeObj = scene:getObjects(tubeName)[1];
    local lObj = scene:getObjects(laserName)[1];
    local e = factory:createExplosion1(const.zOrderExplosion);
    e.pos = lObj.pos;
    scene:addObject(e);
    tubeObj:findRenderQuadComponents(holeName)[1].visible = true;
    lObj.active = true;
    addTimeoutOnce(0.25, function()
        lObj.visible = true;
    end);
end

local function explodeFire(name)
    local objs = scene:getObjects(name);
    for i = 1, #objs, 1 do
        addTimeoutOnce(0.4 * (i - 1), function()
            local e = factory:createExplosion1(const.zOrderExplosion);
            e.pos = objs[i].pos;
            scene:addObject(e);
            objs[i].visible = true;
        end);
    end
end

function startCarl(shakeStream, immediate)
    addTimeoutOnce(1.0, function()
        stainedGlass({0.0, 0, 0, 0.0}, {0.9, 0, 0, 0.5}, const.EaseInQuad, 1.0, redAlert);
    end);
    scene.camera:findCameraComponent():tremor(false);
    scene.camera:findCameraComponent():tremorStart(0.3);
    carl = factory:createScientist();
    carl:setTransform(scene:getObjects("carl0")[1]:getTransform());
    scene:addObject(carl);
    if immediate then
        scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0.0);
        openAirlock("door5", false);
        return;
    end
    scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 3.0);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog135.str1},
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog135.str2},
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog135.str3},
        }, function ()
            scene.cutscene = false;
            local p = scene:getObjects("carl0_path")[1];
            carl.roamBehavior:reset();
            carl.roamBehavior.linearVelocity = 12.0;
            carl.roamBehavior.linearDamping = 4.0;
            carl.roamBehavior.dampDistance = 3.0;
            carl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            carl.roamBehavior:start();
            scene:getObjects("carl1_cp")[1].active = true;
        end);
    end);
end

-- main

shuttle:findParticleEffectComponent().visible = false;

makeAirlock("door6", true);

makeExitDoor();

local function carlOpenDoorFunc()
    scene:getObjects("carl1_cp")[1].active = false;
    local p = scene:getObjects("carl1_path")[1];
    carl.roamBehavior.linearVelocity = 12.0;
    carl.roamBehavior.linearDamping = 4.0;
    carl.roamBehavior.dampDistance = 3.0;
    carl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    carl.roamBehavior:start();
end

setSensorEnterListener("carl0_cp", true, function(other)
    if carlOpenDoor then
        carlOpenDoorFunc();
    end
    carlOpenDoor = true;
end);

setSensorListener("carl1_cp", function(other)
    if carlOpenDoor then
        carlOpenDoorFunc();
    end
    carlOpenDoor = true;
end, function (other)
    carlOpenDoor = false;
end);

setSensorEnterListener("carl2_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        carl.angle = (scene.player.pos - carl.pos):angle();
        showLowerDialog(
        {
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog136.str1},
            {"player", tr.dialog136.str2},
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog136.str3},
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog136.str4},
            {"ally", tr.str4, "factory2/portrait_wounded1.png", tr.dialog136.str5},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("exit1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit1_cp")[1]:getTransform();
    explodeTube("pipe6", "hole1", "laser10");
end);

setSensorEnterListener("exit2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit2_cp")[1]:getTransform();
    explodeTube("pipe6", "hole2", "laser11");
end);

setSensorEnterListener("exit3_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit3_cp")[1]:getTransform();
    explodeTube("pipe6", "hole4", "laser13");
    addTimeoutOnce(0.5, function()
        explodeTube("pipe6", "hole3", "laser12");
    end);
end);

setSensorEnterListener("exit4_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit4_cp")[1]:getTransform();
    explodeTube("pipe6", "hole6", "laser15");
    addTimeoutOnce(0.5, function()
        explodeTube("pipe6", "hole5", "laser14");
    end);
    bg:removeFromParent();
    bg = factory:createBackground("metal5.png", 24.0, 24.0, vec2(0.8, 0.8), const.zOrderBackground);
    scene:addObject(bg);
end);

setSensorEnterListener("exit5_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit5_cp")[1]:getTransform();
    explodeTube("pipe6", "hole7", "laser16");
end);

setSensorEnterListener("exit6_cp", true, function(other)
    local objs = scene:getObjects("boom1");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setBlast(nil, 550000.0, 0.0, { const.SceneObjectTypeRock });
        scene:addObject(e);
    end
end);

setSensorEnterListener("exit7_cp", true, function(other)
    explodeFire("fire10");
end);

setSensorEnterListener("exit8_cp", true, function(other)
    explodeFire("fire11");
end);

setSensorEnterListener("exit9_cp", true, function(other)
    local objs = scene:getObjects("boom2");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        scene:addObject(e);
    end
    scene:getJoints("wall_part0_joint")[1].motorSpeed = -scene:getJoints("wall_part0_joint")[1].motorSpeed;
    scene:getJoints("wall_part1_joint")[1].motorSpeed = -scene:getJoints("wall_part1_joint")[1].motorSpeed;
end);

setSensorEnterListener("exit10_cp", true, function(other)
    local objs = scene:getObjects("boom3");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        scene:addObject(e);
    end
    scene:getJoints("wall_part2_joint")[1].motorSpeed = -scene:getJoints("wall_part2_joint")[1].motorSpeed;
end);

setSensorEnterListener("exit11_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit11_cp")[1]:getTransform();
    local objs = scene:getObjects("boom4");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        scene:addObject(e);
    end
    scene:getJoints("wall_part3_joint")[1].motorSpeed = -scene:getJoints("wall_part3_joint")[1].motorSpeed;
end);

setSensorListener("stones_cp", function(other, self)
    if self.objs[other.cookie] == nil then
        self.objs[other.cookie] = { count = 1, obj = other };
    else
        self.objs[other.cookie].count = self.objs[other.cookie].count + 1;
    end
end, function (other, self)
    self.objs[other.cookie].count = self.objs[other.cookie].count - 1;
    if self.objs[other.cookie].count == 0 then
        if other.name == "debris3" then
            local c = other:findPhysicsBodyComponent();
            c.filterMaskBits = c.filterMaskBits + const.collisionBitCustom2;
            self.objs[other.cookie] = nil;
            other.name = "debris3_mod";
        end
    end
end, { objs = {} });

setSensorEnterListener("exit12_cp", true, function(other)
    local objs = scene:getObjects("boom5");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setBlast(nil, 2000.0, 0.0, { const.SceneObjectTypeRock });
        scene:addObject(e);
    end
    local objs = scene:getObjects("boom5b");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setSound(nil);
        scene:addObject(e);
    end
end);

setSensorEnterListener("exit13_cp", true, function(other)
    local objs = scene:getObjects("boom6");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setBlast(nil, 5000.0, 0.0, { const.SceneObjectTypeRock });
        scene:addObject(e);
    end
    local objs = scene:getObjects("boom6b");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setSound(nil);
        scene:addObject(e);
    end
end);

setSensorEnterListener("exit14_cp", true, function(other)
    local objs = scene:getObjects("boom7");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setBlast(nil, 5000.0, 0.0, { const.SceneObjectTypeRock });
        scene:addObject(e);
    end
    local objs = scene:getObjects("boom7b");
    for _, obj in pairs(objs) do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = obj.pos;
        e:findExplosionComponent():setSound(nil);
        scene:addObject(e);
    end
    addTimeoutOnce(0.5, function()
        local objs = scene:getObjects("boom8");
        for _, obj in pairs(objs) do
            local e = factory:createExplosion1(const.zOrderExplosion);
            e.pos = obj.pos;
            e:findExplosionComponent():setBlast(nil, 5000.0, 0.0, { const.SceneObjectTypeRock });
            scene:addObject(e);
        end
        local objs = scene:getObjects("boom8b");
        for _, obj in pairs(objs) do
            local e = factory:createExplosion1(const.zOrderExplosion);
            e.pos = obj.pos;
            e:findExplosionComponent():setSound(nil);
            scene:addObject(e);
        end
    end);
end);

setSensorEnterListener("exit15_cp", true, function(other)
    local wasTheEnd = gameShell:isAchieved(const.AchievementTheEnd);

    gameShell:setAchieved(const.AchievementTheEnd);

    if (not wasTheEnd) and (settings.skill <= const.SkillEasy) then
        gameShell:setAchieved(const.AchievementChicken);
    end

    if (settings.skill >= const.SkillHard) then
        gameShell:setAchieved(const.AchievementHardcore);
    end

    bg:removeFromParent();

    local scl = 5.0;

    bg = factory:createBackground("ground1.png", 10, 10, vec2(0.2, 0.2), const.zOrderBackground);
    bg:findRenderBackgroundComponent().color = {0.6, 0.6, 0.6, 1.0};
    scene:addObject(bg);

    bg = factory:createBackground("fog.png", 544 / scl / 2, 416 / scl / 2, vec2(10.0, 1.0), const.zOrderBackground + 1)
    bg:findRenderBackgroundComponent().unbound = true;
    bg:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.6};
    scene:addObject(bg);

    bg = factory:createBackground("fog.png", 544 / scl / 1.5, 416 / scl / 1.5, vec2(15.0, 1.0), const.zOrderBackground + 2)
    bg:findRenderBackgroundComponent().unbound = true;
    bg:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.7};
    bg:findRenderBackgroundComponent().offset = vec2(0, 416 / scl / 1.5 / 2);
    scene:addObject(bg);

    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("exit1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 15.0;
    scene.player.roamBehavior.linearDamping = 5.0;
    scene.player.roamBehavior.dampDistance = 3.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 3.0);
    end);
end);

setSensorEnterListener("exit16_cp", true, function(other)
    addTimeoutOnce(0.5, function()
        local p = scene:getObjects("exit2_path")[1];
        scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    end);
end);

setSensorEnterListener("exit17_cp", true, function(other)
    scene.camera:findCameraComponent():follow(shuttle, const.EaseOutQuad, 1.0);
    addTimeoutOnce(1.0, function()
        scene.player.pos = scene:getObjects("target7")[1].pos;
    end);
    addTimeoutOnce(3.0, function()
        local e = scene:getObjects("exhaust0")[1];
        e:findParticleEffectComponent():allowCompletion();
        e:findLightComponent():removeFromParent();
        e = scene:getObjects("exhaust1")[1];
        e:findParticleEffectComponent():resetEmit();
        e.visible = true;
    end);
    addTimeoutOnce(5.0, function()
        local objs = scene:getObjects("boom11");
        local to = 0;
        for n = 1, 6, 1 do
            for i = 1, #objs, 1 do
                addTimeoutOnce(to, function()
                    local e = factory:createExplosion1(const.zOrderExplosion);
                    e.pos = objs[i].pos;
                    scene:addObject(e);
                end);
                to = to + 0.2 + math.random() * 0.3;
            end
        end
    end);
    addTimeoutOnce(6.0, function()
        local e = scene:getObjects("exhaust1")[1];
        e:findParticleEffectComponent():allowCompletion();
        e:findLightComponent():removeFromParent();
        shuttle:findParticleEffectComponent():resetEmit();
        shuttle:findParticleEffectComponent().visible = true;
        shuttle:findLightComponent():getLights()[1].visible = true;

        local tmp = scene:getObjects("shuttle1")[1];
        local rc = tmp:findRenderQuadComponent();
        local qpc = QuadPulseComponent(rc, vec2(0, 0), 0.4, const.EaseInOutQuad, 0.98,
            0.4, const.EaseInOutQuad, 1.02);
        qpc.t = 0;
        tmp:addComponent(qpc);

        local vel = 5;
        addTimeout0(function(cookie, dt)
            vel = vel + dt * 10;
            shuttle.linearVelocity = vec2(vel, 0);
            if vel >= 60 then
                cancelTimeout(cookie);
            end
        end);
        completeLevel(5.0, tr.str34, tr.str35, "e1m11", true);
    end);
end);

setSensorEnterListener("exit18_cp", true, function(other)
    local objs = scene:getObjects("boom9");
    for i = 1, #objs, 1 do
        local e = factory:createExplosion1(const.zOrderExplosion);
        e.pos = objs[i].pos;
        if i ~= 1 then
            e:findExplosionComponent():setSound(nil);
        end
        scene:addObject(e);
    end
    addTimeoutOnce(1.0, function()
        local objs = scene:getObjects("boom10");
        local to = 0;
        for n = 1, 6, 1 do
            for i = 1, #objs, 1 do
                addTimeoutOnce(to, function()
                    local e = factory:createExplosion1(const.zOrderExplosion);
                    e.pos = objs[i].pos;
                    scene:addObject(e);
                end);
                to = to + 0.2 + math.random() * 0.3;
            end
        end
    end);
end);
