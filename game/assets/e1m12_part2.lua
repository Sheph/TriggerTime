local entryDoor1 = scene:getInstances("entry_door1")[1];
local military0 = scene:getObjects("military0")[1];

-- main

if (math.random(1, 2) == 1) then
    local ws1 = factory:createWeaponStation(const.WeaponTypeSuperShotgun, 10);
    ws1:setTransformRecursive(scene:getObjects("ws1")[1]:getTransform());
    scene:addObject(ws1);
else
    local ws1 = factory:createWeaponStation(const.WeaponTypeMachineGun, 100);
    ws1:setTransformRecursive(scene:getObjects("ws1")[1]:getTransform());
    scene:addObject(ws1);
end

if settings.developer == 0 then
    makeAirlock(entryDoor1, true);

    scene.cutscene = true;
    scene.player.linearDamping = 6.0;
    local p = scene:getObjects("entry_path1")[1];
    scene.player.roamBehavior:reset();
    scene.player.roamBehavior.linearVelocity = 15.0;
    scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scene.player.roamBehavior:start();

    setSensorEnterListener("entry_cp1", true, function(other)
        scene.player.roamBehavior.linearDamping = 6.0;
        scene.player.roamBehavior:damp();
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
                {"ally", tr.str12, "common1/portrait_jake.png", tr.dialog144.str1},
                {"ally", tr.str12, "common1/portrait_jake.png", tr.dialog144.str2},
                {"ally", tr.str12, "common1/portrait_jake.png", tr.dialog144.str3},
            }, function ()
                local p = scene:getObjects("entry_path2")[1];
                scene.player.roamBehavior:reset();
                scene.player.roamBehavior.linearVelocity = 15.0;
                scene.player.roamBehavior.linearDamping = 6.0;
                scene.player.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                scene.player.roamBehavior:start();
                addTimeoutOnce(1.0, function()
                    military0.angle = oldAngle;
                end);
            end)
        end);
    end);

    setSensorEnterListener("entry_cp2", true, function(other)
        closeMazeAirlock(entryDoor1, true);
        scene.player.roamBehavior.linearDamping = 6.0;
        scene.player.roamBehavior:damp();
        addTimeoutOnce(0.5, function()
            scene.camera:findCameraComponent():zoomTo(40, const.EaseOutQuad, 1.5);
            scene.player.roamBehavior:reset();
            scene.cutscene = false;
        end);
    end);
else
    makeAirlock(entryDoor1, false);
end
