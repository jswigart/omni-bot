////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __JABASESTATES_H__
#define __JABASESTATES_H__

#include "StateMachine.h"
#include "Path.h"
#include "ScriptManager.h"

class gmScriptGoal;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////
	class ForceJump : public StateChild
	{
	public:

		void RenderDebug();

		float GetPriority();
		StateStatus Update(float fDt);

		void Enter();

		ForceJump();
	private:
		Path::PathPoint		mNextPt;
		float				mDistanceLeft;
		float				mDistanceTotal;
		bool				mIsJumping : 1;
		bool				mIsOnGround : 1;
	};
	//////////////////////////////////////////////////////////////////////////
};

#endif
