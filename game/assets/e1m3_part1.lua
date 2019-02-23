-- main

makeAirlock("door2", true);

setSensorEnterListener("room1_cp", true, function(other)
    setAmbientMusic("ambient3.ogg");
    startAmbientMusic(true);
    scene.respawnPoint = scene:getObjects("room1_cp")[1]:getTransform();
    closeAirlock("door2", true);
end);

setSensorEnterListener("blood1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog24.str1}
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("scream1_cp", true, function(other)
    audio:playSound("scream1.ogg");
end);

setSensorEnterListener("dead_engineer_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.player.angle = (scene:getObjects("dead_engineer_cp")[1].pos - scene.player.pos):angle();
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog25.str1}
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("spider1_cp", true, function(other)
    audio:playSound("spider_passby.ogg");
    local obj = factory:createSpider();
    obj:setTransformRecursive(scene:getObjects("spider1_spawn")[1]:getTransform());
    local s = obj:findSpiderComponent();
    s.target = scene:getObjects("spider1_target")[1];
    s.legVelocity = 150.0;
    s.patrol = true;
    scene:addObject(obj);
end);

setSensorEnterListener("spider1_target_cp", true, function(other)
    other:removeFromParentRecursive();
end);

setSensorEnterListener("spider2_cp", true, function(other)
    local obj = factory:createSpider();
    obj:setTransformRecursive(scene:getObjects("spider2_spawn")[1]:getTransform());
    local s = obj:findSpiderComponent();
    s.target = scene:getObjects("spider2_target")[1];
    local legVelocity = s.legVelocity;
    s.legVelocity = 30.0;
    s.patrol = true;
    scene:addObject(obj);

    scene.player.linearDamping = 6.0;
    scene.player.angle = (scene:getObjects("spider2_cp")[1].pos - scene.player.pos):angle();
    scene.cutscene = true;

    stopMusic(true);
    addTimeoutOnce(1.5, function()
        audio:playSound("spider_encounter.ogg");
    end);

    addTimeoutOnce(6.5, function()
        s:angry();
        addTimeoutOnce(3, function()
            scene.cutscene = false;
            s.legVelocity = legVelocity;
            s.patrol = false;
            s.target = nil;
            s.autoTarget = true;
            startMusic("action5.ogg", false);
        end);
    end);
end);

setSensorEnterListener("spider2_scared_cp", false, function(other, self)
    if other.parent ~= nil then
        other = other.parent;
    end
    if other == self.obj then
        return;
    end
    local s = other:findSpiderComponent();
    if s == nil then
        return;
    end
    s.target = nil;
    s.autoTarget = false;
    self.obj = other;
    addTimeoutOnce(1.0, function()
        s:angry();
        addTimeoutOnce(3, function()
            s:rotate();
            s.target = scene:getObjects("spider2_spawn")[1];
            s.legVelocity = 200.0;
            s.patrol = true;
            startAmbientMusic(true);
            addTimeoutOnce(2, function()
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                showLowerDialog(
                {
                    {"player", tr.dialog26.str1},
                    {"player", tr.dialog26.str2},
                    {"player", tr.dialog26.str3},
                }, function ()
                    scene.cutscene = false;
                end);
            end);
            addTimeoutOnce(5, function()
                other:removeFromParentRecursive();
            end);
        end);
    end);
end, { obj = nil });
