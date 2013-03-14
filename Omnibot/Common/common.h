////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// file: Commonly included headers
//		In order to reduce the clutter in every file that commonly includes
//		some headers, such as STL headers and utility headers, we put them
//		in here so that those files can just include this and get access to
//		all the common stuff.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __COMMON_H__
#define __COMMON_H__

//#include "CodeAnalysis.h"

// Disable some compiler warnings.
#ifdef _WIN32
#pragma warning(   error: 4002 )	// Too many actual parameters for macro: promoted to be an error
#pragma warning( disable: 4097 )	// typedef-name '...' used as synonym for class-name '...'
#pragma warning( disable: 4100 )	// unreferenced formal parameter
#pragma warning( disable: 4127 )	// conditional expression is constant
#pragma warning( disable: 4206 )	// nonstandard extension used : translation unit is empty
#pragma warning( disable: 4505 )	// 'function' : unreferenced local function has been removed
#pragma warning( disable: 4511 )	// 'class' : copy constructor could not be generated
#pragma warning( disable: 4512 )	// assignment operator could not be generated
#pragma warning( disable: 4521 )	// 'class' : multiple copy constructors specified
#pragma warning( disable: 4522 )	// 'class' : multiple assignment operators specified
#pragma warning( disable: 4530 )	// C++ exception handler used, but unwind semantics are not enabled.
#pragma warning( disable: 4714 )	// function 'function' marked as __forceinline not inlined
#pragma warning( disable: 4996 )	// 'function': was declared deprecated
#pragma warning( disable: 6246 )	// Local declaration of 'class' hides declaration of the same name in outer scope
#pragma warning( disable: 6322 )	// Empty _except block
#pragma warning( disable: 6355 )	// _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead
#pragma warning( disable: 4512 )	// 'class' : assignment operator could not be generated
#pragma warning( disable: 6384 )	// Dividing sizeof a pointer by another value
#endif // _WIN32

// Enable some useful ones that are disabled by default
// http://msdn2.microsoft.com/en-us/library/23k5d385(VS.80).aspx
#pragma warning( default: 4062)		// enumerator 'identifier' in switch of enum 'enumeration' is not handled
#pragma warning( default: 4265)		// class has virtual functions, but destructor is not virtual
#pragma warning( default: 4431)		// missing type specifier - int assumed. Note: C no longer supports default-int

// Disable if these get annoying.
#pragma warning( default: 4710 )	// function '...' not inlined
#pragma warning( default: 4711 )	// function '...' selected for automatic inline expansion

#ifdef _OPENMP
#include <omp.h>
#endif

// Boost
#if _MSC_VER == 1600 // cs: ffs
#include <boost/version.hpp>
#if BOOST_VERSION <= 104400
#define BOOST_NO_RVALUE_REFERENCES 1
#endif // boost lib <= 1_44_0
#endif // vs2010

#ifdef _WIN32
#pragma warning( push )
// stfu boost
#pragma warning( disable: 4244 )
#pragma warning( disable: 4265 )
#pragma warning( disable: 6334 )
#pragma warning( disable: 6011 )
#pragma warning( disable: 4913 )
#endif //_WIN32

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/static_assert.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/thread.hpp>

#ifdef _WIN32
#pragma warning( pop )
#endif // _WIN32

// Wild Magic Math Libraries
#include "Wm5Mathematics.h"
using namespace Wm5;

// General purpose stuff.
#include "Functor.h"

// Custom files
#include "Logger.h"
#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "Omni-Bot_Color.h"
#include "CallbackParameters.h"

#include "EngineFuncs.h"
#include "IEngineInterface.h"

#include "prof.h"

#ifdef WIN32
//#define ENABLE_REMOTE_DEBUGGER
//#define ENABLE_FILE_DOWNLOADER
//#define ENABLE_REMOTE_DEBUGGING
#endif

//#define ENABLE_REMOTE_DEBUGGING

// remote debug
#ifdef ENABLE_REMOTE_DEBUGGING
#include "DebugConnection.h"
#endif

// global: g_EngineFuncs is a bot-wide global so that game functionality
//		can be used from anywhere
extern IEngineInterface *g_EngineFuncs;

#ifdef Prof_ENABLED
typedef ProfileZones<> CustomProfilerZone;
extern CustomProfilerZone gDynamicZones;
#endif

#define REGEX_OPTIONS boost::regex::basic|boost::regex::icase|boost::regex::grep

#define OB_DELETE(p)   { if(p) { delete (p); (p)=NULL; } }
#define OB_ARRAY_DELETE(p)   { if(p) { delete [] (p); (p)=NULL; } }
#define OB_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define OB_MAX(a, b)  (((a) > (b)) ? (a) : (b))

enum MoveMode
{
	Run,
	Walk,
	NumMoveModes
};

#define DBG_MSG(debugflag, bot, type, msg) \
	if((debugflag)==0 || (bot)->IsDebugEnabled((debugflag))) { (bot)->OutputDebug((type), (msg)); }

// macro: DEBUG_ONLY
//		A macro that can be used to only allow the contained functionality
//		in debug builds of the library.
#ifdef _DEBUG
#define DEBUG_ONLY(m) m
#else
#define DEBUG_ONLY(m) 0
#endif

#include "Utilities.h"

// cs: FIXME: debug version of OBASSERT doesnt build in linux. doesn't like __VA_ARGS__
#ifdef __linux__
#define OBASSERT(f, sz, ...) {}	// cs: for gcc warnings, was (f)
#else // !__linux__
#ifdef	_DEBUG
#define OBASSERT(f, msg, ...) { static bool bShowAssert = true; \
	if(bShowAssert) { \
	bShowAssert = Utils::AssertFunction((bool)((f)!=0), #f, __FILE__, __LINE__, msg, __VA_ARGS__); \
	} }
#else	// !DEBUG
#define OBASSERT(f, sz, ...) (f)
#endif	// !DEBUG

#endif // cs: FIXME

#ifdef	_DEBUG
#define SOFTASSERTONCE(f, sz, ...) { static bool bShowAssert = true; \
	if(bShowAssert) { \
	bShowAssert = Utils::SoftAssertFunction(Utils::FireOnce, (bool)((f)!=0), #f, __FILE__, __LINE__, msg, __VA_ARGS__); \
	} }
#else	// !DEBUG
#define SOFTASSERTONCE(f, sz, ...)
#endif	// !DEBUG

#define SOFTASSERTALWAYS(f, msg, ...) { static bool bShowAssert = true; \
	if(bShowAssert) { \
	bShowAssert = Utils::SoftAssertFunction(Utils::FireAlways, (bool)((f)!=0), #f, __FILE__, __LINE__, msg, __VA_ARGS__); \
	} }

#define FINDSTATE(var, statename, parent) \
	statename *var = static_cast<statename*>(parent->FindState(#statename)); OBASSERT(var, #statename " Not Found" );

#define FINDSTATEIF(statename, parent, exp) { \
	statename *st = static_cast<statename*>(parent->FindState(#statename));	if(st) st->exp; else OBASSERT(0, #statename " Not Found" ); }

#define FINDSTATE_OPT(var, statename, parent) \
	statename *var = static_cast<statename*>(parent->FindState(#statename));

#define FINDSTATEIF_OPT(statename, parent, exp) { \
	statename *st = static_cast<statename*>(parent->FindState(#statename));	if(st) st->exp; }

#endif
