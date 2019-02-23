local gun2 = scene:getInstances("gun2")[1];
local gun3 = scene:getInstances("gun3")[1];
local gun4 = scene:getInstances("gun4")[1];
local hallEntered = false;
local rod1 = scene:getObjects("door6_rod1")[1];
local rod2 = scene:getObjects("door6_rod2")[1];
local rod3 = scene:getObjects("door6_rod3")[1];
local door6Light1 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door6_light1")[1];
local door6Light2 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door6_light2")[1];
local door6Light3 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door6_light3")[1];
local skull = scene:getObjects("skull0")[1];
local skullTarget = scene:getObjects("skull0_target")[1];
local guardian1 = scene:getObjects("guardian1");

local function activateLight(light)
    light.visible = true;
    light.myColor = light.color[4];
    local tweening = SequentialTweening(true);
    tweening:addTweening(SingleTweening(0.4, const.EaseOutQuad, light.color[4], 1.0, false));
    tweening:addTweening(SingleTweening(0.4, const.EaseInQuad, 1.0, light.color[4], false));
    light.myTimer = addTimeout0(function(cookie, dt, self)
        local c = light.color;
        c[4] = tweening:getValue(self.t);
        light.color = c;
        self.t = self.t + dt;
    end, { t = 0 });
end

local function deactivateLight(light)
    if light.myTimer ~= nil then
        cancelTimeout(light.myTimer);
        light.myTimer = nil;
        local c = light.color;
        c[4] = light.myColor;
        light.color = c;
        light.visible = false;
    end
end

local function coolRod(rod, fn, ...)
    local rc = rod:findRenderQuadComponent();
    local t = 0;
    addTimeout0(function(cookie, dt, args)
        t = t + dt;
        if t > 2 then
            t = 2;
        end
        rc.color = {1, t / 2, t / 2, 1};
        if t == 2 then
            cancelTimeout(cookie);
            fn(unpack2(args));
        end
    end, pack2(...));
end

local function spawnBots(name, scorpProb)
    local spawn = scene:getObjects(name.."_bots1");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 1 / scorpProb) == 1 then
            e = factory:createScorp3();
        else
            e = factory:createTetrocrab3();
        end
        addSpawnedEnemyFreezable(e, obj);
    end
    spawn = scene:getObjects(name.."_bots2");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 1 / scorpProb) == 1 then
            e = factory:createScorp4();
        else
            e = factory:createTetrocrab4();
        end
        addSpawnedEnemyFreezable(e, obj);
    end
end

local function checkDoor6()
    if door6Light1.visible or door6Light2.visible or door6Light3.visible then
        return;
    end
    moveDoor(true, "door6", true);
end

local function showWeaponSwitchInputHint()
    local obj = SceneObject();
    local c = nil;

    if input.usingGamepad then
        obj.pos = vec2(10, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addGamepadBindingHint(c, const.ActionGamepadIdPrimarySwitch);
        c:setDescription(tr.str23);
        obj:addComponent(c);
    else
        obj.pos = vec2(12, scene.gameHeight - 6);
        c = InputHintComponent(-2);
        addBindingHint(c, const.ActionIdPrimarySwitch);
        c:setDescription(tr.str23);
        obj:addComponent(c);
    end

    scene:addObject(obj);

    addTimeout0(function(cookie, dt)
        if not scene.inputPlayer.primarySwitchShowOff then
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                c:setFade(1.0);
            end);
        end
    end);
end

function hallway1Comeback()
    scene:getObjects("hall5_cp")[1].active = true;
    scene:getObjects("hall6_cp")[1].active = true;
end

-- main

activateLight(door6Light1);
activateLight(door6Light2);
activateLight(door6Light3);

makeGun2(gun2, function()
    coolRod(rod1, function()
        audio:playSound("alarm_off.ogg");
        deactivateLight(door6Light1);
        checkDoor6();
    end);
end);

makeGun2(gun3, function()
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(gun3.myObj.pos);
    coolRod(rod2, function()
        audio:playSound("alarm_off.ogg");
        deactivateLight(door6Light2);
        checkDoor6();
    end);
end);

makeGun2(gun4, function()
    coolRod(rod3, function()
        audio:playSound("alarm_off.ogg");
        deactivateLight(door6Light3);
        checkDoor6();
    end);
end);

makeDoor("door6", false);
makeDoor("door7", false);
makeDoor("door8", true);

