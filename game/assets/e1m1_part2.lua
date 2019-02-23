local gun1 = scene:getInstances("gun1")[1];
local gun2 = scene:getInstances("gun2")[1];
local military10 = scene:getObjects("military10")[1];
local military11 = scene:getObjects("military11")[1];
local military12 = scene:getObjects("military12")[1];
local military13 = scene:getObjects("military13")[1];
local military14 = scene:getObjects("military14")[1];
local military15 = scene:getObjects("military15")[1];
local keyl0 = scene:getObjects("keyl0")[1];
local stopMilitaryLoop = false;
local stopFirefightLoop = false;
local enforcer2 = scene:getObjects("enforcer2");

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

local function makeGun(inst, spreadAngle)
    local obj = findObject(inst.objects, "gun1_root");
    local dummy = obj:findDummyComponent("missile");
    inst.weapon = WeaponMachineGunComponent(const.SceneObjectTypeEnemyMissile);
    inst.weapon.haveSound = false;
    inst.weapon.pos = dummy.pos;
    inst.weapon.angle = dummy.angle;
    inst.weapon.velocity = 100.0;
    inst.weapon.spreadAngle = spreadAngle;
    inst.weapon.loopDelay = 0.05;
    obj:addComponent(inst.weapon);
end

local function militarySetupGun(military)
    military.material = const.MaterialFlesh;
    military.myWeapon = WeaponBlasterComponent(true, const.SceneObjectTypeAllyMissile);
    military.myWeapon.pos = vec2(2.0, -0.22);
    military.myWeapon.haveSound = false;
    military.myWeapon.velocity = 30.0;
    military.myWeapon.turns = 1;
    military.myWeapon.shotsPerTurn = 3;
    military.myWeapon.loopDelay = 0.1;
    military:addComponent(military.myWeapon);
end

local function military11Attack1(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path13")[1];
    military11.roamBehavior:reset();
    military11.roamBehavior.linearDamping = 4.0;
    military11.roamBehavior.dampDistance = 2.0;
    military11.roamBehavior.linearVelocity = 10.0;
    military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military11.roamBehavior:start();

    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military11.myWeapon:trigger(true);
        addTimeoutOnce(1.0, function()
            if stopMilitaryLoop then
                return;
            end
            military11.myWeapon:trigger(false);
            addTimeoutOnce(0.5, function()
                if stopMilitaryLoop then
                    return;
                end
                fn(unpack2(args));
            end);
        end);
    end);
end

local function military11Retreat1(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path14")[1];
    military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military11.roamBehavior:reset();
        military11.angle = military11.myAngle;
    end);
    addTimeoutOnce(0.8, function()
        if stopMilitaryLoop then
            return;
        end
        gun1.weapon:trigger(true);
    end);
    addTimeoutOnce(3.0, function()
        if stopMilitaryLoop then
            return;
        end
        gun1.weapon:trigger(false);
        addTimeoutOnce(0.5, function()
            if stopMilitaryLoop then
                return;
            end
            fn(unpack2(args));
        end);
    end);
end

