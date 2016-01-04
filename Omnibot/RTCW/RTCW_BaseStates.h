////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCWBASESTATES_H__
#define __RTCWBASESTATES_H__

#include "StateMachine.h"
#include "BotBaseStates.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	class CallArtillery : public StateChild, public FollowPathUser, public AimerUser
	{
	public:

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message & message, CallbackParameters & cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		CallArtillery();
	private:
		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
		MapGoalPtr		 mMapGoalTarget;
		GameEntity		 mTargetEntity;
		int				 mFireTime;

		FilterPtr		 mWatchFilter;

		bool			 mFired;
		Seconds			 mMinCampTime;
		Seconds			 mMaxCampTime;
		int				 mStance;

		int				 mExpireTime;
	};

	//////////////////////////////////////////////////////////////////////////
};

#endif
