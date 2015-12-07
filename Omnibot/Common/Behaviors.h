////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BEHAVIORS_H__
#define __BEHAVIORS_H__

#include "StateMachine.h"
#include "TacticalManager.h"
#include "BotPathing.h"

class gmScriptGoal;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	class StimulusState : public State
	{
	public:
		StimulusState( const char* name, BehaviorGoal type, StimulusBehavior& stim );

		float GetPriority();
	protected:
		const BehaviorGoal		mType;
		StimulusBehavior&		mStim;
	};

	//////////////////////////////////////////////////////////////////////////

	class StimTouch : public StimulusState, public FollowPathUser
	{
	public:
		StimTouch( const char* name, BehaviorGoal type, StimulusBehavior& stim );

		virtual bool GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt );
		
		StateStatus UpdateState( float fDt );
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	class StimResupply : public StimTouch
	{
	public:
		StimResupply( StimulusBehavior& stim );

		StateStatus UpdateState( float fDt );
	};

	//////////////////////////////////////////////////////////////////////////

	class StimGetWeapon : public StimTouch
	{
	public:
		StimGetWeapon( StimulusBehavior& stim );
		
		StateStatus UpdateState( float fDt );
	};

	//////////////////////////////////////////////////////////////////////////

	class StimGetPowerup : public StimTouch
	{
	public:
		StimGetPowerup( StimulusBehavior& stim );

		StateStatus UpdateState( float fDt );
	};

	//////////////////////////////////////////////////////////////////////////

	class HighLevel : public StateFirstAvailable
	{
	public:
		HighLevel();
		
		float GetPriority();
	private:
		StimulusBehavior	mActiveBehavior;
	};

	//////////////////////////////////////////////////////////////////////////
};

#endif
