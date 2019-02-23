local sky = factory:createBackground("dialog_bg.png", 20.48, 20.48, vec2(1, 1), const.zOrderBackground)
sky:findRenderBackgroundComponent().color = {0.0, 0.0, 0.0, 1.0};
scene:addObject(sky);

rocket.contexts["menu"]:LoadDocument("ui/poster.rml"):Show();

addTimeoutOnce(0.0, function()
    rocket.contexts["menu"]:ShowMouseCursor(false);
end);
