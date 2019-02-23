local display1 = setupDisplay("display1");
local terrain0 = scene:getObjects("terrain0")[1];
local cord0 = {scene:getObjects("cord0_front")[1], scene:getObjects("cord0_back")[1]};
local cord1 = {scene:getObjects("cord1_front")[1], scene:getObjects("cord1_back")[1]};
local cord2 = {scene:getObjects("cord2_front")[1], scene:getObjects("cord2_back")[1]};
local cord3 = {scene:getObjects("cord3_front")[1], scene:getObjects("cord3_back")[1]};
local p1Light1 = terrain0:findLightComponent():getLights("p1_light1")[1];
local p1Light2 = terrain0:findLightComponent():getLights("p1_light2")[1];
local pipe1 = scene:getObjects("pipe1")[1];
local genRc = pipe1:findRenderQuadComponents("gen")[1];
local gen1Light0 = pipe1:findLightComponent():getLights("light0")[1];
local gen2Light0 = scene:getObjects("pipe2")[1]:findLightComponent():getLights("light0")[1];

-- main

makeAirlock("door0", false);
makeAirlock("door1", false);
makeAirlock("door2", false);
makeAirlock("door3", false);
makeAirlockTrigger("door3_cp", "door3");
makeAirlock("door7", true);
makeAirlock("door8", false);
makeAirlock("door9", false);
makeAirlockTrigger("door9_cp", "door9");

if settings.developer == 0 then
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("cam_target1")[1], const.EaseOutQuad, 1.0);
        addTimeoutOnce(2.0, function()
            display1:startAnimation(const.AnimationDefault + 1);
            addTimeoutOnce(1.5, function()
                showLowerDialog(
                {
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog119.str1},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog119.str2},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog119.str3},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog119.str4},
                    {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog119.str5},
                }, function ()
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                    scene.cutscene = false;
                    addTimeoutOnce(1.5, function()
                        fixedSpawn("intro1");
                    end);
                    addTimeoutOnce(3.0, function()
                        display1:startAnimation(const.AnimationDefault);
                    end);
                    addTimeoutOnce(10.0, function()
                        fixedSpawn("intro2");
                    end);
                end);
            end);
        end);
    end);
end

setSensorEnterListener("intro3_cp", true, function(other)
    fixedSpawn("intro3");
    addTimeoutOnce(3.0, function()
        fixedSpawn("intro4");
    end);
end);

setSensorEnterListener("intro5_cp", true, function(other)
    fixedSpawn("intro5");
    addTimeoutOnce(7.0, function()
        fixedSpawn("intro6");
    end);
end);

-- wires puzzle start

local puzzleData = {
    {{ p = 1 },        {},               { d = 1 },              { d = 1 },        {},               { d = 1, r = 1 }, { l = 1 },        { p = 1 }},
    {{},               {},               { d = 1, u = 1 },       { u = 1 },        {},               { u = 1 },        {},               {}},
    {{ d = 1 },        { r = 1 },        { u = 1, l = 1, r = 1}, { l = 1 },        { d = 1 },        { r = 1 },        { l = 1, d = 1 }, { d = 1 }},
    {{ u = 1, r = 1 }, { l = 1 },        { d = 1 },              { d = 1 },        { d = 1, u = 1 }, { d = 1 },        { d = 1, u = 1 }, { u = 1 }},
    {{},               { r = 1 },        { l = 1, u = 1 },       { d = 1, u = 1 }, { u = 1 },        { d = 1, u = 1 }, { d = 1, u = 1 }, {}},
    {{ r = 1 },        { l = 1, r = 1 }, { l = 1 },              { d = 1, u = 1 }, {},               { d = 1, u = 1 }, { u = 1 },        {}},
    {{},               { r = 1 },        { l = 1, r = 1 },       { l = 1, u = 1 }, { d = 1 },        { d = 1, u = 1 }, { r = 1 },        { l = 1 }},
    {{ s = 1 },        {},               {},                     { r = 1 },        { l = 1, u = 1 }, { u = 1 },        {},               { s = 1 }}
};

local puzzleRc = {};

local puzzleVisited = {};

local puzzlePowerVisited = 0;

