local playerFree = false;

local function makeLeverLaser(leverName, laserName)
    makeLever(leverName, false, function()
        disableLever(leverName);
        local objs = scene:getObjects(laserName);
        for _, obj in pairs(objs) do
            obj:addComponent(FadeOutComponent(1.0));
        end
        audio:playSound("alarm_off.ogg");
    end);
end

-- main

-- modify friction of boxes on contact with the player, it's easier to drag
-- the boxes with friction on.
local bi = 1;
while true do
    local boxes = scene:getObjects("box"..bi);
    if #boxes == 0 then
        break;
    end
    bi = bi + 1;
    for _, obj in pairs(boxes) do
        local cmc = CollisionModifyComponent();
        cmc:setFilter({const.SceneObjectTypePlayer});
        cmc:modifyFriction(0.5);
        obj:addComponent(cmc);
    end
end

makeLever("lever1", false, function()
    disableLever("lever1");
    local objs = scene:getObjects("laser1");
    for _, obj in pairs(objs) do
        obj:addComponent(FadeOutComponent(1.0));
    end
    audio:playSound("alarm_off.ogg");
    playerFree = true;
    addTimeoutOnce(2.0, function()
        gameShell:setAchieved(const.AchievementFreedom);
    end);
end);

if settings.developer <= 0 then
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog45.str1},
    }, function ()
        scene.cutscene = false;
    end);
end

setSensorEnterListener("start_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    local control1 = scene:getObjects("control1")[1];
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(control1, const.EaseOutQuad, 1.5);
        addTimeoutOnce(2.5, function()
            local obj = factory:createExplosion1(const.zOrderExplosion);
            obj.pos = control1.pos;
            local ec = obj:findExplosionComponent();
            ec:setBlast(control1, 1500, 5000, { const.SceneObjectTypeEnemy });
            scene:addObject(obj);
            scene:getObjects("sparks1")[1]:removeFromParent();
            local objs = scene:getObjects("fire1");
            for _, obj in pairs(objs) do
                obj.visible = true;
            end
            local enforcer1 = scene:getObjects("enforcer1")[1];
            local p = scene:getObjects("enforcer1_path1")[1];
            enforcer1.roamBehavior:reset();
            enforcer1.roamBehavior.linearVelocity = 10.0;
            enforcer1.roamBehavior.linearDamping = 6.0;
            enforcer1.roamBehavior.dampDistance = 2.0;
            enforcer1.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            enforcer1.roamBehavior:start();
        end);
    end);
end);

setSensorEnterListener("enforcer1_cp", true, function(other)
    addTimeoutOnce(2.0, function()
        local enforcer1 = scene:getObjects("enforcer1")[1];
        local p = scene:getObjects("enforcer1_path2")[1];
        enforcer1.roamBehavior:reset();
        enforcer1.roamBehavior.linearVelocity = 10.0;
        enforcer1.roamBehavior.linearDamping = 6.0;
        enforcer1.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        enforcer1.roamBehavior:start();
        addTimeoutOnce(2.0, function()
            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);
            addTimeoutOnce(1.5, function()
                setSensorEnterListener("box1_cp", false, function(other, pt, bt)
                    if playerFree then
                        return;
                    end
                    addTimeoutOnce(2.0, function()
                        if playerFree then
                            return;
                        end
                        stainedGlass({0, 0, 0, 0}, {0, 0, 0, 1}, const.EaseLinear, 1.0, function()
                            scene.player:setTransform(pt);
                            scene:getObjects("box1")[1]:setTransform(bt);
                            stainedGlass({0, 0, 0, 1}, {0, 0, 0, 0}, const.EaseLinear, 1.0);
                        end);
                    end);
                end, scene.player:getTransform(), scene:getObjects("box1")[1]:getTransform());
                scene.cutscene = false;
            end);
        end);
    end);
end);

setSensorEnterListener("enforcer1_cp2", true, function(other)
    other:removeFromParent();
end);

makeLeverLaser("lever2", "laser2");
makeLeverLaser("lever3", "laser3");
makeLeverLaser("lever4", "laser4");

makeDoor("door1", false);

setSensorEnterListener("chainsaw_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        openDoor("door1", true);
        local objs = scene:getObjects("enforcer2");
        for _, obj in pairs(objs) do
            local e = factory:createEnforcer1();
            e:setTransform(obj:getTransform());
            local ec = e:findTargetableComponent();
            ec.autoTarget = true;
            scene:addObject(e);
        end
    end);
end);
