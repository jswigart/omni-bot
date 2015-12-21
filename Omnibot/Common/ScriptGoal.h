////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SCRIPTGOAL_H__
#define __SCRIPTGOAL_H__

#include "StateMachine.h"
#include "Path.h"
#include "ScriptManager.h"
#include "TriggerManager.h"
#include "Criteria.h"
#include "BotBaseStates.h"

class gmScriptGoal;

namespace AiState
{
	struct MoveOptions
	{
		float		Radius;
		int			ThreadId;
		MoveMode	Mode;

		enum
		{
			MaxAvoidPts = 32
		};
		Vector3f	Avoid[ MaxAvoidPts ];
		int			NumAvoid;

		void FromTable( gmMachine *a_machine, gmTableObject *a_table );

		MoveOptions();
	};

	class FiberState : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
	};

	class ScriptGoal : public StateChild, public FollowPathUser, public AimerUser
	{
	public:
		friend class gmScriptGoal;
		friend class ScriptGoals;

		enum FunctionCallback
		{
			ON_INIT,
			ON_SPAWN,
			ON_GETPRIORITY,
			ON_ENTER,
			ON_EXIT,
			ON_UPDATE,
			ON_PATH_THROUGH,
			NUM_CALLBACKS
		};

		void GetDebugString( std::stringstream &out );
		void RenderDebug();

		float GetPriority();
		void Enter();
		void Exit();
		StateStatus Update( float fDt );

		void SetEnable( bool _enable, const char *_error = 0 );
		void SetSelectable( bool _selectable );

		bool AddScriptAimRequest( Priority::ePriority _prio, Aimer::AimType _type, Vector3f _v );

		void Signal( const gmVariable &_var );
		void KillAllGoalThreads();

		float GetScriptPriority() const
		{
			return mScriptPriority;
		}
		void SetScriptPriority( float _p )
		{
			mScriptPriority = _p;
		}
		void SetFinished()
		{
			mFinished = true;
		}
		
		bool OnPathThrough( const std::string &_s );
		void EndPathThrough();

		gmGCRoot<gmFunctionObject> GetCallback( FunctionCallback cb )
		{
			return mCallbacks[ cb ];
		}
		void SetCallback( FunctionCallback cb, gmGCRoot<gmFunctionObject> f )
		{
			mCallbacks[ cb ] = f;
		}

		// functionality
		bool Goto( const Vector3f &_pos, const MoveOptions &options );
		bool GotoRandom( const MoveOptions &options );
		bool RouteTo( MapGoalPtr mg, const MoveOptions &options );
		void Stop();

		// FollowPathUser functions.
		bool GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt );
		void OnPathSucceeded();
		void OnPathFailed( FollowPathUser::FailType _how );

		// AimerUser functions.
		bool GetAimPosition( Vector3f &_aimpos );
		void OnTarget();

		const Vector3f &GetAimVector() const
		{
			return mAimVector;
		}
		void SetAimVector( const Vector3f &_v )
		{
			mAimVector = _v;
			mOnTargetTime = 0;
		}
		int32_t GetAimWeaponId() const
		{
			return mAimWeaponId;
		}
		void SetAimWeaponId( int32_t id )
		{
			mAimWeaponId = id;
		}

		void SetGetPriorityDelay( int32_t _delay )
		{
			mNextGetPriorityDelay = _delay;
		}
		int32_t GetGetPriorityDelay() const
		{
			return mNextGetPriorityDelay;
		}
		void DelayGetPriority( int32_t _delay )
		{
			mNextGetPriorityUpdate = IGame::GetTime() + _delay;
		}

		void AutoReleaseAim( bool _b )
		{
			mAutoReleaseAim = _b;
		}
		bool AutoReleaseAim()
		{
			return mAutoReleaseAim;
		}

		void AutoReleaseWeapon( bool _b )
		{
			mAutoReleaseWpn = _b;
		}
		bool AutoReleaseWeapon()
		{
			return mAutoReleaseWpn;
		}

		void AutoReleaseTracker( bool _b )
		{
			mAutoReleaseTracker = _b;
		}
		bool AutoReleaseTracker()
		{
			return mAutoReleaseTracker;
		}

		void AutoFinishOnUnAvailable( bool _b )
		{
			mAutoFinishOnUnavailable = _b;
		}
		bool AutoFinishOnUnAvailable() const
		{
			return mAutoFinishOnUnavailable;
		}

		void AutoFinishOnNoProgressSlots( bool _b )
		{
			mAutoFinishOnNoProgressSlots = _b;
		}
		bool AutoFinishOnNoProgressSlots() const
		{
			return mAutoFinishOnNoProgressSlots;
		}

		void AutoFinishOnNoUseSlots( bool _b )
		{
			mAutoFinishOnNoUseSlots = _b;
		}
		bool AutoFinishOnNoUseSlots() const
		{
			return mAutoFinishOnNoUseSlots;
		}

		void SkipGetPriorityWhenActive( bool _b )
		{
			mSkipGetPriorityWhenActive = _b;
		}
		bool SkipGetPriorityWhenActive() const
		{
			return mSkipGetPriorityWhenActive;
		}

		bool AddFinishCriteria( const CheckCriteria &_crit );
		void ClearFinishCriteria( bool _clearpersistent = false );

		void WatchForEntityCategory( float radius, const BitFlag32 &category, int customTrace );
		void UpdateEntityInRadius();
		GameEntity IterateWatchEntity( GameEntity lastEntity );

		void WatchForMapGoalsInRadius( const GoalManager::Query &qry, const GameEntity & ent, float radius );
		void ClearWatchForMapGoalsInRadius();
		void UpdateMapGoalsInRadius();

		// Special case callbacks.
		void ProcessEvent( const Message &_message, CallbackParameters &_cb );
		bool OnInit( gmMachine *_machine );
		void OnSpawn();

		//ThreadList &GetThreadList() { return .mThreadList; }
		MapGoalPtr &GetMapGoal()
		{
			return mMapGoal;
		}
		void SetMapGoal( MapGoalPtr &_mg )
		{
			mMapGoal = _mg;
			mTracker.Reset();
		}
		MapGoal *GetMapGoalPtr()
		{
			return mMapGoal.get();
		}

		void SetParentName( const char *_str );
		std::string GetParentName() const;
		uint32_t GetParentNameHash() const
		{
			return mParentNameHash;
		}

		void SetInsertBeforeName( const char *_str );
		std::string GetInsertBeforeName() const;
		uint32_t GetInsertBeforeHash() const
		{
			return mInsertBeforeHash;
		}

		void SetInsertAfterName( const char *_str );
		std::string GetInsertAfterName() const;
		uint32_t GetInsertAfterHash() const
		{
			return mInsertAfterHash;
		}

		gmUserObject *GetScriptObject( gmMachine *_machine );

		int GetOnTargetTime() const { return mOnTargetTime; }

		//////////////////////////////////////////////////////////////////////////
		int gmfFinished( gmThread *a_thread );
		int gmfGoto( gmThread *a_thread );
		int gmfGotoAsync( gmThread *a_thread );
		int gmfAddAimRequest( gmThread *a_thread );
		int gmfReleaseAimRequest( gmThread *a_thread );
		int gmfAddWeaponRequest( gmThread *a_thread );
		int gmfReleaseWeaponRequest( gmThread *a_thread );
		int gmfUpdateWeaponRequest( gmThread *a_thread );
		int gmfBlockForWeaponChange( gmThread *a_thread );
		int gmfBlockForWeaponFire( gmThread *a_thread );
		int gmfBlockForVoiceMacro( gmThread *a_thread );
		int gmfThreadFork( gmThread *a_thread );
		int gmfSignal( gmThread *a_thread );

		//////////////////////////////////////////////////////////////////////////

		gmGCRoot<gmStringObject> &DebugString()
		{
			return mDebugString;
		}

		//////////////////////////////////////////////////////////////////////////

		bool MarkInProgress( MapGoalPtr _p );
		bool MarkInUse( MapGoalPtr _p );

		//////////////////////////////////////////////////////////////////////////

		const std::string& GetSourceFile() const;

		void SetSourceFile( const std::string& filePath );

		void HotReloadScript();


		ScriptGoal *Clone();
		ScriptGoal( const char *_name );
		~ScriptGoal();
	protected:
		void InternalSignal( const gmVariable &_signal );
		void InternalExit();
	private:
		Vector3f				 mAimVector;
		int32_t					 mAimWeaponId;

		Aimer::AimType			 mScriptAimType;

		float					 mScriptPriority;
		float					 mMinRadius;

		gmGCRoot<gmFunctionObject> mCallbacks[ NUM_CALLBACKS ];
		ThreadScoper			 mActiveThread[ NUM_CALLBACKS ];

		gmGCRoot<gmStringObject> mDebugString;

		uint32_t				 mParentNameHash;
		uint32_t				 mInsertBeforeHash;
		uint32_t				 mInsertAfterHash;

		int32_t					 mNextGetPriorityUpdate;
		int32_t					 mNextGetPriorityDelay;

		struct WatchEntity
		{
			static const int MaxEntities = 64;

			float		mRadius;
			BitFlag32	mCategory;
			int			mCustomTrace;

			struct KnownEnt
			{
				GameEntity	 mEnt;
				int			 mTimeStamp;

				KnownEnt()
				{
					Reset();
				}
				void Reset()
				{
					mEnt.Reset();
					mTimeStamp = 0;
				}
			};
			
			KnownEnt mEntry[ MaxEntities ];

			WatchEntity() : mRadius( 0.f ), mCustomTrace( 0 )
			{
			}
		};
		
		WatchEntity mWatchEntities;

		static const int MaxCriteria = 8;
		CheckCriteria			 mFinishCriteria[ MaxCriteria ];

		typedef std::set<MapGoalPtr> MgSet;
		struct WatchForMapGoal
		{
			GameEntity		 mPositionEnt;
			float			 mRadius;

			MgSet			 mInRadius;
			GoalManager::Query mQuery;

			WatchForMapGoal() 
				: mRadius( 0.f )
			{
			}
		}						mMapGoalInRadius;

		int						mOnTargetTime;
		bool					mFinished : 1;
		bool					mAutoReleaseAim : 1;
		bool					mAutoReleaseWpn : 1;
		bool					mAutoReleaseTracker : 1;
		bool					mAutoFinishOnUnavailable : 1;
		bool					mAutoFinishOnNoProgressSlots : 1;
		bool					mAutoFinishOnNoUseSlots : 1;
		bool					mSkipGetPriorityWhenActive : 1;
		bool					mSkipLastWp : 1;

		MapGoalPtr				mMapGoal;
		MapGoalPtr				mMapGoalRoute;
		Trackers				mTracker;

		std::string				mSourceFile;

		ScriptGoal();
	};
};

#endif
