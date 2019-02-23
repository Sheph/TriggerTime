local cannon = scene:getInstances("cannon");

local function activateLight(light, quiet)
    light.myColor = light.color[4];
    light.myTimer = addTimeoutOnce(0.5, function()
        if not quiet then
            audio:playSound("crystal_activate.ogg");
        end
        local tweening = SequentialTweening(true);
        tweening:addTweening(SingleTweening(0.6, const.EaseOutQuad, light.color[4], 1.0, false));
        tweening:addTweening(SingleTweening(0.6, const.EaseInQuad, 1.0, light.color[4], false));
        light.myTimer = addTimeout0(function(cookie, dt, self)
            local c = light.color;
            c[4] = tweening:getValue(self.t);
            light.color = c;
            self.t = self.t + dt;
        end, { t = 0 });
    end);
end

local function deactivateLight(light)
    if light.myTimer ~= nil then
        cancelTimeout(light.myTimer);
        light.myTimer = nil;
        local c = light.color;
        c[4] = light.myColor;
        light.color = c;
    end
end

local function makeGasPipe()
    local rc = scene:getObjects("terrain0")[1]:findRenderStripeComponents("gas1")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "gas1", 1);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects("terrain0")[1]:addComponent(ac);
end

local function powerupNatan(initial)
    local objs = {};
    local numAlive = 0;
    for _, inst in pairs(cannon) do
        if not inst.myDead then
            numAlive = numAlive + 1;
        end
    end;
    local myDuration = (boss2.maxLife - boss2.life) / (50.0 * numAlive);
    for _, inst in pairs(cannon) do
        if not inst.myDead then
            inst.myCp2.active = false;
            inst.myCp3.active = false;
            if inst.myCp2.cur ~= nil then
                inst.myCp2.cur.linearVelocity = vec2(0, 0);
                inst.myCp2.cur.angularVelocity = 0;
                inst.myCp2.cur.bodyType = const.BodyKinematic;
                inst.myCp2.cur.gravityGunAware = false;
            end
            if inst.myCp3.cur ~= nil then
                inst.myCp3.cur.linearVelocity = vec2(0, 0);
                inst.myCp3.cur.angularVelocity = 0;
                inst.myCp3.cur.bodyType = const.BodyKinematic;
                inst.myCp3.cur.gravityGunAware = false;
            end
            local obj = factory:createPowerBeam(4.0, 24.0);
            obj:setTransform(findObject(inst.objects, "beam"):getTransform());
            scene:addObject(obj);
            table.insert(objs, obj);
            findObject(inst.objects, "stone_powerup_cp").active = true;
            inst.myDuration = myDuration;
            if (inst.myCp2.cur ~= nil) and (inst.myCp3.cur ~= nil) then
                local tout = 0;
                if myDuration > 4 then
                    tout = myDuration - 4;
                end
                addTimeoutOnce(tout, function()
                    audio:playSound("gen_overload.ogg");
                end);
            end
        end
    end;
    if not initial then
        addTimeoutOnce(1.0, function()
            boss2:findBossNatanComponent():startNapalm();
        end);
    end
    local heatTime = 0;
    local snd = audio:createSound("lfire.ogg");
    snd.loop = true;
    snd:play();
    boss2:findBossNatanComponent():startPowerupAnimation();
    addTimeout0(function(cookie, dt)
        for _, obj in pairs(objs) do
            boss2:changeLife(50.0 * dt);
        end
        heatTime = heatTime + dt;
        for _, inst in pairs(cannon) do
            local a = heatTime / myDuration;
            if a > 1 then
                a = 1;
            end
            if (inst.myCp2.cur ~= nil) and (inst.myCp3.cur ~= nil) then
                local rcs = inst.myCannon:findRenderTerrainComponents();
                rcs = table.copy(rcs, inst.myCannon:findRenderStripeComponents());
                for _ , rc in pairs(rcs) do
                    rc.color = {
                        1.0 * (1 - a) + 1.0 * a,
                        1.0 * (1 - a) + 0.0 * a,
                        1.0 * (1 - a) + 0.0 * a,
                        1.0
                    };
                end
            elseif (inst.myCp2.cur ~= nil) or (inst.myCp3.cur ~= nil) then
                local rcs = inst.myCannon:findRenderTerrainComponents();
                rcs = table.copy(rcs, inst.myCannon:findRenderStripeComponents());
                for _ , rc in pairs(rcs) do
                    rc.color = {
                        1.0 * (1 - a) + 1.0 * a,
                        1.0 * (1 - a) + 0.6 * a,
                        1.0 * (1 - a) + 0.6 * a,
                        1.0
                    };
                end
            end
        end
        if boss2:lifePercent() >= 1 then
            cancelTimeout(cookie);
            for _, inst in pairs(cannon) do
                if not inst.myDead then
                    if (inst.myCp2.cur ~= nil) or (inst.myCp3.cur ~= nil) then
                        local rcs = inst.myCannon:findRenderTerrainComponents();
                        rcs = table.copy(rcs, inst.myCannon:findRenderStripeComponents());
                        local sc = rcs[1].color;
                        local heatTime = 0;
                        addTimeout0(function(cookie, dt)
                            heatTime = heatTime + dt;
                            local a = heatTime / 1.0;
                            if a > 1 then
                                a = 1;
                            end
                            for _ , rc in pairs(rcs) do
                                rc.color = {
                                    sc[1] * (1 - a) + 1.0 * a,
                                    sc[2] * (1 - a) + 1.0 * a,
                                    sc[3] * (1 - a) + 1.0 * a,
                                    1.0
                                };
                            end
                            if a == 1 then
                                cancelTimeout(cookie);
                            end
                        end);
                    end
                    if (inst.myCp2.cur ~= nil) and (inst.myCp3.cur ~= nil) then
                        inst.myDead = true;
                        for i = 1, #inst.myFire, 1 do
                            addTimeoutOnce(0.4 * (i - 1), function()
                                local obj = factory:createExplosion1(106);
                                obj.pos = inst.myFire[i].pos;
                                scene:addObject(obj);
                                inst.myFire[i].visible = true;
                            end);
                        end
                    end
                    findObject(inst.objects, "stone_powerup_cp").active = false;
                    if inst.myCp2.cur ~= nil then
                        inst.myCp2.cur:findLightComponent():getLights()[1].visible = false;
                        inst.myCp2.cur:findRenderQuadComponents()[1].color = {1.0, 1.0, 1.0, 1.0};
                        inst.myCp2.cur.bodyType = const.BodyDynamic;
                        inst.myCp2.cur.gravityGunAware = true;
                        inst.myLight2b.visible = false;
                        deactivateLight(inst.myLight2);
                        inst.myCp2.cur = nil;
                    end
                    if inst.myCp3.cur ~= nil then
                        inst.myCp3.cur:findLightComponent():getLights()[1].visible = false;
                        inst.myCp3.cur:findRenderQuadComponents()[1].color = {1.0, 1.0, 1.0, 1.0};
                        inst.myCp3.cur.bodyType = const.BodyDynamic;
                        inst.myCp3.cur.gravityGunAware = true;
                        inst.myLight3b.visible = false;
                        deactivateLight(inst.myLight3);
                        inst.myCp3.cur = nil;
                    end
                    if not inst.myDead then
                        inst.myCp2.active = true;
                        inst.myCp3.active = true;
                    else
                        inst.myLight1.visible = false;
                        inst.myLight2.visible = false;
                        inst.myLight3.visible = false;
                        inst.myFixedStone:findLightComponent():getLights()[1].visible = false;
                        inst.myFixedStone:findRenderQuadComponents()[1].color = {1.0, 1.0, 1.0, 1.0};
                    end
                end
            end
            for _, obj in pairs(objs) do
                obj:findPowerBeamComponent():finish();
            end
            audio:crossfade(snd, nil, 1.0, 1.0, 0.3);
            boss2:findBossNatanComponent():finishPowerupAnimation();
            if initial then
                addTimeoutOnce(2.0, function()
                    audio:playSound("natan_laugh.ogg");
                    scene.cutscene = false;
                    scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                    addTimeoutOnce(0.5, function()
                        boss2:findBossNatanComponent().autoTarget = true;
                    end);
                end);
            else
                boss2:findBossNatanComponent():finishNapalm();
                addTimeoutOnce(2.0, function()
                    boss2:findBossNatanComponent():finishPowerup();
                    local numAlive = 0;
                    for _, inst in pairs(cannon) do
                        if not inst.myDead then
                            numAlive = numAlive + 1;
                        end
                    end;
                    if numAlive > 0 then
                        scene:getObjects("powerup_cp")[1].active = true;
                    else
                        boss2:findBossNatanComponent().powerupObj = nil;
                    end
                end);
            end
        end
    end);
