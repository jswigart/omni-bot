////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotExports.h"

#ifdef _WIN32
#pragma warning(disable:4530) //C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable:4706) //assignment within conditional expression
#endif // _WIN32

#include <assert.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//////////////////////////////////////////////////////////////////////////

#ifndef CHECK_PRINTF_ARGS
#define CHECK_PRINTF_ARGS
#define CHECK_PARAM_VALID
#define CHECK_VALID_BYTES(parm)
#endif

//////////////////////////////////////////////////////////////////////////

std::string					gOmnibotLibPath;
OmnibotFunctionInterface*	gBotFunctions = 0;
IEngineInterface*			gGameFunctions = 0;
bool						gIsOmnibotLoaded = false;

void Omnibot_Load_PrintMsg( const char *_msg );
void Omnibot_Load_PrintErr( const char *_msg );

bool IsOmnibotLoaded()
{
	return gIsOmnibotLoaded && gBotFunctions;
}

const char *Omnibot_GetLibraryPath()
{
	return gOmnibotLibPath.c_str();
}

//////////////////////////////////////////////////////////////////////////

static const char *BOTERRORS[ BOT_NUM_ERRORS ] =
{
	"None",
	"Bot Library not found",
	"Unable to get Bot Functions from DLL",
	"Error Initializing the Bot",
	"Invalid Interface Functions",
	"Wrong Version",
	"Error Initializing File System",
};

void Omnibot_strncpy( char *dest, const char *source, int count )
{
	// Only doing this because some engines(HL2), think it a good idea to fuck up the
	// defines of all basic std::string functions throughout the entire project.
	while ( count && ( *dest++ = *source++ ) ) /* copy std::string */
		count--;

	if ( count ) /* pad out with zeroes */
		while ( --count )
			*dest++ = '\0';
}

const char *Omnibot_ErrorString( omnibot_error err )
{
	return ( ( err >= BOT_ERROR_NONE ) && ( err < BOT_NUM_ERRORS ) ) ? BOTERRORS[ err ] : "";
}

const char *Omnibot_FixPath( const char *_path )
{
	const int iBufferSize = 512;
	static char pathstr[ iBufferSize ] = { 0 };
	Omnibot_strncpy( pathstr, _path, iBufferSize );
	pathstr[ iBufferSize - 1 ] = '\0';

	// unixify the path slashes
	char *pC = pathstr;
	while ( *pC != '\0' )
	{
		if ( *pC == '\\' )
			*pC = '/';
		++pC;
	}

	// trim any trailing slash
	while ( *pC == '/' && pC > pathstr )
	{
		*pC = '\0';
		--pC;
	}
	return pathstr;
}

//////////////////////////////////////////////////////////////////////////

#if defined WIN32 || defined _WINDOWS || defined _WIN32

//////////////////////////////////////////////////////////////////////////
// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOWINRES
#define NOWINRES
#endif
#ifndef NOSERVICE
#define NOSERVICE
#endif
#ifndef NOMCX
#define NOMCX
#endif
#ifndef NOIME
#define NOIME
#endif
#include <stdio.h>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// Utilities

const char *OB_VA( const char* _msg, ... )
{
	static int iCurrentBuffer = 0;
	const int iNumBuffers = 3;
	const int BUF_SIZE = 1024;
	struct BufferInstance
	{
		char buffer[ BUF_SIZE ];
	};
	static BufferInstance buffers[ iNumBuffers ];

	char *pNextBuffer = buffers[ iCurrentBuffer ].buffer;

	va_list list;
	va_start( list, _msg );
	_vsnprintf( pNextBuffer, sizeof( buffers[ iCurrentBuffer ].buffer ), _msg, list );
	va_end( list );

	iCurrentBuffer = ( iCurrentBuffer + 1 ) % iNumBuffers;
	return pNextBuffer;
}

int OB_VA_OWNBUFFER( char *_buffer, int _buffersize, const char* _msg, ... )
{
	va_list list;
	va_start( list, _msg );
	const int ret = _vsnprintf( _buffer, _buffersize, _msg, list );
	va_end( list );
	return ret;
}

static int StringCompareNoCase( const char *s1, const char *s2 )
{
	return _stricmp( s1, s2 );
}

//////////////////////////////////////////////////////////////////////////
HINSTANCE g_BotLibrary = NULL;

