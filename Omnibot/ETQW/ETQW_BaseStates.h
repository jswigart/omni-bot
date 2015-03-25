////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETQWBASESTATES_H__
#define __ETQWBASESTATES_H__

#include "BotBaseStates.h"
#include "MapGoal.h"

namespace AiState
{
	class BuildConstruction : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		BuildConstruction();
	private:
		Trackers		Tracker;

		MapGoalPtr	 mMapGoal;
		Vector3f	 mConstructionPos;
		bool		 mAdjustedPosition;
	};

	//////////////////////////////////////////////////////////////////////////

	class PlantExplosive : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		PlantExplosive();
	private:
		typedef enum
		{
			LAY_EXPLOSIVE,
			ARM_EXPLOSIVE,
			RUNAWAY,
			DETONATE_EXPLOSIVE
		} GoalState;

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;

		GoalState		 mGoalState;
		Vector3f		 mTargetPosition;
		GameEntity		 mExplosiveEntity;
		Vector3f		 mExplosivePosition;
		bool			 mAdjustedPosition : 1;

		State::StateStatus _UpdateDynamite();
		State::StateStatus _UpdateSatchel();
	};

	//////////////////////////////////////////////////////////////////////////

	class MountMg42 : public StateChild, public FollowPathUser, public AimerUser
	{
	public:

		void GetDebugString(std::stringstream &out);
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		MountMg42();
	private:
		MapGoalPtr		 mMapGoal;
		Trackers			Tracker;

		enum ScanType
		{
			SCAN_DEFAULT,
			SCAN_MIDDLE,
			SCAN_LEFT,
			SCAN_RIGHT,
			SCAN_ZONES,

			NUM_SCAN_TYPES
		};
		int				 mScanDirection;
		int				 mNextScanTime;

		TrackTargetZone	 mTargetZone;

		//////////////////////////////////////////////////////////////////////////
		Vector3f		 mMG42Position;

		Vector3f		 mAimPoint;

		Vector3f		 mGunCenterArc;
		Vector3f		 mCurrentMountedAngles;

		Vector3f		 mScanLeft;
		Vector3f		 mScanRight;

		float			 mMinHorizontalArc;
		float			 mMaxHorizontalArc;
		float			 mMinVerticalArc;
		float			 mMaxVerticalArc;

		bool			 mGotGunProperties;
		//////////////////////////////////////////////////////////////////////////

		bool			 mAdjustedPosition;

		bool				_GetMG42Properties();
	};

	//////////////////////////////////////////////////////////////////////////

	class TakeCheckPoint : public StateChild, public FollowPathUser
	{
	public:
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		TakeCheckPoint();
	private:
		Trackers		Tracker;

		MapGoalPtr	 mMapGoal;
		Vector3f	 mTargetPosition;
	};

	//////////////////////////////////////////////////////////////////////////

	class PlantMine : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		void ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		PlantMine();
	private:

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;

		Vector3f		 mTargetPosition;
		GameEntity		 mLandMineEntity;
		Vector3f		 mLandMinePosition;
		Vector3f		 mLandMineVelocity;
	};

	//////////////////////////////////////////////////////////////////////////

	class MobileMg42 : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		MobileMg42();
	private:

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
	};

	//////////////////////////////////////////////////////////////////////////

	class ReviveTeammate : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		typedef enum
		{
			REVIVING,
			HEALING,
		} GoalState;

		void GetDebugString(std::stringstream &out);
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		ReviveTeammate();
	private:
		GoalState		 mGoalState;

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
	};

	//////////////////////////////////////////////////////////////////////////

	class DefuseDynamite : public StateChild, public FollowPathUser, public AimerUser
	{
	public:

		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update(float fDt);

		// FollowPathUser functions.
		bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

		// AimerUser functions.
		bool GetAimPosition(Vector3f &_aimpos);
		void OnTarget();

		DefuseDynamite();
	private:
		Vector3f		 mTargetPosition;

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
	};
};

#endif
