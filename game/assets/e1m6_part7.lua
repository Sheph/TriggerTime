local bs1Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("bs1_light")[1];
local bs2Light = scene:getObjects("terrain0")[1]:findLightComponent():getLights("bs2_light")[1];
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();
local control3 = scene:getObjects("control3_ph")[1]:findPlaceholderComponent();
local control4 = scene:getObjects("control4_ph")[1]:findPlaceholderComponent();
local military0 = scene:getObjects("military0")[1];

local function processLightOn(light)
    addTimeoutOnce(0.5, function()
        audio:playSound("crystal_activate.ogg");
        local tweening = SequentialTweening(true);
        tweening:addTweening(SingleTweening(0.6, const.EaseOutQuad, light.color[4], 1.0, false));
        tweening:addTweening(SingleTweening(0.6, const.EaseInQuad, 1.0, light.color[4], false));
        addTimeout0(function(cookie, dt, self)
            local c = light.color;
            c[4] = tweening:getValue(self.t);
            light.color = c;
            self.t = self.t + dt;
        end, { t = 0 });
        light.my = 1;
        if (bs1Light.my ~= nil) and (bs2Light.my ~= nil) then
            control3.active = true;
            control3.visible = true;
        end
    end);
end

local function roboarmEnter(other, self)
    local items = {};
    local a = -90;
    if scene.inputRoboArm.active then
        a = 90;
    end
    table.insert(items, { pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(a), height = 6, image = "common2/arrow_button.png" });
    self.choice = showChoice(items, function(i)
        if scene.inputRoboArm.active then
            scene.inputPlayer.active = true;
            scene.inputRoboArm.active = false;
            scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        else
            scene.inputPlayer.active = false;
            scene.inputRoboArm.active = true;
            scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
            scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.5);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        end
        self.timer = addTimeoutOnce(1.0, function()
            self.timer = nil;
            roboarmEnter(other, self);
        end);
    end);
end

-- main

makeDoor("door13", false);
makeDoor("door14", false);
makeDoor("door15", false);

makeKeyDoor("red_door", "red_key_ph", function()
    openDoor("door15");
    scene:getObjects("press4_cp")[1].active = false;
    scene:getObjects("press4")[1].active = false;
    scene:getObjects("press4")[1].visible = false;
    scene:getObjects("secret1_press")[1].active = true;
    scene:getObjects("secret1_press")[1].visible = true;
    scene:getObjects("secret1")[1].visible = false;
    local laser4 = scene:getObjects("laser4");
    for _, obj in pairs(laser4) do
        obj.active = false;
        obj.visible = false;
    end
end);

makeDoor("door16", true);

makeDoor("door17", false);
makeDoorTrigger("door17_cp", "door17");

makeDoor("door18", false);
makeDoorTrigger("door18_cp", "door18");

makeDoor("door19", false);
makeDoorTrigger("door19_cp", "door19");

makeDoor("door20", false);

setSensorEnterListener("secret1_cp1", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local oldAngle = military0.angle;
    addTimeoutOnce(1.0, function()
        military0.angle = (scene.player.pos - military0.pos):angle();
        addTimeoutOnce(0.5, function()
            scene.player.angle = (military0.pos - scene.player.pos):angle();
        end);
    end);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str17, "common1/portrait_jake.png", tr.dialog145.str1},
            {"ally", tr.str17, "common1/portrait_jake.png", tr.dialog145.str2},
            {"ally", tr.str17, "common1/portrait_jake.png", tr.dialog145.str3},
            {"ally", tr.str17, "common1/portrait_jake.png", tr.dialog145.str4},
            {"player", tr.dialog145.str5},
            {"ally", tr.str17, "common1/portrait_jake.png", tr.dialog145.str6},
        }, function ()
            userData:setLevelAccessible("e1m12");
            gameShell:setAchieved(const.AchievementExplorer);
            scene.cutscene = false;
            openDoor("door20", true);
            addTimeoutOnce(1.0, function()
                military0.angle = oldAngle;
            end);
        end)
    end);
end);

setSensorEnterListener("secret1_cp2", true, function(other)
    stopMusic(true);
    stainedGlass({0, 0, 0, 0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 1.0, function()
        stainedGlass({0, 0, 0, 1.0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 100.0);
        scene.cutscene = true;
        addTimeoutOnce(0.1, function()
            scene:chainToNextLevel();
            scene:setNextLevel("e1m12.lua", "e1m12.json");
        end);
    end);
end);

setSensorEnterListener("west1_cp", true, function(other)
    local spawn = scene:getObjects("spawn9_small");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 2) == 2 then
            e = factory:createTetrocrab2();
        else
            e = factory:createScorp2();
        end
        addSpawnedEnemy(e, obj);
    end
    local spawn = scene:getObjects("spawn9");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createWarder(), spawn[i]);
        end);
    end
end);

setSensorEnterListener("west2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("west2_cp")[1]:getTransform();
    local spawn = scene:getObjects("spawn10");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createOrbo(), obj);
    end
end);

setSensorEnterListener("west3_cp", true, function(other)
    local spawn = scene:getObjects("spawn11");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
end);