setSensorEnterListener("shotgun_cp", true, function(other)
    if hallEntered then
        scene.inputPlayer.primarySwitchShowOff = true;
    else
        addTimeoutOnce(1.0, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog15.str1},
            }, function ()
                scene.cutscene = false;
                scene.inputPlayer.primarySwitchShowOff = true;
                showWeaponSwitchInputHint();
            end);
        end);
    end
end);

setSensorEnterListener("hall1_cp", true, function(other)
    hallEntered = true;
end);

setSensorEnterListener("hall2_cp", true, function(other)
    fixedSpawn("hall2");
    spawnBots("hall2", 1 / 3);
end);

setSensorEnterListener("hall3_cp", true, function(other)
    fixedSpawn("hall3");
    spawnBots("hall3", 1 / 4);
end);

setSensorEnterListener("hall4_cp", true, function(other)
    fixedSpawn("hall4");
    addTimeoutOnce(10.0, function()
        fixedSpawn("hall4");
        addTimeoutOnce(5.0, function()
            fixedSpawn("hall4b");
            addTimeoutOnce(5.0, function()
                fixedSpawn("hall4b");
            end);
        end);
    end);
end);

setSensorEnterListener("skull0_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("skull0_cp")[1]:getTransform();
    local function wave2()
        for _, obj in pairs(guardian1) do
            obj.mySpawned = factory:createGuardian2();
            addSpawnedEnemy(obj.mySpawned, obj);
            obj.mySpawned:findPhysicsBodyComponent():setFilterGroupIndex(-800);
            obj.mySpawned:applyLinearImpulse(obj.mySpawned:getDirection(250.0), obj.mySpawned.pos, true);
        end
        addTimeout(0.25, function(cookie)
            for _, obj in pairs(guardian1) do
                if obj.mySpawned:alive() then
                    return;
                end
            end
            cancelTimeout(cookie);
            addTimeoutOnce(1.5, function()
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                addTimeoutOnce(1.0, function()
                    scene.camera:findCameraComponent():follow(skullTarget, const.EaseOutQuad, 1.0);
                    addTimeoutOnce(1.5, function()
                        showLowerDialog(
                        {
                            {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog16.str1},
                            {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog16.str2},
                            {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog16.str3},
                        }, function ()
                            startAmbientMusic(true);
                            local p = scene:getObjects("skull0_path")[1];
                            skull.roamBehavior:reset();
                            skull.roamBehavior.linearVelocity = 10.0;
                            skull.roamBehavior.linearDamping = 6.0;
                            skull.roamBehavior.dampDistance = 2.0;
                            skull.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            skull.roamBehavior:start();
                            addTimeoutOnce(1.5, function()
                                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                                scene.cutscene = false;
                                moveDoor(true, "door7");
                                moveDoor(true, "door8", true);
                                scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(gun3.myObj.pos);
                                hallway1Comeback();
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end
    moveDoor(false, "door8");
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.player.angle = (skull.pos - scene.player.pos):angle();
    addTimeoutOnce(1.25, function()
        startMusic("action3.ogg", true);
        scene.camera:findCameraComponent():follow(skull, const.EaseOutQuad, 1.0);
        addTimeoutOnce(1.5, function()
            showLowerDialog(
            {
                {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog17.str1},
                {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog17.str2},
            }, function ()
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                scene.cutscene = false;
                for _, obj in pairs(guardian1) do
                    obj.mySpawned = factory:createGuardian();
                    addSpawnedEnemy(obj.mySpawned, obj);
                    obj.mySpawned:findPhysicsBodyComponent():setFilterGroupIndex(-800);
                    obj.mySpawned:applyLinearImpulse(obj.mySpawned:getDirection(250.0), obj.mySpawned.pos, true);
                end
                addTimeout(0.25, function(cookie)
                    for _, obj in pairs(guardian1) do
                        if obj.mySpawned:alive() then
                            return;
                        end
                    end
                    cancelTimeout(cookie);
                    addTimeoutOnce(1.5, function()
                        scene.player.linearDamping = 6.0;
                        scene.cutscene = true;
                        addTimeoutOnce(1.0, function()
                            scene.camera:findCameraComponent():follow(skull, const.EaseOutQuad, 1.0);
                            addTimeoutOnce(1.5, function()
                                showLowerDialog(
                                {
                                    {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog18.str1},
                                    {"enemy", tr.str3, "factory1/portrait_skull.png", tr.dialog18.str2},
                                }, function ()
                                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                                    scene.cutscene = false;
                                    wave2();
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("hall5_cp", true, function(other)
    spawnBots("hall5", 1 / 3);
end);

setSensorEnterListener("hall6_cp", true, function(other)
    spawnBots("hall6", 1 / 3);
end);
