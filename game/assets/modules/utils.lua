--- Global utility definitions.
-- You can refer to these without a module qualifier, i.e.:
--
--    print(normalizeAngle(math.pi * 5));
--
-- @module utils

function pack2(...)
    return { n = select('#', ...), ... };
end

function unpack2(t)
    return unpack(t, 1, t.n);
end

--- Normalize angle to -pi..pi.
-- @number angle Angle
-- @treturn number
function normalizeAngle(angle)
    angle = math.fmod(angle + math.pi, 2.0 * math.pi);

    if angle < 0 then
        angle = angle + 2.0 * math.pi;
    end

    return angle - math.pi;
end

--- Returns a shallow copy of arrays.
-- A variable number of additional arrays can be passed in as
-- optional arguments. If an array has a hole (a nil entry),
-- copying in a given source array stops at the last consecutive
-- item prior to the hole.
-- @tab t Array
-- @tab[opt=nil] t2 Array
-- @tab[opt=nil] tN Array
-- @treturn tab
table.copy = function(t, ...)
    local copyShallow = function(src, dst, dstStart)
        local result = dst or {};
        local resultStart = 0;
        if dst and dstStart then
            resultStart = dstStart;
        end
        local resultLen = 0;
        if "table" == type(src) then
            resultLen = #src;
            for i = 1, resultLen do
                local value = src[i];
                if nil ~= value then
                    result[i + resultStart] = value;
                else
                    resultLen = i - 1;
                    break;
                end
            end
        end
        return result, resultLen;
    end

    local result, resultStart = copyShallow(t);

    local srcs = { ... }
    for i = 1, #srcs do
        local _, len = copyShallow(srcs[i], result, resultStart);
        resultStart = resultStart + len;
    end

    return result;
end

--- Removes elements from a table based on condition.
-- @tab t Table
-- @func func Condition functor
-- @treturn int Number of elements removed
function table.remove_if(t, func)
    local num = 0;
    for k, v in pairs(t) do
        if func(k, v) then
            t[k] = nil;
            num = num + 1;
        end
    end
    return num;
end

table.size = function(t)
    local cnt = 0;
    for k, v in pairs(t) do
        cnt = cnt + 1;
    end
    return cnt;
end

table.shuffle = function(t)
    local iterations = #t;
    local j;

    for i = iterations, 2, -1 do
        j = math.random(i)
        t[i], t[j] = t[j], t[i]
    end
end

--- Executes a function after given time.
-- @number timeout After this time
-- @tparam func|tab func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
-- @treturn int Timer cookier
function addTimeout(timeout, func, ...)
    if type(func) == "table" then
        local timer = nil;
        local args = pack2(...);
        local t = timeout;
        timer = scene:addTimer(function(dt)
            t = t - dt;
            if (t < 0) then
                t = timeout;
                func.update(timer, unpack2(args));
            end;
        end);
        return timer;
    else
        local timer = nil;
        local args = pack2(...);
        local t = timeout;
        timer = scene:addTimer(function(dt)
            t = t - dt;
            if (t < 0) then
                t = timeout;
                func(timer, unpack2(args));
            end;
        end);
        return timer;
    end
end

--- Executes a function after given time once.
-- @number timeout After this time
-- @tparam func|tab func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
-- @treturn int Timer cookier
function addTimeoutOnce(timeout, func, ...)
    if type(func) == "table" then
        local timer = nil;
        local args = pack2(...);
        local t = timeout;
        timer = scene:addTimer(function(dt)
            t = t - dt;
            if (t < 0) then
                scene:removeTimer(timer);
                func.update(unpack2(args));
            end;
        end);
        return timer;
    else
        local timer = nil;
        local args = pack2(...);
        local t = timeout;
        timer = scene:addTimer(function(dt)
            t = t - dt;
            if (t < 0) then
                scene:removeTimer(timer);
                func(unpack2(args));
            end;
        end);
        return timer;
    end
end

function addTimeout0(func, ...)
    if type(func) == "table" then
        local timer = nil;
        local args = pack2(...);
        timer = scene:addTimer(function(dt)
            func.update(timer, dt, unpack2(args));
        end);
        return timer;
    else
        local timer = nil;
        local args = pack2(...);
        timer = scene:addTimer(function(dt)
            func(timer, dt, unpack2(args));
        end);
        return timer;
    end
end

--- Cancels timeout created by @{addTimeout}.
-- @int cookie Timer cookie returned from @{addTimeout}
function cancelTimeout(cookie)
    scene:removeTimer(cookie);
end

