////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Behaviors.h"
#include "System.h"
#include "BotBaseStates.h"
#include "BotWeaponSystem.h"
#include "RenderBuffer.h"

namespace AiState
{
	StimulusState::StimulusState( const char* name, BehaviorGoal type, StimulusBehavior& stim )
		: State( name )
		, mType( type )
		, mStim( stim )
	{
	}

	float StimulusState::GetPriority()
	{
		return ( mStim.mAction == mType ) ? 1.0f : 0.0f;
	}
	
	//////////////////////////////////////////////////////////////////////////

	StimTouch::StimTouch( const char* name, BehaviorGoal type, StimulusBehavior& stim )
		: StimulusState( name, type, stim )
		, FollowPathUser( name )
	{
	}
	
	bool StimTouch::GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt )
	{
		Destination d;
		d.mPosition = mStim.mStimulus->mPosition;
		d.mRadius = 12.0f;
		desination.push_back( d );
		return true;
	}

	State::StateStatus StimTouch::UpdateState( float fDt )
	{
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	StimResupply::StimResupply( StimulusBehavior& stim )
		: StimTouch( "Resupply", BEHAVIOR_RESUPPLY, stim )
	{
	}

	State::StateStatus StimResupply::UpdateState( float fDt )
	{
		return StimTouch::UpdateState( fDt );
	}

	//////////////////////////////////////////////////////////////////////////
	
	StimGetWeapon::StimGetWeapon( StimulusBehavior& stim )
		: StimTouch( "GetWeapon", BEHAVIOR_GET_WEAPON, stim )
	{
	}

	State::StateStatus StimGetWeapon::UpdateState( float fDt )
	{
		/*FINDSTATE( wp, WeaponSystem, GetRootState() );
		if ( wp && wp->HasWeapon( mStim.mStimulus->mEntInfo.mClassId ) )
			return State_Finished;*/

		if ( mStim.mStimulus->mEntInfo.mFlags.CheckFlag( ENT_FLAG_DISABLED ) )
			return State_Finished;
		
		return StimTouch::UpdateState( fDt );
	}

	//////////////////////////////////////////////////////////////////////////

	StimGetPowerup::StimGetPowerup( StimulusBehavior& stim )
		: StimTouch( "GetPowerup", BEHAVIOR_GET_POWERUP, stim )
	{
	}

	State::StateStatus StimGetPowerup::UpdateState( float fDt )
	{
		if ( GetClient()->HasPowerup( mStim.mStimulus->mEntInfo.mClassId ) )
			return State_Finished;

		return StimTouch::UpdateState( fDt );
	}

	////////////////////////////////////////////////////////////////////////////

	HighLevel::HighLevel()
		: StateFirstAvailable( "HighLevel" )
	{
		//AppendState( new Kill( mActiveBehavior ) );
		//AppendState( new PressButton( mActiveBehavior ) );
		AppendState( new StimResupply( mActiveBehavior ) );
		AppendState( new StimGetWeapon( mActiveBehavior ) );
		AppendState( new StimGetPowerup( mActiveBehavior ) );
		AppendState( new Roam );
		/*AppendState( new StimGetFlag( mActiveBehavior ) );
		AppendState( new StimCapFlag( mActiveBehavior ) );
		AppendState( new StimCapControlPoint( mActiveBehavior ) );
		AppendState( new StimDefend( mActiveBehavior ) );*/
	}
	
	float HighLevel::GetPriority()
	{
		static const size_t MAX_BEHAVIORS = 2;
		StimulusBehavior behaviors[ MAX_BEHAVIORS ];
		const size_t cnt = System::mInstance->mTacticalManager->GetBehaviorsForUser( GetClient()->GetGameEntity(), behaviors, MAX_BEHAVIORS );
		if ( cnt > 0 )
		{
			mActiveBehavior = behaviors[ 0 ];
		}
		else
		{
			mActiveBehavior = StimulusBehavior();
		}
		return StateFirstAvailable::GetPriority();
	}

	//////////////////////////////////////////////////////////////////////////
};