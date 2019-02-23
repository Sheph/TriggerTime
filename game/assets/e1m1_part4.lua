local scientist1 = scene:getObjects("scientist1")[1];
local scientist2 = scene:getObjects("scientist2")[1];
local scientist3 = scene:getObjects("scientist3")[1];
local scientist4 = scene:getObjects("scientist4")[1];
local scientist5 = scene:getObjects("scientist5")[1];

function makeFlickerLamps(name)
    local function flickerTimeout2(self)
        if self.t <= 0 then
            for _, light in pairs(self.lights) do
                light.visible = self.defaultOn;
            end
            return;
        end
        for _, light in pairs(self.lights) do
            light.visible = not light.visible;
        end
        local t = 0.005 + math.random() * 0.005;
        self.t = self.t - t;
        addTimeoutOnce(t, flickerTimeout2, self);
    end

    local function flickerTimeout(self)
        self.t = 0.01 + math.random() * 0.15;
        flickerTimeout2(self);
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, self);
    end

    local insts = scene:getInstances(name);
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        addTimeoutOnce(1.0 + math.random() * 2.0, flickerTimeout, { lights = lights, t = 0, defaultOn = lights[1].visible });
    end
end

local function scientist5Walk1(fn, ...)
    local args = pack2(...);

    local p = scene:getObjects("path39")[1];
    scientist5.roamBehavior:reset();
    scientist5.roamBehavior.linearDamping = 4.0;
    scientist5.roamBehavior.dampDistance = 2.0;
    scientist5.roamBehavior.linearVelocity = 8.0;
    scientist5.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scientist5.roamBehavior:start();

    addTimeoutOnce(5.0, function()
        scientist5.angle = math.rad(-130);
        addTimeoutOnce(3.0, function()
            fn(unpack2(args));
        end);
    end);
end

local function scientist5Walk2(fn, ...)
    local args = pack2(...);

    local p = scene:getObjects("path40")[1];
    scientist5.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());

    addTimeoutOnce(5.0, function()
        scientist5.angle = scientist5.myAngle;
        addTimeoutOnce(3.0, function()
            fn(unpack2(args));
        end);
    end);
end

local function scientist5Loop()
    scientist5Walk1(function()
        scientist5Walk2(scientist5Loop);
    end);
end

function startFootage4()
    scene.lighting.ambientLight = {0.2, 0.2, 0.2, 1.0};
    scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

    local camCookie = startCam("target4", 35);

    local p = scene:getObjects("path35")[1];
    cam0.roamBehavior:reset();
    cam0.roamBehavior.changeAngle = false;
    cam0.roamBehavior.linearVelocity = 7.0;
    cam0.roamBehavior.loop = true;
    cam0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    cam0.roamBehavior:start();

    addTimeout(3.0, function(cookie)
        scientist1.angle = scientist1.angle + math.pi;
    end);

    p = scene:getObjects("path36")[1];
    scientist2.roamBehavior:reset();
    scientist2.roamBehavior.linearVelocity = 8.0;
    scientist2.roamBehavior.loop = true;
    scientist2.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scientist2.roamBehavior:start();

    p = scene:getObjects("path37")[1];
    scientist3.roamBehavior:reset();
    scientist3.roamBehavior.linearVelocity = 8.0;
    scientist3.roamBehavior.loop = true;
    scientist3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scientist3.roamBehavior:start();

    p = scene:getObjects("path38")[1];
    scientist4.roamBehavior:reset();
    scientist4.roamBehavior.linearVelocity = 8.0;
    scientist4.roamBehavior.loop = true;
    scientist4.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    scientist4.roamBehavior:start();

    scientist5.myAngle = scientist5.angle;
    addTimeoutOnce(2.0, function()
        scientist5Loop();
    end);

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog9.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog9.str2},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog9.str3},
        }, function ()
            stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                cam0.roamBehavior:reset();
                stopCam(camCookie);
                startFootage5();
            end);
        end);
    end);
end

-- main

makeFlickerLamps("flicker_lamp");
