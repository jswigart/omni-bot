# -*- mode: python -*-
# Quake4 build script
# TTimo <ttimo@idsoftware.com>
# http://scons.sourceforge.net

import sys, os, time, commands, re, pickle, StringIO, popen2, commands, pdb, zipfile, string
import SCons

sys.path.append( 'sys/scons' )
import scons_utils

conf_filename='site.conf'
# choose configuration variables which should be saved between runs
# ( we handle all those as strings )
serialized=['CC', 'CXX', 'JOBS', 'BUILD', 'IDNET_HOST', 'GL_HARDLINK', 'DEDICATED',
	'DEBUG_MEMORY', 'LIBC_MALLOC', 'ID_NOLANADDRESS', 'ID_MCHECK',
	'TARGET_CORE', 'TARGET_CORE_SMP', 'TARGET_GAME', 'TARGET_SPGAME', 'TARGET_MPGAME', 'TARGET_MONO', 'TARGET_MONO_IS_SP', 'TARGET_DEMO', 'NOCURL',
	'BUILD_ROOT', 'Q4TEST', 'TARGET_GAMEPAK', 'OSX_BUILDSTYLE', 'SILENT', 'GCC_X86_ASM' ]

# global build mode ------------------------------

g_sdk = not os.path.exists( 'sys/scons/SConscript.core' )

# ------------------------------------------------

# help -------------------------------------------

help_string = """
Usage: scons [OPTIONS] [TARGET] [CONFIG]

[OPTIONS] and [TARGET] are covered in command line options, use scons -H

[CONFIG]: KEY="VALUE" [...]
a number of configuration options saved between runs in the """ + conf_filename + """ file
erase """ + conf_filename + """ to start with default settings again

CC (default gcc)
CXX (default g++)
	Specify C and C++ compilers (defaults gcc and g++)
	ex: CC="gcc-4.1"
	You can use ccache and distcc, for instance:
	CC="ccache distcc gcc" CXX="ccache distcc g++"

JOBS (default 1)
	Parallel build

BUILD (default debug)
	Use debug-all/debug/release to select build settings
	ex: BUILD="release"
	debug-all: no optimisations, debugging symbols
	debug: -O -g
	release: all optimisations, including CPU target etc.
	test: release with debug symbols and all optimizations except omitted frame pointers

BUILD_ROOT (default 'build')
	change the build root directory

NOCONF (default 0, not saved)
	ignore site configuration and use defaults + command line only

SILENT ( default 0, saved )
	hide the compiler output, unless error

GCC_X86_ASM ( defaul 0, saved )
	compile in gcc x86 asm optimizations
"""

