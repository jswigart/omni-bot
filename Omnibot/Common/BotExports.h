////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: BotExports
//		In order for the game to call functions from the bot, we must export
//		the functions to the game itself and allow it to call them.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTEXPORTS_H__
#define __BOTEXPORTS_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "IEngineInterface.h"

//////////////////////////////////////////////////////////////////////////
// Export the function on platforms that require it.
#ifdef WIN32
#define OMNIBOT_API __declspec(dllexport)
#else
#define OMNIBOT_API
#endif

// Typedef for the only exported bot function.
typedef OmnibotFunctionInterface* ( *pfnCreateBotInterface )( OmnibotFunctionInterface::Version interfaceVersion );

// note: Export Functions with C Linkage
//	Export with C Linkage so the game interface can acccess it easier.
//	This gets rid of name mangling
//	Wrapped in #ifdef because the game SDK might be in pure C
#ifdef __cplusplus
extern "C"
{
#endif
	// function: ExportBotFunctionsFromDLL
	//		Allow the bot dll to fill in a struct of bot functions the interface
	//		can then call.
	OMNIBOT_API OmnibotFunctionInterface* CreateBotInterface( OmnibotFunctionInterface::Version interfaceVersion );

	OMNIBOT_API void RenderOpenGL();
#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////
// Interfaces

extern OmnibotFunctionInterface*	gBotFunctions;
extern IEngineInterface*			gGameFunctions;

//////////////////////////////////////////////////////////////////////////
// Utility Function
extern "C" const char *OB_VA(const char* _msg, ...);
//////////////////////////////////////////////////////////////////////////

omnibot_error Omnibot_LoadLibrary(int version, const char *lib, const char *path);
void Omnibot_FreeLibrary();
bool IsOmnibotLoaded();
const char *Omnibot_ErrorString(omnibot_error err);
const char *Omnibot_GetLibraryPath();
const char *Omnibot_FixPath(const char *_path);
#endif
