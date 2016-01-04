////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TFBASESTATES_H__
#define __TFBASESTATES_H__

#include "StateMachine.h"
#include "BotBaseStates.h"
#include "Path.h"
#include "ScriptManager.h"
#include "TF_Messages.h"
#include "InternalFsm.h"

class gmScriptGoal;

namespace TF_Options
{
	extern int BUILD_AMMO_TYPE;

	extern int SENTRY_BUILD_AMMO;
	extern int SENTRY_UPGRADE_AMMO;
	extern int SENTRY_REPAIR_AMMO;
	extern int SENTRY_UPGRADE_WPN;

	extern int BUILD_ATTEMPT_DELAY;

	extern int TELEPORT_BUILD_AMMO;
	extern int DISPENSER_BUILD_AMMO;

	extern int PIPE_WEAPON;
	extern int PIPE_WEAPON_WATCH;
	extern int PIPE_AMMO;
	extern int PIPE_MAX_DEPLOYED;

	extern int ROCKETJUMP_WPN;
	extern float GRENADE_VELOCITY;

	extern bool POLL_SENTRY_STATUS;
	extern bool REPAIR_ON_SABOTAGED;

	extern int DisguiseTeamFlags[ TF_TEAM_MAX ];
};

namespace AiState
{
	enum SentryBuildState
	{
		SG_NONE,
		SG_GETTING_AMMO,
		SG_BUILDING,
		SG_AIMING,
		SG_AIMED,
		SG_UPGRADING,
		SG_REPAIRING,
		SG_RESUPPLY,
		SG_DONE,
		SG_NUMBUILDSTATES,
	};
	class SentryBuild :
		public StateChild,
		public FollowPathUser,
		public AimerUser,
		public InternalFSM < SentryBuild, SG_NUMBUILDSTATES >
	{
	public:

		void GetDebugString( std::stringstream &out );

		MapGoalPtr GetBuiltSentryMapGoal() const
		{
			return mBuiltSentry;
		}

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

		static int BuildEquipWeapon;

		SentryBuild();
	private:
		MapGoalPtr	 mMapGoalSentry;
		MapGoalPtr	 mBuiltSentry;
		int			 mNextBuildTry;
		int			 mNeedAmmoAmount;

		float		 mSentryPriority;

		RecordHandle mAmmoPack;

		bool		 mCantBuild : 1;

		bool HasEnoughAmmo( int _ammotype, int _ammorequired );

		STATE_PROTOTYPE( SG_NONE );
		STATE_PROTOTYPE( SG_GETTING_AMMO );
		STATE_PROTOTYPE( SG_BUILDING );
		STATE_PROTOTYPE( SG_AIMING );
		STATE_PROTOTYPE( SG_AIMED );
		STATE_PROTOTYPE( SG_UPGRADING );
		STATE_PROTOTYPE( SG_REPAIRING );
		STATE_PROTOTYPE( SG_RESUPPLY );
		STATE_PROTOTYPE( SG_DONE );
	};

	//////////////////////////////////////////////////////////////////////////
	class SentryAlly : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		enum SubState
		{
			UPGRADING,
			REPAIRING,
			RESUPPLY,
		};

