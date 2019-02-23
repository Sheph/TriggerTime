local control4 = scene:getObjects("control4_ph")[1]:findPlaceholderComponent();
local control5 = scene:getObjects("control5_ph")[1]:findPlaceholderComponent();
local uselessPanel = false;
local keyl = scene:getObjects("keyl")[1];
local keylHack1 = false;

-- main

makeDoor("door15", false);
makeDoorTrigger("door15_cp", "door15");

makeDoor("door16", false);
makeDoorTrigger("door16_cp", "door16");

makeDoor("door17", false);
makeDoor("door18", false);
makeDoor("door19", false);

makeDoor("door20", false);
makeDoorTrigger("door20_cp", "door20");

makeDoor("door22", false);
makeDoorTrigger("door22_cp", "door22");

makeDoor("door23", false);
makeDoorTrigger("door23_cp", "door23");

makeDoor("door21", false);

makeLever("lever12", true, nil, function()
    disableLever("lever12");
    local objs = scene:getObjects("laser12");
    for _, obj in pairs(objs) do
        obj:addComponent(FadeOutComponent(1.0));
    end
    audio:playSound("alarm_off.ogg");
end);

makeKeyDoor("blue_door", "blue_key_ph", function()
    openDoor("door21");
end);

local function control45Func(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        if uselessPanel then
            showLowerDialog(
            {
                {"player", tr.dialog46.str1},
            }, function ()
                scene.cutscene = false;
            end);
        else
            showLowerDialog(
            {
                {"player", tr.dialog47.str1},
            }, function ()
                uselessPanel = true;
                scene.cutscene = false;
            end);
        end
    end);
end

control4.listener = createSensorEnterListener(true, control45Func);
control5.listener = createSensorEnterListener(true, control45Func);

setSensorEnterListener("keyl1_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog48.str1},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog48.str2},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog48.str3},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("backpack_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog49.str1},
        {"player", tr.dialog49.str2},
    }, function ()
        scene.cutscene = false;
    end);
end);

setSensorEnterListener("keyl2_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog50.str1},
    }, function ()
        control4.listener = nil;
        control5.listener = nil;
        scene.cutscene = false;
        local p = scene:getObjects("keyl1_path")[1];
        keyl.roamBehavior:reset();
        keyl.roamBehavior.linearVelocity = 13.0;
        keyl.roamBehavior.linearDamping = 4.0;
        keyl.roamBehavior.dampDistance = 3.0;
        keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
        keyl.roamBehavior:start();
        scene:getObjects("keyl4_cp")[1].active = true;
    end);
end);

local function keylHack1Func()
    scene:getObjects("keyl4_cp")[1].active = false;
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog51.str1},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog51.str2},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog51.str3},
    }, function ()
        addTimeoutOnce(1.5, function()
            openDoor("door18", true);
        end);
        scene.cutscene = false;
    end);
end

setSensorEnterListener("keyl3_cp", true, function(other)
    if keylHack1 then
        keylHack1Func();
    end
    keylHack1 = true;
end);

setSensorListener("keyl4_cp", function(other)
    if keylHack1 then
        keylHack1Func();
    end
    keylHack1 = true;
end, function (other)
    keylHack1 = false;
end);

setSensorEnterListener("keyl5_cp", true, function(other)
    scene:getObjects("keyl6_cp")[1].active = true;
end);

setSensorEnterListener("keyl6_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog52.str1},
        {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog52.str2},
    }, function ()
        scene.cutscene = false;
        openDoor("door17",true);
    end);
end);

setSensorEnterListener("exit1_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("exit1_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    setAmbientMusic("ambient8.ogg");
    startAmbientMusic(true);
    closeDoor("door17", true);
    local p = scene:getObjects("keyl2_path")[1];
    keyl.roamBehavior:reset();
    keyl.roamBehavior.linearVelocity = 13.0;
    keyl.roamBehavior.linearDamping = 4.0;
    keyl.roamBehavior.dampDistance = 3.0;
    keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    keyl.roamBehavior:start();
end);

setSensorEnterListener("exit2_cp", true, function(other)
    addTimeoutOnce(0.5, function()
        showUpperDialog(
        {
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog53.str1},
            {"ally", tr.str6, "common2/portrait_keyl.png", tr.dialog53.str2},
        }, function ()
            scene.cutscene = false;
            addTimeoutOnce(0.5, function()
                local p = scene:getObjects("keyl3_path")[1];
                keyl.roamBehavior:reset();
                keyl.roamBehavior.linearVelocity = 13.0;
                keyl.roamBehavior.linearDamping = 4.0;
                keyl.roamBehavior.dampDistance = 3.0;
                keyl.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
                keyl.roamBehavior:start();
            end);
        end);
    end);
end);
