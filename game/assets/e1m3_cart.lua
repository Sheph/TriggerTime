local platform0 = scene:getObjects("platform0")[1];
cart0 = scene:getObjects("cart0")[1];
local control0 = scene:getObjects("control0_ph")[1]:findPlaceholderComponent();

local function partOnPlatform(platform, part)
    return (part.pos - platform.pos):len() <= 10.4068;
end

local function makePlatform(platform, ...)
    platform.deadbodyAware = true;

    platform:addComponent(CollisionSensorComponent());

    platform.parts = {...};
    platform.objs = {};
    platform.lastAngle = platform.angle;
    platform.startAngle = platform.angle;

    setSensorListener(platform.name, function(other)
        if platform.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            platform.objs[other.cookie] = { count = 1, obj = other };
        else
            platform.objs[other.cookie].count = platform.objs[other.cookie].count + 1;
        end
    end, function (other)
        platform.objs[other.cookie].count = platform.objs[other.cookie].count - 1;
        if platform.objs[other.cookie].count == 0 then
            platform.objs[other.cookie] = nil;
            other.activeDeadbody = false;
        end
    end);
end

local function rotatePlatform(platform, angle, time, easing, fn, ...)
    local function updatePlatform()
        for _, part in pairs(platform.parts) do
            if partOnPlatform(platform, part) then
                local a = platform.angle - platform.lastAngle;
                part:setPosSmoothed(platform.pos + (part.pos - platform.pos):rotated(a));
                part:setAngleSmoothed(part.angle + a);
            end
        end
        for _, v in pairs(platform.objs) do
            local a = platform.angle - platform.lastAngle;
            v.obj:setPosSmoothed(platform.pos + (v.obj.pos - platform.pos):rotated(a));
            v.obj:setAngleSmoothed(v.obj.angle + a);
        end
        platform.lastAngle = platform.angle;
    end

    platform.startAngle = platform.angle;

    local snd = audio:createSound("servo_move.ogg");
    snd:play();
    local tweening = SingleTweening(time, easing, 0, 1, false);
    local t = 0;
    local args = pack2(...);

    addTimeout0(function (cookie, dt)
        if (tweening:finished(t)) then
            local val = tweening:getValue(tweening.duration);
            platform:setAngleSmoothed(platform.startAngle * (1 - val) + angle * val);
            updatePlatform(platform);
            cancelTimeout(cookie);
            if fn then
                fn(unpack2(args));
            end
            return;
        end
        local val = tweening:getValue(t);
        if (val >= 0.9) and (snd ~= nil) then
            snd:stop();
            audio:playSound("servo_stop.ogg");
            snd = nil;
        end
        platform:setAngleSmoothed(platform.startAngle * (1 - val) + angle * val);
        updatePlatform(platform);
        t = t + dt;
    end);
end

function cartDevSetup(dir)
    cart0.myCur = dir;
    platform0.angle = cart0.myAngles[cart0.myCur];
    platform0.lastAngle = platform0.angle;
    platform0.startAngle = platform0.angle;
    cart0.angle = cart0.myAngles[cart0.myCur];
    cart0.myMoved = true;
    cart0.roamBehavior:reset();
    cart0.roamBehavior.linearVelocity = 100;
    cart0.roamBehavior.linearDamping = 50.0;
    cart0.roamBehavior.dampDistance = 3.3;
    cart0.roamBehavior:changePath(cart0.myPaths[cart0.myCur]:findPathComponent().path, cart0.myPaths[cart0.myCur]:getTransform());
    cart0.roamBehavior:start();
    cart0.snd = audio:createSound("cart_move.ogg");
    cart0.snd.loop = true;
    cart0.snd:play();
end

function cart0Stop()
    cart0.roamBehavior:damp();
    cart0.snd:stop();
    cart0.snd = nil;
    audio:playSound("cart_stop.ogg");
end

function cart0Proceed()
    cart0.roamBehavior.linearVelocity = cart0.mySpeed;
    cart0.roamBehavior.dampDistance = 3.3;
    if cart0.snd then
        cart0.snd:stop();
    end
    cart0.snd = audio:createSound("cart_move.ogg");
    cart0.snd.loop = true;
    cart0.snd:play();
end

function cart0Back()
    cart0.myMoved = false;
    cart0.roamBehavior.linearVelocity = -cart0.mySpeed;
    cart0.roamBehavior.dampDistance = 3.3;
    if cart0.snd then
        cart0.snd:stop();
    end
    cart0.snd = audio:createSound("cart_move.ogg");
    cart0.snd.loop = true;
    cart0.snd:play();
end

function cart0Reset()
    cart0.roamBehavior:reset();
    cart0.snd:stop();
    cart0.snd = nil;
    audio:playSound("cart_stop.ogg");
    cart0.pos = platform0.pos;
    cart0.angle = cart0.myAngles[cart0.myCur];
    cart0.myPaths[cart0.myCur] = nil;
