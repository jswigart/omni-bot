////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-15 00:12:40 -0500 (Fri, 15 Mar 2013) $
// $LastChangedRevision: 861 $
//
////////////////////////////////////////////////////////////////////////////////

#include "BotPathing.h"
#include "ScriptManager.h"
#include "BotSteeringSystem.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"
#include "IGameManager.h"
#include "InterfaceFuncs.h"
#include "PathPlannerBase.h"
#include "RenderBuffer.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	FollowPath::Query::Query()
		: mUser( 0 )
		, mMoveMode( Run )
		, mSkipLastPt( false )
		, mFinal( false )
	{
	}

	FollowPath::FollowPath()
		: StateChild( "FollowPath" )
		, mPathStatus( PathFinished )
		, mLookAheadPt( Vector3f::ZERO )
		, mPassThroughState( 0 )
		, mRayDistance( -1.0f )
		, mJumpTime( 0 )
		, mPathInterface( NULL )
		, mNumCachedCorners( 0 )
		, mActiveLinkIndex( 0 )
	{
	}

	FollowPath::~FollowPath()
	{
		delete mPathInterface;
	}

	void FollowPath::Initialize()
	{
		mPathInterface = System::mInstance->mNavigation->AllocPathInterface( GetClient() );
	}

	void FollowPath::GetDebugString( std::stringstream &out )
	{
		if ( mQuery.mUser && IsActive() )
			out << Utils::HashToString( mQuery.mUser->GetFollowUserName() );
		else
			out << "<none>";
	}

	void FollowPath::RenderDebug()
	{
		if ( mPathInterface )
			mPathInterface->Render();

		//RenderBuffer::AddLine(GetClient()->GetPosition(), mPtOnPath, COLOR::BLUE );
		//RenderBuffer::AddLine(GetClient()->GetEyePosition(), mLookAheadPt, COLOR::MAGENTA );
		/*.mCurrentPath.DebugRender(COLOR::RED);
		Path::PathPoint pt;
		mCurrentPath.GetCurrentPt(pt);
		RenderBuffer::AddCircle( pt.mPt, pt.mRadius, COLOR::GREEN );*/
	}

	bool FollowPath::IsOnCustomLink() const
	{
		if ( mCachedCorners[ 0 ].mFlags & NAVFLAGS_PATH_LINK )
			return true;
		return false;
	}

	bool FollowPath::IsOnCustomLink( NavArea type ) const
	{
		if ( mCachedCorners[ 0 ].mFlags & NAVFLAGS_PATH_LINK )
		{
			return mCachedCorners[ 0 ].mArea == type;
		}
		return false;
	}

	bool FollowPath::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mLookAheadPt;
		return true;
	}

	void FollowPath::OnTarget()
	{
	}

	void FollowPath::Enter()
	{
		mLookAheadPt = GetClient()->GetEyePosition() + GetClient()->GetFacingVector() * 512.f;
		FINDSTATEIF( Aimer, GetParent(), AddAimRequest( Priority::Idle, this, GetNameHash() ) );
	}

	void FollowPath::Exit()
	{
		Stop();

		FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( GetClient()->GetPosition() ) );
		FINDSTATEIF( Aimer, GetParent(), ReleaseAimRequest( GetNameHash() ) );
	}

	void FollowPath::Stop( bool _clearuser )
	{
		// _clearuser is true if HighLevel goal was aborted
		if ( mPassThroughState && _clearuser )
		{
			if ( mQuery.mUser && mQuery.mUser->GetFollowUserName() == mPassThroughState )
			{
				// don't interrupt active paththrough
				if ( mSavedQuery.mUser )
				{
					if ( mSavedQuery.mUser != mQuery.mUser )
						mSavedQuery.mUser->OnPathFailed( FollowPathUser::Interrupted );
					mSavedQuery.mUser = 0;
				}
				return;
			}
		}

		if ( mPathStatus == PathInProgress )
		{
			NotifyUserFailed( FollowPathUser::Interrupted );
		}

		mPathStatus = PathFinished;

		mPathInterface->Cancel();

		if ( _clearuser )
			ClearUser();
	}

	void FollowPath::ClearUser()
	{
		mQuery.mUser = 0;
	}

	void FollowPath::CancelPathThrough()
	{
		// always want to do this, if state active or not
		if ( mPassThroughState )
		{
			FINDSTATE( ll, LowLevel, GetRootState() );
			if ( ll != NULL )
			{
				State *pPathThrough = ll->FindState( mPassThroughState );
				if ( pPathThrough )
				{
					pPathThrough->EndPathThrough();
				}
			}
			mPassThroughState = 0;
		}
	}

	void FollowPath::SaveQuery()
	{
		mSavedQuery = mQuery;
	}

	void FollowPath::RestoreQuery()
	{
		mQuery = mSavedQuery;
	}

	void FollowPath::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
	{
		/*switch(_message.GetMessageId())
		{
		HANDLER(MESSAGE_DYNAMIC_PATHS_CHANGED)
		{
		using namespace AiState;
		const Event_DynamicPathsChanged *m = _message.Get<Event_DynamicPathsChanged>();
		if(m != NULL && (m->mTeamMask & 1<<GetClient()->GetTeam()))
		{
		DynamicPathUpdated(m);
		}
		break;
		}
		};*/
	}

	void FollowPath::DynamicPathUpdated( const Event_DynamicPathsChanged *_m )
	{
		/*Path::PathPoint p;
		for(int i = mCurrentPath.GetCurrentPtIndex(); i < mCurrentPath.GetNumPts(); ++i)
		{
		mCurrentPath.GetPt(i,p);

		if(_m->mNavId)
		{
		if(p.mNavId==_m->mNavId)
		{
		Repath();
		break;
		}
		}
		else if(p.mNavFlags&F_NAV_DYNAMIC)
		{
		Repath();
		break;
		}
		}*/
	}

	bool FollowPath::GotoRandomPt( FollowPathUser *_owner, MoveMode _movemode /*= Run*/ )
	{
		mPathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		mPathInterface->UpdateGoalPositionRandom();
		mPathInterface->UpdatePath();

		const PathInterface::PathStatus ps = mPathInterface->GetPathStatus();
		switch ( ps )
		{
			case PathInterface::PATH_VALID:
			case PathInterface::PATH_SEARCHING:
			{
				GetClient()->ResetStuckTime();
				mPathStatus = PathInProgress;
				return true;
			}
			case PathInterface::PATH_NOPATHTOGOAL:
			case PathInterface::PATH_NONE:
			{
				mPathStatus = PathNotFound;
				NotifyUserFailed( FollowPathUser::NoPath );
				if ( !mPassThroughState )
					mQuery.mUser = 0;
			}
		}
		return false;
	}

	bool FollowPath::Goto( FollowPathUser *_owner, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/ )
	{
		bool bFinalDest = true;
		if ( !_owner ) return false;

		DestinationVector destlist;
		if ( _owner->GetNextDestination( destlist, bFinalDest, _skiplastpt ) )
		{
			return Goto( _owner, destlist, _movemode, _skiplastpt, bFinalDest );
		}

		if ( _owner == mQuery.mUser )
		{
			mPathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			mQuery.mUser = 0;
		}
		else
		{
			_owner->OnPathFailed( FollowPathUser::NoPath );
		}
		return false;
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const Vector3f &_pos, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		destlist.push_back( Destination( _pos, _radius ) );
		return Goto( _owner, destlist, _movemode, _skiplastpt );
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const Vector3List &_goals, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		for ( uint32_t i = 0; i < _goals.size(); ++i )
			destlist.push_back( Destination( _goals[ i ], _radius ) );
		return Goto( _owner, destlist, _movemode, _skiplastpt );
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const MapGoalList &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		for ( uint32_t i = 0; i < _goals.size(); ++i )
			destlist.push_back( Destination( _goals[ i ]->GetPosition(), _goals[ i ]->GetRadius() ) );
		return Goto( _owner, destlist, _movemode, _skiplastpt );
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const DestinationVector &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/, bool _final /*= true*/ )
	{
		if ( mPassThroughState && _owner != mQuery.mUser )
		{
			if ( _owner->GetFollowUserName() == mPassThroughState )
			{
				// paththrough called Goto,
				// save HighLevel goal's query
				OBASSERT( !mSavedQuery.mUser, "mSavedQuery overwritten" );
				SaveQuery();
			}
			else if ( mQuery.mUser && mQuery.mUser->GetFollowUserName() == mPassThroughState )
			{
				if ( mSavedQuery.mUser && _owner != mSavedQuery.mUser )
					mSavedQuery.mUser->OnPathFailed( FollowPathUser::Interrupted );

				// remember current query and don't interrupt active paththrough
				mSavedQuery.mUser = _owner;
				mSavedQuery.mDestination = _goals;
				mSavedQuery.mMoveMode = _movemode;
				mSavedQuery.mSkipLastPt = _skiplastpt;
				mSavedQuery.mFinal = _final;
				return true;
			}
		}

		mQuery.mUser = _owner;
		mQuery.mDestination = _goals;
		mQuery.mMoveMode = _movemode;
		mQuery.mSkipLastPt = _skiplastpt;
		mQuery.mFinal = _final;
		return Repath();
	}

	bool FollowPath::Repath()
	{
		if ( !mQuery.mUser )
			return false;
		mQuery.mUser->ResetPathUser();
		mPathThroughPtIndex = -1;

		mPathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		mPathInterface->UpdateGoalPositions( mQuery.mDestination );
		mPathInterface->UpdatePath();

		if ( mPathInterface->GetPathStatus() == PathInterface::PATH_VALID )
		{
			GetClient()->ResetStuckTime();
			mPathStatus = PathInProgress;
		}
		else
		{
			mPathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			if ( !mPassThroughState )
				mQuery.mUser = 0;
		}
		/*.mQuery.mUser->mDestinationIndex =
			System::mInstance->mNavigation->PlanPathToNearest(
			GetClient(),
			GetClient()->GetPosition(),
			mQuery.mDestination,
			GetClient()->GetTeamFlag() );

			if( System::mInstance->mNavigation->FoundGoal() )
			{
			mCurrentPath.Clear();
			System::mInstance->mNavigation->GetPath(.mCurrentPath);
			if(!mQuery.mSkipLastPt)
			{
			Destination dest = mQuery.mDestination[.mQuery.mUser->mDestinationIndex];
			mCurrentPath.AddPt(dest.mPosition, dest.mRadius);
			}
			GetClient()->ResetStuckTime();
			mPathStatus = PathInProgress;
			}
			else
			{
			mPathStatus = PathNotFound;
			NotifyUserFailed(FollowPathUser::NoPath);
			if(!mPassThroughState) mQuery.mUser = 0;
			}*/
		return mPathStatus < PathFinished;
	}

	bool FollowPath::IsMoving() const
	{
		return mPathInterface->GetPathStatus() == PathInterface::PATH_VALID;
	}

	void FollowPath::NotifyUserSuccess()
	{
		if ( mQuery.mUser )
			mQuery.mUser->OnPathSucceeded();
	}

	void FollowPath::NotifyUserFailed( FollowPathUser::FailType _how )
	{
		if ( mQuery.mUser )
			mQuery.mUser->OnPathFailed( _how );

		if ( GetClient()->IsDebugEnabled( BOT_DEBUG_LOG_FAILED_PATHS ) )
		{
			const char *FailType = 0;
			switch ( _how )
			{
				case FollowPathUser::Blocked:
					FailType = "Blocked";
					break;
				case FollowPathUser::NoPath:
					FailType = "Interrupted";
					break;
				case FollowPathUser::None:
				case FollowPathUser::Interrupted:
				default:
					break;
			}

			/*if(FailType)
			{
			File f;
			f.OpenForWrite(va("user/failedpaths.txt"), File::Text, true);
			if(f.IsOpen())
			{
			Vector3f Position = GetClient()->GetPosition();
			Vector3f Dest = Vector3f::ZERO;

			Path::PathPoint pt;
			if(.mCurrentPath.GetCurrentPt(pt))
			Dest = pt.mPt;

			f.WriteString("{");
			f.WriteNewLine();

			f.Printf("\tType = \"%s\",",FailType); f.WriteNewLine();
			f.Printf("\tP = Vector3(%f,%f,%f),",Position.X(),Position.Y(),Position.Z());
			f.WriteNewLine();

			if(_how == FollowPathUser::NoPath)
			{
			f.WriteString("\tDest = {"); f.WriteNewLine();
			for(uint32_t i = 0; i < mQuery.mDestination.size(); ++i)
			{
			f.Printf("\t\tVector3(%f,%f,%f),",
			mQuery.mDestination[i].mPosition.X(),
			mQuery.mDestination[i].mPosition.Y(),
			mQuery.mDestination[i].mPosition.Z());
			f.WriteNewLine();
			}
			f.WriteString("\t},"); f.WriteNewLine();
			}
			else
			{
			f.Printf("\tDest = Vector3(%f,%f,%f),",
			pt.mPt.X(),pt.mPt.Y(),pt.mPt.Z());
			f.WriteNewLine();
			}

			f.WriteString("},"); f.WriteNewLine();
			f.Close();
			}
			}*/
		}
	}

	float FollowPath::GetPriority()
	{
		// always want to do this, if state active or not
		//if(.mPassThroughState)
		//{
		//	FINDSTATE(ll,LowLevel,GetRootState());
		//	if ( ll ) {
		//		State *pPathThrough = ll->FindState(.mPassThroughState);
		//		if(!pPathThrough || (!pPathThrough->IsActive() && pPathThrough->GetLastPriority()<Mathf::EPSILON))
		//		{
		//			// paththrough state finished
		//			if(.mQuery.mUser && mQuery.mUser->GetFollowUserName() == mPassThroughState)
		//			{
		//			 mPassThroughState = 0;

		//				if(.mSavedQuery.mUser)
		//				{
		//					// find new path to the current HighLevel goal
		//					RestoreQuery();
		//					Repath();
		//				}
		//				else
		//				{
		//					Stop(false);
		//				 mQuery.mUser = 0;
		//				}
		//			}
		//			else
		//			{
		//			 mPassThroughState = 0;

		//				if(.mCurrentPath.GetCurrentPtIndex() != mPathThroughPtIndex + 1)
		//				{
		//					// repath if the previous waypoint has paththrough which has been skipped
		//					Path::PathPoint ptPrev;
		//					if(.mCurrentPath.GetPreviousPt(ptPrev) && ptPrev.mOnPathThrough && ptPrev.mOnPathThroughParam)
		//					{
		//						Repath();
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
		return mPathStatus < PathFinished ? ( float )1.0 : ( float )0.0;
	}

	State::StateStatus FollowPath::Update( float fDt )
	{
		rmt_ScopedCPUSample( FollowPathUpdate );
		{
			const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();

			if ( mPathStatus == PathNotFound )
			{
				FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( bottomBounds ) );
				NotifyUserFailed( FollowPathUser::Blocked );
				mPathStatus = PathFinished;
				return State_Finished;
			}

			Vector3f vGotoTarget = bottomBounds;

			float ptRadius = 8.0f; // deprecate this?
			bool b3dMovement = false; // todo: trash this, handle in here rather than steering

			mPathInterface->UpdateSourcePosition( bottomBounds );
			mNumCachedCorners = mPathInterface->GetPathCorners( mCachedCorners, CachedEdges );
			if ( mNumCachedCorners > 0 )
			{
				const bool inWater = ( mCachedCorners[ 0 ].mFlags & NAVFLAGS_SWIM ) != 0;

				b3dMovement = inWater;

				// go to the next corner
				vGotoTarget = mCachedCorners[ 0 ].mPos;

				Vector3f toTarget = vGotoTarget - bottomBounds;
				const float distToCorner = toTarget.Normalize();
				
				if ( IsOnCustomLink() )
				{
					OffMeshConnection conn;
					mPathInterface->GetNavLink( mCachedCorners[ 0 ].mPolyId, conn );
					
					if ( mActiveLink.mPolyId != mCachedCorners[ 0 ].mPolyId )
					{
						mActiveLinkIndex = 0;
						mActiveLink = conn;
					}

					if ( mActiveLink.mPolyId )
					{
						if ( distToCorner < conn.mRadius )
						{
							if ( mActiveLinkIndex < mActiveLink.mVertices.size() )
							{
								++mActiveLinkIndex;
							}
							else
							{
								mPathInterface->CompleteNavLink( mCachedCorners[ 0 ].mPolyId );
							}
						}
					}
				}
				else
				{
					mActiveLink.mPolyId = 0;
					mActiveLinkIndex = 0;
				}
				
				// todo: remove this in favor of locomotion control behaviors running to handle edges?
				GetClient()->ProcessGotoNode( mCachedCorners, mNumCachedCorners );

				// look ahead in the path
				float lookAheadDistance = 128.0;

				// start with the current position
				Vector3f lastPos = bottomBounds;

				for ( size_t i = 0; i < mNumCachedCorners && lookAheadDistance > 0.0f; ++i )
				{
					const PathInterface::PathCorner & c0 = mCachedCorners[ i ];

					Vector3f edgeDir = c0.mPos - lastPos;
					const float edgeLen = edgeDir.Normalize();

					if ( edgeLen > lookAheadDistance )
					{
						mLookAheadPt = lastPos + edgeDir * lookAheadDistance;
						break;
					}
					else
					{
						mLookAheadPt = c0.mPos;
					}
					lookAheadDistance -= edgeLen;
					lastPos = c0.mPos;
				}

				mLookAheadPt += GetClient()->GetEyeGroundOffset();

				//if ( inWater )
				//{
				//	const Box3f & worldBounds = GetClient()->GetWorldBounds();

				//	Vector3f vGotoBot = vGotoTarget;
				//	Vector3f vGotoTop = vGotoTarget;
				//	vGotoTop.Z() += mCachedEdges[ 0 ].mWaterDepth;

				//	/*const float dVertTop = (vGotoTarget - worldBounds.GetCenterTop()).Z();
				//	const float dVertBottom = (vGotoTarget - worldBounds.GetCenterBottom()).Z();*/

				//	// use jump and crouch for moving up/down
				//	if ( worldBounds.GetCenterTop().Z() > vGotoTop.Z() )
				//		GetClient()->PressButton( BOT_BUTTON_CROUCH );
				//	else if ( worldBounds.GetCenterBottom().Z() < vGotoBot.Z() )
				//		GetClient()->PressButton( BOT_BUTTON_JUMP );
				//}

				if ( mCachedCorners[ 0 ].mArea == NAVAREA_LADDER )
				{
					GetClient()->ReleaseButton( BOT_BUTTON_CROUCH );
					GetClient()->PressButton( BOT_BUTTON_JUMP );
				}
				
				if ( mPathInterface->GetCurrentAreaFlags() & NAVFLAGS_CROUCH )
					GetClient()->PressButton( BOT_BUTTON_CROUCH );
				else
				{
					if ( ( mCachedCorners[ 0 ].mFlags & NAVFLAGS_CROUCH ) && distToCorner < 128.0f )
						GetClient()->PressButton( BOT_BUTTON_CROUCH );
					else
						GetClient()->ReleaseButton( BOT_BUTTON_CROUCH );
				}
				
				if ( mCachedCorners[ 0 ].mArea == NAVAREA_DOOR && IGame::GetFrameNumber() & 3 )
					GetClient()->PressButton( BOT_BUTTON_USE );
				/*if ( mCachedCorners[ 0 ].mFlags & F_NAV_JUMPLOW )
					CheckForLowJumps( vGotoTarget );
					if ( mCachedCorners[ 0 ].mFlags & F_NAV_SNEAK )
					GetClient()->PressButton( BOT_BUTTON_WALK );*/
			}

			if ( GetClient()->GetStuckTime() > 2000 )
			{
				FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( bottomBounds ) );
				NotifyUserFailed( FollowPathUser::Blocked );
				mPathStatus = PathFinished;
				return State_Finished;
			}

			FINDSTATEIF( SteeringSystem, GetRootState(),
				SetTarget( vGotoTarget,
				ptRadius,
				mQuery.mMoveMode,
				b3dMovement ) );

			if ( mNumCachedCorners <= 1 && ( b3dMovement ? Length( bottomBounds, vGotoTarget ) : Length2d( bottomBounds, vGotoTarget ) ) < ptRadius )
			{
				NotifyUserSuccess();
				mPathStatus = PathFinished;
				return State_Finished;
			}
		}
		return State_Busy;
	}

	bool FollowPath::CheckForMover( const Vector3f &_pos )
	{
		const float dropHeight = GetClient()->GetWorldBounds().Extent[ 2 ];
		const Vector3f boundsCenter = GetClient()->GetWorldBounds().Center;
		const Vector3f dropPos = boundsCenter - Vector3f( 0.f, 0.f, dropHeight );

		const bool bMover = InterfaceFuncs::IsMoverAt( boundsCenter, dropPos );
		if ( DebugDrawingEnabled() )
		{
			RenderBuffer::AddLine( boundsCenter, dropPos, bMover ? COLOR::GREEN : COLOR::RED );
		}
		return bMover;
	}

	void FollowPath::CheckForLowJumps( const Vector3f &_destination )
	{
		rmt_ScopedCPUSample( CheckForLowJumps );

		// bot MUST NOT press jump button every frame, otherwise he jumps only once
		int32_t time = IGame::GetTime();
		if ( time - mJumpTime < 100 )
			return;

		// todo: should be game dependant
		const float fStepHeight = GetClient()->GetStepHeight();
		//const float fStepBoxWidth = 9.0f; // GAME DEP
		//const float fStepBoxHeight = 16.0f; // GAME DEP
		const float fStepRayLength = 48.0f; // GAME DEP

		// Calculate the vector we're wanting to move, this will be
		// the direction of the trace
		Vector3f vMoveVec = _destination - GetClient()->GetPosition();
		vMoveVec.Z() = 0.0f; // get rid of the vertical component
		vMoveVec.Normalize();

		// Get this entities bounding box to use for traces.
		const Vector3f boundsTop = GetClient()->GetWorldBounds().GetCenterTop();
		const Vector3f boundsBtm = GetClient()->GetWorldBounds().GetCenterBottom();

		Vector3f stepTop = boundsTop + Vector3f( 0.f, 0.f, fStepHeight );
		Vector3f stepBtm = boundsBtm + Vector3f( 0.f, 0.f, fStepHeight );

		stepTop += vMoveVec * fStepRayLength;
		stepBtm += vMoveVec * fStepRayLength;

		AABB traceBnds( Vector3f( -8.f, -8.f, 0.f ), Vector3f( 8.f, 8.f, 8.f ) );

		// Trace a line start to end to see if it hits anything.
		obTraceResult tr;
		EngineFuncs::TraceLine( tr, stepTop, stepBtm,
			&traceBnds, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False );

		bool bHit = false;
		if ( tr.mFraction != 1.0f )
		{
			bHit = true;
			mJumpTime = time;
			GetClient()->PressButton( BOT_BUTTON_JUMP );
		}

		if ( DebugDrawingEnabled() )
		{
			// Line Ray
			RenderBuffer::AddLine( stepTop, stepBtm, bHit ? COLOR::RED : COLOR::GREEN );
		}
	}

	void FollowPath::CheckForGapJumps( const Vector3f &_destination )
	{
		rmt_ScopedCPUSample( CheckForGapJumps );

		// Get this entities bounding box to use for traces.
		Box3f worldObb;
		EngineFuncs::EntityWorldOBB( GetClient()->GetGameEntity(), worldObb );

		//const Vector3f boundsTop = GetClient()->GetWorldBounds().GetCenterTop();
		const Vector3f boundsBtm = GetClient()->GetWorldBounds().GetCenterBottom();

		// Trace a line down from the bot.
		//const float fStartUpOffset = 5.0f;
		static const float fEndDownOffset = GetClient()->GetGameVar( Client::JumpGapOffset );

		Vector3f endPos = boundsBtm - Vector3f( 0.f, 0.f, fEndDownOffset );

		// Trace a line downward to see the distance below us.
		//DEBUG_ONLY(RenderBuffer::AddLine(vStartPos, vEndPos, COLOR::RED));

		// Trace a line directly underneath us to try and detect rapid drops in elevation.
		obTraceResult tr;
		EngineFuncs::TraceLine( tr, boundsBtm, endPos, NULL, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False );

		Vector3f trEnd( tr.mEndpos );
		if ( mRayDistance == -1.0f && tr.mFraction != 1.0f )
		{
			mRayDistance = ( boundsBtm - trEnd ).SquaredLength();
		}
		else if ( mRayDistance != -1.0f && tr.mFraction == 1.0f )
		{
			GetClient()->PressButton( BOT_BUTTON_JUMP );
		}
		else if ( tr.mFraction != 1.0f )
		{
			// See if there is any sudden drops. (+20% length in ray maybe?)
			float fNewDist = ( boundsBtm - trEnd ).SquaredLength();
			if ( fNewDist > ( mRayDistance + ( mRayDistance * 0.2f ) ) )
			{
				// Try to jump.
				GetClient()->PressButton( BOT_BUTTON_JUMP );
			}
			mRayDistance = fNewDist;
		}
	}
};