local function puzzleDone()
    scene.respawnPoint = scene:getObjects("intro3_cp")[1]:getTransform();
    local lightsToGoOut = {};
    lightsToGoOut[p1Light1] = 1;
    lightsToGoOut[p1Light2] = 1;
    lightsToGoOut[gen1Light0] = 1;
    local lights = terrain0:findLightComponent():getLights("go_out_light1");
    for _, light in pairs(lights) do
        lightsToGoOut[light] = 1;
    end
    local lightsToGoIn = {};
    lightsToGoIn[gen2Light0] = gen2Light0.color;
    gen2Light0.color = {0.0, 0.0, 0.0, gen2Light0.color[4]};
    gen2Light0.visible = true;
    local lights = terrain0:findLightComponent():getLights("go_in_light1");
    for _, light in pairs(lights) do
        lightsToGoIn[light] = light.color;
        light.color = {0.0, 0.0, 0.0, light.color[4]};
        light.visible = true;
    end
    for i = 1, 8, 1 do
        for j = 1, 8, 1 do
            local item = puzzleData[i][j];
            if item.socket ~= nil then
                disableSocket(item.socket);
            end
            if item.light ~= nil then
                lightsToGoOut[item.light] = 1;
            end
        end
    end
    addTimeoutOnce(1.0, function()
        audio:playSound("gen_overload.ogg");
    end);
    addTimeoutOnce(2.5, function()
        local tweening = SingleTweening(3.0, const.EaseLinear, 0.0, 1.0, false);
        local genColor = genRc.color;
        addTimeout0(function(cookie, dt, self)
            local v = tweening:getValue(self.t);
            local c = {
                (1 - v) * genColor[1] + v * 1.0,
                (1 - v) * genColor[2] + v * 0.0,
                (1 - v) * genColor[3] + v * 0.0,
                1.0
            };
            genRc.color = c;
            self.t = self.t + dt;
            if tweening:finished(self.t) then
                local laser0 = scene:getObjects("laser0");
                for _, obj in pairs(laser0) do
                    obj:removeFromParent();
                end
                cancelTimeout(cookie);
                local booms = scene:getObjects("pipe1")[1]:findDummyComponents("boom");
                local to = 0.2;
                for _, boom in pairs(booms) do
                    addTimeoutOnce(to, function()
                        local exp = factory:createExplosion1(const.zOrderExplosion);
                        exp.pos = pipe1:getWorldPoint(boom.pos);
                        scene:addObject(exp);
                    end);
                    to = to + 0.3;
                end
                for light, _ in pairs(lightsToGoOut) do
                    lightsToGoOut[light] = light.color;
                end
                local tweening = SingleTweening(3.0, const.EaseLinear, 0.0, 1.0, false);
                addTimeout0(function(cookie, dt, self)
                    local v = tweening:getValue(self.t);
                    local c = {
                        (1 - v) * 1.0 + v * genColor[1],
                        (1 - v) * 0.0 + v * genColor[1],
                        (1 - v) * 0.0 + v * genColor[1],
                        1.0
                    };
                    genRc.color = c;
                    c = {
                        (1 - v) * 0.4 + v * 0.0,
                        (1 - v) * 0.4 + v * 0.0,
                        (1 - v) * 0.4 + v * 0.0,
                        1.0
                    };
                    scene.lighting.ambientLight = c;
                    for light, color in pairs(lightsToGoOut) do
                        c = {
                            (1 - v) * color[1] + v * 0.0,
                            (1 - v) * color[2] + v * 0.0,
                            (1 - v) * color[3] + v * 0.0,
                            color[4],
                        };
                        light.color = c;
                    end
                    self.t = self.t + dt;
                    if tweening:finished(self.t) then
                        cancelTimeout(cookie);
                        addTimeoutOnce(4.0, function()
                            audio:playSound("gen_startup.ogg");
                            local tweening = SingleTweening(1.4, const.EaseLinear, 0.0, 1.0, false);
                            addTimeout0(function(cookie, dt, self)
                                local v = tweening:getValue(self.t);
                                for light, color in pairs(lightsToGoIn) do
                                    local c = {
                                        (1 - v) * 0.0 + v * color[1],
                                        (1 - v) * 0.0 + v * color[2],
                                        (1 - v) * 0.0 + v * color[3],
                                        color[4],
                                    };
                                    light.color = c;
                                end
                                self.t = self.t + dt;
                                if tweening:finished(self.t) then
                                    cancelTimeout(cookie);
                                    addTimeoutOnce(2.0, function()
                                        fixedSpawn("intro7");
                                    end);
                                end
                            end, { t = 0 });
                        end);
                    end
                end, { t = 0 });
            end
        end, { t = 0 });
    end);
end

if settings.developer == 1 then
    addTimeoutOnce(1.0, puzzleDone);
end

