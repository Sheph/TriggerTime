Trigger Time (https://github.com/Sheph/TriggerTime)
=========================

**1. About**
-------------------
Trigger Time is a story-driven, top-down Shoot'em up with a fun gravity gun and physics puzzles to solve.
Blast your way through 9 levels filled with enemies, hazards and bizarre bosses.
Along the way you'll take control of vehicles, pick up weapons, items and boosters.

Check out Trigger Time steam trailer on youtube:

[![Trigger Time Steam trailer](https://img.youtube.com/vi/u9Aoe5sreEg/0.jpg)](https://www.youtube.com/watch?v=u9Aoe5sreEg)

You can also get Trigger Time on Steam for free here: https://store.steampowered.com/app/512920

**2. Status**
-------------------
This is 100% full source code for the game. However, due to some legal stuff it's not possible to publish some of the in-game
music here on github. Those music files were replaced by "5 minutes of silence" dummy ogg files. If you want to have the
music in game you'll have to install the game from Steam (for free), locate music files and overwrite those dummy oggs. More
on that later.

**3. Building on windows**
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
