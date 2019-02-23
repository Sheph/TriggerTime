local function makeTentacleSwing1(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.7;
        local f = 40000;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

local function makeTentacleSwing2(name)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        local t = 0.8;
        local f = 6000;
        obj:addComponent(TentacleSwingComponent(t,
            const.EaseInOutQuad, f,
            t,
            const.EaseInOutQuad, -f));
    end
end

function startFootage2()
    scene.lighting.ambientLight = {0.6, 0.6, 0.6, 1.0};

    scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
    local camCookie = startCam("target2", 50);

    local p = scene:getObjects("path10")[1];
    cam0.roamBehavior:reset();
    cam0.roamBehavior.changeAngle = false;
    cam0.roamBehavior.linearVelocity = 7.0;
    cam0.roamBehavior.loop = true;
    cam0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    cam0.roamBehavior:start();

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog141.str1},
        }, function ()
            local timer, track = nil;

            timer = addTimeoutOnce(0.5, function()
                timer = nil;
                track = FootageTrackComponent(scene:getObjects("anomaly")[1], vec2(-20, 4) * (50 / 35), "subway1/anomaly.png", 10.0 * (50 / 35),
                    tr.str49, 1.0 * (50 / 35), 230);
                track.aimScale = 9.0;
                track.aimTime = 1.0;
                track.trackTime = 1.0;
                cam0:addComponent(track);
            end);

            showLowerDialog(
            {
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog142.str1},
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog142.str2},
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog142.str3},
                {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog142.str4},
            }, function ()
                if timer ~= nil then
                    cancelTimeout(timer);
                    timer = nil;
                end
                stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                    cam0.roamBehavior:reset();
                    if track ~= nil then
                        track:removeFromParent();
                    end
                    stopCam(camCookie);
                    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
                    scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);
                    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
                    scene.camera:findCameraComponent().target = player0;
                    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
                        sarge.angle = (player0.pos - sarge.pos):angle();
                        addTimeoutOnce(0.5, function()
                            player0.angle = (sarge.pos - player0.pos):angle();
                        end);
                        showLowerDialog(
                        {
                            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog143.str1},
                            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog143.str2},
                            {"player", tr.dialog143.str3},
                        }, function ()
                            stopMusic(true);
                            stainedGlass({0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.8}, const.EaseLinear, 2.0, function()
                                local function black()
                                    stainedGlass({0, 0, 0, 0.8}, {0.0, 0.0, 0.0, 0.8}, const.EaseLinear, 100.0, black);
                                end
                                black();
                                local ac = scene:getObjects("wall1")[1]:findAnimationComponent();
                                if ac ~= nil then
                                    ac:removeFromParent();
                                end
                                addTimeoutOnce(1.5, function()
                                    scene.paused = true;
                                    startMusic("theme.ogg", false);
                                    rocket.contexts["menu"]:LoadDocument("ui/credits.rml"):Show();
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end

-- main

makeTentacleSwing1("tentacle1");
makeTentacleSwing2("tentacle2");