setSensorEnterListener("west4_cp", true, function(other)
    local spawn = scene:getObjects("spawn12");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createEnforcer1(), obj);
    end
    local spawn = scene:getObjects("spawn12_orbo");
    for _, obj in pairs(spawn) do
        addSpawnedEnemy(factory:createOrbo(), obj);
    end
end);

setSensorEnterListener("west5_cp", true, function(other)
    local spawn = scene:getObjects("spawn13");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
end);

setSensorEnterListener("bs3_cp", false, function(other)
    if other.name ~= "blue_stone" then
        return;
    end
    local spawn = scene:getObjects("spawn14");
    for _, obj in pairs(spawn) do
        local e;
        if math.random(1, 2) == 2 then
            e = factory:createTetrocrab2();
        else
            e = factory:createScorp2();
        end
        addSpawnedEnemy(e, obj);
    end
    scene:getObjects("bs3_cp")[1].active = false;
end);

setSensorEnterListener("bs4_cp", false, function(other)
    if other.name ~= "blue_stone" then
        return;
    end
    local spawn = scene:getObjects("spawn15");
    for i = 1, #spawn, 1 do
        summon1(spawn[i], i == 1, function()
            local e;
            if math.random(1, 2) == 2 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            addSpawnedEnemy(e, spawn[i]);
        end);
    end
    scene:getObjects("bs4_cp")[1].active = false;
end);

setSensorEnterListener("west6_cp", true, function(other)
    local spawn = scene:getObjects("spawn16");
    for i = 1, #spawn, 1 do
        summon2(spawn[i], i == 1, function()
            addSpawnedEnemy(factory:createGorger1(), spawn[i]);
        end);
    end
end);

setSensorEnterListener("bs1_cp", false, function(other)
    if other.name ~= "blue_stone" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    other.gravityGunAware = false;
    scene:getObjects("bs1_cp")[1].active = false;
    processLightOn(bs1Light);
end);

setSensorEnterListener("bs2_cp", false, function(other)
    if other.name ~= "blue_stone" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    other.gravityGunAware = false;
    scene:getObjects("bs2_cp")[1].active = false;
    processLightOn(bs2Light);
end);

scene:getObjects("roboarm1")[1]:findRoboArmComponent().active = true;

control4.listener = createSensorListener(roboarmEnter, function(other, self)
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
    if scene.inputRoboArm.active then
        scene.inputPlayer.active = true;
        scene.inputRoboArm.active = false;
        scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
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

control3.listener = createSensorEnterListener(true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.5);
        scene.camera:findCameraComponent():follow(scene:getObjects("target3")[1], const.EaseOutQuad, 1.5);
        addTimeoutOnce(2.0, function()
            closeDoor("door16", true);
            addTimeoutOnce(2.0, function()
                scene:getObjects("steam1")[1]:findParticleEffectComponent():resetEmit();
                scene:getObjects("steam1")[1].visible = true;
                scene:getObjects("steam2")[1]:removeFromParent();
                addTimeoutOnce(1.5, function()
                    scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.5);
                    scene.camera:findCameraComponent():follow(scene:getObjects("target4")[1], const.EaseOutQuad, 1.5);
                    addTimeoutOnce(2.0, function()
                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 0.0);
                            scene.camera:findCameraComponent().target = scene:getObjects("target5")[1];
                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                                addTimeoutOnce(1.0, function()
                                    scene:getObjects("fire1")[1]:findWeaponFlamethrowerComponent():trigger(false);
                                    scene:getObjects("fire2")[1]:findWeaponFlamethrowerComponent():trigger(false);
                                    audio:playSound("fire_off.ogg");
                                    addTimeoutOnce(1.5, function()
                                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                                            scene.camera:findCameraComponent().target = scene.player;
                                            scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);
                                            scene.cutscene = false;
                                            control3.active = false;
                                            control3.visible = false;
                                            openDoor("door13", false);
                                            scene:getObjects("ga4")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal5")[1].pos);
                                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5);
                                        end);
                                    end);
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

control2.listener = createSensorEnterListener(true, function(other)
    scene:getObjects("ga4")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal5")[1].pos);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
            scene.camera:findCameraComponent().target = scene:getObjects("target6")[1];
            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                audio:playSound("servo_move.ogg");
                local shields = scene:getInstances("shield1");
                for _, inst in pairs(shields) do
                    findJoint(inst.joints, "shield1_joint").motorSpeed = -findJoint(inst.joints, "shield1_joint").motorSpeed;
                end
                addTimeoutOnce(3.0, function()
                    stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 0.5, function()
                        scene.camera:findCameraComponent().target = scene.player;
                        control2.active = false;
                        control2.visible = false;
                        stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 0.5, function()
                            showLowerDialog(
                            {
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog74.str1},
                                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog74.str2},
                            }, function ()
                                scene.cutscene = false;
                                scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal3")[1].pos);
                                scene:getObjects("boss1_cp")[1].active = true;
                                scene:getObjects("boss2_cp")[1].active = true;
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("boss1_cp", true, function(other)
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal3")[1].pos);
    bridgeOpen(bridge1, 5.0, false);
end);