if ( not g_sdk ):
	help_string += """
DEDICATED (default 0)
	Control regular / dedicated type of build:
	0 - client
	1 - dedicated server
	2 - both

TARGET_CORE (default 1)
	Build the core

TARGET_CORE_SMP (default 0)
	Build an SMP-enabled core

TARGET_GAME (default 1)
	Build the singleplayer and multiplayer game code

TARGET_SPGAME (default 0)
	Build the singleplayer game code

TARGET_MPGAME (default 0)
	Build the multiplayer game code

TARGET_MONO (default 0)
	Build a monolithic binary

TARGET_MONO_IS_SP (default 0)
	Build the monolithic binary as singleplayer instead of multiplayer

TARGET_DEMO (default 0)
	Build demo client ( both a core and game, no mono )
	NOTE: if you *only* want the demo client, set TARGET_CORE and TARGET_GAME to 0

IDNET_HOST (default to source hardcoded)
	Override builtin IDNET_HOST with your own settings
	
GL_HARDLINK (default 0)
	Instead of dynamically loading the OpenGL libraries, use implicit dependencies
	NOTE: no GL logging capability and no r_glDriver with GL_HARDLINK 1

DEBUG_MEMORY (default 0)
	Enables memory logging to file
	
LIBC_MALLOC (default 1)
	Toggle idHeap memory / libc malloc usage
	When libc malloc is on, memory size statistics are wrong ( no _msize )

ID_NOLANADDRESS (default 0)
	Don't recognize any IP as LAN address. This is useful when debugging network
	code where LAN / not LAN influences application behaviour
	
ID_MCHECK (default 2)
	Perform heap consistency checking
	0: on in Debug / off in Release
	1 forces on, 2 forces off
	note that idlib has it's own block allocator/checking
	this should not be considered a replacement, but an additional tool
	note: this is the same as MALLOC_CHECK_, but different from mtrace

SETUP_TAGGED (default 0, not saved)
	build tagged binaries distribution implies release, excludes other setups

SETUP_DEDICATED (default 0, not saved)
	build dedicated server setup. implies release

SETUP_DEMO (default 0, not saved)
	build demo setup. implies release

SETUP_FULL (default 0, not saved)
	build full setup. implies release

SETUP_INCREMENTAL (default 0, not saved)
	builds the incremental setup

TARGET_GAMEPAK (default 0, not saved)
    build a game pak pk4
	if no setup is scheduled, from whatever game is being compiled ( current configuration )
	if setups are scheduled, controls gamepak building during setup ( might wanna use a reference one for pure-compatible updates )

SDK (default 0, not saved)
	build an SDK release

NOCURL (default 0)
	set to 1 to disable usage of libcurl and http/ftp downloads feature

FIX_INCLUDE (default 0, not saved)
	fix include paths while compiling
	wraps around the compiler call to catch and fix include path errors
	note that since this process modifies files on the fly, it's not a good idea to use it with several jobs

FIX_SUPER (default 0, not saved)
	fix usage of __super msvc-ism
	wraps around gcc to do a search and fix pass

Q4TEST (default 0)
	q4test build
	MP-only gamecode, enables binary tagging

ASSETS (optional, not saved)
	point to the setup assets directory

OSX_BUILDSTYLE (default 0)
	Styles 1 & 2 will override CC and CXX only if they are left to the defaults
	0 - Uses the system gcc/include/libs
	1 - Uses GCC 3.3 + 10.3.9 SDK
	2 - Uses GCC 4.0 + 10.4u SDK
"""

Help( help_string )

# end help ---------------------------------------

# sanity -----------------------------------------

EnsureSConsVersion( 0, 96 )

# end sanity -------------------------------------

# system detection -------------------------------

# OS and CPU
OS = commands.getoutput( 'uname -s' )
if ( OS == 'Linux' ):
	cpu = commands.getoutput( 'uname -m' )
	if ( cpu == 'i686' ):
		cpu = 'x86'
	else:
		cpu = 'cpu'
elif ( OS == 'Darwin' ):
	cpu = commands.getoutput( 'uname -m' )
	if ( cpu == 'Power Macintosh' ):
		cpu = 'ppc'
	else:
		cpu = 'cpu'

# end system detection ---------------------------

# default settings -------------------------------

CC = 'gcc-4.1'
CXX = 'g++-4.1'
JOBS = '1'
BUILD = 'debug'
DEDICATED = '0'
TARGET_CORE = '1'
TARGET_GAME = '1'
TARGET_SPGAME = '0'
TARGET_MPGAME = '0'
TARGET_GAMEPAK = '0'
TARGET_MONO = '0'
TARGET_MONO_IS_SP = '0'
TARGET_DEMO = '0'
IDNET_HOST = ''
GL_HARDLINK = '0'
if ( OS == 'Darwin' ):
	GL_HARDLINK = '1'
DEBUG_MEMORY = '0'
LIBC_MALLOC = '1'
ID_NOLANADDRESS = '0'
ID_MCHECK = '2'
BUILD_ROOT = 'build'
SETUP_TAGGED = '0'
SETUP_DEDICATED = '0'
SETUP_DEMO = '0'
SETUP_FULL = '0'
SETUP_INCREMENTAL = '0'
SETUP = '0'	# no cmdline control, will be set to 1 if any form of setup is requested
SDK = '0'
NOCONF = '0'
NOCURL = '0'
FIX_INCLUDES = '0'
FIX_SUPER = '0'
Q4TEST = '0'
ASSETS = ''
OSX_BUILDSTYLE = '0'
SILENT = '0'
TARGET_CORE_SMP = '0'
GCC_X86_ASM = '0'

# end default settings ---------------------------

# site settings ----------------------------------

