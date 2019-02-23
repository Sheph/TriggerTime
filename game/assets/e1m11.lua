local music = nil;
local doors = {};
player0 = nil;
sarge = scene:getObjects("sarge0")[1];
cam0 = scene:getObjects("cam0")[1];
bg = nil;

function startMusic(name, crossfade)
    local newMusic = audio:createStream(name);
    newMusic.loop = true;
    newMusic.isMusic = true;
    if crossfade then
        audio:crossfade(music, newMusic, 0.5, 0.5, 0.3);
    else
        if music ~= nil then
            music:stop();
        end
        newMusic:play();
    end
    music = newMusic;
end

function stopMusic(crossfade)
    if music ~= nil then
        if crossfade then
            audio:crossfade(music, nil, 0.5, 0.5, 0.3);
        else
            music:stop();
        end
    end
    music = nil;
end

function openDoor(name)
    if doors[name] ~= nil then
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_left_joint");
    if j:getJointTranslation() >= j.upperLimit then
        j.motorSpeed = math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    j.motorSpeed = math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            cancelTimeout(cookie);
            doors[name] = nil;
        end
    end);
end

function closeDoor(name, haveSound)
    if doors[name] ~= nil then
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door1_left_joint");
    if j:getJointTranslation() <= j.lowerLimit then
        j.motorSpeed = -math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    j.motorSpeed = -math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() <= j.lowerLimit) then
            cancelTimeout(cookie);
            doors[name] = nil;
        end
    end);
end

function makeDoor(name, opened)
    local inst = scene:getInstances(name)[1];
    scene:addGearJoint(findObject(inst.objects, "door1_left"),
        findObject(inst.objects, "door1_right"),
        findJoint(inst.joints, "door1_left_joint"),
        findJoint(inst.joints, "door1_right_joint"),
        -1, false);
    if opened then
        openDoor(name, false);
    end
end

function makeDoorTrigger(sensorName, name)
    setSensorListener(sensorName, function(other, self)
        if self.num == 0 then
            openDoor(name, true);
        end
        self.num = self.num + 1;
    end, function (other, self)
        self.num = self.num - 1;
        if self.num == 0 then
            closeDoor(name, true);
        end
    end, { num = 0 });
end

local function setupCam()
    local rc = cam0:findRenderQuadComponents("rec")[1];
    local anim = AnimationComponent(rc.drawable);
    anim:addAnimation(const.AnimationDefault, "cam_rec", 1);
    anim:startAnimation(const.AnimationDefault);
    cam0:addComponent(anim);

    cam0.rcs = cam0:findRenderQuadComponents();
    for _, rc in pairs(cam0.rcs) do
        rc.myPos = rc.pos;
        rc.myHeight = rc.height;
    end
end

function startCam(target, zoom)
    for _, rc in pairs(cam0.rcs) do
        rc.pos = (zoom / 35) * rc.myPos;
        rc.height = (zoom / 35) * rc.myHeight;
    end

    local sat = createSaturation(-200, 200, 0.2);

    local bg = factory:createBackground("cam_lines1.png", (1280 / 27) * (zoom / 35), (720 / 27) * (zoom / 35), vec2(0, 0), 220);
    scene:addObject(bg);

    cam0.pos = scene:getObjects(target)[1].pos;

    scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
    scene.camera:findCameraComponent().target = cam0;

    return {sat, bg};
end

function stopCam(cookie)
    cookie[1]:removeFromParent();
    cookie[2]:removeFromParent();
end

function createStatic1Background(zorder)
    local bg = factory:createBackground("static1/0.png", 20, 20, vec2(0, 0), zorder);
    scene:addObject(bg);

    local rc = bg:findRenderBackgroundComponent();
    local display = AnimationComponent(rc.drawable);
    display:addAnimation(const.AnimationDefault, "static1", 1);
    display:startAnimation(const.AnimationDefault);
    bg:addComponent(display);

    return bg;
end

function setupBgAir()
    local bg = {};
    local scl = 5.0;

    bg[1] = factory:createBackground("ground1.png", 20, 20, vec2(0.2, 0.2), const.zOrderBackground);
    bg[1]:findRenderBackgroundComponent().color = {0.8, 0.8, 0.8, 1.0};
    scene:addObject(bg[1]);

    bg[2] = factory:createBackground("fog.png", 544 / scl / 2, 416 / scl / 2, vec2(10.0, 1.0), const.zOrderBackground + 1)
    bg[2]:findRenderBackgroundComponent().unbound = true;
    bg[2]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.6};
    scene:addObject(bg[2]);

    bg[3] = factory:createBackground("fog.png", 544 / scl / 1.5, 416 / scl / 1.5, vec2(15.0, 1.0), const.zOrderBackground + 2)
    bg[3]:findRenderBackgroundComponent().unbound = true;
    bg[3]:findRenderBackgroundComponent().color = {1.0, 1.0, 1.0, 0.7};
    bg[3]:findRenderBackgroundComponent().offset = vec2(0, 416 / scl / 1.5 / 2);
    scene:addObject(bg[3]);

    return bg;
end

-- main

bg = setupBgAir();

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};

math.randomseed(os.time());

setupCam();

if not scene.playable then
    scene.cutscene = true;

    player0 = factory:createPlayer();
    player0:findPlayerComponent().haveGun = false;
    if settings.developer >= 2 then
        player0:setTransform(scene:getObjects("player2")[1]:getTransform());
    else
        player0:setTransform(scene:getObjects("player"..settings.developer)[1]:getTransform());
    end
    scene:addObject(player0);
    local invC = player0:findInvulnerabilityComponent();
    if invC ~= nil then
        invC:removeFromParent();
    end

    scene.camera:findCameraComponent().target = player0;
end

startMusic("ambient1.ogg", false);

require("e1m11_part0");
require("e1m11_part1");
require("e1m11_part2");

if settings.developer == 2 then
    if not scene.playable then
        startFootage1();
    end
end

if settings.developer == 3 then
    if scene.playable then
        scene.lighting.ambientLight = {0.6, 0.6, 0.6, 1.0};
        scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
    else
        startFootage2();
    end
end
