////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "RTCW_BaseStates.h"
#include "RTCW_FilterClosest.h"
#include "RTCW_Messages.h"
#include "BotPathing.h"
#include "WeaponDatabase.h"
#include "RenderBuffer.h"
#include "System.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	CallArtillery::CallArtillery()
		: StateChild( "CallArtillery" )
		, FollowPathUser( "CallArtillery" )
		, mMinCampTime( 1000 )
		, mMaxCampTime( 2000 )
		, mExpireTime( 0 )
		, mStance( StanceStand )
	{
		LimitToWeapon().SetFlag( RTCW_WP_BINOCULARS );
	}

	void CallArtillery::GetDebugString( std::stringstream &out )
	{
		out << (mMapGoal ? mMapGoal->GetName() : "");
	}

	void CallArtillery::RenderDebug()
	{
		if ( IsActive() )
		{
			if ( mMapGoal )
			{
				RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
				RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN );
			}
		}
	}

	// FollowPathUser functions.
	bool CallArtillery::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool CallArtillery::GetAimPosition( Vector3f &_aimpos )
	{
		if ( mMapGoalTarget )
		{
			_aimpos = mMapGoalTarget->GetPosition();
		}
		else if ( mTargetEntity.IsValid() )
		{
			const float LOOKAHEAD_TIME = 5.0f;
			const MemoryRecord *mr = GetClient()->GetSensoryMemory()->GetMemoryRecord( mTargetEntity );
			if ( mr )
			{
				const Vector3f vVehicleOffset = Vector3f( 0.0f, 0.0f, 32.0f );
				_aimpos = vVehicleOffset + mr->mTargetInfo.mLastPosition +
					mr->mTargetInfo.mLastVelocity * LOOKAHEAD_TIME;
				mFireTime = IGame::GetTime() + 1000;
			}
		}
		return true;
	}

	void CallArtillery::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws && ws->CurrentWeaponIs( RTCW_WP_BINOCULARS ) )
		{
			GetClient()->PressButton( BOT_BUTTON_AIM );
			if ( mFireTime < IGame::GetTime() )
			{
				if ( GetClient()->HasEntityFlag( ENT_FLAG_ZOOMING ) )
				{
					ws->FireWeapon();
				}
			}
		}
	}

	float CallArtillery::GetPriority()
	{
		if ( !gEngineFuncs->IsWeaponCharged( GetClient()->GetGameEntity(), RTCW_WP_BINOCULARS, Primary ) )
			return 0.f;

		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();
		mMapGoalTarget.reset();

		//////////////////////////////////////////////////////////////////////////
		RTCW_FilterClosest filter( GetClient(), AiState::SensoryMemory::EntEnemy );
		filter.AddCategory( ENT_CAT_SHOOTABLE );
		FINDSTATE( sensory, SensoryMemory, GetRootState() );
		if ( sensory )
		{
			sensory->QueryMemory( filter );
			mTargetEntity = filter.GetBestEntity();
			if ( mTargetEntity.IsValid() )
				return GetLastPriority();
		}
		//////////////////////////////////////////////////////////////////////////
		{
			GoalManager::Query qry( 0x312ad48d /* CALLARTILLERY */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			for ( uint32_t i = 0; i < qry.mList.size(); ++i )
			{
				if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
					continue;

				if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
					continue;

				mMapGoal = qry.mList[ i ];
				break;
			}
		}
		if ( !mMapGoal )
			return 0.f;
		//////////////////////////////////////////////////////////////////////////
		Vector3f vSource = mMapGoal->GetPosition();
		//vSource = vSource + Vector3(0,0,60);
		vSource.Z() = vSource.Z() + 60;
		//////////////////////////////////////////////////////////////////////////
		if ( !mMapGoalTarget )
		{
			GoalManager::Query qry( 0xb708821b /* ARTILLERY_S */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			for ( uint32_t i = 0; i < qry.mList.size(); ++i )
			{
				if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
					continue;

				if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
					continue;

				{
					Vector3f vTarget = qry.mList[ i ]->GetPosition();
					vTarget.Z() = vTarget.Z() + 20;
					if ( !Client::HasLineOfSightTo( vSource, vTarget ) )
						continue;
				}

				mMapGoalTarget = qry.mList[ i ];
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if ( !mMapGoalTarget )
		{
			GoalManager::Query qry( 0xac0870ca /* ARTILLERY_D */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			for ( uint32_t i = 0; i < qry.mList.size(); ++i )
			{
				if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
					continue;

				if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
					continue;

				{
					Vector3f vTarget = qry.mList[ i ]->GetPosition();
					vTarget.Z() = vTarget.Z() + 20;
					if ( !Client::HasLineOfSightTo( vSource, vTarget ) )
						continue;
				}

				mMapGoalTarget = qry.mList[ i ];
				break;
			}
		}
		return mMapGoalTarget ? mMapGoalTarget->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void CallArtillery::Enter()
	{
		if ( mMapGoalTarget && mMapGoalTarget->GetGoalType() == "ARTILLERY_D" )
			mFireTime = std::numeric_limits<int>::max();
		else
			mFireTime = 0;

		mFired = false;
		mExpireTime = 0;

		if ( mMapGoal )
		{
			mMapGoal->GetProperty( "Stance", mStance );
			mMapGoal->GetProperty( "MinCampTime", mMinCampTime );
			mMapGoal->GetProperty( "MaxCampTime", mMaxCampTime );
		}

		if ( mMapGoalTarget )
		{
			if ( !mWatchFilter )
			{
				mWatchFilter.reset( new RTCW_FilterClosest( GetClient(), AiState::SensoryMemory::EntEnemy ) );
			}

			mWatchFilter->AddClass( FilterSensory::ANYPLAYERCLASS );
			mWatchFilter->AddPosition( mMapGoalTarget->GetPosition() );
			mWatchFilter->SetMaxDistance( 100.f );
			FINDSTATEIF( ProximityWatcher, GetRootState(), AddWatch( GetNameHash(), mWatchFilter, false ) );
		}
		Tracker.InProgress = mMapGoal;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this, Run ) );
	}

	void CallArtillery::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mMapGoal.reset();

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
		FINDSTATEIF( ProximityWatcher, GetRootState(), RemoveWatch( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus CallArtillery::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			if ( mMapGoal )
				BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( mMapGoal && !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;
		if ( mMapGoalTarget && !mMapGoalTarget->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			if ( mFired )
				return State_Finished;

			if ( mExpireTime == 0 )
			{
				mExpireTime = IGame::GetTime() + Mathf::IntervalRandomInt( mMinCampTime.GetMs(), mMaxCampTime.GetMs() );
				Tracker.InUse = mMapGoal;
			}
			else if ( IGame::GetTime() > mExpireTime )
			{
				// Delay it from being used for a while.
				if ( mMapGoal )
					BlackboardDelay( 30.f, mMapGoal->GetSerialNum() );
				return State_Finished;
			}

			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), RTCW_WP_BINOCULARS ) );

			if ( mStance == StanceCrouch )
				GetClient()->PressButton( BOT_BUTTON_CROUCH );
		}
		return State_Busy;
	}

	void CallArtillery::ProcessEvent( const Message& message, CallbackParameters &cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvProximityTrigger )
			{
				if ( msg->mOwnerState == GetNameHash() )
				{
					mFireTime = IGame::GetTime();// + 1000;
				}
				break;
			}
			CASE_MSG( EvWeaponFire )
			{
				if ( msg->mWeaponId == RTCW_WP_BINOCULARS )
				{
					mFired = true;
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
};