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
#pragma warning( disable: 4710 )	// function '...' not inlined
#endif // _WIN32

// Enable some useful ones that are disabled by default
// http://msdn2.microsoft.com/en-us/library/23k5d385(VS.80).aspx
#pragma warning( default: 4062 )	// enumerator 'identifier' in switch of enum 'enumeration' is not handled
#pragma warning( default: 4265 )	// class has virtual functions, but destructor is not virtual
#pragma warning( default: 4431 )	// missing type specifier - int assumed. Note: C no longer supports default-int

// Disable if these get annoying.
#pragma warning( default: 4711 )	// function '...' selected for automatic inline expansion
#endif 

#ifdef _OPENMP
#include <omp.h>
#endif

#include "Remotery.h"

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

// global: g_EngineFuncs is a bot-wide global so that game functionality
//		can be used from anywhere
extern IEngineInterface *gEngineFuncs;

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
