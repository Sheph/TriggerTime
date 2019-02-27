Trigger Time (https://github.com/Sheph/TriggerTime)
=========================

[![bsd license][img_license]](https://github.com/Sheph/TriggerTime/blob/master/COPYING?raw=true) [![Tweet](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/intent/tweet?text=Trigger%20Time%20becomes%20open-source.%20Full%20source%20code%20is%20available%20under%20BSD%20license!&url=https://github.com/Sheph/TriggerTime&hashtags=gamedev,indiedev,indiegame,steam)

[img_license]: https://img.shields.io/badge/license-BSD-lightgrey.svg

+ [1. About](#1-about)
+ [2. Status](#2-status)
+ [3. Building on windows](#3-building-on-windows)
  - [Install CMake](#install-cmake)
  - [Install Visual Studio Express 2013 for Windows Desktop](#install-visual-studio-express-2013-for-windows-desktop)
  - [Generate visual studio solution](#generate-visual-studio-solution)
  - [Build and run the game](#build-and-run-the-game)
+ [4. Building on linux](#4-building-on-linux)
+ [5. Building for android](#5-building-for-android)
  - [Build commands](#build-commands)
  - [Other NDK build options](#other-ndk-build-options)
+ [6. Replacing in-game music](#6-replacing-in-game-music)
+ [7. Running without Steam](#7-running-without-steam)
+ [8. Developer settings](#8-developer-settings)
+ [9. Build for distribution](#9-build-for-distribution)
+ [10. Modifying objects, enemies and levels](#10-modifying-objects-enemies-and-levels)
+ [11. Contacts](#11-contacts)

### 1. About
-------------------
Trigger Time is a story-driven, top-down shoot'em up with a fun gravity gun and physics puzzles to solve.
Blast your way through 9 levels filled with enemies, hazards and bizarre bosses.
Along the way you'll take control of vehicles, pick up weapons, items and boosters.

<img src="https://github.com/Sheph/TriggerTime/blob/master/screenshot1.jpg?raw=true"/><img src="https://github.com/Sheph/TriggerTime/blob/master/screenshot2.jpg?raw=true"/><img src="https://github.com/Sheph/TriggerTime/blob/master/screenshot3.jpg?raw=true"/><img src="https://github.com/Sheph/TriggerTime/blob/master/screenshot4.jpg?raw=true"/><img src="https://github.com/Sheph/TriggerTime/blob/master/screenshot5.jpg?raw=true"/>

Check out Trigger Time steam trailer on youtube:

[![Trigger Time Steam trailer](https://img.youtube.com/vi/u9Aoe5sreEg/0.jpg)](https://www.youtube.com/watch?v=u9Aoe5sreEg)

You can also get Trigger Time on Steam for free here: https://store.steampowered.com/app/512920

### 2. Status
-------------------
This is 100% full source code for the game. However, due to some legal stuff it's not possible to publish some of the in-game
music here on github. Those music files were replaced by "5 minutes of silence" dummy ogg files. If you want to have the
music in game you'll have to install the game from Steam (for free), locate music files and overwrite those dummy oggs. More
on that later in section 6.

Also, by default, Steam is required for the game to run, if you want to run the game without Steam please see section 7 below.

### 3. Building on windows
-------------------

#### Install CMake

Download latest stable (not RC) .exe installer at http://www.cmake.org/download/

Version 3.2.3+ will do.

Run it, choose "Add CMake to the system PATH for all users", accept other defaults.

#### Install Visual Studio Express 2013 for Windows Desktop

Go to https://visualstudio.microsoft.com/vs/older-downloads/

find "Visual Studio Express 2013 for Windows Desktop" and press "download"

You'll need microsoft account, if you don't have one, create it and use it to download the installer.

Run the installer, accept all defaults (You may uncheck "experience improvement program" however).

Launch visual studio, don't select "sign in now", it's not necessary. Wait for first time setup to complete, you
can now exit visual studio if you like.

#### Generate visual studio solution

Open cmd shell (i.e. win key + R, type "cmd"), inside the shell run:

<pre>
cmake_Win32_VS2013.bat
</pre>

If everything was done right, you'll see CMake generating the solution and output:

<pre>
-- Build files have been written to: C:/Projects/TriggerTime/build_Win32_VS2013
</pre>

#### Build and run the game

Open visual studio if you haven't done so already, open
C:\Projects\TriggerTime\build_Win32_VS2013\AIRFORCE.sln solution.

* Set "tt" project as active project, i.e. right-click "tt" project and select "Set as StartUp project"
* Open "tt" project properties, i.e. right-click "tt" project and select "Properties"
* Set "Configuration" to "All Configurations"
* Go to "Configuration Properties -> Debugging" and change "Working Directory" to $(ProjectDir)..\out\bin\$(Configuration)
* Press "Ok"

Now you can build and run the game in different configurations, i.e. to build in release mode
choose "Release" in the topmost combox box and press "ctrl + F5"

Once the game is built you can also run it outside of IDE by simply
running "C:\Projects\TriggerTime\build_Win32_VS2013\out\bin\Release\tt.exe"

### 4. Building on linux
-------------------

Install prerequisites:

<pre>
sudo apt-get install cmake libx11-dev libxxf86vm-dev
</pre>

Then:

<pre>
./cmake_x64_release.sh
cd ../TriggerTime-x64-release
make -j4
</pre>

Then:

<pre>
cd ./out/bin
./tt
</pre>

You can also use other cmake_xxx.sh scripts to generate debug, i386 and steam runtime version of the game.

### 5. Building for android
-------------------

Originally there was also an android build, but at some point the game became "PC only". There're still android makefiles
here and there, the game code still contains touchscreen control code, android platform code, etc. So technically, with some effort, one can get the game running on android again... But for now, the **build is broken**, so you were warned :)

#### Build commands

<pre>
cd ./android
echo "sdk.dir=/home/user/android-sdk-linux/sdk" > local.properties
/home/user/android-ndk-rxx/ndk-build
ant debug
</pre>

Then:

<pre>
adb install ./bin/AirForce-debug.apk
</pre>

#### Other NDK build options

Multithreaded build:
<pre>
/home/user/android-ndk-rxx/ndk-build -j8
</pre>

Build for one arch only:
<pre>
/home/user/android-ndk-rxx/ndk-build APP_ABI=x86
</pre>

Build for native debugging:
<pre>
/home/user/android-ndk-rxx/ndk-build NDK_DEBUG=1 APP_OPTIM=debug
</pre>

Start native debugging:
<pre>
bash /home/user/android-ndk-rxx/ndk-gdb --start
</pre>

### 6. Replacing in-game music
-------------------

Due to some legal stuff it's not possible to publish some of the in-game
music here on github. Those music files were replaced by "5 minutes of silence" dummy ogg files. If you want to have the
music in game you'll have to:

* Go to Steam and install the game: https://store.steampowered.com/app/512920
* If you're on windows, go to C:/Program Files (x86)/Steam/SteamApps/common/Trigger Time/assets
* If you're on linux, go to /home/user/.steam/steam/SteamApps/common/Trigger Time/assets
* Copy all action*.ogg and ambient*.ogg files to ./game/assets with overwriting

### 7. Running without Steam
-------------------

The game is intended to run from Steam, so if you don't have Steam opened you'll see something like this in console:

<pre>
[S_API FAIL] SteamAPI_Init() failed; SteamAPI_IsSteamRunning() failed.
[S_API FAIL] SteamAPI_Init() failed; unable to locate a running instance of Steam, or a local steamclient.so.
Game startup failed, see $HOME/.TriggerTime/log.txt for details
</pre>

In order to fix this open config.ini and change steamApiRequired to false.

### 8. Developer settings
-------------------

config.ini has a lot of stuff for developer to use, some of the settings are:

setting | description
------------ | -------------
developer | an integer from 1 to N specifying debug spawn point, most of the levels have many debug spawn points that are used to check out part of the level without running through from the start. note that if you set this to a non-existent spawn point the game will simply crash in script :)
debugKeys | if set to true in-game debug keys are enabled, such as 'p' for physics debugging, 'r' for render debugging, 'c' for culling debugging, 'g' for ghost mode, 'm' for slowmo, +/-/* for zoom, etc.
atLeastGems| number of games that the player will always have
allLevelsAccessible| make all levels accessible on all difficulty settings
skill| run the game on specified skill

Also, when debugKeys is true you can run the game like this:
<pre>
tt.exe e1m4.lua e1m4.json
</pre>
i.e. launch some level right away.

### 9. Build for distribution
-------------------
* If you're on windows you can select "INSTALL" project from game solution and build it
* If you're on linux you can run "make install"

This will generate game redistributable package, this includes:
* copying all the necessary things to separate directory
* precompiling all lua scripts

The resulting directory will contain everything to run the game in any environment. Game's Steam build was
also made this way.

### 10. Modifying objects, enemies and levels
-------------------

The game heavily uses R.U.B.E Box2D editor - https://www.iforce2d.net/rube

Though game itself is free, R.U.B.E is not free, it costs around $35. There's a demo version on website, but it
can only be used for viewing scenes, not saving them.

Most of the resources that have physics (i.e. objects, props, enemies and entire levels) are stored in .rube format in
games/res directory, you can open those files using R.U.B.E, modify them and then export to game/assets in json format.

For levels, there're also lua scripts that are used to drive things like cutscenes, enemy spawn, etc. Lua scripts are in game/assets directory and are named after levels, i.e. e1m4.rube level will export to e1m4.json and will have e1m4.lua level script (which in turn consists of several parts, i.e. e1m4_part0.lua, e1m4_part1.lua, etc.)

### 11. Contacts
-------------------

E-Mail: shephmail@gmail.com

FaceBook: https://www.facebook.com/TriggerTimeGame

Twitter: https://twitter.com/Shephf
