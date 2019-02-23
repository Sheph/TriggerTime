local music = nil;
local ambientMusic = "";
local isAmbient = false;
local doors = {};
local bridges = {};
local plugs = {};
bridge0 = scene:getObjects("bridge0")[1];
bridge1 = scene:getObjects("bridge1")[1];
bridge2 = scene:getObjects("bridge2")[1];

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
    isAmbient = false;
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

function setAmbientMusic(name)
    if ambientMusic ~= name then
        ambientMusic = name;
        isAmbient = false;
    end
end

function startAmbientMusic(crossfade)
    if not isAmbient then
        startMusic(ambientMusic, crossfade);
        isAmbient = true;
    end
end

function makeWater(obj, name)
    local rc = scene:getObjects(obj)[1]:findRenderTerrainComponents(name)[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "water1", 1);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects(obj)[1]:addComponent(ac);
end

function makeBridge(bridge, len)
    bridge.myStartPos = bridge.pos;
    bridge.myEndPos = bridge.pos + bridge:getDirection(len);
    bridge.myLenSq = len * len;

    local conveyor = scene:getObjects(bridge.name.."_conveyor")[1];
    conveyor.deadbodyAware = true;
    conveyor:addComponent(CollisionSensorComponent());
    conveyor.objs = {};
    setSensorListener(bridge.name.."_conveyor", function(other)
        if conveyor.objs[other.cookie] == nil then
            other.activeDeadbody = true;
            conveyor.objs[other.cookie] = { count = 1, obj = other };
        else
            conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count + 1;
        end
    end, function (other, args)
        if conveyor.objs[other.cookie] == nil then
            return;
        end
        conveyor.objs[other.cookie].count = conveyor.objs[other.cookie].count - 1;
        if conveyor.objs[other.cookie].count == 0 then
            conveyor.objs[other.cookie] = nil;
            other.activeDeadbody = false;
        end
    end);

    bridge.conveyor = conveyor;
    bridge.blocker = scene:getObjects(bridge.name.."_blocker")[1];
end

