local function makeConveyor(name, velocity)
    local conveyor = scene:getObjects(name)[1];
    conveyor.deadbodyAware = true;

    local rc = conveyor:findRenderStripeComponents("belt")[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimation(const.AnimationDefault, "belt1", 12.5 / velocity);
    ac:startAnimation(const.AnimationDefault);
    conveyor:addComponent(ac);

    conveyor:addComponent(CollisionSensorComponent());
    conveyor.objs = {};
    setSensorListener(name, function(other, args)
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
            local obj = conveyor.objs[other.cookie].obj;
            conveyor.objs[other.cookie] = nil;
            obj.activeDeadbody = false;
        end
    end);
    addTimeout0(function(cookie, dt)
        local dir = conveyor:getDirection(velocity * dt);
        for _, v in pairs(conveyor.objs) do
            v.obj:changePosSmoothed(dir.x, dir.y);
        end
    end);
end

local function setupAnim(name, anim, factor)
    local rc = scene:getObjects("terrain0")[1]:findRenderQuadComponents(name)[1];
    local ac = AnimationComponent(rc.drawable);
    ac:addAnimationForceLoop(const.AnimationDefault, anim, factor);
    ac:startAnimation(const.AnimationDefault);
    scene:getObjects("terrain0")[1]:addComponent(ac);
end

local data = {
    [1] = {
        cp = {"e1m2", 1},
        [2] = {"player_legs_walk", 1}
    },
    [2] = {
        cp = {"e1m1", 0},
        [2] = {"military_legs_walk", 1}
    },
    [3] = {
        cp = {"e1m1", 1},
        [2] = {"sarge_legs_walk", 1}
    },
    [4] = {
        cp = {"e1m2", 2},
        [1] = {"tetrocrab1_def", 5/6}
    },
    [5] = {
        cp = {"e1m2", 8},
        [1] = {"tetrocrab2_def", 5/6}
    },
    [6] = {
        cp = {"e1m2", 4},
        [1] = {"tetrobot2_def", 1}
    },
    [7] = {
        cp = {"e1m2", 3}
    },
    [8] = {
        cp = {"e1m2", 0}
    },
    [9] = {
        cp = {"e1m2", 8},
        [1] = {"scorp_walk", 5/6},
        [2] = {"scorp_attack", 1}
    },
    [10] = {
        cp = {"e1m2", 8},
        [1] = {"scorp2_walk", 5/6},
        [2] = {"scorp2_attack", 1}
    },
    [11] = {
        cp = {"e1m2", 11}
    },
    [12] = {
        cp = {"e1m2", 11}
    },
    [13] = {
        cp = {"e1m3", 6}
    },
    [14] = {
        cp = {"e1m3", 7},
        [1] = {"spider_def", 1},
        [2] = {"spider_die", 1},
    },
    [15] = {
        cp = {"e1m3", 5},
    },
    [16] = {
        cp = {"e1m3", 7},
        [1] = {"spidernest_def", 1},
        [2] = {"spidernest_die", 1},
    },
    [17] = {
        cp = {"e1m3", 15},
    },
    [18] = {
        cp = {"e1m4", 6},
        [1] = {"enforcer_torso_def", 1},
        [2] = {"enforcer_torso_mask_def", 1},
        [3] = {"enforcer_torso_die", 1},
        [4] = {"enforcer_torso_mask_die", 1},
        [5] = {"enforcer_legs_def", 1},
    },
    [19] = {
        cp = {"e1m4", 11},
        [1] = {"sentry1_torso_unfold", 1},
        [2] = {"sentry1_torso_def", 1},
        [3] = {"sentry1_legs_unfold", 1},
        [4] = {"sentry1_legs_def", 1},
        [5] = {"sentry1_legs_walk", 1},
    },
    [20] = {
        cp = {"e1m4", 12},
        [1] = {"sentry2_torso_unfold", 1},
        [2] = {"sentry2_torso_def", 1},
        [3] = {"sentry2_legs_unfold", 1},
        [4] = {"sentry2_legs_def", 1},
        [5] = {"sentry2_legs_walk", 1},
    },
    [21] = {
        cp = {"e1m4", 18},
        [1] = {"gorger_def", 1},
        [2] = {"gorger_walk", 1},
        [3] = {"gorger_angry", 1},
        [4] = {"gorger_melee", 1},
        [5] = {"gorger_preshoot", 1},
        [6] = {"gorger_shoot", 1},
        [7] = {"gorger_postshoot", 1},
    },
    [22] = {
        cp = {"e1m5", 14},
        [2] = {"kyle_legs_walk", 1}
    },
    [23] = {
        cp = {"e1m5", 19},
        [1] = {"keeper_walk", 1},
        [2] = {"keeper_angry", 1},
        [3] = {"keeper_melee", 1},
        [4] = {"keeper_gun", 1},
        [5] = {"keeper_preplasma", 1},
        [6] = {"keeper_plasma", 1},
        [7] = {"keeper_postplasma", 1},
        [8] = {"keeper_missile", 1},
        [9] = {"keeper_crawlout", 1},
        [10] = {"keeper_death", 1},
    },
    [24] = {
        cp = {"e1m6", 4},
        [1] = {"warder_def", 1},
        [2] = {"warder_walk", 1},
        [3] = {"warder_shoot", 1},
        [4] = {"warder_melee1", 1},
        [5] = {"warder_melee2", 1},
        [6] = {"warder_melee3", 1},
    },
    [25] = {
        cp = {"e1m6", 5},
        [1] = {"orbo_def", 1},
        [2] = {"orbo_extend", 1},
        [3] = {"orbo_retract", 1},
    },
    [26] = {
        cp = {"e1m7", 6},
        [1] = {"beetle_def", 10/13},
        [2] = {"beetle_die", 1},
    },
    [27] = {
        cp = {"e1m7", 17},
        [4] = {"centipede_lowerleg1", 1},
        [5] = {"centipede_upperleg1", 1},
    },
    [28] = {
        cp = {"e1m7", 10},
    },
    [29] = {
        cp = {"e1m8", 3},
        [1] = {"homer_def", 1},
        [2] = {"homer_pregun", 1},
        [3] = {"homer_gun", 1},
        [4] = {"homer_postgun", 1},
        [5] = {"homer_premissile", 1},
        [6] = {"homer_missile", 1},
        [7] = {"homer_postmissile", 1},
        [8] = {"homer_melee", 1},
    },
    [30] = {
        cp = {"e1m9", 2},
        [1] = {"mech_torso_def", 1},
        [2] = {"mech_torso_pregun", 1},
        [3] = {"mech_torso_gun", 1},
        [4] = {"mech_torso_postgun", 1},
        [5] = {"mech_torso_melee2", 1},
        [6] = {"mech_legs_def", 1},
    },
    [31] = {
        cp = {"e1m10", 4},
        [1] = {"natan_def", 1},
        [2] = {"natan_melee", 1},
        [3] = {"natan_preshoot", 1},
        [4] = {"natan_postshoot", 1},
        [5] = {"natan_syringe", 1},
        [6] = {"natan_ram", 1},
        [7] = {"natan_die", 1},
        [8] = {"natan_dead", 1},
    },
    [32] = {
        cp = {"e1m10", 6},
        [2] = {"scientist_legs_walk", 1}
    },
    [33] = {
        cp = {"e1m1", 1},
    },
    [34] = {
        cp = {"e1m4", 18},
    },
    [38] = {
        cp = {"e1m2", 19},
    },
    [39] = {
        cp = {"e1m3", 18},
    },
    [40] = {
        cp = {"e1m3", 1},
    },
    [41] = {
        cp = {"e1m5", 13},
    },
    [42] = {
        cp = {"e1m6", 0},
    },
    [45] = {
        cp = {"e1m2", 11},
    },
    [46] = {
        cp = {"e1m4", 2},
    },
    [48] = {
        cp = {"e1m5", 19},
    },
    [49] = {
        cp = {"e1m8", 3},
    },
    [50] = {
        cp = {"e1m5", 18},
    },
    [51] = {
        cp = {"e1m3", 0},
    },
    [52] = {
        cp = {"e1m3", 1},
    },
    [53] = {
        cp = {"e1m7", 11},
    },
    [54] = {
        cp = {"e1m5", 1},
    },
    [55] = {
        cp = {"e1m2", 2},
    },
    [56] = {
        cp = {"main_menu", 2},
    },
    [57] = {
        cp = {"e1m2", 1},
    },
    [58] = {
        cp = {"e1m2", 8},
    },
    [59] = {
        cp = {"e1m4", 1},
    },
    [60] = {
        cp = {"e1m10", 1},
    },
};

-- main

scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeBlaster);
scene.player:findPlayerComponent():giveWeapon(const.WeaponTypeGG);

scene.camera:findCameraComponent():zoomTo(35, const.EaseLinear, 0);

scene.lighting.ambientLight = {0.5, 0.5, 0.5, 1.0};

scene.camera:findCameraComponent():setConstraint(scene:getObjects("constraint1")[1].pos, scene:getObjects("constraint2")[1].pos);

makeConveyor("conveyor0", 60.0);

for actor, actorData in pairs(data) do
    for img, anim in pairs(actorData) do
        if img == "cp" then
            setSensorEnterListener(actor.."_cp", true, function(other)
                scene.cutscene = true;
                settings:setDeveloper(anim[2]);
                stainedGlass({0, 0, 0, 0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 0.25, function()
                    stainedGlass({0, 0, 0, 1.0}, {0.0, 0.0, 0.0, 1.0}, const.EaseLinear, 100.0);
                    if anim[1] == "main_menu" then
                        scene:setNextLevel(anim[1]..".lua", "");
                    else
                        scene:setNextLevel(anim[1]..".lua", anim[1]..".json");
                    end
                end);
            end);
        else
            setupAnim(actor.."_"..img, anim[1], anim[2]);
        end
    end
end