end

function startNatan()
    scene.lighting.ambientLight = {0.0, 0.0, 0.0, 1.0};
    scene.camera:findCameraComponent():setConstraint(scene:getObjects("target4")[1].pos - vec2(75, 75), scene:getObjects("target4")[1].pos + vec2(75, 75));
    scene.camera:findCameraComponent():zoomTo(45, const.EaseLinear, 0);
    local c = boss2:findBossNatanComponent();
    local objs = scene:getObjects("natan_teleport");
    local dests = {};
    for _, obj in pairs(objs) do
        table.insert(dests, obj.pos);
    end
    if settings.developer ~= 5 then
        c.powerupObj = scene:getObjects("target4")[1];
    end
    c.teleportDests = dests;
    c:setupInitial();
    addTimeoutOnce(2.0, function()
        scene.camera:findCameraComponent():follow(boss2, const.EaseOutQuad, 1.0);
        addTimeoutOnce(2.0, function()
            showUpperDialog(
            {
                {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog132.str1},
            }, function ()
                addTimeoutOnce(1.0, function()
                    powerupNatan(true);
                end);
            end);
        end);
    end);
    addTimeout(0.25, function(cookie)
        if c.deathFinished then
            cancelTimeout(cookie);
            addTimeoutOnce(2.0, function()
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                addTimeoutOnce(1.0, function()
                    scene.camera:findCameraComponent():follow(boss2, const.EaseOutQuad, 1.0);
                    addTimeoutOnce(2.0, function()
                        showLowerDialog(
                        {
                            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog133.str1},
                            {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog133.str2},
                        }, function ()
                            scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                            scene.cutscene = false;
                            addTimeoutOnce(2.0, function()
                                stopMusic(true);
                                scene.camera:findCameraComponent():tremorStart(0.5);
                                local stream = audio:createStream("queen_shake.ogg");
                                stream.loop = true;
                                stream:play();
                                local teleStream = audio:createStream("teleport_ready.ogg");
                                teleStream:play();
                                stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 0.7}, const.EaseLinear, 4.0, function()
                                    scene.player.linearDamping = 6.0;
                                    scene.cutscene = true;
                                    teleStream:stop();
                                    audio:playSound("teleport_beam.ogg");
                                    stainedGlass({1.0, 1.0, 1.0, 1}, {1.0, 1.0, 1.0, 1}, const.EaseLinear, 1.0, function()
                                        scene.lighting.ambientLight = {0.3, 0.3, 0.3, 1.0};
                                        scene.camera:findCameraComponent():setConstraint(vec2(0, 0), vec2(0, 0));
                                        scene.camera:findCameraComponent():zoomTo(50, const.EaseLinear, 0);
                                        scene.player:setTransform(scene:getObjects("target6")[1]:getTransform());
                                        scene.respawnPoint = scene:getObjects("target6")[1]:getTransform();
                                        stainedGlass({1.0, 1.0, 1.0, 1}, {0, 0, 0, 0}, const.EaseLinear, 1.0);
                                        startCarl(stream, false);
                                    end);
                                end);
                            end);
                        end);
                    end);
                end);
            end);
        end
    end);
