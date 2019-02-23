tr = {}

do
    for i = 1, 200, 1 do
        tr["dialog"..i] = {}
    end
end

-- UI

tr.ui = {}

tr.ui.BACK = "Back"
tr.ui.APPLY = "Apply"
tr.ui.START = "Start"
tr.ui.OR = "or"
tr.ui.OFF = "Off"
tr.ui.NEXT = "Next"

tr.ui.MAIN_MENU_PLAY = "Play"
tr.ui.MAIN_MENU_OPTIONS = "Options"
tr.ui.MAIN_MENU_ACHIEVEMENTS = "Achievements"
tr.ui.MAIN_MENU_CREDITS = "Credits"
tr.ui.MAIN_MENU_QUIT = "Quit"

tr.ui.SKILL_TITLE = "Difficulty"
tr.ui.SKILL_EASY = "Easy"
tr.ui.SKILL_EASY_PROP1 = "- Player has 7 lives"
tr.ui.SKILL_EASY_PROP2 = "- Player health is increased"
tr.ui.SKILL_EASY_PROP3 = "- Enemies are weaker"
tr.ui.SKILL_EASY_PROP4 = "- More puzzle hints"
tr.ui.SKILL_NORMAL = "Normal"
tr.ui.SKILL_NORMAL_PROP1 = "- Player has 6 lives"
tr.ui.SKILL_NORMAL_PROP2 = "- Player health is moderate"
tr.ui.SKILL_NORMAL_PROP3 = "- Enemies are moderate"
tr.ui.SKILL_NORMAL_PROP4 = "- Some puzzle hints"
tr.ui.SKILL_HARD = "Hard"
tr.ui.SKILL_HARD_PROP1 = "- Player has 5 lives"
tr.ui.SKILL_HARD_PROP2 = "- Player health is decreased"
tr.ui.SKILL_HARD_PROP3 = "- Enemies are tougher"
tr.ui.SKILL_HARD_PROP4 = "- No puzzle hints"

tr.ui.PLAY_TITLE = "Missions"
tr.ui.PLAY_NOT_AVAILABLE_IN_DEMO = "Not available in demo"
tr.ui.PLAY_SECRET = "Secret"
tr.ui.PLAY_LOCKED = "Locked"

tr.ui.SHOP_TITLE = "Mission details"
tr.ui.SHOP_YOU_HAVE = "You have "
tr.ui.SHOP_ITEM = "Item"
tr.ui.SHOP_COST = "Cost"
tr.ui.SHOP_EQUIPPED = "Equipped"
tr.ui.SHOP_EXTRA_LIFE = "Extra Life"
tr.ui.SHOP_EXTRA_LIFE_ABOUT = "One extra life. The more you have the bigger your chances are."
tr.ui.SHOP_AMMO = "1.5x ammo"
tr.ui.SHOP_AMMO_ABOUT = "1.5 times more ammo in weapon stations, ammo boxes and crates."
tr.ui.SHOP_ARMOR = "Armor"
tr.ui.SHOP_ARMOR_ABOUT = "Take 1.5 less bullet and explosion splash damage."
tr.ui.SHOP_BLASTER = "Super blaster"
tr.ui.SHOP_BLASTER_ABOUT = "You start with super blaster instead of blaster."
tr.ui.SHOP_NOTICE = "* All purchases are one-time."

tr.ui.OPTIONS_TITLE = "Options"
tr.ui.OPTIONS_VIDEO = "Video"
tr.ui.OPTIONS_BRIGHTNESS = "Brightness"
tr.ui.OPTIONS_AUDIO = "Audio"
tr.ui.OPTIONS_CONTROLS = "Keyboard+Mouse"
tr.ui.OPTIONS_GAMEPAD = "Gamepad"

tr.ui.VIDEO_TITLE = "Video"
tr.ui.VIDEO_RESOLUTION = "Resolution:"
tr.ui.VIDEO_FULLSCREEN = "Fullscreen:"
tr.ui.VIDEO_AA = "Antialiasing:"
tr.ui.VIDEO_VSYNC = "Vertical sync:"
tr.ui.VIDEO_FILTER = "Texture filter:"
tr.ui.VIDEO_FILTER_BILINEAR = "Bilinear"
tr.ui.VIDEO_FILTER_TRILINEAR = "Trilinear (better)"

tr.ui.BRIGHTNESS_TITLE = "Brightness"
tr.ui.BRIGHTNESS_ABOUT = "Adjust the slider so that the inner square is barely visible"

tr.ui.AUDIO_TITLE = "Audio"
tr.ui.AUDIO_SOUND = "Sound volume:"
tr.ui.AUDIO_MUSIC = "Music volume:"

tr.ui.CONTROLS_TITLE = "Keyboard+Mouse"
tr.ui.CONTROLS_MOVE_UP = "Move up:"
tr.ui.CONTROLS_MOVE_DOWN = "Move down:"
tr.ui.CONTROLS_MOVE_LEFT = "Move left:"
tr.ui.CONTROLS_MOVE_RIGHT = "Move right:"
tr.ui.CONTROLS_PRIMARY_FIRE = "Primary fire:"
tr.ui.CONTROLS_SECONDARY_FIRE = "Secondary fire:"
tr.ui.CONTROLS_PRIMARY_SWITCH = "Primary weapon switch:"
tr.ui.CONTROLS_SECONDARY_SWITCH = "Secondary weapon switch:"
tr.ui.CONTROLS_INTERACT = "Interact:"
tr.ui.CONTROLS_RUN = "Run:"

