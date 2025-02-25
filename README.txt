NOTE: This README (everything behind the line of dashes) was included in the SkinDeep sourcecode
(skindeep_sourcecode_2025_02_13.zip)

SkinDeep isn't really based on dhewm3 1.5.4, but some older version.

Building SkinDeep on Linux should work as expected with CMake.

The only thing you need to change to run the Demo is to go to the skindeep_demo/ directory from Steam,
there into basedemo/glsl/ and 
rename "motionBlurBlendo.frag" to "motionblurBlendo.frag" (note the lowercase "b" in "blur")
and "motionBlurBlendo.vert" to "motionblurBlendo.vert"

This is necessary because unlike on Windows, filesystems on Linux are case-sensitive,
and materials/fx.mtr in pak000.pk4 has the line "program            motionblurBlendo.vfp" where
it's written like that.

Get the SkinDeep demo at https://store.steampowered.com/app/3483180/Skin_Deep_Demo/

-------------

dhewm 3 version 1.5.4 for Windows

Released August 3rd 2024 - celebrating 20 years of Doom3!

This build contains the new ImGui based settings menu and Soft Particles.
You can open the menu with F10, unless you have something else bound to F10,
or by entering "dhewm3Settings" in the console.

dhewm3_notools.exe is built without the MFC-based tools like the D3Radiant level editor.
It should work better with nvidia DSR and AMD VSR (which render the game at a higher resolution than
the display supports and then scale it down, like some kind of anti-aliasing on steroids).

This is built with Visual Studio 2017, so if it doesn’t start on your system make sure you have
the Visual C++ 2017 Redistributable for X86 installed.
You can download it at: https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist

 ABOUT
=======

dhewm 3 is a Doom 3 GPL source port, known to work on at least Windows, Linux, macOS and FreeBSD.

The goal of dhewm 3 is bring _DOOM 3_ with the help of SDL to all suitable
platforms.

Bugs present in the original _DOOM 3_ will be fixed (when identified) without
altering the original gameplay.

The official homepage is: https://dhewm3.org

Mods supported by dhewm3: https://dhewm3.org/mods.html

The project is hosted at: https://github.com/dhewm

Download the latest release: https://github.com/dhewm/dhewm3/releases/latest

Consult the FAQ at: https://github.com/dhewm/dhewm3/wiki/FAQ

Report bugs here: https://github.com/dhewm/dhewm3/issues


Compared to the original DOOM 3, some of the changes of dhewm 3 worth mentioning are:

- 64-bit port
- SDL for low-level OS support, OpenGL and input handling
- OpenAL for audio output, all OS-specific audio backends are gone
- OpenAL EFX for EAX reverb effects (read: EAX-like sound effects on all platforms/hardware)
- Gamepad support
- Better support for widescreen (and arbitrary display resolutions)
- A portable build system based on CMake
- (Cross-)compilation with MinGW-w64
- An advanced, mod-independent settings menu (opened with F10 by default)
- Lots of bugfixes

See Changelog.txt for a more complete changelog.


 HOW TO INSTALL
================

You'll need the game data from a Doom3 installation patched to 1.3.1.
Specifically, you'll need the following .pk4 files for the main game:

  Filename          Size   MD5-sum
----------------- ------  ---------------------------------
 base/pak000.pk4   337MB   71b8d37b2444d3d86a36fd61783844fe

 base/pak001.pk4   220MB   4bc4f3ba04ec2b4f4837be40e840a3c1

 base/pak002.pk4   398MB   fa84069e9642ad9aa4b49624150cc345

 base/pak003.pk4   303MB   f22d8464997924e4913e467e7d62d5fe

 base/pak004.pk4   227MB   38561a3c73f93f2e6fd31abf1d4e9102

 base/pak005.pk4   540KB   2afd4ece27d36393b7538d55a345b90d

 base/pak006.pk4   214KB   a6e7003fa9dcc75073dc02b56399b370

 base/pak007.pk4   118KB   6319f086f930ec1618ab09b4c20c268c

 base/pak008.pk4    12KB   28750b7841de9453eb335bad6841a2a5
-----------------------------------------------------------

... and (optionally) these .pk4 files for the *Resurrection of Evil* addon:

  Filename           Size  MD5-sum
----------------- ------- ---------------------------------
 d3xp/pak000.pk4   514MB   a883fef0fd10aadeb73d34c462ff865d

 d3xp/pak001.pk4    98KB   06fc9be965e345587064056bf22236d2
-----------------------------------------------------------

.. or alternatively you can use the demo00.pk4 from the free Demo version, see below for more information.

Just put the .pk4 files into the corresponding directories (base/ and d3xp/) that are next to this README.

Alternatively you can have the dhewm3 binaries (dhewm3.exe, base.dll etc) and the game data in
separate places and tell dhewm3 where to find the directory that contains base/ and d3xp/,
with the following commandline argument:
> dhewm3.exe +set fs_basepath c:\path\to\Doom3\

The following sections explain how to obtain these .pk4 files.

 ... from CD/DVD + Patch
 -----------------------
If you bought the game on CDs/DVD, base/pak000.pk4 - pak004.pk4 and d3xp/pak000.pk4
can be copied from the disks, the other files are from the patch.