end

function startChamber(immediate)
    if immediate then
        scene:getObjects("border0")[1]:removeFromParent();
        scene:getObjects("blocker_boss1")[1]:removeFromParent();
        scene:instanciate("e1m10_chamber.json", scene:getObjects("boss1_cp")[1]:getTransform());
        return;
    end
    scene.player.linearDamping = 6.0;
    scene.cutscene = true;
    addTimeoutOnce(1.0, function()
        scene.camera:findCameraComponent():follow(scene:getObjects("boss1_cp")[1], const.EaseOutQuad, 1.0);
        addTimeoutOnce(2.0, function()
            audio:playSound("natan_chamber.ogg");
            addTimeoutOnce(2.0, function()
                scene.camera:findCameraComponent():follow(scene.player, const.EaseOutQuad, 1.0);
                scene.cutscene = false;
            end);
            scene:getObjects("border0")[1]:addComponent(FadeOutComponent(3.0));
            local chamber = scene:instanciate("e1m10_chamber.json", scene:getObjects("boss1_cp")[1]:getTransform());
            for _, obj in pairs(chamber.objects) do
                obj:setColorRecursive({1.0, 1.0, 1.0, 0.0});
            end
            local tweening = SingleTweening(3.0, const.EaseLinear, 0.0, 1.0, false);
            addTimeout0(function(cookie, dt, self)
                local v = tweening:getValue(self.t);
                for _, obj in pairs(chamber.objects) do
                    obj:setColorRecursive({1.0, 1.0, 1.0, v});
                end
                if tweening:finished(self.t) then
                    cancelTimeout(cookie);
                    scene:getObjects("blocker_boss1")[1]:removeFromParent();
                    return;
                end
                self.t = self.t + dt;
            end, { t = 0 });
            findObject(chamber.objects, "chamber0_cp"):findCollisionSensorComponent().listener = createSensorEnterListener(true, function(other)
                scene.player.linearDamping = 6.0;
                scene.cutscene = true;
                addTimeoutOnce(1.0, function()
                    showLowerDialog(
                    {
                        {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog134.str1},
                        {"enemy", tr.str5, "factory4/portrait_natan.png", tr.dialog134.str2},
                    }, function ()
                        scene.cutscene = false;
                    end);
                end);
            end);
            findObject(chamber.objects, "chamber1_cp"):findCollisionSensorComponent().listener = createSensorEnterListener(true, function(other)
                local j = findJoint(chamber.joints, "teleport1_door_joint");
                j.motorSpeed = -j.motorSpeed;
                local stream = audio:createSound("servo_move.ogg");
                stream:play();
                addTimeoutOnce(0.5, function()
                    stream:stop();
                end);
                addTimeoutOnce(2.0, function()
                    local tc = findObject(chamber.objects, "teleport1"):findTeleportComponent();
                    tc:start(8.0);
                    addTimeout0(function(cookie, dt)
                        if tc.finished then
                            cancelTimeout(cookie);
                        end;
                    end);
                    addTimeoutOnce(12.0, function()
                        stainedGlass({0, 0, 0, 0}, {1.0, 1.0, 1.0, 0.7}, const.EaseLinear, 8.3, function()
                            scene.player.linearDamping = 6.0;
                            scene.cutscene = true;
                            stainedGlass({1.0, 1.0, 1.0, 1}, {1.0, 1.0, 1.0, 1}, const.EaseLinear, 1.0, function()
                                startMusic("action15.ogg", true);
                                scene.player:setTransform(scene:getObjects("target5")[1]:getTransform());
                                scene.respawnPoint = scene:getObjects("target5")[1]:getTransform();
                                j.motorSpeed = -j.motorSpeed;
                                startNatan();
                                stainedGlass({1.0, 1.0, 1.0, 1}, {0, 0, 0, 0}, const.EaseLinear, 1.0);
                            end);
                        end);
                    end);
                end);
            end);
        end);
    end);