tr.ui.GAMEPAD_TITLE = "Gamepad"
tr.ui.GAMEPAD_MOVE = "Move:"
tr.ui.GAMEPAD_LEFTSTICK = "Left stick"
tr.ui.GAMEPAD_AIM = "Aim:"
tr.ui.GAMEPAD_RIGHTSTICK = "Right stick"
tr.ui.GAMEPAD_PRIMARY_FIRE = "Primary fire:"
tr.ui.GAMEPAD_SECONDARY_FIRE = "Secondary fire:"
tr.ui.GAMEPAD_PRIMARY_SWITCH = "Primary weapon switch:"
tr.ui.GAMEPAD_SECONDARY_SWITCH = "Secondary weapon switch:"
tr.ui.GAMEPAD_INTERACT = "Interact:"
tr.ui.GAMEPAD_RUN = "Run:"
tr.ui.GAMEPAD_STICK_DEADZONE = "Stick deadzone:"
tr.ui.GAMEPAD_TRIGGER_DEADZONE = "Trigger deadzone:"

tr.ui.ACHIEVEMENTS_TITLE = "Achievements"

tr.ui.PAUSE_TITLE = "Pause"
tr.ui.PAUSE_RESUME = "Resume"
tr.ui.PAUSE_OPTIONS = "Options"
tr.ui.PAUSE_MAIN_MENU = "Main menu"

tr.ui.DIALOG_YES = "Yes"
tr.ui.DIALOG_NO = "No"

tr.ui.COMPLETED_TITLE = "Mission Completed!"
tr.ui.COMPLETED_MISSION = "Mission:"
tr.ui.COMPLETED_KILLED = "Enemies killed:"
tr.ui.COMPLETED_ACCURACY = "Accuracy:"
tr.ui.COMPLETED_TIME = "Time spent:"
tr.ui.COMPLETED_NEXT_MISSION = "Next mission:"
tr.ui.COMPLETED_MAIN_MENU = "Main menu"
tr.ui.COMPLETED_NEXT = "Next mission"

tr.ui.GAMEOVER_TITLE = "GAME OVER"
tr.ui.GAMEOVER_MAIN_MENU = "Main menu"
tr.ui.GAMEOVER_TRY_AGAIN = "Try again"

tr.ui.CREDITS_DESIGN = "GAME DESIGN AND PROGRAMMING"
tr.ui.CREDITS_DESIGN_1 = "Stanislav 'shephf' Vorobiov"
tr.ui.CREDITS_ART = "ART"
tr.ui.CREDITS_ART_1 = "Anatoly 'uksusss' Smirnov"
tr.ui.CREDITS_SOUND = "SOUND DESIGN"
tr.ui.CREDITS_SOUND_1 = "Artem Samoilenko"
tr.ui.CREDITS_MUSIC = "MUSIC"
tr.ui.CREDITS_MUSIC_1 = "John Fleming"
tr.ui.CREDITS_MUSIC_2 = "Igor Dvorkin"
tr.ui.CREDITS_MUSIC_3 = "Darren Leigh Purkiss"
tr.ui.CREDITS_MUSIC_4 = "Philip Guyler"
tr.ui.CREDITS_MUSIC_5 = "Richard Lacy"
tr.ui.CREDITS_MUSIC_6 = "Tom Quick"
tr.ui.CREDITS_MUSIC_7 = "Jeff Dale"
tr.ui.CREDITS_MUSIC_8 = "Barrie Gledden"
tr.ui.CREDITS_MUSIC_9 = "David Tobin"
tr.ui.CREDITS_MUSIC_10 = "Doug Boyle"
tr.ui.CREDITS_MUSIC_11 = "Terry Devine-King"
tr.ui.CREDITS_MUSIC_12 = "Adam Drake"
tr.ui.CREDITS_MUSIC_13 = "Jason Pedder"
tr.ui.CREDITS_MUSIC_14 = "Gareth Johnson"
tr.ui.CREDITS_MUSIC_15 = "Lunar Synth"
tr.ui.CREDITS_MUSIC_16 = "Kevin MacLeod"
tr.ui.CREDITS_MUSIC_17 = "alxdmusic"
tr.ui.CREDITS_MUSIC_18 = "ErikMMusic"
tr.ui.CREDITS_MUSIC_19 = "Dneproman"
tr.ui.CREDITS_RESOURCES_USED = "RESOURCES USED"
tr.ui.CREDITS_RESOURCES_USED_ART = "Art"
tr.ui.CREDITS_RESOURCES_USED_ART_1 = "http://www.dundjinni.com"
tr.ui.CREDITS_RESOURCES_USED_ART_2 = "http://opengameart.org"
tr.ui.CREDITS_RESOURCES_USED_MUSIC = "Music and sounds"
tr.ui.CREDITS_RESOURCES_USED_MUSIC_1 = "http://www.freesound.org"
tr.ui.CREDITS_ADDITIONAL_ART = "ADDITIONAL ART"
tr.ui.CREDITS_ADDITIONAL_ART_1 = "Stanislav 'shephf' Vorobiov"
tr.ui.CREDITS_ADDITIONAL_ART_2A = "Severin Meyer "
tr.ui.CREDITS_ADDITIONAL_ART_2B = "- Xolonium font"
tr.ui.CREDITS_ADDITIONAL_ART_3A = "Alexander Kalachev "
tr.ui.CREDITS_ADDITIONAL_ART_3B = "- Days font"
tr.ui.CREDITS_THANKS = "SPECIAL THANKS"
tr.ui.CREDITS_THANKS_1A = "Dmitry 'divol13' Volokitin "
tr.ui.CREDITS_THANKS_1B = "- For a lot of"
tr.ui.CREDITS_THANKS_1C = " play testing and valuable suggestions"
tr.ui.CREDITS_THANKS_2A = "iforce2d "
tr.ui.CREDITS_THANKS_2B = "- For quick responses"
tr.ui.CREDITS_THANKS_2C = " regarding R.U.B.E. editor support"
tr.ui.CREDITS_CONTACTS = "CONTACTS"
tr.ui.CREDITS_CONTACTS_1 = "Twitter - https://twitter.com/Shephf"
tr.ui.CREDITS_CONTACTS_2 = "Facebook - https://www.facebook.com/TriggerTimeGame"
tr.ui.CREDITS_CONTACTS_3 = "E-Mail - shephmail@gmail.com"
tr.ui.CREDITS_THIRD_PARTY_SW = "THIRD-PARTY SOFTWARE USED"
tr.ui.CREDITS_THIRD_PARTY_TOOLS = "THIRD-PARTY TOOLS USED"
tr.ui.CREDITS_THIRD_PARTY_TOOLS_1 = "R.U.B.E by iforce2d"
tr.ui.CREDITS_THIRD_PARTY_TOOLS_2 = "TexturePacker by CodeAndWeb"
tr.ui.CREDITS_THIRD_PARTY_TOOLS_3 = "2D particle editor from libGDX project"
tr.ui.CREDITS_THANKS_FOR_PLAYING = "Thanks for playing!"

