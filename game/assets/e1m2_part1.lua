local gun1 = scene:getInstances("gun1");
local spawner1 = scene:getInstances("spawner1");
local ha1Targets = scene:getObjects("ha1_target");

local function activateLight(light)
    light.visible = true;
    light.myColor = light.color[4];
    local tweening = SequentialTweening(true);
    tweening:addTweening(SingleTweening(0.4, const.EaseOutQuad, light.color[4], 1.0, false));
    tweening:addTweening(SingleTweening(0.4, const.EaseInQuad, 1.0, light.color[4], false));
    light.myTimer = addTimeout0(function(cookie, dt, self)
        local c = light.color;
        c[4] = tweening:getValue(self.t);
        light.color = c;
        self.t = self.t + dt;
    end, { t = 0 });
end

local function deactivateLight(light)
    if light.myTimer ~= nil then
        cancelTimeout(light.myTimer);
        light.myTimer = nil;
        local c = light.color;
        c[4] = light.myColor;
        light.color = c;
        light.visible = false;
    end
end

local function makeGun1(insts)
    for _, inst in pairs(insts) do
        local obj = findObject(inst.objects, "gun1_root");
        inst.weapon = WeaponHeaterComponent(const.SceneObjectTypeEnemyMissile);
        inst.weapon.damage = 60.0;
        inst.weapon.length = 20.0;
        inst.weapon.impulse = 50.0;
        inst.weapon.duration = 0.8;
        inst.weapon.interval = 0.0;
        inst.weapon.minWidth = 0.8;
        inst.weapon.maxWidth = 1.8;
        inst.weapon.haveSound = false;
        inst.weapon.measure = true;
        obj:addComponent(inst.weapon);
        inst.weapon:trigger(true);
    end
end

local function setupSpawner(inst)
    inst.myRoot = findObject(inst.objects, "spawner1_root");
    inst.myRod = findObject(inst.objects, "spawner1_rod");
    inst.myHoleCp = findObject(inst.objects, "spawner1_hole_cp");
    inst.mySpawn = findObjects(inst.objects, "spawner1_spawn");
    inst.myLight = inst.myRoot:findLightComponent():getLights()[1];

    inst.myMeas = MeasurementComponent();
    inst.myRod:addComponent(inst.myMeas);
    inst.myHeat = 0;
    inst.myHp = 1;
    inst.myOn = false;
    inst.mySpawnTimer = nil;
    inst.myFirstOn = true;

    inst.myHoleCp:findCollisionSensorComponent().listener = createSensorListener(function(other)
    end, function(other)
        local c = other:findPhysicsBodyComponent();
        if (c ~= nil) and (c.filterGroupIndex == -800) then
            c.filterGroupIndex = 0;
        end
    end);
end

local function enableSpawner(inst)
    addTimeout0(function(cookie, dt, self)
        if inst.myMeas.heaterDamageReceived == inst.myHeat then
            inst.myHp = inst.myHp - dt;
        else
            inst.myHp = inst.myHp + dt;
        end
        if inst.myHp > 1 then
            inst.myHp = 1;
        elseif inst.myHp < 0 then
            inst.myHp = 0;
        end
        inst.myHeat = inst.myMeas.heaterDamageReceived;
        inst.myRod.color = {1, 1 - inst.myHp, 1 - inst.myHp, 1};
        if not inst.myOn and (inst.myHp == 1) then
            inst.myOn = true;
            if not inst.myFirstOn then
                audio:playSound("crystal_activate.ogg");
            end
            inst.myFirstOn = false;
            activateLight(inst.myLight);
            inst.mySpawnTimer = addTimeout(0.25, function(cookie)
                for _, obj in pairs(inst.mySpawn) do
                    if (obj.mySpawned == nil) or (obj.mySpawned:dead()) then
                        if obj.myT == nil then
                            obj.myT = math.random(0, 3);
                        end
                        if obj.myT > 8 then
                            obj.myT = 0;
                            obj.mySpawned = factory:createScorp3();
                            addSpawnedEnemy(obj.mySpawned, obj);
                            obj.mySpawned:findPhysicsBodyComponent():setFilterGroupIndex(-800);
                            obj.mySpawned:applyLinearImpulse(obj.mySpawned:getDirection(150.0), obj.mySpawned.pos, true);
                        end
                        obj.myT = obj.myT + 0.25;
                    end
                end
            end);
        elseif inst.myOn and (inst.myHp == 0) then
            scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():removeAllHints();
            audio:playSound("alarm_off.ogg");
            inst.myOn = false;
            deactivateLight(inst.myLight);
            cancelTimeout(inst.mySpawnTimer);
            inst.mySpawnTimer = nil;
            for _, obj in pairs(inst.mySpawn) do
                obj.myT = nil;
            end
        end
    end);
end

-- main

scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent().timeout = 30.0;
for _, obj in pairs(ha1Targets) do
    scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHintTrigger(obj.pos, 1.0);
    scene:getObjects("ha1")[1]:findPuzzleHintAreaComponent():addHint(obj.pos);
end

if settings.skill <= const.SkillNormal then
    scene:getObjects("ha1")[1].active = true;
end

makeGun1(gun1);

for _ ,inst in pairs(spawner1) do
    setupSpawner(inst);
end

setSensorEnterListener("rock2_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("rock2_cp")[1]:getTransform();
    for _ ,inst in pairs(spawner1) do
        enableSpawner(inst);
    end
    local spawn = scene:getObjects("tb_spawn3");
    local mySpawned = nil;
    local myT = 0;
    local myDoneT = 0;
    addTimeout(0.25, function(cookie)
        if (mySpawned == nil) or (mySpawned:dead()) then
            if myT > 8 then
                myT = 0;
                mySpawned = factory:createTetrobot2();
                addSpawnedEnemy(mySpawned, spawn[math.random(1, #spawn)]);
                mySpawned:findPhysicsBodyComponent():setFilterGroupIndex(-300);
            end
            myT = myT + 0.25;
        end
        for _ ,inst in pairs(spawner1) do
            if inst.myOn then
                myDoneT = 0;
                return;
            end
        end
        myDoneT = myDoneT + 0.25;
        if myDoneT > 3 then
            cancelTimeout(cookie);
            for _, inst in pairs(gun1) do
                inst.weapon:trigger(false);
            end
            closeStone("door5_joint");
            scene:getObjects("ga2")[1]:findGoalAreaComponent():addGoal(scene:getObjects("goal2")[1].pos);
            gameShell:setAchieved(const.AchievementGravityGunExpert);
        end
    end);
end);

setSensorEnterListener("rock3_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("rock3_cp")[1]:getTransform();
    openStone("door5_joint");
end);