if ( not ARGUMENTS.has_key( 'NOCONF' ) or ARGUMENTS['NOCONF'] != '1' ):
	site_dict = {}
	if (os.path.exists(conf_filename)):
		site_file = open(conf_filename, 'r')
		p = pickle.Unpickler(site_file)
		site_dict = p.load()
		print 'Loading build configuration from ' + conf_filename + ':'
		for k, v in site_dict.items():
			exec_cmd = k + '=\'' + v + '\''
			print '  ' + exec_cmd
			exec(exec_cmd)
else:
	print 'Site settings ignored'

# end site settings ------------------------------

# command line settings --------------------------

for k in ARGUMENTS.keys():
	exec_cmd = k + '=\'' + ARGUMENTS[k] + '\''
	print 'Command line: ' + exec_cmd
	exec( exec_cmd )

# end command line settings ----------------------

# save site configuration ----------------------

if ( not ARGUMENTS.has_key( 'NOCONF' ) or ARGUMENTS['NOCONF'] != '1' ):
	for k in serialized:
		exec_cmd = 'site_dict[\'' + k + '\'] = ' + k
		exec(exec_cmd)

	site_file = open(conf_filename, 'w')
	p = pickle.Pickler(site_file)
	p.dump(site_dict)
	site_file.close()

# end save site configuration ------------------

# configuration rules --------------------------

if ( SETUP_TAGGED != '0' or SETUP_DEDICATED != '0' or SETUP_DEMO != '0' or SETUP_FULL != '0' or SETUP_INCREMENTAL != '0' ):
	DEDICATED	= '2'
	BUILD		= 'release'
	SETUP		= '1'
	TARGET_GAME = '1'
	TARGET_CORE = '1'
	TARGET_CORE_SMP = '1'
	TARGET_GAMEPAK = '0'

if ( TARGET_GAMEPAK == '1' ):
	TARGET_GAME = '1'

if ( SETUP != '0' ):
	if ( SETUP_TAGGED != '0' ):
		SETUP_DEDICATED = '0'
		SETUP_DEMO = '0'
		SETUP_FULL = '0'
		Q4TEST = '1'
	else:
		Q4TEST = '0'

if ( g_sdk ):
	TARGET_CORE = '0'
	TARGET_CORE_SMP = '0'
	TARGET_MONO = '0'
	TARGET_DEMO = '0'

if ( SDK != '0' ):
	DEDICATED = '0'
	TARGET_CORE = '0'
	TARGET_CORE_SMP = '0'
	TARGET_GAME = '0'
	TARGET_MPGAME = '0'
	TARGET_SPGAME = '0'
	TARGET_MONO = '0'
	TARGET_DEMO = '0'

if ( TARGET_GAME == '1' ):
	TARGET_MPGAME = '1'
	TARGET_SPGAME = '1'

if ( BUILD == 'test' ):
	print 'WARNING: compiling a release build in test configuration'

# end configuration rules ----------------------

# general configuration, target selection --------

g_build = BUILD_ROOT + '/' + BUILD

SConsignFile( 'scons.signatures' )

if ( GL_HARDLINK != '0' ):
	g_build += '-hardlink'

if ( DEBUG_MEMORY != '0' ):
	g_build += '-debugmem'
	
if ( LIBC_MALLOC != '1' ):
	g_build += '-nolibcmalloc'

if ( Q4TEST != '0' ):
	g_build += '-q4test'

SetOption('num_jobs', JOBS)

LINK = CXX

# common flags
# BASE + CORE + OPT for engine
# BASE + GAME + OPT for game
# _noopt versions of the environements are built without the OPT

BASECPPFLAGS = [ ]
CORECPPPATH = [ ]
CORELIBPATH = [ ]
CORECPPFLAGS = [ ]
GAMECPPFLAGS = [ ]
BASELINKFLAGS = [ ]
CORELINKFLAGS = [ ]

# for release build, further optimisations that may not work on all files
OPTCPPFLAGS = [ ]

BASECPPFLAGS.append( '-pipe' )
# warn all
BASECPPFLAGS.append( '-Wall' )
# don't wrap gcc messages
BASECPPFLAGS.append( '-fmessage-length=0' )

if ( OS == 'Linux' ):
	# gcc 4.x option only - only export what we mean to from the game SO
	BASECPPFLAGS.append( '-fvisibility=hidden' )
	# get the 64 bits machine on the distcc array to produce 32 bit binaries :)
	BASECPPFLAGS.append( '-m32' )
	BASELINKFLAGS.append( '-m32' )

