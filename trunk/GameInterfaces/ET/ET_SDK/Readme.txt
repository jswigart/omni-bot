Wolfenstein™: Enemy Territory™ Source Code

This is the source code for Wolfenstein: Enemy Territory.  This source code is not officially supported in any capacity by id Software, Activision or Splash Damage.  Below you will find very basic instructions on how to get the source code working so that you can create your own Enemy Territory mods.  For online help and resources, please visit www.castlewolfenstein.com or www.splashdamage.com 

Windows:
========

The first thing you have to do is run the installer.  Do this by double-clicking on the WET_Source.exe.  By default, this will install the source code to the C:\WET_Source folder.

To use the source, you must have Visual C++ version 6 or greater installed.  The source has been found to work with .NET edition of Visual C++, but Visual C++ 6 is recommended for better compatibility.

1. Open the src folder.
2. Double-click on the wolf.dsw file to open the project workspace.
3. In the build menu select the “Set Active Configurations” option.
a. Note: for .NET this will appear as “Configuration Manager”.
4. Select Win32 Release for all versions of the code.
5. At this point, you should be able to compile the code.

Linux:
======

The first thing you have to do is run the installer.
Do this by executing et-linux-2.60-sdk.x86.run.  By default, this will install the source code to the /usr/local/games/enemy-territory-sdk folder.

To use the source, you must have gcc 2.95 or greater installed. You must also have scons[1] installed.

1. Type the following commands at a command prompt:
   cd /usr/local/games/enemy-territory-sdk
   scons -c
   scons
2. After a few minutes of compiling, you should have cgame, qagame and ui .so files in your directory.

By default, a 'debug' build is created, which includes full debugging symbols suitable for use with gdb.
To make a non-debug 'release' build, pass the parameter BUILD=release to scons, eg:
   scons BUILD=release

You can use the included cross-scons.sh script to build win32 dlls on linux via mingw[2]. You will
need to edit the script to point to the location of your mingw install and mingw binary directories.
Place the cross-scons.sh script someplace in your PATH, such as /usr/local/bin. To build win32 dlls
you would then type 'cross-scons.sh' in place of 'scons'.

[1] http://www.scons.org/
[2] http://www.mingw.org/

Mac:
====

Unpack et-mac-2.60-sdk.zip with stuffit or other archiver that supports pkzip archives.

To use the source, you must have gcc 2.95 or greater installed. You must also have scons[1] installed.
In order for the build scripts to create compressed bundles, you will need pkzip[2] or p7zip[3] installed.
You can get premade osx packages for these utilities from the fink[4] project.

1. Type the following commands at a command prompt:
   cd /place/you/unpacked/the/sdk/to
   scons -c
   scons
2. After a few minutes of compiling, you should have cgame, qagame and ui .so files in your directory.

By default, a 'debug' build is created, which includes full debugging symbols suitable for use with gdb.
To make a non-debug 'release' build, pass the parameter BUILD=debug to scons, eg:
   scons BUILD=release

Mac osx requires shared libraries to be packaged into "bundles". This can be done automatically as part of
the build process. To build bundles, pass the parameter BUILDBUNDLE=1 to scons, eg:
   scons BUILDBUNDLE=1
cgame_mac and ui_mac are compressed bundles that go into your mod's pk3, and qagame_mac.bundle goes
into the Wolfenstein ET.app/Contents/Resources/ directory on the server.

[1] http://www.scons.org/
[2] http://www.pkzip.com/
[3] http://p7zip.sourceforge.net/
[4] http://fink.sourceforge.net/