		void GetDebugString( std::stringstream &out );

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		SentryAlly();
	private:
		RecordHandle mAllySentry;
		SubState	 mState;
	};

	//////////////////////////////////////////////////////////////////////////
	class DispenserBuild : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		enum SubState
		{
			DISP_NONE,
			DISP_GETTING_AMMO,
			DISP_BUILDING,
			DISP_REPAIRING,
			DISP_DONE,
		};

		void GetDebugString( std::stringstream &out );

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		static int BuildEquipWeapon;

		DispenserBuild();
	private:
		MapGoalPtr	 mMapGoalDisp;
		MapGoalPtr	 mBuiltDisp;
		int			 mNextBuildTry;
		int			 mNextAmmoCheck;
		SubState	 mState;
		bool		 mCantBuild : 1;
	};

	//////////////////////////////////////////////////////////////////////////
	class Sentry : public StatePrioritized
	{
	public:
		typedef struct
		{
			TF_BuildableStatus mStatus;
			GameEntity		 mEntity;
			Vector3f		 mPosition;
			Vector3f		 mFacing;
			int				 mLevel;
			int				 mHealth;
			int				 mMaxHealth;
			int				 mShells[ 2 ];
			int				 mRockets[ 2 ];
			bool			 mSabotaged;

			void Reset()
			{
				mStatus = BUILDABLE_INVALID;
				mEntity.Reset();
				mPosition = Vector3f::ZERO;
				mFacing = Vector3f::ZERO;
				mLevel = 0;
				mHealth = mMaxHealth = 0;
				mShells[ 0 ] = mShells[ 1 ] = 0;
				mShells[ 0 ] = mRockets[ 1 ] = 0;
				mSabotaged = false;
			}
		} SentryStatus;

		void GetDebugString( std::stringstream &out );

		bool SentryFullyBuilt() const
		{
			return ( mSentryStatus.mStatus == BUILDABLE_BUILT );
		}
		bool HasSentry() const
		{
			return mSentryStatus.mEntity.IsValid();
		}
		bool SentryBuilding() const
		{
			return ( mSentryStatus.mStatus == BUILDABLE_BUILDING );
		}

		const SentryStatus &GetSentryStatus() const
		{
			return mSentryStatus;
		}

		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		void UpdateSentryStatus( const ParamsSentryStatus_TF & stats );

		Sentry();
	private:
		SentryStatus mSentryStatus;
	};

	//////////////////////////////////////////////////////////////////////////
	class Dispenser : public StatePrioritized
	{
	public:
		typedef struct
		{
			TF_BuildableStatus mStatus;
			GameEntity		 mEntity;
			Vector3f		 mPosition;
			Vector3f		 mFacing;
			int				 mHealth;
			int				 mCells;
			int				 mNails;
			int				 mRockets;
			int				 mShells;
			int				 mArmor;
			bool			 mSabotaged;

			void Reset()
			{
				mStatus = BUILDABLE_INVALID;
				mEntity.Reset();
				mPosition = Vector3f::ZERO;
				mFacing = Vector3f::ZERO;
				mHealth = 0;
				mCells = 0;
				mNails = 0;
				mRockets = 0;
				mShells = 0;
				mArmor = 0;
				mSabotaged = false;
			}
		} DispenserStatus;

		void GetDebugString( std::stringstream &out );

		bool DispenserFullyBuilt() const
		{
			return ( mDispenserStatus.mStatus == BUILDABLE_BUILT );
		}
		bool HasDispenser() const
		{
			return mDispenserStatus.mEntity.IsValid();
		}
		bool DispenserBuilding() const
		{
			return ( mDispenserStatus.mStatus == BUILDABLE_BUILDING );
		}

		const DispenserStatus &GetDispenserStatus() const
		{
			return mDispenserStatus;
		}

		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		void UpdateDispenserStatus( const ParamsDispenserStatus_TF &_stats );

		Dispenser();
	private:
		DispenserStatus	 mDispenserStatus;
	};

	//////////////////////////////////////////////////////////////////////////
	class DetpackBuild : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		enum SubState
		{
			NONE,
			DETPACK_BUILDING,
		};

		void GetDebugString( std::stringstream &out );

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		DetpackBuild();
	private:
		MapGoalPtr	 mMapGoal;
		int			 mNextBuildTry;
		SubState	 mState;
		int			 mDetpackFuse;
		bool		 mCantBuild : 1;
	};
	//////////////////////////////////////////////////////////////////////////

	class Detpack : public StatePrioritized
	{
	public:
		typedef struct
		{
			TF_BuildableStatus mStatus;
			GameEntity		 mEntity;
			Vector3f		 mPosition;
		} DetpackStatus;

		bool DetpackFullyBuilt() const
		{
			return ( mDetpackStatus.mStatus == BUILDABLE_BUILT );
		}
		bool HasDetpack() const
		{
			return mDetpackStatus.mEntity.IsValid();
		}
		bool DetpackBuilding() const
		{
			return ( mDetpackStatus.mStatus == BUILDABLE_BUILDING );
		}

		const DetpackStatus &GetDetpackStatus() const
		{
			return mDetpackStatus;
		}

		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		void UpdateDetpackStatus( const Event_DetpackStatus_TF& stats );

		Detpack();
	private:
		DetpackStatus mDetpackStatus;
	};
	//////////////////////////////////////////////////////////////////////////

	class PipeTrap : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		enum SubState
		{
			IDLE,
			LAY_PIPES,
			WATCH_PIPES,
			GETTING_AMMO,
		};

		enum Order
		{
			OrderRandomAll,
			OrderRandomPick1,
			OrderSequentialAll,
		};

		struct PipeInfo
		{
			GameEntity mEntity;
			Vector3f mPosition;
			uint32_t mTrapIndex : 4;
			uint32_t mMoving : 1;
		};

		struct Pipes
		{
			enum
			{
				MaxPipes = 8
			};
			PipeInfo mPipes[ MaxPipes ];
			int		 mPipeCount;
		};

		struct Trap
		{
			Vector3f mSource;
			Vector3f mFacing;
			void Reset()
			{
				mSource = Vector3f::ZERO;
				mFacing = Vector3f::ZERO;
			}
		};

		struct WaitPos
		{
			Vector3f mPosition;
			Vector3f mFacing;
			int		 mMinWaitTime;
			int		 mMaxWaitTime;

			void Reset()
			{
				mPosition = Vector3f::ZERO;
				mFacing = Vector3f::ZERO;
				mMinWaitTime = 2000;
				mMaxWaitTime = 6000;
			}
		};

		const Pipes &GetPipes() const
		{
			return mPipes;
		}
		int GetPipeCount() const
		{
			return mPipes.mPipeCount;
		}

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );
		void GetDebugString( std::stringstream &out );
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

		PipeTrap();
	private:
		SubState mSubstate;
		Pipes	 mPipes;

		enum
		{
			MaxPlacementPts = 8, MaxWaitPositions = 8
		};
		Trap	 mTraps[ MaxPlacementPts ];
		WaitPos	 mWait[ MaxWaitPositions ];

		MapGoalPtr mMapGoal;
		FilterPtr mWatchFilter;

		int32_t	 mPlaceOrder;
		int32_t	 mWaitOrder;

		int32_t	 mExpireTime;

		uint32_t mNumTraps : 4;
		uint32_t mNumWaits : 4;

		uint32_t mCurrentTrap : 4;
		uint32_t mCurrentWait : 4;

		bool		CacheGoalInfo( MapGoalPtr mg );
		bool		ShouldGoForAmmo();
	};

	//////////////////////////////////////////////////////////////////////////

	class RocketJump : public StateChild, public AimerUser
	{
	public:

		void RenderDebug();

		float GetPriority();
		StateStatus Update( float fDt );

		void Enter();
		void Exit();

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		RocketJump();
	private:
		Path::PathPoint mNextPt;
		bool		 mIsDone : 1;
	};

	//////////////////////////////////////////////////////////////////////////

	class ConcussionJump : public StateChild, public AimerUser
	{
	public:

		void RenderDebug();

		float GetPriority();
		StateStatus Update( float fDt );

		void Enter();
		void Exit();

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		ConcussionJump();
	private:
		Path::PathPoint mNextPt;
	};

	//////////////////////////////////////////////////////////////////////////

	class ThrowGrenade : public StateChild, public AimerUser
	{
	public:
		enum AimMode
		{
			GRENADE_AIM,
			GRENADE_RID,
		};

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		float GetPriority();
		StateStatus Update( float fDt );

		void Enter();
		void Exit();

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		void OnSpawn();

		float GetGrenadeSpeed( int _type ) const;

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		ThrowGrenade();
	private:
		int	 mPrimaryGrenade;
		int	 mSecondaryGrenade;

		int	 mGren1Ammo, mGren1AmmoMax;
		int	 mGren2Ammo, mGren2AmmoMax;

		int	 mGrenType;

		int	 mLastThrowTime;

		Vector3f mAimPos;

		AimMode mAimMode;

		bool mOnTarget;

		float _GetProjectileSpeed( int _type ) const;
		float _GetProjectileGravity( int _type ) const;
		void _UpdateAmmo();
		void _UpdateGrenadeTypes();
	};

	//////////////////////////////////////////////////////////////////////////
	class Teleporter : public StatePrioritized
	{
	public:
		typedef struct
		{
			TF_BuildableStatus mStatusEntrance;
			TF_BuildableStatus mStatusExit;
			GameEntity		 mEntityEntrance;
			GameEntity		 mEntityExit;

			Vector3f		 mEntrancePos;
			Vector3f		 mExitPos;

			bool			 mSabotagedEntry;
			bool			 mSabotagedExit;

			void Reset()
			{
				mStatusEntrance = BUILDABLE_INVALID;
				mStatusExit = BUILDABLE_INVALID;
				mEntityEntrance.Reset();
				mEntityExit.Reset();

				mSabotagedEntry = false;
				mSabotagedExit = false;
			}
		} TeleporterStatus;

		bool TeleporterEntranceFullyBuilt() const
		{
			return ( mTeleporterStatus.mStatusEntrance == BUILDABLE_BUILT );
		}
		bool TeleporterExitFullyBuilt() const
		{
			return ( mTeleporterStatus.mStatusExit == BUILDABLE_BUILT );
		}
		bool HasTeleporterEntrance() const
		{
			return mTeleporterStatus.mEntityEntrance.IsValid();
		}
		bool HasTeleporterExit() const
		{
			return mTeleporterStatus.mEntityExit.IsValid();
		}
		bool TeleporterEntranceBuilding() const
		{
			return ( mTeleporterStatus.mStatusEntrance == BUILDABLE_BUILDING );
		}
		bool TeleporterExitBuilding() const
		{
			return ( mTeleporterStatus.mStatusExit == BUILDABLE_BUILDING );
		}

		const TeleporterStatus &GetTeleporterStatus() const
		{
			return mTeleporterStatus;
		}

		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		void UpdateTeleporterStatus( const ParamsTeleporterStatus_TF &_stats );

		Teleporter();
	private:
		TeleporterStatus	 mTeleporterStatus;
	};

	//////////////////////////////////////////////////////////////////////////
	class TeleporterBuild : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		enum SubState
		{
			NONE,
			GETTING_AMMO,
			TELE_BUILDING_ENTRANCE,
			TELE_BUILDING_EXIT,
		};

		void GetDebugString( std::stringstream &out );

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void ProcessEvent( const Message &_message, CallbackParameters &_cb );

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		static int BuildEquipWeapon;

		TeleporterBuild();
	private:
		MapGoalPtr	 mMapGoalTeleEntrance;
		MapGoalPtr	 mBuiltTeleEntrance;

		MapGoalPtr	 mMapGoalTeleExit;
		MapGoalPtr	 mBuiltTeleExit;

		int			 mNextBuildTry;
		int			 mNextAmmoCheck;
		SubState	 mState;
		bool		 mCantBuild : 1;
	};
};

#endif