if ( g_sdk or SDK != '0' ):
	BASECPPFLAGS.append( '-DQ4SDK' )

if ( Q4TEST == '1' ):
	# _MPBETA implie ID_TAGGED_BUILD but also disables some single player functionality
	#BASECPPFLAGS.append( '-D_MPBETA' )
	BASECPPFLAGS.append( '-DID_TAGGED_BUILD' )

if ( OS == 'Darwin' ):
	# a few more common defines
	BASECPPFLAGS += [ '-Wno-long-double', '-arch', 'ppc', '-fasm-blocks', '-fpascal-strings', '-faltivec', '-mcpu=G5', '-mtune=G5' ]
	BASECPPFLAGS += [ '-DMACOS_X' ]
	BASECPPFLAGS += [ '-Wno-unknown-pragmas' ]
	BASECPPFLAGS += [ '-DMAC_OS_X_VERSION_MIN_REQUIRED=1030' ]
	# Override CC & CXX only if they contain the default values. Allows for distcc invocations
	if ( OSX_BUILDSTYLE == '1' ):
		if ( CC == 'gcc' ):
			CC = [ '/usr/bin/gcc-3.3' ]
		if ( CXX == 'g++' ):
			CXX = [ '/usr/bin/g++-3.3' ]

		BASECPPFLAGS += [ '-isystem', '/Developer/SDKs/MacOSX10.3.9.sdk/usr/include/gcc/darwin/3.3' ]
		BASECPPFLAGS += [ '-I/Developer/SDKs/MacOSX10.3.9.sdk/usr/include/gcc/darwin/3.3/c++' ]
		BASECPPFLAGS += [ '-I/Developer/SDKs/MacOSX10.3.9.sdk/usr/include/gcc/darwin/3.3/c++/ppc-darwin' ]
		BASECPPFLAGS += [ '-isystem', '/Developer/SDKs/MacOSX10.3.9.sdk/usr/include' ]
		BASELINKFLAGS += [ '-Wl,-syslibroot,/Developer/SDKs/MacOSX10.3.9.sdk' ] 
		CORELIBPATH += [ '/Developer/SDKs/MacOSX10.3.9.sdk/usr/lib' ]
		os.environ['NEXT_ROOT'] = '/Developer/SDKs/MacOSX10.3.9.sdk'
		os.environ['MACOSX_DEPLOYMENT_TARGET'] = '10.3' 
	elif ( OSX_BUILDSTYLE == '2' ):
		if ( CC == 'gcc' ):
			CC = [ '/usr/bin/gcc-4.0' ]
		if ( CXX == 'g++' ):
			CXX = [ '/usr/bin/g++-4.0' ]

		BASECPPFLAGS += [ '-isystem', '/Developer/SDKs/MacOSX10.4u.sdk/usr/include/gcc/darwin/4.0' ]
		BASECPPFLAGS += [ '-mone-byte-bool' ]
		BASECPPFLAGS += [ '-fvisibility-inlines-hidden' ]
		BASECPPFLAGS += [ '-fpermissive' ]
		BASECPPFLAGS += [ '-I/Developer/SDKs/MacOSX10.4u.sdk/usr/include/gcc/darwin/4.0/c++' ]
		BASECPPFLAGS += [ '-I/Developer/SDKs/MacOSX10.4u.sdk/usr/include/gcc/darwin/4.0/c++/ppc-darwin' ]
		BASECPPFLAGS += [ '-isystem', '/Developer/SDKs/MacOSX10.4u.sdk/usr/include' ]
		BASELINKFLAGS += [ '-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk' ] 
		CORELIBPATH += [ '/Developer/SDKs/MacOSX10.4u.sdk/usr/lib' ]
		os.environ['NEXT_ROOT'] = '/Developer/SDKs/MacOSX10.4u.sdk'
		os.environ['MACOSX_DEPLOYMENT_TARGET'] = '10.3' 

if ( BUILD == 'debug-all' ):
	BASECPPFLAGS.append( '-g' )
	BASECPPFLAGS.append( '-D_DEBUG' )
	if ( ID_MCHECK == '0' ):
		ID_MCHECK = '1'
