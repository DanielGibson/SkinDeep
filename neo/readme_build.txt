
set F5 debug settings for build:
Set monstergame > properties > debugging > working directory to: d:\games\monstergame
Set monstergame > properties > debugging > command to: d:\games\monstergame\monstergame.exe




Removed cmake stuff, so everything below here is now mostly obsolete.....

1. If you see this error:

"cmd.exe" exited with code 1.

it is a CMAKE error! Delete cmakecache.txt in the neo\solution folder. Start a new build. It should remake cmakecache.txt for you.


2. In visual studio, remove the BASE project. BASE is the vanilla dll file. You'll be using the D3XP expansion dll instead.

3. Change monstergame > properties > debugging > working directory to the game's root folder. d:\games\monstergame

4. Change monstergame > properties > debugging > command to the game's exe file. d:\games\monstergame\monstergame.exe


If you see this error:
The system cannot find the path specified.
"cmd.exe" exited with code 3

it might be because you are missing CMAKE installation. The compiler expects cmake to be in:
D:\program files (x86)\cmake


The build uses cmake. Has been tested on cmake 3.8.1 and cmake 3.17.0-rc2


If you need to use cmake to regenerate the solution, the dependencies are all included in the neo/solution/dependencies folder.
All include folders, paste this: solution/dependencies/include
All library folders, paste this: solution/dependencies/lib

Their dependency folder structure is all setup in such a way that you can just copy/paste these two folder names for all lib/include fields.

------> EXCEPT for sdl2 include folder: solution/dependencies/include




cmakelists.txt:
-find_package(Vorbis REQUIRED)
+find_package(VORBIS REQUIRED)
 
-find_package(VorbisFile REQUIRED)
+find_package(VORBISFILE REQUIRED)