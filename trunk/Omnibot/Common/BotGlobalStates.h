////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-01-26 18:13:10 -0800 (Sat, 26 Jan 2008) $
// $LastChangedRevision: 2373 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BotGlobalStates_H__
#define __BotGlobalStates_H__

#include "StateMachine.h"
#include "ScriptManager.h"

class gmScriptGoal;

namespace AiState
{
	class RegionTriggers : public StateChild
	{
	public:

		RegionTriggers();
	private:
	};

	class GlobalRoot : public StateFirstAvailable
	{
	public:
		GlobalRoot();
	private:
	};
};

#endif
