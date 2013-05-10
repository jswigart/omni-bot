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
#include "NavigationFlags.h"
#include "PathPlannerBase.h"
#include "RenderBuffer.h"

extern float g_fTopWaypointOffset;
extern float g_fBottomWaypointOffset;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////
#ifdef NEW_FOLLOW_PATH
	bool FollowPath::m_OldLadderStyle = false;

	FollowPath::FollowPath()
		: StateChild("FollowPath")
		, m_PathStatus(PathFinished)
		, m_LookAheadPt(Vector3f::ZERO)
		, m_PassThroughState(0)
		, m_RayDistance(-1.0f)
		, m_JumpTime(0)
		, m_PathInterface( NULL )
	{
		m_PathInterface = System::mInstance->mNavigation->AllocPathInterface();
	}

	FollowPath::~FollowPath()
	{
		delete m_PathInterface;
	}

	void FollowPath::GetDebugString(std::stringstream &out)
	{
		if(m_Query.m_User && IsActive())
			out << Utils::HashToString(m_Query.m_User->GetFollowUserName());
		else
			out << "<none>";
	}

	void FollowPath::RenderDebug()
	{
		m_PathInterface->Render();

		/*RenderBuffer::AddLine(GetClient()->GetPosition(), m_PtOnPath, COLOR::BLUE );
		RenderBuffer::AddLine(GetClient()->GetPosition(), m_LookAheadPt, COLOR::MAGENTA );
		m_CurrentPath.DebugRender(COLOR::RED);
		Path::PathPoint pt;
		m_CurrentPath.GetCurrentPt(pt);
		RenderBuffer::AddCircle( pt.m_Pt, pt.m_Radius, COLOR::GREEN );*/
	}

	bool FollowPath::IsNavFlagActive( const NavFlags & flags ) const
	{
		// todo
		return false;
	}

	bool FollowPath::GetAimPosition(Vector3f &_aimpos)
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
		FINDSTATEIF(Aimer, GetParent(), AddAimRequest(Priority::Idle, this, GetNameHash()));
	}

	void FollowPath::Exit()
	{
		Stop();

		FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(GetClient()->GetPosition()));
		FINDSTATEIF(Aimer, GetParent(), ReleaseAimRequest(GetNameHash()));
	}

	void FollowPath::Stop(bool _clearuser)
	{
		// _clearuser is true if HighLevel goal was aborted
		if(m_PassThroughState && _clearuser)
		{
			if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
			{
				// don't interrupt active paththrough
				if(m_SavedQuery.m_User)
				{
					if(m_SavedQuery.m_User!=m_Query.m_User)
						m_SavedQuery.m_User->OnPathFailed(FollowPathUser::Interrupted);
					m_SavedQuery.m_User = 0;
				}
				return;
			}
		}

		if(m_PathStatus == PathInProgress)
		{
			NotifyUserFailed(FollowPathUser::Interrupted);
		}

		m_PathStatus = PathFinished;

		m_PathInterface->Cancel();

		if(_clearuser)
			ClearUser();
	}

	void FollowPath::ClearUser()
	{
		m_Query.m_User = 0;
	}

	void FollowPath::CancelPathThrough()
	{
		// always want to do this, if state active or not
		if(m_PassThroughState)
		{
			FINDSTATE(ll,LowLevel,GetRootState());
			if ( ll != NULL ) {
				State *pPathThrough = ll->FindState(m_PassThroughState);
				if(pPathThrough)
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

	void FollowPath::ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb)
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

	void FollowPath::DynamicPathUpdated(const Event_DynamicPathsChanged *_m)
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

	bool FollowPath::GotoRandomPt(FollowPathUser *_owner, MoveMode _movemode /*= Run*/)
	{
		m_PathInterface->UpdateSourcePosition( GetClient()->GetPosition() );
		m_PathInterface->UpdateGoalPositionRandom();
		m_PathInterface->UpdatePath();
		return m_PathInterface->GetPathStatus() == PathInterface::PATH_VALID;
	}

	bool FollowPath::Goto(FollowPathUser *_owner, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		bool bFinalDest = true;
		if (!_owner) return false;

		DestinationVector destlist;
		if(_owner->GetNextDestination(destlist, bFinalDest, _skiplastpt))
		{
			return Goto(_owner, destlist, _movemode, _skiplastpt, bFinalDest);
		}

		if(_owner == m_Query.m_User)
		{
			m_PathStatus = PathNotFound;
			NotifyUserFailed(FollowPathUser::NoPath);
			m_Query.m_User = 0;
		}
		else
		{
			_owner->OnPathFailed(FollowPathUser::NoPath);
		}
		return false;
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const Vector3f &_pos, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		destlist.push_back(Destination(_pos,_radius));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const Vector3List &_goals, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		for(obuint32 i = 0; i < _goals.size(); ++i)
			destlist.push_back(Destination(_goals[i],_radius));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const MapGoalList &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		for(obuint32 i = 0; i < _goals.size(); ++i)
			destlist.push_back(Destination(_goals[i]->GetPosition(),_goals[i]->GetRadius()));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const DestinationVector &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/, bool _final /*= true*/)
	{
		if (m_PassThroughState && _owner != m_Query.m_User)
		{
			if(_owner->GetFollowUserName() == m_PassThroughState)
			{
				// paththrough called Goto,
				// save HighLevel goal's query
				OBASSERT(!m_SavedQuery.m_User, "m_SavedQuery overwritten");
				SaveQuery();
			}
			else if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
			{
				if (m_SavedQuery.m_User && _owner != m_SavedQuery.m_User)
					m_SavedQuery.m_User->OnPathFailed(FollowPathUser::Interrupted);

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
		if(!m_Query.m_User) return false;
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
			NotifyUserFailed(FollowPathUser::NoPath);
			if(!m_PassThroughState) 
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
		if(m_Query.m_User)
			m_Query.m_User->OnPathSucceeded();
	}

	void FollowPath::NotifyUserFailed(FollowPathUser::FailType _how)
	{
		if(m_Query.m_User)
			m_Query.m_User->OnPathFailed(_how);

		if(GetClient()->IsDebugEnabled(BOT_DEBUG_LOG_FAILED_PATHS))
		{
			const char *FailType = 0;
			switch(_how)
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
		return m_PathStatus < PathFinished ? (obReal)1.0 : (obReal)0.0;
	}
	
	State::StateStatus FollowPath::Update(float fDt)
	{
		Prof(FollowPath);
		{
			Prof(Update);

			float ptRadius = 4.0f; // deprecate this?
			bool b3dMovement = false; // todo: trash this, handle in here rather than steering

			const Vector3f vPos = GetClient()->GetPosition();
			Vector3f vGotoTarget = vPos;
			
			mNumCachedEdges = m_PathInterface->GetNextMoveEdges( mCachedEdges, CachedEdges );
			if ( mNumCachedEdges > 0 )
			{
				GetClient()->ProcessGotoNode( mCachedEdges, mNumCachedEdges );

				vGotoTarget = mCachedEdges[ 0 ].mDst;
				

				m_PathInterface->GetPointAlongPath( 128.f, m_LookAheadPt );
			}

			/*
			bool b3dDistanceCheck = (pt.m_NavFlags & F_NAV_ELEVATOR) || (pt.m_NavFlags & F_NAV_CLIMB);
			bool b3dMovement = b3dDistanceCheck;
			bool bConsumeWaypoint = true;

			const Vector3f vPos = GetClient()->GetPosition();
			Vector3f vGotoTarget = RawWpPos(pt);

			float fDistanceSq = 0.f;
			const float fDistSq = SquaredLength(vPos, vGotoTarget);
			const float fDistSq2d = SquaredLength2d(vPos, vGotoTarget);
			const float fWpRadiusSq = Mathf::Sqr(pt.m_Radius);

			if(b3dDistanceCheck)
				fDistanceSq = fDistSq;
			else
				fDistanceSq = fDistSq2d;

			// Handle traversal options.
			if(pt.m_NavFlags & F_NAV_DOOR && IGame::GetFrameNumber() & 3)
				GetClient()->PressButton(BOT_BUTTON_USE);
			if(pt.m_NavFlags & F_NAV_SNEAK)
				GetClient()->PressButton(BOT_BUTTON_WALK);
			if(pt.m_NavFlags & F_NAV_ELEVATOR)
			{
				if(!CheckForMover(pt.m_Pt) &&
					!CheckForMover(GetClient()->GetPosition()))
				{
					GetClient()->ResetStuckTime();

					bConsumeWaypoint = false;
					Path::PathPoint lastpt;
					if(m_CurrentPath.GetPreviousPt(lastpt))
					{
						if(lastpt.m_NavFlags & F_NAV_ELEVATOR)
							vGotoTarget = vPos;
						else
							vGotoTarget = lastpt.m_Pt;
					}
					else
						vGotoTarget = vPos;
				}
				else
				{
					// hack: let it succeed if we're in 2d radius and above the wp
					if(fDistSq2d < fWpRadiusSq && vPos.Z() > vGotoTarget.Z())
					{
						fDistanceSq = fWpRadiusSq;
					}
				}
			}
			if(pt.m_NavFlags & F_NAV_TELEPORT)
			{
				Path::PathPoint nextpt;
				if(m_CurrentPath.GetNextPt(nextpt))
				{
					if(nextpt.m_NavFlags & F_NAV_TELEPORT)
					{
						// have we teleported to the destination teleport?
						const float fDistSqTp = SquaredLength2d(vPos, nextpt.m_Pt);
						if(fDistSqTp < Mathf::Sqr(nextpt.m_Radius))
						{
							fDistanceSq = 0.f;
							bConsumeWaypoint = true;
						}
						else
							bConsumeWaypoint = false;
					}
				}
			}

			// Close enough?
			if(bConsumeWaypoint && fDistanceSq <= fWpRadiusSq)
			{
				//////////////////////////////////////////////////////////////////////////
				if(!m_PassThroughState && pt.m_OnPathThrough && pt.m_OnPathThroughParam)
				{
					FINDSTATE(ll,LowLevel,GetRootState());
					State *pPathThrough = ll->FindState(pt.m_OnPathThrough);
					if(pPathThrough)
					{
						std::string s = Utils::HashToString(pt.m_OnPathThroughParam);
						if(pPathThrough->OnPathThrough(s))
						{
							m_SavedQuery.m_User = 0;
							m_PassThroughState = pt.m_OnPathThrough;
							m_PathThroughPtIndex = m_CurrentPath.GetCurrentPtIndex();
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(m_CurrentPath.IsEndOfPath())
				{
					if(!m_Query.m_Final)
					{
						bool bSkipLast = false, bFinalDest = false;
						DestinationVector destlist;
						if(m_Query.m_User && m_Query.m_User->GetNextDestination(destlist, bFinalDest, bSkipLast))
						{
							if(Goto(m_Query.m_User, destlist, m_Query.m_MoveMode, bSkipLast, bFinalDest))
							{
								return State_Busy;
							}
						}
					}

					FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(vPos));
					NotifyUserSuccess();
					m_PathStatus = PathFinished;
					return State_Finished;
				}

				m_CurrentPath.NextPt();
				GetClient()->ResetStuckTime();
			}
			*/
			if(GetClient()->GetStuckTime() > 2000)
			{
				FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(vPos));
				NotifyUserFailed(FollowPathUser::Blocked);
				m_PathStatus = PathFinished;
				return State_Finished;
			}
			
			FINDSTATEIF(SteeringSystem, GetRootState(),
				SetTarget(vGotoTarget,
				ptRadius,
				m_Query.m_MoveMode,
				b3dMovement));

			//if(pt.m_NavFlags & F_NAV_UNDERWATER)
			//{
			//b3dMovement = true;
			//if(NeedsAir())
			//}
			/*
			if(pt.m_NavFlags & F_NAV_JUMP && (fDistanceSq <= Mathf::Sqr(pt.m_Radius)))
				GetClient()->PressButton(BOT_BUTTON_JUMP);
			if(pt.m_NavFlags & F_NAV_JUMPLOW)
				CheckForLowJumps(pt.m_Pt);
			if(pt.m_NavFlags & F_NAV_JUMPGAP)
				CheckForGapJumps(pt.m_Pt);
			if (pt.m_NavFlags & F_NAV_PRONE)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (pt.m_NavFlags & F_NAV_CROUCH)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);
			
			m_PtOnPath = m_CurrentPath.FindNearestPtOnPath(vPos, &m_LookAheadPt, 128.f);*/

			//////////////////////////////////////////////////////////////////////////
			//if(!m_OldLadderStyle)
			//{
			//	if(GetClient()->HasEntityFlag(ENT_FLAG_ONLADDER))
			//	{
			//		float fHeight = pt.m_Pt.Z() - vPos.Z();

			//		if(m_LadderDirection == 0)
			//		{
			//			if(fDistanceSq < fWpRadiusSq * 4)
			//			{
			//				// current waypoint has not been consumed yet
			//				Path::PathPoint nextpt;
			//				if(m_CurrentPath.GetNextPt(nextpt))
			//				{
			//					fHeight = nextpt.m_Pt.Z() - vPos.Z();
			//				}
			//			}
			//		}

			//		if(Mathf::FAbs(fHeight) > g_fTopWaypointOffset || m_LadderDirection == 0)
			//		{
			//			if (fHeight > 0) m_LadderDirection = 1;
			//			else m_LadderDirection = -1;
			//		}

			//		if( 2 * fHeight * fHeight > fDistSq2d //climb only if next waypoint is above or below player
			//			|| m_LadderDirection > 0 && fHeight > -g_fTopWaypointOffset
			//			|| m_LadderDirection < 0 && -fHeight > g_fTopWaypointOffset)
			//		{
			//			Vector3f vEye = GetClient()->GetEyePosition();
			//			Vector3f vLook = pt.m_Pt - vEye;
			//			float h, p, r, cl = Mathf::DegToRad(60.f);

			//			if(m_LadderDirection > 0)
			//			{
			//				GetClient()->PressButton(BOT_BUTTON_MOVEUP);
			//				vLook.Z() += g_fTopWaypointOffset;
			//				vLook.ToSpherical(h, p, r);
			//				p = ClampT(p,-cl,cl);
			//			}
			//			else
			//			{
			//				GetClient()->PressButton(BOT_BUTTON_MOVEDN);
			//				vLook.ToSpherical(h, p, r);
			//				p = cl;
			//			}

			//			vLook.FromSpherical(h, p, r);
			//			m_LookAheadPt = vEye + vLook;
			//			//RenderBuffer::AddLine(vEye,m_LookAheadPt, (m_LadderDirection > 0) ? COLOR::GREEN : COLOR::RED,3.f);
			//		}
			//	}
			//	else
			//	{
			//		m_LadderDirection = 0;
			//	}
			//}
			//////////////////////////////////////////////////////////////////////////

			// Ignore vertical look points unless it's significantly out of our bbox height.
			const Box3f &worldObb = GetClient()->GetWorldBounds();
			const float fTolerance = worldObb.Extent[2];
			if(Mathf::FAbs(m_LookAheadPt.Z() - worldObb.Center.Z()) < fTolerance)
			{
				m_LookAheadPt.Z() = GetClient()->GetEyePosition().Z();
			}
		}
		return State_Busy;
	}

	bool FollowPath::CheckForMover(const Vector3f &_pos)
	{
		const float fWpHeight = g_fTopWaypointOffset - g_fBottomWaypointOffset;

		// Use a ray the size of a waypoint offset down by half a waypoint height.

		Vector3f pos1 = _pos + Vector3f(0.f,0.f,g_fTopWaypointOffset);
		pos1.Z() -= (fWpHeight * 0.5f);
		Vector3f pos2 = pos1;
		pos2.Z() -= fWpHeight;

		const bool bMover = InterfaceFuncs::IsMoverAt(pos1,pos2);

		if( DebugDrawingEnabled() )
		{
			RenderBuffer::AddLine( pos1,pos2,bMover?COLOR::GREEN:COLOR::RED );
		}
		return bMover;
	}

	void FollowPath::CheckForLowJumps(const Vector3f &_destination)
	{
		Prof(CheckForLowJumps);

		// bot MUST NOT press jump button every frame, otherwise he jumps only once
		obint32 time = IGame::GetTime();
		if(time - m_JumpTime < 100) return;

		// todo: should be game dependant
		const float fStepHeight = GetClient()->GetStepHeight();
		const float fStepBoxWidth = 9.0f; // GAME DEP
		const float fStepBoxHeight = 16.0f; // GAME DEP
		const float fStepRayLength = 48.0f; // GAME DEP

		// Calculate the vector we're wanting to move, this will be
		// the direction of the trace
		Vector3f vMoveVec = _destination - GetClient()->GetPosition();
		vMoveVec.Z() = 0.0f; // get rid of the vertical component
		vMoveVec.Normalize();

		// Get this entities bounding box to use for traces.
		AABB worldAABB, localAABB;
		EngineFuncs::EntityWorldAABB(GetClient()->GetGameEntity(), worldAABB);

		// Calculate the local AABB
		const Vector3f &vPosition = GetClient()->GetPosition();

		// Adjust the AABB mins to the step height
		localAABB.m_Mins[2] = worldAABB.m_Mins[2] + fStepHeight - vPosition[2];
		// Adjust the AABB mins to the step height + fStepBoxHeight
		localAABB.m_Maxs[2] = localAABB.m_Mins[2] + fStepBoxHeight;
		// Adjust the mins and max for the width of the box
		localAABB.m_Mins[0] = localAABB.m_Mins[1] = -fStepBoxWidth;
		localAABB.m_Maxs[0] = localAABB.m_Maxs[1] = fStepBoxWidth;

		// Trace a line forward from the bot.
		Vector3f vStartPos;
		worldAABB.CenterPoint(vStartPos);

		// The end of the ray is out towards the move direction, fStepRayLength length
		Vector3f vEndPos(vStartPos + (vMoveVec * fStepRayLength));

		// Trace a line start to end to see if it hits anything.
		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStartPos, vEndPos,
			&localAABB, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False);

		bool bHit = false;
		if(tr.m_Fraction != 1.0f)
		{
			bHit = true;
			m_JumpTime = time;
			GetClient()->PressButton(BOT_BUTTON_JUMP);
		}

		if(DebugDrawingEnabled())
		{
			// Line Ray
			RenderBuffer::AddLine(vStartPos, vEndPos,
				bHit ? COLOR::RED : COLOR::GREEN );

			Vector3f vBottomStartLine(vStartPos), vBottomEndLine(vEndPos);
			vBottomStartLine.Z() += localAABB.m_Mins[2];
			vBottomEndLine.Z() += localAABB.m_Mins[2];
			RenderBuffer::AddLine(vBottomStartLine, vBottomEndLine,
				bHit ? COLOR::RED : COLOR::GREEN );

			Vector3f vTopStartLine(vStartPos), vTopEndLine(vEndPos);
			vTopStartLine.Z() += localAABB.m_Maxs[2];
			vTopEndLine.Z() += localAABB.m_Maxs[2];
			RenderBuffer::AddLine(vTopStartLine, vTopEndLine,
				bHit ? COLOR::RED : COLOR::GREEN );
		}
	}

	void FollowPath::CheckForGapJumps(const Vector3f &_destination)
	{
		Prof(CheckForGapJumps);

		// Get this entities bounding box to use for traces.
		AABB aabb;
		EngineFuncs::EntityWorldAABB(GetClient()->GetGameEntity(), aabb);

		// Trace a line down from the bot.
		//const float fStartUpOffset = 5.0f;
		static const float fEndDownOffset = GetClient()->GetGameVar(Client::JumpGapOffset);
		Vector3f vStartPos(Vector3f::ZERO);
		aabb.CenterPoint(vStartPos);

		Vector3f vEndPos(vStartPos);
		vEndPos.Z() -= fEndDownOffset;

		// Trace a line downward to see the distance below us.
		//DEBUG_ONLY(RenderBuffer::AddLine(vStartPos, vEndPos, COLOR::RED));

		// Trace a line directly underneath us to try and detect rapid drops in elevation.
		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStartPos, vEndPos, NULL,
			TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False);

		Vector3f trEnd(tr.m_Endpos);
		if(m_RayDistance == -1.0f && tr.m_Fraction != 1.0f)
		{
			m_RayDistance = (vStartPos - trEnd).SquaredLength();
		}
		else if(m_RayDistance != -1.0f && tr.m_Fraction == 1.0f)
		{
			GetClient()->PressButton(BOT_BUTTON_JUMP);
		}
		else if(tr.m_Fraction != 1.0f)
		{
			// See if there is any sudden drops. (+20% length in ray maybe?)
			float fNewDist = (vStartPos - trEnd).SquaredLength();
			if(fNewDist > (m_RayDistance + (m_RayDistance * 0.2f)))
			{
				// Try to jump.
				GetClient()->PressButton(BOT_BUTTON_JUMP);
			}
			m_RayDistance = fNewDist;
		}
	}
#else
bool FollowPath::m_OldLadderStyle = false;

	FollowPath::FollowPath()
		: StateChild("FollowPath")
		, m_PathStatus(PathFinished)
		, m_PtOnPath(Vector3f::ZERO)
		, m_LookAheadPt(Vector3f::ZERO)
		, m_PassThroughState(0)
		, m_RayDistance(-1.0f)
		, m_JumpTime(0)
	{
	}

	void FollowPath::GetDebugString(std::stringstream &out)
	{
		if(m_Query.m_User && IsActive())
			out << Utils::HashToString(m_Query.m_User->GetFollowUserName());
		else
			out << "<none>";
	}

	void FollowPath::RenderDebug()
	{
		RenderBuffer::AddLine(GetClient()->GetPosition(), m_PtOnPath, COLOR::BLUE );
		RenderBuffer::AddLine(GetClient()->GetPosition(), m_LookAheadPt, COLOR::MAGENTA );
		m_CurrentPath.DebugRender(COLOR::RED);
		Path::PathPoint pt;
		m_CurrentPath.GetCurrentPt(pt);
		RenderBuffer::AddCircle( pt.m_Pt, pt.m_Radius, COLOR::GREEN );
	}

	bool FollowPath::IsNavFlagActive( const NavFlags & flags ) const
	{
		Path::PathPoint pp;
		if(IsMoving() && GetCurrentPath().GetCurrentPt(pp) && (pp.m_NavFlags & flags ) )
			return true;

		// todo
		return false;
	}

	bool FollowPath::GetAimPosition(Vector3f &_aimpos)
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
		FINDSTATEIF(Aimer, GetParent(), AddAimRequest(Priority::Idle, this, GetNameHash()));
	}

	void FollowPath::Exit()
	{
		Stop();

		FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(GetClient()->GetPosition()));
		FINDSTATEIF(Aimer, GetParent(), ReleaseAimRequest(GetNameHash()));
	}

	void FollowPath::Stop(bool _clearuser)
	{
		// _clearuser is true if HighLevel goal was aborted
		if(m_PassThroughState && _clearuser)
		{
			if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
			{
				// don't interrupt active paththrough
				if(m_SavedQuery.m_User)
				{
					if(m_SavedQuery.m_User!=m_Query.m_User)
						m_SavedQuery.m_User->OnPathFailed(FollowPathUser::Interrupted);
					m_SavedQuery.m_User = 0;
				}
				return;
			}
		}

		if(m_PathStatus == PathInProgress)
		{
			NotifyUserFailed(FollowPathUser::Interrupted);
		}

		m_PathStatus = PathFinished;
		m_CurrentPath.Clear();

		if(_clearuser)
			ClearUser();
	}

	void FollowPath::ClearUser()
	{
		m_Query.m_User = 0;
	}

	void FollowPath::CancelPathThrough()
	{
		// always want to do this, if state active or not
		if(m_PassThroughState)
		{
			FINDSTATE(ll,LowLevel,GetRootState());
			if ( ll != NULL ) {
				State *pPathThrough = ll->FindState(m_PassThroughState);
				if(pPathThrough)
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

	void FollowPath::ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb)
	{
		switch(_message.GetMessageId())
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
		};
	}

	void FollowPath::DynamicPathUpdated(const Event_DynamicPathsChanged *_m)
	{
		Path::PathPoint p;
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
		}
	}

	bool FollowPath::GotoRandomPt(FollowPathUser *_owner, MoveMode _movemode /*= Run*/)
	{
		Vector3f vDestination = System::mInstance->mNavigation->GetRandomDestination(GetClient(),GetClient()->GetPosition(),GetClient()->GetTeamFlag());
		return Goto(_owner,vDestination);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		bool bFinalDest = true;
		if (!_owner) return false;

		DestinationVector destlist;
		if(_owner->GetNextDestination(destlist, bFinalDest, _skiplastpt))
		{
			return Goto(_owner, destlist, _movemode, _skiplastpt, bFinalDest);
		}

		if(_owner == m_Query.m_User)
		{
			m_PathStatus = PathNotFound;
			NotifyUserFailed(FollowPathUser::NoPath);
			m_Query.m_User = 0;
		}
		else
		{
			_owner->OnPathFailed(FollowPathUser::NoPath);
		}
		return false;
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const Vector3f &_pos, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		destlist.push_back(Destination(_pos,_radius));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const Vector3List &_goals, float _radius /*= 32.f*/, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		for(obuint32 i = 0; i < _goals.size(); ++i)
			destlist.push_back(Destination(_goals[i],_radius));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const MapGoalList &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/)
	{
		DestinationVector destlist;
		for(obuint32 i = 0; i < _goals.size(); ++i)
			destlist.push_back(Destination(_goals[i]->GetPosition(),_goals[i]->GetRadius()));
		return Goto(_owner, destlist, _movemode, _skiplastpt);
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const DestinationVector &_goals, MoveMode _movemode /*= Run*/, bool _skiplastpt /*= false*/, bool _final /*= true*/)
	{
		if (m_PassThroughState && _owner != m_Query.m_User)
		{
			if(_owner->GetFollowUserName() == m_PassThroughState)
			{
				// paththrough called Goto,
				// save HighLevel goal's query
				OBASSERT(!m_SavedQuery.m_User, "m_SavedQuery overwritten");
				SaveQuery();
			}
			else if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
			{
				if (m_SavedQuery.m_User && _owner != m_SavedQuery.m_User)
					m_SavedQuery.m_User->OnPathFailed(FollowPathUser::Interrupted);

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
		if(!m_Query.m_User) return false;
		m_Query.m_User->ResetPathUser();
		m_PathThroughPtIndex = -1;

		m_Query.m_User->m_DestinationIndex =
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
		}
		return m_PathStatus < PathFinished;
	}

	bool FollowPath::Goto(FollowPathUser *_owner, const Path &_path, MoveMode _movemode /*= Run*/)
	{
		if(!_owner)
			return false;
		//OBASSERT(_owner,"No User Defined!");

		m_Query.m_SkipLastPt = false;
		m_Query.m_Final = true;
		m_Query.m_User = _owner;
		m_Query.m_MoveMode = _movemode;
		m_Query.m_User->ResetPathUser();
		m_Query.m_Destination.resize(0);

		GetClient()->ResetStuckTime();
		m_CurrentPath = _path;
		m_PathStatus = PathInProgress;
		return true;
	}

	bool FollowPath::IsMoving() const
	{
		return !m_CurrentPath.IsEndOfPath();
	}

	void FollowPath::NotifyUserSuccess()
	{
		if(m_Query.m_User)
			m_Query.m_User->OnPathSucceeded();
	}

	void FollowPath::NotifyUserFailed(FollowPathUser::FailType _how)
	{
		if(m_Query.m_User)
			m_Query.m_User->OnPathFailed(_how);

		if(GetClient()->IsDebugEnabled(BOT_DEBUG_LOG_FAILED_PATHS))
		{
			const char *FailType = 0;
			switch(_how)
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

			if(FailType)
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
			}
		}
	}

	obReal FollowPath::GetPriority()
	{
		// always want to do this, if state active or not
		if(m_PassThroughState)
		{
			FINDSTATE(ll,LowLevel,GetRootState());
			if ( ll ) {
				State *pPathThrough = ll->FindState(m_PassThroughState);
				if(!pPathThrough || (!pPathThrough->IsActive() && pPathThrough->GetLastPriority()<Mathf::EPSILON))
				{
					// paththrough state finished
					if(m_Query.m_User && m_Query.m_User->GetFollowUserName() == m_PassThroughState)
					{
						m_PassThroughState = 0;

						if(m_SavedQuery.m_User)
						{
							// find new path to the current HighLevel goal
							RestoreQuery();
							Repath();
						}
						else
						{
							Stop(false);
							m_Query.m_User = 0;
						}
					}
					else
					{
						m_PassThroughState = 0;

						if(m_CurrentPath.GetCurrentPtIndex() != m_PathThroughPtIndex + 1)
						{
							// repath if the previous waypoint has paththrough which has been skipped
							Path::PathPoint ptPrev;
							if(m_CurrentPath.GetPreviousPt(ptPrev) && ptPrev.m_OnPathThrough && ptPrev.m_OnPathThroughParam)
							{
								Repath();
							}
						}
					}
				}
			}
		}
		return m_PathStatus < PathFinished ? (obReal)1.0 : (obReal)0.0;
	}

	Vector3f RawWpPos(const Path::PathPoint &pp)
	{
		const float fWpHeight = g_fTopWaypointOffset - g_fBottomWaypointOffset;

		Vector3f v = pp.m_Pt;
		v.Z() -= fWpHeight*0.5f;
		v.Z() -= g_fBottomWaypointOffset;
		return v;
	}

	State::StateStatus FollowPath::Update(float fDt)
	{
		Prof(FollowPath);
		{
			Prof(Update);

			Path::PathPoint pt;
			m_CurrentPath.GetCurrentPt(pt);

			GetClient()->ProcessGotoNode(m_CurrentPath);

			bool b3dDistanceCheck = (pt.m_NavFlags & F_NAV_ELEVATOR) || (pt.m_NavFlags & F_NAV_CLIMB);
			bool b3dMovement = b3dDistanceCheck;
			bool bConsumeWaypoint = true;

			const Vector3f vPos = GetClient()->GetPosition();
			Vector3f vGotoTarget = RawWpPos(pt);

			float fDistanceSq = 0.f;
			const float fDistSq = SquaredLength(vPos, vGotoTarget);
			const float fDistSq2d = SquaredLength2d(vPos, vGotoTarget);
			const float fWpRadiusSq = Mathf::Sqr(pt.m_Radius);

			if(b3dDistanceCheck)
				fDistanceSq = fDistSq;
			else
				fDistanceSq = fDistSq2d;

			// Handle traversal options.
			if(pt.m_NavFlags & F_NAV_DOOR && IGame::GetFrameNumber() & 3)
				GetClient()->PressButton(BOT_BUTTON_USE);
			if(pt.m_NavFlags & F_NAV_SNEAK)
				GetClient()->PressButton(BOT_BUTTON_WALK);
			if(pt.m_NavFlags & F_NAV_ELEVATOR)
			{
				if(!CheckForMover(pt.m_Pt) &&
					!CheckForMover(GetClient()->GetPosition()))
				{
					GetClient()->ResetStuckTime();

					bConsumeWaypoint = false;
					Path::PathPoint lastpt;
					if(m_CurrentPath.GetPreviousPt(lastpt))
					{
						if(lastpt.m_NavFlags & F_NAV_ELEVATOR)
							vGotoTarget = vPos;
						else
							vGotoTarget = lastpt.m_Pt;
					}
					else
						vGotoTarget = vPos;
				}
				else
				{
					// hack: let it succeed if we're in 2d radius and above the wp
					if(fDistSq2d < fWpRadiusSq && vPos.Z() > vGotoTarget.Z())
					{
						fDistanceSq = fWpRadiusSq;
					}
				}
			}
			if(pt.m_NavFlags & F_NAV_TELEPORT)
			{
				Path::PathPoint nextpt;
				if(m_CurrentPath.GetNextPt(nextpt))
				{
					if(nextpt.m_NavFlags & F_NAV_TELEPORT)
					{
						// have we teleported to the destination teleport?
						const float fDistSqTp = SquaredLength2d(vPos, nextpt.m_Pt);
						if(fDistSqTp < Mathf::Sqr(nextpt.m_Radius))
						{
							fDistanceSq = 0.f;
							bConsumeWaypoint = true;
						}
						else
							bConsumeWaypoint = false;
					}
				}
			}

			// Close enough?
			if(bConsumeWaypoint && fDistanceSq <= fWpRadiusSq)
			{
				//////////////////////////////////////////////////////////////////////////
				if(!m_PassThroughState && pt.m_OnPathThrough && pt.m_OnPathThroughParam)
				{
					FINDSTATE(ll,LowLevel,GetRootState());
					State *pPathThrough = ll->FindState(pt.m_OnPathThrough);
					if(pPathThrough)
					{
						std::string s = Utils::HashToString(pt.m_OnPathThroughParam);
						if(pPathThrough->OnPathThrough(s))
						{
							m_SavedQuery.m_User = 0;
							m_PassThroughState = pt.m_OnPathThrough;
							m_PathThroughPtIndex = m_CurrentPath.GetCurrentPtIndex();
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(m_CurrentPath.IsEndOfPath())
				{
					if(!m_Query.m_Final)
					{
						bool bSkipLast = false, bFinalDest = false;
						DestinationVector destlist;
						if(m_Query.m_User && m_Query.m_User->GetNextDestination(destlist, bFinalDest, bSkipLast))
						{
							if(Goto(m_Query.m_User, destlist, m_Query.m_MoveMode, bSkipLast, bFinalDest))
							{
								return State_Busy;
							}
						}
					}

					FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(vPos));
					NotifyUserSuccess();
					m_PathStatus = PathFinished;
					return State_Finished;
				}

				m_CurrentPath.NextPt();
				GetClient()->ResetStuckTime();
			}

			if(GetClient()->GetStuckTime() > 2000)
			{
				FINDSTATEIF(SteeringSystem, GetRootState(), SetTarget(vPos));
				NotifyUserFailed(FollowPathUser::Blocked);
				m_PathStatus = PathFinished;
				return State_Finished;
			}

			FINDSTATEIF(SteeringSystem, GetRootState(),
				SetTarget(vGotoTarget,
				pt.m_Radius,
				m_Query.m_MoveMode,
				b3dMovement));

			//if(pt.m_NavFlags & F_NAV_UNDERWATER)
			//{
			//b3dMovement = true;
			//if(NeedsAir())
			//}

			if(pt.m_NavFlags & F_NAV_JUMP && (fDistanceSq <= Mathf::Sqr(pt.m_Radius)))
				GetClient()->PressButton(BOT_BUTTON_JUMP);
			if(pt.m_NavFlags & F_NAV_JUMPLOW)
				CheckForLowJumps(pt.m_Pt);
			if(pt.m_NavFlags & F_NAV_JUMPGAP)
				CheckForGapJumps(pt.m_Pt);
			if (pt.m_NavFlags & F_NAV_PRONE)
				GetClient()->PressButton(BOT_BUTTON_PRONE);
			else if (pt.m_NavFlags & F_NAV_CROUCH)
				GetClient()->PressButton(BOT_BUTTON_CROUCH);

			m_PtOnPath = m_CurrentPath.FindNearestPtOnPath(vPos, &m_LookAheadPt, 128.f);

			//////////////////////////////////////////////////////////////////////////
			if(!m_OldLadderStyle)
			{
				if(GetClient()->HasEntityFlag(ENT_FLAG_ONLADDER))
				{
					float fHeight = pt.m_Pt.Z() - vPos.Z();

					if(m_LadderDirection == 0)
					{
						if(fDistanceSq < fWpRadiusSq * 4)
						{
							// current waypoint has not been consumed yet
							Path::PathPoint nextpt;
							if(m_CurrentPath.GetNextPt(nextpt))
							{
								fHeight = nextpt.m_Pt.Z() - vPos.Z();
							}
						}
					}

					if(Mathf::FAbs(fHeight) > g_fTopWaypointOffset || m_LadderDirection == 0)
					{
						if (fHeight > 0) m_LadderDirection = 1;
						else m_LadderDirection = -1;
					}

					if( 2 * fHeight * fHeight > fDistSq2d //climb only if next waypoint is above or below player
						|| m_LadderDirection > 0 && fHeight > -g_fTopWaypointOffset
						|| m_LadderDirection < 0 && -fHeight > g_fTopWaypointOffset)
					{
						Vector3f vEye = GetClient()->GetEyePosition();
						Vector3f vLook = pt.m_Pt - vEye;
						float h, p, r, cl = Mathf::DegToRad(60.f);

						if(m_LadderDirection > 0)
						{
							GetClient()->PressButton(BOT_BUTTON_MOVEUP);
							vLook.Z() += g_fTopWaypointOffset;
							vLook.ToSpherical(h, p, r);
							p = ClampT(p,-cl,cl);
						}
						else
						{
							GetClient()->PressButton(BOT_BUTTON_MOVEDN);
							vLook.ToSpherical(h, p, r);
							p = cl;
						}

						vLook.FromSpherical(h, p, r);
						m_LookAheadPt = vEye + vLook;
						//RenderBuffer::AddLine(vEye,m_LookAheadPt, (m_LadderDirection > 0) ? COLOR::GREEN : COLOR::RED,3.f);
					}
				}
				else
				{
					m_LadderDirection = 0;
				}
			}
			//////////////////////////////////////////////////////////////////////////

			// Ignore vertical look points unless it's significantly out of our bbox height.
			const Box3f &worldObb = GetClient()->GetWorldBounds();
			const float fTolerance = worldObb.Extent[2];
			if(Mathf::FAbs(m_LookAheadPt.Z() - worldObb.Center.Z()) < fTolerance)
			{
				m_LookAheadPt.Z() = GetClient()->GetEyePosition().Z();
			}
		}
		return State_Busy;
	}

	bool FollowPath::CheckForMover(const Vector3f &_pos)
	{
		const float fWpHeight = g_fTopWaypointOffset - g_fBottomWaypointOffset;

		// Use a ray the size of a waypoint offset down by half a waypoint height.

		Vector3f pos1 = _pos + Vector3f(0.f,0.f,g_fTopWaypointOffset);
		pos1.Z() -= (fWpHeight * 0.5f);
		Vector3f pos2 = pos1;
		pos2.Z() -= fWpHeight;

		const bool bMover = InterfaceFuncs::IsMoverAt(pos1,pos2);

		if( DebugDrawingEnabled() )
		{
			RenderBuffer::AddLine( pos1,pos2,bMover?COLOR::GREEN:COLOR::RED );
		}
		return bMover;
	}

	void FollowPath::CheckForLowJumps(const Vector3f &_destination)
	{
		Prof(CheckForLowJumps);

		// bot MUST NOT press jump button every frame, otherwise he jumps only once
		obint32 time = IGame::GetTime();
		if(time - m_JumpTime < 100) return;

		// todo: should be game dependant
		const float fStepHeight = GetClient()->GetStepHeight();
		const float fStepBoxWidth = 9.0f; // GAME DEP
		const float fStepBoxHeight = 16.0f; // GAME DEP
		const float fStepRayLength = 48.0f; // GAME DEP

		// Calculate the vector we're wanting to move, this will be
		// the direction of the trace
		Vector3f vMoveVec = _destination - GetClient()->GetPosition();
		vMoveVec.Z() = 0.0f; // get rid of the vertical component
		vMoveVec.Normalize();

		// Get this entities bounding box to use for traces.
		AABB worldAABB, localAABB;
		EngineFuncs::EntityWorldAABB(GetClient()->GetGameEntity(), worldAABB);

		// Calculate the local AABB
		const Vector3f &vPosition = GetClient()->GetPosition();

		// Adjust the AABB mins to the step height
		localAABB.m_Mins[2] = worldAABB.m_Mins[2] + fStepHeight - vPosition[2];
		// Adjust the AABB mins to the step height + fStepBoxHeight
		localAABB.m_Maxs[2] = localAABB.m_Mins[2] + fStepBoxHeight;
		// Adjust the mins and max for the width of the box
		localAABB.m_Mins[0] = localAABB.m_Mins[1] = -fStepBoxWidth;
		localAABB.m_Maxs[0] = localAABB.m_Maxs[1] = fStepBoxWidth;

		// Trace a line forward from the bot.
		Vector3f vStartPos;
		worldAABB.CenterPoint(vStartPos);

		// The end of the ray is out towards the move direction, fStepRayLength length
		Vector3f vEndPos(vStartPos + (vMoveVec * fStepRayLength));

		// Trace a line start to end to see if it hits anything.
		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStartPos, vEndPos,
			&localAABB, TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False);

		bool bHit = false;
		if(tr.m_Fraction != 1.0f)
		{
			bHit = true;
			m_JumpTime = time;
			GetClient()->PressButton(BOT_BUTTON_JUMP);
		}

		if(DebugDrawingEnabled())
		{
			// Line Ray
			RenderBuffer::AddLine(vStartPos, vEndPos,
				bHit ? COLOR::RED : COLOR::GREEN );

			Vector3f vBottomStartLine(vStartPos), vBottomEndLine(vEndPos);
			vBottomStartLine.Z() += localAABB.m_Mins[2];
			vBottomEndLine.Z() += localAABB.m_Mins[2];
			RenderBuffer::AddLine(vBottomStartLine, vBottomEndLine,
				bHit ? COLOR::RED : COLOR::GREEN );

			Vector3f vTopStartLine(vStartPos), vTopEndLine(vEndPos);
			vTopStartLine.Z() += localAABB.m_Maxs[2];
			vTopEndLine.Z() += localAABB.m_Maxs[2];
			RenderBuffer::AddLine(vTopStartLine, vTopEndLine,
				bHit ? COLOR::RED : COLOR::GREEN );
		}
	}

	void FollowPath::CheckForGapJumps(const Vector3f &_destination)
	{
		Prof(CheckForGapJumps);

		// Get this entities bounding box to use for traces.
		AABB aabb;
		EngineFuncs::EntityWorldAABB(GetClient()->GetGameEntity(), aabb);

		// Trace a line down from the bot.
		//const float fStartUpOffset = 5.0f;
		static const float fEndDownOffset = GetClient()->GetGameVar(Client::JumpGapOffset);
		Vector3f vStartPos(Vector3f::ZERO);
		aabb.CenterPoint(vStartPos);

		Vector3f vEndPos(vStartPos);
		vEndPos.Z() -= fEndDownOffset;

		// Trace a line downward to see the distance below us.
		//DEBUG_ONLY(RenderBuffer::AddLine(vStartPos, vEndPos, COLOR::RED));

		// Trace a line directly underneath us to try and detect rapid drops in elevation.
		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStartPos, vEndPos, NULL,
			TR_MASK_SOLID | TR_MASK_PLAYERCLIP, GetClient()->GetGameID(), False);

		Vector3f trEnd(tr.m_Endpos);
		if(m_RayDistance == -1.0f && tr.m_Fraction != 1.0f)
		{
			m_RayDistance = (vStartPos - trEnd).SquaredLength();
		}
		else if(m_RayDistance != -1.0f && tr.m_Fraction == 1.0f)
		{
			GetClient()->PressButton(BOT_BUTTON_JUMP);
		}
		else if(tr.m_Fraction != 1.0f)
		{
			// See if there is any sudden drops. (+20% length in ray maybe?)
			float fNewDist = (vStartPos - trEnd).SquaredLength();
			if(fNewDist > (m_RayDistance + (m_RayDistance * 0.2f)))
			{
				// Try to jump.
				GetClient()->PressButton(BOT_BUTTON_JUMP);
			}
			m_RayDistance = fNewDist;
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////
};