function bridgeOpen(bridge, velocity, quiet, fn, ...)
    if bridges[bridge.name] ~= nil then
        if not quiet then
            bridges[bridge.name].s:stop();
        end
        cancelTimeout(bridges[bridge.name].timer);
    end
    local s = nil;
    if not quiet then
        s = audio:createSound("cart_move.ogg");
        s.loop = true;
        s:play();
    end
    bridge.linearVelocity = bridge:getDirection(velocity);
    bridges[bridge.name] = {};
    bridges[bridge.name].s = s;
    bridges[bridge.name].timer = addTimeout0(function(cookie, dt, args)
        local dir = bridge.conveyor:getDirection(velocity * dt);
        for _, v in pairs(bridge.conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
        if (bridge.pos - bridge.myStartPos):lenSq() < bridge.myLenSq then
            return;
        end
        bridges[bridge.name] = nil;
        if not quiet then
            s:stop();
            audio:playSound("cart_stop.ogg");
        end
        cancelTimeout(cookie);
        bridge.linearVelocity = vec2(0, 0);
        bridge.active = false;
        bridge.blocker.active = false;
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function bridgeClose(bridge, velocity, quiet, fn, ...)
    if bridges[bridge.name] ~= nil then
        if not quiet then
            bridges[bridge.name].s:stop();
        end
        cancelTimeout(bridges[bridge.name].timer);
    end
    local s = nil;
    if not quiet then
        s = audio:createSound("cart_move.ogg");
        s.loop = true;
        s:play();
    end
    bridge.active = true;
    bridge.blocker.active = true;
    bridge.linearVelocity = bridge:getDirection(-velocity);
    bridges[bridge.name] = {};
    bridges[bridge.name].s = s;
    bridges[bridge.name].timer = addTimeout0(function(cookie, dt, args)
        local dir = bridge.conveyor:getDirection(-velocity * dt);
        for _, v in pairs(bridge.conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
        if (bridge.pos - bridge.myEndPos):lenSq() < bridge.myLenSq then
            return;
        end
        bridges[bridge.name] = nil;
        if not quiet then
            s:stop();
            audio:playSound("cart_stop.ogg");
        end
        cancelTimeout(cookie);
        bridge.linearVelocity = vec2(0, 0);
        if fn ~= nil then
            fn(unpack2(args));
        end
    end, pack2(...));
end

function openDoor(name, haveSound)
    local snd = nil;
    if doors[name] ~= nil then
        snd = doors[name].snd;
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door2_part_joint");
    if j:getJointTranslation() >= j.upperLimit then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    if (snd == nil) and haveSound then
        doors[name].snd = audio:createSound("servo_move.ogg");
        doors[name].snd.loop = true;
        doors[name].snd:play();
    else
        doors[name].snd = snd;
    end
    j.motorSpeed = math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() >= j.upperLimit) then
            cancelTimeout(cookie);
            if haveSound then
                doors[name].snd:stop();
            end
            doors[name] = nil;
        end
    end);
end

function closeDoor(name, haveSound)
    local snd = nil;
    if doors[name] ~= nil then
        snd = doors[name].snd;
        cancelTimeout(doors[name].cookie);
        doors[name] = nil;
    end
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door2_part_joint");
    if j:getJointTranslation() <= j.lowerLimit then
        if snd ~= nil then
            snd:stop();
        end
        j.motorSpeed = -math.abs(j.motorSpeed);
        return;
    end
    doors[name] = {};
    if (snd == nil) and haveSound then
        doors[name].snd = audio:createSound("servo_move.ogg");
        doors[name].snd.loop = true;
        doors[name].snd:play();
    else
        doors[name].snd = snd;
    end
    j.motorSpeed = -math.abs(j.motorSpeed);
    doors[name].cookie = addTimeout0(function(cookie, dt)
        if (j:getJointTranslation() <= j.lowerLimit) then
            cancelTimeout(cookie);
            if haveSound then
                doors[name].snd:stop();
            end
            doors[name] = nil;
        end
    end);
end

function doorOpened(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "door2_part_joint");
    return j:getJointTranslation() >= j.upperLimit;
end

function makeDoor(name, opened)
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

function setSensorEnterListenerWithAlly(name, func, ...)
    setSensorListener(name, function(other, self)
        if other.type == const.SceneObjectTypePlayer then
            self.timer = addTimeout(0.1, function(cookie)
                for _, obj in pairs(scene.combatAllies) do
                    if self.allies[obj.cookie] == nil then
                        return;
                    end
                end
                cancelTimeout(cookie);
                self.timer = nil;
                scene:getObjects(name)[1].active = false;
                func(unpack2(self.args));
            end);
        else
            self.allies[other.cookie] = 1;
        end
    end, function (other, self)
        if other.type == const.SceneObjectTypePlayer then
            if self.timer ~= nil then
                cancelTimeout(self.timer);
                self.timer = nil;
            end
        else
            self.allies[other.cookie] = nil;
        end
    end, { timer = nil, args = pack2(...), allies = {} });
end

function makeWaterPipe(name)
    local rc = scene:getObjects(name)[1]:findRenderStripeComponents("water")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "water2", 1);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects(name)[1]:addComponent(ac);
end

function makePlug(plug, plugFn, unplugFn, ...)
    plug.mySocket = nil;
    plug.myPlugFn = plugFn;
    plug.myUnplugFn = unplugFn;
    plug.myArgs = pack2(...);
    plugs[plug.cookie] = plug;
end

function makeSocket(socket)
    socket.myPlug = nil;
    socket:addComponent(CollisionSensorComponent());
    setSensorEnterListener(socket.name, false, function(other)
        if socket.myPlug ~= nil then
            return;
        end
        if plugs[other.cookie] == nil then
            return;
        end
        other = plugs[other.cookie];
        local aw = scene.player:findPlayerComponent().altWeapon;
        if aw.heldObject == other then
            aw:cancel();
        end
        other.mySocket = socket;
        other:setTransform(socket:getTransform());
        other:findRenderQuadComponents("nopins")[1].visible = true;
        other:findRenderQuadComponents("pins")[1].visible = false;
        socket.myJ = scene:addWeldJoint(other, socket, socket:getWorldPoint(vec2(0, 0)), true);
        socket.myPlug = other;
        if other.myPlugFn ~= nil then
            other.myPlugFn(socket, unpack2(other.myArgs));
        end
    end);
end

function disableSocket(socket)
    socket:findLightComponent():getLights("light0")[1].visible = false;
    setSensorEnterListener(socket.name, false, function(other) end);
    if socket.myPlug ~= nil then
        socket.myPlug.gravityGunAware = false;
        socket.myPlug:findLightComponent():getLights("light0")[1].visible = false;
    end
end

