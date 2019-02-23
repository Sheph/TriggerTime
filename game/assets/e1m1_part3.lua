function startFootage3()
    scene.lighting.ambientLight = {0.4, 0.4, 0.4, 1.0};
    scene.camera:findCameraComponent():zoomTo(45, const.EaseLinear, 0);
    local bg = setupBgAir();

    local camCookie = startCam("target3", 45);

    local p = scene:getObjects("path34")[1];
    cam0.roamBehavior:reset();
    cam0.roamBehavior.changeAngle = false;
    cam0.roamBehavior.linearVelocity = 10.0;
    cam0.roamBehavior.loop = true;
    cam0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    cam0.roamBehavior:start();

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog8.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog8.str2},
        }, function ()
            stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                cam0.roamBehavior:reset();
                for _, obj in pairs(bg) do
                    obj:removeFromParent();
                end
                stopCam(camCookie);
                startFootage4();
            end);
        end);
    end);
end

-- main
