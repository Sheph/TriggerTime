local display2 = setupDisplay("display2");
local display3 = setupDisplay("display3");
local control0 = scene:getObjects("control0_ph")[1]:findPlaceholderComponent();
local mech = scene:getObjects("mech")[1];
local door5Opened = false;
local door5EnemyInside = false;
local door5PlayerInside = false;
local keeper1 = scene:getObjects("keeper1")[1];
local powergen1 = scene:getObjects("powergen1")[1];
local powergen2 = scene:getObjects("powergen2")[1];
local door4Light1 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door4_light1")[1];
local door4Light2 = scene:getObjects("terrain0")[1]:findLightComponent():getLights("door4_light2")[1];
local control0Done = false;

function enableHallwayDoors()
    scene:getObjects("door10_cp")[1].active = true;
    scene:getObjects("door11_cp")[1].active = true;
    scene:getObjects("door12_cp")[1].active = true;
    scene:getObjects("door13_cp")[1].active = true;
    scene:getObjects("door14_cp")[1].active = true;
    scene:getObjects("door15_cp")[1].active = true;
    scene:getObjects("door16_cp")[1].active = true;
    scene:getObjects("door17_cp")[1].active = true;
end

-- main

makeAirlock("door4", false);
makeAirlock("door5", false);
makeAirlock("door6", false);
makeAirlock("door10", false);
makeAirlockTrigger("door10_cp", "door10");
makeAirlock("door11", false);
makeAirlockTrigger("door11_cp", "door11");
makeAirlock("door12", false);
makeAirlockTrigger("door12_cp", "door12");
makeAirlock("door13", false);
makeAirlockTrigger("door13_cp", "door13");
makeAirlock("door14", false);
makeAirlockTrigger("door14_cp", "door14");
makeAirlock("door15", false);
makeAirlockTrigger("door15_cp", "door15");
makeAirlock("door16", false);
makeAirlockTrigger("door16_cp", "door16");
makeAirlock("door17", false);
makeAirlockTrigger("door17_cp", "door17");
makeAirlock("door18", false);
makeAirlock("door19", false);
makeAirlock("door20", false);

makeKeyDoor("blue_door", "blue_key_ph", function()
    scene:getObjects("door5_cp")[1].active = true;
end);

setSensorListener("door5_cp", function(other, self)
    if self.num == 0 then
        door5Opened = true;
        openAirlock("door5", true);
        local c = scene:getObjects("door5_cp")[1]:findPhysicsBodyComponent();
        c.filterMaskBits = const.collisionBitPlayer + const.collisionBitEnemy + const.collisionBitEnemyBuilding;
        if door5EnemyInside and (not control0Done) then
            control0.active = false;
            control0.visible = false;
        end
    end
    self.num = self.num + 1;
end, function (other, self)
    self.num = self.num - 1;
    if self.num == 0 then
        door5Opened = false;
        closeAirlock("door5", true);
        if (not door5EnemyInside) and door5PlayerInside then
            local c = scene:getObjects("door5_cp")[1]:findPhysicsBodyComponent();
            c.filterMaskBits = const.collisionBitPlayer;
            if not control0Done then
                control0.active = true;
                control0.visible = true;
            end
        end
    end
end, { num = 0 });

setSensorListener("door5_check_cp", function(other, objs, self)
    if other.type == const.SceneObjectTypePlayer then
        door5PlayerInside = true;
        return;
    end
    if objs[other.cookie] == nil then
        objs[other.cookie] = { count = 1, obj = other };
        if table.size(objs) == 1 then
            door5EnemyInside = true;
            if door5Opened and (not control0Done) then
                control0.active = false;
                control0.visible = false;
            end
        end
    else
        objs[other.cookie].count = objs[other.cookie].count + 1;
    end
end, function (other, objs, self)
    if other.type == const.SceneObjectTypePlayer then
        door5PlayerInside = false;
        local c = scene:getObjects("door5_cp")[1]:findPhysicsBodyComponent();
        c.filterMaskBits = const.collisionBitPlayer + const.collisionBitEnemy + const.collisionBitEnemyBuilding;
        return;
    end
    objs[other.cookie].count = objs[other.cookie].count - 1;
    if objs[other.cookie].count == 0 then
        objs[other.cookie] = nil;
    end
    if table.size(objs) == 0 then
        door5EnemyInside = false;
        if door5PlayerInside and (not door5Opened) then
            local c = scene:getObjects("door5_cp")[1]:findPhysicsBodyComponent();
            c.filterMaskBits = const.collisionBitPlayer;
        end
        if not control0Done then
            control0.active = true;
            control0.visible = true;
        end
    end
end, {});