--

tr.str1 = "Jake"
tr.str2 = "Sarge"
tr.str3 = "Skull"
tr.str4 = "Carl"
tr.str5 = "Professor Nathan"
tr.str6 = "Kyle"
tr.str7 = "Guard"
tr.str8 = "Headquarters, 3:00 p.m."
tr.str9 = "Name: Professor Nathan\n"..
    "Occupation: Biology and cybernetics specialist\n"..
    "Status: Enemy #1\n\n"..
    "Database entry id: 292354121\n"..
    "Tracking id: 891223811\n"..
    "Security hash: 2fcab58712467eab\n"..
    "4004583eb8fb7f89\n"..
    "55ad340609f4b302\n"..
    "83e488832571415a\n"..
    "085125e8f7cdc99f\n"..
    "d91dbdf280373c5b\n"..
    "d8823e3156348f5b\n"..
    "e99f33420f577ee8"
tr.str10 = "Name: %s\n"..
    "Rank: Private\n\n"..
    "Database entry id: 192364779\n"..
    "Tracking id: 121245823\n"..
    "Security hash: 83e488832571415a\n"..
    "d91dbdf280373c5b\n"..
    "55ad340609f4b302"
tr.str11 = "Name: Kyle\n"..
    "Occupation: Hacker\n"..
    "Rank: Private\n\n"..
    "Database entry id: 992857111\n"..
    "Tracking id: 120209822\n"..
    "Security hash: e99f33420f577ee8\n"..
    "d8823e3156348f5b\n"..
    "085125e8f7cdc99f\n"..
    "83e488832571415a\n"..
    "55ad340609f4b302\n"..
    "d91dbdf280373c5b\n"..
    "4004583eb8fb7f89\n"..
    "2fcab58712467eab"
tr.str12 = "John"
tr.str13 = "Clyde"
tr.str14 = "Alex"
tr.str15 = "Sam"
tr.str16 = "Stas"
tr.str17 = "Jim"
tr.str18 = " - Move up/down"
tr.str19 = " - Move left/right"
tr.str20 = " - Primary fire"
tr.str21 = " - Secondary fire"
tr.str22 = " - Interact"
tr.str23 = " - Primary weapon switch"
tr.str24 = " - Secondary weapon switch"
tr.str25 = "Introduction"
tr.str26 = "The canyon"
tr.str27 = "The mine"
tr.str28 = "The factory"
tr.str29 = "The prison"
tr.str30 = "The power plant"
tr.str31 = "The toxic dump"
tr.str32 = "The subway"
tr.str33 = "The laboratory"
tr.str34 = "The inner chamber"
tr.str35 = "The epilogue"
tr.str36 = "We've received new intelligence data on our old enemy - professor Nathan. Get to the briefing room ASAP, marine!"
tr.str37 = "We can't storm Nathan's lab directly, so we're sending you through a \"back door\". We'll drop you off on a canyon, it's not very heavily guarded, so it's the safest place to drop you. Good luck."
tr.str38 = "These mines lead to Nathan's facilities. Passing through shouldn't be a problem, only workers and service engineers are here and I'll take them out easily."
tr.str39 = "I've reached Nathan's facilities, but I'm not sure where am I exactly. I need to search for the laboratory entrance."
tr.str40 = "This doesn't look good, I got to find a way out of here... somehow..."
tr.str41 = "Looks like Kyle and I have reached Nathan's power plant, where to next?"
tr.str42 = "I had to leave Kyle at the power plant and one of the power plant's tunnels lead me here... "
tr.str43 = "After defeating the hideous specimen X-13 I've reached this place, it's some sort of a subway."
tr.str44 = "Finally, I've reached Nathan's laboratory. I got to put an end to Nathan's experiments and find Kyle."
tr.str45 = "The entire laboratory is destroyed, time to deal with Nathan himself and save Kyle."
tr.str46 = "Do you really want to quit to main menu ?"
tr.str47 = "Your level progress as well as purchased items will be lost"
tr.str48 = "Dave"
tr.str49 = "Object: Unknown anomaly\n"..
    "Status: Unknown\n\n"..
    "Database entry id: ?????????\n"..
    "Tracking id: 911563976\n"..
    "Security hash: 2fcab58712467eab\n"..
    "4004583eb8fb7f89\n"..
    "55ad340609f4b302\n"..
    "83e488832571415a\n"..
    "085125e8f7cdc99f\n"..
    "d91dbdf280373c5b\n"..
    "d8823e3156348f5b\n"..
    "e99f33420f577ee8"
