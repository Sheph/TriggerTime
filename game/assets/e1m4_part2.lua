local tb2 = scene:getObjects("tb2");
local tb2Spawned = {};
local tb3 = scene:getObjects("tb3");
local tb3Spawned = {};
local press1Joint = scene:getJoints("press1_joint")[1];
local conveyor2Spawn = scene:getObjects("conveyor2_spawn");
local conveyor3Spawn = scene:getObjects("conveyor3_spawn");
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();
local control3 = scene:getObjects("control3_ph")[1]:findPlaceholderComponent();
local control4 = scene:getObjects("control4_ph")[1]:findPlaceholderComponent();
local rodLight1 = scene:getObjects("terrain1")[1]:findLightComponent():getLights("rod_light1")[1];
local rodLight2 = scene:getObjects("terrain1")[1]:findLightComponent():getLights("rod_light2")[1];
local rod1Done = false;
local display1 = nil;

local function roboarmEnter(other, self)
    local items = {};
    local a = -90;
    if scene.inputRoboArm.active then
        a = 90;
    end
    table.insert(items, { pos = vec2(0, scene.gameHeight / 2 - 4), angle = math.rad(a), height = 6, image = "common2/arrow_button.png" });
    self.choice = showChoice(items, function(i)
        if scene.inputRoboArm.active then
            scene.inputPlayer.active = true;
            scene.inputRoboArm.active = false;
            self.c.active = false;
            scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.5);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        else
            scene.inputPlayer.active = false;
            scene.inputRoboArm.active = true;
            self.c.active = true;
            scene.camera:findCameraComponent():setConstraint(scene:getObjects(self.ct.."_1")[1].pos, scene:getObjects(self.ct.."_2")[1].pos);
            scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        end
        self.timer = addTimeoutOnce(1.0, function()
            self.timer = nil;
            roboarmEnter(other, self);
        end);
    end);
end

local function roboarmExit(other, self)
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
    if scene.inputRoboArm.active then
        scene.inputPlayer.active = true;
        scene.inputRoboArm.active = false;
        self.c.active = false;
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
end

function setupRod1Done(quiet)
    rod1Done = true;
    openAirlock("door12");
    alarmLightOn("alarm2", quiet);
    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal2")[1].pos);
    local objs = scene:getObjects("enemy7_enforcer");
    for _, obj in pairs(objs) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end
    objs = scene:getObjects("enemy7_other");
    for _, obj in pairs(objs) do
        local e = nil;
        if math.random(1, 3) == 3 then
            e = factory:createScorp2();
        else
            e = factory:createTetrocrab2();
        end
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end
end

local function checkAlarm2()
    if not rodLight1.visible or not rodLight2.visible then
        return;
    end
    setupRod1Done(false);
end