On Windows you can just install the game and the official 1.3.1 patch for Windows
(get it at https://archive.org/details/Doom_3_1.3.1) and then get the files from the
installation directory (or copy dhewm3 in there).

 ... from Steam
 --------------
First you need to buy Doom3 of course (https://store.steampowered.com/app/208200/DOOM_3/).
It also includes the Resurrection of Evil Addon (and the BFG Edition re-release of the game,
which is not supported by dhewm3).

Thankfully the game from Steam is already fully patched to 1.3.1.

On Windows, just install it and copy the game data from there (see above for what files are needed).

 ... using the Doom3 Demo gamedata
 ---------------------------------
First you need to download the Doom3 Demo, of course.
For Windows, you can get D3Demo.exe at Fileplanet and just run the installer:
https://www.fileplanet.com/archive/p-15998/DOOM-3-Demo

You'll need `demo/demo00.pk4` for playing, either in exactly that directory, or you can copy it
into `base/`. Note that you don't need the patches of the full version, adding their pk4s when
using Demo gamedata breaks the game.

`demo00.pk4` has a size of 462MB but there are minimal differences between the one
for Linux and Windows, so they have different md5sums:
Windows : md5sum bd410abbb649b9512d65b794869df9fe size 483534533 Bytes
Linux   : md5sum 70c2c63ef1190158f1ebd6c255b22d8e size 483535485 Bytes

Both work on all platforms though, but the Linux demo had been released two months after
the Demo for Windows and contains at least one tiny bugfix: A missing string for " or "
which is used when binding multiple keys to the same action - the Windows Demo shows
a nonsense string instead.

(You can download the Linux demo at 
 https://files.holarse-linuxgaming.de/native/Spiele/Doom%203/Demo/doom3-linux-1.1.1286-demo.x86.run
 and, in a POSIX compatible shell with `tar` installed, extract the relevant file from the .run
 installer with `sh doom3-linux-1.1.1286-demo.x86.run --tar xf demo/`)


 Configuration
===============

See Configuration.txt for dhewm3-specific configuration, especially for 
using gamepads.

 GENERAL NOTES
===============

# Back End Rendering of Stencil Shadows

The Doom 3 GPL source code release **did** not include functionality enabling rendering
of stencil shadows via the "depth fail" method, a functionality commonly known as
"Carmack's Reverse".  
It has been restored in dhewm3 1.5.1 after Creative Labs' patent finally expired
(https://patents.google.com/patent/US6384822B1/en).

Note that this did not change the visual appearance of the game, and didn't seem to
make a noticeable performance difference (on halfway-recent hardware) either.

# MayaImport

The code for our Maya export plugin is included, if you are a Maya licensee
you can obtain the SDK from Autodesk.


 LICENSE
=========

See LibLicenses.txt for licenses of bundled libraries

See COPYING.txt for the GNU GENERAL PUBLIC LICENSE

ADDITIONAL TERMS:  The Doom 3 GPL Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU GPL which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

EXCLUDED CODE:  The code described below and contained in the Doom 3 GPL Source Code release is not part of the Program covered by the GPL and is expressly excluded from its terms.  You are solely responsible for obtaining from the copyright holder a license for such code and complying with the applicable license terms.

## Dear ImGui

neo/libs/imgui/*

The MIT License (MIT)

Copyright (c) 2014-2024 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


## PropTree

neo/tools/common/PropTree/*

Copyright (C) 1998-2001 Scott Ramsay

sramsay@gonavi.com

http://www.gonavi.com

This material is provided "as is", with absolutely no warranty expressed
or implied. Any use is at your own risk.

Permission to use or copy this software for any purpose is hereby granted
without fee, provided the above notices are retained on all copies.
Permission to modify the code and to distribute modified code is granted,
provided the above notices are retained, and a notice that the code was
modified is included with the above copyright notice.

If you use this code, drop me an email.  I'd like to know if you find the code
useful.


## Base64 implementation

neo/idlib/Base64.cpp

Copyright (c) 1996 Lars Wirzenius.  All rights reserved.

June 14 2003: TTimo <ttimo@idsoftware.com>

modified + endian bug fixes

http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=197039

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.


## miniz

src/framework/miniz/*

The MIT License (MIT)

Copyright 2013-2014 RAD Game Tools and Valve Software
Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC

All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


## IO on .zip files using minizip

src/framework/minizip/*

Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

Modifications of Unzip for Zip64
Copyright (C) 2007-2008 Even Rouault

Modifications for Zip64 support
Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.


## MD4 Message-Digest Algorithm

neo/idlib/hashing/MD4.cpp

Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD4 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD4 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.


## MD5 Message-Digest Algorithm

neo/idlib/hashing/MD5.cpp

This code implements the MD5 message-digest algorithm.
The algorithm is due to Ron Rivest.  This code was
written by Colin Plumb in 1993, no copyright is claimed.
This code is in the public domain; do with it what you wish.


## CRC32 Checksum

neo/idlib/hashing/CRC32.cpp

Copyright (C) 1995-1998 Mark Adler

## stb_image and stb_vorbis

neo/renderer/stb_image.h
neo/sound/stb_vorbis.h

Used to decode JPEG and OGG Vorbis files.

from https://github.com/nothings/stb/

Copyright (c) 2017 Sean Barrett

Released under MIT License and Unlicense (Public Domain)