tr.str50 = " - Move"
tr.str51 = " - Aim"
tr.str52 = "The secret path"
tr.str53 = "I found this secret path at the power plant. I wonder where it leads to..."
tr.str54 = "Containers"
tr.str55 = "You"
tr.str56 = " - Run"

-- e1m1_part0

tr.dialog1.str1 = "Sarge called me, said it's urgent."
tr.dialog1.str2 = "Go ahead, he's waiting for you."

tr.dialog2.str1 = "It's about time you showed up, marine. There's a new mission for you."
tr.dialog2.str2 = "Here, let me show you the briefing video that we have..."

-- e1m1_part1

tr.dialog3.str1 = "Not long ago we've received new intelligence data on our old enemy - professor Nathan."
tr.dialog3.str2 = "We believe he's creating an army of cybernetic mutants in his laboratory."

-- e1m1_part2

tr.dialog4.str1 = "In order to stop him we've started an assault on his laboratory."

tr.dialog5.str1 = "A squad of our best men was sent in."

tr.dialog6.str1 = "Among others was your friend and our best hacker - private Kyle."

tr.dialog7.str1 = "But Nathan's forces were too strong, the squad got into a serious firefight and we lost contact with them. Their fate is unknown..."

-- e1m1_part3

tr.dialog8.str1 = "Since we can't storm Nathan's lab directly, we're sending you through a \"back door\"."
tr.dialog8.str2 = "We'll drop you off on a canyon, it's not very heavily guarded, so it's the safest place to drop you."

-- e1m1_part4

tr.dialog9.str1 = "From there you should find a passage to Nathan's mines."
tr.dialog9.str2 = "Only workers and service engineers are in that area, you should take them out easily."
tr.dialog9.str3 = "You'll have to go through the mines and find an entrance to Nathan's facilities."

-- e1m1_part5

tr.dialog10.str1 = "We don't have anymore intelligence data, but hopefully you'll be able to reach Nathan's labs this way."
tr.dialog10.str2 = "Once you reach it, destroy everything, his creations must be destroyed at all costs."

tr.dialog11.str1 = "That's it marine, we're counting on you."
tr.dialog11.str2 = "You're moving out now!"

-- e1m2_part0

tr.dialog12.str1 = "Damn, the passage is sealed."
tr.dialog12.str2 = "But I can use my gravity gun to clear it up..."

-- e1m2_part2

tr.dialog13.str1 = "Is it a way to the mines behind this laser door?"
tr.dialog13.str2 = "It looks like I'll need some sort of key to go there..."

tr.dialog14.str1 = "This elevator is my only way right now, hope I'll be able to operate it."

-- e1m2_part3

tr.dialog15.str1 = "Great, a shotgun, let's give it a try..."

tr.dialog16.str1 = "Damn you."
tr.dialog16.str2 = "You're off the hook for now, but I'm not done with you yet."
tr.dialog16.str3 = "Nobody tresspasses professor Nathan's territory!"

tr.dialog17.str1 = "So, what do we have here, an intruder."
tr.dialog17.str2 = "Let's see how you handle these..."

tr.dialog18.str1 = "Not bad, but enough of these games."
tr.dialog18.str2 = "Finish him!"

-- e1m2_part5

tr.dialog19.str1 = "Taken care of."
tr.dialog19.str2 = "That service elevator over there will probably take me down to the mines, I should step on it..."

tr.dialog20.str1 = "Hold it, intruder, you're not going any further!"
tr.dialog20.str2 = "It's time to end you!"

-- e1m3_cart

tr.dialog21.str1 = "This is probably a way out of here. Those rocks look heavy, I'll have to find some explosives to clear the path."
tr.dialog21.str2 = "But I don't want to wander around with all those spiders, I think I can use that old mine cart to light my way."

-- e1m3_part0

tr.dialog22.str1 = "These mines should lead me to Nathan's facilities."
tr.dialog22.str2 = "Passing through shouldn't be a problem, only workers and service engineers are here and I'll take them out easily."

tr.dialog23.str1 = "It's too dark, I need to find a flashlight or something before going there..."

-- e1m3_part1

tr.dialog24.str1 = "What the..."

tr.dialog25.str1 = "That's one of service engineers, what the hell happened here..."

tr.dialog26.str1 = "That was a close one, that thing nearly killed me..."
tr.dialog26.str2 = "But there's no turning back, the only way is through the mine."
tr.dialog26.str3 = "It seems that this creature afraids of well-lit areas, so I can use them as a hideout."

