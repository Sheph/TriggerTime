local music = audio:createStream("theme.ogg");
music.loop = true;
music.isMusic = true;
music:play();

rocket.contexts["menu"]:LoadDocument("ui/endofdemo.rml"):Show();