elif ( BUILD == 'debug' ):
	BASECPPFLAGS.append( '-g' )
	BASECPPFLAGS.append( '-O1' )
	BASECPPFLAGS.append( '-D_DEBUG' )
	if ( ID_MCHECK == '0' ):
		ID_MCHECK = '1'
elif ( BUILD == 'test' ):
	BASECPPFLAGS.append( '-g' )
	BASECPPFLAGS.append( '-D_FINAL' )
	BASECPPFLAGS.append( '-D_TEST' )
	if ( OS == 'Linux' ):
		# Don't omit frame pointers in the test build
		OPTCPPFLAGS = [ '-O3', '-march=pentium3', '-Winline', '-ffast-math', '-fno-unsafe-math-optimizations' ]
		if ( ID_MCHECK == '0' ):
			ID_MCHECK = '2'
	elif ( OS == 'Darwin' ):
		OPTCPPFLAGS = [ '-O3', '-falign-functions=16', '-falign-loops=16', '-finline' ]
elif ( BUILD == 'release' ):
	BASECPPFLAGS.append( '-D_FINAL' )
	if ( OS == 'Linux' ):
		# -fomit-frame-pointer: "-O also turns on -fomit-frame-pointer on machines where doing so does not interfere with debugging."
		#   on x86 have to set it explicitely
		# -finline-functions: implicit at -O3
		# -fschedule-insns2: implicit at -O2
		# no-unsafe-math-optimizations: that should be on by default really. hit some wonko bugs in physics code because of that
		OPTCPPFLAGS = [ '-O3', '-march=pentium3', '-Winline', '-ffast-math', '-fno-unsafe-math-optimizations', '-fomit-frame-pointer' ]
		if ( ID_MCHECK == '0' ):
			ID_MCHECK = '2'
	elif ( OS == 'Darwin' ):
		OPTCPPFLAGS = [ '-O3', '-falign-functions=16', '-falign-loops=16', '-finline' ]
else:
	print 'Unknown build configuration ' + BUILD
	sys.exit(0)

if ( GL_HARDLINK != '0' ):
	CORECPPFLAGS.append( '-DID_GL_HARDLINK' )

if ( DEBUG_MEMORY != '0' ):
	BASECPPFLAGS += [ '-DID_DEBUG_MEMORY', '-DID_REDIRECT_NEWDELETE' ]
	
if ( LIBC_MALLOC != '1' ):
	BASECPPFLAGS.append( '-DUSE_LIBC_MALLOC=0' )

if ( len( IDNET_HOST ) ):
	CORECPPFLAGS.append( '-DIDNET_HOST=\\"%s\\"' % IDNET_HOST )

if ( ID_NOLANADDRESS != '0' ):
	CORECPPFLAGS.append( '-DID_NOLANADDRESS' )
	
if ( ID_MCHECK == '1' ):
	BASECPPFLAGS.append( '-DID_MCHECK' )

# create the build environements

if ( FIX_INCLUDES == '1' ):
	CC = './sys/scons/fixincludes.py \'' + CC + '\''
	CXX = './sys/scons/fixincludes.py \'' + CXX + '\''

if ( FIX_SUPER == '1' ):
	CC = './sys/scons/fixsuper.py \'' + CC + '\''
	CXX = './sys/scons/fixsuper.py \'' + CXX + '\''

g_base_env = Environment( ENV = os.environ, CC = CC, CXX = CXX, LINK = LINK, CPPFLAGS = BASECPPFLAGS, LINKFLAGS = BASELINKFLAGS, CPPPATH = CORECPPPATH, LIBPATH = CORELIBPATH, OS = OS )
scons_utils.SetupUtils( g_base_env )
g_base_env.Append( CXXFLAGS = [ '-Wno-invalid-offsetof' ] )

g_env = g_base_env.Copy()

g_env['CPPFLAGS'] += OPTCPPFLAGS
g_env['CPPFLAGS'] += CORECPPFLAGS
g_env['LINKFLAGS'] += CORELINKFLAGS

if ( BUILD != 'release' and BUILD != 'test' ):
	g_env_noopt = g_env.Copy()
else:
	g_env_noopt = g_base_env.Copy()
	g_env_noopt['CPPFLAGS'] += CORECPPFLAGS