-- e1m3_part2

tr.dialog27.str1 = "These things... they killed everyone here... it's all his fault..."
tr.dialog27.str2 = "I don't think I'll last much longer... Here, take this key, it'll help you get out of here..."
tr.dialog27.str3 = "You work for Nathan, do you expect me to trust you..."
tr.dialog27.str4 = "Hey you..."
tr.dialog27.str5 = "Looks like he's dead. I don't think I have any other choice but to try his key."

-- e1m3_part3

tr.dialog28.str1 = "What the hell is that..."

tr.dialog29.str1 = "Oh my lord..."

tr.dialog30.str1 = "It's dead, I can't believe I did it."
tr.dialog30.str2 = "It was some kind of spider queen, so that spider invasion probably started here."
tr.dialog30.str3 = "It doesn't matter anymore, I need to get going."

tr.dialog31.str1 = "What was that..."

tr.dialog32.str1 = "Awful sound... Where is it coming from..."

tr.dialog33.str1 = "That sound... It freaks me out..."

tr.dialog34.str1 = "Finally, this passage will probably lead me straight to Nathan's facilities."
tr.dialog34.str2 = "I should hurry up."

tr.dialog35.str1 = "Those sounds again, whatever they are, I don't want to find out."

tr.dialog36.str1 = "The service elevator, at last!"
tr.dialog36.str2 = "I have no idea what awaits me, our intelligence never made it that far."
tr.dialog36.str3 = "But come on, how can things possibly get worse..."

-- e1m3_pipes

tr.dialog37.str1 = "I hope that the flames that were blocking my cart are now gone."

-- e1m4_final

tr.dialog38.str1 = "Enough of this..."
tr.dialog38.str2 = "You're tougher than I thought, marine, you can prove useful to me."
tr.dialog38.str3 = "Take him to prison sector, I'll deal with him later."

tr.dialog39.str1 = "That's probably the hallway to the labs."
tr.dialog39.str2 = "I need to go there..."

tr.dialog40.str1 = "Well now, another marine..."
tr.dialog40.str2 = "It's hard to believe you made it that far on your own, you're pretty tough."
tr.dialog40.str3 = "But not as tough as my latest creations you're about to face - the deadly mix of metal and flesh."
tr.dialog40.str4 = "Your journey ends here."

-- e1m4_part0

tr.dialog41.str1 = "It's some kind of communication panel, I think I can contact our HQ using it."
tr.dialog41.str2 = "... ... ... ... ... ... ... almost done ... ... ... ... ... ... ... ... ..."
tr.dialog41.str3 = "HQ, do you read..."
tr.dialog41.str4 = "Marine ... ... ... Is that you?"
tr.dialog41.str5 = "It's me, I've reached Nathan's facilities, but I'm not sure where am I exactly..."
tr.dialog41.str6 = "Good to know that you're still alive."
tr.dialog41.str7 = "You got to find an entrance to the lab, look around, it's probably behind some heavily secured door..."
tr.dialog41.str8 = "Be sure to ... ... ..."
tr.dialog41.str9 = "What the..."

-- e1m4_part1

tr.dialog42.str1 = "Hmm, proximity mines. Better grab those, they'll certainly be useful."

-- e1m4_part2

tr.dialog43.str1 = "Attention all personnel, there's an intruder in factory sector \"B\"."
tr.dialog43.str2 = "Stop him at all costs."

-- e1m4_part3

tr.dialog44.str1 = "I think this panel programs the sentry over there, I can program it to be on my side."
tr.dialog44.str2 = "The sentry seems to have some security access, I can use it to pass further."
tr.dialog44.str3 = "I better be careful not to let anything happen to the sentry, otherwise I won't be able to pass."

-- e1m5_part0

tr.dialog45.str1 = "This doesn't look good, I got to find a way out of here... somehow..."

-- e1m5_part3

tr.dialog46.str1 = "This panel also needs code to operate, damn..."

tr.dialog47.str1 = "This panel needs some kind of code to operate, it's useless to me..."

tr.dialog48.str1 = "Oh, thank god it's you."
tr.dialog48.str2 = "Nathan's men killed everyone in our squad and they captured me."
tr.dialog48.str3 = "You got to help me, find a way to open my cell..."

tr.dialog49.str1 = "I can't believe it, my backpack."
tr.dialog49.str2 = "Maybe there's hope to get out of this alive..."

tr.dialog50.str1 = "Thanks, now let's get out of here."

tr.dialog51.str1 = "Hold on a sec, I'll deal with main gate access."
tr.dialog51.str2 = "They don't call me the best hacker for nothing."
tr.dialog51.str3 = "In the meantime, you can grab some guns in the armory, I'll open the door for you..."

tr.dialog52.str1 = "Done. Main gate will now open."
tr.dialog52.str2 = "Go check it out, if it's safe I'll follow."

tr.dialog53.str1 = "Damn, they blocked the door, they're probably heading this way."
tr.dialog53.str2 = "Take care of them, I'll try the other gate."

-- e1m5_part4

tr.dialog54.str1 = "Are you ok?"
tr.dialog54.str2 = "I'm fine, no time for chatting, can you deactivate the lasers?"
tr.dialog54.str3 = "I can try, give me a sec..."

tr.dialog55.str1 = "This one's done, let's go try the others..."

