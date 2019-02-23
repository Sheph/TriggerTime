local shuttle = scene:getObjects("shuttle1_mover")[1];
local military0 = scene:getObjects("military0")[1];
local military1 = scene:getObjects("military1")[1];
local military10 = scene:getObjects("military10")[1];
local military11 = scene:getObjects("military11")[1];
local display1 = nil;

local function setupDisplay(name)
    local rc = scene:getObjects("wall1")[1]:findRenderProjComponents(name)[1];
    local display = AnimationComponent(rc.drawable);
    display:addAnimation(const.AnimationDefault, "tv_def", 1);
    display:startAnimation(const.AnimationDefault);
    scene:getObjects("wall1")[1]:addComponent(display);

    return rc;
end

-- main

military0:findDudeComponent().haveGun = false;
military1:findDudeComponent().haveGun = false;
military10:findDudeComponent().haveGun = false;
military11:findDudeComponent().haveGun = false;

if not scene.playable then
    if settings.developer == 0 then
        local scl = 5.0;
        bg[4] = factory:createBackground("fog.png", 544 / scl, 416 / scl, vec2(30.0, 1.0), 110)
        bg[4]:findRenderBackgroundComponent().unbound = true;
        bg[4]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.8};
        scene:addObject(bg[4]);

        stainedGlass({0, 0, 0, 1.0}, {0, 0, 0, 0}, const.EaseInQuad, 3.0);

        scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
        scene.camera:findCameraComponent().target = shuttle;
        local p = scene:getObjects("path1")[1];
        shuttle.roamBehavior:reset();
        shuttle.roamBehavior.linearVelocity = 60.0;
        shuttle.roamBehavior.linearDamping = 4.0;
        shuttle.roamBehavior.dampDistance = 28.0;
        shuttle.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        shuttle.roamBehavior:start();

        local tmp = scene:getObjects("shuttle1")[1];
        local rc = tmp:findRenderQuadComponent();
        tmp:addComponent(QuadPulseComponent(rc, vec2(0, 0), 0.4, const.EaseInOutQuad, 0.98,
            0.4, const.EaseInOutQuad, 1.02));

        setSensorEnterListener("intro1_cp", true, function(other)
            if other.name ~= "shuttle1" then
                return;
            end
            addTimeoutOnce(1.5, function()
                player0:setTransform(scene:getObjects("intro1_cp")[1]:getTransform());
                scene.camera:findCameraComponent():follow(player0, const.EaseOutQuad, 1.0);
                player0.linearDamping = 4.0;
                local p = scene:getObjects("path2")[1];
                player0.roamBehavior:reset();
                player0.roamBehavior.linearVelocity = 14.0;
                player0.roamBehavior.linearDamping = 4.0;
                player0.roamBehavior.dampDistance = 4.0;
                player0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                player0.roamBehavior:start();
                scene.camera:findCameraComponent():zoomTo(35, const.EaseInOutQuad, 3.0);
                bg[4]:addComponent(FadeOutComponent(4.0));
                bg[4] = nil;
            end);
        end);
    end
end

display1 = setupDisplay("display1");
makeDoor("door0", false);
makeDoorTrigger("door0_cp", "door0");
makeDoor("door1", false);
makeDoorTrigger("door1_cp", "door1");
makeDoor("door2", false);

setSensorEnterListener("player1_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        military0.angle = (player0.pos - military0.pos):angle();
        addTimeoutOnce(0.5, function()
            military1.angle = (player0.pos - military1.pos):angle();
        end);
    end);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str48, "common1/portrait_jake.png", tr.dialog137.str1},
            {"ally", tr.str48, "common1/portrait_jake.png", tr.dialog137.str2},
        }, function ()
            player0.linearDamping = 4.0;
            local p = scene:getObjects("path3")[1];
            player0.roamBehavior:reset();
            player0.roamBehavior.linearDamping = 4.0;
            player0.roamBehavior.dampDistance = 4.0;
            player0.roamBehavior.linearVelocity = 10.0;
            player0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            player0.roamBehavior:start();
            addTimeoutOnce(1.0, function()
                military1.angle = math.pi;
                addTimeoutOnce(1.0, function()
                    military0.angle = math.pi;
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("player2_cp", true, function(other)
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
    scene.camera:findCameraComponent():follow(player0, const.EaseOutQuad, 1.5);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog138.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog138.str2},
        }, function ()
            local p = scene:getObjects("path5")[1];
            sarge.roamBehavior:reset();
            sarge.roamBehavior.linearDamping = 4.0;
            sarge.roamBehavior.dampDistance = 3.0;
            sarge.roamBehavior.linearVelocity = 10.0;
            sarge.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            sarge.roamBehavior:start();
            addTimeoutOnce(0.5, function()
                p = scene:getObjects("path4")[1];
                player0.roamBehavior:reset();
                player0.roamBehavior.linearDamping = 4.0;
                player0.roamBehavior.dampDistance = 3.0;
                player0.roamBehavior.linearVelocity = 10.0;
                player0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                player0.roamBehavior:start();
            end);
            addTimeoutOnce(3.0, function()
                scene:getObjects("terrain0")[1]:findLightComponent():getLights("tv_light")[1].visible = true;
                display1.visible = true;
                addTimeoutOnce(1.0, function()
                    stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 1.0}, const.EaseLinear, 1.0, function()
                        startFootage1();
                    end);
                end);
            end);
        end);
    end);
end);
