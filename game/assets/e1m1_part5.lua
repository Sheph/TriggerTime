function startFootage5()
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

    local camCookie = startCam("target1", 35);

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
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog10.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog10.str2},
        }, function ()
            stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                cam0.roamBehavior:reset();
                stopCam(camCookie);
                scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
                scene.camera:findCameraComponent().target = player0;
                stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
                    sarge.angle = (player0.pos - sarge.pos):angle();
                    addTimeoutOnce(0.5, function()
                        player0.angle = (sarge.pos - player0.pos):angle();
                    end);
                    showLowerDialog(
                    {
                        {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog11.str1},
                        {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog11.str2},
                    }, function ()
                        stopMusic(true);
                        stainedGlass({0, 0, 0, 0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 2.0, function()
                            stainedGlass({0, 0, 0, 1.0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 100.0);
                            addTimeoutOnce(2.0, function()
                                scene:setNextLevel("e1m2.lua", "e1m2.json");
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end

-- main