tr.dialog56.str1 = "Just a second here..."
tr.dialog56.str2 = "I think I can turn off one of them..."

tr.dialog57.str1 = "Sorry, that's the best I can do, you see, the cipher they're using doesn't..."
tr.dialog57.str2 = "Doesn't matter, if that's the only way we can go then let's go before more of these things arrive."

-- e1m5_yard

tr.dialog58.str1 = "Attention, detainee has escaped from sector 2, get him!"

-- e1m6_part0

tr.dialog59.str1 = "Interesting, let's see where this door leads to. Kyle, you stay here, I go check it out."
tr.dialog59.str2 = "Ok, no problem."

tr.dialog60.str1 = "No point in going back there..."

tr.dialog61.str1 = "Ok, it's clear. Let me look around here..."

tr.dialog62.str1 = "Looks like we're at the Nathan's power plant, there's some tunnel here."
tr.dialog62.str2 = "Kyle, get over here and check this out..."

tr.dialog63.str1 = "This will only take a second... ... ..."
tr.dialog63.str2 = "Got it. According to these files this tunnel leads to power plant's toxic waste dump, this is our only way out."
tr.dialog63.str3 = "Unfortunately, this tunnel will only open in extreme circumstances such as reactor core damage, so we probably should look for other..."
tr.dialog63.str4 = "Wait a second, what if we damage the reactor core on purpose. I think that service bridge over there leads straight to reactor core, let's check it out."

tr.dialog64.str1 = "Ok, I'm about to... ... done!"
tr.dialog64.str2 = "This panel can control the service bridges as well as some other parts of the facility."
tr.dialog64.str3 = "So, if you just extend this service bridge that leads to the core, I can smash it and we'll be on our way."
tr.dialog64.str4 = "Exactly. Also, this panel can act as an intercom, go grab an earpiece over there, we'll then be able to communicate remotely. That is, if something goes wrong..."

tr.dialog65.str1 = "Got it."
tr.dialog65.str2 = "Good. I'm having problems extending the service bridge right now, why don't you go restore your ammunition and health while I'm at it. I'll deactivate the lasers for you."

tr.dialog66.str1 = "I'm done. The service bridge will now extend."
tr.dialog66.str2 = "Also, see this sentry here, I've programmed it to be on your side, it'll go with you, just in case..."
tr.dialog66.str3 = "Thanks Kyle, so, let's roll."

-- e1m6_part1

tr.dialog67.str1 = "Get out of there, now!"

tr.dialog68.str1 = "Bad news, Kyle, all reactor core rods are shielded, we got to expose them somehow..."
tr.dialog68.str2 = "I see... There's a reactor core control room in eastern wing, let's try our luck there. I'll extend another service bridge now, hold on."

-- e1m6_part2

tr.dialog69.str1 = "Kyle, we have a problem, I can't get to the control room, you see, it's a ..."
tr.dialog69.str2 = "A leak in main steam line, I see it right here on the display, I'm thinking about it..."
tr.dialog69.str3 = "There seems to be a backup generator in the facility, if we can make it work, we'll redirect all the steam there and it'll clear the path."
tr.dialog69.str4 = "First, we'll have to enable backup cooling system, the path to the cooling facility lies through the main generator, I've opened the door for you, go check it out."

-- e1m6_part4

tr.dialog70.str1 = "Kyle, I've enabled backup cooling system, now what?"
tr.dialog70.str2 = "Excellent work! But it looks like backup generator does not have enough resistance crystals loaded in it."
tr.dialog70.str3 = "Without them the generator will not start. You'll have to go to the west wing of the power plant and fix this."
tr.dialog70.str4 = "Proceed through the water pump section to the service bridge, I'll get you on the other side then."

-- e1m6_part5

tr.dialog71.str1 = "Get out of there, hurry!"

tr.dialog72.str1 = "I'm extending service bridge to west wing, hold on."

-- e1m6_part6

tr.dialog73.str1 = "All resistance crystals are in place, Kyle."
tr.dialog73.str2 = "Very good! Now for the final touch, reach the steam room in north wing, you'll be able to redirect steam flow to this backup generator from there."

-- e1m6_part7

tr.dialog74.str1 = "Excellent job, reactor core rods are exposed now!"
tr.dialog74.str2 = "I'll extend east service bridge again, it'll get you to the core."

tr.dialog145.str1 = "Oh, thank god it's you..."
tr.dialog145.str2 = "Me and 2 other guys from Nathan lab storm squad managed to survive, everyone else were killed..."
tr.dialog145.str3 = "This is a way to toxic dump tunnel, our only way out of here. John and Sam went to check it out."
tr.dialog145.str4 = "You can go there too right now. Or you can go later from the mission menu."
tr.dialog145.str5 = "Mission menu ? What are you talking about ?"
tr.dialog145.str6 = "Don't you get it yet, you found a secret level!"

-- e1m6_part8

tr.dialog75.str1 = "You did it! Now we need to get out of here, I'll extend the south bridge now, hold on."

tr.dialog76.str1 = "Damn, it won't move..."
tr.dialog76.str2 = "It seems that there's another passage to the toxic dump on the west side of the facility, I'll try extending west bridge now."

tr.dialog77.str1 = "It worked, go to the toxic dump tunnel door."

tr.dialog78.str1 = "I see strange message here on display, it says \"unsolicited reactor core exposure, core protector had been released\""
tr.dialog78.str2 = "\"core protector\", what's that suppose to mean?"