--- Enter-only sensor listener helper.
-- @bool once Only call once
-- @func func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
-- @treturn SensorListener Sensor listener
function createSensorEnterListener(once, func, ...)
    class 'UtilListener' (SensorListener)

    function UtilListener:__init(once, func, args)
        SensorListener.__init(self, self);
        self.once = once;
        self.func = func;
        self.args = args;
        self.done = false;
    end

    function UtilListener:sensorEnter(other)
        if self.once and self.done then
            return;
        end
        self.done = true;
        self.func(other, unpack2(self.args));
    end

    function UtilListener:sensorExit(other)
    end

    return UtilListener(once, func, pack2(...));
end

--- Set enter-only sensor listener.
-- @string name Sensor object name
-- @bool once Only call once
-- @func func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
function setSensorEnterListener(name, once, func, ...)
    local objs = scene:getObjects(name);
    for _, obj in pairs(objs) do
        obj:findCollisionSensorComponent().listener = createSensorEnterListener(once, func, ...);
    end
end

function createSensorListener(enterFunc, exitFunc, ...)
    class 'UtilListener' (SensorListener)

    function UtilListener:__init(enterFunc, exitFunc, args)
        SensorListener.__init(self, self);
        self.enterFunc = enterFunc;
        self.exitFunc = exitFunc;
        self.args = args;
    end

    function UtilListener:sensorEnter(other)
        self.enterFunc(other, unpack2(self.args));
    end

    function UtilListener:sensorExit(other)
        self.exitFunc(other, unpack2(self.args));
    end

    return UtilListener(enterFunc, exitFunc, pack2(...));
end

function setSensorListener(name, enterFunc, exitFunc, ...)
    local obj = scene:getObjects(name)[1];
    obj:findCollisionSensorComponent().listener = createSensorListener(enterFunc, exitFunc, ...);
end

local function showDialog(x, y, w, h, lines, func, ...)
    class 'UtilDialog' (DialogComponent)

    function UtilDialog:__init(lines, func, args)
        DialogComponent.__init(self, self, vec2(x, y), w, h);
        self.lines = lines;
        self.func = func;
        self.args = args;
        self.i = 2;
    end

    function UtilDialog:onRegister()
        if self.lines[1][1] == "player" then
            self:setTitlePlayer(tr.str55);
            self:setMessage(self.lines[1][2]);
        elseif self.lines[1][1] == "ally" then
            self:setTitleAlly(self.lines[1][2], self.lines[1][3]);
            self:setMessage(self.lines[1][4]);
        else
            self:setTitleEnemy(self.lines[1][2], self.lines[1][3]);
            self:setMessage(self.lines[1][4]);
        end
    end

    function UtilDialog:onUnregister()
    end

    function UtilDialog:okPressed()
        if self.i > #self.lines then
            self:endDialog();
            self.func(unpack2(self.args));
        else
            if self.lines[self.i][1] == "player" then
                self:setTitlePlayer(tr.str55);
                self:setMessage(self.lines[self.i][2]);
            elseif self.lines[self.i][1] == "ally" then
                self:setTitleAlly(self.lines[self.i][2], self.lines[self.i][3]);
                self:setMessage(self.lines[self.i][4]);
            else
                self:setTitleEnemy(self.lines[self.i][2], self.lines[self.i][3]);
                self:setMessage(self.lines[self.i][4]);
            end
            self.i = self.i + 1;
        end
    end

    local dialog = SceneObject();
    dialog:addComponent(UtilDialog(lines, func, pack2(...)));
    scene:addObject(dialog);

    return dialog;
end

--- Show lower dialog.
-- @tab lines Dialog lines
-- @func func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
-- @treturn SceneObject Dialog object
function showLowerDialog(lines, func, ...)
    return showDialog(scene.gameWidth / 2 - 15, 1, 30, 8, lines, func, ...);
end

function showUpperDialog(lines, func, ...)
    return showDialog(scene.gameWidth / 2 - 15, scene.gameHeight - 10, 30, 8, lines, func, ...);
end

function showMsg(x, y, w, h, timeout, line, func, ...)
    class 'UtilDialog' (DialogComponent)

    function UtilDialog:__init(line)
        DialogComponent.__init(self, self, vec2(x, y), w, h);
        self.fastComplete = false;
        self.line = line;
    end

    function UtilDialog:onRegister()
        if self.line[1] == "player" then
            self:setTitlePlayer(tr.str55);
            self:setMessage(self.line[2]);
        elseif self.line[1] == "ally" then
            self:setTitleAlly(self.line[2], self.line[3]);
            self:setMessage(self.line[4]);
        else
            self:setTitleEnemy(self.line[2], self.line[3]);
            self:setMessage(self.line[4]);
        end
    end

    function UtilDialog:onUnregister()
    end

    function UtilDialog:okPressed()
    end

    local dialog = SceneObject();
    local dialogC = UtilDialog(line);
    dialog:addComponent(dialogC);
    scene:addObject(dialog);

    addTimeoutOnce(timeout, function(self)
        self.dialogC:endDialog();
        if self.func ~= nil then
            self.func(unpack2(self.args));
        end
    end, { dialogC = dialogC, func = func, args = pack2(...) });

    return dialog;
