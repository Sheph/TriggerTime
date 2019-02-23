local walker1 = scene:getObjects("walker1")[1];
local jars = {};

-- main

makeAirlock("door26", true);

for i = 32, 36, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    makeJar1WithCreature(jars[i], "cr"..i);
end

for i = 37, 43, 1 do
    jars[i] = scene:getInstances("jar"..i)[1];
    makeJar3WithCreature(jars[i], "cr"..i);
end

setSensorEnterListener("walker4_cp", false, function(other, self)
    scene.respawnPoint = scene:getObjects("walker4_cp")[1]:getTransform();
    if self.first then
        self.first = false;
        fixedSpawn("walker4");
    end
end, { first = true });

setSensorEnterListener("walker5_cp", true, function(other)
    fixedSpawn("walker5");
end);

setSensorEnterListener("walker1_cp", false, function(other)
    if ((other.parent ~= walker1) or (other.name ~= "heart")) then
        return;
    end
    walker1:findWalkerComponent():expose();
    scene:getObjects("walker1_cp")[1].active = false;
end);

setSensorEnterListener("walker2_cp", false, function(other)
    if ((other.parent ~= walker1) or (other.name ~= "heart")) then
        return;
    end
    scene:getObjects("walker1_cp")[1].active = true;
end);

setSensorEnterListener("walker3_cp", true, function(other)
    scene.respawnPoint = scene:getObjects("walker3_cp")[1]:getTransform();
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    closeAirlock("door26", true);
    addTimeoutOnce(1.5, function()
        audio:playSound("lights_on.ogg");
        scene:getObjects("terrain0")[1]:findLightComponent():getLights("walker_light1")[1].visible = true;
        addTimeoutOnce(1.5, function()
            scene.camera:findCameraComponent():follow(walker1, const.EaseInOutQuad, 1.0);
            addTimeoutOnce(1.5, function()
                walker1:findWalkerComponent():angry();
                addTimeoutOnce(2.5, function()
                    walker1:findTargetableComponent().autoTarget = true;
                    addTimeoutOnce(1.0, function()
                        scene.camera:findCameraComponent():follow(scene.player, const.EaseInOutQuad, 1.0);
                        addTimeoutOnce(1.0, function()
                            scene.cutscene = false;
                        end);
                    end);
                end);
            end);
        end);
    end);
    addTimeout(0.25, function(cookie)
        if not walker1:findWalkerComponent().deathFinished then
            return;
        end
        cancelTimeout(cookie);
        addTimeoutOnce(1.5, function()
            scene:getObjects("walker_room_blocker")[1].active = false;
            openAirlock("door26", true);
            bossDone(2);
        end);
    end);
end);