local function military11Retreat2(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path15")[1];
    military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    addTimeoutOnce(0.8, function()
        if stopMilitaryLoop then
            return;
        end
        gun1.weapon:trigger(true);
    end);
    addTimeoutOnce(3.5, function()
        if stopMilitaryLoop then
            return;
        end
        local p = scene:getObjects("path16")[1];
        military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        addTimeoutOnce(1.5, function()
            if stopMilitaryLoop then
                return;
            end
            military11.angle = military11.myAngle;
            addTimeoutOnce(1.0, function()
                if stopMilitaryLoop then
                    return;
                end
                fn(unpack2(args));
            end);
        end);
    end);
    addTimeoutOnce(5.0, function()
        if stopMilitaryLoop then
            return;
        end
        gun1.weapon:trigger(false);
    end);
end

local function military11Loop()
    if stopMilitaryLoop then
        return;
    end
    military11Attack1(function()
        if math.random(1, 3) ~= 1 then
            military11Retreat1(military11Loop);
        else
            military11Retreat2(military11Loop);
        end
    end);
end

local function military12Attack1(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path17")[1];
    military12.roamBehavior:reset();
    military12.roamBehavior.linearDamping = 4.0;
    military12.roamBehavior.dampDistance = 2.0;
    military12.roamBehavior.linearVelocity = 10.0;
    military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military12.roamBehavior:start();

    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military12.myWeapon:trigger(true);
        addTimeoutOnce(1.0, function()
            if stopMilitaryLoop then
                return;
            end
            military12.myWeapon:trigger(false);
            addTimeoutOnce(0.5, function()
                if stopMilitaryLoop then
                    return;
                end
                fn(unpack2(args));
            end);
        end);
    end);
end

local function military12Retreat1(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path18")[1];
    military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    addTimeoutOnce(1.3, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(true);
    end);
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(false);
    end);
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        local p = scene:getObjects("path19")[1];
        military10.roamBehavior:reset();
        military10.roamBehavior.linearDamping = 4.0;
        military10.roamBehavior.dampDistance = 2.0;
        military10.roamBehavior.linearVelocity = 10.0;
        military10.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        military10.roamBehavior:start();
        addTimeoutOnce(2.3, function()
            if stopMilitaryLoop then
                return;
            end
            military10.myWeapon:trigger(true);
            addTimeoutOnce(1.0, function()
                if stopMilitaryLoop then
                    return;
                end
                military10.myWeapon:trigger(false);
                addTimeoutOnce(0.5, function()
                    if stopMilitaryLoop then
                        return;
                    end
                    local p = scene:getObjects("path20")[1];
                    military10.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                    addTimeoutOnce(1.3, function()
                        if stopMilitaryLoop then
                            return;
                        end
                        gun2.weapon:trigger(true);
                    end);
                    addTimeoutOnce(2.5, function()
                        if stopMilitaryLoop then
                            return;
                        end
                        military10.angle = military10.myAngle;
                    end);
                    addTimeoutOnce(1.5, function()
                        if stopMilitaryLoop then
                            return;
                        end
                        local p = scene:getObjects("path21")[1];
                        military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        addTimeoutOnce(1.0, function()
                            if stopMilitaryLoop then
                                return;
                            end
                            gun2.weapon:trigger(false);
                        end);
                        addTimeoutOnce(1.5, function()
                            if stopMilitaryLoop then
                                return;
                            end
                            military12.angle = military12.myAngle;
                            addTimeoutOnce(0.5, function()
                                if stopMilitaryLoop then
                                    return;
                                end
                                fn(unpack2(args));
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end

local function military12Retreat2(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path22")[1];
    military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military12.roamBehavior:reset();
        military12.angle = military12.myAngle;
    end);
    addTimeoutOnce(1.2, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(true);
    end);
    addTimeoutOnce(3.0, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(false);
        addTimeoutOnce(0.5, function()
            if stopMilitaryLoop then
                return;
            end
            fn(unpack2(args));
        end);
    end);
end

local function military12Loop()
    if stopMilitaryLoop then
        return;
    end
    military12Attack1(function()
        if math.random(1, 3) ~= 1 then
            military12Retreat1(military12Loop);
        else
            military12Retreat2(military12Loop);
        end
    end);
end

local function military15Walk1(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path23")[1];
    military15.roamBehavior:reset();
    military15.roamBehavior.linearDamping = 4.0;
    military15.roamBehavior.dampDistance = 2.0;
    military15.roamBehavior.linearVelocity = 10.0;
    military15.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military15.roamBehavior:start();

    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military15.angle = math.rad(200);
        addTimeoutOnce(3.0, function()
            if stopMilitaryLoop then
                return;
            end
            fn(unpack2(args));
        end);
    end);
end

local function military15Walk2(fn, ...)
    if stopMilitaryLoop then
        return;
    end

    local args = pack2(...);

    local p = scene:getObjects("path24")[1];
    military15.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());

    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military15.angle = military15.myAngle;
        addTimeoutOnce(3.0, function()
            if stopMilitaryLoop then
                return;
            end
            fn(unpack2(args));
        end);
    end);
end

local function military15Loop()
    if stopMilitaryLoop then
        return;
    end
    military15Walk1(function()
        military15Walk2(military15Loop);
    end);
end

local function fastTrack(military, name, pos)
    local track = FootageTrackComponent(military, pos, "common1/portrait_jake.png", 5.0,
        string.format(tr.str10, name), 0.8, 230);
    track.aimMinScale = 1.5;
    track.aimScale = 9.0;
    track.aimTime = 0.25;
    track.transitionTime = 0.25;
    track.imageTime = 0.25;
    track.trackTime = 0.25;
    track.keepTime = 1.0;
    track.fadeTime = 1.0;
    cam0:addComponent(track);
    return track;
end

local function keylTrack()
    local track = FootageTrackComponent(keyl0, vec2(-8, 6) * (45 / 35), "common2/portrait_keyl.png", 10.0 * (45 / 35),
        tr.str11, 1.0 * (45 / 35), 230);
    track.aimMinScale = 1.5;
    track.aimScale = 9.0;
    track.aimTime = 1.0;
    track.trackTime = 1.0;
    cam0:addComponent(track);
    return track;
end

function startFootage2()
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
    scene.camera:findCameraComponent():zoomTo(45, const.EaseLinear, 0);

    local camCookie = startCam("target2", 45);

    military10.myAngle = military10.angle;
    militarySetupGun(military10);

    military11.myAngle = military11.angle;
    militarySetupGun(military11);
    military11Loop();

    military12.myAngle = military12.angle;
    militarySetupGun(military12);

    addTimeoutOnce(0.5, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(true);
    end);
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        gun2.weapon:trigger(false);
        addTimeoutOnce(0.5, function()
            if stopMilitaryLoop then
                return;
            end
            military12Loop();
        end);
    end);

    addTimeout0(function(cookie, dt)
        if stopMilitaryLoop then
            cancelTimeout(cookie);
            return;
        end
        military13.angle = (military11.pos - military13.pos):angle();
        military14.angle = (military10.pos - military14.pos):angle();
        keyl0.angle = (military12.pos - keyl0.pos):angle();
    end);

    military15.myAngle = military15.angle;
    addTimeoutOnce(2.0, function()
        if stopMilitaryLoop then
            return;
        end
        military15Loop();
    end);

    militarySetupGun(military13);
    militarySetupGun(military14);
    militarySetupGun(military15);

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog4.str1},
        }, function ()
            local trackTimers = {};
            local tracks = {};
            local to = 0.4;
            table.insert(tracks, fastTrack(military10, tr.str12, vec2(-10, 3)));
            trackTimers[1] = addTimeoutOnce(to, function()
                trackTimers[1] = nil;
                table.insert(tracks, fastTrack(military11, tr.str13, vec2(5, 7)));
            end);
            trackTimers[2] = addTimeoutOnce(to * 2, function()
                trackTimers[2] = nil;
                table.insert(tracks, fastTrack(military12, tr.str14, vec2(0, -4)));
            end);
            trackTimers[3] = addTimeoutOnce(to * 3, function()
                trackTimers[3] = nil;
                table.insert(tracks, fastTrack(military13, tr.str15, vec2(-20, 12)));
            end);
            trackTimers[4] = addTimeoutOnce(to * 4, function()
                trackTimers[4] = nil;
                table.insert(tracks, fastTrack(military14, tr.str16, vec2(-23, -2)));
            end);
            trackTimers[5] = addTimeoutOnce(to * 5, function()
                trackTimers[5] = nil;
                table.insert(tracks, fastTrack(military15, tr.str17, vec2(-32, 5)));
            end);
            showLowerDialog(
            {
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog5.str1},
            }, function ()
                for _, timer in pairs(trackTimers) do
                    cancelTimeout(timer);
                end
                for _, track in pairs(tracks) do
                    if track.parent ~= nil then
                        track:removeFromParent();
                    end
                end
                local timer, track = nil;
                timer = addTimeoutOnce(1.0, function()
                    timer = nil;
                    track = keylTrack();
                end);
                showLowerDialog(
                {
                    {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog6.str1},
                }, function ()
                    local staticBg = nil;
                    if timer ~= nil then
                        cancelTimeout(timer);
                    end
                    if track ~= nil then
                        track:removeFromParent();
                    end
                    addTimeoutOnce(1.0, function()
                        if stopFirefightLoop then
                            return;
                        end
                        local j = scene:getJoints("door8_joint")[1];
                        j.motorSpeed = -j.motorSpeed;
                        scene.camera:findCameraComponent():tremorStart(0.3);
                        addTimeout0(function(cookie, dt)
                            if j:getJointTranslation() >= j.upperLimit then
                                cancelTimeout(cookie);
                                addTimeoutOnce(0.2, function()
                                    scene.camera:findCameraComponent():tremor(false);
                                end);
                            end
                        end);
                    end);
                    addTimeoutOnce(2.0, function()
                        stopMilitaryLoop = true;
                        military10.roamBehavior:reset();
                        military11.roamBehavior:reset();
                        military12.roamBehavior:reset();
                        military15.roamBehavior:reset();

                        military10.myWeapon:trigger(false);
                        military11.myWeapon:trigger(false);
                        military12.myWeapon:trigger(false);

                        gun1.weapon:trigger(false);
                        gun2.weapon:trigger(false);

                        local p = scene:getObjects("path25")[1];
                        military10.roamBehavior:reset();
                        military10.roamBehavior.linearDamping = 4.0;
                        military10.roamBehavior.dampDistance = 2.0;
                        military10.roamBehavior.linearVelocity = 10.0;
                        military10.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        military10.roamBehavior:start();

                        military12.roamBehavior:reset();
                        military12.roamBehavior.linearDamping = 4.0;
                        military12.roamBehavior.dampDistance = 2.0;
                        military12.roamBehavior.linearVelocity = 10.0;
                        military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        military12.roamBehavior:start();

                        local p = scene:getObjects("path26")[1];
                        military11.roamBehavior:reset();
                        military11.roamBehavior.linearDamping = 4.0;
                        military11.roamBehavior.dampDistance = 2.0;
                        military11.roamBehavior.linearVelocity = 10.0;
                        military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        military11.roamBehavior:start();

                        local p = scene:getObjects("path27")[1];
                        military15.roamBehavior:reset();
                        military15.roamBehavior.linearDamping = 4.0;
                        military15.roamBehavior.dampDistance = 2.0;
                        military15.roamBehavior.linearVelocity = 10.0;
                        military15.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                        military15.roamBehavior:start();

                        scene:getObjects("military1_cp")[1].active = true;
                    end);
                    addTimeoutOnce(4.5, function()
                        openAirlock("door7");
                        addTimeoutOnce(1.0, function()
                            for _, obj in pairs(enforcer2) do
                                obj:findWeaponComponent().haveSound = false;
                                obj:findTargetableComponent().patrol = true;
                                obj:findTargetableComponent().target = scene:getObjects("enforcer1_cp")[1];
                            end
                        end);
                        addTimeoutOnce(0.5, function()
                            scene:getObjects("military1_cp")[1].active = false;
                            local p = scene:getObjects("path29")[1];
                            military11.roamBehavior:reset();
                            military11.roamBehavior.linearDamping = 4.0;
                            military11.roamBehavior.dampDistance = 2.0;
                            military11.roamBehavior.linearVelocity = 10.0;
                            military11.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military11.roamBehavior:start();
                        end);
                        addTimeoutOnce(0.8, function()
                            local p = scene:getObjects("path30")[1];
                            military12.roamBehavior:reset();
                            military12.roamBehavior.linearDamping = 4.0;
                            military12.roamBehavior.dampDistance = 2.0;
                            military12.roamBehavior.linearVelocity = 10.0;
                            military12.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military12.roamBehavior:start();
                        end);
                        addTimeoutOnce(1.5, function()
                            local p = scene:getObjects("path28")[1];
                            military10.roamBehavior:reset();
                            military10.roamBehavior.linearDamping = 4.0;
                            military10.roamBehavior.dampDistance = 2.0;
                            military10.roamBehavior.linearVelocity = 10.0;
                            military10.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military10.roamBehavior:start();
                        end);
                        addTimeoutOnce(1.8, function()
                            local p = scene:getObjects("path31")[1];
                            military13.roamBehavior:reset();
                            military13.roamBehavior.linearDamping = 4.0;
                            military13.roamBehavior.dampDistance = 2.0;
                            military13.roamBehavior.linearVelocity = 10.0;
                            military13.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military13.roamBehavior:start();
                        end);
                        addTimeoutOnce(2.0, function()
                            local p = scene:getObjects("path32")[1];
                            military14.roamBehavior:reset();
                            military14.roamBehavior.linearDamping = 4.0;
                            military14.roamBehavior.dampDistance = 2.0;
                            military14.roamBehavior.linearVelocity = 10.0;
                            military14.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military14.roamBehavior:start();
                        end);
                        addTimeoutOnce(2.2, function()
                            local p = scene:getObjects("path33")[1];
                            military15.roamBehavior:reset();
                            military15.roamBehavior.linearDamping = 4.0;
                            military15.roamBehavior.dampDistance = 2.0;
                            military15.roamBehavior.linearVelocity = 10.0;
                            military15.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                            military15.roamBehavior:start();
                        end);
                        addTimeoutOnce(5.5, function()
                            addTimeoutOnce(4.0, function()
                                for _, obj in pairs(enforcer2) do
                                    obj:removeFromParent();
                                end
                                military10:removeFromParent();
                                military11:removeFromParent();
                                military12:removeFromParent();
                                military13:removeFromParent();
                                military14:removeFromParent();
                                military15:removeFromParent();
                            end);
                            if stopFirefightLoop then
                                return;
                            end
                            staticBg = createStatic1Background(225);
                            local rc = staticBg:findRenderBackgroundComponent();
                            rc.color = {1, 1, 1, 0};
                            local t = 0;
                            addTimeout0(function(cookie, dt)
                                if stopFirefightLoop then
                                    cancelTimeout(cookie);
                                    return;
                                end
                                t = t + dt;
                                if t >= 3 then
                                    t = 3;
                                end
                                rc.color = {1, 1, 1, t / 3};
                                if t == 3 then
                                    cancelTimeout(cookie);
                                end
                            end);
                        end);
                    end);
                    showLowerDialog(
                    {
                        {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog7.str1},
                    }, function ()
                        stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                            scene.camera:findCameraComponent():tremor(false);
                            stopFirefightLoop = true;
                            if staticBg ~= nil then
                                staticBg:removeFromParent();
                            end
                            stopCam(camCookie);
                            startFootage3();
                        end);
                    end);
                end);
            end);
        end);
    end);
