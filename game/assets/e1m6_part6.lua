local crystal2 = scene:getInstances("crystal2");
local crystal1cpObjs = {};
local crystal2cpObjs = {};
local crystal1Lights = {};
local crystal2Lights = {};
local crystal1DoneLight = scene:getObjects("terrain0")[1]:findLightComponent():getLights("crystal1_done_light")[1];
local crystal2DoneLight = scene:getObjects("terrain0")[1]:findLightComponent():getLights("crystal2_done_light")[1];
local press4Joint = scene:getJoints("press4_joint")[1];
local keeper1 = scene:getObjects("keeper1")[1];
local keeper2 = scene:getObjects("keeper2")[1];

local function processLightOn(light)
    light.visible = true;
    if crystal1DoneLight.visible and crystal2DoneLight.visible then
        scene:getObjects("keeper1_cp")[1].active = true;
        scene.respawnPoint = scene:getObjects("keeper1_cp")[1]:getTransform();
        scene.player.linearDamping = 6.0;
        scene.cutscene = true;
        showLowerDialog(
        {
            {"player", tr.dialog73.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog73.str2}
        }, function ()
            scene.cutscene = false;
        end);
    end
    addTimeoutOnce(1.5, function()
        audio:playSound("crystal_activate.ogg");
        local tweening = SequentialTweening(true);
        tweening:addTweening(SingleTweening(0.6, const.EaseOutQuad, 0.0, 1.0, false));
        tweening:addTweening(SingleTweening(0.6, const.EaseInQuad, 1.0, 0.0, false));
        addTimeout0(function(cookie, dt, self)
            local c = light.color;
            c[4] = tweening:getValue(self.t);
            light.color = c;
            self.t = self.t + dt;
        end, { t = 0 });
    end);
end

-- main

for i = 1, 5, 1 do
    crystal1Lights[i] = scene:getObjects("terrain0")[1]:findLightComponent():getLights("crystal1_light"..i)[1];
    crystal2Lights[i] = scene:getObjects("terrain0")[1]:findLightComponent():getLights("crystal2_light"..i)[1];
end

makeDoor("door9", false);
makeDoorTrigger("door9_cp", "door9");

makeDoor("door10", false);
makeDoorTrigger("door10_cp", "door10");

makeDoor("door11", true);
makeDoor("door12", true);

for _, inst in pairs(crystal2) do
    findObject(inst.objects, "crystal").gravityGunAware = true;
end

setSensorEnterListener("crystal1_cp", false, function(other)
    if other.name ~= "crystal" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    other.gravityGunAware = false;
    if crystal1cpObjs[other.cookie] == nil then
        crystal1cpObjs[other.cookie] = { count = 1, obj = other };
    else
        crystal1cpObjs[other.cookie].count = crystal1cpObjs[other.cookie].count + 1;
    end
    local sz = table.size(crystal1cpObjs);
    for i = 1, 5, 1 do
        crystal1Lights[i].visible = (i <= sz);
    end
    if (sz == 5) and (not crystal1DoneLight.visible) then
        closeDoor("door11", true);
        processLightOn(crystal1DoneLight);
    end
end);

setSensorEnterListener("crystal2_cp", false, function(other)
    if other.name ~= "crystal" then
        return;
    end
    local aw = scene.player:findPlayerComponent().altWeapon;
    if aw.heldObject == other then
        aw:cancel();
    end
    other.gravityGunAware = false;
    if crystal2cpObjs[other.cookie] == nil then
        crystal2cpObjs[other.cookie] = { count = 1, obj = other };
    else
        crystal2cpObjs[other.cookie].count = crystal2cpObjs[other.cookie].count + 1;
    end
    local sz = table.size(crystal2cpObjs);
    for i = 1, 5, 1 do
        crystal2Lights[i].visible = (i <= sz);
    end
    if (sz == 5) and (not crystal2DoneLight.visible) then
        closeDoor("door12", true);
        processLightOn(crystal2DoneLight);
    end
end);

press4Joint.myLowerLimit = press4Joint.lowerLimit;
press4Joint.myUpperLimit = press4Joint.upperLimit;
local tn = press4Joint:getJointTranslation();
press4Joint:setLimits(tn, tn + 0.01);

setSensorListener("press4_cp", function(other, self)
    press4Joint:setLimits(press4Joint.myLowerLimit, press4Joint.myUpperLimit);
    press4Joint.motorEnabled = true;
    self.timer = addTimeout0(function(cookie, dt)
        if press4Joint.motorSpeed < 0 then
            if (press4Joint:getJointTranslation() <= press4Joint.lowerLimit) or
               (press4Joint:getJointSpeed() >= -(math.abs(press4Joint.motorSpeed) - 5)) then
                press4Joint.motorSpeed = math.abs(press4Joint.motorSpeed);
            end
        else
            if (press4Joint:getJointTranslation() >= press4Joint.upperLimit) then
                press4Joint.motorSpeed = -math.abs(press4Joint.motorSpeed);
            end
        end
    end);
end, function(other, self)
    cancelTimeout(self.timer);
    local tn = press4Joint:getJointTranslation();
    press4Joint:setLimits(tn, tn + 0.01);
    press4Joint.motorEnabled = false;
end, { timer = nil });

setSensorEnterListener("keeper1_cp", true, function(other)
    local laser4 = scene:getObjects("laser4");
    for _, obj in pairs(laser4) do
        obj.active = true;
        obj.visible = true;
    end
    scene:getObjects("door10_cp")[1].active = false;
    startMusic("action10.ogg", true);
    scene.respawnPoint = scene:getObjects("keeper1_cp")[1]:getTransform();
    keeper1:findKeeperComponent().autoTarget = true;
    keeper2:findKeeperComponent().autoTarget = true;
    addTimeout0(function(cookie, dt, self)
        if self.last == 0 then
            if keeper1:dead() then
                self.last = 2;
            elseif keeper2:dead() then
                self.last = 1;
            end
        end
        if keeper1:alive() or keeper2:alive() then
            return;
        end
        cancelTimeout(cookie);
        startAmbientMusic(true);
        addTimeoutOnce(1.0, function()
            openDoor("door10", true);
        end);
        local last = self.last;
        addTimeout0(function(cookie, dt)
            local kc;
            if last == 1 then
                kc = keeper1:findPhysicsBodyComponent();
            else
                kc = keeper2:findPhysicsBodyComponent();
            end
            if kc == nil then
                cancelTimeout(cookie);
                local k = factory:createPowerupInventory(const.InventoryItemRedKey);
                if self.last == 1 then
                    k.pos = keeper1.pos;
                else
                    k.pos = keeper2.pos;
                end
                scene:addObject(k);
            end
        end);
    end, { last = 0 });
    addTimeoutOnce(0.5, function()
        openHatch("hatch1");
        addTimeoutOnce(0.5, function()
            keeper1:findKeeperComponent():crawlOut();
            addTimeoutOnce(1.8, function()
                closeHatch("hatch1");
            end);
        end);
    end);
    addTimeoutOnce(3.0, function()
        openHatch("hatch2");
        addTimeoutOnce(0.5, function()
            keeper2:findKeeperComponent():crawlOut();
            addTimeoutOnce(1.8, function()
                closeHatch("hatch2");
            end);
        end);
    end);
end);