#	g_env_noopt.Append( CPPFLAGS = '-O1' )
	g_env_noopt['LINKFLAGS'] += CORELINKFLAGS

g_game_env = g_base_env.Copy()
g_game_env['CPPFLAGS'] += OPTCPPFLAGS
g_game_env['CPPFLAGS'] += GAMECPPFLAGS

# maintain this dangerous optimization off at all times
g_env.Append( CPPFLAGS = '-fno-strict-aliasing' )
g_env_noopt.Append( CPPFLAGS = '-fno-strict-aliasing' )
g_game_env.Append( CPPFLAGS = '-fno-strict-aliasing' )

if ( int(JOBS) > 1 ):
	print 'Using buffered process output'
	silent = False
	if ( SILENT == '1' ):
		silent = True
	scons_utils.SetupBufferedOutput( g_env, silent )
	scons_utils.SetupBufferedOutput( g_game_env, silent )

# mark the globals

local_dedicated = 0
# 0 for monolithic build
local_gamedll = 1
# carry around rather than using .a, avoids binutils bugs
idlib_objects = []
game_objects = []
local_demo = 0
# curl usage. there is a global toggle flag
local_curl = 0
curl_lib = []
# if idlib should produce PIC objects ( depending on core or game inclusion )
local_idlibpic = 0
eventdefs = None
# compile for SMP ( affects idlib and core )
local_smp = 0
idsdl_info = []
local_mpgame = 0

GLOBALS = 'g_env g_env_noopt g_game_env OS ID_MCHECK idlib_objects game_objects local_dedicated local_gamedll local_demo local_idlibpic curl_lib local_curl local_smp idsdl_info local_mpgame eventdefs GL_HARDLINK NOCURL Q4TEST OSX_BUILDSTYLE TARGET_CORE_SMP BUILD GCC_X86_ASM'

# end general configuration ----------------------

# targets ----------------------------------------

Export( 'GLOBALS ' + GLOBALS )

quake4	= None
q4ded	= None
game	= None
mpgame	= None
q4_mon	= None

Default( None )

# build curl if needed
if ( NOCURL == '0' and ( TARGET_CORE == '1' or TARGET_MONO == '1' or TARGET_CORE_SMP == '1' ) ):
	# 1: debug, 2: release
	if ( BUILD == 'release' or BUILD == 'test' ):
		local_curl = 2
	else:
		local_curl = 1
	Export( 'GLOBALS ' + GLOBALS )
	curl_lib = SConscript( 'sys/scons/SConscript.curl' )

# build our custom SDL library if needed
if ( TARGET_CORE_SMP == '1' ):
	Export( 'GLOBALS ' + GLOBALS )
	BuildDir( g_build + '/sdl', '.', duplicate = 0 )
	idsdl_info = SConscript( 'sys/scons/SConscript.idsdl' )

if ( TARGET_CORE_SMP == '1' ):
	local_gamedll = 1
	local_demo = 0
	local_idlibpic = 0
	local_dedicated = 0
	local_smp = 1
	Export( 'GLOBALS ' + GLOBALS )
	BuildDir( g_build + '/core-smp/glimp', '.', duplicate = 1 )
	SConscript( g_build + '/core-smp/glimp/sys/scons/SConscript.gl' )
	BuildDir( g_build + '/core-smp', '.', duplicate = 0 )
	idlib_objects = SConscript( g_build + '/core-smp/sys/scons/SConscript.idlib' )
	Export( 'GLOBALS ' + GLOBALS ) # update idlib_objects
	quake4smp = SConscript( g_build + '/core-smp/sys/scons/SConscript.core' )

	if ( BUILD != 'test' ):
		quake4smp = InstallAs( '#quake4smp.%s' % cpu, quake4smp )

	if ( OS == 'Linux' ):
		Default( quake4smp )
	