tr.dialog79.str1 = "Ok, I'm opening the doors now."

tr.dialog80.str1 = "This door over here is broken, it won't open."
tr.dialog80.str2 = "It looks like you'll have to go without me."
tr.dialog80.str3 = "No way, there's got to be another way, we just need to..."
tr.dialog80.str4 = "Just go, don't waste your time. The facility is in red alert mode, more Nathan's forces will probably be here soon."
tr.dialog80.str5 = "I'll try to think of something, but you can't help me right now..."

-- e1m7_part0

tr.dialog81.str1 = "I need to find an exit from this toxic dump facility, something tells me it's not going to be behind next door..."

tr.dialog82.str1 = "What are these... They look nasty"

-- e1m7_part1

tr.dialog83.str1 = "Looks like the only way is to take a ride on that boat."

-- e1m7_part3

tr.dialog84.str1 = "It looks like the ride is over... For now."

tr.dialog85.str1 = "I think I know what to do with this..."

-- e1m7_part4

tr.dialog86.str1 = "This panel says \"The subway tunnel\", looks like a way out."
tr.dialog86.str2 = "But these things are blocking my path, I'll have to find another way..."

tr.dialog87.str1 = "This panel says \"Danger, specimen X-13, proceed with extreme caution\"."
tr.dialog87.str2 = "But I have to go, I don't have any other choice..."

tr.dialog88.str1 = "I have no idea what that thing was, I never seen anything like it."
tr.dialog88.str2 = "But it was some kind of \"specimen\", I think Nathan is up to something big, I need to stop him before it's too late..."

-- e1m8_part0

tr.dialog89.str1 = "This elevator is not powered on. I need to power it on elsewhere first..."

tr.dialog90.str1 = "This panel says \"Transport pod service access\"."
tr.dialog90.str2 = "I think I should enable it."

tr.dialog91.str1 = "There's no point in going further, I should go back..."

tr.dialog92.str1 = "I shouldn't go further, it's too dangerous..."

tr.dialog93.str1 = "You went all the way here just to get a lousy chainsaw, I hope you're happy now >:("

tr.dialog94.str1 = "You want to get us killed, turn back now!"

tr.dialog95.str1 = "Tell me, why are you doing this? I'm just curious..."

-- e1m8_part1

tr.dialog96.str1 = "These are universal energy shields that protect against all kinds of projectiles."
tr.dialog96.str2 = "They were developed specially for our squad, I guess Nathan got his hands on these..."

tr.dialog97.str1 = "Let's see, this is station access control panel."
tr.dialog97.str2 = "Might be a good idea to use it."

-- e1m8_part2

tr.dialog98.str1 = "It's pitch black over there, I need to light my way somehow..."

tr.dialog99.str1 = "This panel says \"Specimen Y-5, adult, male\"."
tr.dialog99.str2 = "It's those things that I encountered back in the mines."
tr.dialog99.str3 = "They're contained here, I just hope they won't get out..."

tr.dialog100.str1 = "This panel says \"Specimen Y-5, babies\"."

tr.dialog101.str1 = "This panel says \"Specimen Y-5, adult, female\"."

tr.dialog102.str1 = "What's with the pods here... I wonder what they're used for..."

tr.dialog103.str1 = "It looks like the pods above are used to transport spiders, but where?"

-- e1m8_part3

tr.dialog104.str1 = "These pods... I wonder what they're used for..."

tr.dialog105.str1 = "They breed those things and transport them, but where?"

tr.dialog106.str1 = "It's pitch black over there, not safe to go..."

tr.dialog107.str1 = "It's pitch black over there, not safe to go..."

tr.dialog108.str1 = "This panel says \"Breeder lighting control\"."
tr.dialog108.str2 = "I should give it a try."

tr.dialog109.str1 = "Oh my lord, I got to get out of here!"

tr.dialog110.str1 = "I better think twice before opening this door..."

-- e1m8_part4

tr.dialog111.str1 = "Report status, have you found the escaped marine?"
tr.dialog111.str2 = "Not yet, but I've captured the hacker guy that came along with him."

tr.dialog112.str1 = "Excellent, tell your men to take him to my laboratory."
tr.dialog112.str2 = "As for yourself, stay there and keep an eye out, I want that marine dead!"

tr.dialog113.str1 = "Wait, please don't shoot."
tr.dialog113.str2 = "Hey, I remember you, you helped me back in the mines. But I thought you died..."
tr.dialog113.str3 = "Listen, I know you got a lot of questions, but there's no time to waste."
tr.dialog113.str4 = "Your friend Kyle is in danger, if you want to help him, you should hurry up."
tr.dialog113.str5 = "These pods are going straight to Nathan's lab, your only choice is to take a ride."
tr.dialog113.str6 = "But why are you helping me, who are you?"

tr.dialog114.str1 = "What, where did he go?"
tr.dialog114.str2 = "Anyway, he helped me back then, if he says that Kyle is in danger then perhaps it's true."
tr.dialog114.str3 = "I should hurry up."

tr.dialog115.str1 = "Well hello there. It's been a while since we last met."
tr.dialog115.str2 = "You again, but I thought you..."
tr.dialog115.str3 = "Died? Not a chance, I don't go down that easily, but you will..."
tr.dialog115.str4 = "Prepare to die!"

tr.dialog116.str1 = "That's it, playtime is over. Die!"

