////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __BOTBASESTATES_H__
#define __BOTBASESTATES_H__

#include "StateMachine.h"
#include "Path.h"
#include "Criteria.h"
#include "MapGoal.h"
#include "FilterSensory.h"

class gmScriptGoal;

namespace AiState
{
	//////////////////////////////////////////////////////////////////////////

	class FollowPathUser
	{
	public:
		friend class Navigator;

		enum FailType
		{
			None,
			NoPath,
			Blocked,
			Interrupted,
		};

		virtual bool GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt )
		{
			return false;
		}

		virtual void OnPathSucceeded()
		{
			mPathSuccess = true;
		};
		virtual void OnPathFailed( FailType _how )
		{
			mPathFailed = _how;
		};

		bool DidPathSucceed() const
		{
			return mPathSuccess;
		}
		bool DidPathFail() const
		{
			return mPathFailed != 0;
		}
		FailType GetFailType() const
		{
			return mPathFailed;
		}

		int GetDestinationIndex() const
		{
			return mDestinationIndex;
		}

		void ResetPathUser()
		{
			mPathFailed = None; mPathSuccess = false;
		}
		bool InProgress() const
		{
			return mPathFailed == None&&!mPathSuccess;
		}

		void SetSourceThread( int _threadId )
		{
			mCallingThread = _threadId;
		}
		int GetSourceThread() const
		{
			return mCallingThread;
		}

		uint32_t GetFollowUserName() const
		{
			return mUserName;
		}

		void SetUserName( uint32_t name );
		void SetUserName( const std::string &name );

		FollowPathUser( const std::string &user );
		FollowPathUser( uint32_t name );
		virtual ~FollowPathUser()
		{
		}
	private:
		uint32_t				 mUserName;

		int							mCallingThread;

		int							mDestinationIndex;

		FailType				 mPathFailed : 3;
		uint8_t					 mPathSuccess : 1;