bool OB_ShowLastError( const char *context )
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf,
		0, NULL );

	//////////////////////////////////////////////////////////////////////////
	// Strip Newlines
	char *pMessage = (char*)lpMsgBuf;
	int i = (int)strlen( pMessage ) - 1;
	while ( pMessage[ i ] == '\n' || pMessage[ i ] == '\r' )
		pMessage[ i-- ] = 0;
	//////////////////////////////////////////////////////////////////////////

	Omnibot_Load_PrintErr( OB_VA( "%s Failed with Error: %s", context, pMessage ) );
	LocalFree( lpMsgBuf );
	return true;
}

HINSTANCE Omnibot_LL( const char *file )
{
	//////////////////////////////////////////////////////////////////////////
	// Parse Variables
	// $(ProgramFiles)
	// $(OMNIBOT)

	//////////////////////////////////////////////////////////////////////////
	gOmnibotLibPath = file;
	HINSTANCE hndl = LoadLibrary( gOmnibotLibPath.c_str() );
	if ( !hndl )
		OB_ShowLastError( "LoadLibrary" );
	Omnibot_Load_PrintMsg( OB_VA( "Looking for %s, %s", gOmnibotLibPath.c_str(), hndl ? "found." : "not found" ) );
	return hndl;
}

omnibot_error Omnibot_LoadLibrary( int version, const char *lib, const char *path )
{
	omnibot_error r = BOT_ERROR_NONE;
	g_BotLibrary = Omnibot_LL( OB_VA( "%s\\%s.dll", path ? path : ".", lib ) );
	if ( g_BotLibrary == 0 )
		g_BotLibrary = Omnibot_LL( OB_VA( ".\\omni-bot\\%s.dll", lib ) );
	if ( g_BotLibrary == 0 )
		g_BotLibrary = Omnibot_LL( OB_VA( "%s.dll", lib ) );
	if ( g_BotLibrary == 0 )
	{
		gOmnibotLibPath.clear();
		r = BOT_ERROR_CANTLOADDLL;
	}
	else
	{
		Omnibot_Load_PrintMsg( OB_VA( "Found Omni-bot: %s, Attempting to Initialize", gOmnibotLibPath.c_str() ) );
		pfnCreateBotInterface pfnGetBotInterface = 0;
		pfnGetBotInterface = (pfnCreateBotInterface)GetProcAddress( g_BotLibrary, "CreateBotInterface" );
		if ( pfnGetBotInterface == 0 )
		{
			r = BOT_ERROR_CANTGETBOTFUNCTIONS;
			Omnibot_Load_PrintErr( OB_VA( "Omni-bot Failed with Error: %s", Omnibot_ErrorString( r ) ) );
		}
		else
		{
			gBotFunctions = pfnGetBotInterface( OmnibotFunctionInterface::VERSION_LATEST );
			if ( gBotFunctions != NULL )
			{
				Omnibot_Load_PrintMsg( "Omni-bot Loaded Successfully" );
				r = gBotFunctions->Initialize( gGameFunctions, version );
				gIsOmnibotLoaded = ( r == BOT_ERROR_NONE );
			}

			// cs: removed else so interface errors can be printed
			if ( r != BOT_ERROR_NONE )
			{
				Omnibot_Load_PrintErr( OB_VA( "Omni-bot Failed with Error: %s", Omnibot_ErrorString( r ) ) );
				Omnibot_FreeLibrary();
			}
		}
	}
	return r;
}

void Omnibot_FreeLibrary()
{
	if ( g_BotLibrary )
	{
		FreeLibrary( g_BotLibrary );
		g_BotLibrary = 0;
	}
	memset( &gBotFunctions, 0, sizeof( gBotFunctions ) );

	delete gGameFunctions;
	gGameFunctions = 0;

	gIsOmnibotLoaded = false;
}

#elif defined __linux__ || ((defined __MACH__) && (defined __APPLE__))

#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
// Utilities

