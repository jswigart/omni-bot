////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-07 21:15:21 -0600 (Thu, 07 Mar 2013) $
// $LastChangedRevision: 837 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTPATHING_H__
#define __BOTPATHING_H__

#include "StateMachine.h"
#include "Path.h"
#include "BotBaseStates.h"
#include "MapGoal.h"
#include "PathPlannerBase.h"

class gmScriptGoal;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////
	class FollowPath : public StateChild, public AimerUser
	{
	public:
		enum PathStatus
		{
			PathPending,
			PathInProgress,
			PathFinished,
			PathNotFound,
		};

		bool IsOnCustomLink( NavArea linkType ) const;

		bool GotoRandomPt( FollowPathUser *_owner, MoveMode _movemode = Run );
		bool Goto( FollowPathUser *_owner, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *_owner, const Vector3f &_pos, float _radius = 32.f, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *_owner, const Vector3List &_goals, float _radius = 32.f, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *_owner, const MapGoalList &_goals, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *_owner, const DestinationVector &_goals, MoveMode _movemode = Run, bool _skiplastpt = false, bool _final = true );
		void Stop( bool _clearuser = false );
		void ClearUser();
		bool Repath();
		void CancelPathThrough();

		void SaveQuery();
		void RestoreQuery();

		void ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb );

		void DynamicPathUpdated( const Event_DynamicPathsChanged *_m );

		const Vector3f &GetLookAheadPt() const
		{
			return m_LookAheadPt;
		}

		bool IsMoving() const;

		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );
		void NotifyUserSuccess();
		void NotifyUserFailed( FollowPathUser::FailType _how );

		virtual obReal GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		void Initialize();

		FollowPath();
		~FollowPath();
	private:
		struct Query
		{
			FollowPathUser	*	m_User;
			DestinationVector	m_Destination;
			MoveMode			m_MoveMode;
			bool				m_SkipLastPt;
			bool				m_Final;

			Query()
				: m_User( 0 )
				, m_MoveMode( Run )
				, m_SkipLastPt( false )
				, m_Final( false )
			{
			}
		};

		Query				m_Query;
		Query				m_SavedQuery;

		// Internal Data
		PathStatus	m_PathStatus;
		Vector3f	m_LookAheadPt;
		int			m_LadderDirection;
		obint32		m_JumpTime;

		enum
		{
			CachedEdges = 8
		};
		PathInterface::PathCorner		mCachedCorners[ CachedEdges ];
		size_t							mNumCachedCorners;

		obuint32	m_PassThroughState;
		int			m_PathThroughPtIndex;

		// For jump behaviors.
		float		m_RayDistance;

		bool CheckForMover( const Vector3f &_pos );
		void CheckForLowJumps( const Vector3f &_destination );
		void CheckForGapJumps( const Vector3f &_destination );

		PathInterface * m_PathInterface;
	};
};

#endif
