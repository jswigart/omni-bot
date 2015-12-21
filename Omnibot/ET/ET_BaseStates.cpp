////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ET_BaseStates.h"
#include "ET_FilterClosest.h"
#include "ET_Messages.h"
#include "BotPathing.h"
#include "System.h"
#include "RenderBuffer.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	PlantMine::PlantMine()
		: StateChild( "PlantMine" )
		, FollowPathUser( "PlantMine" )
	{
		LimitToWeapon().SetFlag( ET_WP_LANDMINE );
		SetAlwaysRecieveEvents( true );
	}

	void PlantMine::GetDebugString( std::stringstream &out )
	{
		out << ( mMapGoal ? mMapGoal->GetName() : "" );
	}

	MapGoal *PlantMine::GetMapGoalPtr()
	{
		return mMapGoal.get();
	}

	void PlantMine::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN );
		}
	}

	// FollowPathUser functions.
	bool PlantMine::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 32.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool PlantMine::GetAimPosition( Vector3f &_aimpos )
	{
		static float MINE_PITCH = -75.f;
		if ( mLandMineEntity.IsValid() )
			_aimpos = GetClient()->GetEyePosition() + Utils::ChangePitch( mMapGoal->GetFacing(), MINE_PITCH ) * 32.f;
		else
			_aimpos = mLandMinePosition;
		return true;
	}

	void PlantMine::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws )
		{
			if ( mLandMineEntity.IsValid() && ws->CurrentWeaponIs( ET_WP_PLIERS ) )
				ws->FireWeapon();
			else if ( mLandMineEntity.IsValid() && ws->CurrentWeaponIs( ET_WP_LANDMINE ) )
				ws->FireWeapon();
		}
	}

	float PlantMine::GetPriority()
	{
		int currentMines, maxMines;		
		gEnemyTerritoryFuncs->TeamMineCount( GetClient()->GetGameEntity(), currentMines, maxMines );
		if ( currentMines >= maxMines ) 
			return 0.f;

		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		if ( gEngineFuncs->IsWeaponCharged( GetClient()->GetGameEntity(), ET_WP_LANDMINE, Primary ) )
		{
			GoalManager::Query qry( 0xf2dffa59 /* PLANTMINE */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			qry.GetBest( mMapGoal );
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void PlantMine::Enter()
	{
		Tracker.InProgress = mMapGoal;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this, Run, true ) );
	}

	void PlantMine::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mLandMineEntity.Reset();
		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus PlantMine::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		// If it's not destroyable, consider it a success.
		if ( gEnemyTerritoryFuncs->IsDestroyable( GetClient()->GetGameEntity(), mMapGoal->GetEntity() ) != CONST_DESTROYABLE )
		{
			return State_Finished;
		}

		if ( mLandMineEntity.IsValid() && !IGame::IsEntityValid( mLandMineEntity ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			GetClient()->ResetStuckTime();

			// abort if they have a target
			if ( GetClient()->GetTargetingSystem()->HasTarget() )
				return State_Finished;

			static float THROW_DISTANCE = 40.f;
			static float ARM_DISTANCE = 32.f;

			// Have we already thrown out a mine?
			if ( mLandMineEntity.IsValid() )
			{
				// Is it armed yet?
				if ( gEnemyTerritoryFuncs->GetExplosiveState( mLandMineEntity ) == XPLO_ARMED )
				{
					BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
					return State_Finished;
				}

				// Disable avoidance for this frame.
				//mClient->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime());

				// Not armed yet, keep trying.
				if ( EngineFuncs::EntityPosition( mLandMineEntity, mLandMinePosition ) &&
					EngineFuncs::EntityVelocity( mLandMineEntity, mLandMineVelocity ) )
				{
					GetClient()->PressButton( BOT_BUTTON_CROUCH );

					FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ET_WP_PLIERS ) );

					// Get into position?
					Vector3f vMineToMe = GetClient()->GetPosition() - mLandMinePosition;
					vMineToMe.Flatten();

					/*RenderBuffer::AddLine(.mLandMinePosition,
				mLandMinePosition + Normalize(vMineToMe) * ARM_DISTANCE,
				COLOR::GREEN,
				5.f);*/

					GetClient()->GetSteeringSystem()->SetTarget(
						mLandMinePosition + Normalize( vMineToMe ) * ARM_DISTANCE );
				}
				return State_Busy;
			}

			// Get into position?
			Vector3f vMineToMe = GetClient()->GetPosition() - mMapGoal->GetPosition();
			vMineToMe.Flatten();

			/*RenderBuffer::AddLine(.mMapGoal->GetPosition(),
		mMapGoal->GetPosition() + Normalize(vMineToMe) * THROW_DISTANCE,
		COLOR::GREEN,
		5.f);*/

			GetClient()->GetSteeringSystem()->SetTarget(
				mMapGoal->GetPosition() + Normalize( vMineToMe ) * THROW_DISTANCE );

			// keep watching the target position.
			// cs: make sure they are within bounds before planting
			// TODO: FIXME: check for unreachable.
			const float fDistanceToTarget = SquaredLength2d( mMapGoal->GetPosition(), GetClient()->GetPosition() );
			if ( fDistanceToTarget <= 1024 ) // 32 units.
			{
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ET_WP_LANDMINE ) );
			}
			else
			{
				GetClient()->GetSteeringSystem()->SetTarget( mMapGoal->GetPosition() );
			}
		}
		return State_Busy;
	}

	void PlantMine::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvWeaponFire )
			{
				if ( msg->mWeaponId == ET_WP_LANDMINE && msg->mProjectile.IsValid() )
				{
					mLandMineEntity = msg->mProjectile;
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	MobileMortar::MobileMortar()
		: StateChild( "MobileMortar" )
		, FollowPathUser( "MobileMortar" )
	{
		LimitToWeapon().SetFlag( ET_WP_MORTAR );
	}

	void MobileMortar::GetDebugString( std::stringstream &out )
	{
		out << ( mMapGoal ? mMapGoal->GetName() : "" );
	}

	MapGoal *MobileMortar::GetMapGoalPtr()
	{
		return mMapGoal.get();
	}

	void MobileMortar::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::CYAN );
		}
	}

	// FollowPathUser functions.
	bool MobileMortar::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 32.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool MobileMortar::GetAimPosition( Vector3f &_aimpos )
	{
		FINDSTATE( wp, WeaponSystem, GetRootState() );
		if ( wp != NULL && wp->CurrentWeaponIs( ET_WP_MORTAR_SET ) )
			_aimpos = GetClient()->GetEyePosition() + mMortarAim[ mCurrentAim ];
		else
			_aimpos = GetClient()->GetEyePosition() + mMapGoal->GetFacing();
		return true;
	}

	void MobileMortar::OnTarget()
	{
		FINDSTATE( wp, WeaponSystem, GetRootState() );
		if ( wp )
		{
			if ( !wp->CurrentWeaponIs( ET_WP_MORTAR_SET ) )
			{
				if ( wp->CurrentWeaponIs( ET_WP_MORTAR ) )
					wp->AddWeaponRequest( Priority::Medium, GetNameHash(), ET_WP_MORTAR_SET );
				else
					wp->AddWeaponRequest( Priority::Medium, GetNameHash(), ET_WP_MORTAR );
			}
			else
			{
				if ( IGame::GetTime() >= mFireDelay )
				{
					wp->FireWeapon();
				}
			}
		}
	}

	float MobileMortar::GetPriority()
	{
		int curAmmo = 0, maxAmmo = 0;
		gEngineFuncs->GetCurrentAmmo( GetClient()->GetGameEntity(), ET_WP_MORTAR, Primary, curAmmo, maxAmmo );
		if ( curAmmo <= 0 )
			return 0.f;

		if ( IsActive() )
			return GetLastPriority();

		/*if(!gEngineFuncs->IsWeaponCharged(GetClient(), ET_WP_MORTAR_SET))
		return 0.f;*/

		mMapGoal.reset();

		GoalManager::Query qry( 0x74708d7a /* MOBILEMORTAR */, GetClient() );
		System::mInstance->mGoalManager->GetGoals( qry );
		if ( !qry.GetBest( mMapGoal ) || !CacheGoalInfo( mMapGoal ) )
			mMapGoal.reset();

		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void MobileMortar::Enter()
	{
		Tracker.InProgress = mMapGoal;
		mFireDelay = 0;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this, Run ) );
	}

	void MobileMortar::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );
		Tracker.Reset();
	}

	State::StateStatus MobileMortar::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( !Tracker.InUse && mMapGoal->GetSlotsOpen( MapGoal::TRACK_INUSE ) < 1 )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			if ( mFireDelay == 0 )
			{
				Tracker.InUse = mMapGoal;
				mFireDelay = IGame::GetTime() + 2000;
			}

			// TODO: FIXME: check for unreachable.
			const float fDistanceToTarget = SquaredLength2d( mMapGoal->GetPosition(), GetClient()->GetPosition() );
			if ( fDistanceToTarget <= 1024 ) // 32 units.
			{
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			}
			else
			{
				GetClient()->GetSteeringSystem()->SetTarget( mMapGoal->GetPosition() );
			}
		}
		return State_Busy;
	}

	void MobileMortar::ProcessEvent( const Message & message, CallbackParameters & cb )
	{
		switch ( message.Id() )
		{
			CASE_MSG( EvWeaponFire )
			{
				if ( msg->mProjectile.IsValid() )
				{
					EntityInfo entInfo;
					if ( IGame::GetEntityInfo( msg->mProjectile, entInfo ) )
					{
						if ( entInfo.mGroup == ENT_GRP_PROJECTILE && entInfo.mClassId == ET_CLASSEX_MORTAR )
						{
							mCurrentAim = ( mCurrentAim + 1 ) % mNumMortarAims;
							mFireDelay = IGame::GetTime() + 2000;
						}
					}
				}
				break;
			}
		}
	}

	bool MobileMortar::CacheGoalInfo( MapGoalPtr mg )
	{
		mCurrentAim = 0;
		mNumMortarAims = 0;

		for ( int i = 0; i < MaxMortarAims; ++i )
		{
			if ( mMapGoal->GetProperty( va( "MortarAim[%d]", i ).c_str(), mMortarAim[ mNumMortarAims ] ) )
			{
				if ( mMortarAim[ mNumMortarAims ].IsZero() )
				{
					++mNumMortarAims;
				}
			}
		}
		std::random_shuffle( mMortarAim, mMortarAim + mNumMortarAims );
		return mNumMortarAims > 0;
	}

	//////////////////////////////////////////////////////////////////////////

	CallArtillery::CallArtillery()
		: StateChild( "CallArtillery" )
		, FollowPathUser( "CallArtillery" )
		, mMinCampTime( 1.f )
		, mMaxCampTime( 2.f )
		, mStance( StanceStand )
		, mExpireTime( 0 )
	{
		LimitToWeapon().SetFlag( ET_WP_BINOCULARS );
	}

	void CallArtillery::GetDebugString( std::stringstream &out )
	{
		out << ( mMapGoal ? mMapGoal->GetName() : "" );
	}

	MapGoal *CallArtillery::GetMapGoalPtr()
	{
		return mMapGoal.get();
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
			const MemoryRecord* mr = GetClient()->GetSensoryMemory()->GetMemoryRecord( mTargetEntity );
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
		if ( ws != NULL && ws->CurrentWeaponIs( ET_WP_BINOCULARS ) )
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
		if ( !gEnemyTerritoryFuncs->IsWeaponCharged( GetClient()->GetGameEntity(), ET_WP_BINOCULARS, Primary ) )
			return 0.f;

		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();
		mMapGoalTarget.reset();

		//////////////////////////////////////////////////////////////////////////
		ET_FilterClosest filter( GetClient(), AiState::SensoryMemory::EntEnemy );
		filter.AddCategory( ENT_CAT_SHOOTABLE );
		filter.AddClass( ET_CLASSEX_VEHICLE );
		filter.AddClass( ET_CLASSEX_VEHICLE_HVY );
		FINDSTATE( sensory, SensoryMemory, GetRootState() );
		if ( sensory )
		{
			sensory->QueryMemory( filter );

			if ( filter.GetBestEntity().IsValid() )
			{
				GoalManager::Query q( 0xa411a092 /* MOVER */ );
				q.Ent( filter.GetBestEntity() );
				System::mInstance->mGoalManager->GetGoals( q );

				if ( !q.mList.empty() )
				{
					mTargetEntity = filter.GetBestEntity();
					return q.mList.front()->GetDefaultPriority();
				}
			}
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
		if ( mMapGoal )
			return 0.f;
		//////////////////////////////////////////////////////////////////////////
		Vector3f vSource = mMapGoal->GetPosition();
		//vSource = vSource + Vector3(0,0,60);
		vSource.Z() = vSource.Z() + 60;
		//////////////////////////////////////////////////////////////////////////
		if ( mMapGoalTarget )
		{
			GoalManager::Query qry( 0xb708821b /* ARTILLERY_S */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			for ( uint32_t i = 0; i < qry.mList.size(); ++i )
			{
				if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
					continue;

				if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
					continue;

				if ( !Client::HasLineOfSightTo( vSource, qry.mList[ i ]->GetPosition() ) )
					continue;

				mMapGoalTarget = qry.mList[ i ];
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if ( mMapGoalTarget )
		{
			GoalManager::Query qry( 0xac0870ca /* ARTILLERY_D */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qry );
			for ( uint32_t i = 0; i < qry.mList.size(); ++i )
			{
				if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
					continue;

				if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
					continue;

				if ( !Client::HasLineOfSightTo( vSource, qry.mList[ i ]->GetPosition() ) )
					continue;

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
			if ( mWatchFilter )
			{
				mWatchFilter.reset( new ET_FilterClosest( GetClient(), AiState::SensoryMemory::EntEnemy ) );
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

		if ( mMapGoal && mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;
		if ( mMapGoalTarget && mMapGoalTarget->IsAvailable( GetClient()->GetTeam() ) )
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
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ET_WP_BINOCULARS ) );

			if ( mStance == StanceProne )
				GetClient()->PressButton( BOT_BUTTON_PRONE );
			else if ( mStance == StanceCrouch )
				GetClient()->PressButton( BOT_BUTTON_CROUCH );
		}
		return State_Busy;
	}

	void CallArtillery::ProcessEvent( const Message & message, CallbackParameters & cb )
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
				if ( msg->mWeaponId == ET_WP_BINOCULARS )
				{
					mFired = true;
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	UseCabinet::UseCabinet()
		: StateChild( "UseCabinet" )
		, FollowPathUser( "UseCabinet" )
		, mRange( 1250.f )
	{
	}

	void UseCabinet::GetDebugString( std::stringstream &out )
	{
		out <<
			( mMapGoal ? mMapGoal->GetName() : "" ) <<
			" (" << mHealthPriority << "," << mAmmoPriority << ")";
	}

	MapGoal *UseCabinet::GetMapGoalPtr()
	{
		return mMapGoal.get();
	}

	void UseCabinet::RenderDebug()
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
	bool UseCabinet::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	/*bool UseHealthCabinet::GetAimPosition(Vector3f &_aimpos)
	{
	if(.mMapGoalTarget)
	{
	_aimpos = mMapGoalTarget->GetPosition();
	}
	else if(.mTargetEntity.IsValid())
	{
	const float LOOKAHEAD_TIME = 5.0f;
	const MemoryRecord mr = GetClient()->GetSensoryMemory()->GetMemoryRecord(.mTargetEntity);
	if(mr)
	{
	const Vector3f vVehicleOffset = Vector3f(0.0f, 0.0f, 32.0f);
	_aimpos = vVehicleOffset + mr->mTargetInfo.mLastPosition +
	mr->mTargetInfo.mLastVelocity * LOOKAHEAD_TIME;
	mFireTime = IGame::GetTime() + 1000;
	}
	}
	return true;
	}

	void UseHealthCabinet::OnTarget()
	{
	FINDSTATE(ws, WeaponSystem, GetRootState());
	if(ws && ws->CurrentWeaponIs(ET_WP_BINOCULARS))
	{
	GetClient()->PressButton(BOT_BUTTON_AIM);
	if(.mFireTime < IGame::GetTime())
	{
	if(GetClient()->HasEntityFlag(ENT_FLAG_ZOOMING))
	{
	ws->FireWeapon();
	}
	}
	}
	}*/

	float UseCabinet::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mHealthPriority = 0.f;
		mAmmoPriority = 0.f;

		const float fHealthPc = GetClient()->GetHealthPercent();
		const float fHealthPcInv = ClampT( 1.f - fHealthPc, 0.f, 1.f );
		const float fHealthPriority = fHealthPcInv;
		const float fAmmoPriority = GetClient()->GetWeaponSystem()->GetMostDesiredAmmo( mAmmoType, mGetAmmoAmount );

		mMapGoal.reset();

		//MapGoalPtr mgHealth;
		//MapGoalPtr mgAmmo;

		// don't bother if desire for health is too low
		if ( fHealthPriority > 0.7f )
		{
			GoalManager::Query qryHealth( 0x63217fa7 /* HEALTHCAB */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qryHealth );
			float fDistToHealthCabinet;

			MapGoalList::iterator mIt = qryHealth.mList.begin();
			for ( ; mIt != qryHealth.mList.end(); )
			{
				if ( BlackboardIsDelayed( ( *mIt )->GetSerialNum() ) ||
					( *mIt )->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
				{
					mIt = qryHealth.mList.erase( mIt );
					continue;
				}

				fDistToHealthCabinet = SquaredLength2d( ( *mIt )->GetPosition(), GetClient()->GetPosition() );
				( *mIt )->GetProperty( "Range", mRange );
				if ( fDistToHealthCabinet > mRange * mRange )
				{
					mIt = qryHealth.mList.erase( mIt );
					continue;
				}

				++mIt;
			}

			if ( !qryHealth.mList.empty() )
			{
				//mgAmmo = mgHealth;
				mQuery = qryHealth;
				mCabinetType = Health;
				mHealthPriority = fHealthPriority;
			}
		}

		// don't bother if we need health more than mmo or if desire for mmo is too low
		if ( fAmmoPriority > fHealthPriority && fAmmoPriority > 0.7f && mAmmoType != -1 )
		{
			GoalManager::Query qryAmmo( 0x52ad0a47 /* AMMOCAB */, GetClient() );
			System::mInstance->mGoalManager->GetGoals( qryAmmo );
			float fDistToAmmoCabinet;

			MapGoalList::iterator mIt = qryAmmo.mList.begin();
			for ( ; mIt != qryAmmo.mList.end(); )
			{
				if ( BlackboardIsDelayed( ( *mIt )->GetSerialNum() ) ||
					( *mIt )->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
				{
					mIt = qryAmmo.mList.erase( mIt );
					continue;
				}

				fDistToAmmoCabinet = SquaredLength2d( ( *mIt )->GetPosition(), GetClient()->GetPosition() );
				( *mIt )->GetProperty( "Range", mRange );
				if ( fDistToAmmoCabinet > mRange * mRange )
				{
					mIt = qryAmmo.mList.erase( mIt );
					continue;
				}

				++mIt;
			}

			if ( !qryAmmo.mList.empty() )
			{
				//.mMapGoal = mgAmmo;
				mQuery = qryAmmo;
				mCabinetType = mmo;
				mAmmoPriority = fAmmoPriority;
			}
		}

		return mHealthPriority > mAmmoPriority ? mHealthPriority : mAmmoPriority;
	}

	void UseCabinet::Enter()
	{
		mUseTime = 0;
		FINDSTATEIF( Navigator, GetRootState(), Goto( this, mQuery.mList, Run, true ) );
		if ( !DidPathFail() )
		{
			mMapGoal = mQuery.mList[ GetDestinationIndex() ];
			Tracker.InProgress = mMapGoal;
		}
	}

	void UseCabinet::Exit()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );

		mMapGoal.reset();

		//FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus UseCabinet::Update( float fDt )
	{
		if ( mMapGoal || DidPathFail() )
		{
			// delay all of m.
			for ( uint32_t i = 0; i < mQuery.mList.size(); ++i )
				BlackboardDelay( 10.f, mQuery.mList[ i ]->GetSerialNum() );
			return State_Finished;
		}

		ParamsCabinetData data;
		if ( gEnemyTerritoryFuncs->GetCabinetData( mMapGoal->GetEntity(), data ) &&
			data.mCurrentAmount == 0 )
		{
			BlackboardDelay( 20.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( mMapGoal && mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		switch ( mCabinetType )
		{
			case Health:
				if ( GetClient()->GetHealthPercent() >= 1.0 )
					return State_Finished;
				break;
			case mmo:
			{
				if ( mGetAmmoAmount > 0 )
				{
					if ( GetClient()->GetWeaponSystem()->HasAmmo( mAmmoType, Primary, mGetAmmoAmount ) )
						return State_Finished;
				}
				else
				{
					int mmoType = 0, GetAmmo = 0;
					if ( GetClient()->GetWeaponSystem()->GetMostDesiredAmmo( mmoType, GetAmmo ) <= 0.f )
						return State_Finished;
				}
				break;
			}
		}

		if ( DidPathSucceed() )
		{
			GetClient()->GetSteeringSystem()->SetTarget( mMapGoal->GetWorldUsePoint() );

			if ( mUseTime == 0 )
			{
				mUseTime = IGame::GetTime();
			}
			else if ( IGame::GetTime() - mUseTime > 15000 )
			{
				//timeout
				BlackboardDelay( 30.f, mMapGoal->GetSerialNum() );
				return State_Finished;
			}
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	//BuildConstruction::BuildConstruction()
	//	: StateChild("BuildConstruction")
	//	, FollowPathUser("BuildConstruction")
	//	, mAdjustedPosition(false)
	//	, mCrouch(true)
	//	, mProne(false)
	//	, mIgnoreTargets(false)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_PLIERS);
	//}

	//void BuildConstruction::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void BuildConstruction::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mConstructionPos,COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool BuildConstruction::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//	{
	//		_skiplastpt = true;
	//		_final = true;
	//	}
	//	return true;
	//}

	//// AimerUser functions.
	//bool BuildConstruction::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mConstructionPos;
	//	return true;
	//}

	//void BuildConstruction::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws && ws->CurrentWeaponIs(ET_WP_PLIERS))
	//		ws->FireWeapon();

	//	//gEngineFuncs->GetCurrentCursorHint
	//}

	//float BuildConstruction::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();
	//
	//mMapGoal.reset();

	//	GoalManager::Query qry(0xc39bf2a3 /* BUILD */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//			continue;

	//		if(qry.mList[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//			continue;

	//		ConstructableState cState = gEngineFuncs->GetConstructableState(GetClient(),qry.mList[i]->GetEntity());
	//		if(cState == CONST_UNBUILT)
	//		{
	//		mMapGoal = qry.mList[i];
	//			break;
	//		}
	//	}
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void BuildConstruction::Enter()
	//{
	//mAdjustedPosition = false;
	//mMapGoal->GetWorldBounds().CenterPoint(.mConstructionPos);
	//	Tracker.InProgress = mMapGoal;

	//mMapGoal->GetProperty("Crouch",.mCrouch);
	//mMapGoal->GetProperty("Prone",.mProne);
	//mMapGoal->GetProperty("IgnoreTargets",.mIgnoreTargets);
	//
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void BuildConstruction::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	Tracker.Reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));
	//}

	//State::StateStatus BuildConstruction::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	//////////////////////////////////////////////////////////////////////////
	//	// Check the construction status
	//	ConstructableState cState = gEngineFuncs->GetConstructableState(GetClient(),.mMapGoal->GetEntity());
	//	switch(cState)
	//	{
	//	case CONST_INVALID: // Invalid constructable, fail
	//		return State_Finished;
	//	case CONST_BUILT:	// It's built, consider it a success.
	//		return State_Finished;
	//	case CONST_UNBUILT:
	//	case CONST_BROKEN:
	//		// This is what we want.
	//		break;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(DidPathSucceed())
	//	{
	//		const float fDistanceToConstruction = SquaredLength2d(.mConstructionPos, GetClient()->GetPosition());
	//		if (fDistanceToConstruction > 4096.0f)
	//		{
	//			// check for badly waypointed maps
	//			if (!mAdjustedPosition)
	//			{
	//				// use our z value because some trigger entities may be below the ground
	//				Vector3f checkPos(.mConstructionPos.X(), mConstructionPos.Y(), GetClient()->GetEyePosition().Z());

	//				obTraceResult tr;
	//				EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(),checkPos,
	//					NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), GetClient()->GetGameID(), True);

	//				if (tr.mFraction != 1.0f && !tr.mHitEntity.IsValid())
	//				{
	//				mMapGoal->SetDeleteMe(true);
	//					return State_Finished;
	//				}

	//				// do a trace to adjust position
	//				EngineFuncs::TraceLine(tr,
	//					GetClient()->GetEyePosition(),
	//				mConstructionPos,
	//					NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, False);

	//				if (tr.mFraction != 1.0f)
	//				{
	//				mConstructionPos = *(Vector3f*)tr.mEndpos;
	//				}

	//			mAdjustedPosition = true;
	//			}
	//		}
	//		else
	//		{
	//			if(.mProne)
	//				GetClient()->PressButton(BOT_BUTTON_PRONE);
	//			else if(.mCrouch)
	//				GetClient()->PressButton(BOT_BUTTON_CROUCH);

	//			Priority::ePriority pri = mIgnoreTargets ? Priority::High : Priority::Medium;
	//			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_PLIERS));
	//		}
	//		GetClient()->GetSteeringSystem()->SetTarget(.mConstructionPos, 64.f);
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//PlantExplosive::PlantExplosive()
	//	: StateChild("PlantExplosive")
	//	, FollowPathUser("PlantExplosive")
	//	, mIgnoreTargets(false)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_DYNAMITE);
	//	LimitToWeapon().SetFlag(ET_WP_SATCHEL);
	//	LimitToWeapon().SetFlag(ET_WP_SATCHEL_DET);
	//	SetAlwaysRecieveEvents(true);
	//}

	//void PlantExplosive::GetDebugString(std::stringstream &out)
	//{
	//	switch(.mGoalState)
	//	{
	//	case LAY_EXPLOSIVE:
	//		out << "Lay Explosive ";
	//		break;
	//	case ARM_EXPLOSIVE:
	//		out << "Arm Explosive ";
	//		break;
	//	case RUNAWAY:
	//		out << "Run! ";
	//		break;
	//	case DETONATE_EXPLOSIVE:
	//		out << "Det Explosive ";
	//		break;
	//	default:
	//		break;
	//	}
	//	out << std::endl << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void PlantExplosive::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE, 5.f);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool PlantExplosive::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool PlantExplosive::GetAimPosition(Vector3f &_aimpos)
	//{
	//	switch(.mGoalState)
	//	{
	//	case LAY_EXPLOSIVE:
	//		_aimpos = mTargetPosition;
	//		break;
	//	case ARM_EXPLOSIVE:
	//		_aimpos = mExplosivePosition;
	//		break;
	//	case RUNAWAY:
	//	case DETONATE_EXPLOSIVE:
	//	default:
	//		return false;
	//	}
	//	return true;
	//}

	//void PlantExplosive::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws)
	//	{
	//		if(.mGoalState == LAY_EXPLOSIVE)
	//		{
	//			if(ws->CurrentWeaponIs(ET_WP_DYNAMITE) || ws->CurrentWeaponIs(ET_WP_SATCHEL))
	//				ws->FireWeapon();
	//		}
	//		else if(.mGoalState == ARM_EXPLOSIVE)
	//		{
	//			if(ws->CurrentWeaponIs(ET_WP_PLIERS))
	//				ws->FireWeapon();
	//		}
	//	}
	//}

	//float PlantExplosive::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	ExplosiveTargetType		targetType = XPLO_TYPE_DYNAMITE;
	//	ET_Weapon				weaponType = ET_WP_DYNAMITE;
	//	switch(GetClient()->GetClass())
	//	{
	//	case ET_CLASS_ENGINEER:
	//		{
	//			weaponType = ET_WP_DYNAMITE;
	//			targetType = XPLO_TYPE_DYNAMITE;
	//			break;
	//		}
	//	case ET_CLASS_COVERTOPS:
	//		{
	//			weaponType = ET_WP_SATCHEL;
	//			targetType = XPLO_TYPE_SATCHEL;
	//			break;
	//		}
	//	default:
	//		return 0.0;
	//	}

	//mMapGoal.reset();

	//	if(gEngineFuncs->IsWeaponCharged(GetClient(), weaponType, Primary))
	//	{
	//		{
	//			GoalManager::Query qry(0xbbcae592 /* PLANT */, GetClient());
	//			System::mInstance->mGoalManager->GetGoals(qry);
	//			for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//			{
	//				if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//					continue;

	//				/*if(!(qry.mList[i]->GetGoalState() & targetType))
	//					continue;*/

	//				if(qry.mList[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//					continue;

	//				if(!_IsGoalAchievable(qry.mList[i], weaponType))
	//					continue;

	//				ConstructableState cState = gEngineFuncs->IsDestroyable(GetClient(), qry.mList[i]->GetEntity());
	//				if(cState == CONST_DESTROYABLE)
	//				{
	//				mMapGoal = qry.mList[i];
	//					break;
	//				}
	//			}
	//		}

	//		if(!mMapGoal)
	//		{
	//			GoalManager::Query qry(0xa411a092 /* MOVER */, GetClient());
	//			System::mInstance->mGoalManager->GetGoals(qry);
	//			for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//			{
	//				if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//					continue;

	//				if(qry.mList[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//					continue;
	//
	//				if(!(qry.mList[i]->GetGoalState() & targetType))
	//					continue;

	//			mMapGoal = qry.mList[i];
	//				break;
	//			}
	//		}
	//	}
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void PlantExplosive::Enter()
	//{
	//mMapGoal->GetProperty("IgnoreTargets",.mIgnoreTargets);
	//
	//	// set position to base of construction
	//	AABB aabb = mMapGoal->GetWorldBounds();
	//	aabb.CenterPoint(.mExplosivePosition);
	//mExplosivePosition.Z() = aabb.mMins[2];
	//mTargetPosition = mExplosivePosition;
	//mAdjustedPosition = false;
	//mGoalState = LAY_EXPLOSIVE;
	//	Tracker.InProgress = mMapGoal;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void PlantExplosive::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mExplosiveEntity.Reset();

	//mMapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus PlantExplosive::Update(float fDt)
	//{
	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	// If it's not destroyable, consider it a success.
	//	if(!gEngineFuncs->IsDestroyable(GetClient(), mMapGoal->GetEntity()))
	//		return State_Finished;

	//	if(.mExplosiveEntity.IsValid() && !IGame::IsEntityValid(.mExplosiveEntity))
	//		return State_Finished;

	//	if(DidPathSucceed() || mGoalState==DETONATE_EXPLOSIVE)
	//	{
	//		switch(GetClient()->GetClass())
	//		{
	//		case ET_CLASS_ENGINEER:
	//			return _UpdateDynamite();
	//		case ET_CLASS_COVERTOPS:
	//			return _UpdateSatchel();
	//		default:
	//		}
	//	}

	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}
	//	return State_Busy;
	//}

	//State::StateStatus PlantExplosive::_UpdateDynamite()
	//{
	//	Priority::ePriority pri = mIgnoreTargets ? Priority::High : Priority::Medium;

	//	switch(.mGoalState)
	//	{
	//	case LAY_EXPLOSIVE:
	//		{
	//			if(!gEngineFuncs->IsWeaponCharged(GetClient(), ET_WP_DYNAMITE, Primary))
	//				return State_Finished;

	//			/*if(mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
	//			{
	//				RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE, 1.f);

	//				Vector3f vCenter;
	//			mMapGoal->GetWorldBounds().CenterPoint(vCenter);
	//				RenderBuffer::AddLine(mClient->GetPosition(), vCenter, COLOR::GREEN, 1.0f);
	//				RenderBuffer::AddLine(mClient->GetPosition(), mMapGoal->GetPosition(), COLOR::RED, 1.0f);
	//			}*/

	//			const float fDistanceToTarget = SquaredLength2d(.mTargetPosition, GetClient()->GetPosition());
	//			if (fDistanceToTarget > Mathf::Sqr(100.0f))
	//			{
	//				// check for badly waypointed maps
	//				if (!mAdjustedPosition)
	//				{
	//				mAdjustedPosition = true;

	//					// use our z value because some trigger entities may be below the ground
	//					Vector3f vCheckPos(.mTargetPosition.X(), mTargetPosition.Y(), GetClient()->GetEyePosition().Z());
	//					/*if(mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
	//					{
	//						RenderBuffer::AddLine(GetClient()->GetEyePosition(), vCheckPos, COLOR::GREEN, 2.0f);
	//					}*/

	//					obTraceResult tr;
	//					EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), vCheckPos,
	//						NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), GetClient()->GetGameID(), True);
	//					if (tr.mFraction != 1.0f && !tr.mHitEntity.IsValid())
	//					{
	//						//.mTargetEntity->SetDeleteMe(true);
	//						return State_Finished;
	//					}

	//					// do a trace to adjust position
	//					EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), mTargetPosition,
	//						NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, False);
	//					if (tr.mFraction != 1.0f)
	//					{
	//					mTargetPosition = Vector3f(tr.mEndpos);
	//					}
	//				}

	//				// Move toward it.
	//				GetClient()->GetSteeringSystem()->SetTarget(.mTargetPosition);
	//			}
	//			else
	//			{
	//				// We're within range, so let's start laying.
	//				GetClient()->ResetStuckTime();
	//				FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_DYNAMITE));
	//			}
	//			break;
	//		}
	//	case ARM_EXPLOSIVE:
	//		{
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);

	//			if(gEngineFuncs->GetExplosiveState(GetClient(), mExplosiveEntity) == XPLO_ARMED)
	//			{
	//				BlackboardDelay(30.f, mMapGoal->GetSerialNum());
	//				return State_Finished;
	//			}

	//			// Disable avoidance for this frame.
	//			GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime());

	//			// update dynamite position
	//			EngineFuncs::EntityPosition(.mExplosiveEntity, mExplosivePosition);

	//			// move a little bit close if dyno too far away
	//			if ((.mExplosivePosition - GetClient()->GetPosition()).SquaredLength() > 2500.0f)
	//			{
	//				GetClient()->GetSteeringSystem()->SetTarget(.mExplosivePosition);
	//			}
	//			else
	//			{
	//				GetClient()->ResetStuckTime();
	//				FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_PLIERS));
	//			}
	//			break;
	//		}
	//	default:
	//		// keep processing
	//		break;
	//	}
	//	return State_Busy;
	//}

	//State::StateStatus PlantExplosive::_UpdateSatchel()
	//{
	//	Priority::ePriority pri = mIgnoreTargets ? Priority::High : Priority::Medium;

	//	switch(.mGoalState)
	//	{
	//	case LAY_EXPLOSIVE:
	//		{
	//			if(!gEngineFuncs->IsWeaponCharged(GetClient(), ET_WP_SATCHEL, Primary))
	//				return State_Finished;

	//			const float fDistanceToTarget = SquaredLength2d(.mTargetPosition, GetClient()->GetPosition());
	//			if (fDistanceToTarget > 10000.0f)
	//			{
	//				// Move toward it.
	//				//GetClient()->GetSteeringSystem()->SetTarget(.mTargetPosition);

	//				// check for badly waypointed maps
	//				if (!mAdjustedPosition)
	//				{
	//				mAdjustedPosition = true;

	//					// use our z value because some trigger entities may be below the ground
	//					Vector3f vCheckPos(.mTargetPosition.X(), mTargetPosition.Y(), GetClient()->GetEyePosition().Z());
	//					/*if(mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
	//					{
	//						RenderBuffer::AddLine(mClient->GetEyePosition(), vCheckPos, COLOR::GREEN, 2.0f);
	//					}*/

	//					obTraceResult tr;
	//					EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), vCheckPos,
	//						NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), GetClient()->GetGameID(), True);
	//					if (tr.mFraction != 1.0f && !tr.mHitEntity.IsValid())
	//					{
	//						AABB aabb, mapaabb;
	//						EngineFuncs::EntityWorldAABB(.mMapGoal->GetEntity(), aabb);
	//						//gEngineFuncs->GetMapExtents(mapaabb);

	//						//.mTargetEntity->SetDeleteMe(true);
	//						return State_Finished;
	//					}

	//					// do a trace to adjust position
	//					EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), mTargetPosition,
	//						NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, False);
	//					if (tr.mFraction != 1.0f)
	//					{
	//					mTargetPosition = Vector3f(tr.mEndpos);
	//					}
	//				}
	//			}
	//			else
	//			{
	//				// We're within range, so let's start laying.
	//				GetClient()->ResetStuckTime();
	//				FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_SATCHEL));
	//			}
	//			break;
	//		}
	//	case ARM_EXPLOSIVE:
	//	case RUNAWAY:
	//		{
	//			// Generate a random goal.
	//			FINDSTATEIF(FollowPath,GetRootState(),GotoRandomPt(this));
	//		mGoalState = DETONATE_EXPLOSIVE;
	//			break;
	//		}
	//	case DETONATE_EXPLOSIVE:
	//		{
	//			if(DidPathFail() || DidPathSucceed())
	//			{
	//				FINDSTATEIF(FollowPath,GetRootState(),GotoRandomPt(this));
	//				break;
	//			}

	//			// Are we far enough away to blow it up?
	//			const float SATCHEL_DET_DISTANCE = 350.0f;
	//			const bool BLOW_TARGET_OR_NOT = true;

	//			Vector3f vSatchelPos;
	//			if(EngineFuncs::EntityPosition(.mExplosiveEntity, vSatchelPos))
	//			{
	//				if(Length(GetClient()->GetPosition(), vSatchelPos) >= SATCHEL_DET_DISTANCE)
	//				{
	//					// Do we need this? perhaps make this flag scriptable?
	//					if(BLOW_TARGET_OR_NOT || !GetClient()->GetTargetingSystem()->HasTarget())
	//					{
	//						FINDSTATE(ws,WeaponSystem,GetRootState());
	//						if(ws)
	//						{
	//							ws->AddWeaponRequest(Priority::Override, GetNameHash(), ET_WP_SATCHEL_DET);
	//							if(ws->CurrentWeaponIs(ET_WP_SATCHEL_DET))
	//								ws->FireWeapon();
	//						}

	//						ExplosiveState eState = gEngineFuncs->GetExplosiveState(GetClient(), mExplosiveEntity);
	//						if(eState == XPLO_INVALID)
	//							return State_Finished;
	//					}
	//				}
	//			}
	//			break;
	//		}
	//	}
	//	return State_Busy;
	//}

	//void PlantExplosive::ProcessEvent(const Message & message, CallbackParameters & cb)
	//{
	//	if(IsActive())
	//	{
	//		Priority::ePriority pri = mIgnoreTargets ? Priority::High : Priority::Medium;
	//		switch(_message.GetMessageId())
	//		{
	//			HANDLER(ACTION_WEAPON_FIRE)
	//			{
	//				const Event_WeaponFire m = message.Get<Event_WeaponFire>();
	//				if(m->mWeaponId == ET_WP_DYNAMITE && m->mProjectile.IsValid())
	//				{
	//					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm/det)
	//				mExplosiveEntity = m->mProjectile;
	//				mGoalState = ARM_EXPLOSIVE;
	//					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm/det)

	//					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_PLIERS));
	//				}
	//				else if(m->mWeaponId == ET_WP_SATCHEL && m->mProjectile.IsValid())
	//				{
	//					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm/det)
	//				mExplosiveEntity = m->mProjectile;
	//				mGoalState = RUNAWAY;
	//					SetLastPriority(2.f); // up the priority so we aren't interrupted(to arm/det)

	//					FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//					FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));
	//				}
	//				break;
	//			}
	//		}
	//	}
	//}

	//bool PlantExplosive::_IsGoalAchievable(MapGoalPtr _g, int _weaponId)
	//{
	//	static bool bDynaUnderWater = false;
	//	static bool bSatchelUnderWater = false;
	//	static bool bDynaCached = false;
	//	static bool bSatchelCached = false;
	//	if(!bDynaCached)
	//	{
	//		WeaponPtr w = g_WeaponDatabase.GetWeapon(ET_WP_DYNAMITE);
	//		if(w)
	//		{
	//			bDynaUnderWater = w->GetFireMode(Primary).CheckFlag(Weapon::Waterproof);
	//			bDynaCached = true;
	//		}
	//	}
	//	if(!bSatchelCached)
	//	{
	//		WeaponPtr w = g_WeaponDatabase.GetWeapon(ET_WP_SATCHEL);
	//		if(w)
	//		{
	//			bSatchelUnderWater = w->GetFireMode(Primary).CheckFlag(Weapon::Waterproof);
	//			bSatchelCached = true;
	//		}
	//	}
	//	return _weaponId==ET_WP_DYNAMITE ? bDynaUnderWater : bSatchelUnderWater;
	//}

	//////////////////////////////////////////////////////////////////////////

	//Flamethrower::Flamethrower()
	//	: StateChild("Flamethrower")
	//	, FollowPathUser("Flamethrower")
	//	, mMinCampTime(2000)
	//	, mMaxCampTime(8000)
	//	, mExpireTime(0)
	//	, mStance(StanceStand)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_FLAMETHROWER);
	//}

	//void Flamethrower::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void Flamethrower::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//	mTargetZone.RenderDebug();
	//	}
	//}

	//bool Flamethrower::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	_final = mMapGoal->RouteTo(GetClient(), _desination);
	//	return true;
	//}

	//bool Flamethrower::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mAimPosition;
	//	return true;
	//}

	//void Flamethrower::OnTarget()
	//{
	//	FINDSTATE(wp,WeaponSystem,GetRootState());

	//	if (wp)
	//	{
	//		if(!wp->CurrentWeaponIs(ET_WP_FLAMETHROWER))
	//		{
	//			wp->AddWeaponRequest(Priority::Medium, GetNameHash(), ET_WP_FLAMETHROWER);
	//		}
	//	}
	//}

	//float Flamethrower::GetPriority()
	//{
	//	int curAmmo = 0, maxAmmo = 0;
	//	gEngineFuncs->GetCurrentAmmo(GetClient()->GetGameEntity(),ET_WP_FLAMETHROWER,Primary,curAmmo,maxAmmo);
	//	if(curAmmo <= 0)
	//		return 0.f;

	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0x86584d00 /* FLAME */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	qry.GetBest(.mMapGoal);

	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void Flamethrower::Enter()
	//{
	//mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//	Tracker.InProgress = mMapGoal;

	//mTargetZone.Restart(256.f);
	//mExpireTime = 0;

	//mMapGoal->GetProperty("Stance",.mStance);
	//mMapGoal->GetProperty("MinCampTime",.mMinCampTime);
	//mMapGoal->GetProperty("MaxCampTime",.mMaxCampTime);

	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void Flamethrower::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
	//	Tracker.Reset();
	//}

	//State::StateStatus Flamethrower::Update(float fDt)
	//{
	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	//////////////////////////////////////////////////////////////////////////
	//	if(DidPathFail())
	//	{
	//		// Delay it from being used for a while.
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(!Tracker.InUse && mMapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		// Only hang around here for a certain mount of time
	//		if(.mExpireTime==0)
	//		{
	//		mExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(.mMinCampTime,.mMaxCampTime);
	//			Tracker.InUse = mMapGoal;
	//		}
	//		else if(IGame::GetTime() > mExpireTime)
	//		{
	//			// Delay it from being used for a while.
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));

	//	mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//	mTargetZone.Update(GetClient());

	//		if(.mTargetZone.HasAim())
	//		mAimPosition = mTargetZone.GetAimPosition();

	//		GetClient()->GetSteeringSystem()->SetTarget(.mMapGoal->GetPosition(), mMapGoal->GetRadius());

	//		NavFlags nodeFlags = mMapGoal->GetFlags();
	//		if (nodeFlags & F_NAV_PRONE || mStance==StanceProne)
	//			GetClient()->PressButton(BOT_BUTTON_PRONE);
	//		else if (nodeFlags & F_NAV_CROUCH || mStance==StanceCrouch)
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);
	//	}
	//	return State_Busy;
	//}

	////////////////////////////////////////////////////////////////////////////

	//Panzer::Panzer()
	//	: StateChild("Panzer")
	//	, FollowPathUser("Panzer")
	//	, mMinCampTime(2000)
	//	, mMaxCampTime(8000)
	//	, mExpireTime(0)
	//	, mStance(StanceStand)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_PANZERFAUST);
	//}

	//void Panzer::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void Panzer::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//	mTargetZone.RenderDebug();
	//	}
	//}

	//bool Panzer::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	_final = mMapGoal->RouteTo(GetClient(), _desination);
	//	return true;
	//}

	//bool Panzer::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mAimPosition;
	//	return true;
	//}

	//void Panzer::OnTarget()
	//{
	//	FINDSTATE(wp,WeaponSystem,GetRootState());

	//	if (wp)
	//	{
	//		if(!wp->CurrentWeaponIs(ET_WP_PANZERFAUST))
	//		{
	//			wp->AddWeaponRequest(Priority::Medium, GetNameHash(), ET_WP_PANZERFAUST);
	//		}
	//	}
	//}

	//float Panzer::GetPriority()
	//{
	//	int curAmmo = 0, maxAmmo = 0;
	//	gEngineFuncs->GetCurrentAmmo(GetClient()->GetGameEntity(),ET_WP_PANZERFAUST,Primary,curAmmo,maxAmmo);
	//	if(curAmmo <= 0)
	//		return 0.f;

	//	if(!gEngineFuncs->IsWeaponCharged(GetClient(), ET_WP_PANZERFAUST, Primary))
	//		return 0.f;

	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0x731f6315 /* PANZER */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	qry.GetBest(.mMapGoal);

	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void Panzer::Enter()
	//{
	//mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//	Tracker.InProgress = mMapGoal;

	//mTargetZone.Restart(256.f);
	//mExpireTime = 0;
	//
	//mMapGoal->GetProperty("Stance",.mStance);
	//mMapGoal->GetProperty("MinCampTime",.mMinCampTime);
	//mMapGoal->GetProperty("MaxCampTime",.mMaxCampTime);

	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void Panzer::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
	//	Tracker.Reset();
	//}

	//State::StateStatus Panzer::Update(float fDt)
	//{
	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	//////////////////////////////////////////////////////////////////////////
	//	if(DidPathFail())
	//	{
	//		// Delay it from being used for a while.
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(!Tracker.InUse && mMapGoal->GetSlotsOpen(MapGoal::TRACK_INUSE) < 1)
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		// Only hang around here for a certain mount of time
	//		if(.mExpireTime==0)
	//		{
	//		mExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(.mMinCampTime,.mMaxCampTime);
	//			Tracker.InUse = mMapGoal;
	//		}
	//		else if(IGame::GetTime() > mExpireTime)
	//		{
	//			// Delay it from being used for a while.
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::Low,this,GetNameHash()));

	//	mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;

	//	mTargetZone.Update(GetClient());

	//		if(.mTargetZone.HasAim())
	//		mAimPosition = mTargetZone.GetAimPosition();

	//		GetClient()->GetSteeringSystem()->SetTarget(.mMapGoal->GetPosition(), mMapGoal->GetRadius());

	//		NavFlags nodeFlags = mMapGoal->GetFlags();
	//		if (nodeFlags & F_NAV_PRONE || mStance==StanceProne)
	//			GetClient()->PressButton(BOT_BUTTON_PRONE);
	//		else if (nodeFlags & F_NAV_CROUCH || mStance==StanceCrouch)
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);
	//	}
	//	return State_Busy;
	//}

	///////////////////////////////////////////////////////////////////////////

	//MountMg42::MountMg42()
	//	: StateChild("MountMg42")
	//	, FollowPathUser("MountMg42")
	//	, mMinCampTime(6.f)
	//	, mMaxCampTime(10.f)
	//	, mExpireTime(0)
	//	, mStartTime(0)
	//	, mScanDirection(SCAN_DEFAULT)
	//	, mNextScanTime(0)
	//	, mIgnoreTargets(false)
	//{
	//}

	//void MountMg42::GetDebugString(std::stringstream &out)
	//{
	//	if(IsActive())
	//	{
	//		if(!GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED))
	//			out << "Mounting ";
	//		else
	//		{
	//			switch(.mScanDirection)
	//			{
	//			case SCAN_DEFAULT:
	//				out << "Scan Facing ";
	//				break;
	//			case SCAN_MIDDLE:
	//				out << "Scan Middle ";
	//				break;
	//			case SCAN_LEFT:
	//				out << "Scan Left ";
	//				break;
	//			case SCAN_RIGHT:
	//				out << "Scan Right ";
	//				break;
	//			}
	//		}

	//		if(.mMapGoal)
	//			out << mMapGoal->GetName();
	//	}
	//}

	//void MountMg42::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	mTargetZone.RenderDebug();
	//	}
	//}

	//// FollowPathUser functions.
	//bool MountMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool MountMg42::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mAimPoint;
	//	return true;
	//}

	//void MountMg42::OnTarget()
	//{
	//	if(!GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED) && (IGame::GetFrameNumber()&1))
	//		GetClient()->PressButton(BOT_BUTTON_USE);
	//}

	//float MountMg42::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();
	//
	//	BitFlag64 entFlags;

	//	GoalManager::Query qry(0xe1a2b09c /* MOUNTMG42 */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//			continue;

	//		if(qry.mList[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//			continue;

	//		GameEntity gunOwner = gEngineFuncs->GetMountedPlayerOnMG42(GetClient(), qry.mList[i]->GetEntity());
	//		int gunHealth = gEngineFuncs->GetGunHealth(GetClient(), qry.mList[i]->GetEntity());
	//		bool bBroken = gEngineFuncs->IsMountableGunRepairable(GetClient(), qry.mList[i]->GetEntity());

	//		if(bBroken)
	//			continue;

	//		if(!gEngineFuncs->GetEntityFlags(qry.mList[i]->GetEntity(), entFlags) ||
	//			!entFlags.CheckFlag(ET_ENT_FLAG_ISMOUNTABLE))
	//			continue;

	//		// Make sure nobody has it mounted.
	//		if((!gunOwner.IsValid() || !GetClient()->IsAllied(gunOwner)) && (gunHealth > 0))
	//		{
	//		mMapGoal = qry.mList[i];
	//			break;
	//		}
	//	}
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void MountMg42::Enter()
	//{
	//mMapGoal->GetProperty("MinCampTime",.mMinCampTime);
	//mMapGoal->GetProperty("MaxCampTime",.mMaxCampTime);
	//mMapGoal->GetProperty("IgnoreTargets",.mIgnoreTargets);

	//mExpireTime = 0;

	//mScanDirection = SCAN_MIDDLE;
	//mNextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom(2000.0f, 7000.0f);
	//mAimPoint = mMapGoal->GetPosition();
	//mMG42Position = mAimPoint;
	//mScanLeft = Vector3f::ZERO;
	//mScanRight = Vector3f::ZERO;
	//mGotGunProperties = false;
	//	Tracker.InProgress = mMapGoal;
	//mTargetZone.Restart(256.f);
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void MountMg42::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));

	//	if(GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED))
	//		GetClient()->PressButton(BOT_BUTTON_USE);

	//	Tracker.Reset();
	//}

	//State::StateStatus MountMg42::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	//////////////////////////////////////////////////////////////////////////
	//	// Only fail if a friendly player is on this gun or gun has been destroyed in the meantime
	//	//int gunHealth = gEngineFuncs->GetGunHealth(.mClient, mMG42Goal->GetEntity());
	//	GameEntity mounter = gEngineFuncs->GetMountedPlayerOnMG42(GetClient(), mMapGoal->GetEntity());
	//	if(gEngineFuncs->IsMountableGunRepairable(GetClient(), mMapGoal->GetEntity()) ||
	//	mounter.IsValid() && mounter != GetClient()->GetGameEntity()) && GetClient()->IsAllied(mounter)))
	//	{
	//		return State_Finished;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(DidPathSucceed())
	//	{
	//		GetClient()->GetSteeringSystem()->SetTarget(.mMapGoal->GetPosition());

	//		const bool bMounted = GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED);
	//		const int currentTime = IGame::GetTime();

	//		Priority::ePriority pri = mIgnoreTargets && !bMounted ? Priority::High : Priority::Low;
	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));

	//		// Only hang around here for a certain mount of time. 3 seconds max if they don't get mounted.
	//		if(.mExpireTime==0)
	//		{
	//		mExpireTime = currentTime+Mathf::IntervalRandomInt(.mMinCampTime.GetMs(),.mMaxCampTime.GetMs());
	//		mStartTime = currentTime;
	//			Tracker.InUse = mMapGoal;
	//		}
	//		else if(currentTime > mExpireTime || (!bMounted && currentTime - mStartTime > 3000))
	//		{
	//			// Delay it from being used for a while.
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		if(bMounted)
	//		{
	//		mTargetZone.Update(GetClient());

	//			if(!mGotGunProperties)
	//			{
	//			mGotGunProperties = true;
	//				_GetMG42Properties();
	//			mAimPoint = mMapGoal->GetPosition() + mGunCenterArc * 512.f;
	//			}

	//			if(.mNextScanTime < IGame::GetTime())
	//			{
	//			mNextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom(2000.0f, 7000.0f);
	//			mScanDirection = (int)Mathf::IntervalRandom(0.0f, (float)NUM_SCAN_TYPES);

	//				// we're mounted, so lets look around mid view.
	//			mTargetZone.UpdateAimPosition();
	//			}

	//			if(.mTargetZone.HasAim())
	//			mScanDirection = SCAN_ZONES;

	//			switch(.mScanDirection)
	//			{
	//			case SCAN_DEFAULT:
	//				if(.mMapGoal->GetFacing() != Vector3f::ZERO)
	//				{
	//				mAimPoint = mMG42Position + mMapGoal->GetFacing() * 1024.f;
	//					break;
	//				}
	//			case SCAN_MIDDLE:
	//				{
	//				mAimPoint = mMG42Position + mGunCenterArc * 1024.f;
	//					break;
	//				}
	//			case SCAN_LEFT:
	//				if(.mScanLeft != Vector3f::ZERO)
	//				{
	//				mAimPoint = mMG42Position + mScanLeft * 1024.f;
	//					break;
	//				}
	//			case SCAN_RIGHT:
	//				if(.mScanRight != Vector3f::ZERO)
	//				{
	//				mAimPoint = mMG42Position + mScanRight * 1024.f;
	//					break;
	//				}
	//			case SCAN_ZONES:
	//				{
	//				mAimPoint = mTargetZone.GetAimPosition();
	//					break;
	//				}
	//			default:
	//				break;
	//			}
	//		}
	//	}
	//	return State_Busy;
	//}

	//bool MountMg42::_GetMG42Properties()
	//{
	//	ET_MG42Info data;
	//	if(!gEngineFuncs->GetMg42Properties(GetClient(), data))
	//		return false;

	//mGunCenterArc = Vector3f(data.mCenterFacing);

	//mMinHorizontalArc = data.mMinHorizontalArc;
	//mMaxHorizontalArc = data.mMaxHorizontalArc;
	//mMinVerticalArc = data.mMinVerticalArc;
	//mMaxVerticalArc = data.mMaxVerticalArc;

	//	// Calculate the planes for the MG42

	//	/*Matrix3f planeMatrices[4];
	//	planeMatrices[0].FromEulerAnglesXYZ(.mMinHorizontalArc, 0.0f, 0.0f);
	//	planeMatrices[1].FromEulerAnglesXYZ(.mMaxHorizontalArc, 0.0f, 0.0f);
	//	planeMatrices[2].FromEulerAnglesXYZ(0.0f, mMinHorizontalArc, 0.0f);
	//	planeMatrices[3].FromEulerAnglesXYZ(0.0f, mMaxHorizontalArc, 0.0f);

	//mGunArcPlanes[0] = Plane3f(.mGunCenterArc * planeMatrices[0], mMG42Position);
	//mGunArcPlanes[1] = Plane3f(.mGunCenterArc * planeMatrices[1], mMG42Position);
	//mGunArcPlanes[2] = Plane3f(.mGunCenterArc * planeMatrices[2], mMG42Position);
	//mGunArcPlanes[3] = Plane3f(.mGunCenterArc * planeMatrices[3], mMG42Position);*/

	//	const float fScanPc = 0.4f;

	//	Quaternionf ql;
	//	ql.FromAxisAngle(Vector3f::UNIT_Z, Mathf::DegToRad(.mMinHorizontalArc * fScanPc));
	//mScanLeft = ql.Rotate(.mGunCenterArc);

	//	Quaternionf qr;
	//	qr.FromAxisAngle(Vector3f::UNIT_Z, Mathf::DegToRad(.mMaxHorizontalArc * fScanPc));
	//mScanRight = qr.Rotate(.mGunCenterArc);

	//	return true;
	//}

	//////////////////////////////////////////////////////////////////////////

	//MobileMg42::MobileMg42()
	//	: StateChild("MobileMg42")
	//	, FollowPathUser("MobileMg42")
	//	, mMinCampTime(6.f)
	//	, mMaxCampTime(10.f)
	//	, mExpireTime(0)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_MOBILE_MG42);
	//}

	//void MobileMg42::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void MobileMg42::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::CYAN,5.f);
	//	mTargetZone.RenderDebug();
	//	}
	//}

	//// FollowPathUser functions.
	//bool MobileMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool MobileMg42::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mAimPosition;
	//	return true;
	//}

	//void MobileMg42::OnTarget()
	//{
	//	FINDSTATE(wp,WeaponSystem,GetRootState());
	//	if(wp)
	//	{
	//		if(!wp->CurrentWeaponIs(ET_WP_MOBILE_MG42_SET) && GetClient()->HasEntityFlag(ENT_FLAG_PRONED))
	//		{
	//			if(wp->CurrentWeaponIs(ET_WP_MOBILE_MG42))
	//				wp->AddWeaponRequest(Priority::Medium, GetNameHash(), ET_WP_MOBILE_MG42_SET);
	//			else
	//				wp->AddWeaponRequest(Priority::Medium, GetNameHash(), ET_WP_MOBILE_MG42);
	//		}
	//		else
	//		{
	//			wp->SetOverrideWeaponID(ET_WP_MOBILE_MG42_SET);
	//		}
	//	}
	//}

	//float MobileMg42::GetPriority()
	//{
	//	FINDSTATE(at,AttackTarget,GetRootState());
	//	if(at != NULL && at->IsActive() && at->TargetExceedsWeaponLimits())
	//		return 0.f;

	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0xbe8488ed /* MOBILEMG42 */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	qry.GetBest(.mMapGoal);
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void MobileMg42::Enter()
	//{
	//mMapGoal->GetProperty("MinCampTime",.mMinCampTime);
	//mMapGoal->GetProperty("MaxCampTime",.mMaxCampTime);
	//mExpireTime = 0;

	//mTargetZone.Restart(256.f);
	//	Tracker.InProgress = mMapGoal;
	//mAimPosition = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void MobileMg42::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATE(wp,WeaponSystem,GetRootState());
	//	if(wp)
	//	{
	//		wp->ReleaseWeaponRequest(GetNameHash());
	//		wp->ClearOverrideWeaponID();
	//	}
	//	Tracker.Reset();
	//}

	//State::StateStatus MobileMg42::Update(float fDt)
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
	//		// Only hang around here for a certain mount of time
	//		if(.mExpireTime==0)
	//		{
	//		mExpireTime = IGame::GetTime()+Mathf::IntervalRandomInt(.mMinCampTime.GetMs(),.mMaxCampTime.GetMs());
	//			Tracker.InUse = mMapGoal;
	//		}
	//		else if(IGame::GetTime() > mExpireTime)
	//		{
	//			// Delay it from being used for a while.
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		GetClient()->PressButton(BOT_BUTTON_PRONE);

	//		Priority::ePriority pri = /*.mIgnoreTargets ? Priority::High :*/ Priority::Low;
	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));

	//		// Handle aim limitations.
	//		FINDSTATE(wp,WeaponSystem,GetRootState());
	//		if(wp != NULL && wp->CurrentWeaponIs(ET_WP_MOBILE_MG42_SET))
	//		{
	//		mTargetZone.Update(GetClient());
	//			if(.mTargetZone.HasAim())
	//			mAimPosition = mTargetZone.GetAimPosition();
	//		}
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//RepairMg42::RepairMg42()
	//	: StateChild("RepairMg42")
	//	, FollowPathUser("RepairMg42")
	//	, mIgnoreTargets(false)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_PLIERS);
	//}

	//void RepairMg42::GetDebugString(std::stringstream &out)
	//{
	//	if(IsActive())
	//	{
	//		//if(!GetClient()->HasEntityFlag(ET_ENT_FLAG_MOUNTED))
	//		out << "Repairing " + (.mMapGoal?.mMapGoal->GetName():"");
	//	}
	//}

	//void RepairMg42::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool RepairMg42::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool RepairMg42::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mMG42Position;
	//	return true;
	//}

	//void RepairMg42::OnTarget()
	//{
	//	FINDSTATE(ws,WeaponSystem,GetRootState());
	//	if(ws != NULL && ws->GetCurrentRequestOwner() == GetNameHash())
	//		ws->FireWeapon();
	//}

	//float RepairMg42::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//	BitFlag64 entFlags;

	//	GoalManager::Query qry(0x17929136 /* REPAIRMG42 */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//			continue;

	//		bool bBroken = gEngineFuncs->IsMountableGunRepairable(GetClient(), qry.mList[i]->GetEntity());
	//		if(!bBroken)
	//			continue;

	//	mMapGoal = qry.mList[i];
	//		break;
	//	}
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void RepairMg42::Enter()
	//{
	//mMapGoal->GetProperty("IgnoreTargets",.mIgnoreTargets);

	//	Tracker.InProgress = mMapGoal;
	//mMG42Position = mMapGoal->GetWorldBounds().Center;
	//	//.mMapGoal->GetWorldBounds().CenterBottom(.mMG42Position);
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void RepairMg42::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	Tracker.Reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem,GetRootState(),ReleaseWeaponRequest(GetNameHash()));
	//}

	//State::StateStatus RepairMg42::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	bool bBroken = gEngineFuncs->IsMountableGunRepairable(GetClient(), mMapGoal->GetEntity());
	//	if(!bBroken || mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		GetClient()->PressButton(BOT_BUTTON_CROUCH);
	//		GetClient()->GetSteeringSystem()->SetTarget(.mMapGoal->GetPosition());

	//		Priority::ePriority pri = mIgnoreTargets ? Priority::High : Priority::Medium;
	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(pri,this,GetNameHash()));
	//		FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(pri, GetNameHash(), ET_WP_PLIERS));
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//TakeCheckPoint::TakeCheckPoint()
	//	: StateChild("TakeCheckPoint")
	//	, FollowPathUser("TakeCheckPoint")
	//{
	//}

	//void TakeCheckPoint::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void TakeCheckPoint::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool TakeCheckPoint::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//float TakeCheckPoint::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0xf7e4a57f /* CHECKPOINT */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	qry.GetBest(.mMapGoal);

	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void TakeCheckPoint::Enter()
	//{
	//mTargetPosition = mMapGoal->GetWorldBounds().Center;
	//	Tracker.InProgress = mMapGoal;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run, true));
	//}

	//void TakeCheckPoint::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();
	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(), ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus TakeCheckPoint::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//	mTargetPosition.Z() = GetClient()->GetPosition().Z();
	//		GetClient()->GetSteeringSystem()->SetTarget(.mTargetPosition, 32.f);
	//	}
	//	return State_Busy;
	//}

	//////////////////////////////////////////////////////////////////////////

	//ReviveTeammate::ReviveTeammate()
	//	: StateChild("ReviveTeammate")
	//	, FollowPathUser("ReviveTeammate")
	//	, mRange(2000.f)
	//{
	//	LimitToWeapon().SetFlag(ET_WP_SYRINGE);
	//}

	//void ReviveTeammate::GetDebugString(std::stringstream &out)
	//{
	//	switch(.mGoalState)
	//	{
	//	case REVIVING:
	//		out << "Reviving ";
	//		break;
	//	case HEALING:
	//		out << "Healing ";
	//		break;
	//	}

	//	if(.mMapGoal && mMapGoal->GetEntity().IsValid())
	//		out << std::endl << EngineFuncs::EntityName(.mMapGoal->GetEntity(), "<noname>");
	//}

	//void ReviveTeammate::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);
	//		RenderBuffer::AddLine(GetClient()->GetPosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::MAGENTA,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool ReviveTeammate::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool ReviveTeammate::GetAimPosition(Vector3f &_aimpos)
	//{
	//	const Box3f obb = mMapGoal->GetWorldBounds();
	//	if ( mGoalState == REVIVING )
	//		_aimpos = obb.GetCenterBottom();
	//	else
	//		_aimpos = obb.Center;
	//	return true;
	//}

	//void ReviveTeammate::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws)
	//	{
	//		if(gEngineFuncs->IsAlive(.mMapGoal->GetEntity()))
	//		{
	//			if(ws->CurrentWeaponIs(ET_WP_MEDKIT))
	//				ws->FireWeapon();
	//		}
	//		else
	//		{
	//			if(ws->CurrentWeaponIs(ET_WP_SYRINGE))
	//				ws->FireWeapon();
	//		}
	//	}
	//}

	//float ReviveTeammate::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	//qry.GetBest(.mMapGoal);

	//	float fDistToRevive;
	//	float fClosestRevive = 0.f;
	//	MapGoalList::iterator mIt = qry.mList.begin();
	//	for(; mIt != qry.mList.end(); )
	//	{
	//		fDistToRevive = SquaredLength2d((*mIt)->GetPosition(), GetClient()->GetPosition());
	//		(*mIt)->GetProperty("Range",.mRange);
	//		if ( fDistToRevive > mRange * mRange )
	//		{
	//			BlackboardDelay(5.f, (*mIt)->GetSerialNum()); // ignore it for a while so dist calcs aren't done every frame
	//	mIt = qry.mList.erase(mIt);
	//			continue;
	//		}

	//		// use the closest one or the first one found within 200 units
	//		if ( fClosestRevive == 0.f || (fDistToRevive < fClosestRevive && fDistToRevive > 40000.f) )
	//		{
	//			fClosestRevive = fDistToRevive;
	//		mMapGoal = (*mIt);
	//		}

	//		++mIt;
	//	}

	//mList = qry.mList;
	//	// mMapGoal.reset();
	//	// TODO: check weapon capability vs target underwater?

	//	return mList.empty() && mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void ReviveTeammate::Enter()
	//{
	//mGoalState = REVIVING;

	//mCheckReviveTimer.Delay(2.f);

	//	FINDSTATE(fp, FollowPath, GetRootState());
	//	if(fp != NULL && fp->Goto(this, mList, Run))
	//	{
	//	mMapGoal = mList[GetDestinationIndex()];
	//		Tracker.InProgress = mMapGoal;
	//	}
	//	else
	//	{
	//	mMapGoal.reset();
	//		Tracker.Reset();
	//		for(uint32_t i = 0; i < mList.size(); ++i)
	//		{
	//			BlackboardDelay(10.f, mList[i]->GetSerialNum());
	//		}
	//	}
	//}

	//void ReviveTeammate::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus ReviveTeammate::Update(float fDt)
	//{
	//	if(!mMapGoal)
	//		return State_Finished;

	//	if(.mCheckReviveTimer.IsExpired())
	//	{
	//		// finish if there's new goals, so we can activate again and go to a new goal
	//		if(AreThereNewReviveGoals())
	//			return State_Finished;
	//	mCheckReviveTimer.Delay(2.f);
	//	}

	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	GameEntity reviveEnt = mMapGoal->GetEntity();

	//	//////////////////////////////////////////////////////////////////////////
	//	Msg_HealthArmor ha;
	//	if(gEngineFuncs->GetHealthAndArmor(reviveEnt, ha) && ha.mCurrentHealth >= ha.mMaxHealth)
	//		return State_Finished;

	//	BitFlag64 ef;
	//	if(gEngineFuncs->GetEntityFlags(reviveEnt,ef))
	//	{
	//		if(ef.CheckFlag(ET_ENT_FLAG_INLIMBO))
	//			return State_Finished;
	//	}
	//	//////////////////////////////////////////////////////////////////////////

	//	if(DidPathSucceed())
	//	{
	//		if(GetClient()->GetStuckTime() > 2000)
	//		{
	//			BlackboardDelay(10.f, mMapGoal->GetSerialNum());
	//			return State_Finished;
	//		}

	//		FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::High,this,GetNameHash()));
	//		GetClient()->GetSteeringSystem()->SetTarget(.mMapGoal->GetPosition());

	//		switch(.mGoalState)
	//		{
	//		case REVIVING:
	//			{
	//				if(gEngineFuncs->IsAlive(reviveEnt))
	//				{
	//				mGoalState = HEALING;
	//				}
	//				else
	//				{
	//					GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
	//					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::High, GetNameHash(), ET_WP_SYRINGE));
	//				}

	//				// if the height differences are significant, jump/crouch
	//				const Vector3f eyePos = GetClient()->GetEyePosition();
	//				Vector3f aimPos;
	//				GetAimPosition( aimPos );
	//				const float heightDiff = aimPos.Z() - eyePos.Z();
	//				if ( heightDiff > 20.f ) {
	//					if(GetClient()->GetEntityFlags().CheckFlag(ENT_FLAG_ONGROUND)) {
	//						GetClient()->PressButton(BOT_BUTTON_JUMP);
	//					}
	//				} else /*if ( heightDiff > 20.f )*/ {
	//					BitFlag64 btns;
	//					btns.SetFlag(BOT_BUTTON_CROUCH);
	//					GetClient()->HoldButton(btns,IGame::GetDeltaTime()*2);
	//				}

	//				break;
	//			}
	//		case HEALING:
	//			{
	//				// cs: bb delay any time the goal needs interrupted here
	//				// otherwise it's a loop of goto and then finish which causes them
	//				// to just 'stick' to the target without doing anything useful

	//				if(GetClient()->GetTargetingSystem()->HasTarget())
	//				{
	//					BlackboardDelay(5.f, mMapGoal->GetSerialNum());
	//					return State_Finished;
	//				}

	//				if(!gEngineFuncs->IsWeaponCharged(GetClient(), ET_WP_MEDKIT, Primary))
	//				{
	//					BlackboardDelay(5.f, mMapGoal->GetSerialNum());
	//					return State_Finished;
	//				}

	//				GetClient()->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime() + 1000);
	//				FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::High, GetNameHash(), ET_WP_MEDKIT));
	//				break;
	//			}
	//		}
	//	}
	//	return State_Busy;
	//}

	//bool ReviveTeammate::AreThereNewReviveGoals()
	//{
	//	GoalManager::Query qry(0x2086cdf0 /* REVIVE */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);

	//	for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//	{
	//		if(std::find(.mList.begin(),.mList.end(),qry.mList[i]) == mList.end())
	//			return true;
	//	}

	//	return false;
	//}

	//////////////////////////////////////////////////////////////////////////

	//DefuseDynamite::DefuseDynamite()
	//	: StateChild("DefuseDynamite")
	//	, FollowPathUser("DefuseDynamite")
	//{
	//	LimitToWeapon().SetFlag(ET_WP_PLIERS);
	//}

	//void DefuseDynamite::GetDebugString(std::stringstream &out)
	//{
	//	out << (.mMapGoal ? mMapGoal->GetName() : "");
	//}

	//void DefuseDynamite::RenderDebug()
	//{
	//	if(IsActive())
	//	{
	//		RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);
	//		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::GREEN,5.f);
	//	}
	//}

	//// FollowPathUser functions.
	//bool DefuseDynamite::GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt)
	//{
	//	if(.mMapGoal && mMapGoal->RouteTo(GetClient(), _desination, 64.f))
	//		_final = false;
	//	else
	//		_final = true;
	//	return true;
	//}

	//// AimerUser functions.
	//bool DefuseDynamite::GetAimPosition(Vector3f &_aimpos)
	//{
	//	_aimpos = mMapGoal->GetWorldBounds().Center;
	//	return true;
	//}

	//void DefuseDynamite::OnTarget()
	//{
	//	FINDSTATE(ws, WeaponSystem, GetRootState());
	//	if(ws != NULL && ws->CurrentWeaponIs(ET_WP_PLIERS))
	//		ws->FireWeapon();
	//}

	//float DefuseDynamite::GetPriority()
	//{
	//	if(IsActive())
	//		return GetLastPriority();

	//mMapGoal.reset();

	//	GoalManager::Query qry(0x1899efc7 /* DEFUSE */, GetClient());
	//	System::mInstance->mGoalManager->GetGoals(qry);
	//	for(uint32_t i = 0; i < qry.mList.size(); ++i)
	//	{
	//		if(BlackboardIsDelayed(qry.mList[i]->GetSerialNum()))
	//			continue;

	//		if(qry.mList[i]->GetSlotsOpen(MapGoal::TRACK_INPROGRESS) < 1)
	//			continue;

	//		if(gEngineFuncs->GetExplosiveState(GetClient(), qry.mList[i]->GetEntity()) == XPLO_ARMED)
	//		{
	//		mMapGoal = qry.mList[i];
	//			break;
	//		}
	//		else
	//		{
	//			qry.mList[i]->SetDeleteMe(true);
	//		}
	//	}
	//	return mMapGoal ? mMapGoal->GetPriorityForClient(GetClient()) : 0.f;
	//}

	//void DefuseDynamite::Enter()
	//{
	//mTargetPosition = mMapGoal->GetWorldBounds().Center;
	//	Tracker.InProgress = mMapGoal;
	//	FINDSTATEIF(FollowPath, GetRootState(), Goto(this, Run));
	//}

	//void DefuseDynamite::Exit()
	//{
	//	FINDSTATEIF(FollowPath, GetRootState(), Stop(true));

	//mMapGoal.reset();

	//	FINDSTATEIF(Aimer,GetRootState(),ReleaseAimRequest(GetNameHash()));
	//	FINDSTATEIF(WeaponSystem, GetRootState(),ReleaseWeaponRequest(GetNameHash()));

	//	Tracker.Reset();
	//}

	//State::StateStatus DefuseDynamite::Update(float fDt)
	//{
	//	if(DidPathFail())
	//	{
	//		BlackboardDelay(2.f, mMapGoal->GetSerialNum()); // cs: was 10 seconds
	//		return State_Finished;
	//	}

	//	if(!mMapGoal->IsAvailable(GetClient()->GetTeam()))
	//		return State_Finished;

	//	if(DidPathSucceed())
	//	{
	//		ExplosiveState eState = gEngineFuncs->GetExplosiveState(GetClient(), mMapGoal->GetEntity());
	//		switch(eState)
	//		{
	//		case XPLO_INVALID:
	//		case XPLO_UNARMED:
	//			return State_Finished;
	//		default:
	//			break; // keep processing
	//		}

	//	mTargetPosition = mMapGoal->GetWorldBounds().Center;

	//		const float fDistanceToDynamite = SquaredLength2d(.mTargetPosition, GetClient()->GetPosition());
	//		if(fDistanceToDynamite > 2500.0f)
	//		{
	//			GetClient()->GetSteeringSystem()->SetTarget(.mTargetPosition);
	//		}
	//		else
	//		{
	//			GetClient()->PressButton(BOT_BUTTON_CROUCH);

	//			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(Priority::High,this,GetNameHash()));
	//			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(Priority::High, GetNameHash(), ET_WP_PLIERS));
	//		}
	//	}
	//	return State_Busy;
	//}
};