control0.listener = createSensorListener(function(other, self)
    scene:getObjects("control0_ph")[1].freezable = false;
    local items = {};
    table.insert(items, { pos = vec2(0, -scene.gameHeight / 2 + 4), angle = math.rad(-90), height = 6, image = "common2/arrow_button.png" });
    self.choice = showChoice(items, function(i)
        openAirlock("door6", true);
        mech:findMechComponent().active = true;
        scene.inputPlayer.active = false;
        scene.inputMech.active = true;
        scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 1.0);
        scene.camera:findCameraComponent():follow(mech, const.EaseOutQuad, 1.0);
        scene:addCombatAlly(mech);
    end);
end, function(other, self)
    scene:getObjects("control0_ph")[1].freezable = true;
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
    if scene.inputMech.active then
        scene.inputPlayer.active = true;
        scene.inputMech.active = false;
        mech:findMechComponent().active = false;
        scene:removeCombatAlly(mech);
        scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
        if scene.player:alive() then
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        end
    end
end, { choice = nil, first = true });

setSensorEnterListener("hallway0_cp", true, function(other)
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog120.str1},
            {"player", tr.dialog120.str2},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("hallway1_cp", true, function(other)
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 2.5);
        scene.camera:findCameraComponent():follow(scene:getObjects("cam_target2")[1], const.EaseOutQuad, 2.5);
        addTimeoutOnce(3.5, function()
            display2:startAnimation(const.AnimationDefault + 1);
            display3:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.0, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog121.str1},
                }, function ()
                    scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 2.5);
                    scene.camera:findCameraComponent():follow(mech, const.EaseOutQuad, 2.5);
                    addTimeoutOnce(0.25, function()
                        local objs = scene:getObjects("hallway1_enemy");
                        for _, obj in pairs(objs) do
                            obj:findTargetableComponent().autoTarget = true;
                        end
                    end);
                    addTimeoutOnce(2.5, function()
                        display2:startAnimation(const.AnimationDefault);
                        display3:startAnimation(const.AnimationDefault);
                        scene.cutscene = false;
                        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(powergen1.pos);
                        scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(powergen2.pos);
                    end);
                end);
            end);
        end);
    end);
    addTimeout(0.25, function(cookie)
        if powergen1:dead() then
            if door4Light1.visible then
                door4Light1.visible = false;
                scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(powergen1.pos);
            end
        end

        if powergen2:dead() then
            if door4Light2.visible then
                door4Light2.visible = false;
                scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(powergen2.pos);
            end
        end

        if (not door4Light1.visible) and (not door4Light2.visible) then
            cancelTimeout(cookie);
            addTimeoutOnce(3.0, function()
                control0Done = true;
                control0.active = false;
                control0.visible = false;
                addTimeoutOnce(3.5, function()
                    openAirlock("door4", true);
                    enableHallwayDoors();
                end);
            end);
            return;
        end

        if mech:dead() then
            cancelTimeout(cookie);
            scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(powergen1.pos);
            scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(powergen2.pos);
            addTimeoutOnce(5.0, function()
                scene.cutscene = true;
                scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
                addTimeoutOnce(2.0, function()
                    showLowerDialog(
                    {
                        {"player", tr.dialog122.str1},
                    }, function ()
                        gameover();
                    end);
                end);
            end);
        end;
    end);
end);

setSensorEnterListener("hallway1b_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        fixedSpawn("hallway1b");
    end);
end);