local function puzzleUpdate()
    local function setLight(i, j)
        if puzzleData[i][j].light ~= nil then
            return;
        end
        local light = PointLight("");
        light.color = {0.0, 0.0, 1.0, 0.4};
        light.diffuse = false;
        light.xray = true;
        light.distance = 7.0;
        light.intensity = 2.0;
        light.pos = puzzleRc[i][j].pos;
        scene.lighting:addLight(light);
        puzzleData[i][j].light = light;
    end

    local function puzzleFunc(i, j)
        if puzzleVisited[i][j] then
            return;
        end
        puzzleVisited[i][j] = true;
        setLight(i, j);
        local item = puzzleData[i][j];
        if item.p ~= nil then
            puzzlePowerVisited = puzzlePowerVisited + 1;
        end
        if item.u ~= nil then
            puzzleFunc(i - 1, j);
        end
        if item.d ~= nil then
            puzzleFunc(i + 1, j);
        end
        if item.l ~= nil then
            puzzleFunc(i, j - 1);
        end
        if item.r ~= nil then
            puzzleFunc(i, j + 1);
        end
        if (item.socket == nil) or (item.socket.myPlug == nil) then
            puzzleVisited[i][j] = false;
            return;
        end
        local otherS = item.socket.myPlug.myOther.mySocket;
        if otherS == nil then
            puzzleVisited[i][j] = false;
            return;
        end
        puzzleFunc(otherS.myDataI, otherS.myDataJ, true);
        puzzleVisited[i][j] = false;
    end

    local isDone = false;

    for i = 1, 8, 1 do
        for j = 1, 8, 1 do
            local item = puzzleData[i][j];
            if item.light ~= nil then
                item.light:remove();
                item.light = nil;
            end
        end
    end
    for i = 1, 8, 1 do
        for j = 1, 8, 1 do
            local item = puzzleData[i][j];
            if item.p ~= nil then
                puzzlePowerVisited = 0;
                puzzleFunc(i, j);
                if puzzlePowerVisited == 2 then
                    isDone = true;
                end
            end
        end
    end
    if puzzleData[8][1].light ~= nil then
        p1Light1.visible = true;
    else
        p1Light1.visible = false;
    end
    if puzzleData[8][8].light ~= nil then
        p1Light2.visible = true;
        openAirlock("door8", true);
    else
        p1Light2.visible = false;
    end
    if isDone then
        puzzleDone();
    end
end

for i = 1, 8, 1 do
    puzzleRc[i] = {};
    puzzleVisited[i] = {};
    for j = 1, 8, 1 do
        puzzleRc[i][j] = terrain0:findRenderQuadComponents("p1_"..i.."_"..j)[1];
        puzzleVisited[i][j] = false;
        local rc = puzzleRc[i][j];
        local item = puzzleData[i][j];
        local num = 0;
        if (item.u ~= nil) or (item.p ~= nil) then
            local c = factory:createQuad("subway1/wire1.png", rc.height, rc.zOrder - 1);
            c.pos = rc.pos;
            c.angle = rc.angle;
            terrain0:addComponent(c);
            num = num + 1;
        end
        if (item.d ~= nil) or ((item.s ~= nil)) then
            local c = factory:createQuad("subway1/wire1.png", rc.height, rc.zOrder - 1);
            c.pos = rc.pos;
            c.angle = rc.angle + math.pi;
            terrain0:addComponent(c);
            num = num + 1;
        end
        if item.l ~= nil then
            local c = factory:createQuad("subway1/wire1.png", rc.height, rc.zOrder - 1);
            c.pos = rc.pos;
            c.angle = rc.angle + math.pi / 2;
            terrain0:addComponent(c);
            num = num + 1;
        end
        if item.r ~= nil then
            local c = factory:createQuad("subway1/wire1.png", rc.height, rc.zOrder - 1);
            c.pos = rc.pos;
            c.angle = rc.angle - math.pi / 2;
            terrain0:addComponent(c);
            num = num + 1;
        end
        if num == 1 then
            -- needs socket
            item.socket = findObject(scene:instanciate("e1m9_socket1.json", rc.pos, rc.angle - math.pi / 2).objects, "socket1");
            item.socket.myDataI = i;
            item.socket.myDataJ = j;
            makeSocket(item.socket);
        end
    end
end

makePlugs(cord0, function(plug, socket)
    puzzleUpdate();
end, function(plug, socket)
    puzzleUpdate();
end);

makePlugs(cord1, function(plug, socket)
    puzzleUpdate();
end, function(plug, socket)
    puzzleUpdate();
end);

makePlugs(cord2, function(plug, socket)
    puzzleUpdate();
end, function(plug, socket)
    puzzleUpdate();
end);

makePlugs(cord3, function(plug, socket)
    puzzleUpdate();
end, function(plug, socket)
    puzzleUpdate();
end);

-- wires puzzle end
