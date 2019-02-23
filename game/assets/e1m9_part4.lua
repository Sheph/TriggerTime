local terrain0 = scene:getObjects("terrain0")[1];
local jars = {};
local display5 = setupDisplay("display5");
local snake1 = scene:getObjects("snake1")[1];
local snake1eye = snake1:getObjects("eye")[1];
local snakePaths = {{}, {}, {}};

-- main

local rc = terrain0:findRenderTerrainComponents("toxic1")[1];
local ac = AnimationComponent(rc.drawable);
ac:addAnimation(const.AnimationDefault, "toxic2", 1);
ac:startAnimation(const.AnimationDefault);
terrain0:addComponent(ac);

makeAirlock("door27", true);
makeAirlock("door28", true);
makeAirlock("door29", false);

for i = 44, 54, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    makeJar1WithCreature(jars[i], "cr"..i);
end

scene:getObjects("snake_path_init")[1].pos = vec2(300, 500);
for i = 1,3,1 do
    for j = 1,3,1 do
        local paths = scene:getObjects("snake_path"..i.."_"..j);
        paths = table.copy(paths, scene:getObjects("snake_path"..i.."_"..j.."b"));
        for _, path in pairs(paths) do
            if path.active then
                path.pos = vec2(300, 500);
                table.insert(snakePaths[i], {path, j});
            end
        end
    end
end

local objs = { snake1 };
local objs = table.copy(objs, snake1:getObjects("tentacle_base")[1]:findRenderTentacleComponent().objects);
snake1.movers = {};
snake1.joints = {};
for _, obj in pairs(objs) do
    local dummy = factory:createDummy();
    dummy:setTransform(obj:getTransform());
    scene:addObject(dummy);
    local joint;
    if obj == snake1 then
        joint = scene:addMotorJoint(dummy, obj, vec2(0, 0), 3000.0, 3000.0, 0.1, false);
        dummy.head = true;
    else
        joint = scene:addMotorJoint(dummy, obj, vec2(0, 0), 300.0, 300.0, 0.1, false);
        dummy.head = false;
    end
    table.insert(snake1.movers, dummy);
    table.insert(snake1.joints, joint);
end

setSensorEnterListener("snake3_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("snake3_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("snake3");
    end
end, { first = true });

local playerInCenter = false;

setSensorListener("snake1_cp", function(other, self)
    playerInCenter = true;
end, function(other)
    playerInCenter = false;
end);

setSensorEnterListener("snake2_cp", true, function(other)
    closeAirlock("door27", true);
    closeAirlock("door28", false);
    scene.respawnPoint = scene:getObjects("snake2_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.camera:findCameraComponent():zoomTo(45, const.EaseOutQuad, 1.0);
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("cam_target4")[1], const.EaseOutQuad, 1.0);
        addTimeoutOnce(2.0, function()
            display5:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.5, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog129.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog129.str2},
                }, function ()
                    local angry = true;
                    startMusic("action14.ogg", true);
                    snake1.path = scene:getObjects("snake_path_init")[1];
                    snake1.out = 2;
                    snake1:findSnakeComponent():setWalk();
                    for _, mover in pairs(snake1.movers) do
                        mover.roamBehavior:reset();
                        mover.roamBehavior.linearVelocity = 17.0;
                        if not mover.head then
                            mover.roamBehavior.angleOffset = math.pi;
                        end
                        mover.roamBehavior:changePath(snake1.path:findPathComponent().path, snake1.path:getTransform());
                        mover.roamBehavior:start();
                    end
                    scene.camera:findCameraComponent():follow(snake1, const.EaseInOutQuad, 1.0);
                    addTimeoutOnce(17.0, function()
                        scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.0);
                        addTimeoutOnce(1.0, function()
                            scene.cutscene = false;
                            snake1:findTargetableComponent().autoTarget = true;
                            addTimeoutOnce(2.0, function()
                                display5:startAnimation(const.AnimationDefault);
                            end);
                        end);
                    end);
                    addTimeout0(function(cookie, dt, self)
                        if snake1eye:dead() then
                            cancelTimeout(cookie);
                            for _, mover in pairs(snake1.movers) do
                                mover.roamBehavior:reset();
                                mover.linearVelocity = vec2(0, 0);
                                mover.angularVelocity = 0;
                            end
                            for _, j in pairs(snake1.joints) do
                                j:remove();
                            end
                            snake1:findSnakeComponent():setDie(10000.0, 10000.0, 0.0, 4000.0);
                            return;
                        end
                        local mover = snake1.movers[self.i];
                        if not mover.roamBehavior.finished then
                            return;
                        end
                        if self.i == 1 then
                            if angry then
                                snake1.path = scene:getObjects("snake_path2_3b")[1];
                                snake1.out = 3;
                            else
                                local sp = snakePaths[snake1.out][math.random(1, #snakePaths[snake1.out])];
                                snake1.path = sp[1];
                                snake1.out = sp[2];
                            end
                        end
                        mover.roamBehavior:reset();
                        mover.roamBehavior.linearVelocity = 17.0;
                        if not mover.head then
                            mover.roamBehavior.angleOffset = math.pi;
                        end
                        mover.roamBehavior:changePath(snake1.path:findPathComponent().path, snake1.path:getTransform());
                        if (self.i == 1) and playerInCenter then
                            snake1:findSnakeComponent():setIdle();
                            for _, mover in pairs(snake1.movers) do
                                mover.roamBehavior.paused = true;
                                mover.linearVelocity = vec2(0, 0);
                                mover.angularVelocity = 0;
                            end
                            addTimeoutOnce(1.0, function()
                                if snake1eye:dead() then
                                    return;
                                end
                                local tout;
                                local tout2;
                                if angry then
                                    snake1:findSnakeComponent():setAngry(45000.0, 45000.0, 0.35);
                                    tout = 2.0;
                                    tout2 = 2.0;
                                    scene.camera:findCameraComponent().target = nil;
                                else
                                    snake1:findSnakeComponent():setMegaShoot(15000.0, 15000.0, 1.0);
                                    tout = 5.0;
                                    tout2 = 1.0;
                                end
                                addTimeoutOnce(tout, function()
                                    if snake1eye:dead() then
                                        return;
                                    end
                                    snake1:findSnakeComponent():setIdle();
                                    addTimeoutOnce(tout2, function()
                                        if snake1eye:dead() then
                                            return;
                                        end
                                        if angry then
                                            scene.camera:findCameraComponent().target = snake1;
                                        end
                                        angry = false;
                                        audio:playSound("snake_walk.ogg");
                                        snake1:findSnakeComponent():setWalk();
                                        mover.roamBehavior:start();
                                        for _, mover in pairs(snake1.movers) do
                                            mover.roamBehavior.paused = false;
                                        end
                                    end);
                                end);
                            end);
                        else
                            mover.roamBehavior:start();
                        end
                        self.i = self.i + 1;
                        if self.i > #snake1.movers then
                            self.i = 1;
                        end
                    end, { i = 1 });
                    addTimeout(0.25, function(cookie)
                        if not snake1:findSnakeComponent().deathFinished then
                            return;
                        end
                        cancelTimeout(cookie);
                        startAmbientMusic(true);
                        addTimeoutOnce(1.5, function()
                            scene:getObjects("snake_room_blocker")[1].active = false;
                            openAirlock("door27", true);
                            openAirlock("door28", false);
                            scene.camera:findCameraComponent():zoomTo(35, const.EaseOutQuad, 1.0);
                            bossDone(3);
                        end);
                    end);
                end);
            end);
        end);
    end);
end);
