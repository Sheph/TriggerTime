local gorger0 = scene:getObjects("gorger0")[1];
local gorger1 = scene:getObjects("gorger1")[1];

local function makeJar1(jar, floats)
    jar.myRoot = findObject(jar.objects, "jar_root");
    jar.myPart = findObject(jar.objects, "jar_part");

    local rc = jar.myPart:findRenderTerrainComponents()[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "toxic2", 1);
    ac:startAnimation(const.AnimationDefault);
    jar.myPart:addComponent(ac);

    if floats ~= nil then
        for _, f in pairs(floats) do
            local name = f[1];
            local obj = scene;
            local i = string.find(f[1], ".", 1, true);
            if i ~= nil then
                obj = scene:getObjects(string.sub(f[1], 1, i - 1))[1];
                name = string.sub(f[1], i + 1);
            end
            obj = obj:getObjects(name)[1];
            local c = FloatComponent(1.0, 0.5, f[2]);
            c.target = obj;
            jar.myPart:addComponent(c);
        end
    end
end

local function makeJar1WithCreature(jar, name)
    local creature = scene:getObjects(name)[1];
    creature:findPhysicsBodyComponent():disableAllFixtures();
    local objs = creature:getObjects();
    for _, obj in pairs(objs) do
        obj:findPhysicsBodyComponent():disableAllFixtures();
    end
    local i = creature:findCreatureComponent().I;
    if i == 1 then
        makeJar1(jar, {{name, 20.0}, {name..".tail", 20.0}});
    elseif i == 2 then
        makeJar1(jar, {{name, 30.0}});
    elseif i == 3 then
        makeJar1(jar, {{name, 50.0}, {name..".leg1", 9.0}, {name..".leg2", 9.0}, {name..".leg3", 9.0}, {name..".leg4", 9.0}});
    elseif i == 4 then
        makeJar1(jar, {{name, 100.0}});
    elseif i == 5 then
        makeJar1(jar, {{name, 30.0}, {name..".tail1", 15.0}, {name..".tail2", 3.0}});
    elseif i == 6 then
        makeJar1(jar, {{name, 30.0}, {name..".leg1", 3.0}, {name..".leg2", 3.0}, {name..".leg3", 3.0}, {name..".leg4", 3.0}, {name..".leg5", 3.0}, {name..".leg6", 3.0}});
    else
        assert(false);
    end
end

local function makeJar1Empty(jar)
    makeJar1(jar);
    jar.myRoot.freezable = false;
end

function startFootage1()
    local camCookie = startCam("target1", 35);

    local p = scene:getObjects("path10")[1];
    cam0.roamBehavior:reset();
    cam0.roamBehavior.changeAngle = false;
    cam0.roamBehavior.linearVelocity = 7.0;
    cam0.roamBehavior.loop = true;
    cam0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    cam0.roamBehavior:start();

    gorger0:findRenderHealthbarComponent():removeFromParent();
    gorger0:findGorgerComponent():setWalk();

    p = scene:getObjects("path11")[1];
    gorger0.roamBehavior:reset();
    gorger0.roamBehavior.linearVelocity = 6.0;
    gorger0.roamBehavior.loop = true;
    gorger0.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    gorger0.roamBehavior:start();

    gorger1:findRenderHealthbarComponent():removeFromParent();
    gorger1:findGorgerComponent():setWalk();

    p = scene:getObjects("path12")[1];
    gorger1.roamBehavior:reset();
    gorger1.roamBehavior.linearVelocity = 6.0;
    gorger1.roamBehavior.loop = true;
    gorger1.roamBehavior:changePath(p:findPathComponent().path, p:getTransform());
    gorger1.roamBehavior:start();

    local timer, track = nil;

    timer = addTimeoutOnce(2.5, function()
        timer = nil;
        cam0.roamBehavior:reset();
        track = FootageTrackComponent(scene:getObjects("natan0")[1], vec2(-20, 4), "factory4/natan1.png", 10.0,
            tr.str9, 1.0, 230);
        track.aimScale = 9.0;
        track.aimTime = 1.0;
        track.trackTime = 1.0;
        cam0:addComponent(track);
    end);

    stainedGlass({1.0, 1.0, 1.0, 1.0}, {0, 0, 0, 0}, const.EaseLinear, 1.0, function()
        showLowerDialog(
        {
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog3.str1},
            {"ally", tr.str2, "common2/portrait_sarge.png", tr.dialog3.str2},
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
                gorger0:removeFromParent();
                gorger1:removeFromParent();
                scene:getObjects("natan0")[1]:removeFromParent();
                local objs = scene:getObjects("enforcer0");
                for _, obj in pairs(objs) do
                    obj:removeFromParent();
                end
                startFootage2();
            end);
        end);
    end);
end

-- main

for i = 6, 21, 1 do
    local jar = scene:getInstances("jar"..i)[1];
    if (i == 8) or (i == 16) or (i == 21) then
        makeJar1Empty(jar);
    else
        makeJar1WithCreature(jar, "cr"..i);
    end
end