local function sentryUnfoldCp(other, name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local c = obj:findSentryComponent();
        c:unfold();
        c.autoTarget = true;
    end
end

-- main

setSensorEnterListener("catwalk3_cp", false, function(other)
    for _, obj in pairs(tb2Spawned) do
        if obj:alive() then
            return;
        end
    end
    table.shuffle(tb2);
    tb2Spawned = {};
    for i = 1,(#tb2 * 3 / 4),1 do
        local e = factory:createTetrobot();
        e:setTransform(tb2[i]:getTransform());
        e:findTargetableComponent().autoTarget = true;
        scene:addObject(e);
        e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
        table.insert(tb2Spawned, e);
    end
end);

setSensorEnterListener("enemy6_spawn", true, function(other)
    local objs = scene:getObjects("enemy6_enforcer");
    for _, obj in pairs(objs) do
        local e = factory:createEnforcer1();
        e:setTransform(obj:getTransform());
        local ec = e:findTargetableComponent();
        ec.autoTarget = true;
        scene:addObject(e);
    end
end);

local rc = scene:getObjects("terrain0")[1]:findRenderTerrainComponents("lava1")[1];
local ac = AnimationComponent(rc.drawable);
ac:addAnimation(const.AnimationDefault, "lava1", 1);
ac:startAnimation(const.AnimationDefault);
scene:getObjects("terrain0")[1]:addComponent(ac);

makeLavaPipe("lava1");
makeLavaPipe("lava2");
makeLavaPipe("lava3");
makeGasPipe("pipe3");
makeLavaDamage("lava_damage1", 200.0);

setSensorEnterListener("lava1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("lava1_cp")[1]:getTransform();
end);

setSensorEnterListener("catwalk4_cp", false, function(other)
    for _, obj in pairs(tb3Spawned) do
        if obj:alive() then
            return;
        end
    end
    table.shuffle(tb3);
    tb3Spawned = {};
    for i = 1,(#tb3 * 1 / 2),1 do
        local e = factory:createTetrobot();
        e:setTransform(tb3[i]:getTransform());
        e:findTargetableComponent().autoTarget = true;
        scene:addObject(e);
        e:findPhysicsBodyComponent():setFilterGroupIndex(-300);
        table.insert(tb3Spawned, e);
    end
end);

setSensorEnterListener("catwalk5_cp", true, function(other)
    fixedSpawn("catwalk5");
    spawnBots("catwalk5", 0.3);
end);

addTimeout0(function(cookie, dt)
    if (press1Joint:getJointTranslation() <= press1Joint.lowerLimit) then
        press1Joint.motorSpeed = math.abs(press1Joint.motorSpeed);
    elseif (press1Joint:getJointTranslation() >= press1Joint.upperLimit) then
        press1Joint.motorSpeed = -math.abs(press1Joint.motorSpeed);
    end
end);

makeGear("press1", "press1_joint", "press2", "press2_joint", 1.5);
makeGear("press1", "press1_joint", "press3", "press3_joint", -1);
makeGear("press1", "press1_joint", "press4", "press4_joint", 1.5);
makeGear("press1", "press1_joint", "press5", "press5_joint", -1);
makeGear("press1", "press1_joint", "press6", "press6_joint", 1.5);
makeGear("press1", "press1_joint", "press7", "press7_joint", -1);
makeGear("press1", "press1_joint", "press8", "press8_joint", 1.5);

makeAirlock("door10", false);
makeAirlock("door11", false);
makeAirlock("door12", false);
makeAirlock("door13", false);

makeConveyor("conveyor2", 9.0, function(obj)
    if obj.name == "robopart" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(1.0, function(cookie)
    if stopAllConveyors then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor2Spawn[math.random(1, #conveyor2Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    if math.random(1, 6) == 6 then
        scene:instanciate("e1m4_robopart1.json", xf);
    elseif math.random(1, 3) == 3 then
        scene:instanciate("e1m4_robopart2.json", xf);
    else
        scene:instanciate("e1m4_robopart3.json", xf);
    end
end);

makeConveyor2("conveyor3", 9.0, function(obj)
    return obj.name ~= "robopart2";
end, function(obj)
    if obj.name == "robopart" then
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

addTimeout(1.0, function(cookie)
    if stopAllConveyors then
        cancelTimeout(cookie);
        return;
    end
    local spawn = conveyor3Spawn[math.random(1, #conveyor3Spawn)];
    local xf = spawn:getTransform();
    xf.q = rot(math.random(0, math.pi * 2.0));
    if math.random(1, 6) == 6 then
        scene:instanciate("e1m4_robopart8.json", xf);
    elseif math.random(1, 6) == 6 then
        scene:instanciate("e1m4_robopart7.json", xf);
    elseif math.random(1, 2) == 1 then
        scene:instanciate("e1m4_robopart6.json", xf);
    elseif math.random(1, 2) == 1 then
        scene:instanciate("e1m4_robopart5.json", xf);
    else
        scene:instanciate("e1m4_robopart4.json", xf);
    end
end);

setSensorEnterListener("conveyor2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("conveyor2_cp")[1]:getTransform();
end);

control2.listener = createSensorListener(roboarmEnter, roboarmExit,
    { c = scene:getObjects("roboarm1")[1]:findRoboArmComponent(), ct = "constraint2", choice = nil, timer = nil });
control3.listener = createSensorListener(roboarmEnter, roboarmExit,
    { c = scene:getObjects("roboarm2")[1]:findRoboArmComponent(), ct = "constraint3", choice = nil, timer = nil });

setSensorListener("rod_cp1", function(other, self)
    if other.name ~= "rod1" or rod1Done then
        return;
    end
    rodLight1.visible = true;
    checkAlarm2();
end, function (other, self)
    if other.name ~= "rod1" or rod1Done then
        return;
    end
    rodLight1.visible = false;
end);

setSensorListener("rod_cp2", function(other, self)
    if other.name ~= "rod1" or rod1Done then
        return;
    end
    rodLight2.visible = true;
    checkAlarm2();
end, function (other, self)
    if other.name ~= "rod1" or rod1Done then
        return;
    end
    rodLight2.visible = false;
end);

setSensorEnterListener("sentry1_cp", true, sentryUnfoldCp, "sentry1");
setSensorEnterListener("sentry2_cp", true, sentryUnfoldCp, "sentry2");

makeAirlock("door14", true);
makeAirlock("door15", false);
makeAirlock("door16", false);
makeAirlockTrigger("door16_cp", "door16");
display1 = setupDisplay("display1");

setSensorEnterListener("display1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("display1_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("display1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 8.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("display1_cp2", true, function(other)
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    addTimeoutOnce(1.0, function()
        scene.player.roamBehavior:reset();
        display1:startAnimation(const.AnimationDefault + 1);
        addTimeoutOnce(1.0, function()
            showLowerDialog(
            {
                {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog43.str1},
                {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog43.str2},
            }, function ()
                scene.cutscene = false;
                closeAirlock("door14", true);
                local objs = scene:getObjects("display1_enforcer");
                for _, obj in pairs(objs) do
                    obj:findTargetableComponent().autoTarget = true;
                end
                addTimeout0(function(cookie, dt)
                    for _, obj in pairs(objs) do
                        if obj:alive() then
                            return;
                        end
                    end
                    cancelTimeout(cookie);
                    addTimeoutOnce(1.0, function()
                        local objs = scene:getObjects("display1_sentry");
                        for _, obj in pairs(objs) do
                            local c = obj:findSentryComponent();
                            c:unfold();
                            c.autoTarget = true;
                        end
                        addTimeout0(function(cookie, dt)
                            for _, obj in pairs(objs) do
                                if obj:alive() then
                                    return;
                                end
                            end
                            cancelTimeout(cookie);
                            addTimeoutOnce(1.0, function()
                                openAirlock("door14", true);
                                openAirlock("door15", true);
                                local objs = scene:getObjects("display1_sentry2");
                                for _, obj in pairs(objs) do
                                    local e = factory:createSentry1(false);
                                    e:setTransformRecursive(obj:getTransform());
                                    e:findTargetableComponent().autoTarget = true;
                                    scene:addObject(e);
                                end
                                scene:getObjects("sentry1_cp")[1].active = true;
                                scene:getObjects("sentry2_cp")[1].active = true;
                            end);
                        end);
                    end);
                end);
                addTimeoutOnce(1.5, function()
                    display1:startAnimation(const.AnimationDefault);
                end);
            end);
        end);
    end);
end);

scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("ws2")[1].pos);
scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("control4_ph")[1].pos);

local weapon2Cnt = 0;

setSensorEnterListener("weapon2_cp", true, function(other)
    scene.inputPlayer.primarySwitchShowOff = true;
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("ws2")[1].pos);
    weapon2Cnt = weapon2Cnt + 1;
    if weapon2Cnt == 2 then
        scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal3")[1].pos);
        scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal2")[1].pos);
    end
    local objs = scene:getObjects("sentry3");
    for _, obj in pairs(objs) do
        local e = factory:createSentry1(false);
        e:setTransformRecursive(obj:getTransform());
        e:findTargetableComponent().autoTarget = true;
        scene:addObject(e);
    end
end);

control4.listener = createSensorEnterListener(true, function(other)
    control4.active = false;
    control4.visible = false;
    alarmLightOff("alarm2", false);
    openAirlock("door10", false);
    openAirlock("door29", false);
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("control4_ph")[1].pos);
    weapon2Cnt = weapon2Cnt + 1;
    if weapon2Cnt == 2 then
        scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal3")[1].pos);
        scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal2")[1].pos);
    end
end);
