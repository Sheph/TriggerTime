local maze1data = { spawned = false };
local maze23data = { spawned = false };
local maze45data = { spawned = true };
local maze6data = { spawned = true };
local spiders = {};
local woundedRc = scene:getObjects("wounded1")[1]:findRenderQuadComponents("deadbody")[1];

local function mazeSpiderCp(other, self, spawn)
    if self.spawned then
        return;
    end
    if #spiders == 0 then
        startMusic("action5.ogg", false);
    end
    local obj = factory:createSpider();
    obj:setTransformRecursive(scene:getObjects(spawn)[1]:getTransform());
    local s = obj:findSpiderComponent();
    s.autoTarget = true;
    scene:addObject(obj);
    table.insert(spiders, obj);
    self.spawned = true;
end

local function spiderScared(other, retreat)
    if other.parent ~= nil then
        other = other.parent;
    end
    local s = other:findSpiderComponent();
    if s == nil then
        return;
    end
    if not s.autoTarget then
        return;
    end
    s.target = nil;
    s.autoTarget = false;
    addTimeoutOnce(1.0, function()
        s:angry();
        addTimeoutOnce(3, function()
            s:rotate();
            s.target = scene:getObjects(retreat)[1];
            s.legVelocity = 200.0;
            s.patrol = true;
            addTimeoutOnce(5, function()
                other:removeFromParent();
            end);
        end);
    end);
end;

-- main

local woundedAc = AnimationComponent(woundedRc.drawable);
woundedAc:addAnimation(const.AnimationDefault, "scientist_wounded_idle", 1);
woundedAc:addAnimation(const.AnimationDefault + 1, "scientist_wounded_die", 1);
woundedAc:startAnimation(const.AnimationDefault);
woundedRc.parent:addComponent(woundedAc);

makeAirlock("door3", true);

makeKeyDoor("red_door", "red_key_ph", function()
    local objs = scene:getObjects("red_door_laser");
    for _, obj in pairs(objs) do
        obj:addComponent(FadeOutComponent(1.0));
    end
end);

setSensorEnterListener("room2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("room2_spawn_cp")[1]:getTransform();
    closeAirlock("door3", true);
end);

setSensorEnterListener("spider3_scared_cp", false, spiderScared, "spider3_retreat");

setSensorEnterListener("room2_spawn_cp", false, function(other)
    local cc = scene.camera:findCameraComponent();
    for _, spider in pairs(spiders) do
        if not cc:rectVisible(spider.pos, 12.0, 12.0) then
            spider:removeFromParent();
        end
    end
    if #spiders > 0 then
        startAmbientMusic(true);
    end
    maze1data.spawned = false;
    maze23data.spawned = false;
    maze45data.spawned = true;
    maze6data.spawned = true;
    spiders = {};
end);

setSensorEnterListener("maze1_cp", false, mazeSpiderCp, maze1data, "spider3_spawn");
setSensorEnterListener("maze2_cp", false, mazeSpiderCp, maze23data, "spider4_spawn");
setSensorEnterListener("maze3_cp", false, mazeSpiderCp, maze23data, "spider5_spawn");

setSensorEnterListener("spider4_scared_cp", false, spiderScared, "spider4_retreat");

setSensorEnterListener("room3_spawn_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("room3_spawn_cp")[1]:getTransform();
    local cc = scene.camera:findCameraComponent();
    for _, spider in pairs(spiders) do
        if not cc:rectVisible(spider.pos, 12.0, 12.0) then
            spider:removeFromParent();
        end
    end
    if #spiders > 0 then
        startAmbientMusic(true);
    end
    maze1data.spawned = true;
    maze23data.spawned = true;
    maze45data.spawned = false;
    maze6data.spawned = false;
    spiders = {};
end);

setSensorEnterListener("wounded1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.player.angle = (scene:getObjects("wounded1")[1].pos - scene.player.pos):angle();
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str4, "factory2/portrait_wounded0.png", tr.dialog27.str1},
            {"ally", tr.str4, "factory2/portrait_wounded0.png", tr.dialog27.str2},
            {"player", tr.dialog27.str3}
        }, function ()
            woundedAc:startAnimation(const.AnimationDefault + 1);
            showLowerDialog(
            {
                {"player", tr.dialog27.str4},
                {"player", tr.dialog27.str5}
            }, function ()
                scene.cutscene = false;
                scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
            end);
        end);
    end);
end);


setSensorEnterListener("maze4_cp", false, mazeSpiderCp, maze45data, "spider6_spawn");
setSensorEnterListener("maze5_cp", false, mazeSpiderCp, maze45data, "spider7_spawn");
setSensorEnterListener("maze6_cp", false, mazeSpiderCp, maze6data, "spider8_spawn");

setSensorEnterListener("spider5_scared_cp", false, spiderScared, "spider3_retreat");

setSensorEnterListener("room4_spawn_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("room4_spawn_cp")[1]:getTransform();
    local cc = scene.camera:findCameraComponent();
    for _, spider in pairs(spiders) do
        if not cc:rectVisible(spider.pos, 12.0, 12.0) then
            spider:removeFromParent();
        end
    end
    if #spiders > 0 then
        startAmbientMusic(true);
    end
    maze1data.spawned = true;
    maze23data.spawned = true;
    maze45data.spawned = true;
    maze6data.spawned = true;
    spiders = {};
end);
