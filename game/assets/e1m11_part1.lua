local military2 = scene:getObjects("military2")[1];
local military3 = scene:getObjects("military3")[1];
local military4 = scene:getObjects("military4")[1];
local military5 = scene:getObjects("military5")[1];
local military6 = scene:getObjects("military6")[1];
local military7 = scene:getObjects("military7")[1];
local military8 = scene:getObjects("military8")[1];
local military9 = scene:getObjects("military9")[1];
local part2Ready = false;
local part2Need = false;

local function openAirlock(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "airlock_door1_joint");
    local wj = findJoint(inst.joints, "airlock_wheel_joint");
    wj.motorSpeed = math.abs(wj.motorSpeed);
    addTimeout0(function(cookie, dt)
        if wj.jointAngle >= wj.upperLimit then
            cancelTimeout(cookie);
            j.motorSpeed = math.abs(j.motorSpeed);
            addTimeout0(function(cookie, dt)
                if j:getJointTranslation() >= j.upperLimit then
                    cancelTimeout(cookie);
                end
            end);
        end
    end);
end

local function makeAirlock(name)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "airlock_door1"),
        findObject(inst.objects, "airlock_door2"),
        findJoint(inst.joints, "airlock_door1_joint"),
        findJoint(inst.joints, "airlock_door2_joint"),
        -1, false);
end

local function part2()
    local p = scene:getObjects("path5_2")[1];
    military5.roamBehavior:reset();
    military5.roamBehavior.linearDamping = 4.0;
    military5.roamBehavior.dampDistance = 2.0;
    military5.roamBehavior.linearVelocity = 10.0;
    military5.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military5.roamBehavior:start();

    addTimeoutOnce(0.2, function()
        local p = scene:getObjects("path4_2")[1];
        military4.roamBehavior:reset();
        military4.roamBehavior.linearDamping = 4.0;
        military4.roamBehavior.dampDistance = 2.0;
        military4.roamBehavior.linearVelocity = 10.0;
        military4.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military4.roamBehavior:start();
    end);

    addTimeoutOnce(1.0, function()
        local p = scene:getObjects("path2_2")[1];
        military2.roamBehavior:reset();
        military2.roamBehavior.linearDamping = 4.0;
        military2.roamBehavior.dampDistance = 2.0;
        military2.roamBehavior.linearVelocity = 10.0;
        military2.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military2.roamBehavior:start();
    end);

    addTimeoutOnce(1.3, function()
        local p = scene:getObjects("path3_2")[1];
        military3.roamBehavior:reset();
        military3.roamBehavior.linearDamping = 4.0;
        military3.roamBehavior.dampDistance = 2.0;
        military3.roamBehavior.linearVelocity = 10.0;
        military3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military3.roamBehavior:start();
    end);
end

function startFootage1()
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};

    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
    local camCookie = startCam("target1", 50);

    local p = scene:getObjects("path6")[1];
    cam0.roamBehavior:reset();
    cam0.roamBehavior.changeAngle = false;
    cam0.roamBehavior.linearVelocity = 7.0;
    cam0.roamBehavior.loop = true;
    cam0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    cam0.roamBehavior:start();

    p = scene:getObjects("path2_1")[1];
    military2.roamBehavior:reset();
    military2.roamBehavior.linearDamping = 4.0;
    military2.roamBehavior.dampDistance = 2.0;
    military2.roamBehavior.linearVelocity = 10.0;
    military2.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military2.roamBehavior:start();

    p = scene:getObjects("path3_1")[1];
    military3.roamBehavior:reset();
    military3.roamBehavior.linearDamping = 4.0;
    military3.roamBehavior.dampDistance = 2.0;
    military3.roamBehavior.linearVelocity = 10.0;
    military3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military3.roamBehavior:start();

    p = scene:getObjects("path4_1")[1];
    military4.roamBehavior:reset();
    military4.roamBehavior.linearDamping = 4.0;
    military4.roamBehavior.dampDistance = 2.0;
    military4.roamBehavior.linearVelocity = 10.0;
    military4.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military4.roamBehavior:start();

    p = scene:getObjects("path5_1")[1];
    military5.roamBehavior:reset();
    military5.roamBehavior.linearDamping = 4.0;
    military5.roamBehavior.dampDistance = 2.0;
    military5.roamBehavior.linearVelocity = 10.0;
    military5.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military5.roamBehavior:start();

    addTimeoutOnce(3.0, function()
        local p = scene:getObjects("path6_1")[1];
        military6.roamBehavior:reset();
        military6.roamBehavior.linearDamping = 4.0;
        military6.roamBehavior.dampDistance = 2.0;
        military6.roamBehavior.linearVelocity = 10.0;
        military6.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military6.roamBehavior:start();

        p = scene:getObjects("path7_1")[1];
        military7.roamBehavior:reset();
        military7.roamBehavior.linearDamping = 4.0;
        military7.roamBehavior.dampDistance = 2.0;
        military7.roamBehavior.linearVelocity = 10.0;
        military7.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military7.roamBehavior:start();

        p = scene:getObjects("path8_1")[1];
        military8.roamBehavior:reset();
        military8.roamBehavior.linearDamping = 4.0;
        military8.roamBehavior.dampDistance = 2.0;
        military8.roamBehavior.linearVelocity = 10.0;
        military8.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military8.roamBehavior:start();

        p = scene:getObjects("path9_1")[1];
        military9.roamBehavior:reset();
        military9.roamBehavior.linearDamping = 4.0;
        military9.roamBehavior.dampDistance = 2.0;
        military9.roamBehavior.linearVelocity = 10.0;
        military9.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military9.roamBehavior:start();
    end);

    addTimeoutOnce(8.0, function()
        part2Ready = true;
        if part2Need then
            part2();
        end
    end);

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog139.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog139.str2},
        }, function ()
            part2Need = true;
            if part2Ready then
                part2();
            end
            showLowerDialog(
            {
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog140.str1},
            }, function ()
                stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                    cam0.roamBehavior:reset();
                    stopCam(camCookie);
                    startFootage2();
                end);
            end);
        end);
    end);
end

-- main

makeAirlock("door3");
openAirlock("door3");
makeAirlock("door4");
openAirlock("door4");
makeAirlock("door5");
openAirlock("door5");
