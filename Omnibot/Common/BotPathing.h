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
	class Navigator : public StateChild, public AimerUser
	{
	public:
		enum PathStatus
		{
			PathPending,
			PathInProgress,
			PathFinished,
			PathNotFound,
		};

		size_t GetNumCorners() const;
		const PathInterface::PathCorner& GetCorner( size_t index ) const;

		bool IsOnCustomLink() const;
		bool IsOnCustomLink( NavAreaFlags type ) const;
		bool ApproachingArea( NavAreaFlags type, float lookahead ) const;
		int GetAreaEntitiesAlongPath( float lookahead, GameEntity entities [], size_t maxEntities ) const;
		size_t GetAreaEntitiesInRadius( const Vector3f& pos, float radius, GameEntity entities [], size_t maxEntities );

		bool NavTrace( PathInterface::NavTraceResult& result, const Vector3f& start, const Vector3f& end );

		bool GotoRandomPt( FollowPathUser *_owner, MoveMode _movemode = Run );
		bool Goto( FollowPathUser *owner, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *owner, const Vector3f &_pos, float _radius = 32.f, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *owner, const Vector3List &_goals, float _radius = 32.f, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *owner, const MapGoalList &_goals, MoveMode _movemode = Run, bool _skiplastpt = false );
		bool Goto( FollowPathUser *owner, const DestinationVector &_goals, MoveMode _movemode = Run, bool _skiplastpt = false, bool _final = true );
		void Stop( bool clearuser = false );
		void ClearUser();
		bool Repath();
		void CancelPathThrough();

		void SaveQuery();
		void RestoreQuery();

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		const Vector3f &GetLookAheadPt() const
		{
			return mLookAheadPt;
		}

		bool IsMoving() const;

		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );
		void NotifyUserSuccess();
		void NotifyUserFailed( FollowPathUser::FailType _how );

		virtual float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		void Initialize();

		Navigator();
		~Navigator();
	private:
		struct Query
		{
			FollowPathUser	*	mUser;
			DestinationVector	mDestination;
			MoveMode			mMoveMode;
			bool				mSkipLastPt;
			bool				mFinal;

			Query();
		};

		Query						mQuery;
		Query						mSavedQuery;

		// Internal Data
		PathStatus					mPathStatus;
		Vector3f					mLookAheadPt;
		int							mLadderDirection;
		int32_t						mJumpTime;

		OffMeshConnection			mActiveLink;
		size_t						mActiveLinkIndex;

		static const size_t CachedEdges = 8;
		PathInterface::PathCorner	mCachedCorners[ CachedEdges ];
		size_t						mNumCachedCorners;

		uint32_t					mPassThroughState;
		int							mPathThroughPtIndex;

		// For jump behaviors.
		float						mRayDistance;

		bool CheckForMover( const Vector3f &_pos );
		void CheckForLowJumps( const Vector3f &_destination );
		void CheckForGapJumps( const Vector3f &_destination );

		PathInterface *				mPathInterface;
	};
};

#endif