if ( TARGET_CORE == '1' ):
	local_gamedll = 1
	local_demo = 0
	local_idlibpic = 0
	local_smp = 0
	if ( DEDICATED == '0' or DEDICATED == '2' ):
		local_dedicated = 0
		Export( 'GLOBALS ' + GLOBALS )
		
		BuildDir( g_build + '/core/glimp', '.', duplicate = 1 )
		SConscript( g_build + '/core/glimp/sys/scons/SConscript.gl' )
		BuildDir( g_build + '/core', '.', duplicate = 0 )
		idlib_objects = SConscript( g_build + '/core/sys/scons/SConscript.idlib' )
		Export( 'GLOBALS ' + GLOBALS ) # update idlib_objects
		quake4 = SConscript( g_build + '/core/sys/scons/SConscript.core' )

		if ( BUILD != 'test' ):
			quake4 = InstallAs( '#quake4.%s' % cpu, quake4 )

		if ( OS == 'Linux' ):
			Default( quake4 )
		
	if ( DEDICATED == '1' or DEDICATED == '2' ):
		local_dedicated = 1
		Export( 'GLOBALS ' + GLOBALS )
		
		BuildDir( g_build + '/dedicated/glimp', '.', duplicate = 1 )
		SConscript( g_build + '/dedicated/glimp/sys/scons/SConscript.gl' )
		BuildDir( g_build + '/dedicated', '.', duplicate = 0 )
		idlib_objects = SConscript( g_build + '/dedicated/sys/scons/SConscript.idlib' )
		Export( 'GLOBALS ' + GLOBALS )
		q4ded = SConscript( g_build + '/dedicated/sys/scons/SConscript.core' )

		if ( BUILD != 'test' ):
			q4ded = InstallAs( '#q4ded.%s' % cpu, q4ded )

		if ( OS == 'Linux' ):
			Default( q4ded )

if ( TARGET_SPGAME == '1' ):
	local_gamedll = 1
	local_demo = 0
	local_dedicated = 0
	local_idlibpic = 1
	Export( 'GLOBALS ' + GLOBALS )
	BuildDir( g_build + '/game', '.', duplicate = 0 )
	idlib_objects = SConscript( g_build + '/game/sys/scons/SConscript.idlib' )
	local_mpgame = 0
	Export( 'GLOBALS ' + GLOBALS )
	game = SConscript( g_build + '/game/sys/scons/SConscript.game' )

	if ( BUILD != 'test' ):
		if ( OS == 'Darwin' ):
			game = InstallAs( '#spgame.so' , game )
		else:
			game = InstallAs( '#spgame%s.so' % cpu, game )

	Default( game )

if ( TARGET_MPGAME == '1' ):
	local_gamedll = 1
	local_demo = 0
	local_dedicated = 0
	local_idlibpic = 1
	Export( 'GLOBALS ' + GLOBALS )
	BuildDir( g_build + '/mpgame', '.', duplicate = 0 )
	idlib_objects = SConscript( g_build + '/mpgame/sys/scons/SConscript.idlib' )
	local_mpgame = 1
	Export( 'GLOBALS ' + GLOBALS )
	mpgame = SConscript( g_build + '/mpgame/sys/scons/SConscript.game' )

	if ( BUILD != 'test' ):
		if ( OS == 'Darwin' ):
			mpgame = InstallAs( '#mpgame.so' , mpgame )
		else:
			mpgame = InstallAs( '#mpgame%s.so' % cpu, mpgame )

	Default( mpgame )
	
if ( TARGET_MONO == '1' ):
	# the game in a single piece
	local_gamedll = 0
	local_dedicated = 0
	local_demo = 0
	local_idlibpic = 0
	if ( TARGET_MONO_IS_SP == '1' ):
		local_mpgame = 0
	else:
		local_mpgame = 1
	if ( DEDICATED == '0' or DEDICATED == '2' ):
		Export( 'GLOBALS ' + GLOBALS )
		BuildDir( g_build + '/mono/glimp', '.', duplicate = 1 )
		SConscript( g_build + '/mono/glimp/sys/scons/SConscript.gl' )
		BuildDir( g_build + '/mono', '.', duplicate = 0 )
		idlib_objects = SConscript( g_build + '/mono/sys/scons/SConscript.idlib' )
		game_objects = SConscript( g_build + '/mono/sys/scons/SConscript.game' )
		Export( 'GLOBALS ' + GLOBALS )
		q4_mono = SConscript( g_build + '/mono/sys/scons/SConscript.core' )

		if ( BUILD != 'test' ):
			q4_mono = InstallAs( '#q4mono.%s' % cpu, q4_mono )

		if ( OS == 'Linux' ):
			Default( q4_mono )
	
	if ( DEDICATED == '1' or DEDICATED == '2' ):
		local_dedicated = 1
		Export( 'GLOBALS ' + GLOBALS )
		BuildDir( g_build + '/monoded/glimp', '.', duplicate = 1 )
		SConscript( g_build + '/monoded/glimp/sys/scons/SConscript.gl' )
		BuildDir( g_build + '/monoded', '.', duplicate = 0 )
		idlib_objects = SConscript( g_build + '/monoded/sys/scons/SConscript.idlib' )
		game_objects = SConscript( g_build + '/monoded/sys/scons/SConscript.game' )
		Export( 'GLOBALS ' + GLOBALS )
		q4_monoded = SConscript( g_build + '/monoded/sys/scons/SConscript.core' )

		if ( BUILD != 'test' ):
			q4_monoded = InstallAs( '#q4monoded.%s' % cpu, q4_monoded )

		if ( OS == 'Linux' ):
			Default( q4_monoded )

