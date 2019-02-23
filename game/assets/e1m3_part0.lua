-- main

makeAirlock("door1", false);
makeAirlockTrigger("door1_cp", "door1");
makeAirlock("door10", false);

if settings.developer == 0 then
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog22.str1},
        {"player", tr.dialog22.str2},
    }, function ()
        scene.cutscene = false;
    end);
end

setSensorEnterListener("mine_entry_cp", false, function(other)
    if other:findPlayerComponent().flashlight then
        return;
    end
    scene.player.linearVelocity = scene:getObjects("mine_entry_cp")[1]:getDirection(5.0);
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog23.str1},
    }, function ()
        scene.cutscene = false;
    end);
end);
