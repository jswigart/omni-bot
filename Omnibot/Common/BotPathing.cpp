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
#include "PathPlannerBase.h"
#include "RenderBuffer.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	Navigator::Query::Query()
		: mUser( 0 )
		, mMoveMode( Run )
		, mSkipLastPt( false )
		, mFinal( false )
	{
	}

	Navigator::Navigator()
		: StateChild( "Navigator" )
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

	Navigator::~Navigator()
	{
		delete mPathInterface;
	}

	void Navigator::Initialize()
	{
		mPathInterface = System::mInstance->mNavigation->AllocPathInterface();

		NavFlags includeFlags = NAVFLAGS_WALK;
		NavFlags excludeFlags = NAVFLAGS_NONE;
		GetClient()->GetNavFlags( includeFlags, excludeFlags );
		mPathInterface->UpdateNavFlags( includeFlags, excludeFlags );
	}

	void Navigator::GetDebugString( std::stringstream &out )
	{
		if ( mQuery.mUser && IsActive() )
			out << Utils::HashToString( mQuery.mUser->GetFollowUserName() );
		else
			out << "<none>";
	}

	void Navigator::RenderDebug()
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

	size_t Navigator::GetNumCorners() const
	{
		return mNumCachedCorners;
	}

	const PathInterface::PathCorner& Navigator::GetCorner( size_t index ) const
	{
		return mCachedCorners[ index ];
	}

	bool Navigator::IsOnCustomLink() const
	{
		return mCachedCorners[ 0 ].mIsLink;
	}

	bool Navigator::IsOnCustomLink( NavAreaFlags type ) const
	{
		return mCachedCorners[ 0 ].mIsLink && ( mCachedCorners[ 0 ].mAreaMask & type );
	}

	bool Navigator::ApproachingArea( NavAreaFlags type, float lookahead ) const
	{
		const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();
		Vector3f lastPos = bottomBounds;

		for ( size_t i = 0; i < mNumCachedCorners && lookahead > 0.0f; ++i )
		{
			if ( ( mCachedCorners[ i ].mAreaMask & type ) != 0 )
				return true;

			Vector3f seg = mCachedCorners[ i ].mPos - lastPos;
			lastPos = mCachedCorners[ i ].mPos;
			const float d = seg.Normalize();
			lookahead -= d;
		}

		return false;
	}

	int Navigator::GetAreaEntitiesAlongPath( float lookahead, GameEntity entities [], size_t maxEntities ) const
	{
		size_t numEntitiesOut = 0;

		const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();
		Vector3f lastPos = bottomBounds;

		for ( size_t i = 0; i < mNumCachedCorners && lookahead > 0.0f; ++i )
		{
			Vector3f seg = mCachedCorners[ i ].mPos - lastPos;
			const float d = seg.Normalize();
			lookahead -= d;

			if ( lookahead < 0.0 )
				break;

			for ( int e = 0; e < PathInterface::PathCorner::MAX_ENTITIES; ++e )
			{
				if ( !mCachedCorners[ i ].mEntities[ e ].IsValid() )
					break;

				entities[ numEntitiesOut++ ] = mCachedCorners[ i ].mEntities[ e ];

				if ( numEntitiesOut >= maxEntities )
					return numEntitiesOut;
			}

			lastPos = mCachedCorners[ i ].mPos;
		}

		return numEntitiesOut;
	}

	size_t Navigator::GetAreaEntitiesInRadius( const Vector3f& pos, float radius, GameEntity entities [], size_t maxEntities )
	{
		return mPathInterface->FindAreaEntitiesInRadius( pos, radius, entities, maxEntities );
	}

	bool Navigator::NavTrace( PathInterface::NavTraceResult& result, const Vector3f& start, const Vector3f& end )
	{
		return mPathInterface->NavTrace( result, start, end );
	}

	bool Navigator::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = mLookAheadPt;
		return true;
	}

	void Navigator::OnTarget()
	{
	}

	void Navigator::Enter()
	{
		mLookAheadPt = GetClient()->GetEyePosition() + GetClient()->GetFacingVector() * 512.f;
		FINDSTATEIF( Aimer, GetParent(), AddAimRequest( Priority::Idle, this, GetNameHash() ) );
		GetClient()->ResetStuckTime();
	}

	void Navigator::Exit()
	{
		Stop();

		FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( GetClient()->GetPosition() ) );
		FINDSTATEIF( Aimer, GetParent(), ReleaseAimRequest( GetNameHash() ) );
	}

	void Navigator::Stop( bool _clearuser )
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

	void Navigator::ClearUser()
	{
		mQuery.mUser = 0;
	}

	void Navigator::CancelPathThrough()
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

	void Navigator::SaveQuery()
	{
		mSavedQuery = mQuery;
	}

	void Navigator::RestoreQuery()
	{
		mQuery = mSavedQuery;
	}

	void Navigator::ProcessEvent( const Message &_message, CallbackParameters &_cb )
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

	bool Navigator::GotoRandomPt( FollowPathUser* owner, MoveMode movemode /*= Run*/ )
	{
		NavFlags includeFlags = NAVFLAGS_WALK;
		NavFlags excludeFlags = NAVFLAGS_NONE;
		GetClient()->GetNavFlags( includeFlags, excludeFlags );
		mPathInterface->UpdateNavFlags( includeFlags, excludeFlags );
		mPathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		mPathInterface->UpdateGoalPositionRandom();
		mPathInterface->UpdatePath( false );

		const PathInterface::PathStatus ps = mPathInterface->GetPathStatus();
		switch ( ps )
		{
			case PathInterface::PATH_VALID:
			case PathInterface::PATH_SEARCHING:
			{
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

	bool Navigator::Goto( FollowPathUser *owner, MoveMode movemode /*= Run*/, bool skiplastpt /*= false*/ )
	{
		bool finalDest = true;
		if ( !owner ) return false;

		DestinationVector destlist;
		if ( owner->GetNextDestination( destlist, finalDest, skiplastpt ) )
		{
			return Goto( owner, destlist, movemode, skiplastpt, finalDest );
		}

		if ( owner == mQuery.mUser )
		{
			mPathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			mQuery.mUser = 0;
		}
		else
		{
			owner->OnPathFailed( FollowPathUser::NoPath );
		}
		return false;
	}

	bool Navigator::Goto( FollowPathUser *owner, const Vector3f &_pos, float _radius /*= 32.f*/, MoveMode movemode /*= Run*/, bool skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		destlist.push_back( Destination( _pos, _radius ) );
		return Goto( owner, destlist, movemode, skiplastpt );
	}

	bool Navigator::Goto( FollowPathUser *owner, const Vector3List &goals, float _radius /*= 32.f*/, MoveMode movemode /*= Run*/, bool skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		for ( uint32_t i = 0; i < goals.size(); ++i )
			destlist.push_back( Destination( goals[ i ], _radius ) );
		return Goto( owner, destlist, movemode, skiplastpt );
	}

	bool Navigator::Goto( FollowPathUser *owner, const MapGoalList &goals, MoveMode movemode /*= Run*/, bool skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		for ( uint32_t i = 0; i < goals.size(); ++i )
			destlist.push_back( Destination( goals[ i ]->GetPosition(), goals[ i ]->GetRadius() ) );
		return Goto( owner, destlist, movemode, skiplastpt );
	}

	bool Navigator::Goto( FollowPathUser *owner, const DestinationVector &goals, MoveMode movemode /*= Run*/, bool skiplastpt /*= false*/, bool final /*= true*/ )
	{
		if ( mPassThroughState && owner != mQuery.mUser )
		{
			if ( owner->GetFollowUserName() == mPassThroughState )
			{
				// paththrough called Goto,
				// save HighLevel goal's query
				SaveQuery();
			}
			else if ( mQuery.mUser && mQuery.mUser->GetFollowUserName() == mPassThroughState )
			{
				if ( mSavedQuery.mUser && owner != mSavedQuery.mUser )
					mSavedQuery.mUser->OnPathFailed( FollowPathUser::Interrupted );

				// remember current query and don't interrupt active paththrough
				mSavedQuery.mUser = owner;
				mSavedQuery.mDestination = goals;
				mSavedQuery.mMoveMode = movemode;
				mSavedQuery.mSkipLastPt = skiplastpt;
				mSavedQuery.mFinal = final;
				return true;
			}
		}

		mQuery.mUser = owner;
		mQuery.mDestination = goals;
		mQuery.mMoveMode = movemode;
		mQuery.mSkipLastPt = skiplastpt;
		mQuery.mFinal = final;
		return Repath();
	}

	bool Navigator::Repath()
	{
		if ( !mQuery.mUser )
			return false;

		mQuery.mUser->ResetPathUser();
		mPathThroughPtIndex = -1;

		NavFlags includeFlags = NAVFLAGS_WALK;
		NavFlags excludeFlags = NAVFLAGS_NONE;
		GetClient()->GetNavFlags( includeFlags, excludeFlags );
		mPathInterface->UpdateNavFlags( includeFlags, excludeFlags );
		mPathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		mPathInterface->UpdateGoalPositions( mQuery.mDestination );
		mPathInterface->UpdatePath( false );
		
		if ( mPathInterface->GetPathStatus() == PathInterface::PATH_VALID )
		{
			mPathStatus = PathInProgress;
		}
		else
		{
			mPathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			if ( !mPassThroughState )
				mQuery.mUser = 0;
		}
		return mPathStatus < PathFinished;
	}

	bool Navigator::IsMoving() const
	{
		return mPathInterface->GetPathStatus() == PathInterface::PATH_VALID;
	}

	void Navigator::NotifyUserSuccess()
	{
		if ( mQuery.mUser )
			mQuery.mUser->OnPathSucceeded();
	}

	void Navigator::NotifyUserFailed( FollowPathUser::FailType how )
	{
		if ( mQuery.mUser )
			mQuery.mUser->OnPathFailed( how );

		if ( GetClient()->IsDebugEnabled( BOT_DEBUG_LOG_FAILED_PATHS ) )
		{
			const char *FailType = 0;
			switch ( how )
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

		if(how == FollowPathUser::NoPath)
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

	float Navigator::GetPriority()
	{
		return 1.0;
	}

	State::StateStatus Navigator::Update( float fDt )
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

			Vector3f dstPos = bottomBounds;

			float dstRadius = 8.0f; // deprecate this?
			float pathDistance = 0.0f;
			bool b3dMovement = false; // todo: trash this, handle in here rather than steering

			NavFlags includeFlags = NAVFLAGS_WALK;
			NavFlags excludeFlags = NAVFLAGS_NONE;
			GetClient()->GetNavFlags( includeFlags, excludeFlags );
			mPathInterface->UpdateNavFlags( includeFlags, excludeFlags );
			mPathInterface->UpdateSourcePosition( bottomBounds );
			mNumCachedCorners = mPathInterface->GetPathCorners( mCachedCorners, CachedEdges );
			if ( mNumCachedCorners > 0 )
			{
				const bool inWater = ( mCachedCorners[ 0 ].mAreaMask & NAVFLAGS_WATER ) != 0;

				b3dMovement = inWater;

				// go to the next corner
				dstPos = mCachedCorners[ 0 ].mPos;

				Vector3f toTarget = dstPos - bottomBounds;
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

				// todo: remove this in favor of locomotion control behaviors running to handle edge types?
				GetClient()->ProcessGotoNode( mCachedCorners, mNumCachedCorners );

				// look ahead in the path
				float lookAheadDistance = 128.0;

				// start with the current position
				Vector3f lastPos = bottomBounds;

				for ( size_t i = 0; i < mNumCachedCorners; ++i )
				{
					const PathInterface::PathCorner & c0 = mCachedCorners[ i ];

					Vector3f edgeDir = c0.mPos - lastPos;
					const float edgeLen = edgeDir.Normalize();

					pathDistance += edgeLen;

					if ( lookAheadDistance > 0.0f )
					{
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
					}

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

				if ( mCachedCorners[ 0 ].mAreaMask & NAVFLAGS_LADDER )
				{
					GetClient()->ReleaseButton( BOT_BUTTON_CROUCH );
					GetClient()->PressButton( BOT_BUTTON_JUMP );
				}

				if ( mPathInterface->GetCurrentAreaFlags() & NAVFLAGS_CROUCH )
					GetClient()->PressButton( BOT_BUTTON_CROUCH );
				else
				{
					if ( ( mCachedCorners[ 0 ].mAreaMask & NAVFLAGS_CROUCH ) && distToCorner < 128.0f )
						GetClient()->PressButton( BOT_BUTTON_CROUCH );
					else
						GetClient()->ReleaseButton( BOT_BUTTON_CROUCH );
				}

				if ( mCachedCorners[ 0 ].mAreaMask == NAVFLAGS_DOOR && IGame::GetFrameNumber() & 3 )
					GetClient()->PressButton( BOT_BUTTON_USE );
				/*if ( mCachedCorners[ 0 ].mFlags & F_NAV_JUMPLOW )
					CheckForLowJumps( vGotoTarget );
					if ( mCachedCorners[ 0 ].mFlags & F_NAV_SNEAK )
					GetClient()->PressButton( BOT_BUTTON_WALK );*/
			}

			if ( GetClient()->GetStuckTime() > 1000 )
			{
				FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( bottomBounds ) );
				NotifyUserFailed( FollowPathUser::Blocked );
				mPathStatus = PathFinished;
				return State_Finished;
			}
			else if ( GetClient()->GetStuckTime() > 250 )
			{
				float wallDist;
				Vector3f wallPos, wallNormal;
				if ( mPathInterface->GetNearestWall( wallPos, wallNormal, wallDist ) )
				{
					Vector3f moveDir = dstPos - bottomBounds;
					moveDir.Flatten();
					moveDir.Normalize();
					moveDir += wallNormal;
					moveDir.Normalize();
					dstPos = bottomBounds + moveDir * 8.0f;
				}
			}

			FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( dstPos, dstRadius, pathDistance, mQuery.mMoveMode, b3dMovement ) );

			if ( mNumCachedCorners <= 1 && ( b3dMovement ? Length( bottomBounds, dstPos ) : Length2d( bottomBounds, dstPos ) ) < dstRadius )
			{
				NotifyUserSuccess();
				mPathStatus = PathFinished;
				return State_Finished;
			}
		}
		return State_Busy;
	}

	bool Navigator::CheckForMover( const Vector3f &_pos )
	{
		const float dropHeight = GetClient()->GetWorldBounds().Extent[ 2 ];
		const Vector3f boundsCenter = GetClient()->GetWorldBounds().Center;
		const Vector3f dropPos = boundsCenter - Vector3f( 0.f, 0.f, dropHeight );
		
		GameEntity entityMover;
		const bool bMover = gEngineFuncs->IsMoverAt( boundsCenter, dropPos, entityMover );
		if ( DebugDrawingEnabled() )
		{
			RenderBuffer::AddLine( boundsCenter, dropPos, bMover ? COLOR::GREEN : COLOR::RED );
		}
		return bMover;
	}

	void Navigator::CheckForLowJumps( const Vector3f &_destination )
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
			&traceBnds, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), false );

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

	void Navigator::CheckForGapJumps( const Vector3f &_destination )
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
		EngineFuncs::TraceLine( tr, boundsBtm, endPos, NULL, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), false );

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