setSensorEnterListener("hallway2_cp", true, function(other)
    local objs = scene:getObjects("hallway2_enemy");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("hallway2b_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        fixedSpawn("hallway2b");
    end);
    local objs = scene:getObjects("hallway2b_enemy");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("hallway3_cp", true, function(other)
    local objs = scene:getObjects("hallway3_enemy");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("hallway4_cp", true, function(other)
    addTimeoutOnce(2.0, function()
        keeper1:findKeeperComponent().autoTarget = true;
        addTimeoutOnce(0.5, function()
            openHatch("hatch1");
            addTimeoutOnce(0.5, function()
                keeper1:findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch("hatch1");
                end);
            end);
        end);
    end);
    addTimeoutOnce(9.0, function()
        local keeper2 = factory:createKeeper(true);
        keeper2:setTransform(scene:getObjects("keeper2")[1]:getTransform());
        scene:addObject(keeper2);
        keeper2:findKeeperComponent().autoTarget = true;
        addTimeoutOnce(0.5, function()
            openHatch("hatch1");
            addTimeoutOnce(0.5, function()
                keeper2:findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch("hatch1");
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("hallway5_cp", true, function(other)
    local objs = scene:getObjects("hallway5_enemy");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
end);

setSensorEnterListener("hallway5b_cp", true, function(other)
    fixedSpawn("hallway5b");
    scene:getObjects("hallway6_cp")[1].active = true;
end);

setSensorEnterListener("hallway5c_cp", true, function(other)
    fixedSpawn("hallway5b");
end);

setSensorEnterListener("hallway6_cp", true, function(other)
    addTimeoutOnce(9.0, function()
        local keeper2 = factory:createKeeper(true);
        keeper2:setTransform(scene:getObjects("keeper2")[1]:getTransform());
        scene:addObject(keeper2);
        keeper2:findKeeperComponent().autoTarget = true;
        addTimeoutOnce(0.5, function()
            openHatch("hatch1");
            addTimeoutOnce(0.5, function()
                keeper2:findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch("hatch1");
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("hallway7_cp", true, function(other)
    fixedSpawn("hallway7");
end);

setSensorEnterListener("hallway7b_cp", true, function(other)
    fixedSpawn("hallway7b");
    scene:getObjects("hallway8_cp")[1].active = true;
end);

setSensorEnterListener("hallway8_cp", true, function(other)
    addTimeoutOnce(9.0, function()
        local keeper2 = factory:createKeeper(true);
        keeper2:setTransform(scene:getObjects("keeper2")[1]:getTransform());
        scene:addObject(keeper2);
        keeper2:findKeeperComponent().autoTarget = true;
        addTimeoutOnce(0.5, function()
            openHatch("hatch1");
            addTimeoutOnce(0.5, function()
                keeper2:findKeeperComponent():crawlOut();
                addTimeoutOnce(1.8, function()
                    closeHatch("hatch1");
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("hallway9_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("hallway9_cp")[1]:getTransform();
    closeAirlock("door4", true);
end);

setSensorEnterListener("jars1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog123.str1},
        }, function ()
            scene.cutscene = false;
            updateCountdown();
        end);
    end);
end);

setSensorEnterListener("boss1_cp", true, function(other)
    scene:getObjects("boss2_cp")[1].active = true;
    scene.cutscene = true;
    scene.camera:findCameraComponent():zoomTo(60, const.EaseOutQuad, 1.5);
    scene.player.linearDamping = 6.0;
    local p = scene:getObjects("boss1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 10.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("boss2_cp", true, function(other)
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog124.str1},
            {"player", tr.dialog124.str2}
        }, function ()
            addTimeoutOnce(1.0, function()
                display2:startAnimation(const.AnimationDefault + 1);
                display3:startAnimation(const.AnimationDefault + 1);
                addTimeoutOnce(1.5, function()
                    showLowerDialog(
                    {
                        {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog125.str1},
                        {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog125.str2},
                        {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog125.str3},
                    }, function ()
                        scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("boss1_cp")[1].pos);
                        openAirlock("door18", true);
                        scene.player.roamBehavior:reset();
                        scene.cutscene = false;
                        scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
                        addTimeoutOnce(2.0, function()
                            display2:startAnimation(const.AnimationDefault);
                            display3:startAnimation(const.AnimationDefault);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("boss3_cp", true, function(other)
    completeLevel(0.5, tr.str33, tr.str34, "e1m10", false, function()
        print("numAnimals = "..scene.stats.numAnimals..", numKilledAnimals = "..scene.stats.numKilledAnimals);
        if scene.stats.numKilledAnimals == 0 then
            gameShell:setAchieved(const.AchievementSaveTheAnimals);
        elseif scene.stats.numKilledAnimals == scene.stats.numAnimals then
            gameShell:setAchieved(const.AchievementKillTheAnimals);
        end
    end);
end);
