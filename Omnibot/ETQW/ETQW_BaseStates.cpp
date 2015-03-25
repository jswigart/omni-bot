////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ETQW_BaseStates.h"
#include "ETQW_InterfaceFuncs.h"

#include "BotPathing.h"
#include "BotWeaponSystem.h"
#include "BotSteeringSystem.h"
#include "BotTargetingSystem.h"

#include "RenderBuffer.h"

namespace AiState
{
	BuildConstruction::BuildConstruction()
		: StateChild( "BuildConstruction" )
		, FollowPathUser( "BuildConstruction" )
		, mAdjustedPosition( false )
	{
		LimitToClass().SetFlag( ETQW_CLASS_ENGINEER );
	}

	void BuildConstruction::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mConstructionPos, COLOR::GREEN );
		}
	}

	// FollowPathUser functions.
	bool BuildConstruction::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool BuildConstruction::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mConstructionPos;
		return true;
	}

	void BuildConstruction::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws && ws->CurrentWeaponIs( ETQW_WP_PLIERS ) )
			ws->FireWeapon();

		//InterfaceFuncs::GetCurrentCursorHint
	}

	float BuildConstruction::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		GoalManager::Query qry( 0xc39bf2a3 /* BUILD */, GetClient() );
		GoalManager::GetInstance()->GetGoals( qry );
		for ( uint32_t i = 0; i < qry.mList.size(); ++i )
		{
			if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
				continue;

			if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
				continue;

			ConstructableState cState = InterfaceFuncs::GetConstructableState( GetClient(), qry.mList[ i ]->GetEntity() );
			if ( cState == CONST_UNBUILT )
			{
				mMapGoal = qry.mList[ i ];
				break;
			}
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void BuildConstruction::Enter()
	{
		mAdjustedPosition = false;
		mConstructionPos = mMapGoal->GetWorldBounds().Center;

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run, true ) );

		Tracker.InProgress = mMapGoal;
	}

	void BuildConstruction::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus BuildConstruction::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		// Check the construction status
		ConstructableState cState = InterfaceFuncs::GetConstructableState( GetClient(), mMapGoal->GetEntity() );
		switch ( cState )
		{
			case CONST_INVALID: // Invalid constructable, fail
				return State_Finished;
			case CONST_BUILT:	// It's built, consider it a success.
				return State_Finished;
			case CONST_UNBUILT:
			case CONST_BROKEN:
				// This is what we want.
				break;
		}
		//////////////////////////////////////////////////////////////////////////

		if ( DidPathSucceed() )
		{
			float fDistanceToConstruction = ( mConstructionPos - GetClient()->GetPosition() ).SquaredLength();
			if ( fDistanceToConstruction > 4096.0f )
			{
				// check for badly waypointed maps
				if ( !mAdjustedPosition )
				{
					// use our z value because some trigger entities may be below the ground
					Vector3f checkPos( mConstructionPos.X(), mConstructionPos.Y(), GetClient()->GetEyePosition().Z() );

					obTraceResult tr;
					EngineFuncs::TraceLine( tr, GetClient()->GetEyePosition(), checkPos,
						NULL, ( TR_MASK_SOLID | TR_MASK_PLAYERCLIP ), GetClient()->GetGameID(), True );

					if ( tr.mFraction != 1.0f && !tr.mHitEntity.IsValid() )
					{
						mMapGoal->SetDeleteMe( true );
						return State_Finished;
					}

					// do a trace to adjust position
					EngineFuncs::TraceLine( tr,
						GetClient()->GetEyePosition(),
						mConstructionPos,
						NULL, ( TR_MASK_SOLID | TR_MASK_PLAYERCLIP ), -1, False );

					if ( tr.mFraction != 1.0f )
					{
						mConstructionPos = *(Vector3f*)tr.mEndpos;
					}

					mAdjustedPosition = true;
				}
			}
			else
			{
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_PLIERS ) );
			}
			GetClient()->GetSteeringSystem()->SetTarget( mConstructionPos, 64.f );
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	PlantExplosive::PlantExplosive()
		: StateChild( "PlantExplosive" )
		, FollowPathUser( "PlantExplosive" )
	{
		LimitToClass().SetFlag( ETQW_CLASS_ENGINEER );
		LimitToClass().SetFlag( ETQW_CLASS_COVERTOPS );
		SetAlwaysRecieveEvents( true );
	}

	void PlantExplosive::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
		}
	}

	// FollowPathUser functions.
	bool PlantExplosive::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool PlantExplosive::GetAimPosition( Vector3f &_aimpos )
	{
		switch ( mGoalState )
		{
			case LAY_EXPLOSIVE:
				_aimpos = mTargetPosition;
				break;
			case ARM_EXPLOSIVE:
				_aimpos = mExplosivePosition;
				break;
			case RUNAWAY:
			case DETONATE_EXPLOSIVE:
			default:
				OBASSERT( 0, "Invalid Aim State" );
				return false;
		}
		return true;
	}

	void PlantExplosive::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws )
		{
			if ( mGoalState == LAY_EXPLOSIVE )
			{
				if ( ws->CurrentWeaponIs( ETQW_WP_HE_CHARGE ) )
					ws->FireWeapon();
			}
			else if ( mGoalState == ARM_EXPLOSIVE )
			{
				if ( ws->CurrentWeaponIs( ETQW_WP_PLIERS ) )
					ws->FireWeapon();
			}
			else if ( mGoalState == DETONATE_EXPLOSIVE )
			{
				/*if(ws->CurrentWeaponIs(ETQW_WP_SATCHEL_DET))
				ws->FireWeapon();*/
			}
		}
	}

	float PlantExplosive::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		ExplosiveTargetType myTargetType = XPLO_TYPE_DYNAMITE;
		ETQW_Weapon weaponType = ETQW_WP_HE_CHARGE;
		switch ( GetClient()->GetClass() )
		{
			case ETQW_CLASS_ENGINEER:
			{
				weaponType = ETQW_WP_HE_CHARGE;
				myTargetType = XPLO_TYPE_DYNAMITE;
				break;
			}
			/*case ETQW_CLASS_COVERTOPS:
			{
			weaponType = ETQW_WP_SATCHEL;
			myTargetType = XPLO_TYPE_SATCHEL;
			break;
			}*/
			default:
				OBASSERT( 0, "Wrong Class with Evaluator_PlantExplosive" );
				return 0.0;
		}

		mMapGoal.reset();

		if ( InterfaceFuncs::IsWeaponCharged( GetClient(), weaponType, Primary ) )
		{
			{
				GoalManager::Query qry( 0xbbcae592 /* PLANT */, GetClient() );
				GoalManager::GetInstance()->GetGoals( qry );
				for ( uint32_t i = 0; i < qry.mList.size(); ++i )
				{
					if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
						continue;

					if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
						continue;

					ConstructableState cState = InterfaceFuncs::IsDestroyable( GetClient(), qry.mList[ i ]->GetEntity() );
					if ( cState == CONST_DESTROYABLE )
					{
						mMapGoal = qry.mList[ i ];
						break;
					}
				}
			}

			if ( !mMapGoal )
			{
				GoalManager::Query qry( 0xa411a092 /* MOVER */, GetClient() );
				GoalManager::GetInstance()->GetGoals( qry );
				for ( uint32_t i = 0; i < qry.mList.size(); ++i )
				{
					if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
						continue;

					mMapGoal = qry.mList[ i ];
					break;
				}
			}
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void PlantExplosive::Enter()
	{
		// set position to base of construction
		Box3f obb = mMapGoal->GetWorldBounds();
		mExplosivePosition = obb.GetCenterBottom();
		mTargetPosition = mExplosivePosition;

		mAdjustedPosition = false;
		mGoalState = LAY_EXPLOSIVE;

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run, true ) );

		Tracker.InProgress = mMapGoal;
	}

	void PlantExplosive::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mExplosiveEntity.Reset();

		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus PlantExplosive::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		// If it's not destroyable, consider it a success.
		if ( !InterfaceFuncs::IsDestroyable( GetClient(), mMapGoal->GetEntity() ) )
			return State_Finished;

		if ( mExplosiveEntity.IsValid() && !IGame::IsEntityValid( mExplosiveEntity ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			switch ( GetClient()->GetClass() )
			{
				case ETQW_CLASS_ENGINEER:
					return _UpdateDynamite();
				case ETQW_CLASS_COVERTOPS:
					return _UpdateSatchel();
				default:
					OBASSERT( 0, "Wrong Class in PlantExplosive" );
			}
		}
		return State_Busy;
	}

	State::StateStatus PlantExplosive::_UpdateDynamite()
	{
		switch ( mGoalState )
		{
			case LAY_EXPLOSIVE:
			{
				if ( !InterfaceFuncs::IsWeaponCharged( GetClient(), ETQW_WP_HE_CHARGE, Primary ) )
					return State_Finished;

				/*if(.mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
				{
				RenderBuffer::AddOBB(.mMapGoal->GetWorldBounds(), COLOR::ORANGE);

				Vector3f vCenter;
				mMapGoal->GetWorldBounds().CenterPoint(vCenter);
				RenderBuffer::AddLine(.mClient->GetPosition(), vCenter, COLOR::GREEN, 1.0f);
				RenderBuffer::AddLine(.mClient->GetPosition(), mMapGoal->GetPosition(), COLOR::RED, 1.0f);
				}*/

				float fDistanceToTarget = ( mTargetPosition - GetClient()->GetPosition() ).SquaredLength();
				if ( fDistanceToTarget > 10000.0f )
				{
					// Move toward it.
					//.mClient->GetSteeringSystem()->SetTarget(.mTargetPosition);

					// check for badly waypointed maps
					if ( !mAdjustedPosition )
					{
						mAdjustedPosition = true;

						// use our z value because some trigger entities may be below the ground
						Vector3f vCheckPos( mTargetPosition.X(), mTargetPosition.Y(), GetClient()->GetEyePosition().Z() );
						/*if(.mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
						{
						RenderBuffer::AddLine(GetClient()->GetEyePosition(), vCheckPos, COLOR::GREEN, 2.0f);
						}*/

						obTraceResult tr;
						EngineFuncs::TraceLine( tr, GetClient()->GetEyePosition(), vCheckPos,
							NULL, ( TR_MASK_SOLID | TR_MASK_PLAYERCLIP ), GetClient()->GetGameID(), True );
						if ( tr.mFraction != 1.0f && !tr.mHitEntity.IsValid() )
						{
							//.mTargetEntity->SetDeleteMe(true);
							return State_Finished;
						}

						// do a trace to adjust position
						EngineFuncs::TraceLine( tr, GetClient()->GetEyePosition(), mTargetPosition,
							NULL, ( TR_MASK_SOLID | TR_MASK_PLAYERCLIP ), -1, False );
						if ( tr.mFraction != 1.0f )
						{
							mTargetPosition = Vector3f( tr.mEndpos );
						}
					}
				}
				else
				{
					// We're within range, so let's start laying.
					GetClient()->ResetStuckTime();
					FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_HE_CHARGE ) );
				}
				break;
			}
			case ARM_EXPLOSIVE:
			{
				if ( InterfaceFuncs::GetExplosiveState( GetClient(), mExplosiveEntity ) == XPLO_ARMED )
				{
					BlackboardDelay( 30.f, mMapGoal->GetSerialNum() );
					return State_Finished;
				}

				// Disable avoidance for this frame.
				GetClient()->GetSteeringSystem()->SetNoAvoidTime( IGame::GetTime() );

				// update dynamite position
				EngineFuncs::EntityPosition( mExplosiveEntity, mExplosivePosition );

				// move a little bit close if dyno too far away
				if ( ( mExplosivePosition - GetClient()->GetPosition() ).SquaredLength() > 2500.0f )
				{
					GetClient()->GetSteeringSystem()->SetTarget( mExplosivePosition );
				}
				else
				{
					GetClient()->ResetStuckTime();
					FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_PLIERS ) );
				}
				break;
			}
			default:
				// keep processing
				break;
		}
		return State_Busy;
	}

	State::StateStatus PlantExplosive::_UpdateSatchel()
	{
		//switch(.mGoalState)
		//{
		//case LAY_EXPLOSIVE:
		//	{
		//		if(!InterfaceFuncs::IsWeaponCharged(GetClient(), ETQW_WP_SATCHEL, Primary))
		//			return State_Finished;

		//		float fDistanceToTarget = (.mTargetPosition - GetClient()->GetPosition()).SquaredLength();
		//		if (fDistanceToTarget > 10000.0f)
		//		{
		//			// Move toward it.
		//			//GetClient()->GetSteeringSystem()->SetTarget(.mTargetPosition);

		//			// check for badly waypointed maps
		//			if (!mAdjustedPosition)
		//			{
		//			 mAdjustedPosition = true;

		//				// use our z value because some trigger entities may be below the ground
		//				Vector3f vCheckPos(.mTargetPosition.X(), mTargetPosition.Y(), GetClient()->GetEyePosition().Z());
		//				/*if(.mClient->IsDebugEnabled(BOT_DEBUG_GOALS))
		//				{
		//					RenderBuffer::AddLine(.mClient->GetEyePosition(), vCheckPos, COLOR::GREEN, 2.0f);
		//				}*/

		//				obTraceResult tr;
		//				EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), vCheckPos,
		//					NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), GetClient()->GetGameID(), True);
		//				if (tr.mFraction != 1.0f && !tr.mHitEntity.IsValid())
		//				{
		//					AABB aabb, mapaabb;
		//					EngineFuncs::EntityWorldAABB(.mMapGoal->GetEntity(), aabb);
		//					//gEngineFuncs->GetMapExtents(mapaabb);

		//					//.mTargetEntity->SetDeleteMe(true);
		//					return State_Finished;
		//				}

		//				// do a trace to adjust position
		//				EngineFuncs::TraceLine(tr, GetClient()->GetEyePosition(), mTargetPosition,
		//					NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, False);
		//				if (tr.mFraction != 1.0f)
		//				{
		//				 mTargetPosition = Vector3f(tr.mEndpos);
		//				}
		//			}
		//		}
		//		else
		//		{
		//			// We're within range, so let's start laying.
		//			GetClient()->ResetStuckTime();
		//			FINDSTATEIF(Aimer,GetRootState(),AddAimRequest(0.6f,this,GetNameHash()));
		//			FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(0.6f, GetNameHash(), ETQW_WP_SATCHEL));
		//		}
		//		break;
		//	}
		//case ARM_EXPLOSIVE:
		//case RUNAWAY:
		//	{
		//		OBASSERT(.mExplosiveEntity.IsValid(), "No Explosive Entity!");

		//		// Generate a random goal.
		//		FINDSTATEIF(FollowPath,GetRootState(),GotoRandomPt(this));
		//	 mGoalState = DETONATE_EXPLOSIVE;
		//		break;
		//	}
		//case DETONATE_EXPLOSIVE:
		//	{
		//		// Are we far enough away to blow it up?
		//		const float SATCHEL_DETQW_DISTANCE = 350.0f;
		//		const bool BLOW_TARGETQW_OR_NOT = true;

		//		Vector3f vSatchelPos;
		//		if(EngineFuncs::EntityPosition(.mExplosiveEntity, vSatchelPos))
		//		{
		//			if((GetClient()->GetPosition() - vSatchelPos).Length() >= SATCHEL_DETQW_DISTANCE)
		//			{
		//				// Do we need this? perhaps make this flag scriptable?
		//				if(BLOW_TARGETQW_OR_NOT || !GetClient()->GetTargetingSystem()->HasTarget())
		//				{
		//					FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(1.f, GetNameHash(), ETQW_WP_SATCHEL_DET));
		//					ExplosiveState eState = InterfaceFuncs::GetExplosiveState(GetClient(), mExplosiveEntity);
		//					if(eState == XPLO_INVALID)
		//						return State_Finished;
		//				}
		//			}
		//		}
		//		break;
		//	}
		//}
		return State_Busy;
	}

	void PlantExplosive::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
	{
		switch ( _message.GetMessageId() )
		{
			HANDLER( ACTION_WEAPON_FIRE )
			{
				const Event_WeaponFire *m = _message.Get<Event_WeaponFire>();
				if ( m->mWeaponId == ETQW_WP_HE_CHARGE && m->mProjectile.IsValid() )
				{
					mExplosiveEntity = m->mProjectile;
					mGoalState = ARM_EXPLOSIVE;
				}
				/*else if(m->mWeaponId == ETQW_WP_SATCHEL && m->mProjectile.IsValid())
				{
				mExplosiveEntity = m->mProjectile;
				mGoalState = RUNAWAY;
				}*/
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	MountMg42::MountMg42()
		: StateChild( "MountMg42" )
		, FollowPathUser( "MountMg42" )
		, mScanDirection( SCAN_DEFAULT )
		, mNextScanTime( 0 )
	{
	}

	void MountMg42::GetDebugString( std::stringstream &out )
	{
		if ( IsActive() )
		{
			if ( !GetClient()->HasEntityFlag( ETQW_ENT_FLAG_MOUNTED ) )
				out << "Mounting ";
			else
			{
				switch ( mScanDirection )
				{
					case SCAN_DEFAULT:
						out << "Scan Facing ";
						break;
					case SCAN_MIDDLE:
						out << "Scan Middle ";
						break;
					case SCAN_LEFT:
						out << "Scan Left ";
						break;
					case SCAN_RIGHT:
						out << "Scan Right ";
						break;
				}
			}

			if ( mMapGoal )
				out << mMapGoal->GetName();
		}
	}

	void MountMg42::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
			mTargetZone.RenderDebug();
		}
	}

	// FollowPathUser functions.
	bool MountMg42::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool MountMg42::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mAimPoint;
		return true;
	}

	void MountMg42::OnTarget()
	{
		if ( !GetClient()->HasEntityFlag( ETQW_ENT_FLAG_MOUNTED ) )
			GetClient()->PressButton( BOT_BUTTON_USE );
	}

	float MountMg42::GetPriority()
	{
		if ( IsActive() || GetClient()->HasEntityFlag( ETQW_ENT_FLAG_MOUNTED ) )
			return GetLastPriority();

		BitFlag64 entFlags;

		GoalManager::Query qry( 0xe1a2b09c /* MOUNTMG42 */, GetClient() );
		GoalManager::GetInstance()->GetGoals( qry );
		for ( uint32_t i = 0; i < qry.mList.size(); ++i )
		{
			if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
				continue;

			GameEntity gunOwner = InterfaceFuncs::GetMountedPlayerOnMG42( GetClient(), qry.mList[ i ]->GetEntity() );
			int gunHealth = InterfaceFuncs::GetGunHealth( GetClient(), qry.mList[ i ]->GetEntity() );
			bool bBroken = InterfaceFuncs::IsMountableGunRepairable( GetClient(), qry.mList[ i ]->GetEntity() );

			if ( bBroken )
				continue;
			
			EntityInfo entInfo;			
			if ( !IGame::GetEntityInfo( qry.mList[ i ]->GetEntity(), entInfo ) || !entInfo.mFlags.CheckFlag( ETQW_ENT_FLAG_ISMOUNTABLE ) )
				continue;

			// Make sure nobody has it mounted.
			if ( ( !gunOwner.IsValid() || !GetClient()->IsAllied( gunOwner ) ) && ( gunHealth > 0 ) )
			{
				mMapGoal = qry.mList[ i ];
				break;
			}
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void MountMg42::Enter()
	{
		mScanDirection = SCAN_MIDDLE;
		mNextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom( 2000.0f, 7000.0f );

		mAimPoint = mMapGoal->GetPosition();
		mMG42Position = mAimPoint;

		mScanLeft = Vector3f::ZERO;
		mScanRight = Vector3f::ZERO;

		mGotGunProperties = false;
		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run, true ) );

		Tracker.InProgress = mMapGoal;

		mTargetZone.Restart( 256.f );
	}

	void MountMg42::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );

		if ( GetClient()->HasEntityFlag( ETQW_ENT_FLAG_MOUNTED ) )
			GetClient()->PressButton( BOT_BUTTON_USE );

		Tracker.Reset();
	}

	State::StateStatus MountMg42::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		//////////////////////////////////////////////////////////////////////////
		// Only fail if a friendly player is on this gun or gun has been destroyed in the meantime
		//int gunHealth = InterfaceFuncs::GetGunHealth(.mClient, mMG42Goal->GetEntity());
		GameEntity mounter = InterfaceFuncs::GetMountedPlayerOnMG42( GetClient(), mMapGoal->GetEntity() );
		if ( InterfaceFuncs::IsMountableGunRepairable( GetClient(), mMapGoal->GetEntity() ) ||
			( mounter.IsValid() && ( mounter != GetClient()->GetGameEntity() ) && GetClient()->IsAllied( mounter ) ) )
		{
			return State_Finished;
		}
		//////////////////////////////////////////////////////////////////////////

		if ( DidPathSucceed() )
		{
			GetClient()->GetSteeringSystem()->SetTarget( mMapGoal->GetPosition() );
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );

			if ( GetClient()->HasEntityFlag( ETQW_ENT_FLAG_MOUNTED ) )
			{
				mTargetZone.Update( GetClient() );

				if ( !mGotGunProperties )
				{
					mGotGunProperties = true;
					_GetMG42Properties();
					mAimPoint = mMapGoal->GetPosition() + mGunCenterArc * 512.f;
				}

				if ( mNextScanTime < IGame::GetTime() )
				{
					mNextScanTime = IGame::GetTime() + (int)Mathf::IntervalRandom( 2000.0f, 7000.0f );
					mScanDirection = (int)Mathf::IntervalRandom( 0.0f, (float)NUM_SCAN_TYPES );

					// we're mounted, so lets look around mid view.
					mTargetZone.UpdateAimPosition();
				}

				if ( mTargetZone.HasAim() )
					mScanDirection = SCAN_ZONES;

				switch ( mScanDirection )
				{
					case SCAN_DEFAULT:
						if ( mMapGoal->GetFacing() != Vector3f::ZERO )
						{
							mAimPoint = mMG42Position + mMapGoal->GetFacing() * 1024.f;
							break;
						}
					case SCAN_MIDDLE:
					{
						mAimPoint = mMG42Position + mGunCenterArc * 1024.f;
						break;
					}
					case SCAN_LEFT:
						if ( mScanLeft != Vector3f::ZERO )
						{
							mAimPoint = mMG42Position + mScanLeft * 1024.f;
							break;
						}
					case SCAN_RIGHT:
						if ( mScanRight != Vector3f::ZERO )
						{
							mAimPoint = mMG42Position + mScanRight * 1024.f;
							break;
						}
					case SCAN_ZONES:
					{
						mAimPoint = mTargetZone.GetAimPosition();
						break;
					}
					default:
						break;
				}
			}
		}
		return State_Busy;
	}

	bool MountMg42::_GetMG42Properties()
	{
		ETQW_MG42Info data;
		if ( !InterfaceFuncs::GetMg42Properties( GetClient(), data ) )
			return false;

		mGunCenterArc = Vector3f( data.mCenterFacing );

		mMinHorizontalArc = data.mMinHorizontalArc;
		mMaxHorizontalArc = data.mMaxHorizontalArc;
		mMinVerticalArc = data.mMinVerticalArc;
		mMaxVerticalArc = data.mMaxVerticalArc;

		// Calculate the planes for the MG42

		/*Matrix3f planeMatrices[4];
		planeMatrices[0].FromEulerAnglesXYZ(.mMinHorizontalArc, 0.0f, 0.0f);
		planeMatrices[1].FromEulerAnglesXYZ(.mMaxHorizontalArc, 0.0f, 0.0f);
		planeMatrices[2].FromEulerAnglesXYZ(0.0f, mMinHorizontalArc, 0.0f);
		planeMatrices[3].FromEulerAnglesXYZ(0.0f, mMaxHorizontalArc, 0.0f);

		mGunArcPlanes[0] = Plane3f(.mGunCenterArc * planeMatrices[0], mMG42Position);
		mGunArcPlanes[1] = Plane3f(.mGunCenterArc * planeMatrices[1], mMG42Position);
		mGunArcPlanes[2] = Plane3f(.mGunCenterArc * planeMatrices[2], mMG42Position);
		mGunArcPlanes[3] = Plane3f(.mGunCenterArc * planeMatrices[3], mMG42Position);*/

		const float fScanPc = 0.4f;

		Quaternionf ql;
		ql.FromAxisAngle( Vector3f::UNIT_Z, Mathf::DegToRad( mMinHorizontalArc * fScanPc ) );
		mScanLeft = ql.Rotate( mGunCenterArc );

		Quaternionf qr;
		qr.FromAxisAngle( Vector3f::UNIT_Z, Mathf::DegToRad( mMaxHorizontalArc * fScanPc ) );
		mScanRight = qr.Rotate( mGunCenterArc );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	TakeCheckPoint::TakeCheckPoint()
		: StateChild( "TakeCheckPoint" )
		, FollowPathUser( "TakeCheckPoint" )
	{
	}

	void TakeCheckPoint::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
		}
	}

	// FollowPathUser functions.
	bool TakeCheckPoint::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	float TakeCheckPoint::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		GoalManager::Query qry( 0xf7e4a57f /* CHECKPOINT */, GetClient() );
		GoalManager::GetInstance()->GetGoals( qry );
		qry.GetBest( mMapGoal );

		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void TakeCheckPoint::Enter()
	{
		mTargetPosition = mMapGoal->GetWorldBounds().Center;

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run, true ) );

		Tracker.InProgress = mMapGoal;
	}

	void TakeCheckPoint::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();
		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus TakeCheckPoint::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			mTargetPosition.Z() = GetClient()->GetPosition().Z();
			GetClient()->GetSteeringSystem()->SetTarget( mTargetPosition, 32.f );
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	PlantMine::PlantMine()
		: StateChild( "PlantMine" )
		, FollowPathUser( "PlantMine" )
	{
		LimitToClass().SetFlag( ETQW_CLASS_ENGINEER );
		SetAlwaysRecieveEvents( true );
	}

	void PlantMine::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
		}
	}

	// FollowPathUser functions.
	bool PlantMine::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool PlantMine::GetAimPosition( Vector3f &_aimpos )
	{
		if ( !mLandMineEntity.IsValid() )
			_aimpos = mTargetPosition;
		else
			_aimpos = mLandMinePosition;
		return true;
	}

	void PlantMine::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws )
		{
			if ( mLandMineEntity.IsValid() && ws->CurrentWeaponIs( ETQW_WP_PLIERS ) )
				ws->FireWeapon();
			else if ( !mLandMineEntity.IsValid() && ws->CurrentWeaponIs( ETQW_WP_LANDMINE ) )
				ws->FireWeapon();
		}
	}

	float PlantMine::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		if ( InterfaceFuncs::IsWeaponCharged( GetClient(), ETQW_WP_LANDMINE, Primary ) )
		{
			GoalManager::Query qry( 0xf2dffa59 /* PLANTMINE */, GetClient() );
			GoalManager::GetInstance()->GetGoals( qry );
			qry.GetBest( mMapGoal );
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void PlantMine::Enter()
	{
		// generate a random position in the goal radius
		float fRandDistance = Mathf::IntervalRandom( 0.0f, mMapGoal->GetRadius() );
		Quaternionf quat( Vector3f::UNIT_Y, Mathf::DegToRad( Mathf::IntervalRandom( 0.0f, 360.0f ) ) );
		mTargetPosition = mMapGoal->GetPosition() + quat.Rotate( Vector3f::UNIT_Y * fRandDistance );

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run, true ) );

		Tracker.InProgress = mMapGoal;
	}

	void PlantMine::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

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

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		// If it's not destroyable, consider it a success.
		if ( !InterfaceFuncs::IsDestroyable( GetClient(), mMapGoal->GetEntity() ) )
		{
			return State_Finished;
		}

		if ( mLandMineEntity.IsValid() && !IGame::IsEntityValid( mLandMineEntity ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			GetClient()->ResetStuckTime();

			static float MIN_DIST_TO_TARGETPOS = 32.0f;
			static float MIN_DIST_TO_MINE = 32.0f;

			// Have we already thrown out a mine?
			if ( mLandMineEntity.IsValid() )
			{
				// Is it armed yet?
				if ( InterfaceFuncs::GetExplosiveState( GetClient(), mLandMineEntity ) == XPLO_ARMED )
					return State_Finished;

				// Disable avoidance for this frame.
				//.mClient->GetSteeringSystem()->SetNoAvoidTime(IGame::GetTime());

				// Not armed yet, keep trying.
				if ( EngineFuncs::EntityPosition( mLandMineEntity, mLandMinePosition ) &&
					EngineFuncs::EntityVelocity( mLandMineEntity, mLandMineVelocity ) )
				{
					GetClient()->PressButton( BOT_BUTTON_CROUCH );

					FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_PLIERS ) );

					// Do we need to get closer?
					bool bCloseEnough = ( GetClient()->GetPosition() - mLandMinePosition ).Length() < MIN_DIST_TO_MINE;
					GetClient()->GetSteeringSystem()->SetTarget( bCloseEnough ? GetClient()->GetPosition() : mLandMinePosition );
				}
				return State_Busy;
			}

			// Move closer if necessary.
			bool bCloseEnough = ( GetClient()->GetPosition() - mTargetPosition ).Length() < MIN_DIST_TO_TARGETPOS;
			GetClient()->GetSteeringSystem()->SetTarget( bCloseEnough ? GetClient()->GetPosition() : mTargetPosition );

			// keep watching the target position.
			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_LANDMINE ) );
		}
		return State_Busy;
	}

	void PlantMine::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
	{
		switch ( _message.GetMessageId() )
		{
			HANDLER( ACTION_WEAPON_FIRE )
			{
				const Event_WeaponFire *m = _message.Get<Event_WeaponFire>();
				if ( m->mWeaponId == ETQW_WP_LANDMINE && m->mProjectile.IsValid() )
				{
					mLandMineEntity = m->mProjectile;
				}
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	MobileMg42::MobileMg42()
		: StateChild( "MobileMg42" )
		, FollowPathUser( "MobileMg42" )
	{
		LimitToClass().SetFlag( ETQW_CLASS_SOLDIER );
	}

	void MobileMg42::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
		}
	}

	// FollowPathUser functions.
	bool MobileMg42::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool MobileMg42::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mMapGoal->GetPosition() + mMapGoal->GetFacing() * 1024.f;
		return true;
	}

	void MobileMg42::OnTarget()
	{
		//FINDSTATEIF(WeaponSystem, GetRootState(), AddWeaponRequest(1.f, GetNameHash(), ETQW_WP_MOBILE_MG42_SET));
	}

	float MobileMg42::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		if ( InterfaceFuncs::IsWeaponCharged( GetClient(), ETQW_WP_LANDMINE, Primary ) )
		{
			GoalManager::Query qry( 0xbe8488ed /* MOBILEMG42 */, GetClient() );
			GoalManager::GetInstance()->GetGoals( qry );
			qry.GetBest( mMapGoal );
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void MobileMg42::Enter()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run ) );

		Tracker.InProgress = mMapGoal;
	}

	void MobileMg42::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus MobileMg42::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			GetClient()->PressButton( BOT_BUTTON_PRONE );
			if ( GetClient()->HasEntityFlag( ENT_FLAG_PRONED ) )
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	ReviveTeammate::ReviveTeammate()
		: StateChild( "ReviveTeammate" )
		, FollowPathUser( "ReviveTeammate" )
	{
		LimitToClass().SetFlag( ETQW_CLASS_MEDIC );
	}

	void ReviveTeammate::GetDebugString( std::stringstream &out )
	{
		switch ( mGoalState )
		{
			case REVIVING:
				out << "Reviving ";
				break;
			case HEALING:
				out << "Healing ";
				break;
		}

		if ( mMapGoal && mMapGoal->GetEntity().IsValid() )
			out << EngineFuncs::EntityName( mMapGoal->GetEntity(), "<noname>" );
	}

	void ReviveTeammate::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetPosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::MAGENTA, 5.f );
		}
	}

	// FollowPathUser functions.
	bool ReviveTeammate::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool ReviveTeammate::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mMapGoal->GetWorldBounds().GetCenterBottom();
		return true;
	}

	void ReviveTeammate::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws )
		{
			if ( InterfaceFuncs::IsAlive( mMapGoal->GetEntity() ) )
			{
				if ( ws->CurrentWeaponIs( ETQW_WP_HEALTH ) )
					ws->FireWeapon();
			}
			else
			{
				if ( ws->CurrentWeaponIs( ETQW_WP_NEEDLE ) )
					ws->FireWeapon();
			}
		}
	}

	float ReviveTeammate::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		GoalManager::Query qry( 0x2086cdf0 /* REVIVE */, GetClient() );
		GoalManager::GetInstance()->GetGoals( qry );
		qry.GetBest( mMapGoal );

		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void ReviveTeammate::Enter()
	{
		mGoalState = REVIVING;

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run ) );

		Tracker.InProgress = mMapGoal;
	}

	void ReviveTeammate::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus ReviveTeammate::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		GameEntity reviveEnt = mMapGoal->GetEntity();

		if ( DidPathSucceed() )
		{
			Vector3f vEntPos;
			if ( !EngineFuncs::EntityPosition( reviveEnt, vEntPos ) )
				return State_Finished;

			FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
			GetClient()->GetSteeringSystem()->SetTarget( mMapGoal->GetPosition() );

			switch ( mGoalState )
			{
				case REVIVING:
				{
					if ( InterfaceFuncs::IsAlive( reviveEnt ) )
						mGoalState = HEALING;

					GetClient()->GetSteeringSystem()->SetNoAvoidTime( IGame::GetTime() + 1000 );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_NEEDLE ) );
					break;
				}
				case HEALING:
				{
					if ( GetClient()->GetTargetingSystem()->HasTarget() )
						return State_Finished;

					if ( !InterfaceFuncs::IsWeaponCharged( GetClient(), ETQW_WP_HEALTH, Primary ) )
						return State_Finished;

					EntityInfo entInfo;					
					if ( IGame::GetEntityInfo( reviveEnt, entInfo ) && entInfo.HealthPercent() >= 1.0f )
						return State_Finished;

					GetClient()->GetSteeringSystem()->SetNoAvoidTime( IGame::GetTime() + 1000 );
					FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_NEEDLE ) );
					break;
				}
			}
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////

	DefuseDynamite::DefuseDynamite()
		: StateChild( "DefuseDynamite" )
		, FollowPathUser( "DefuseDynamite" )
	{
		LimitToClass().SetFlag( ETQW_CLASS_ENGINEER );
	}

	void DefuseDynamite::RenderDebug()
	{
		if ( IsActive() )
		{
			RenderBuffer::AddOBB( mMapGoal->GetWorldBounds(), COLOR::ORANGE );
			RenderBuffer::AddLine( GetClient()->GetEyePosition(), mMapGoal->GetPosition(), COLOR::GREEN, 5.f );
		}
	}

	// FollowPathUser functions.
	bool DefuseDynamite::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		if ( mMapGoal && mMapGoal->RouteTo( GetClient(), _desination, 64.f ) )
			_final = false;
		else
			_final = true;
		return true;
	}

	// AimerUser functions.
	bool DefuseDynamite::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mMapGoal->GetPosition();
		return true;
	}

	void DefuseDynamite::OnTarget()
	{
		FINDSTATE( ws, WeaponSystem, GetRootState() );
		if ( ws && ws->CurrentWeaponIs( ETQW_WP_PLIERS ) )
			ws->FireWeapon();
	}

	float DefuseDynamite::GetPriority()
	{
		if ( IsActive() )
			return GetLastPriority();

		mMapGoal.reset();

		GoalManager::Query qry( 0x1899efc7 /* DEFUSE */, GetClient() );
		GoalManager::GetInstance()->GetGoals( qry );
		for ( uint32_t i = 0; i < qry.mList.size(); ++i )
		{
			if ( BlackboardIsDelayed( qry.mList[ i ]->GetSerialNum() ) )
				continue;

			if ( qry.mList[ i ]->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) < 1 )
				continue;

			if ( InterfaceFuncs::GetExplosiveState( GetClient(), qry.mList[ i ]->GetEntity() ) == XPLO_ARMED )
			{
				mMapGoal = qry.mList[ i ];
				break;
			}
			else
			{
				qry.mList[ i ]->SetDeleteMe( true );
			}
		}
		return mMapGoal ? mMapGoal->GetPriorityForClient( GetClient() ) : 0.f;
	}

	void DefuseDynamite::Enter()
	{
		mTargetPosition = mMapGoal->GetWorldBounds().Center;

		FINDSTATEIF( FollowPath, GetRootState(), Goto( this, Run ) );

		Tracker.InProgress = mMapGoal;
	}

	void DefuseDynamite::Exit()
	{
		FINDSTATEIF( FollowPath, GetRootState(), Stop( true ) );

		mMapGoal.reset();

		FINDSTATEIF( Aimer, GetRootState(), ReleaseAimRequest( GetNameHash() ) );
		FINDSTATEIF( WeaponSystem, GetRootState(), ReleaseWeaponRequest( GetNameHash() ) );

		Tracker.Reset();
	}

	State::StateStatus DefuseDynamite::Update( float fDt )
	{
		if ( DidPathFail() )
		{
			BlackboardDelay( 10.f, mMapGoal->GetSerialNum() );
			return State_Finished;
		}

		if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
			return State_Finished;

		if ( DidPathSucceed() )
		{
			ExplosiveState eState = InterfaceFuncs::GetExplosiveState( GetClient(), mMapGoal->GetEntity() );
			switch ( eState )
			{
				case XPLO_INVALID:
				case XPLO_UNARMED:
					return State_Finished;
				default:
					break; // keep processing
			}

			mTargetPosition = mMapGoal->GetWorldBounds().Center;
			float fDistanceToDynamite = ( mTargetPosition - GetClient()->GetPosition() ).SquaredLength();

			if ( fDistanceToDynamite > 2500.0f )
			{
				GetClient()->GetSteeringSystem()->SetTarget( mTargetPosition );
			}
			else
			{
				GetClient()->PressButton( BOT_BUTTON_CROUCH );
				FINDSTATEIF( Aimer, GetRootState(), AddAimRequest( Priority::Medium, this, GetNameHash() ) );
				FINDSTATEIF( WeaponSystem, GetRootState(), AddWeaponRequest( Priority::Medium, GetNameHash(), ETQW_WP_PLIERS ) );
			}
		}
		return State_Busy;
	}

	//////////////////////////////////////////////////////////////////////////
};