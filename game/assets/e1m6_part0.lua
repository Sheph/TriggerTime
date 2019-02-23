local keyl = scene:getObjects("keyl")[1];
local playerVisitedStations = 0;

function part0Done(immediate)
    local sentry1 = scene:getObjects("sentry1")[1];
    local c = sentry1:findSentryComponent();
    c:unfold();
    scene:addCombatAlly(sentry1);
    c.autoTarget = true;
    c.followPlayer = true;
    c.targetDistance = 6.0;
    if immediate then
        bridgeOpen(bridge0, 8.0, false);
    else
        addTimeoutOnce(3.0, function()
            bridgeOpen(bridge0, 8.0, false);
        end);
    end
end

-- main

makeWater("terrain0", "water0");

makeDoor("door0", false);
makeDoorTrigger("door0_cp", "door0");

makeDoor("door1", false);

if settings.developer == 0 then
    scene.cutscene = true;

    local p = scene:getObjects("player1_path")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 15.0;
    scene.player.roamBehavior.linearDamping = 4.0;
    scene.player.roamBehavior.dampDistance = 3.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();

    p = scene:getObjects("keyl1_path")[1];
    keyl.roamBehavior:reset();
    keyl.roamBehavior.linearVelocity = 15.0;
    keyl.roamBehavior.linearDamping = 4.0;
    keyl.roamBehavior.dampDistance = 3.0;
    keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    keyl.roamBehavior:start();

    setSensorEnterListener("start1_cp", true, function(other)
        scene.player.roamBehavior:damp();
        keyl.roamBehavior:damp();

        addTimeoutOnce(1.5, function()
            scene.player.roamBehavior:reset();
            showLowerDialog(
            {
                {"player", tr.dialog59.str1},
                {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog59.str2}
            }, function ()
                scene.cutscene = false;
            end);
        end);
    end);
end

setSensorEnterListener("start0_cp", false, function(other)
    scene.player.linearVelocity = scene:getObjects("start0_cp")[1]:getDirection(5.0);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog60.str1},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("start2_cp", true, function(other)
    local objs = scene:getObjects("enemy1");
    for _, obj in pairs(objs) do
        obj:findTargetableComponent().autoTarget = true;
    end
    addTimeout(0.5, function(cookie)
        for _, obj in pairs(objs) do
            if obj:alive() then
                return;
            end
        end
        cancelTimeout(cookie);
        addTimeoutOnce(1.5, function()
            scene.player.linearDamping = 6.0;
            scene.cutscene = true;
            showLowerDialog(
            {
                {"player", tr.dialog61.str1},
            }, function ()
                scene.cutscene = false;
                scene:getObjects("start3_cp")[1].active = true;
            end);
        end);
    end);
end);

setSensorEnterListener("start3_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showUpperDialog(
    {
        {"player", tr.dialog62.str1},
        {"player", tr.dialog62.str2},
    }, function ()
        scene.cutscene = false;
        local p = scene:getObjects("keyl2_path")[1];
        keyl.roamBehavior:reset();
        keyl.roamBehavior.linearVelocity = 15.0;
        keyl.roamBehavior.linearDamping = 4.0;
        keyl.roamBehavior.dampDistance = 3.0;
        keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        keyl.roamBehavior:start();
    end);
end);

setSensorEnterListener("keyl1_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showUpperDialog(
        {
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog63.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog63.str2},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog63.str3},
            {"player", tr.dialog63.str4}
        }, function ()
            scene.cutscene = false;
            local p = scene:getObjects("keyl3_path")[1];
            keyl.roamBehavior:reset();
            keyl.roamBehavior.linearVelocity = 15.0;
            keyl.roamBehavior.linearDamping = 4.0;
            keyl.roamBehavior.dampDistance = 3.0;
            keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            keyl.roamBehavior:start();
        end);
    end);
end);

setSensorEnterListener("keyl2_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog64.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog64.str2},
            {"player", tr.dialog64.str3},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog64.str4}
        }, function ()
            scene:getObjects("player1_cp")[1].active = true;
            scene.cutscene = false;
            addTimeoutOnce(1.0, function()
                audio:playSound("alarm_off.ogg");
                local laser0 = scene:getObjects("laser0");
                for _, obj in pairs(laser0) do
                    obj:addComponent(FadeOutComponent(1.0));
                end
            end);
        end);
    end);
end);

setSensorEnterListener("player1_cp", false, function(other)
    if not other:findPlayerComponent().earpiece then
        return;
    end
    scene:getObjects("player1_cp")[1].active = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog65.str1},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog65.str2}
    }, function ()
        scene.cutscene = false;
        addTimeoutOnce(1.0, function()
            audio:playSound("alarm_off.ogg");
            local laser0 = scene:getObjects("laser1");
            for _, obj in pairs(laser0) do
                obj:addComponent(FadeOutComponent(1.0));
            end
            scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("ws1")[1].pos);
            scene:getObjects("ga1")[1]:findGoalAreaComponent():addGoal(scene:getObjects("hs1")[1].pos);
        end);
    end);
end);

setSensorEnterListener("player2_cp", true, function(other)
    playerVisitedStations = playerVisitedStations + 1;
    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("hs1")[1].pos);
end);

setSensorEnterListener("player3_cp", true, function(other)
    playerVisitedStations = playerVisitedStations + 1;
    scene:getObjects("ga1")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("ws1")[1].pos);
end);

setSensorEnterListener("player4_cp", false, function(other)
    if playerVisitedStations ~= 2 then
        return;
    end
    scene:getObjects("player4_cp")[1].active = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog66.str1},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog66.str2},
        {"player", tr.dialog66.str3},
    }, function ()
        scene.cutscene = false;
        part0Done(false);
    end);
end);
