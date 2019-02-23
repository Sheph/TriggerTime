local function explodePipe(pipeName, holeName, fireName, fireScale)
    local pipeObj = scene:getObjects(pipeName)[1];
    local fObj = scene:getObjects(fireName)[1];
    local e = factory:createExplosion1(const.zOrderExplosion);
    e.pos = fObj.pos;
    scene:addObject(e);
    pipeObj:findRenderQuadComponents(holeName)[1].visible = true;
    local f = fObj:findWeaponFlamethrowerComponent();
    f.damage = 5000.0;
    f:setColors({0.0, 0.6117647, 1.0, 1.0}, {0.0, 0.03529412, 1.0, 1.0});
    f.haveLight = true;
    f.haveSound = false;
    f:setScale(fireScale, 0);
    f:trigger(true);
end

-- main

makeDoor("door2", false);
makeDoor("door3", false);
makeDoor("door4", false);
makeDoorTrigger("door3_cp", "door3");
makeDoor("door6", false);
makeDoorTrigger("door6_cp", "door6");

setSensorEnterListener("east1_cp", true, function(other)
    local spawn2 = scene:getObjects("spawn2");
    for i = 1, #spawn2, 1 do
        summon1(spawn2[i], i == 1, function()
            local e = factory:createWarder();
            e:setTransform(spawn2[i]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
end);

setSensorEnterListener("east2_cp", true, function(other)
    local spawn3 = scene:getObjects("spawn3");
    for i = 1, #spawn3, 1 do
        summon1(spawn3[i], i == 1, function()
            local e;
            if math.random(1, 3) == 3 then
                e = factory:createWarder();
            else
                e = factory:createEnforcer1();
            end
            e:setTransform(spawn3[i]:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end);
    end
end);

setSensorEnterListener("fire1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    explodePipe("pipe1", "hole1", "fire1", 1.2);
    addTimeoutOnce(0.5, function()
        explodePipe("pipe1", "hole2", "fire2", 1.2);
    end);
    addTimeoutOnce(2.5, function()
        showLowerDialog(
        {
            {"player", tr.dialog69.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog69.str2},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog69.str3},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog69.str4}
        }, function ()
            scene.cutscene = false;
            openDoor("door2", false);
            scene:getObjects("ga3")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal2")[1].pos);
            local spawn4 = scene:getObjects("spawn4");
            for _, obj in pairs(spawn4) do
                local e = factory:createTetrocrab2();
                e:setTransform(obj:getTransform());
                local ec = e:findTargetableComponent();
                ec.autoTarget = true;
                scene:addObject(e);
            end
            local spawn4sc = scene:getObjects("spawn4_sc");
            for _, obj in pairs(spawn4sc) do
                local e = factory:createScorp2();
                e:setTransform(obj:getTransform());
                local ec = e:findTargetableComponent();
                ec.autoTarget = true;
                scene:addObject(e);
            end
        end);
    end);
end);

setSensorEnterListener("door2_cp", true, function(other)
    scene:getObjects("ga3")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("goal2")[1].pos);
end);
