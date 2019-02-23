local train = scene:getObjects("train0")[1];
local trainDoor = nil;
local train0data = {
    cur = 1,
    expanded = true,
    lower = 1,
    upper = 3,
    center = scene:getObjects("arena0")[1].pos,
    points = {
        [1] = {
            [1] = scene:getObjects("train_dest1a")[1].pos,
            [2] = scene:getObjects("train_dest1b")[1].pos,
        },
        [2] = {
            [1] = scene:getObjects("train_dest2a")[1].pos,
            [2] = scene:getObjects("train_dest2b")[1].pos,
        },
        [3] = {
            [1] = scene:getObjects("train_dest3a")[1].pos,
            [2] = scene:getObjects("train_dest3b")[1].pos,
        }
    },
    blockers = {
        [1] = scene:getObjects("train_dest1_blocker")[1],
        [2] = scene:getObjects("train_dest2_blocker")[1],
        [3] = scene:getObjects("train_dest3_blocker")[1]
    }
};
local train0ph = scene:getObjects("train0_ph")[1]:findPlaceholderComponent();
local trainBlocker = scene:getObjects("train0_blocker")[1];
local interacted = false;

local function makePlatform(platform, parts)
    platform.deadbodyAware = true;

    local function enterFunc(other)
        if platform.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            platform.objs[other.cookie] = { count = 1, obj = other };
        else
            platform.objs[other.cookie].count = platform.objs[other.cookie].count + 1;
        end
    end;

    local c = CollisionSensorComponent();
    platform:addComponent(c);
    platform.objs = {};
    platform.lastPos = platform.pos;
    platform.lastAngle = platform.angle;

    if parts ~= nil then
        for _, p in pairs(parts) do
            enterFunc(p);
        end
    end

    c.listener = createSensorListener(enterFunc, function (other)
        platform.objs[other.cookie].count = platform.objs[other.cookie].count - 1;
        if platform.objs[other.cookie].count == 0 then
            platform.objs[other.cookie] = nil;
            other.activeDeadbody = false;
        end
    end);
end

local function updatePlatform(platform)
    local pa = platform.angle;
    local pp = platform.pos;
    local a = pa - platform.lastAngle;
    for _, v in pairs(platform.objs) do
        v.obj:setPosSmoothed(pp + (v.obj.pos - platform.lastPos):rotated(a));
        v.obj:setAngleSmoothed(v.obj.angle + a);
    end
    platform.lastPos = pp;
    platform.lastAngle = pa;
    scene.respawnPoint = transform(platform.pos + platform:getDirection(3.0), rot(platform.angle));
end

local function moveTrainDoor(jointName, open, fn, ...)
    local snd = nil;
    if trainDoor ~= nil then
        snd = trainDoor.snd;
        cancelTimeout(trainDoor.cookie);
        trainDoor = nil;
    end
    local j = scene:getJoints(jointName)[1];
    if (not open and j:getJointTranslation() >= j.upperLimit) or
       (open and j:getJointTranslation() <= j.lowerLimit) then
        if snd ~= nil then
            snd:stop();
        end
        if fn then
            fn(...);
        end
        return;
    end
    trainDoor = {};
    if snd == nil then
        trainDoor.snd = audio:createSound("door_move.ogg");
        trainDoor.snd.loop = true;
        trainDoor.snd:play();
    else
        trainDoor.snd = snd;
    end
    if open then
        j.motorSpeed = -math.abs(j.motorSpeed);
    else
        j.motorSpeed = math.abs(j.motorSpeed);
    end
    trainDoor.cookie = addTimeout0(function(cookie, dt, args)
        if (not open and j:getJointTranslation() >= j.upperLimit) or
           (open and j:getJointTranslation() <= j.lowerLimit) then
            trainDoor.snd:stop();
            trainDoor = nil;
            cancelTimeout(cookie);
            if fn then
                fn(unpack2(args));
            end
        end
    end, pack2(...));
end

