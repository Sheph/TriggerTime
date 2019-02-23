local sky = factory:createBackground("dialog_bg.png", 20.48, 20.48, vec2(1, 1), const.zOrderBackground)
scene:addObject(sky);

rocket.contexts["menu"]:LoadDocument("ui/logo.rml"):Show();

addTimeoutOnce(0.0, function()
    rocket.contexts["menu"]:ShowMouseCursor(false);
end);
