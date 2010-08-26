////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-01-26 18:13:10 -0800 (Sat, 26 Jan 2008) $
// $LastChangedRevision: 2373 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "BotGlobalStates.h"

namespace AiState
{
	RegionTriggers::RegionTriggers() 
		: StateChild("RegionTriggers")
	{
	}

	//////////////////////////////////////////////////////////////////////////

	GlobalRoot::GlobalRoot() : StateFirstAvailable("GlobalRoot") 
	{
		AppendState(new RegionTriggers);
	}

};