end

-- main

cart0.myCur = 2;
cart0.myAngles = {math.rad(19), math.rad(90), math.rad(167)};
cart0.myPaths = { scene:getObjects("path1")[1], scene:getObjects("path2")[1], scene:getObjects("path3")[1] };
cart0.myMoved = false;
cart0.mySpeed = 10.0;
cart0.snd = nil;

makePlatform(platform0, cart0);

do
    local insts = scene:getInstances("lamp_off");
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        for _, light in pairs(lights) do
            light.visible = false;
        end
    end

    insts = scene:getInstances("flicker_lamp_off");
    for _, inst in pairs(insts) do
        local lights = findObject(inst.objects, "lamp"):findLightComponent():getLights();
        for _, light in pairs(lights) do
            light.visible = false;
        end
    end
end

makeFlickerLamps("flicker_lamp");
makeFlickerLamps("flicker_lamp_off");

setSensorEnterListener("dynomite_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    showLowerDialog(
    {
        {"player", tr.dialog21.str1},
        {"player", tr.dialog21.str2},
    }, function ()
        scene.cutscene = false;
        control0.active = true;
        control0.visible = true;
    end);
end);

control0.listener = createSensorListener(function(other, self)
    if cart0.myMoved then
        local items = {};
        table.insert(items, { pos = vec2(0, 0), angle = math.rad(180), height = 6, image = "common2/arrow_button.png" });
        self.choice = showChoice(items, function(i)
            control0.active = false;
            control0.visible = false;
            cart0.myMoved = false;
            cart0.roamBehavior.linearVelocity = -cart0.mySpeed;
            cart0.roamBehavior.dampDistance = 3.3;
            if cart0.snd then
                cart0.snd:stop();
            end
            cart0.snd = audio:createSound("cart_move.ogg");
            cart0.snd.loop = true;
            cart0.snd:play();
            addTimeout0(function(cookie, dt)
                if (cart0.linearVelocity == vec2(0, 0)) and partOnPlatform(platform0, cart0) then
                    cancelTimeout(cookie);
                    cart0.roamBehavior:reset();
                    cart0.snd:stop();
                    cart0.snd = nil;
                    audio:playSound("cart_stop.ogg");
                    cart0.pos = platform0.pos;
                    cart0.angle = cart0.myAngles[cart0.myCur];
                    control0.active = true;
                    control0.visible = true;
                end
            end);
        end);
        self.choice.angle = cart0.myAngles[cart0.myCur];
        return;
    end
    local items = {};
    table.insert(items, { pos = vec2(7, 0), angle = math.rad(0), height = 6, image = "common2/arrow_button.png" });
    if cart0.myCur > 1 then
        table.insert(items, { pos = vec2(0, -7), angle = math.rad(-90), height = 6, image = "common2/arrow_button.png" });
    end
    if cart0.myCur < #cart0.myAngles then
        table.insert(items, { pos = vec2(0, 7), angle = math.rad(90), height = 6, image = "common2/arrow_button.png" });
    end
    self.choice = showChoice(items, function(i, hasRight)
        control0.active = false;
        control0.visible = false;
        if i == 0 then
            if cart0.myPaths[cart0.myCur] == nil then
                audio:playSound("cart_denied.ogg");
                addTimeoutOnce(1.0, function()
                    control0.active = true;
                    control0.visible = true;
                end);
                return;
            end
            cart0.myMoved = true;
            cart0.roamBehavior:reset();
            cart0.roamBehavior.linearVelocity = cart0.mySpeed;
            cart0.roamBehavior.linearDamping = 3.0;
            cart0.roamBehavior.dampDistance = 3.3;
            cart0.roamBehavior:changePath(cart0.myPaths[cart0.myCur]:findPathComponent().path, cart0.myPaths[cart0.myCur]:getTransform());
            cart0.roamBehavior:start();
            cart0.snd = audio:createSound("cart_move.ogg");
            cart0.snd.loop = true;
            cart0.snd:play();
            return;
        elseif i == 1 then
            if hasRight then
                cart0.myCur = cart0.myCur - 1;
                rotatePlatform(platform0, cart0.myAngles[cart0.myCur], 2.0, const.EaseInOutQuad, function()
                    control0.active = true;
                    control0.visible = true;
                end);
                return;
            end
        end
        cart0.myCur = cart0.myCur + 1;
        rotatePlatform(platform0, cart0.myAngles[cart0.myCur], 2.0, const.EaseInOutQuad, function()
            control0.active = true;
            control0.visible = true;
        end);
    end, cart0.myCur > 1);
    self.choice.angle = cart0.myAngles[cart0.myCur];
end, function(other, self)
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
end, { choice = nil });

setSensorEnterListener("cart_back_cp", false, function()
    if cart0.myMoved then
        control0.active = true;
        control0.visible = true;
    end
end);
