local music = audio:createStream("theme.ogg");
music.loop = true;
music.isMusic = true;
music:play();

local dummy = factory:createDummy();

dummy.pos = vec2(0, 0);
dummy.linearVelocity = vec2(5, 0);

scene:addObject(dummy);

local cameraC = scene.camera:findCameraComponent();

cameraC:setTarget(dummy);

local sky = factory:createBackground("metal12.png", 20.48, 20.48, vec2(1, 1.0), const.zOrderBackground)
sky:findRenderBackgroundComponent().offset = vec2(0, scene.gameHeight / 2);
scene:addObject(sky);

local clouds1 = factory:createBackground("fog.png", 544 / 6, 416 / 6, vec2(2, 1.0), const.zOrderBackground + 1)
clouds1:findRenderBackgroundComponent().offset = vec2(0, scene.gameHeight / 2);
scene:addObject(clouds1);

local clouds2 = factory:createBackground("fog.png", 544 / 5, 416 / 5, vec2(4, 0.2), const.zOrderBackground + 2)
clouds2:findRenderBackgroundComponent().offset = vec2(0, scene.gameHeight / 2);
scene:addObject(clouds2);

rocket.contexts["menu"]:LoadDocument("ui/main_menu.rml"):Show();
