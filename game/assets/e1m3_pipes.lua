local fireDamage = 400.0;
local puzzleFireDamage = 5000.0;
local blastImpulse = 100.0;
local blastDamage = 250.0;
local fireOff = false;

local function setupFire(fireName, scale, on)
    if fireOff then
        return;
    end
    local objs = scene:getObjects(fireName);
    for _, obj in pairs(objs) do
        local f = obj:findWeaponFlamethrowerComponent();
        f.damage = fireDamage;
        f.haveLight = true;
        f.haveSound = false;
        f:setScale(scale, 0);
        f:trigger(on);
    end
end

local function setupPuzzleFire(fireName, scale, on)
    if fireOff then
        return;
    end
    local objs = scene:getObjects(fireName);
    for _, obj in pairs(objs) do
        local f = obj:findWeaponFlamethrowerComponent();
        f.damage = puzzleFireDamage;
        f:setColors({0.0, 0.6117647, 1.0, 1.0}, {0.0, 0.03529412, 1.0, 1.0});
        f.haveLight = true;
        f.haveSound = false;
        f:setScale(scale, 0);
        f:trigger(on);
    end
end

local function explodePipe(pipeName, holeName, fireName, fireScale, puzzle, noDamage)
    if fireOff then
        return;
    end
    local pipeObj = scene:getObjects(pipeName)[1];
    local fObj = scene:getObjects(fireName)[1];
    local e = factory:createExplosion1(const.zOrderExplosion);
    e.pos = fObj.pos;
    local ec = e:findExplosionComponent();
    local damage = blastDamage;
    if noDamage then
        damage = 0;
    end
    ec:setBlast(pipeObj, blastImpulse, damage, { const.SceneObjectTypePlayer, const.SceneObjectTypeEnemy });
    scene:addObject(e);
    pipeObj:findRenderQuadComponents(holeName)[1].visible = true;
    if puzzle then
        setupPuzzleFire(fireName, fireScale, true);
    else
        setupFire(fireName, fireScale, true);
    end
end

local function setupValve(phName, fireName, altFireName, fn, ...)
    if fireOff then
        return;
    end
    local ph = scene:getObjects(phName)[1]:findPlaceholderComponent();
    local objs = scene:getObjects(fireName);
    if altFireName ~= nil then
        objs = table.copy(objs, scene:getObjects(altFireName));
    end
    ph.listener = createSensorListener(function(other, self)
        local items = {};
        local a = -90;
        if objs[1]:findWeaponFlamethrowerComponent().triggerHeld then
            a = 90;
        end
        table.insert(items, { pos = vec2(0, 0), angle = math.rad(a), height = 6, image = "common2/arrow_button.png" });
        self.choice = showChoice(items, function(i)
            local isOn = not objs[1]:findWeaponFlamethrowerComponent().triggerHeld;
            if isOn then
                audio:playSound("fire_on.ogg");
            else
                audio:playSound("fire_off.ogg");
            end
            for _, obj in pairs(objs) do
                local f = obj:findWeaponFlamethrowerComponent();
                f:trigger(not f.triggerHeld);
            end
            ph.active = false;
            ph.visible = false;
            addTimeoutOnce(1.0, function()
                ph.active = true;
                ph.visible = true;
            end);
            if fn ~= nil then
                fn(isOn, unpack2(self.args));
            end;
        end);
    end, function(other, self)
        if self.choice ~= nil then
            self.choice:removeFromParent();
        end
    end, { choice = nil, args = pack2(...) });
end

local function startFireTimer(fireName, toScale, toDuration, toDelay, fromDuration, fromDelay)
    if fireOff then
        return;
    end
    local objs = scene:getObjects(fireName);
    for _, obj in pairs(objs) do
        local f = obj:findWeaponFlamethrowerComponent();
        obj.myScale = f.scale;
    end
    local scaleTo;
    local function scaleFrom()
        if fireOff then
            return;
        end
        for _, obj in pairs(objs) do
            local f = obj:findWeaponFlamethrowerComponent();
            f:setScale(obj.myScale, fromDuration);
        end
        addTimeoutOnce(fromDuration + toDelay, scaleTo);
    end
    scaleTo = function()
        if fireOff then
            return;
        end
        for _, obj in pairs(objs) do
            local f = obj:findWeaponFlamethrowerComponent();
            f:setScale(toScale, toDuration);
        end
        addTimeoutOnce(toDuration + fromDelay, scaleFrom);
    end
    addTimeoutOnce(toDelay, scaleTo);
end

local function startFireTimerSimple(fireName, toScale, duration, delay)
    startFireTimer(fireName, toScale, duration, delay, duration, delay);
end

-- main

setupFire("fire1", 0.8, true);

setSensorEnterListener("fire2_cp", false, function(other, self)
    if not self.started then
        explodePipe("pipe2", "hole1", "fire2", 0.5);
        addTimeoutOnce(0.3, function()
            explodePipe("pipe2", "hole2", "fire3", 0.5);
        end);
        resetCartRespawn();
        scene.respawnPoint = scene:getObjects("fire2_cp")[1]:getTransform();
        self.started = true;
    end
    -- reset puzzle 1
    setupPuzzleFire("fire5", 0.8, true);
    setupPuzzleFire("fire6", 0.8, false);
    setupPuzzleFire("fire7", 0.8, false);
end, { started = false });

setupFire("fire4", 0.8, true);
setupValve("valve1_ph", "fire4");

-- puzzle 1

setupValve("valve2_ph", "fire6", "fire7");
setupValve("valve3_ph", "fire5", "fire6");
setupValve("valve4_ph", "fire7", "fire5");

setSensorEnterListener("fire8_cp", false, function(other, self)
    if not self.started then
        explodePipe("pipe9", "hole1", "fire8", 0.8, true);
        scene.respawnPoint = scene:getObjects("fire8_cp")[1]:getTransform();
        self.started = true;
    end
end, { started = false });

local masterValvePh = scene:getObjects("valve5_ph")[1]:findPlaceholderComponent();
masterValvePh.listener = createSensorEnterListener(true, function(other)
    for i = 0,4,1 do
        addTimeoutOnce(0.2 * i, function()
            audio:playSound("fire_off.ogg");
        end);
    end
    for i = 1,5,1 do
        local ph = scene:getObjects("valve"..i.."_ph")[1]:findPlaceholderComponent();
        ph.active = false;
        ph.visible = false;
    end
    for i = 1,8,1 do
        local objs = scene:getObjects("fire"..i);
        for _, obj in pairs(objs) do
            local f = obj:findWeaponFlamethrowerComponent();
            f:trigger(false);
        end
    end
    setSensorEnterListener("baby7_cp", true, babySpawnCp, "baby7", 0.5);
    setSensorEnterListener("baby8_cp", true, babySpawnCp, "baby8", 0.5);
    setSensorEnterListener("fire_end_cp", true, function()
        setCartRespawn();
        cart0Proceed();
    end);
    fireOff = true;
    addTimeoutOnce(2.0, function()
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog37.str1},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);