		FollowPathUser();
	};

	//////////////////////////////////////////////////////////////////////////
	class TrackTargetZone
	{
	public:
		enum
		{
			MaxTargetZones = 8
		};
		struct TargetZone
		{
			Vector3f mPosition;
			int32_t	 mTargetCount;
			bool	 mInUse : 1;
		};

		void Restart( float _radius );

		void UpdateAimPosition();

		bool HasAim() const
		{
			return mValidAim;
		}

		const Vector3f &GetAimPosition()
		{
			return mAimPosition;
		}

		void RenderDebug();

		void Update( Client *_client );

		TrackTargetZone();
	private:
		float		 mRadius;
		Vector3f	 mAimPosition;
		GameEntity	 mLastTarget;
		TargetZone	 mTargetZones[ MaxTargetZones ];
		bool		 mValidAim;
	};
	//////////////////////////////////////////////////////////////////////////
	class AimerUser
	{
	public:
		virtual bool GetAimPosition( Vector3f &_aimpos ) = 0;
		virtual void OnTarget() = 0;

		virtual ~AimerUser()
		{
		}
	private:
	};

	class Aimer : public StateChild
	{
	public:
		enum AimType
		{
			WorldPosition,
			WorldFacing,
			MoveDirection,
			UserCallback,
		};

		struct AimRequest
		{
			friend class Aimer;

			Priority::ePriority	 mPriority;
			uint32_t 			 mOwner;
			AimType				 mAimType;
			Vector3f			 mAimVector;
			AimerUser			*mAimerUser;

			void Reset();
			AimRequest();
		};

		enum
		{
			MaxAimRequests = 8
		};

		bool AddAimRequest( Priority::ePriority _prio, AimerUser *_owner, uint32_t _ownername );
		bool AddAimFacingRequest( Priority::ePriority _prio, uint32_t _owner, const Vector3f &_v );
		bool AddAimPositionRequest( Priority::ePriority _prio, uint32_t _owner, const Vector3f &_v );
		bool AddAimMoveDirRequest( Priority::ePriority _prio, uint32_t _owner );

		void ReleaseAimRequest( uint32_t _owner );
		void UpdateAimRequest( uint32_t _owner, const Vector3f &_pos );

		AimRequest *GetHighestAimRequest( bool _clearontarget );

		void OnSpawn();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void GetDebugString( std::stringstream &out );

		int GetAllRequests( AimRequest *_records, int _max );

		void RenderDebug();

		Aimer();
	private:
		AimRequest mAimRequests[ MaxAimRequests ];
		uint32_t  mBestAimOwner;

		AimRequest *FindAimRequest( uint32_t _owner );
	};

	//////////////////////////////////////////////////////////////////////////

	class CaptureTheFlag : public StateChild, public FollowPathUser
	{
	public:
		enum GoalState
		{
			Idle,
			GettingFlag,
			CarryingToCap,
			CarryingToHold,
			HoldingFlag,
		};

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		GoalState GetGoalState() const
		{
			return mGoalState;
		}

		void GetDebugString( std::stringstream &out );
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		// FollowPathUser
		bool GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt );

		CaptureTheFlag();
	private:
		GoalState				 mGoalState;
		int						 mLastFlagState;
		int32_t					 mNextMoveTime;
		MapGoalPtr				 mMapGoalFlag;
		MapGoalPtr				 mMapGoalCap;

		Trackers					Tracker;

		bool LookForCapGoal( MapGoalPtr &ptr, GoalState &st );
	};

	//////////////////////////////////////////////////////////////////////////

	class ReturnTheFlag : public StateChild, public FollowPathUser
	{
	public:

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		//void GetDebugString(std::stringstream &out);
		MapGoal *GetMapGoalPtr();
		void RenderDebug();

		// FollowPathUser

		ReturnTheFlag();
	private:
		Vector3f		 mLastGoalPosition;
		MapGoalPtr		 mMapGoal;

		TrackInUse		 mMapGoalProg;
	};

	//////////////////////////////////////////////////////////////////////////

	//class Defend : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination( DestinationVector& desination, bool& final, bool& skiplastpt);

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	Defend();
	//private:
	//	Trackers		Tracker;
	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mAimPosition;
	//
	//	TrackTargetZone mTargetZone;

	//	Seconds		 mMinCampTime;
	//	Seconds		 mMaxCampTime;
	//	int			 mEquipWeapon;
	//	int			 mStance;

	//	int			 mExpireTime;
	//};

	//////////////////////////////////////////////////////////////////////////

	//class Attack : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination( DestinationVector& desination, bool& final, bool &skiplastpt );

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	Attack();
	//private:
	//	Trackers		Tracker;
	//	MapGoalPtr	 mMapGoal;
	//	Vector3f	 mAimPosition;

	//	TrackTargetZone mTargetZone;

	//	Seconds		 mMinCampTime;
	//	Seconds		 mMaxCampTime;
	//	int			 mEquipWeapon;
	//	int			 mStance;

	//	int			 mExpireTime;
	//};

	//////////////////////////////////////////////////////////////////////////

	class Roam : public StateChild, public FollowPathUser
	{
	public:
		float GetPriority();

		void Exit();
		StateStatus Update( float fDt );

		Roam();
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	/*class HighLevel : public StatePrioritized
	{
	public:
		HighLevel();
	private:
	};*/

	//////////////////////////////////////////////////////////////////////////

	class LookAround : public StateChild
	{
	public:

		int GetNextLookTime();

		void OnSpawn();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		LookAround();
	private:
		int	 mNextLookTime;

		float mAwareness[ 8 ]; // 8 cardinal directions
	};

	//////////////////////////////////////////////////////////////////////////

	class MotorControl : public StateSimultaneous
	{
	public:
		MotorControl();
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	class LowLevel : public StateSimultaneous
	{
	public:
		LowLevel();
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	class Main : public StateSimultaneous
	{
	public:
		float GetPriority();
		void Enter();
		Main();
		void OnSpawn();
	private:
		bool mOnSpawnCalled;
	};

	//////////////////////////////////////////////////////////////////////////

	class Dead : public StateChild
	{
	public:
		float GetPriority();
		StateStatus Update( float fDt );
		Dead();
	private:
		bool bForceActivate;
	};

	//////////////////////////////////////////////////////////////////////////

	class Warmup : public StateChild
	{
	public:
		float GetPriority();
		StateStatus Update( float fDt );
		Warmup();
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	class Root : public StateFirstAvailable
	{
	public:
		Root();
	private:
	};

	//////////////////////////////////////////////////////////////////////////

	struct EvProximityTrigger
	{
		typedef MessageT<EvProximityTrigger, MSG_PROXIMITY_TRIGGER> Msg;
		
		uint32_t		mOwnerState;
		GameEntity		mEntity;
		Vector3f		mPosition;
	};

	class ProximityWatcher : public StateChild
	{
	public:

		enum
		{
			MaxTriggers = 8
		};
		struct Trigger
		{
			uint32_t mOwnerState;
			FilterPtr mSensoryFilter;
			bool	 mDeleteOnFire : 1;
		};

		void AddWatch( uint32_t _owner, FilterPtr _filter, bool _fireonce = true );
		void RemoveWatch( uint32_t _owner );

		void RenderDebug();

		float GetPriority();
		StateStatus Update( float fDt );

		ProximityWatcher();
	private:
		Trigger	 mTriggers[ MaxTriggers ];
	};

	//////////////////////////////////////////////////////////////////////////

	//class Snipe : public StateChild, public FollowPathUser, public AimerUser
	//{
	//public:

	//	void GetDebugString(std::stringstream &out);
	//	void RenderDebug();

	//	float GetPriority();
	//	void Enter();
	//	void Exit();
	//	StateStatus Update(float fDt);

	//	// FollowPathUser functions.
	//	bool GetNextDestination( DestinationVector& desination, bool& final, bool &skiplastpt );

	//	// AimerUser functions.
	//	bool GetAimPosition(Vector3f &_aimpos);
	//	void OnTarget();

	//	Snipe();
	//private:
	//	Trackers			Tracker;
	//	MapGoalPtr		 mMapGoal;

	//	Vector3f		 mAimPosition;

	//	GameEntity		 mTargetEntity;
	//	Vector3f		 mLastTargetPos;

	//	int				 mScopedWeaponId;
	//	int				 mNonScopedWeaponId;

	//	TrackTargetZone	 mTargetZone;

	//	Seconds			 mMinCampTime;
	//	Seconds			 mMaxCampTime;
	//	int				 mStance;

	//	int				 mExpireTime;
	//	int				 mNextScanTime;
	//};

	//////////////////////////////////////////////////////////////////////////

	class FloodFiller : public StateChild
	{
	public:
		enum FillState
		{
			FillIdle,
			FillInit,
			FillSearching,
			FillOpenNess,
			FillSectorize,
			FillDone,
		};

		enum Direction
		{
			DIR_NORTH,
			DIR_EAST,
			DIR_SOUTH,
			DIR_WEST,

			DIR_NUM
		};

		FillState GetFillState() const
		{
			return State;
		}
		void NextFillState();
		void StartFloodFill( const Vector3f &_Start, float _Radius = 16.f );

		void Reset();

		//////////////////////////////////////////////////////////////////////////

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		FloodFiller();
	private:
		Vector3f	Start;
		float		Radius;

		FillState	State;

		AABB		FloodBlock;

		//////////////////////////////////////////////////////////////////////////
		struct IntOffset2d
		{
			int16_t		X;
			int16_t		Y;
		};
		//////////////////////////////////////////////////////////////////////////
		struct Node;
		struct Connection
		{
			Node		*Destination;
			uint8_t		Cover : 4;
			uint8_t		Jump : 1;

			void Reset();
		};
		struct Node
		{
			IntOffset2d		MinOffset;
			IntOffset2d		MaxOffset;
			float			Height;

			Connection		Connections[ DIR_NUM ];

			uint16_t		SectorId;

			uint8_t			OpenNess;

			uint8_t			Open : 1;
			uint8_t			NearObject : 1;
			uint8_t			NearEdge : 1;
			uint8_t			Sectorized : 1;

			void Init( int16_t _X = 0, int16_t _Y = 0, float _Height = 0.f, bool _Open = false );
		};

		Vector3f				_GetNodePosition( const Node &_Node );
		FloodFiller::Node *		_NextOpenNode();
		Node *					_NodeExists( int16_t _X, int16_t _Y, float _Height );
		bool					_TestNode( const Node *_Node );
		bool					_DropToGround( Node *_Node );
		void					_MakeConnection( Node *_NodeA, Node *_NodeB, Direction _Dir );
		void					_FillOpenNess( bool _ResetAll );
		void					_MergeSectors();
		bool					_CanMergeWith( Node *_Node, Node *_WithNode );

		static const int NumSectors = 8192;
		Node		Nodes[ NumSectors ];
		int			FreeNode;
	};

	//////////////////////////////////////////////////////////////////////////
};

#endif
