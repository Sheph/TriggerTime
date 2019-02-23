local bossBlock1 = scene:getObjects("boss_block1")[1];
local bossDoor1 = bossInst:getInstances("boss_door1")[1];
local boss = scene:getObjects("boss1")[1];

-- main

makeAirlock(bossDoor1, false);

setSensorEnterListener("boss1_cp", true, function(other)
    playerInDoor = true;
    closeMazeAirlock(bossDoor, true);
    scene.respawnPoint = scene:getObjects("boss1_cp")[1]:getTransform();

    scene.player.linearDamping = 6.0;
    scene.player.angle = (boss.pos - scene.player.pos):angle();
    scene.cutscene = true;

    local p1 = bossBlock1.pos + bossBlock1:getDirection(-19.0 - 6) + bossBlock1:getDirection(-34.0 - 6):perpendicular();
    local p2 = bossBlock1.pos + bossBlock1:getDirection(49.0 + 6) + bossBlock1:getDirection(34.0 + 6):perpendicular();

    scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 1.5);
    scene.camera:findCameraComponent():setConstraint(vec2(math.min(p1.x, p2.x), math.min(p1.y, p2.y)),
        vec2(math.max(p1.x, p2.x), math.max(p1.y, p2.y)));
    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.5);

    startMusic("action3.ogg", true);

    addTimeoutOnce(1.5, function()
        boss:findTargetableComponent().target = scene:getObjects("boss1_center")[1];
        boss:findTargetableComponent().patrol = true;
        addTimeoutOnce(7.5, function()
            boss:findTargetableComponent().target = nil;
            boss:findTargetableComponent().autoTarget = true;
            scene.cutscene = false;
        end);
    end);
    addTimeout(0.25, function(cookie)
        if not boss:findBossBeholderComponent().deathFinished then
            return;
        end
        cancelTimeout(cookie);
        addTimeoutOnce(1.0, function()
            startAmbientMusic(true);
            scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
            scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 2.0);
            scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 2.0);
            openMazeAirlock(bossDoor1, true);
        end);
    end);
end);

setSensorEnterListener("boss2_cp", true, function(other)
    completeLevel(0.25, tr.str52, tr.str31, "e1m7");
end);
