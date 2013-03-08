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

		void GetDebugString(std::stringstream &out);
		void RenderDebug();

		obReal GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		CallArtillery();
	private:
		Trackers			Tracker;
		MapGoalPtr			m_MapGoal;
		MapGoalPtr			m_MapGoalTarget;
		GameEntity			m_TargetEntity;
		int					m_FireTime;

		FilterPtr			m_WatchFilter;

		bool				m_Fired;
		Seconds				m_MinCampTime;
		Seconds				m_MaxCampTime;
		int					m_Stance;

		int					m_ExpireTime;
	};

	//////////////////////////////////////////////////////////////////////////
};

#endif