if ( OS == 'Darwin' ):
	src = []
	if ( TARGET_CORE == '1' ):
		if ( DEDICATED == '0' or DEDICATED == '2' ):
			src.append( quake4 )
		if ( DEDICATED == '1' or DEDICATED == '2' ):
			src.append( q4ded )
	if ( TARGET_MONO == '1' ):
		if ( DEDICATED == '0' or DEDICATED == '2' ):
			src.append( q4_mono )
		if ( DEDICATED == '1' or DEDICATED == '2' ):
			src.append( q4_monoded )
	if ( len( src ) ):
		q4mac = Command( 'q4mac', src, Action( g_env.BuildBundle ) )
		Default( q4mac )	

if ( SETUP == '1' ):
	brandelf = Program( 'brandelf', 'sys/linux/setup/brandelf.c' )
	setup_source = [ brandelf, quake4, q4ded, game, quake4smp ]
	do_gamepak = ( TARGET_GAMEPAK != '0' )
	setups = []
	if ( SETUP_TAGGED == '1' ):
		g_env_tagged = g_env.Copy()
		g_env_tagged.Prepare( do_gamepak, ASSETS )
		setup_tagged = Command( 'setup_tagged', setup_source, Action( g_env_tagged.BuildSetup ) )
		Default( setup_tagged )
		setups.append( setup_tagged )
	if ( SETUP_DEMO == '1' ):
		g_env_demo = g_env.Copy()
		g_env_demo.Prepare( do_gamepak, ASSETS )
		setup_demo = Command( 'setup_demo', setup_source, Action( g_env_demo.BuildSetup ) )
		Default( setup_demo )
		setups.append( setup_demo )
	if ( SETUP_DEDICATED == '1' ):
		g_env_ded = g_env.Copy()
		g_env_ded.Prepare( do_gamepak, ASSETS )
		setup_ded = Command( 'setup_ded', setup_source, Action( g_env_ded.BuildSetup ) )
		Default( setup_ded )
		setups.append( setup_ded )
	if ( SETUP_FULL == '1' ):
		g_env_full = g_env.Copy()
		g_env_full.Prepare( do_gamepak, ASSETS )
		setup_full = Command( 'setup_full', setup_source, Action( g_env_full.BuildSetup ) )
		Default( setup_full )
		setups.append( setup_full )
	if ( SETUP_INCREMENTAL == '1' ):
		g_env_incr = g_env.Copy()
		g_env_incr.Prepare( do_gamepak, ASSETS )
		setup_incr = Command( 'setup_incremental', setup_source, Action( g_env_incr.BuildSetup ) )
		Default( setup_incr )
		setups.append( setup_incr )
	# setup dependencies so they are built sequentially
	i = 1
	while ( i < len( setups ) ):
		Depends( setups[ i ], setups[ i - 1 ] )
		i += 1
else:
	if ( TARGET_GAMEPAK == '1' ):
		spgame_pak = Command( 'spgamepak', game, Action( g_env.BuildGamePak ) )
		Default( spgame_pak )
		mpgame_pak = Command( 'mpgamepak', mpgame, Action( g_env.BuildGamePak ) )
		Default( mpgame_pak )

if ( SDK != '0' ):
	setup_sdk = Command( 'sdk', [ ], Action( g_env.BuildSDK ) )
	Default( setup_sdk )
	
# end targets ------------------------------------
