This is an unofficial port of SkinDeep to Linux

Currently (2025-05-07) the code is based on Hotfix02: https://store.steampowered.com/news/app/301280/view/529845510803032094
but has additional changes to make it build and work on Linux (and I replaced libjpeg with stb_image.h).

You can find the original source code as released by Blendo Games in the "upstream" branch.

This code should work with both the SkinDeep demo that used to be available on Steam
and the full version that you can buy for little money at

https://store.steampowered.com/app/301280/Skin_Deep/

Building SkinDeep on Linux should work as expected with CMake.

If you want to use this with gamedata from the Demo, make sure to enable `SDDEMO` in CMake.

# Building this source code

See https://github.com/dhewm/dhewm3/?tab=readme-ov-file#compiling for compiling instructions.
Note that due to this being based on an older version of dhewm3, you'll need additional dependencies:
zlib, libogg, libvorbis, libvorbisfile
Just like with current dhewm3, you need:
libSDL2, libopenal (OpenAL-Soft), cmake, make or ninja, GCC/G++ or clang/clang++
(you won't need libcurl though)

Furthermore, Blendo Games added a Visual Studio solution for building and includes the necessary
dependencies for Windows in this repo, see neo/solution/
As this unofficial port focuses on Linux, I don't maintain or test the Visual Studio solution.
I want this to still compile on Windows, but by using CMake, just like on all other platforms.
(Note that CMake can generate Visual Studio solutions)

