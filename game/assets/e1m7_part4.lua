local park5 = scene:getObjects("park5_ph")[1]:findPlaceholderComponent();
local tentacle14 = scene:getObjects("tentacle14");
local tentacle14weld = weldTentacleBones(tentacle14, {3, 6, 9}, 2.5, 0.5);
local control1 = scene:getObjects("control1_ph")[1]:findPlaceholderComponent();
local control2 = scene:getObjects("control2_ph")[1]:findPlaceholderComponent();

local function makeTentacleSwing1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.6 + math.random() * 0.4;
        local f = 120000 + math.random() * 10000.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentaclePulse1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local timeF = 0.2 + math.random() * 0.2;
        local phaseF = 3.0 + math.random() * 2.0;
        local a1F = 0.1 + math.random() * 0.2;
        local a2F = 0.1 + math.random() * 0.2;
        obj:addComponent(TentaclePulseComponent(15, timeF, phaseF, a1F, a2F));
    end
end

local function makeTentacleSwing2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 1.0 + math.random() * 0.5;
        local f = 1000 + math.random() * 500.0;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

-- main

makeAirlock("door18", true);

setSensorEnterListener("boat9_cp", true, function(other)
    closeAirlock("door18", true);
    stopPlatforms2();
end);

makeBoatPark(park5, "park5_exit");

makeDoor("door16", true);
makeDoor("door17", true);
makeDoor("door19", false);

makeTentacleFlesh("tentacle14");
makeTentacleSwing1("tentacle14");

makeTentacleFlesh("tentacle15");
makeTentacleSwing2("tentacle15");
makeTentaclePulse1("tentacle15");

control1.listener = createSensorEnterListener(true, function(other)
    control1.active = false;
    control1.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog86.str1},
            {"player", tr.dialog86.str2},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

control2.listener = createSensorEnterListener(true, function(other)
    control2.active = false;
    control2.visible = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog87.str1},
            {"player", tr.dialog87.str2},
        }, function ()
            scene.cutscene = false;
            openDoor("door19", true);
        end);
    end);
end);

setSensorEnterListener("boss1_cp", true, function(other)
    control1.active = false;
    control1.visible = false;
    scene.respawnPoint = scene:getObjects("boss1_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local p = scene:getObjects("boss1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = settings.player.moveSpeed;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();
end);

setSensorEnterListener("boss2_cp", true, function(other)
    closeDoor("door17", true);
    scene.player.roamBehavior.linearDamping = 6.0;
    scene.player.roamBehavior:damp();
    local bc = scene:getObjects("boss")[1]:findBossSquidComponent();
    addTimeoutOnce(1.0, function()
        startMusic("action12.ogg", true);
        scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
        scene.camera:findCameraComponent():zoomTo(50, const.EaseOutQuad, 1.5);
        scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
        addTimeoutOnce(1.5, function()
            bc.autoTarget = true;
        end);
    end);
    addTimeoutOnce(6.0, function()
        scene.player.roamBehavior:reset();
        scene.cutscene = false;
    end);
    addTimeout(0.5, function(cookie)
        if bc.deathFinished then
            cancelTimeout(cookie);
            startAmbientMusic(true);
            scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
            scene.camera:findCameraComponent():zoomTo(35.0, const.EaseInOutQuad, 3.0);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 3.0);
            addTimeoutOnce(3.0, function()
                unweldTentacleBones(tentacle14weld);
                scene.camera:findCameraComponent():tremorStart(0.3);
                local stream = audio:createStream("queen_shake.ogg");
                stream:play();
                local tentacle15 = scene:getObjects("tentacle15")[1];
                local bones = tentacle15:findRenderTentacleComponent().objects;
                bones[#bones].bodyType = const.BodyDynamic;
                tentacle15.linearVelocity = tentacle15:getDirection(-13.0);
                for _, obj in pairs(tentacle14) do
                    obj.linearVelocity = obj:getDirection(-25.0);
                end
                addTimeoutOnce(2.5, function()
                    scene.camera:findCameraComponent():tremor(false);
                    stream:stop();
                    tentacle15.linearVelocity = vec2(0, 0);
                    for _, obj in pairs(tentacle14) do
                        obj.linearVelocity = vec2(0, 0);
                    end
                    scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal1")[1].pos);
                end);
            end);
        end
    end);
end);

setSensorEnterListener("boss3_cp", true, function(other)
    scene:getObjects("ga2")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal1")[1].pos);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog88.str1},
            {"player", tr.dialog88.str2},
        }, function ()
            completeLevel(1.0, tr.str31, tr.str32, "e1m8");
        end);
    end);
end);