const char *OB_VA(const char* _msg, ...)
{
	static int iCurrentBuffer = 0;
	const int iNumBuffers = 3;
	const int BUF_SIZE = 1024;
	struct BufferInstance
	{
		char buffer[BUF_SIZE];
	};
	static BufferInstance buffers[iNumBuffers];

	char *pNextBuffer = buffers[iCurrentBuffer].buffer;

	va_list list;
	va_start(list, _msg);
	vsnprintf(pNextBuffer, sizeof(buffers[iCurrentBuffer].buffer), _msg, list);
	va_end(list);

	iCurrentBuffer = (iCurrentBuffer+1)%iNumBuffers;
	return pNextBuffer;
}

int OB_VA_OWNBUFFER(char *_buffer, int _buffersize, CHECK_PRINTF_ARGS const char* _msg, ...)
{
	va_list list;
	va_start(list, _msg);
	const int ret = vsnprintf(_buffer, _buffersize, _msg, list);
	va_end(list);
	return ret;
}

static int StringCompareNoCase(const char *s1, const char *s2)
{
	return strcasecmp(s1,s2);
}

#include <dlfcn.h>
#define GetProcAddress dlsym
#ifndef NULL
#define NULL 0
#endif

//////////////////////////////////////////////////////////////////////////
void *g_BotLibrary = NULL;

bool OB_ShowLastError(const char *context, const char *errormsg)
{
	Omnibot_Load_PrintErr(OB_VA("%s Failed with Error: %s", context, errormsg?errormsg:"<unknown error>"));
	return true;
}

void *Omnibot_LL(const char *file)
{
	g_OmnibotLibPath = file;
	void *pLib = dlopen(g_OmnibotLibPath.c_str(), RTLD_NOW);
	if(!pLib)
		OB_ShowLastError("LoadLibrary", dlerror());

	Omnibot_Load_PrintMsg(OB_VA("Looking for %s, ", g_OmnibotLibPath.c_str(), pLib ? "found." : "not found"));
	return pLib;
}

omnibot_error Omnibot_LoadLibrary(int version, const char *lib, const char *path)
{
	omnibot_error r = BOT_ERROR_NONE;
	g_BotLibrary = Omnibot_LL(OB_VA("%s/%s.so", path ? path : ".", lib));
	if(!g_BotLibrary)
	{
		g_BotLibrary = Omnibot_LL(OB_VA("./%s.so", lib));
	}
	if(!g_BotLibrary)
	{
		char *homeDir = getenv("HOME");
		if(homeDir)
			g_BotLibrary = Omnibot_LL(OB_VA("%s/omni-bot/%s.so", homeDir, lib));
	}
	if(!g_BotLibrary)
	{
		char *homeDir = getenv("HOME");
		if(homeDir)
			g_BotLibrary = Omnibot_LL(OB_VA("%s.so", lib));
	}
	if(!g_BotLibrary)
	{
		g_OmnibotLibPath.clear();
		r = BOT_ERROR_CANTLOADDLL;
	}
	else
	{
		Omnibot_Load_PrintMsg(OB_VA("Found Omni-bot: %s, Attempting to Initialize", g_OmnibotLibPath.c_str()));
		pfnGetFunctionsFromDLL pfnGetBotFuncs = 0;
		memset(&gBotFunctions, 0, sizeof(gBotFunctions));
		pfnGetBotFuncs = (pfnGetFunctionsFromDLL)GetProcAddress(g_BotLibrary, "ExportBotFunctionsFromDLL");
		if(!pfnGetBotFuncs)
		{
			r = BOT_ERROR_CANTGETBOTFUNCTIONS;
			Omnibot_Load_PrintErr(OB_VA("Omni-bot Failed with Error: %s", Omnibot_ErrorString(r)));
			OB_ShowLastError("GetProcAddress", dlerror());
		}
		else
		{
			r = pfnGetBotFuncs(&gBotFunctions, sizeof(gBotFunctions));
			if(r == BOT_ERROR_NONE)
			{
				Omnibot_Load_PrintMsg("Omni-bot Loaded Successfully");
				r = gBotFunctions->pfnInitialize(gGameFunctions, version);
				g_IsOmnibotLoaded = (r == BOT_ERROR_NONE);
			}
		}
	}
	return r;
}

void Omnibot_FreeLibrary()
{
	if(g_BotLibrary)
	{
		dlclose(g_BotLibrary);
		g_BotLibrary = 0;
	}
	memset(&gBotFunctions, 0, sizeof(gBotFunctions));
	memset(&gGameFunctions, 0, sizeof(gGameFunctions));
	g_IsOmnibotLoaded = false;
}

