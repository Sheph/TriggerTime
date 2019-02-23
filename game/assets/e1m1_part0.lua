local military0 = scene:getObjects("military0")[1];
local military1 = scene:getObjects("military1")[1];
local military2 = scene:getObjects("military2")[1];
local military3 = scene:getObjects("military3")[1];
local military4 = scene:getObjects("military4")[1];
local military5 = scene:getObjects("military5")[1];
local military6 = scene:getObjects("military6")[1];
local military7 = scene:getObjects("military7")[1];
local military8 = scene:getObjects("military8")[1];
local military16 = scene:getObjects("military16")[1];
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
military2:findDudeComponent().haveGun = false;
military3:findDudeComponent().haveGun = false;
military4:findDudeComponent().haveGun = false;
military5:findDudeComponent().haveGun = false;
military6:findDudeComponent().haveGun = false;
military7:findDudeComponent().haveGun = false;
military8:findDudeComponent().haveGun = false;
military16:findDudeComponent().haveGun = false;

if not scene.playable then
    if settings.developer == 0 then
        stainedGlass({0, 0, 0, 1.0}, {0, 0, 0, 0}, const.EaseInQuad, 3.0);

        player0.linearDamping = 4.0;
        local p = scene:getObjects("path1")[1];
        player0.roamBehavior:reset();
        player0.roamBehavior.linearDamping = 4.0;
        player0.roamBehavior.dampDistance = 4.0;
        player0.roamBehavior.linearVelocity = 10.0;
        player0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        player0.roamBehavior:start();

        addTimeoutOnce(3.5, function()
            local c = RenderTextComponent(vec2(-12, -8), 0, 100.0, tr.str8, 150);
            c.charSize = 2.0;
            c.color = {0, 0, 0, 0};
            scene.camera:addComponent(c);
            local t = 0;
            addTimeout0(function(cookie, dt)
                t = t + dt;
                if t >= 2 then
                    t = 2;
                end
                c.color = {0, 0.6, 0, t / 2};
                if t == 2 then
                    cancelTimeout(cookie);
                    addTimeoutOnce(3.0, function()
                        addTimeout0(function(cookie, dt)
                            t = t - dt;
                            if t <= 0 then
                                t = 0;
                            end
                            c.color = {0, 0.6, 0, t / 2};
                            if t == 0 then
                                cancelTimeout(cookie);
                                c:removeFromParent();
                            end
                        end);
                    end);
                end
            end);
        end);
    end
end

display1 = setupDisplay("display1");
makeDoor("door0", false);
makeDoor("door1", true);
makeDoor("door2", true);
makeDoor("door3", false);
makeDoor("door4", true);
makeDoor("door5", false);
makeDoorTrigger("door5_cp", "door5");
makeDoor("door6", false);

setSensorEnterListener("player1_cp", true, function(other)
    local p = scene:getObjects("path4")[1];
    military6.roamBehavior:reset();
    military6.roamBehavior.linearDamping = 4.0;
    military6.roamBehavior.dampDistance = 3.0;
    military6.roamBehavior.linearVelocity = 8.0;
    military6.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military6.roamBehavior:start();

    p = scene:getObjects("path3")[1];
    military7.roamBehavior:reset();
    military7.roamBehavior.linearDamping = 4.0;
    military7.roamBehavior.dampDistance = 3.0;
    military7.roamBehavior.linearVelocity = 8.0;
    military7.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military7.roamBehavior:start();

    p = scene:getObjects("path2")[1];
    military8.roamBehavior:reset();
    military8.roamBehavior.linearDamping = 4.0;
    military8.roamBehavior.dampDistance = 3.0;
    military8.roamBehavior.linearVelocity = 10.0;
    military8.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military8.roamBehavior:start();
end);

setSensorEnterListener("player2_cp", true, function(other)
    military4.angle = (military5.pos - military4.pos):angle();
    addTimeoutOnce(1.0, function()
        military5.angle = (military4.pos - military5.pos):angle();
    end);

    local p = scene:getObjects("path5")[1];
    military3.roamBehavior:reset();
    military3.roamBehavior.linearDamping = 4.0;
    military3.roamBehavior.dampDistance = 3.0;
    military3.roamBehavior.linearVelocity = 10.0;
    military3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military3.roamBehavior:start();
end);

setSensorEnterListener("player3_cp", true, function(other)
    local p = scene:getObjects("path6")[1];
    military3.roamBehavior:reset();
    military3.roamBehavior.linearDamping = 4.0;
    military3.roamBehavior.dampDistance = 3.0;
    military3.roamBehavior.linearVelocity = 10.0;
    military3.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    military3.roamBehavior:start();
end);

setSensorEnterListener("player4_cp", true, function(other)
    addTimeoutOnce(1.0, function()
        military0.angle = (player0.pos - military0.pos):angle();
        addTimeoutOnce(0.5, function()
            military1.angle = (player0.pos - military1.pos):angle();
        end);
    end);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog1.str1},
            {"ally", tr.str1, "common1/portrait_jake.png", tr.dialog1.str2},
        }, function ()
            player0.linearDamping = 4.0;
            local p = scene:getObjects("path7")[1];
            player0.roamBehavior:reset();
            player0.roamBehavior.linearDamping = 4.0;
            player0.roamBehavior.dampDistance = 4.0;
            player0.roamBehavior.linearVelocity = 10.0;
            player0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            player0.roamBehavior:start();
            addTimeoutOnce(1.0, function()
                military1.angle = 0;
                addTimeoutOnce(1.0, function()
                    military0.angle = 0;
                end);
            end);
        end);
    end);
end);

setSensorEnterListener("player5_cp", true, function(other)
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1_1")[1].pos, scene:getObjects("constraint1_2")[1].pos);
    scene.camera:findCameraComponent():follow(player0, const.EaseOutQuad, 1.5);
    addTimeoutOnce(2.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog2.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog2.str2},
        }, function ()
            local p = scene:getObjects("path8")[1];
            sarge.roamBehavior:reset();
            sarge.roamBehavior.linearDamping = 4.0;
            sarge.roamBehavior.dampDistance = 3.0;
            sarge.roamBehavior.linearVelocity = 10.0;
            sarge.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
            sarge.roamBehavior:start();
            addTimeoutOnce(0.5, function()
                p = scene:getObjects("path9")[1];
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
