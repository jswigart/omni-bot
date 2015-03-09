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

	FollowPath::FollowPath()
		: StateChild( "FollowPath" )
		, m_PathStatus( PathFinished )
		, m_LookAheadPt( Vector3f::ZERO )
		, m_PassThroughState( 0 )
		, m_RayDistance( -1.0f )
		, m_JumpTime( 0 )
		, m_PathInterface( NULL )
		, mNumCachedCorners( 0 )
	{
	}

	FollowPath::~FollowPath()
	{
		delete m_PathInterface;
	}

	void FollowPath::Initialize()
	{
		m_PathInterface = System::mInstance->mNavigation->AllocPathInterface( GetClient() );
	}

	void FollowPath::GetDebugString( std::stringstream &out )
	{
		if ( m_Query.m_User && IsActive() )
			out << Utils::HashToString( m_Query.m_User->GetFollowUserName() );
		else
			out << "<none>";
	}

	void FollowPath::RenderDebug()
	{
		if ( m_PathInterface )
			m_PathInterface->Render();

		//RenderBuffer::AddLine(GetClient()->GetPosition(), m_PtOnPath, COLOR::BLUE );
		//RenderBuffer::AddLine(GetClient()->GetEyePosition(), m_LookAheadPt, COLOR::MAGENTA );
		/*m_CurrentPath.DebugRender(COLOR::RED);
		Path::PathPoint pt;
		m_CurrentPath.GetCurrentPt(pt);
		RenderBuffer::AddCircle( pt.m_Pt, pt.m_Radius, COLOR::GREEN );*/
	}

	bool FollowPath::IsOnCustomLink( NavArea linkType ) const
	{
		// todo
		return false;
	}

	bool FollowPath::GetAimPosition( Vector3f &_aimpos )
	{
		_aimpos = m_LookAheadPt;
		return true;
	}

	void FollowPath::OnTarget()
	{
	}

	void FollowPath::Enter()
	{
		m_LookAheadPt = GetClient()->GetEyePosition() + GetClient()->GetFacingVector() * 512.f;
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
		if ( m_PassThroughState && _clearuser )
		{
			if ( m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState )
			{
				// don't interrupt active paththrough
				if ( m_SavedQuery.m_User )
				{
					if ( m_SavedQuery.m_User != m_Query.m_User )
						m_SavedQuery.m_User->OnPathFailed( FollowPathUser::Interrupted );
					m_SavedQuery.m_User = 0;
				}
				return;
			}
		}

		if ( m_PathStatus == PathInProgress )
		{
			NotifyUserFailed( FollowPathUser::Interrupted );
		}

		m_PathStatus = PathFinished;

		m_PathInterface->Cancel();

		if ( _clearuser )
			ClearUser();
	}

	void FollowPath::ClearUser()
	{
		m_Query.m_User = 0;
	}

	void FollowPath::CancelPathThrough()
	{
		// always want to do this, if state active or not
		if ( m_PassThroughState )
		{
			FINDSTATE( ll, LowLevel, GetRootState() );
			if ( ll != NULL )
			{
				State *pPathThrough = ll->FindState( m_PassThroughState );
				if ( pPathThrough )
				{
					pPathThrough->EndPathThrough();
				}
			}
			m_PassThroughState = 0;
		}
	}

	void FollowPath::SaveQuery()
	{
		m_SavedQuery = m_Query;
	}

	void FollowPath::RestoreQuery()
	{
		m_Query = m_SavedQuery;
	}

	void FollowPath::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
	{
		/*switch(_message.GetMessageId())
		{
		HANDLER(MESSAGE_DYNAMIC_PATHS_CHANGED)
		{
		using namespace AiState;
		const Event_DynamicPathsChanged *m = _message.Get<Event_DynamicPathsChanged>();
		if(m != NULL && (m->m_TeamMask & 1<<GetClient()->GetTeam()))
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
		for(int i = m_CurrentPath.GetCurrentPtIndex(); i < m_CurrentPath.GetNumPts(); ++i)
		{
		m_CurrentPath.GetPt(i,p);

		if(_m->m_NavId)
		{
		if(p.m_NavId==_m->m_NavId)
		{
		Repath();
		break;
		}
		}
		else if(p.m_NavFlags&F_NAV_DYNAMIC)
		{
		Repath();
		break;
		}
		}*/
	}

	bool FollowPath::GotoRandomPt( FollowPathUser *_owner, MoveMode _movemode /*= Run*/ )
	{
		m_PathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		m_PathInterface->UpdateGoalPositionRandom();
		m_PathInterface->UpdatePath();

		const PathInterface::PathStatus ps = m_PathInterface->GetPathStatus();
		switch ( ps )
		{
			case PathInterface::PATH_VALID:
			case PathInterface::PATH_SEARCHING:
			{
				GetClient()->ResetStuckTime();
				m_PathStatus = PathInProgress;
				return true;
			}
			case PathInterface::PATH_NOPATHTOGOAL:
			case PathInterface::PATH_NONE:
			{
				m_PathStatus = PathNotFound;
				NotifyUserFailed( FollowPathUser::NoPath );
				if ( !m_PassThroughState )
					m_Query.m_User = 0;
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

		if ( _owner == m_Query.m_User )
		{
			m_PathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			m_Query.m_User = 0;
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
		for ( obuint32 i = 0; i < _goals.size(); ++i )
			destlist.push_back( Destination( _goals[ i ], _radius ) );
		return Goto( _owner, destlist, _movemode, _skiplastpt );
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const MapGoalList &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/ )
	{
		DestinationVector destlist;
		for ( obuint32 i = 0; i < _goals.size(); ++i )
			destlist.push_back( Destination( _goals[ i ]->GetPosition(), _goals[ i ]->GetRadius() ) );
		return Goto( _owner, destlist, _movemode, _skiplastpt );
	}

	bool FollowPath::Goto( FollowPathUser *_owner, const DestinationVector &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/, bool _final /*= true*/ )
	{
		if ( m_PassThroughState && _owner != m_Query.m_User )
		{
			if ( _owner->GetFollowUserName() == m_PassThroughState )
			{
				// paththrough called Goto,
				// save HighLevel goal's query
				OBASSERT( !m_SavedQuery.m_User, "m_SavedQuery overwritten" );
				SaveQuery();
			}
			else if ( m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState )
			{
				if ( m_SavedQuery.m_User && _owner != m_SavedQuery.m_User )
					m_SavedQuery.m_User->OnPathFailed( FollowPathUser::Interrupted );

				// remember current query and don't interrupt active paththrough
				m_SavedQuery.m_User = _owner;
				m_SavedQuery.m_Destination = _goals;
				m_SavedQuery.m_MoveMode = _movemode;
				m_SavedQuery.m_SkipLastPt = _skiplastpt;
				m_SavedQuery.m_Final = _final;
				return true;
			}
		}

		m_Query.m_User = _owner;
		m_Query.m_Destination = _goals;
		m_Query.m_MoveMode = _movemode;
		m_Query.m_SkipLastPt = _skiplastpt;
		m_Query.m_Final = _final;
		return Repath();
	}

	bool FollowPath::Repath()
	{
		if ( !m_Query.m_User )
			return false;
		m_Query.m_User->ResetPathUser();
		m_PathThroughPtIndex = -1;

		m_PathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		m_PathInterface->UpdateGoalPositions( m_Query.m_Destination );
		m_PathInterface->UpdatePath();

		if ( m_PathInterface->GetPathStatus() == PathInterface::PATH_VALID )
		{
			GetClient()->ResetStuckTime();
			m_PathStatus = PathInProgress;
		}
		else
		{
			m_PathStatus = PathNotFound;
			NotifyUserFailed( FollowPathUser::NoPath );
			if ( !m_PassThroughState )
				m_Query.m_User = 0;
		}
		/*m_Query.m_User->m_DestinationIndex =
			System::mInstance->mNavigation->PlanPathToNearest(
			GetClient(),
			GetClient()->GetPosition(),
			m_Query.m_Destination,
			GetClient()->GetTeamFlag() );

			if( System::mInstance->mNavigation->FoundGoal() )
			{
			m_CurrentPath.Clear();
			System::mInstance->mNavigation->GetPath(m_CurrentPath);
			if(!m_Query.m_SkipLastPt)
			{
			Destination dest = m_Query.m_Destination[m_Query.m_User->m_DestinationIndex];
			m_CurrentPath.AddPt(dest.m_Position, dest.m_Radius);
			}
			GetClient()->ResetStuckTime();
			m_PathStatus = PathInProgress;
			}
			else
			{
			m_PathStatus = PathNotFound;
			NotifyUserFailed(FollowPathUser::NoPath);
			if(!m_PassThroughState) m_Query.m_User = 0;
			}*/
		return m_PathStatus < PathFinished;
	}

	bool FollowPath::IsMoving() const
	{
		return m_PathInterface->GetPathStatus() == PathInterface::PATH_VALID;
	}

	void FollowPath::NotifyUserSuccess()
	{
		if ( m_Query.m_User )
			m_Query.m_User->OnPathSucceeded();
	}

	void FollowPath::NotifyUserFailed( FollowPathUser::FailType _how )
	{
		if ( m_Query.m_User )
			m_Query.m_User->OnPathFailed( _how );

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
			if(m_CurrentPath.GetCurrentPt(pt))
			Dest = pt.m_Pt;

			f.WriteString("{");
			f.WriteNewLine();

			f.Printf("\tType = \"%s\",",FailType); f.WriteNewLine();
			f.Printf("\tP = Vector3(%f,%f,%f),",Position.X(),Position.Y(),Position.Z());
			f.WriteNewLine();

			if(_how == FollowPathUser::NoPath)
			{
			f.WriteString("\tDest = {"); f.WriteNewLine();
			for(obuint32 i = 0; i < m_Query.m_Destination.size(); ++i)
			{
			f.Printf("\t\tVector3(%f,%f,%f),",
			m_Query.m_Destination[i].m_Position.X(),
			m_Query.m_Destination[i].m_Position.Y(),
			m_Query.m_Destination[i].m_Position.Z());
			f.WriteNewLine();
			}
			f.WriteString("\t},"); f.WriteNewLine();
			}
			else
			{
			f.Printf("\tDest = Vector3(%f,%f,%f),",
			pt.m_Pt.X(),pt.m_Pt.Y(),pt.m_Pt.Z());
			f.WriteNewLine();
			}

			f.WriteString("},"); f.WriteNewLine();
			f.Close();
			}
			}*/
		}
	}

	obReal FollowPath::GetPriority()
	{
		// always want to do this, if state active or not
		//if(m_PassThroughState)
		//{
		//	FINDSTATE(ll,LowLevel,GetRootState());
		//	if ( ll ) {
		//		State *pPathThrough = ll->FindState(m_PassThroughState);
		//		if(!pPathThrough || (!pPathThrough->IsActive() && pPathThrough->GetLastPriority()<Mathf::EPSILON))
		//		{
		//			// paththrough state finished
		//			if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
		//			{
		//				m_PassThroughState = 0;

		//				if(m_SavedQuery.m_User)
		//				{
		//					// find new path to the current HighLevel goal
		//					RestoreQuery();
		//					Repath();
		//				}
		//				else
		//				{
		//					Stop(false);
		//					m_Query.m_User = 0;
		//				}
		//			}
		//			else
		//			{
		//				m_PassThroughState = 0;

		//				if(m_CurrentPath.GetCurrentPtIndex() != m_PathThroughPtIndex + 1)
		//				{
		//					// repath if the previous waypoint has paththrough which has been skipped
		//					Path::PathPoint ptPrev;
		//					if(m_CurrentPath.GetPreviousPt(ptPrev) && ptPrev.m_OnPathThrough && ptPrev.m_OnPathThroughParam)
		//					{
		//						Repath();
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
		return m_PathStatus < PathFinished ? ( obReal )1.0 : ( obReal )0.0;
	}

	State::StateStatus FollowPath::Update( float fDt )
	{
		Prof( FollowPath );
		{
			Prof( Update );

			const Vector3f bottomBounds = GetClient()->GetWorldBounds().GetCenterBottom();

			if ( m_PathStatus == PathNotFound )
			{
				FINDSTATEIF( SteeringSystem, GetRootState(), SetTarget( bottomBounds ) );
				NotifyUserFailed( FollowPathUser::Blocked );
				m_PathStatus = PathFinished;
				return State_Finished;
			}

			Vector3f vGotoTarget = bottomBounds;

			float ptRadius = 8.0f; // deprecate this?
			bool b3dMovement = false; // todo: trash this, handle in here rather than steering

			m_PathInterface->UpdateSourcePosition( bottomBounds );
			mNumCachedCorners = m_PathInterface->GetPathCorners( mCachedCorners, CachedEdges );
			if ( mNumCachedCorners > 0 )
			{
				const bool inWater = ( mCachedCorners[ 0 ].mFlags & NAVFLAGS_SWIM ) != 0;

				// todo: remove this in favor of locomotion control behaviors running to handle edges?
				GetClient()->ProcessGotoNode( mCachedCorners, mNumCachedCorners );

				b3dMovement = inWater;

				{
					// go to the next corner
					vGotoTarget = mCachedCorners[ 0 ].mPos;

					Vector3f toTarget = vGotoTarget - bottomBounds;
					/*const float distToCorner =*/ toTarget.Normalize();

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
							m_LookAheadPt = lastPos + edgeDir * lookAheadDistance;
							break;
						}
						else
						{
							m_LookAheadPt = c0.mPos;
						}
						lookAheadDistance -= edgeLen;
						lastPos = c0.mPos;
					}

					m_LookAheadPt += GetClient()->GetEyeGroundOffset();
				}

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
				m_PathStatus = PathFinished;
				return State_Finished;
			}

			FINDSTATEIF( SteeringSystem, GetRootState(),
				SetTarget( vGotoTarget,
				ptRadius,
				m_Query.m_MoveMode,
				b3dMovement ) );

			if ( mNumCachedCorners <= 1 && ( b3dMovement ? Length( bottomBounds, vGotoTarget ) : Length2d( bottomBounds, vGotoTarget ) ) < ptRadius )
			{
				NotifyUserSuccess();
				m_PathStatus = PathFinished;
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
		Prof( CheckForLowJumps );

		// bot MUST NOT press jump button every frame, otherwise he jumps only once
		obint32 time = IGame::GetTime();
		if ( time - m_JumpTime < 100 )
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
		if ( tr.m_Fraction != 1.0f )
		{
			bHit = true;
			m_JumpTime = time;
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
		Prof( CheckForGapJumps );

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

		Vector3f trEnd( tr.m_Endpos );
		if ( m_RayDistance == -1.0f && tr.m_Fraction != 1.0f )
		{
			m_RayDistance = ( boundsBtm - trEnd ).SquaredLength();
		}
		else if ( m_RayDistance != -1.0f && tr.m_Fraction == 1.0f )
		{
			GetClient()->PressButton( BOT_BUTTON_JUMP );
		}
		else if ( tr.m_Fraction != 1.0f )
		{
			// See if there is any sudden drops. (+20% length in ray maybe?)
			float fNewDist = ( boundsBtm - trEnd ).SquaredLength();
			if ( fNewDist > ( m_RayDistance + ( m_RayDistance * 0.2f ) ) )
			{
				// Try to jump.
				GetClient()->PressButton( BOT_BUTTON_JUMP );
			}
			m_RayDistance = fNewDist;
		}
	}
};
