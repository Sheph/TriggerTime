--- Global UI utility definitions.
-- You can refer to these without a module qualifier.
--
-- @module ui_utils
--

UIDocList = {};
UIInTransition = false;
UICompleted = {};

local function UIAddTimer(zorder, timeout, func, ...)
    class 'UtilUITimer' (UITimerComponent)

    function UtilUITimer:__init(zorder, timeout, func, args)
        UITimerComponent.__init(self, self, timeout, zorder);
        self.func = func;
        self.args = args;
    end

    function UtilUITimer:onRegister()
    end

    function UtilUITimer:onUnregister()
    end

    function UtilUITimer:timeoutReached(dt)
        self.func(dt, unpack2(self.args));
    end

    local timer = SceneObject();
    timer:addComponent(UtilUITimer(zorder, timeout, func, pack2(...)));
    scene:addObject(timer);

    return timer;
end

function UIAddTimeout(zorder, timeout, func, ...)
    local f = func;
    local timer;
    timer = UIAddTimer(zorder, timeout, function(dt, ...)
        f(timer, ...);
    end, ...);
    return timer;
end

function UIAddTimeoutOnce(zorder, timeout, func, ...)
    local f = func;
    local timer;
    timer = UIAddTimer(zorder, timeout, function(dt, ...)
        timer:removeFromParent();
        f(...);
    end, ...);
    return timer;
end

function UIAddTimeout0(zorder, func, ...)
    local f = func;
    local timer;
    timer = UIAddTimer(zorder, 0, function(dt, ...)
        f(timer, dt, ...);
    end, ...);
    return timer;
end

function UICancelTimeout(cookie)
    cookie:removeFromParent();
end

function UITransitionElementOut(zorder, elem, duration, x2, y2, fn, ...)
    local tweening = SingleTweening(duration, const.EaseInQuad, 0.0, 1.0, false);
    local t = 0;

    local x1 = 0;
    if (elem.style.left ~= "0") and (elem.style.left ~= "0.0000px") then
        x1 = elem:GetAbsoluteLeft();
    end

    local y1 = 0;
    if (elem.style.top ~= "0") and (elem.style.top ~= "0.0000px") then
        y1 = elem:GetAbsoluteTop();
    end

    local args = pack2(...);

    UIAddTimeout0(zorder, function(cookie, dt)
        t = t + dt;
        local v = tweening:getValue(t);
        elem.style.left = (x1 + v * x2).."px";
        elem.style.top = (y1 + v * y2).."px";
        if tweening:finished(t) then
            UICancelTimeout(cookie);
            if fn ~= nil then
                fn(unpack2(args));
            end
        end
    end);
end

function UITransitionElementIn(zorder, elem, duration, x2, y2, fn, ...)
    local tweening = SingleTweening(duration, const.EaseOutQuad, 1.0, 0.0, false);
    local t = 0;

    local x1 = 0;
    if (elem.style.left ~= "0") and (elem.style.left ~= "0.0000px") then
        x1 = elem:GetAbsoluteLeft();
    end

    local y1 = 0;
    if (elem.style.top ~= "0") and (elem.style.top ~= "0.0000px") then
        y1 = elem:GetAbsoluteTop();
    end

    elem.style.left = (x1 + x2).."px";
    elem.style.top = (y1 + y2).."px";

    local args = pack2(...);

    UIAddTimeout0(zorder, function(cookie, dt)
        t = t + dt;
        local v = tweening:getValue(t);
        elem.style.left = (x1 + v * x2).."px";
        elem.style.top = (y1 + v * y2).."px";
        if tweening:finished(t) then
            UICancelTimeout(cookie);
            if fn ~= nil then
                fn(unpack2(args));
            end
        end
    end);
end

function UIListSize(lst)
    local cnt = 0;
    for _, v in rocket_ipairs(lst) do
        cnt = cnt + 1;
    end
    return cnt;
end

function UIShowDialog(document, text, smallText, fn, ...)
    local nd = document.context:LoadDocument("ui/dialog.rml");
    local elem = nd:GetElementById("dialog_text");
    elem.inner_rml = text;
    elem = nd:GetElementById("dialog_small_text");
    elem.inner_rml = smallText;
    elem = nd:GetElementById("dialog_yes");
    local args = pack2(...);
    elem:AddEventListener("click", function(event, target, doc)
        fn(nd, unpack2(args));
    end);
    nd:Show(DocumentFocus.MODAL);
end

function UIFadeOut(fn, ...)
    local c = StainedGlassComponent(100);
    c:setColor({0, 0, 0, 0}, const.EaseLinear, 0);
    c:setColor({0, 0, 0, 1.0}, const.EaseOutQuad, 0.5);

    local obj = SceneObject();
    obj:addComponent(c);
    scene:addObject(obj);

    UIAddTimeoutOnce(100, 0.5, function(args)
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function UIFadeIn()
    local c = StainedGlassComponent(99);
    c:setColor({0, 0, 0, 1.0}, const.EaseLinear, 0);
    c:setColor({0, 0, 0, 0}, const.EaseInQuad, 1.5);

    local obj = SceneObject();
    obj:addComponent(c);
    scene:addObject(obj);

    UIAddTimeoutOnce(99, 1.5, function()
        obj:removeFromParent();
    end);
end

function UIStainedGlass(zorder, fromColor, toColor, easing, timeout, fn, ...)
    local c = StainedGlassComponent(zorder);
    c:setColor(fromColor, const.EaseLinear, 0);
    c:setColor(toColor, easing, timeout);

    local obj = SceneObject();
    obj:addComponent(c);
    scene:addObject(obj);

    UIAddTimeoutOnce(zorder, timeout, function(args)
        if fn ~= nil then
            fn(obj, unpack2(args));
        end
    end, pack2(...));
end