//////////////////////////////////////////////////////////////////////////

#else

#error "Unsupported Platform or Missing platform #defines";

#endif

//////////////////////////////////////////////////////////////////////////
KeyVals::KeyVals()
{
	Reset();
}
void KeyVals::Reset()
{
	memset( mKey, 0, sizeof( mKey ) );
	memset( mString, 0, sizeof( mString ) );
	memset( mValue, 0, sizeof( mValue ) );
}
bool KeyVals::SetInt( const char *_key, int _val )
{
	return SetKeyVal( _key, obUserData( _val ) );
}
bool KeyVals::SetFloat( const char *_key, float _val )
{
	return SetKeyVal( _key, obUserData( _val ) );
}
bool KeyVals::SetEntity( const char *_key, GameEntity _val )
{
	return SetKeyVal( _key, obUserData( _val ) );
}
bool KeyVals::SetVector( const char *_key, float _x, float _y, float _z )
{
	return SetKeyVal( _key, obUserData( _x, _y, _z ) );
}
bool KeyVals::SetVector( const char *_key, const float *_v )
{
	return SetKeyVal( _key, obUserData( _v[ 0 ], _v[ 1 ], _v[ 2 ] ) );
}
bool KeyVals::SetString( const char *_key, const char *_value )
{
	_value = _value ? _value : "";

	for ( int a = 0; a < MaxArgs; ++a )
	{
		// look for the first null std::string
		if ( mString[ a ][ 0 ] == '\0' )
		{
			Omnibot_strncpy( &mString[ a ][ 0 ], _value, MaxStringLength - 1 );
			return SetKeyVal( _key, obUserData( &mString[ a ][ 0 ] ) );
		}
	}
	assert( false );
	return false;
}
bool KeyVals::Set( const char *_key, const obUserData &_value )
{
	return SetKeyVal( _key, _value );
}
bool KeyVals::SetKeyVal( const char *_key, const obUserData &_ud )
{
	if ( !_key )
		return false;

	int ifree = -1;
	for ( int i = 0; i < MaxArgs; ++i )
	{
		if ( ifree == -1 && mKey[ i ][ 0 ] == 0 )
			ifree = i;
		if ( !StringCompareNoCase( mKey[ i ], _key ) )
		{
			mValue[ i ] = _ud;
			return true;
		}
	}
	if ( ifree != -1 )
	{
		Omnibot_strncpy( &mKey[ ifree ][ 0 ], _key, MaxArgLength - 1 );
		mValue[ ifree ] = _ud;
		return true;
	}
	return false;
}

bool KeyVals::GetInt( const char *_key, int &_val ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_val = d.GetInt();
		return true;
	}
	return false;
}
bool KeyVals::GetFloat( const char *_key, float &_val ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_val = d.GetFloat();
		return true;
	}
	return false;
}
bool KeyVals::GetEntity( const char *_key, GameEntity &_val ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_val = d.GetEntity();
		return true;
	}
	return false;
}
bool KeyVals::GetVector( const char *_key, float &_x, float &_y, float &_z ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_x = d.GetVector()[ 0 ];
		_y = d.GetVector()[ 1 ];
		_z = d.GetVector()[ 2 ];
		return true;
	}
	return false;
}
bool KeyVals::GetVector( const char *_key, float *_v ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_v[ 0 ] = d.GetVector()[ 0 ];
		_v[ 1 ] = d.GetVector()[ 1 ];
		_v[ 2 ] = d.GetVector()[ 2 ];
		return true;
	}
	return false;
}
bool KeyVals::GetString( const char *_key, const char *&_val ) const
{
	obUserData d;
	if ( GetKeyVal( _key, d ) )
	{
		_val = d.GetString();
		return true;
	}
	return false;
}
bool KeyVals::GetKeyVal( const char *_key, obUserData &_ud ) const
{
	for ( int i = 0; i < MaxArgs; ++i )
	{
		if ( !StringCompareNoCase( mKey[ i ], _key ) )
		{
			_ud = mValue[ i ];
			return true;
		}
	}
	return false;
}
void KeyVals::GetKV( int _index, const char *&_key, obUserData &ud ) const
{
	_key = mKey[ _index ];
	ud = mValue[ _index ];
}