end

-- main

makeAirlock("door7");
makeGun(gun1, math.rad(10));
makeGun(gun2, math.rad(10));

makeGear("door8", "door8_joint", "door9", "door9_joint", -1);

setSensorEnterListener("military1_cp", false, function(other)
    other.roamBehavior:reset();
    if other == military10 then
        other.angle = math.rad(180);
    elseif other == military11 then
        other.angle = math.rad(200);
    elseif other == military12 then
        other.angle = math.rad(165);
    elseif other == military15 then
        other.angle = math.rad(-20);
    end
end);

local targets = {military10, military11, military12, military13, military14, military15};
local nextTarget = 1;

setSensorEnterListener("enforcer1_cp", false, function(other)
    if other:findTargetableComponent().patrol then
        other:findTargetableComponent().patrol = false;
        other:findTargetableComponent().target = targets[nextTarget];
        nextTarget = nextTarget + 1;
        if nextTarget > #targets then
            nextTarget = 1;
        end
    end
end);

local firefight = {};

setSensorEnterListener("military2_cp", false, function(other)
    if firefight[other.cookie] == nil then
        firefight[other.cookie] = 1;
        local function func()
            if other:scene() == nil then
                return;
            end
            other:findWeaponComponent():trigger(true);
            addTimeoutOnce(0.1 + math.random() * 0.2, function()
                if other:scene() == nil then
                    return;
                end
                other:findWeaponComponent():trigger(false);
                addTimeoutOnce(1.0 + math.random() * 1.0, func);
            end);
        end
        addTimeoutOnce(0.5, func);
    end
end);