end

function showUpperMsg(timeout, line, func, ...)
    return showMsg(scene.gameWidth / 2 - 15, scene.gameHeight - 10, 30, 8, timeout, line, func, ...);
end

function showLowerMsg(timeout, line, func, ...)
    return showMsg(scene.gameWidth / 2 - 15, 1, 30, 8, timeout, line, func, ...);
end

--- Creates simple collision handler.
-- @func func Functor to call
-- @param[opt] arg1
-- @param[opt] arg2
-- @param[opt] argN
-- @treturn CollisionComponent Collision component
function createCollisionHandler(func, ...)
    class 'UtilCollisionHandler' (CollisionComponent)

    function UtilCollisionHandler:__init(func, args)
        CollisionComponent.__init(self, self);
        self.func = func;
        self.args = args;
        self.cookies = {};
    end

    function UtilCollisionHandler:beginCollision(cookie, other, points, normalImpulses, tangentImpulses)
        self.cookies[cookie] = {other, points};
        self.func(0, other, points, normalImpulses, tangentImpulses, unpack2(self.args));
    end

    function UtilCollisionHandler:updateCollision(cookie, normalImpulses, tangentImpulses)
        self.func(1, self.cookies[cookie][1], self.cookies[cookie][2], normalImpulses, tangentImpulses, unpack2(self.args));
    end

    function UtilCollisionHandler:endCollision(cookie)
        self.func(2, self.cookies[cookie][1], self.cookies[cookie][2], nil, nil, unpack2(self.args));
        local newCookies = {};
        for i, v in pairs(self.cookies) do
            if i ~= cookie then
                newCookies[i] = v;
            end
        end
        self.cookies = newCookies;
    end

    function UtilCollisionHandler:update(dt)
    end

    function UtilCollisionHandler:onRegister()
    end

    function UtilCollisionHandler:onUnregister()
    end

    return UtilCollisionHandler(func, pack2(...));
end

function showMissionDetails(level)
    table.insert(UIDocList, "ui/shop.rml");
    local nd = rocket.contexts["menu"]:LoadDocument("ui/shop.rml");
    UIShop.Level = level;
    nd:Show();
end

function completeLevel(timeout, thisMission, nextMission, nextLevelName, noshop, fn)
    addTimeoutOnce(timeout, function()
        scene.paused = true;

        if fn ~= nil then
            fn();
        end

        local es = scene.stats.enemiesSpawned;
        local ek = scene.stats.enemiesKilled;
        if (ek > es) then
            print("enemies killed("..ek..") > enemies spawned("..es..") - wierd!");
            ek = es;
        end

        local pt = scene.stats.playTime;

        if scene.stats.accuracy >= 0.85 then
            gameShell:setAchieved(const.AchievementAimBot);
        end

        if not scene.player:findPlayerComponent().flagLifeLost then
            gameShell:setAchieved(const.AchievementHeartbroken);
        end

        if (scene.player:findPlayerComponent().numHearts == 0) and (scene.player:lifePercent() < 0.25) then
            gameShell:setAchieved(const.AchievementThatWasClose);
        end

        if not scene.player:findPlayerComponent().flagDamaged then
            gameShell:setAchieved(const.AchievementNotAScratch);
        end

        UICompleted.Mission = thisMission;
        UICompleted.Killed = ek.."/"..es;
        UICompleted.Accuracy = string.format("%d%%", scene.stats.accuracy * 100);
        UICompleted.Time = string.format("%.2d:%.2d:%.2d", pt / (60 * 60), (pt / 60) % 60, pt % 60);
        UICompleted.NextMission = nextMission;
        UICompleted.NextLevelName = nextLevelName;
        UICompleted.NoShop = noshop;
        rocket.contexts["menu"]:LoadDocument("ui/completed.rml");
    end);
end

function gameover()
    scene.paused = true;
    rocket.contexts["menu"]:LoadDocument("ui/gameover.rml");
end

function makeGear(objNameA, jointNameA, objNameB, jointNameB, ratio, collide)
    local part1 = scene:getObjects(objNameA)[1];
    local part1_joint = scene:getJoints(jointNameA)[1];
    local part2 = scene:getObjects(objNameB)[1];
    local part2_joint = scene:getJoints(jointNameB)[1];
    if collide == nil then
        collide = false;
    end
    return scene:addGearJoint(part1, part2, part1_joint, part2_joint, ratio, collide);
end