local function moveTrain(expand, room, time, easing, fn, ...)
    local snd = audio:createSound("servo_move.ogg");
    snd:play();
    local tweening = nil;
    if expand then
        tweening = SingleTweening(time, easing, 0, 1, false);
    else
        tweening = SingleTweening(time, easing, 1, 0, false);
    end
    addTimeout0(function (cookie, dt, self)
        if (self.tweening:finished(self.t)) then
            local val = self.tweening:getValue(self.tweening.duration);
            local c = train0data.center;
            local p1 = train0data.points[self.room][1];
            local p2 = train0data.points[self.room][2];
            local p = p1 * (1 - val) + p2 * val;
            train:setPosSmoothed(p);
            train:setAngleSmoothed(normalizeAngle((p - c):angle()));
            updatePlatform(train);
            cancelTimeout(cookie);
            train0data.expanded = self.expand;
            train0data.cur = self.room;
            if self.fn then
                self.fn(unpack2(self.args));
            end
            return;
        end
        local val = self.tweening:getValue(self.t);
        if ((self.expand and (val >= 0.9)) or (not self.expand and (val <= 0.1))) and (self.snd ~= nil) then
            self.snd:stop();
            audio:playSound("servo_stop.ogg");
            self.snd = nil;
        end
        local c = train0data.center;
        local p1 = train0data.points[self.room][1];
        local p2 = train0data.points[self.room][2];
        local p = p1 * (1 - val) + p2 * val;
        train:setPosSmoothed(p);
        train:setAngleSmoothed(normalizeAngle((p - c):angle()));
        updatePlatform(train);
        self.t = self.t + dt;
    end, { expand = expand, snd = snd, room = room, t = 0, tweening = tweening, fn = fn, args = pack2(...) });
end

local function rotateTrain(room1, room2, time, easing, fn, ...)
    local snd = audio:createSound("servo_move.ogg");
    snd:play();
    local tweening = SingleTweening(time, easing, 0, 1, false);
    addTimeout0(function (cookie, dt, self)
        if (self.tweening:finished(self.t)) then
            local val = self.tweening:getValue(self.tweening.duration);
            local c = train0data.center;
            local p0 = train0data.points[2][1];
            local p1 = train0data.points[self.room1][1];
            local p2 = train0data.points[self.room2][1];
            local angle = (normalizeAngle((p0 - c):angle() - (p1 - c):angle()) + normalizeAngle((p2 - c):angle() - (p0 - c):angle())) * val;
            local p = c + (p1 - c):rotated(angle);
            train:setPosSmoothed(p);
            train:setAngleSmoothed(normalizeAngle((p - c):angle()));
            updatePlatform(train);
            cancelTimeout(cookie);
            train0data.cur = self.room2;
            if self.fn then
                self.fn(unpack2(self.args));
            end
            return;
        end
        local val = self.tweening:getValue(self.t);
        if (val >= 0.9) and (self.snd ~= nil) then
            self.snd:stop();
            audio:playSound("servo_stop.ogg");
            self.snd = nil;
        end
        local c = train0data.center;
        local p0 = train0data.points[2][1];
        local p1 = train0data.points[self.room1][1];
        local p2 = train0data.points[self.room2][1];
        local angle = (normalizeAngle((p0 - c):angle() - (p1 - c):angle()) + normalizeAngle((p2 - c):angle() - (p0 - c):angle())) * val;
        local p = c + (p1 - c):rotated(angle);
        train:setPosSmoothed(p);
        train:setAngleSmoothed(normalizeAngle((p - c):angle()));
        updatePlatform(train);
        self.t = self.t + dt;
    end, { snd = snd, room1 = room1, room2 = room2, t = 0, tweening = tweening, fn = fn, args = pack2(...) });
end

local function showInteractInputHint()
    addTimeoutOnce(1.0, function()
        local obj = SceneObject();
        local c = nil;

        if input.usingGamepad then
            obj.pos = vec2(12, scene.gameHeight - 6);
            c = InputHintComponent(-2);
            addGamepadBindingHint(c, const.ActionGamepadIdInteract, true);
            c:setDescription(tr.str22);
            obj:addComponent(c);
        else
            obj.pos = vec2(12, scene.gameHeight - 6);
            c = InputHintComponent(-2);
            addBindingHint(c, const.ActionIdInteract, true);
            c:setDescription(tr.str22);
            obj:addComponent(c);
        end

        scene:addObject(obj);

        addTimeout0(function(cookie, dt)
            if interacted then
                cancelTimeout(cookie);
                addTimeoutOnce(2.0, function()
                    c:setFade(1.0);
                end);
            end
        end);
    end);
end

function setupTrain(room)
    local tmp = scene.respawnPoint;
    train0data.cur = room;
    train.pos = train0data.points[train0data.cur][2];
    train.angle = normalizeAngle((train.pos - train0data.center):angle());
    updatePlatform(train);
    if room ~= 1 then
        train0data.blockers[1].active = true;
        train0data.blockers[train0data.cur].active = false;
    end
    scene.respawnPoint = tmp;
end

-- main

setSensorEnterListener("rock4_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("rock4_cp")[1]:getTransform();
    setupBgAir();
end);

