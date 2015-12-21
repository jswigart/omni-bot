////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotBaseStates.h"
#include "BotPathing.h"
#include "ScriptManager.h"
#include "BotSteeringSystem.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"
#include "IGameManager.h"
#include "PathPlannerBase.h"
#include "Behaviors.h"
#include "RenderBuffer.h"

const float ROAM_GOAL_PRIORITY = 0.05f;
const float CTF_PRIORITY = 0.55f;
const float RETURN_FLAG_PRIORITY = 0.35f;
const float SNIPE_PRIORITY = 0.7f;
const float ATTACK_GOAL_PRIORITY = 0.5f;
const float DEFEND_GOAL_PRIORITY = 0.5f;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	FollowPathUser::FollowPathUser( const std::string &user )
		: mUserName( 0 )
		, mCallingThread( GM_INVALID_THREAD )
		, mDestinationIndex( 0 )
		, mPathFailed( None )
		, mPathSuccess( false )
	{
		mUserName = Utils::MakeHash32( user );
	}
	FollowPathUser::FollowPathUser( uint32_t name )
		: mUserName( name )
		, mCallingThread( GM_INVALID_THREAD )
		, mPathFailed( None )
		, mPathSuccess( false )
	{
	}

	void FollowPathUser::SetUserName( uint32_t name )
	{
		mUserName = name;
	}

	void FollowPathUser::SetUserName( const std::string &name )
	{
		SetUserName( Utils::MakeHash32( name ) );
	}

	//////////////////////////////////////////////////////////////////////////
	CaptureTheFlag::CaptureTheFlag()
		: StateChild( "CaptureTheFlag" )
		, FollowPathUser( "CaptureTheFlag" )
		, mGoalState( Idle )
		, mLastFlagState( 0 )
		, mNextMoveTime( 0 )
	{
	}

	void CaptureTheFlag::GetDebugString( std::stringstream &out )
	{
		if ( !IsActive() )
			return;

		switch ( mGoalState )
		{
			case Idle:
				out << "Idle";
				break;
			case GettingFlag:
				out << "Getting Flag";
				break;
			case CarryingToCap:
				out << "Carrying To Cap";
				break;
			case CarryingToHold:
				out << "Carrying To Hold";
				break;
			case HoldingFlag:
				out << "Holding Flag";
				break;
			default:
				break;
		}

		if ( mMapGoalFlag )
			out << std::endl << mMapGoalFlag->GetName();
		if ( mMapGoalCap )
			out << std::endl << mMapGoalCap->GetName();
	}

	MapGoal *CaptureTheFlag::GetMapGoalPtr()
	{
		return mMapGoalCap ? mMapGoalCap.get() : mMapGoalFlag.get();
	}

	void CaptureTheFlag::RenderDebug()
	{
		if ( mMapGoalFlag )
		{
			RenderBuffer::AddOBB( mMapGoalFlag->GetWorldBounds(), COLOR::GREEN );
		}
		if ( mMapGoalCap )
		{
			RenderBuffer::AddOBB( mMapGoalCap->GetWorldBounds(), COLOR::GREEN );
		}
	}

	bool CaptureTheFlag::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		switch ( GetGoalState() )
		{
			case GettingFlag:
			{
				if ( !mMapGoalFlag )
					return false;

				_final = !mMapGoalFlag->RouteTo( GetClient(), _desination );
				return true;
			}
			case CarryingToCap:
			case CarryingToHold:
			case HoldingFlag:
			{
				_final = !mMapGoalCap->RouteTo( GetClient(), _desination );
				return true;
			}
			case Idle:
				break;
		}
		return false;
	}

	float CaptureTheFlag::GetPriority()
	{
		if ( !IsActive() )
		{
			if ( !mMapGoalFlag )
			{
				MapGoalPtr available, carrying;
				GoalManager::Query qry( 0xbdeaa8d7 /* flag */, GetClient() );
				System::mInstance->mGoalManager->GetGoals( qry );
				for ( uint32_t i = 0; i < qry.mList.size(); ++i )
				{
					if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
						continue;

					if ( qry.mList[ i ]->GetOwner() == GetClient()->GetGameEntity() )
					{
						carrying = qry.mList[ i ];
						continue;
					}

					if ( !GetClient()->IsFlagGrabbable( qry.mList[ i ] ) )
						continue;

					if ( !available )
					{
						if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
							continue;

						if ( qry.mList[ i ]->GetGoalState() != S_FLAG_CARRIED )
						{
							available = qry.mList[ i ];
							continue;
						}
					}
				}

				if ( carrying )
				{
					mMapGoalFlag = carrying;
				}
				else if ( available )
				{
					mGoalState = GettingFlag;
					mMapGoalFlag = available;
				}
			}

			if ( GetClient()->DoesBotHaveFlag( mMapGoalFlag ) && !mMapGoalCap )
			{
				// if we got a flag but not cap lets not activate
				// TODO: consider an evade substate?
				if ( !LookForCapGoal( mMapGoalCap, mGoalState ) )
					mMapGoalFlag.reset();
			}
		}

		// cs: check mMapGoalCap first in case the bot randomly walks over a dropped flag
		// otherwise it will return zero priority since mMapGoalFlag is set to 'carrying' above
		if ( mMapGoalCap )
			return mMapGoalCap->GetPriorityForClient( GetClient() );
		if ( mMapGoalFlag )
			return mMapGoalFlag->GetPriorityForClient( GetClient() );

		return 0.f;
	}

	void CaptureTheFlag::Enter()
	{
		mLastFlagState = mMapGoalFlag ? mMapGoalFlag->GetGoalState() : 0;

		Tracker.InProgress = mMapGoalFlag;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
	}

	void CaptureTheFlag::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mGoalState = Idle;
		mNextMoveTime = 0;

		mMapGoalFlag.reset();
		mMapGoalCap.reset();

		Tracker.Reset();
	}

	State::StateStatus CaptureTheFlag::Update( float fDt )
	{
		switch ( GetGoalState() )
		{
			case GettingFlag:
			{
				// cs: this is a hack. if too many are in progress, bail.
				if ( mMapGoalFlag && mMapGoalFlag->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 0 )
				{
					BlackboardDelay( 10.f, mMapGoalFlag->GetSerialNum() );
					return State_Finished;
				}

				if ( mMapGoalFlag && !GetClient()->IsFlagGrabbable( mMapGoalFlag ) )
					return State_Finished;

				if ( GetClient()->DoesBotHaveFlag( mMapGoalFlag ) )
				{
					mMapGoalCap.reset();
					if ( !LookForCapGoal( mMapGoalCap, mGoalState ) )
					{
						return State_Finished;
					}
					Tracker.InUse = mMapGoalFlag;
					FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
				}
				else
				{
					/*if(mMapGoalFlag)
					{
					const int flagState = mMapGoalFlag->GetGoalState();
					if(flagState != mLastFlagState)
					{
					FINDSTATEIF(FollowPath,GetRootState(),Stop());
					return State_Finished;
					}
					}*/

					// Someone else grab it?
					if ( mMapGoalFlag->GetGoalState() == S_FLAG_CARRIED )
					{
						FINDSTATEIF( Navigator, GetRootState(), Stop() );
						return State_Finished;
					}

					const int iMyTeam = GetClient()->GetTeam();
					if ( mMapGoalFlag->GetControllingTeam( iMyTeam ) )
						return State_Finished;

					if ( DidPathSucceed() )
						GetClient()->GetSteeringSystem()->SetTarget( mMapGoalFlag->GetPosition(), mMapGoalFlag->GetRadius() );
				}

				// Doing this last so we can change state before finishing
				// flag may be disabled as part of pickup process, and we
				// want to be able to go into the cap state before that.
				if ( mMapGoalFlag && !mMapGoalFlag->IsAvailable( GetClient()->GetTeam() ) )
					return State_Finished;
				break;
			}
			case CarryingToCap:
			{
				if ( !mMapGoalCap->IsAvailable( GetClient()->GetTeam() ) )
					return State_Finished;

				if ( !GetClient()->DoesBotHaveFlag( mMapGoalFlag ) )
					return State_Finished;

				if ( DidPathSucceed() )
					GetClient()->GetSteeringSystem()->SetTarget( mMapGoalCap->GetPosition(), mMapGoalCap->GetRadius() );

				if ( mMapGoalCap && !mMapGoalCap->IsAvailable( GetClient()->GetTeam() ) )
					return State_Finished;
				break;
			}
			case CarryingToHold:
			{
				if ( DidPathSucceed() )
				{
					// shift positions?
					mGoalState = HoldingFlag;
					mNextMoveTime = IGame::GetTime() + Utils::SecondsToMilliseconds( Mathf::UnitRandom() * 10.f );
				}

				if ( mMapGoalCap && !mMapGoalCap->IsAvailable( GetClient()->GetTeam() ) )
					return State_Finished;
				break;
			}
			case HoldingFlag:
			{
				// We don't care about path failure in here, so we return instead of letting it
				// check mPathFailed below.
				if ( IGame::GetTime() >= mNextMoveTime )
				{
					mGoalState = CarryingToHold;
					FINDSTATEIF( Navigator, GetRootState(), Goto( this ) );
				}

				if ( mMapGoalCap && !mMapGoalCap->IsAvailable( GetClient()->GetTeam() ) )
					return State_Finished;

				return State_Busy;
			}
			case Idle:
			{
				return State_Finished;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			if ( mMapGoalFlag )
				BlackboardDelay( 10.f, mMapGoalFlag->GetSerialNum() );
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		return State_Busy;
	}

	bool CaptureTheFlag::LookForCapGoal( MapGoalPtr &ptr, GoalState &st )
	{
		//////////////////////////////////////////////////////////////////////////
		{
			GoalManager::Query qry( 0xc9326a43 /* cappoint */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			if ( qry.GetBest( ptr ) )
			{
				mGoalState = CarryingToCap;
				return true;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		{
			GoalManager::Query qry( 0xc7ab68ba /* caphold */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			if ( qry.GetBest( ptr ) )
			{
				mGoalState = CarryingToHold;
				return true;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	//Snipe::Snipe()
	//	: StateChild("Snipe")
	//	, FollowPathUser("Snipe")
	//	, mMinCampTime(6.0)
	//	, mMaxCampTime(10.0)
	//	, mExpireTime(0)
	//	, mStance(StanceStand)
	//	, mNextScanTime(0)
	//{
	//}

	//void Snipe::GetDebugString(std::stringstream &out)
	//{
	//	out << (mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void Snipe::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(mMapGoal->GetWorldBounds(), COLOR::ORANGE, MIN_RENDER_TIME);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),mMapGoal->GetPosition(),COLOR::GREEN,MIN_RENDER_TIME);
	//		mTargetZone.RenderDebug();
	//	}
	//}

	//// FollowPathUser functions.
	//bool Snipe::GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt)
	//{
	//	if(mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool Snipe::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mAimPosition;
	//	return true;
	//}

	//void Snipe::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws && ws->CurrentWeaponIs(mNonScopedWeaponId) || ws->CurrentWeaponIs(mScopedWeaponId))
	//	{
	//		ws->ChargeWeapon();
	//		ws->ZoomWeapon();
	//	}
	//}

	//float Snipe::GetPriority()
	//{
	//	if(!GetClient()->CanBotSnipe())
	//		return 0.f;

	//	if(IsActive())
	//		return GetLastPriority();

	//	mMapGoal.reset();

	//	GoalManager::Query qry(0xe8fbadc0 /* snipe */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	qry.GetBest(mMapGoal);

	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void Snipe::Enter()
	//{
	//	mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//	GetClient()->GetSniperWeapon(mNonScopedWeaponId, mScopedWeaponId);

	//	mExpireTime = 0;

	//	mMapGoal->GetProperty("Stance",mStance);
	//	mMapGoal->GetProperty("MinCampTime",mMinCampTime);
	//	mMapGoal->GetProperty("MaxCampTime",mMaxCampTime);

	//	Tracker.InProgress = mMapGoal;
	//	mTargetZone.Restart(256.f);
	//	mNextScanTime = 0;

	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void Snipe::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//	mMapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus Snipe::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(!Tracker.InUse && mMapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		// Only hang around here for a certain amount of time
	//		if(mExpireTime==0)
	//		{
	//		 mExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(mMinCampTime.GetMs(),mMaxCampTime.GetMs());
	//			Tracker.InUse = mMapGoal;
	//		}
	//		else if(IGame::GetTime() > mExpireTime)
	//		{
	//			// Delay it from being used for a while.
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));
	//		FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::Medium, GetNameHash(), mScopedWeaponId));

	//		mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//		mTargetZone.Update(GetClient());

	//		if(mNextScanTime <= IGame::GetTime())
	//		{
	//			mNextScanTime = IGame::GetTime()+Mathf::IntervalRandomInt(2000,4000);
	//			mTargetZone.UpdateAimPosition();
	//		}
	//
	//		if(mTargetZone.HasAim())
	//			mAimPosition = mTargetZone.GetAimPosition();

	//		GetClient()->GetSteeringSystem()->SetTarget(mMapGoal->GetPosition(), mMapGoal->GetRadius());

	//		if(mStance==StanceProne)
	//			GetClient()->PressButton(BOT_BUTTON_PRONE);
	//		else if(mStance==StanceCrouch)
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);
	//	}
	//	return State_Busy;
	//}
	//////////////////////////////////////////////////////////////////////////

	ReturnTheFlag::ReturnTheFlag()
		: StateChild( "ReturnTheFlag" )
		, FollowPathUser( "ReturnTheFlag" )
	{
		mLastGoalPosition = Vector3f::ZERO;
	}

	/*void ReturnTheFlag::GetDebugString(std::stringstream &out)
	{
	return "";
	}*/

	MapGoal *ReturnTheFlag::GetMapGoalPtr()
	{
		return mMapGoal.get();
	}

	void ReturnTheFlag::RenderDebug()
	{
		if ( mMapGoal )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::GREEN ); // todo: convert to renderbuffer
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetWorldUsePoint(), COLOR::MAGENTA );
		}
	}

	float ReturnTheFlag::GetPriority()
	{
		if ( !mMapGoal )
		{
			if ( !mMapGoal )
			{
				GoalManager::Query qry( 0xa06840e5 /* flagreturn */, GetClient() );
				System::mInstance->mGoalManager->GetGoals( qry );
				for ( uint32_t i = 0; i < qry.mList.size(); ++i )
				{
					if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
						continue;

					if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INUSE ) < 1 )
						continue;

					const int iGoalState = qry.mList[ i ]->GetGoalState();

					if ( iGoalState == S_FLAG_DROPPED || iGoalState == S_FLAG_CARRIED )
					{
						mMapGoal = qry.mList[ i ];
						break;
					}
					else if ( iGoalState == S_FLAG_AT_BASE )
					{
						continue;
					}
					else if ( GetClient()->IsFlagGrabbable( qry.mList[ i ] ) )
					{
						mMapGoal = qry.mList[ i ];
						break;
					}
				}
			}
		}

		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void ReturnTheFlag::Enter()
	{
		mMapGoalProg = mMapGoal;
		mLastGoalPosition = mMapGoal->GetWorldUsePoint();
		FINDSTATEIF( Navigator, GetRootState(), Goto( this, mLastGoalPosition, mMapGoal->GetRadius() ) );
	}

	void ReturnTheFlag::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mMapGoalProg.Reset();
		mMapGoal.reset();
		mLastGoalPosition = Vector3f::ZERO;
	}

	State::StateStatus ReturnTheFlag::Update( float fDt )
	{
		//////////////////////////////////////////////////////////////////////////
		if ( DidPathFail() )
		{
			// Delay it from being used for a while.
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( mMapGoal->GetDeleteMe() )
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		// Re-plan if it has moved a bit from last position
		// Predict path?
		Vector3f vUpdatedPosition = mMapGoal->GetWorldUsePoint();
		if ( SquaredLength( mLastGoalPosition, vUpdatedPosition ) > Mathf::Sqr( 100.f ) )
		{
			mLastGoalPosition = vUpdatedPosition;
			FINDSTATEIF( Navigator, GetRootState(), Goto( this, mLastGoalPosition, mMapGoal->GetRadius() ) );
		}

		if ( mMapGoal->GetGoalState() == S_FLAG_AT_BASE || !GetClient()->IsFlagGrabbable( mMapGoal ) )
			return State_Finished;

		FINDSTATE( follow, Navigator, GetRootState() );
		if ( follow != NULL && !follow->IsActive() )
			follow->Goto( this, mLastGoalPosition, mMapGoal->GetRadius() );

		return State_Busy;
	}
	
	//////////////////////////////////////////////////////////////////////////

	Roam::Roam()
		: StateChild( "Roam" )
		, FollowPathUser( "Roam" )
	{
	}

	float Roam::GetPriority()
	{
		return ROAM_GOAL_PRIORITY;
	}

	void Roam::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );
	}

	State::StateStatus Roam::Update( float fDt )
	{
		FINDSTATE( follow, Navigator, GetRootState() );
		if ( follow != NULL && !follow->IsActive() )
			follow->GotoRandomPt( this );
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	/*HighLevel::HighLevel()
		: StatePrioritized( "HighLevel" )
	{
		AppendState( new CaptureTheFlag );
		AppendState( new ReturnTheFlag );

		AppendState( new Roam );
	}*/

	//////////////////////////////////////////////////////////////////////////

	Aimer::AimRequest::AimRequest()
	{
		Reset();
	}

	void Aimer::AimRequest::Reset()
	{
		mOwner = 0;
		mPriority = Priority::Zero;
		mAimVector = Vector3f::ZERO;
		mAimType = WorldPosition;
		mAimerUser = NULL;
	}

	Aimer::Aimer()
		: StateChild( "Aimer" )
		, mBestAimOwner( 0 )
	{
		for ( int32_t i = 0; i < MaxAimRequests; ++i )
			mAimRequests[ i ].Reset();

		// Initialize default aim request.
		mAimRequests[ 0 ].mPriority = Priority::Min;
		mAimRequests[ 0 ].mAimerUser = 0;
		mAimRequests[ 0 ].mAimVector = Vector3f::UNIT_X;
		mAimRequests[ 0 ].mAimType = WorldFacing;
		mAimRequests[ 0 ].mOwner = GetNameHash();
	}

	Aimer::AimRequest *Aimer::FindAimRequest( uint32_t _owner )
	{
		int iOpen = -1;
		for ( int32_t i = 0; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mOwner == _owner )
			{
				iOpen = i;
				break;
			}
			if ( mAimRequests[ i ].mPriority == Priority::Zero )
			{
				if ( iOpen == -1 )
					iOpen = i;
			}
		}

		if ( iOpen != -1 )
		{
			return &mAimRequests[ iOpen ];
		}
		return NULL;
	}

	bool Aimer::AddAimRequest( Priority::ePriority _prio, AimerUser *_owner, uint32_t _ownername )
	{
		AimRequest *pRequest = FindAimRequest( _ownername );
		if ( pRequest )
		{
			pRequest->mPriority = _prio;
			pRequest->mOwner = _ownername;
			pRequest->mAimType = UserCallback;
			pRequest->mAimerUser = _owner;
			return true;
		}
		return false;
	}

	bool Aimer::AddAimFacingRequest( Priority::ePriority _prio, uint32_t _owner, const Vector3f &_v )
	{
		AimRequest *pRequest = FindAimRequest( _owner );
		if ( pRequest )
		{
			pRequest->mPriority = _prio;
			pRequest->mOwner = _owner;
			pRequest->mAimType = WorldFacing;
			pRequest->mAimerUser = NULL;
			pRequest->mAimVector = _v;
			return true;
		}
		return false;
	}

	bool Aimer::AddAimPositionRequest( Priority::ePriority _prio, uint32_t _owner, const Vector3f &_v )
	{
		AimRequest *pRequest = FindAimRequest( _owner );
		if ( pRequest )
		{
			pRequest->mPriority = _prio;
			pRequest->mOwner = _owner;
			pRequest->mAimType = WorldPosition;
			pRequest->mAimerUser = NULL;
			pRequest->mAimVector = _v;
			return true;
		}
		return false;
	}

	bool Aimer::AddAimMoveDirRequest( Priority::ePriority _prio, uint32_t _owner )
	{
		AimRequest *pRequest = FindAimRequest( _owner );
		if ( pRequest )
		{
			pRequest->mPriority = _prio;
			pRequest->mOwner = _owner;
			pRequest->mAimType = MoveDirection;
			pRequest->mAimerUser = NULL;
			return true;
		}
		return false;
	}

	void Aimer::ReleaseAimRequest( uint32_t _owner )
	{
		for ( int32_t i = 0; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mOwner == _owner )
			{
				mAimRequests[ i ].Reset();
				break;
			}
		}
	}

	void Aimer::UpdateAimRequest( uint32_t _owner, const Vector3f &_pos )
	{
		for ( int32_t i = 0; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mOwner == _owner )
			{
				mAimRequests[ i ].mAimVector = _pos;
				break;
			}
		}
	}

	void Aimer::GetDebugString( std::stringstream &out )
	{
		out << Utils::HashToString( mBestAimOwner );
	}

	int Aimer::GetAllRequests( AimRequest *_records, int _max )
	{
		int iNum = 0;
		for ( int i = 0; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mPriority == Priority::Zero )
				continue;

			_records[ iNum++ ] = mAimRequests[ i ];

			if ( iNum >= _max - 1 )
				break;
		}
		return iNum;
	}

	void Aimer::RenderDebug()
	{
		const Aimer::AimRequest *pCurrent = Aimer::GetHighestAimRequest( false );

		for ( int32_t i = 1; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mPriority > Priority::Zero )
			{
				obColor c = pCurrent == &mAimRequests[ i ] ? COLOR::MAGENTA : COLOR::WHITE;
				if ( mAimRequests[ i ].mAimType != WorldFacing )
					RenderBuffer::AddLine( GetClient()->GetEyePosition(), mAimRequests[ i ].mAimVector, c );
				else
					RenderBuffer::AddLine( GetClient()->GetEyePosition(), GetClient()->GetEyePosition() + GetClient()->GetFacingVector() * 128.f, c );
			}
		}
	}

	Aimer::AimRequest *Aimer::GetHighestAimRequest( bool _clearontarget )
	{
		int iBestAim = 0;
		for ( int32_t i = 1; i < MaxAimRequests; ++i )
		{
			if ( mAimRequests[ i ].mPriority > mAimRequests[ iBestAim ].mPriority )
				iBestAim = i;
		}
		return &mAimRequests[ iBestAim ];
	}

	void Aimer::OnSpawn()
	{
		mAimRequests[ 0 ].mAimVector = GetClient()->GetFacingVector();
	}

	void Aimer::Enter()
	{
	}

	void Aimer::Exit()
	{
	}

	State::StateStatus Aimer::Update( float fDt )
	{
		rmt_ScopedCPUSample( AimerUpdate );

		{
			AimRequest *curAim = GetHighestAimRequest( true );
			mBestAimOwner = curAim->mOwner;
			switch ( curAim->mAimType )
			{
				case WorldPosition:
				{
					GetClient()->TurnTowardPosition( curAim->mAimVector );
					break;
				}
				case WorldFacing:
				{
					GetClient()->TurnTowardFacing( curAim->mAimVector );
					break;
				}
				case MoveDirection:
				{
					FINDSTATE( nav, Navigator, GetRootState() );
					if ( nav != NULL && nav->IsActive() )
					{
						curAim->mAimVector = nav->GetLookAheadPt();
						GetClient()->TurnTowardPosition( curAim->mAimVector );
						break;
					}
					FINDSTATE( steer, SteeringSystem, GetParent() );
					if ( steer )
					{
						curAim->mAimVector = steer->GetTarget();
						curAim->mAimVector.Z() = GetClient()->GetEyePosition().Z();
						GetClient()->TurnTowardPosition( curAim->mAimVector );
					}
					break;
				}
				case UserCallback:
				{
					rmt_ScopedCPUSample( AimerCallback );
					if ( curAim->mAimerUser && curAim->mAimerUser->GetAimPosition( curAim->mAimVector ) && GetClient()->TurnTowardPosition( curAim->mAimVector ) )
					{
						curAim->mAimerUser->OnTarget();
					}
					break;
				}
			}
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	LookAround::LookAround()
		: StateChild( "LookAround" )
		, mNextLookTime( 0 )
	{
	}

	int LookAround::GetNextLookTime()
	{
		return IGame::GetTime() + Utils::SecondsToMilliseconds( Mathf::IntervalRandom( 5.f, 15.f ) );
	}

	void LookAround::OnSpawn()
	{
		mNextLookTime = GetNextLookTime();
	}

	float LookAround::GetPriority()
	{
		if ( IGame::GetTime() > mNextLookTime )
		{
			FINDSTATE( nav, Navigator, GetParent() );
			if ( nav )
			{
				Path::PathPoint pp;

				if ( nav->IsMoving() && !nav->IsOnCustomLink() )
				{
					mNextLookTime = GetNextLookTime();
					return 0.f;
				}
			}
			return 1.f;
		}
		return 0.0f;
	}

	void LookAround::Enter()
	{
		//Quaternionf q(Vector3f::UNIT_Z, Mathf::IntervalRandom(-Mathf::PI, Mathf::PI));
		//Vector3f vStartAim = q.Rotate(GetClient()->GetFacingVector());
		FINDSTATE( aim, Aimer, GetParent() );
		if ( aim )
			aim->AddAimFacingRequest( Priority::Low, GetNameHash(), -GetClient()->GetFacingVector() );
	}

	void LookAround::Exit()
	{
		FINDSTATEIF( Aimer, GetParent(), ReleaseAimRequest( GetNameHash() ) );
		mNextLookTime = GetNextLookTime();
	}

	State::StateStatus LookAround::Update( float fDt )
	{
		return ( GetStateTime() > 1.f ) ? State_Finished : State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	MotorControl::MotorControl()
		: StateSimultaneous( "MotorControl" )
	{
		AppendState( new Navigator );
		AppendState( new SteeringSystem );
		AppendState( new Aimer );
		AppendState( new LookAround );
		//AppendState(new ClimbLadder);
		//AppendState(new RideElevator);
	}

	//////////////////////////////////////////////////////////////////////////

	LowLevel::LowLevel()
		: StateSimultaneous( "LowLevel" )
	{
		AppendState( new MotorControl );
		AppendState( new WeaponSystem );
		AppendState( new TargetingSystem );
		AppendState( new SensoryMemory );
		AppendState( new ProximityWatcher );
		//AppendState(new FloodFiller);
	}

	//////////////////////////////////////////////////////////////////////////

	Main::Main()
		: StateSimultaneous( "Main" ),
		mOnSpawnCalled( false )
	{
		AppendState( new LowLevel );
		AppendState( new HighLevel );
	}

	float Main::GetPriority()
	{
		if ( IGame::GetGameState() != IGame::GetLastGameState() )
			return 0.f;

		//return StateSimultaneous::GetPriority();
		return 1.0f;
	}

	void Main::Enter()
	{
		if ( !mOnSpawnCalled )
			GetRootState()->OnSpawn();
		mOnSpawnCalled = false;
		
		GetClient()->SendEvent( EvSpawn::Msg() );
	}

	void Main::OnSpawn()
	{
		mOnSpawnCalled = true;
		State::OnSpawn();
	}

	//////////////////////////////////////////////////////////////////////////

	Dead::Dead()
		: StateChild( "Dead" )
		, bForceActivate( true )
	{
	}

	float Dead::GetPriority()
	{
		if ( bForceActivate )
		{
			bForceActivate = false;
			return 1.f;
		}
		return !gEngineFuncs->IsAlive( GetClient()->GetGameEntity() ) ? 1.f : 0.f;
	}

	State::StateStatus Dead::Update( float fDt )
	{
		if ( IGame::GetFrameNumber() & 2 )
		{
			GetClient()->PressButton( BOT_BUTTON_RESPAWN );
		}
		GetClient()->SetMovementVector( Vector3f::ZERO );
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	Warmup::Warmup()
		: StateChild( "Warmup" )
	{
	}

	float Warmup::GetPriority()
	{
		const GameState gameState = gEngineFuncs->GetGameState();
		return ( gameState != GAME_STATE_PLAYING && gameState != GAME_STATE_SUDDENDEATH ) ? 1.f : 0.f;
	}

	State::StateStatus Warmup::Update( float fDt )
	{
		// need to do something special here?
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	Root::Root() : StateFirstAvailable( "Root" )
	{
		AppendState( new Dead );
		AppendState( new Main );
	}

	//////////////////////////////////////////////////////////////////////////

	ProximityWatcher::ProximityWatcher()
		: StateChild( "ProximityWatcher" )
	{
		for ( int i = 0; i < MaxTriggers; ++i )
		{
			mTriggers[ i ].mOwnerState = 0;
			mTriggers[ i ].mDeleteOnFire = true;
		}
	}
	void ProximityWatcher::RenderDebug()
	{
		for ( int i = 0; i < MaxTriggers; ++i )
		{
			if ( mTriggers[ i ].mSensoryFilter )
			{
				for ( int p = 0; p < mTriggers[ i ].mSensoryFilter->GetNumPositions(); ++p )
				{
					float r = std::max( mTriggers[ i ].mSensoryFilter->GetMaxDistance(), 10.f );

					RenderBuffer::AddCircle( mTriggers[ i ].mSensoryFilter->GetPosition( p ),
						r, COLOR::MAGENTA );
				}
			}
		}
	}
	void ProximityWatcher::AddWatch( uint32_t _owner, FilterPtr _filter, bool _fireonce )
	{
		for ( int i = 0; i < MaxTriggers; ++i )
		{
			if ( !mTriggers[ i ].mSensoryFilter )
			{
				mTriggers[ i ].mOwnerState = _owner;
				mTriggers[ i ].mSensoryFilter = _filter;
				mTriggers[ i ].mDeleteOnFire = _fireonce;
			}
		}
	}
	void ProximityWatcher::RemoveWatch( uint32_t _owner )
	{
		for ( int i = 0; i < MaxTriggers; ++i )
		{
			if ( mTriggers[ i ].mSensoryFilter && mTriggers[ i ].mOwnerState == _owner )
			{
				mTriggers[ i ].mOwnerState = 0;
				mTriggers[ i ].mSensoryFilter.reset();
				mTriggers[ i ].mDeleteOnFire = false;
			}
		}
	}

	float ProximityWatcher::GetPriority()
	{
		for ( int i = 0; i < MaxTriggers; ++i )
		{
			if ( mTriggers[ i ].mSensoryFilter )
				return 1.f;
		}
		return 0.f;
	}

	State::StateStatus ProximityWatcher::Update( float fDt )
	{
		SensoryMemory *sensory = GetClient()->GetSensoryMemory();

		for ( int i = 0; i < MaxTriggers; ++i )
		{
			if ( mTriggers[ i ].mSensoryFilter )
			{
				mTriggers[ i ].mSensoryFilter->Reset();

				// Set up ignore list so we don't get events every frame from
				// entities that are still inside the trigger
				// basically only want the event when they enter.
				//.mTriggers[i].mSensoryFilter->ResetIgnoreEntity();

				sensory->QueryMemory( *mTriggers[ i ].mSensoryFilter );

				if ( mTriggers[ i ].mSensoryFilter->DetectedSomething() )
				{
					EvProximityTrigger::Msg event;
					event.mData.mOwnerState = mTriggers[ i ].mOwnerState;
					event.mData.mEntity = mTriggers[ i ].mSensoryFilter->GetBestEntity();
					event.mData.mPosition = mTriggers[ i ].mSensoryFilter->GetTriggerPosition();
					GetClient()->SendEvent( event );

					if ( mTriggers[ i ].mDeleteOnFire )
					{
						mTriggers[ i ].mOwnerState = 0;
						mTriggers[ i ].mSensoryFilter.reset();
						mTriggers[ i ].mDeleteOnFire = false;
					}
				}
			}
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	TrackTargetZone::TrackTargetZone()
		: mRadius( 0.f )
	{
		Restart( 0.f );
	}
	void TrackTargetZone::Restart( float _radius )
	{
		mRadius = _radius;
		mValidAim = false;

		mLastTarget.Reset();
		for ( int i = 0; i < MaxTargetZones; ++i )
		{
			mTargetZones[ i ].mInUse = false;
			mTargetZones[ i ].mTargetCount = 0;
			mTargetZones[ i ].mPosition = Vector3f::ZERO;
		}
	}

	void TrackTargetZone::UpdateAimPosition()
	{
		int iNumZones = 0;
		float fTotalWeight = 0.f;
		for ( int i = 0; i < MaxTargetZones; ++i )
		{
			if ( mTargetZones[ i ].mInUse )
			{
				fTotalWeight += (float)mTargetZones[ i ].mTargetCount;
				iNumZones++;
			}
		}

		float fRand = Mathf::IntervalRandom( 0.f, fTotalWeight );
		for ( int i = 0; i < MaxTargetZones; ++i )
		{
			if ( mTargetZones[ i ].mInUse )
			{
				fRand -= (float)mTargetZones[ i ].mTargetCount;
				if ( fRand < 0.f )
				{
					mAimPosition = mTargetZones[ i ].mPosition;
					mValidAim = true;
					return;
				}
			}
		}
		mValidAim = false;
	}

	void TrackTargetZone::RenderDebug()
	{
		for ( int i = 0; i < MaxTargetZones; ++i )
		{
			if ( mTargetZones[ i ].mInUse )
			{
				RenderBuffer::AddCircle(
					mTargetZones[ i ].mPosition,
					mRadius,
					COLOR::MAGENTA );

				RenderBuffer::AddString3d(
					mTargetZones[ i ].mPosition,
					COLOR::WHITE,
					va( "%d", mTargetZones[ i ].mTargetCount ).c_str() );
			}
		}
	}

	void TrackTargetZone::Update( Client *_client )
	{
		const MemoryRecord *pTargetRec = _client->GetTargetingSystem()->GetCurrentTargetRecord();
		if ( pTargetRec != NULL && pTargetRec->GetEntity() != mLastTarget )
		{
			for ( int i = 0; i < MaxTargetZones; ++i )
			{
				// if new target, add it to the zone counter
				bool bFound = false;
				TargetZone *pFreeZone = 0;
				for ( int z = 0; z < MaxTargetZones; ++z )
				{
					if ( mTargetZones[ z ].mInUse )
					{
						const float fSqDistance = SquaredLength( mTargetZones[ z ].mPosition, pTargetRec->GetLastSensedPosition() );

						if ( fSqDistance < Mathf::Sqr( mRadius ) )
						{
							mTargetZones[ z ].mTargetCount++;
							bFound = true;
						}
					}
					else
					{
						if ( !pFreeZone )
							pFreeZone = &mTargetZones[ z ];
					}
				}

				if ( !bFound && pFreeZone )
				{
					pFreeZone->mInUse = true;
					pFreeZone->mPosition = pTargetRec->GetLastSensedPosition();
					pFreeZone->mTargetCount = 1;
				}
			}

			mLastTarget = pTargetRec->GetEntity();
		}

		if ( mLastTarget.IsValid() && !gEngineFuncs->IsAlive( mLastTarget ) )
			mLastTarget.Reset();
	}

	//////////////////////////////////////////////////////////////////////////

	void FloodFiller::Connection::Reset()
	{
		Destination = 0;
		Jump = false;
	}

	void FloodFiller::Node::Init( int16_t _X, int16_t _Y, float _Height, bool _Open /* = false */ )
	{
		MinOffset.X = MaxOffset.X = _X;
		MinOffset.Y = MaxOffset.Y = _Y;

		Height = _Height;
		Open = _Open;

		OpenNess = 0;

		for ( int i = 0; i < DIR_NUM; ++i )
			Connections[ i ].Reset();

		NearObject = false;
		NearEdge = false;
		Sectorized = false;
		SectorId = 0;
	}

	//////////////////////////////////////////////////////////////////////////

	const float CHARACTER_HEIGHT = 64.f;
	const float CHARACTER_STEPHEIGHT = 18.f;
	const float CHARACTER_JUMPHEIGHT = 60.f;
	const float MIN_COVER_HEIGHT = 40.f;
	const float MAX_COVER_HEIGHT = 60.f;

	FloodFiller::FloodFiller()
		: StateChild( "FloodFiller" )
	{
		Reset();
		State = FillDone;
	}

	Vector3f FloodFiller::_GetNodePosition( const Node &_Node )
	{
		Vector3f vNodePos = Start;

		const float nX = (float)( _Node.MinOffset.X + _Node.MaxOffset.X ) * 0.5f;
		const float nY = (float)( _Node.MinOffset.Y + _Node.MaxOffset.Y ) * 0.5f;

		vNodePos.X() += ( ( Radius*2.f ) * nX );
		vNodePos.Y() += ( ( Radius*2.f ) * nY );
		vNodePos.Z() = _Node.Height;

		return vNodePos;
	}

	FloodFiller::Node *FloodFiller::_NodeExists( int16_t _X, int16_t _Y, float _Height )
	{
		for ( int i = 0; i < FreeNode; ++i )
		{
			if ( Nodes[ i ].MinOffset.X >= _X && Nodes[ i ].MinOffset.Y >= _Y &&
				Nodes[ i ].MaxOffset.X <= _X && Nodes[ i ].MaxOffset.Y <= _Y )
			{
				if ( Mathf::FAbs( _Height - Nodes[ i ].Height ) < CHARACTER_HEIGHT )
					return &Nodes[ i ];
			}
		}
		return NULL;
	}

	FloodFiller::Node *FloodFiller::_NextOpenNode()
	{
		for ( int i = 0; i < FreeNode; ++i )
		{
			if ( Nodes[ i ].Open )
			{
				return &Nodes[ i ];
			}
		}
		return NULL;
	}

	bool FloodFiller::_TestNode( const Node *_Node )
	{
		Vector3f vPos = _GetNodePosition( *_Node );

		obTraceResult tr;
		EngineFuncs::TraceLine( tr,
			vPos,
			vPos,
			&FloodBlock,
			TR_MASK_FLOODFILL,
			-1,
			false );

		return ( tr.mFraction == 1.0f );
	}

	bool FloodFiller::_DropToGround( Node *_Node )
	{
		Vector3f vPos = _GetNodePosition( *_Node );

		const float fDropDistance = 512.f;

		AABB bounds = FloodBlock;
		bounds.mMaxs[ 2 ] = CHARACTER_HEIGHT - CHARACTER_JUMPHEIGHT;

		static float START_OFFSET = CHARACTER_JUMPHEIGHT + 10.f;
		static float END_OFFSET = -fDropDistance;

		obTraceResult tr;
		EngineFuncs::TraceLine( tr,
			vPos + Vector3f( 0.f, 0.f, START_OFFSET ),
			vPos + Vector3f( 0.f, 0.f, END_OFFSET ),
			&bounds,
			TR_MASK_FLOODFILL,
			-1,
			false );

		bool bGood = true;

		// adjust node height
		if ( tr.mFraction < 1.f )
			_Node->Height = tr.mEndpos[ 2 ] + CHARACTER_STEPHEIGHT;

		if ( tr.mStartSolid )
			bGood = false;

		return bGood;
	}

	void FloodFiller::_MakeConnection( Node *_NodeA, Node *_NodeB, Direction _Dir )
	{
		{
			_NodeA->Connections[ _Dir ].Destination = _NodeB;
			_NodeA->Connections[ _Dir ].Jump = ( _NodeB->Height - _NodeA->Height ) >= CHARACTER_STEPHEIGHT;

			if ( ( _NodeB->Height - _NodeA->Height ) >= MIN_COVER_HEIGHT &&
				( _NodeB->Height - _NodeA->Height ) <= MAX_COVER_HEIGHT )
			{
				_NodeA->Connections[ _Dir ].Cover |= ( 1 << _Dir );
			}
		}

		{
			//// connect the opposite direction
			//Direction Opposite[DIR_NUM] =
			//{
			//	DIR_SOUTH,
			//	DIR_WEST,
			//	DIR_NORTH,
			//	DIR_EAST
			//};

			//_Dir = Opposite[_Dir];
			//_NodeB->Connections[_Dir].Destination = _NodeB;
			//_NodeB->Connections[_Dir].Jump = (_NodeA->Height - _NodeB->Height) >= CHARACTER_STEPHEIGHT;
			//_NodeB->Connections[_Dir].Cover = (_NodeA->Height - _NodeB->Height) >= MIN_COVER_HEIGHT &&
			//	(_NodeA->Height - _NodeB->Height) <= MAX_COVER_HEIGHT;
		}
	}

	void FloodFiller::_FillOpenNess( bool _ResetAll )
	{
		//////////////////////////////////////////////////////////////////////////
		for ( int i = 0; i < FreeNode; ++i )
		{
			if ( _ResetAll )
				Nodes[ i ].OpenNess = 0;

			if ( !Nodes[ i ].Sectorized )
			{
				if ( Nodes[ i ].NearEdge || Nodes[ i ].NearObject )
					Nodes[ i ].OpenNess = 1;

				// bordering another sector also resets
				for ( int d = 0; d < DIR_NUM; ++d )
				{
					if ( Nodes[ i ].Connections[ d ].Destination )
					{
						if ( Nodes[ i ].Connections[ d ].Destination->Sectorized )
							Nodes[ i ].OpenNess = 1;
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		uint8_t iCurrentDistance = 1;
		bool bKeepGoing = true;
		while ( bKeepGoing )
		{
			bKeepGoing = false;

			for ( int i = 0; i < FreeNode; ++i )
			{
				if ( Nodes[ i ].OpenNess == iCurrentDistance )
				{
					for ( int d = 0; d < DIR_NUM; ++d )
					{
						if ( Nodes[ i ].Connections[ d ].Destination )
						{
							if ( !Nodes[ i ].Connections[ d ].Destination->OpenNess )
							{
								bKeepGoing = true;
								Nodes[ i ].Connections[ d ].Destination->OpenNess = Nodes[ i ].OpenNess + 1;
							}
						}
					}
				}
			}
			++iCurrentDistance;
		}
	}

	bool FloodFiller::_CanMergeWith( Node *_Node, Node *_WithNode )
	{
		if ( !_WithNode )
			return false;

		if ( Mathf::FAbs( _WithNode->Height - _Node->Height ) >= CHARACTER_STEPHEIGHT )
			return false;

		if ( _WithNode->SectorId != 0 )
			return false;

		return true;
	}

	void FloodFiller::_MergeSectors()
	{
		_FillOpenNess( true );

		uint16_t NextSectorId = 0;

		while ( true )
		{
			NextSectorId++;
			//////////////////////////////////////////////////////////////////////////
			Node *pLargestNode = 0;
			uint8_t iLargestOpenness = 0;
			for ( int i = 0; i < FreeNode; ++i )
			{
				if ( !Nodes[ i ].Sectorized && Nodes[ i ].OpenNess > iLargestOpenness )
				{
					pLargestNode = &Nodes[ i ];
					iLargestOpenness = Nodes[ i ].OpenNess;
				}
			}
			if ( !pLargestNode )
				break;
			//////////////////////////////////////////////////////////////////////////
			if ( pLargestNode )
			{
				/*RenderBuffer::AddString3d(
				_GetNodePosition(*pLargestNode) + Vector3f(0,0,64),
				30.f,
				"%d",NextSectorId);*/

				pLargestNode->Sectorized = true;
				pLargestNode->SectorId = NextSectorId;

				Node s;
				s.Init( pLargestNode->MinOffset.X, pLargestNode->MinOffset.Y, pLargestNode->Height, false );

				int ExpansionMask = ( 1 << DIR_NORTH ) | ( 1 << DIR_EAST ) | ( 1 << DIR_SOUTH ) | ( 1 << DIR_WEST );

				int NumMergeNodes = 0;
				Node *MergeNodes[ 2048 ] = {};

				MergeNodes[ NumMergeNodes++ ] = pLargestNode;

				while ( ExpansionMask )
				{
					for ( int d = DIR_NORTH; d < DIR_NUM; ++d )
					{
						int NumMergeEdge = 0;
						Node *MergeEdge[ 2048 ] = {};

						if ( ExpansionMask & ( 1 << d ) )
						{
							switch ( d )
							{
								case DIR_NORTH:
								case DIR_SOUTH:
								{
									//for(int c = 0)

									int16_t y = 0;
									if ( d == DIR_NORTH )
										y = s.MaxOffset.Y + 1;
									else
										y = s.MinOffset.Y - 1;
									for ( int16_t x = s.MinOffset.X; x <= s.MaxOffset.X; ++x )
									{
										Node *pNext = _NodeExists( x, y, s.Height );
										if ( !_CanMergeWith( pLargestNode, pNext ) )
										{
											/*Node tn;
											tn.MinOffset.X = tn.MaxOffset.X = x;
											tn.MinOffset.Y = tn.MaxOffset.Y = y;
											tn.Height = s.Height;
											Vector3f vMissing = _GetNodePosition(tn);
											RenderBuffer::AddLine(
											vMissing,
											vMissing + Vector3f(0,0,64),
											COLOR::ORANGE,
											20.f);*/

											ExpansionMask &= ~( 1 << d );
											break;
										}

										MergeEdge[ NumMergeEdge++ ] = pNext;
									}
									break;
								}
								case DIR_EAST:
								case DIR_WEST:
								{
									int16_t x = 0;
									if ( d == DIR_EAST )
										x = s.MaxOffset.X + 1;
									else
										x = s.MinOffset.X - 1;
									for ( int16_t y = s.MinOffset.Y; y <= s.MaxOffset.Y; ++y )
									{
										Node *pNext = _NodeExists( x, y, s.Height );
										if ( !_CanMergeWith( pLargestNode, pNext ) )
										{
											/*Node tn;
											tn.MinOffset.X = tn.MaxOffset.X = x;
											tn.MinOffset.Y = tn.MaxOffset.Y = y;
											tn.Height = s.Height;
											Vector3f vMissing = _GetNodePosition(tn);
											RenderBuffer::AddLine(
											vMissing,
											vMissing + Vector3f(0,0,64),
											COLOR::ORANGE,
											20.f);*/

											ExpansionMask &= ~( 1 << d );
											break;
										}

										MergeEdge[ NumMergeEdge++ ] = pNext;
									}
									break;
								}
							}
							//////////////////////////////////////////////////////////////////////////
							if ( ExpansionMask & ( 1 << d ) )
							{
								// adjust the bounds
								switch ( d )
								{
									case DIR_NORTH:
										s.MaxOffset.Y++;
										break;
									case DIR_SOUTH:
										s.MinOffset.Y--;
										break;
									case DIR_EAST:
										s.MaxOffset.X++;
										break;
									case DIR_WEST:
										s.MinOffset.X--;
										break;
								}

								// add to the merge list
								for ( int m = 0; m < NumMergeEdge; ++m )
								{
									MergeEdge[ m ]->Sectorized = true;
									MergeEdge[ m ]->SectorId = NextSectorId;
									MergeNodes[ NumMergeNodes++ ] = MergeEdge[ m ];
								}
							}
						}
					}
				}

				_FillOpenNess( false );

				// done merging sector, debug render
				Node *EdgeNodes[ DIR_NUM ][ 2048 ] = {};
				int NumEdges[ DIR_NUM ] = {};

				for ( int s = 0; s < NumMergeNodes; ++s )
				{
					for ( int d = DIR_NORTH; d < DIR_NUM; ++d )
					{
						bool bHasDirectionConnection = false;
						if ( MergeNodes[ s ]->Connections[ d ].Destination )
						{
							bHasDirectionConnection = true;
							if ( MergeNodes[ s ]->SectorId != MergeNodes[ s ]->Connections[ d ].Destination->SectorId )
							{
								EdgeNodes[ d ][ NumEdges[ d ]++ ] = MergeNodes[ s ];
							}
						}
						if ( !bHasDirectionConnection )
							EdgeNodes[ d ][ NumEdges[ d ]++ ] = MergeNodes[ s ];
					}
				}

				static float RT = 10.f;
				if ( RT > 0.f )
				{
					for ( int d = DIR_NORTH; d < DIR_NUM; ++d )
					{
						Vector3f vP1, vP2;
						float fFurthestPoints = 0.f;

						for ( int i1 = 0; i1 < NumEdges[ d ]; ++i1 )
						{
							for ( int i2 = 0; i2 < NumEdges[ d ]; ++i2 )
							{
								Vector3f p1 = _GetNodePosition( *EdgeNodes[ d ][ i1 ] );
								Vector3f p2 = _GetNodePosition( *EdgeNodes[ d ][ i2 ] );
								float fDist = Length( p1, p2 );

								if ( fFurthestPoints == 0.f )
								{
									vP1 = p1;
									vP2 = p2;
								}

								if ( fDist > fFurthestPoints )
								{
									fFurthestPoints = fDist;
									vP1 = p1;
									vP2 = p2;
								}
							}
						}
						if ( fFurthestPoints > 0.f )
						{
							RenderBuffer::AddLine(
								vP1 + Vector3f( 0, 0, 64 ),
								vP2 + Vector3f( 0, 0, 64 ),
								COLOR::BLUE, RT );
						}
					}
				}
			}
		}
	}

	void FloodFiller::Reset()
	{
		for ( int i = 0; i < NumSectors; ++i )
		{
			Nodes[ i ].Init();
		}
		FreeNode = 0;
	}

	void FloodFiller::NextFillState()
	{
		if ( State < FillDone )
			State = (FillState)( State + 1 );
	}

	void FloodFiller::StartFloodFill( const Vector3f &_Start, float _Radius )
	{
		Start = _Start;
		Radius = _Radius;

		FloodBlock = AABB( Vector3f::ZERO );
		FloodBlock.ExpandAxis( 0, Radius );
		FloodBlock.ExpandAxis( 1, Radius );
		FloodBlock.mMaxs[ 2 ] = CHARACTER_HEIGHT - CHARACTER_STEPHEIGHT;

		State = FillInit;
	}

	void FloodFiller::GetDebugString( std::stringstream &out )
	{
	}

	void FloodFiller::RenderDebug()
	{
		Vector3f vAim;
		Utils::GetLocalAimPoint( vAim );

		float fNearestSector = 99999999.f;
		const Node *pNearestSector = 0;

		for ( int i = 0; i < FreeNode; ++i )
		{
			//if(FloodNode[i].OffsetX && FloodNode[i].OffsetY)
			{
				const Node &node = Nodes[ i ];

				obColor nodeCol = COLOR::GREEN;
				if ( node.NearEdge )
					nodeCol = COLOR::MAGENTA;
				if ( node.NearObject )
					nodeCol = COLOR::RED;

				Vector3f vNodePos = _GetNodePosition( node );

				/*static bool RenderOpenness = false;
				if (RenderOpenness)
				{
				RenderBuffer::AddString3d(
				vNodePos,
				COLOR::WHITE,
				RENDER_TIME,
				"%d",
				node.OpenNess);
				}*/

				float fSectorDist = Length( vNodePos, vAim );
				if ( fSectorDist < fNearestSector )
				{
					fNearestSector = fSectorDist;
					pNearestSector = &node;
				}

				for ( int d = 0; d < DIR_NUM; ++d )
				{
					if ( node.Connections[ d ].Destination )
					{
						Vector3f vNeighbor = _GetNodePosition( *node.Connections[ d ].Destination );
						RenderBuffer::AddLine( vNodePos + Vector3f( 0.f, 0.f, 8.f ), vNeighbor, nodeCol );

						const float fDist = Length( vNodePos, vNeighbor );
						if ( fDist > 128.f )
						{
							Utils::OutputDebug( kNormal, "bad connection: %f", fDist );
						}
					}

					static bool bDrawCover = false;
					if ( bDrawCover )
					{
						if ( node.Connections[ d ].Cover )
						{
							int16_t Offset[ DIR_NUM ][ 2 ] =
							{
								{ 0, 1 },
								{ 1, 0 },
								{ 0, -1 },
								{ -1, 0 },
							};

							Vector3f vNodePos2 = _GetNodePosition( node );
							for ( int j = 0; j < DIR_NUM; ++j )
							{
								if ( node.Connections[ d ].Cover & ( 1 << j ) )
								{
									Vector3f vNeighborPos = vNodePos2;
									vNeighborPos.X() += ( Radius*2.f ) * Offset[ j ][ 0 ];
									vNeighborPos.Y() += ( Radius*2.f ) * Offset[ j ][ 1 ];
									RenderBuffer::AddLine(
										vNodePos2 + Vector3f( 0.f, 0.f, 32.f ),
										vNeighborPos + Vector3f( 0.f, 0.f, 32.f ),
										COLOR::BLUE );
								}
							}
						}
					}
				}
			}
		}

		if ( pNearestSector )
		{
			int iNumConnections = 0;
			for ( int d = 0; d < DIR_NUM; ++d )
				if ( pNearestSector->Connections[ d ].Destination )
					++iNumConnections;
			Vector3f vNodePos = _GetNodePosition( *pNearestSector );
			RenderBuffer::AddString3d(
				vNodePos + Vector3f( 0, 0, 32 ),
				COLOR::WHITE,
				va( "%d, sid %d", pNearestSector - Nodes, pNearestSector->Sectorized ? pNearestSector->SectorId : -1 ).c_str() );
			RenderBuffer::AddLine( vNodePos, vNodePos + Vector3f( 0, 0, 32.f ), COLOR::CYAN );
		}
	}

	float FloodFiller::GetPriority()
	{
		//return State != FillDone ? 1.f : 0.f;
		return 1.f;
	}
	void FloodFiller::Enter()
	{
		DebugDraw( true );
	}
	void FloodFiller::Exit()
	{
		//DebugDraw(false);
	}
	State::StateStatus FloodFiller::Update( float fDt )
	{
		//Timer tmr;
		if ( Length( Start, GetClient()->GetPosition() ) > 256.f )
			StartFloodFill( GetClient()->GetPosition() );

		//////////////////////////////////////////////////////////////////////////
		if ( State == FillInit )
		{
			Reset();

			// Add a starting flood node
			Nodes[ FreeNode++ ].Init( 0, 0, Start.Z(), true );

			_DropToGround( &Nodes[ 0 ] );

			NextFillState();
		}
		//////////////////////////////////////////////////////////////////////////
		while ( State == FillSearching )
		{
			Node *currentNode = _NextOpenNode();
			if ( !currentNode )
			{
				NextFillState();
				return State_Busy;
			}

			currentNode->Open = false;

			// Expand in each direction
			for ( int d = DIR_NORTH; d < DIR_NUM; ++d )
			{
				int16_t Offset[ DIR_NUM ][ 2 ] =
				{
					{ 0, 1 },
					{ 1, 0 },
					{ 0, -1 },
					{ -1, 0 },
				};

				int16_t iOffX = currentNode->MinOffset.X + Offset[ d ][ 0 ];
				int16_t iOffY = currentNode->MinOffset.Y + Offset[ d ][ 1 ];
				float fHeight = currentNode->Height;

				Node *pNeighbor = _NodeExists( iOffX, iOffY, fHeight );
				if ( pNeighbor )
				{
					_MakeConnection( currentNode, pNeighbor, (Direction)d );
					continue;
				}

				if ( FreeNode == NumSectors )
				{
					_MergeSectors();

					// fail if that didn't free up anything.
					if ( FreeNode == NumSectors )
					{
						NextFillState();
						return State_Busy;
					}
				}

				// next node
				pNeighbor = &Nodes[ FreeNode ];
				pNeighbor->Init( iOffX, iOffY, fHeight, false );

				static float FLOOD_RADIUS = 512.f;
				Vector3f vNodePos = _GetNodePosition( *pNeighbor );
				if ( Length( vNodePos, Start ) > FLOOD_RADIUS )
				{
					currentNode->NearEdge = true;
					continue;
				}

				if ( !_DropToGround( pNeighbor ) )
				{
					currentNode->NearObject |= true;
					continue;
				}

				currentNode->NearEdge |= ( currentNode->Height - pNeighbor->Height ) > CHARACTER_JUMPHEIGHT;

				// make the connection.
				_MakeConnection( currentNode, pNeighbor, (Direction)d );

				pNeighbor->Open = true;

				++FreeNode;
			}

			if ( currentNode->NearObject || currentNode->NearEdge )
				currentNode->OpenNess = 1;
		}
		//////////////////////////////////////////////////////////////////////////
		if ( State == FillOpenNess )
		{
			NextFillState();
			//_FillOpenNess();
		}
		//////////////////////////////////////////////////////////////////////////
		if ( State == FillSectorize )
		{
			_MergeSectors();
			NextFillState();
		}
		//////////////////////////////////////////////////////////////////////////
		if ( State == FillDone )
		{
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////
};