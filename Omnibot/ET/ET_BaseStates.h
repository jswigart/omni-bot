////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ETBASESTATES_H__
#define __ETBASESTATES_H__

#include "StateMachine.h"

#include "BotSteeringSystem.h"
#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	class PlantMine : public StateChild, public FollowPathUser, public AimerUser
	{
	public:

		void GetDebugString( std::stringstream &out );
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message & message, CallbackParameters & cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		PlantMine();
	private:

		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;

		GameEntity		 mLandMineEntity;
		Vector3f		 mLandMinePosition;
		Vector3f		 mLandMineVelocity;
	};

	//////////////////////////////////////////////////////////////////////////

	class MobileMortar : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		void GetDebugString( std::stringstream &out );
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message & message, CallbackParameters & cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		MobileMortar();
	private:
		enum
		{
			MaxMortarAims = 8
		};
		Vector3f			mMortarAim[ MaxMortarAims ];
		int					mCurrentAim;
		int					mNumMortarAims;
		int					mFireDelay;

		MapGoalPtr			mMapGoal;
		Trackers			Tracker;

		bool CacheGoalInfo( MapGoalPtr mg );
	};

	//////////////////////////////////////////////////////////////////////////

	class CallArtillery : public StateChild, public FollowPathUser, public AimerUser
	{
	public:

		void GetDebugString( std::stringstream &out );
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message & message, CallbackParameters & cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		CallArtillery();
	private:
		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
		MapGoalPtr		 mMapGoalTarget;
		GameEntity		 mTargetEntity;
		int				 mFireTime;

		FilterPtr		 mWatchFilter;

		bool			 mFired;
		Seconds			 mMinCampTime;
		Seconds			 mMaxCampTime;
		int				 mStance;

		int				 mExpireTime;
	};

	//////////////////////////////////////////////////////////////////////////

	class UseCabinet : public StateChild, public FollowPathUser//, public AimerUser
	{
	public:
		enum CabinetType
		{
			Health,
			mmo,
		};

		void GetDebugString( std::stringstream &out );
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		UseCabinet();
	private:
		Trackers			Tracker;
		MapGoalPtr		 mMapGoal;
		CabinetType		 mCabinetType;
		int				 mAmmoType;
		int				 mGetAmmoAmount;
		int32_t			 mUseTime;

		float			 mHealthPriority;
		float			 mAmmoPriority;
		float			 mRange;

		GoalManager::Query mQuery;
	};

	//////////////////////////////////////////////////////////////////////////

	//class Flamethrower : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	Flamethrower();
	//private:
	//	Trackers		Tracker;
	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mAimPosition;
	//
	//	TrackTargetZone .mTargetZone;

	//	int			 mMinCampTime;
	//	int			 mMaxCampTime;
	//	int			 mStance;

	//	int			 mExpireTime;
	//};

	////////////////////////////////////////////////////////////////////////////

	//class Panzer : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	Panzer();
	//private:
	//	Trackers		Tracker;
	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mAimPosition;
	//
	//	TrackTargetZone .mTargetZone;

	//	int			 mMinCampTime;
	//	int			 mMaxCampTime;
	//	int			 mStance;

	//	int			 mExpireTime;
	//};

	//class BuildConstruction : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	BuildConstruction();
	//private:
	//	Trackers		Tracker;

	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mConstructionPos;
	//	bool		 mAdjustedPosition;
	//	bool		 mCrouch;
	//	bool		 mProne;
	//	bool		 mIgnoreTargets;
	//};

	//////////////////////////////////////////////////////////////////////////

	//class PlantExplosive : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:
	//	typedef ET_SetDynamiteGoal Mg;

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	void ProcessEvent(const Message & message, CallbackParameters & cb);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	PlantExplosive();
	//private:
	//	typedef enum
	//	{
	//		LAY_EXPLOSIVE,
	//		ARM_EXPLOSIVE,
	//		RUNAWAY,
	//		DETONATE_EXPLOSIVE
	//	} GoalState;

	//	Trackers			Tracker;
	//	MapGoalPtr		 mMapGoal;

	//	GoalState		 mGoalState;
	//	Vector3f		 mTargetPosition;
	//	GameEntity		 mExplosiveEntity;
	//	Vector3f		 mExplosivePosition;
	//	bool			 mAdjustedPosition;
	//	bool			 mIgnoreTargets;

	//	State::StateStatus _UpdateDynamite();
	//	State::StateStatus _UpdateSatchel();
	//	bool _IsGoalAchievable(MapGoalPtr _g, int _weaponId);
	//};

	//////////////////////////////////////////////////////////////////////////

	//class MountMg42 : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:
	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	MountMg42();
	//private:
	//	MapGoalPtr		 mMapGoal;
	//	Trackers			Tracker;

	//	enum ScanType
	//	{
	//		SCAN_DEFAULT,
	//		SCAN_MIDDLE,
	//		SCAN_LEFT,
	//		SCAN_RIGHT,
	//		SCAN_ZONES,

	//		NUM_SCAN_TYPES
	//	};
	//	int				 mScanDirection;
	//	int				 mNextScanTime;

	//	TrackTargetZone	 mTargetZone;

	//	Seconds				 mMinCampTime;
	//	Seconds				 mMaxCampTime;
	//	int				 mStance;

	//	//////////////////////////////////////////////////////////////////////////
	//	Vector3f		 mMG42Position;

	//	Vector3f		 mAimPoint;

	//	Vector3f		 mGunCenterArc;
	//	Vector3f		 mCurrentMountedAngles;
	//
	//	Vector3f		 mScanLeft;
	//	Vector3f		 mScanRight;

	//	float			 mMinHorizontalArc;
	//	float			 mMaxHorizontalArc;
	//	float			 mMinVerticalArc;
	//	float			 mMaxVerticalArc;

	//	int				 mExpireTime;
	//	int				 mStartTime;

	//	bool			 mGotGunProperties;
	//	//////////////////////////////////////////////////////////////////////////

	//	bool			 mAdjustedPosition;
	//	bool			 mIgnoreTargets;

	//	bool				_GetMG42Properties();
	//};

	//////////////////////////////////////////////////////////////////////////

	//class MobileMg42 : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	MobileMg42();
	//private:
	//	Vector3f		 mAimPosition;
	//	WeaponLimits	 mLimits;
	//	MapGoalPtr		 mMapGoal;
	//	TrackTargetZone	 mTargetZone;
	//	Trackers			Tracker;

	//	Seconds				 mMinCampTime;
	//	Seconds				 mMaxCampTime;

	//	int				 mExpireTime;
	//};

	//////////////////////////////////////////////////////////////////////////

	//class RepairMg42 : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:
	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	RepairMg42();
	//private:
	//	Vector3f		 mMG42Position;
	//	MapGoalPtr		 mMapGoal;
	//	Trackers			Tracker;

	//	bool			 mIgnoreTargets;
	//};

	//////////////////////////////////////////////////////////////////////////

	//class TakeCheckPoint : public StateChild, public FollowPathUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	TakeCheckPoint();
	//private:
	//	Trackers		Tracker;

	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mTargetPosition;
	//};

	//////////////////////////////////////////////////////////////////////////

	//class ReviveTeammate : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:
	//	typedef enum
	//	{
	//		REVIVING,
	//		HEALING,
	//	} GoalState;

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	ReviveTeammate();
	//private:
	//	GoalState		 mGoalState;

	//	GameTimer		 mCheckReviveTimer;

	//	Trackers			Tracker;
	//	MapGoalPtr		 mMapGoal;
	//	MapGoalList		 mList;
	//	float			 mRange;

	//	bool AreThereNewReviveGoals();
	//};

	//////////////////////////////////////////////////////////////////////////

	//class DefuseDynamite : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination(DestinationVector &_desination, bool &_final, bool &_skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	DefuseDynamite();
	//private:
	//	Vector3f		 mTargetPosition;

	//	Trackers			Tracker;
	//	MapGoalPtr		 mMapGoal;
	//};
};

#endif