addTimeout(0.1, function()
    local obj = scene.player:findPlayerComponent().altWeapon.heldObject;
    if (obj == nil) or (plugs[obj.cookie] == nil) then
        return;
    end
    obj = plugs[obj.cookie];
    local socket = obj.mySocket;
    if socket == nil then
        return;
    end
    socket.myJ:remove();
    socket.myJ = nil;
    socket.myPlug = nil;
    obj.mySocket = nil;
    obj:findRenderQuadComponents("nopins")[1].visible = false;
    obj:findRenderQuadComponents("pins")[1].visible = true;
    if obj.myUnplugFn ~= nil then
        obj.myUnplugFn(socket, unpack2(obj.myArgs));
    end
end);

function openHatch(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = math.abs(j.motorSpeed);
end

function closeHatch(name)
    local inst = scene:getInstances(name)[1];
    local j = findJoint(inst.joints, "hatch1_joint");
    j.motorSpeed = -math.abs(j.motorSpeed);
end

function makeKeyDoor(doorName, phName, fn, ...)
    local doorObj = scene:getObjects(doorName)[1];
    local phObj = scene:getObjects(phName)[1];
    local phC = phObj:findPlaceholderComponent();

    phC.listener = createSensorEnterListener(false, function(other, fn, args)
        if not scene.player:findPlayerComponent().inventory:have(phC.item) then
            return;
        end
        scene.player:findPlayerComponent().inventory:take(phC.item);
        phObj:removeFromParent();
        doorObj:findRenderQuadComponents("key")[1].visible = true;
        audio:playSound("key.ogg");
        fn(unpack2(args));
    end, fn, pack2(...));
end

-- main

if settings.developer == 0 then
    scene.player:findPlayerComponent():changeLifeNoDamage(-80.0 * (scene.player.maxLife / 200.0));
end
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeMachineGun);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeRLauncher);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeProxMine);
scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 50.0);
scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeRLauncher, 3.0);
scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeProxMine, 3.0);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.2, 0.2, 0.2, 1.0};

math.randomseed(os.time());

setAmbientMusic("ambient9.ogg");
startAmbientMusic(false);

makeBridge(bridge0, 65.0);
makeBridge(bridge1, 63.0);
makeBridge(bridge2, 64.0);

require("e1m6_part0");
require("e1m6_part1");
require("e1m6_part2");
require("e1m6_part3");
require("e1m6_part4");
require("e1m6_part5");
require("e1m6_part6");
require("e1m6_part7");
require("e1m6_part8");

if settings.developer >= 1 then
    scene.respawnPoint = scene:getObjects("player_"..settings.developer)[1]:getTransform();
    scene.player:setTransform(scene.respawnPoint);
end

if settings.developer >= 2 and settings.developer <= 3 then
    part0Done(true);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 450.0);
end

if settings.developer == 3 then
    scene:getObjects("sentry1")[1]:setPosRecursive(scene:getObjects("sentry1_1")[1].pos);
end

if settings.developer >= 4 then
    scene:getObjects("bridge1_cp")[1].active = false;
    scene:getObjects("core1_cp")[1].active = false;
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypeMachineGun, 200.0);
end

if settings.developer >= 5 then
    scene:getObjects("fire1_cp")[1].active = false;
end

if settings.developer == 6 then
    startMusic("action10.ogg", false);
end

if settings.developer >= 7 then
    scene.player:findPlayerComponent():giveWeapon(const.WeaponTypePlasmaGun);
    scene.player:findPlayerComponent():changeAmmo(const.WeaponTypePlasmaGun, 250.0);
    openDoor("door8", false);
    openDoor("door4", false);
    scene:getObjects("core2_cp")[1].active = true;
    scene:getObjects("east3_cp")[1].active = true;
end

if settings.developer >= 9 then
    scene:getObjects("bridge2_cp")[1].active = false;
    scene:getObjects("core2_cp")[1].active = false;
    scene:getObjects("east3_cp")[1].active = false;
end

if settings.developer == 10 then
    scene:getObjects("keeper1_cp")[1].active = true;
end

if settings.developer == 11 then
    scene.player:findPlayerComponent().inventory:give(const.InventoryItemRedKey);
end

if settings.developer >= 12 then
    openDoor("door15", false);
end

if settings.developer == 15 then
    local shields = scene:getInstances("shield1");
    for _, inst in pairs(shields) do
        findJoint(inst.joints, "shield1_joint").motorSpeed = -findJoint(inst.joints, "shield1_joint").motorSpeed;
    end
    scene:getObjects("boss2_cp")[1].active = true;
end

if settings.developer == 16 then
    scene:getObjects("toxic1_cp")[1].active = true;
end