end

-- main

makeGasPipe();

setSensorListener("boss3_cp", function(other, self)
    other:findCollisionDamageComponent().enabled = false;
end, function(other)
    other:findCollisionDamageComponent().enabled = true;
end);

for _, inst in pairs(cannon) do
    local cannonObj = findObject(inst.objects, "cannon");
    inst.myCannon = cannonObj;
    inst.myFixedStone = findObject(inst.objects, "fixed_stone");
    inst.myCp2 = findObject(inst.objects, "stone2_cp");
    inst.myCp3 = findObject(inst.objects, "stone3_cp");
    inst.myLight1 = cannonObj:findLightComponent():getLights("stone1_light1")[1];
    inst.myLight2 = cannonObj:findLightComponent():getLights("stone2_light1")[1];
    inst.myLight3 = cannonObj:findLightComponent():getLights("stone3_light1")[1];
    inst.myLight2b = cannonObj:findLightComponent():getLights("stone2_light2")[1];
    inst.myLight3b = cannonObj:findLightComponent():getLights("stone3_light2")[1];
    inst.myFire = findObjects(inst.objects, "fire1");
    for _, obj in pairs(inst.myFire) do
        obj.angle = 0;
    end
    activateLight(inst.myLight1, true);
    findObject(inst.objects, "stone_powerup_cp"):findCollisionSensorComponent().listener = createSensorListener(function(other, self)
        if (other.name ~= "stone1") then
            return;
        end
        if other:findLightComponent():getLights()[1].visible then
            return;
        end
        self.objs[other.cookie] = {other, 0};
        if table.size(self.objs) == 1 then
            self.timer = addTimeout0(function(cookie, dt)
                for cookie, obj in pairs(self.objs) do
                    obj[2] = obj[2] + dt;
                    if obj[2] >= inst.myDuration * 0.8 then
                        obj[1]:findLightComponent():getLights()[1].visible = true;
                        obj[1]:findRenderQuadComponents()[1].color = {0.0, 0.0, 1.0, 1.0};
                        obj[1].active = false;
                        obj[1].active = true;
                    end
                end
            end);
        end
    end, function(other, self)
        if self.objs[other.cookie] == nil then
            return;
        end
        self.objs[other.cookie] = nil;
        if table.size(self.objs) == 0 then
            cancelTimeout(self.timer);
        end
    end, { objs = {}, timer = nil });
    inst.myCp2:findCollisionSensorComponent().listener = createSensorListener(function(other)
        if (other.name ~= "stone1") or (inst.myCp2.cur ~= nil) or (not other:findLightComponent():getLights()[1].visible) then
            return;
        end
        inst.myCp2.cur = other;
        inst.myLight2b.visible = true;
        activateLight(inst.myLight2);
    end, function(other)
        if (inst.myCp2.cur ~= nil) and (inst.myCp2.cur.cookie == other.cookie) then
            if inst.myCp2.active then
                inst.myCp2.cur = nil;
                inst.myLight2b.visible = false;
                deactivateLight(inst.myLight2);
            end
        end
    end);
    inst.myCp3:findCollisionSensorComponent().listener = createSensorListener(function(other)
        if (other.name ~= "stone1") or (inst.myCp3.cur ~= nil) or (not other:findLightComponent():getLights()[1].visible) then
            return;
        end
        inst.myCp3.cur = other;
        inst.myLight3b.visible = true;
        activateLight(inst.myLight3);
    end, function(other)
        if (inst.myCp3.active) and (inst.myCp3.cur ~= nil) and (inst.myCp3.cur.cookie == other.cookie) then
            if inst.myCp3.active then
                inst.myCp3.cur = nil;
                inst.myLight3b.visible = false;
                deactivateLight(inst.myLight3);
            end
        end
    end);
end

setSensorListener("powerup_cp", function(other, self)
    self.timer = addTimeout0(function(cookie, dt)
        if boss2:findBossNatanComponent().needPowerup then
            cancelTimeout(cookie);
            self.timer = nil;
            scene:getObjects("powerup_cp")[1].active = false;
            addTimeoutOnce(1.0, function()
                powerupNatan(false);
            end);
        end
    end);
end, function(other, self)
    if self.timer ~= nil then
        cancelTimeout(self.timer);
        self.timer = nil;
    end
end, { timer = nil });