setSensorEnterListener("rock5_cp", false, function(other)
    scene.respawnPoint = scene:getObjects("rock5_cp")[1]:getTransform();
    setupBgMetal();
end);

setSensorEnterListener("red_door_encounter_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.player.angle = (scene:getObjects("red_door")[1].pos - scene.player.pos):angle();
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog13.str1},
            {"player", tr.dialog13.str2},
        }, function ()
            scene.cutscene = false;
        end);
    end);
end);

setSensorEnterListener("train_encounter_cp", true, function(other)
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    scene.player.angle = (train.pos - scene.player.pos):angle();
    addTimeoutOnce(1.0, function()
        showLowerDialog(
        {
            {"player", tr.dialog14.str1},
        }, function ()
            scene.cutscene = false;
            showInteractInputHint();
        end);
    end);
end);

makeGear("train0", "train0_door_joint", "train0_cogwheel", "train0_cogwheel_joint", 1);
makePlatform(train, { scene:getObjects("train0_door")[1],
    scene:getObjects("train0_cogwheel")[1],
    scene:getObjects("train0_ph")[1],
    trainBlocker });

train0ph.listener = createSensorListener(function(other, self)
    local p1 = train0data.points[train0data.cur][1];
    local p2 = train0data.points[train0data.cur][2];
    local angle = normalizeAngle((p2 - p1):angle() + (math.pi / 2));
    if train0data.expanded then
        self.choice = showChoice({
            { pos = vec2(0, 0), angle = math.rad(90), height = 6, image = "common2/arrow_button.png" },
        }, function (i)
            interacted = true;
            train0ph.active = false;
            train0ph.visible = false;
            trainBlocker.active = true;
            scene.respawnPoint = transform(train.pos + train:getDirection(3.0), rot(train.angle));
            moveTrainDoor("train0_door_joint", false, function()
                train0data.blockers[train0data.cur].active = true;
                addTimeoutOnce(0.8, function()
                    moveTrain(false, train0data.cur, 2.2, const.EaseInOutQuad, function ()
                        train0ph.active = true;
                        train0ph.visible = true;
                    end);
                end);
            end);
        end);
        self.choice.angle = angle;
        return;
    end

    local items = {};
    local hasUpper = train0data.cur < train0data.upper;
    local hasLower = train0data.cur > train0data.lower;

    table.insert(items, { pos = vec2(0, -7), angle = math.rad(-90), height = 6, image = "common2/arrow_button.png" });
    if hasUpper then
        table.insert(items, { pos = vec2(7, 0), angle = math.rad(0), height = 6, image = "common2/arrow_button.png" });
    end
    if hasLower then
        table.insert(items, { pos = vec2(-7, 0), angle = math.rad(180), height = 6, image = "common2/arrow_button.png" });
    end

    self.choice = showChoice(items, function (i, hasUpper)
        train0ph.active = false;
        train0ph.visible = false;
        if i == 0 then
            moveTrain(true, train0data.cur, 2.2, const.EaseInOutQuad, function ()
                train0data.blockers[train0data.cur].active = false;
                addTimeoutOnce(0.8, function()
                    trainBlocker.active = false;
                    moveTrainDoor("train0_door_joint", true, function()
                        train0ph.active = true;
                        train0ph.visible = true;
                    end);
                end);
            end);
        elseif i == 1 then
            if hasUpper then
                rotateTrain(train0data.cur, train0data.cur + 1, 2.2, const.EaseInOutQuad, function ()
                    train0ph.active = true;
                    train0ph.visible = true;
                end);
            else
                rotateTrain(train0data.cur, train0data.cur - 1, 2.2, const.EaseInOutQuad, function ()
                    train0ph.active = true;
                    train0ph.visible = true;
                end);
            end
        else
            rotateTrain(train0data.cur, train0data.cur - 1, 2.2, const.EaseInOutQuad, function ()
                train0ph.active = true;
                train0ph.visible = true;
            end);
        end
    end, hasUpper);
    self.choice.angle = angle;
end, function(other, self)
    if self.choice ~= nil then
        self.choice:removeFromParent();
    end
end, { choice = nil });

makeKeyDoor("red_door", "red_key_ph", function()
    scene:getObjects("ga4")[1]:findGoalAreaComponent():removeGoal(scene:getObjects("red_door")[1].pos);
    local objs = scene:getObjects("red_door_laser");
    for _, obj in pairs(objs) do
        obj:addComponent(FadeOutComponent(1.0));
    end
end);