tr.dialog117.str1 = "Another problem taken care of."

tr.dialog118.str1 = "This looks like a hallway to the laboratory. And here I come..."

-- e1m9_part0

tr.dialog119.str1 = "It's you..."
tr.dialog119.str2 = "Who would've thought that one man can cause so much trouble. I've underestimated you."
tr.dialog119.str3 = "But you have underestimated what you're up against."
tr.dialog119.str4 = "I can see you, I can hear you, I'm everywhere."
tr.dialog119.str5 = "And this time there will be no mercy for you..."

-- e1m9_part1

tr.dialog120.str1 = "This mech is my only hope to get this door opened."
tr.dialog120.str2 = "Better not screw this up..."

tr.dialog121.str1 = "Stop him! Don't let him get to sector \"B\"..."

tr.dialog122.str1 = "With that mech destroyed I won't be able to proceed. All is lost..."

tr.dialog123.str1 = "Now to end Nathan's experiments once and for all."

tr.dialog124.str1 = "Nathan, do you hear me? I've wiped out this entire place, how do you like that?"
tr.dialog124.str2 = "It's over, surrender..."

tr.dialog125.str1 = "Over? I don't think so... You haven't seen anything yet."
tr.dialog125.str2 = "And your friend Kyle is right here with me, why don't you come join us."
tr.dialog125.str3 = "If you dare..."

-- e1m9_part2

tr.dialog126.str1 = "You. How dare you destroy my work."
tr.dialog126.str2 = "A taste of my neurotoxin is what you need."

tr.dialog127.str1 = "I got to find a way out of here, fast!"

tr.dialog128.str1 = "I think I can disable neurotoxin admission from this panel."

-- e1m9_part4

tr.dialog129.str1 = "You're destroying the work of my life. You're gonna pay for that."
tr.dialog129.str2 = "You'll be a prey for my experiment."

-- e1m10_part0

tr.dialog130.str1 = "So, we finally met face to face..."
tr.dialog130.str2 = "Cut the talking, where's Kyle?"
tr.dialog130.str3 = "So eager to see your friend... Don't worry, he's here."
tr.dialog130.str4 = "He's now part of my ultimate creation. It'll be a pleasure watching him destroy you."

tr.dialog131.str1 = "Kyle, buddy, it really is you... What have Nathan done to you..."
tr.dialog131.str2 = "I'm sorry... I'm sorry but I have to do this..."

-- e1m10_part1

tr.dialog132.str1 = "Tremble before me, as you're about to learn that I'm unstoppable..."

tr.dialog133.str1 = "You... You've... You've defeated me... I'm dying..."
tr.dialog133.str2 = "But.. you're... you're going down... with... me... ..."

tr.dialog134.str1 = "You... you destroyed my best creation. My masterpiece..."
tr.dialog134.str2 = "But I'm not giving up, I'm waiting for you in my chamber... I'm waiting to kill you!"

-- e1m10_part2

tr.dialog135.str1 = "You did it. You killed him!"
tr.dialog135.str2 = "But the facility is set to self-destruct in case of professor's death. This entire place is gonna explode within minutes."
tr.dialog135.str3 = "There's an emergency shuttle nearby, follow me, we must hurry..."

tr.dialog136.str1 = "This passage will get you straight to emergency shuttle, hurry up!"
tr.dialog136.str2 = "Wait, aren't you going?"
tr.dialog136.str3 = "No... I was so blind helping Nathan all that time..."
tr.dialog136.str4 = "I realized what I was doing, but it was too late... I don't deserve better fate than Nathan..."
tr.dialog136.str5 = "Now go... And.. I'm sorry for your friend Kyle... I'm sorry that I couldn't prevent all that..."

-- e1m11_part0

tr.dialog137.str1 = "Good job out there, bro."
tr.dialog137.str2 = "Sarge is waiting for you, go right in."

tr.dialog138.str1 = "Well done, marine. I can't believe you took out Nathan and all his minions all by yourself."
tr.dialog138.str2 = "But we have another problem, have a look at our latest intelligence data."

-- e1m11_part1

tr.dialog139.str1 = "As Nathan died the entire facility self-destruction sequence was activated."
tr.dialog139.str2 = "The perimiter security weakened, so we got closer to the heart of the facility to make sure that nothing is left of Nathan's work."

tr.dialog140.str1 = "Everything looked clear, so we moved on forward."

-- e1m11_part2

tr.dialog141.str1 = "And then we found this..."

tr.dialog142.str1 = "This is some unknown anomaly we've never seen before."
tr.dialog142.str2 = "Our science team says that it's some kind of inter-dimensional portal."
tr.dialog142.str3 = "So things that you've faced in the labs, Nathan probably learned to somehow use this other dimension."
tr.dialog142.str4 = "He died, but hell knows what's gonna come out from that portal..."

tr.dialog143.str1 = "You know about these creatures more than any of us, so you're the only one who can handle this."
tr.dialog143.str2 = "The portal must be dealt with! You're going back, marine, meet up with the bravo team, they'll instruct you further."
tr.dialog143.str3 = "I'm ready to do my job as always, sir!"

-- e1m12_part2

tr.dialog144.str1 = "Hey, good to see you here."
tr.dialog144.str2 = "The toxic dump tunnel is this way, Sam went there a few hours ago and I didn't hear from him since then..."
tr.dialog144.str3 = "Go check it out, but be careful! I have no idea what's over there..."
