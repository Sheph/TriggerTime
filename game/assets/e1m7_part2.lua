local platform3 = scene:getObjects("platform3")[1];
local platform4 = scene:getObjects("platform4")[1];
local platform5 = scene:getObjects("platform5")[1];
local sidePowergens = {
    scene:getObjects("powergen4")[1],
    scene:getObjects("powergen5")[1],
    scene:getObjects("powergen6")[1]
};
local sidePowergenLights = {
    platform4:findLightComponent():getLights("powergen4_light")[1],
    platform4:findLightComponent():getLights("powergen5_light")[1],
    platform4:findLightComponent():getLights("powergen6_light")[1]
};
local mainPowergen = scene:getObjects("powergen7")[1];
local mainPowergenLight = scene:getObjects("terrain0")[1]:findLightComponent():getLights("powergen7_light")[1];
local park3 = scene:getObjects("park3_ph")[1]:findPlaceholderComponent();

function stopPlatforms1()
    platform3.angularVelocity = 0;
    platform4.angularVelocity = 0;
    platform5.angularVelocity = 0;
end

-- main

makePlatform(platform3, sidePowergens, true);

platform3.myAngularVelocity = platform3.angularVelocity;
platform3.angularVelocity = 0;
platform4.myAngularVelocity = platform4.angularVelocity;
platform4.angularVelocity = 0;
platform5.myAngularVelocity = platform5.angularVelocity;
platform5.angularVelocity = 0;

makeAirlock("door10", true);
makeAirlock("door11", false);

setSensorEnterListener("boat5_cp", true, function(other)
    platform3.angularVelocity = platform3.myAngularVelocity;
    platform4.angularVelocity = platform4.myAngularVelocity;
    platform5.angularVelocity = platform5.myAngularVelocity;
end);

setSensorEnterListener("boat6_cp", true, function(other)
    closeAirlock("door10", true);
end);

makeBoatPark(park3, "park3_exit");

addTimeout(0.25, function(cookie)
    local done = true;
    for i = 1, 3, 1 do
        if sidePowergens[i] ~= nil then
            if sidePowergens[i]:dead() then
                sidePowergenLights[i].visible = false;
                sidePowergens[i] = nil;
            else
                done = false;
            end
        end
    end
    if done then
        cancelTimeout(cookie);
        platform4.angularVelocity = 0;
    end
end);

addTimeout(0.25, function(cookie)
    if mainPowergen:dead() then
        mainPowergenLight.visible = false;
        cancelTimeout(cookie);
        openAirlock("door11", true);
    end
end);
