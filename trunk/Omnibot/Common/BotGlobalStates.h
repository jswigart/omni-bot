////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTGLOBALSTATES_H__
#define __BOTGLOBALSTATES_H__

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
