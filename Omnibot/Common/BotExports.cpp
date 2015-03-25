////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

//#include "vld.h"

#include "BotExports.h"
#include "Omni-Bot.h"
#include "RenderBuffer.h"
//#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID )
{
	hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

OMNIBOT_API OmnibotFunctionInterface* CreateBotInterface( OmnibotFunctionInterface::Version interfaceVersion )
{
	if ( interfaceVersion == OmnibotFunctionInterface::VERSION_LATEST )
	{
		return new OmnibotFunctions();
	}
	return NULL;
}

OMNIBOT_API void RenderOpenGL()
{
	RenderBuffer::RenderToOpenGL();
}