function showChoice(items, func, ...)
    class 'UtilChoice' (ChoiceComponent)

    function UtilChoice:__init(func, args)
        ChoiceComponent.__init(self, self, 0);
        self.func = func;
        self.args = args;
    end

    function UtilChoice:onRegister()
    end

    function UtilChoice:onUnregister()
    end

    function UtilChoice:onPress(i)
        if i < 0 then
            return;
        end
        self.parent:removeFromParent();
        self.func(i, unpack2(self.args));
    end

    local choice = SceneObject();
    choice.pos = vec2(scene.gameWidth / 2, scene.gameHeight / 2);
    local choiceC = UtilChoice(func, pack2(...));

    for _, item in ipairs(items) do
        choiceC:addItem(item.pos, item.angle, item.height, item.image);
    end

    choice:addComponent(choiceC);
    scene:addObject(choice);

    return choice;
end

function findObject(objs, name)
    for _, obj in pairs(objs) do
        if obj.name == name then
            return obj;
        end
    end
    return nil;
end

function findObjects(objs, name)
    local ret = {};
    for _, obj in pairs(objs) do
        if obj.name == name then
            table.insert(ret, obj);
        end
    end
    return ret;
end

function findJoint(joints, name)
    for _, j in pairs(joints) do
        if j.name == name then
            return j;
        end
    end
    return nil;
end

function stainedGlass(fromColor, toColor, easing, timeout, fn, ...)
    local c = StainedGlassComponent(-100);
    c:setColor(fromColor, const.EaseLinear, 0);
    c:setColor(toColor, easing, timeout);

    local obj = SceneObject();
    obj:addComponent(c);
    scene:addObject(obj);

    addTimeoutOnce(timeout, function(args)
        obj:removeFromParent();
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function summon1(dummy, haveSound, fn, ...)
    local obj = factory:createSummon1(haveSound, const.zOrderExplosion);
    obj:setTransform(dummy:getTransform());
    scene:addObject(obj);
    addTimeoutOnce(2.0, function(args)
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function summon2(dummy, haveSound, fn, ...)
    local obj = factory:createSummon2(haveSound, const.zOrderExplosion);
    obj:setTransform(dummy:getTransform());
    scene:addObject(obj);
    addTimeoutOnce(2.0, function(args)
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function addSpawnedEnemy(enemy, spawner)
    enemy:setTransformRecursive(spawner:getTransform());
    local ec = enemy:findTargetableComponent();
    ec.autoTarget = true;
    scene:addObject(enemy);
end

function addSpawnedEnemyFreezable(enemy, spawner)
    enemy.freezable = spawner.freezable;
    enemy.freezeRadius = spawner.freezeRadius;
    enemy:setTransformRecursive(spawner:getTransform());
    local ec = enemy:findTargetableComponent();
    ec.autoTarget = true;
    scene:addObject(enemy);
end

function removeTentacle(obj)
    local bones = obj:findRenderTentacleComponent().objects;
    for _, bone in pairs(bones) do
        bone:removeFromParent();
    end
end

function showCountdown(w, h)
    local obj = SceneObject();
    obj.pos = vec2((scene.gameWidth - w) / 2, scene.gameHeight - (h + 2));
    local c = CountdownComponent(w, h);
    obj:addComponent(c);
    scene:addObject(obj);

    return c;
end

function createSaturation(lowZOrder, highZOrder, saturation)
    local obj = SceneObject();
    obj:addComponent(RenderSaturationComponent(saturation, lowZOrder));
    obj:addComponent(RenderSaturationComponent(1.0, highZOrder));
    scene:addObject(obj);

    return obj;
end

function addBindingHint(c, actionId, both)
    local ib = input:binding(actionId):ib(0);
    if ib:empty() then
        return;
    end
    if ib:isKey() then
        if ib:isLongKey() then
            c:addKbLong(ib.str);
        else
            c:addKbNormal(ib.str);
        end
    else
        if ib:isMb(true) then
            c:addMb(true);
        else
            c:addMb(false);
        end
    end
    if both then
        local ib = input:binding(actionId):ib(1);
        if ib:empty() then
            return;
        end
        c:addText(",");
        if ib:isKey() then
            if ib:isLongKey() then
                c:addKbLong(ib.str);
            else
                c:addKbNormal(ib.str);
            end
        else
            if ib:isMb(true) then
                c:addMb(true);
            else
                c:addMb(false);
            end
        end
    end
end

function addGamepadBindingHint(c, actionId, both)
    local ib = input:gamepadBinding(actionId):ib(0);
    if ib:empty() then
        return;
    end
    c:addGamepadButton(ib.button);
    if both then
        local ib = input:gamepadBinding(actionId):ib(1);
        if ib:empty() then
            return;
        end
        c:addText(",");
        c:addGamepadButton(ib.button);
    end
end
