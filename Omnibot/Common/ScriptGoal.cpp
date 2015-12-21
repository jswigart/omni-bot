////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "BotBaseStates.h"
#include "BotPathing.h"
#include "Base_Messages.h"
#include "ScriptManager.h"
#include "IGameManager.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"
#include "PathPlannerBase.h"
#include "gmScriptGoal.h"
#include "gmCall.h"

//////////////////////////////////////////////////////////////////////////

namespace AiState
{
	MoveOptions::MoveOptions()
		: Radius( 32.f )
		, ThreadId( GM_INVALID_THREAD )
		, Mode( Run )
		, NumAvoid( 0 )
	{
	}

	void MoveOptions::FromTable( gmMachine *a_machine, gmTableObject *a_table )
	{
		Mode = a_table->Get( a_machine, "MoveMode" ).GetIntSafe( Run ) == Walk ? Walk : Run;
		a_table->Get( a_machine, "Radius" ).GetFloat( Radius, Radius );

		gmTableObject *avoidTable = a_table->Get( a_machine, "Avoid" ).GetTableObjectSafe();
		if ( avoidTable )
		{
			gmTableIterator tIt;
			gmTableNode *pNode = avoidTable->GetFirst( tIt );
			while ( pNode )
			{
				/*const char *valueString = pNode->m_value.GetCStringSafe();
				valueString;
				if(!Q_stricmp(valueString,"enemies"))
				{
				}
				if(!Q_stricmp(valueString,"allies"))
				{
				}*/

				pNode = avoidTable->GetNext( tIt );
			}
		}
	}

	ScriptGoal::ScriptGoal( const char *_name )
		: StateChild( _name )
		, FollowPathUser( _name )
		, mAimVector( Vector3f::ZERO )
		, mAimWeaponId( 0 )
		, mScriptAimType( Aimer::MoveDirection )
		, mScriptPriority( 0.0f )
		, mMinRadius( 0.f )
		, mParentNameHash( 0 )
		, mInsertBeforeHash( 0 )
		, mInsertAfterHash( 0 )
		, mNextGetPriorityUpdate( 0 )
		, mNextGetPriorityDelay( 0 )
		, mFinished( false )
		, mOnTargetTime( 0 )
		, mAutoReleaseAim( true )
		, mAutoReleaseWpn( true )
		, mAutoReleaseTracker( true )
		, mAutoFinishOnUnavailable( true )
		, mAutoFinishOnNoProgressSlots( true )
		, mAutoFinishOnNoUseSlots( true )
		, mSkipGetPriorityWhenActive( false )
		, mSkipLastWp( false )
	{
		SetScriptGoal( true );
	}

	ScriptGoal::~ScriptGoal()
	{
		if ( mScriptObject )
		{
			gmScriptGoal::NullifyObject( mScriptObject );
			mScriptObject = 0; //NULL;
		}
	}

	void ScriptGoal::GetDebugString( std::stringstream &out )
	{
		if ( mDebugString )
			out << mDebugString->GetString();
		else if ( mMapGoal )
			out << mMapGoal->GetName();
	}

	void ScriptGoal::RenderDebug()
	{
		/*if(IsActive())
		{
		RenderBuffer::AddLine(GetClient()->GetEyePosition(),.mMapGoal->GetPosition(),COLOR::CYAN,5.f);
		}*/
	}

	const std::string& ScriptGoal::GetSourceFile() const
	{
		return mSourceFile;
	}

	void ScriptGoal::SetSourceFile( const std::string& filePath )
	{
		mSourceFile = filePath.c_str();
	}

	void ScriptGoal::HotReloadScript()
	{
		
	}

	ScriptGoal *ScriptGoal::Clone()
	{
		ScriptGoal *pNewGoal = new ScriptGoal( GetName().c_str() );
		*pNewGoal = *this;
		pNewGoal->mScriptObject = 0; //NULL; // dont want to copy this

		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		gmTableObject *pTheirTable = gmScriptGoal::GetUserTable( GetScriptObject( pMachine ) );
		gmScriptGoal::gmBindUserObject *pObj =
			gmScriptGoal::GetUserBoundObject( pMachine, pNewGoal->GetScriptObject( pMachine ) );
		pObj->m_table = pTheirTable->Duplicate( pMachine );

		return pNewGoal;
	}

	void ScriptGoal::SetParentName( const char *_str )
	{
		mParentNameHash = Utils::MakeHash32( _str );
	}

	std::string ScriptGoal::GetParentName() const
	{
		return Utils::HashToString( GetParentNameHash() );
	}

	void ScriptGoal::SetInsertBeforeName( const char *_str )
	{
		mInsertBeforeHash = Utils::MakeHash32( _str );
	}

	std::string ScriptGoal::GetInsertBeforeName() const
	{
		return Utils::HashToString( GetInsertBeforeHash() );
	}

	void ScriptGoal::SetInsertAfterName( const char *_str )
	{
		mInsertAfterHash = Utils::MakeHash32( _str );
	}

	std::string ScriptGoal::GetInsertAfterName() const
	{
		return Utils::HashToString( GetInsertAfterHash() );
	}

	gmUserObject *ScriptGoal::GetScriptObject( gmMachine *_machine )
	{
		DisableGCInScope gcEn( _machine );

		if ( !mEventTable )
			mEventTable.Set( _machine->AllocTableObject(), _machine );
		if ( !mCommandTable )
			mCommandTable.Set( _machine->AllocTableObject(), _machine );
		if ( !mScriptObject )
			mScriptObject.Set( gmScriptGoal::WrapObject( _machine, this ), _machine );
		return mScriptObject;
	}

	void ScriptGoal::InternalExit()
	{
		State::InternalExit();
		// always kill goal threads on an exit, we may not actually be active
		// such as if we are running event threads we need to clean up
		KillAllGoalThreads();
	}

	void ScriptGoal::InternalSignal( const gmVariable &_signal )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		for ( int i = 0; i < NUM_CALLBACKS; ++i )
		{
			if ( mActiveThread[ i ] )
			{
				pMachine->Signal( _signal, mActiveThread[ i ], GM_INVALID_THREAD );
			}
		}
		for ( int i = 0; i < mNumThreads; ++i )
		{
			if ( mThreadList[ i ] != GM_INVALID_THREAD )
			{
				pMachine->Signal( _signal, mThreadList[ i ], GM_INVALID_THREAD );
			}
		}
	}

	void ScriptGoal::KillAllGoalThreads()
	{
		for ( int i = 0; i < NUM_CALLBACKS; ++i )
			mActiveThread[ i ].Kill();

		gmMachine * pM = ScriptManager::GetInstance()->GetMachine();
		for ( int i = 0; i < mNumThreads; ++i )
		{
			if ( mThreadList[ i ] != GM_INVALID_THREAD )
			{
				pM->KillThread( mThreadList[ i ] );
				mThreadList[ i ] = GM_INVALID_THREAD;
			}
		}
		mNumThreads = 0;
	}

	bool ScriptGoal::Goto( const Vector3f &_pos, const MoveOptions &options )
	{
		mSkipLastWp = false;
		mMinRadius = options.Radius;

		SetSourceThread( options.ThreadId );
		FINDSTATE( nav, Navigator, GetRootState() );
		if ( nav )
			return nav->Goto( this, _pos, options.Radius, options.Mode );
		return false;
	}

	bool ScriptGoal::GotoRandom( const MoveOptions &options )
	{
		mSkipLastWp = false;
		mMinRadius = options.Radius;

		SetSourceThread( options.ThreadId );
		FINDSTATE( nav, Navigator, GetRootState() );
		if ( nav )
		{
			return nav->GotoRandomPt( this );
		}
		return false;
	}

	void ScriptGoal::Stop()
	{
		FINDSTATEIF( Navigator, GetRootState(), Stop() );
	}

	bool ScriptGoal::RouteTo( MapGoalPtr mg, const MoveOptions &options )
	{
		if ( mg )
		{
			mMapGoalRoute = mg;
			mMinRadius = options.Radius;

			SetSourceThread( options.ThreadId );
			FINDSTATE( nav, Navigator, GetRootState() );
			if ( nav )
				return nav->Goto( this, options.Mode );
		}
		return false;
	}

	// FollowPathUser functions.
	bool ScriptGoal::GetNextDestination( DestinationVector &_desination, bool &_final, bool &_skiplastpt )
	{
		_skiplastpt = mSkipLastWp;
		if ( mMapGoalRoute && mMapGoalRoute->RouteTo( GetClient(), _desination, mMinRadius ) )
		{
			_final = false;
		}
		else
		{
			_final = true;
		}
		return true;
	}

	void ScriptGoal::OnPathSucceeded()
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		if ( GetSourceThread() == GM_INVALID_THREAD )
		{
			if ( mActiveThread[ ON_UPDATE ].IsActive() )
				pMachine->Signal( gmVariable( MSG_PATH_SUCCESS ), mActiveThread[ ON_UPDATE ].ThreadId(), GM_INVALID_THREAD );
			// TODO: send it to forked threads also?
		}
		else
		{
			pMachine->Signal( gmVariable( MSG_PATH_SUCCESS ), GetSourceThread(), GM_INVALID_THREAD );
		}
	}

	void ScriptGoal::OnPathFailed( FollowPathUser::FailType _how )
	{
		gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
		if ( GetSourceThread() == GM_INVALID_THREAD )
		{
			if ( mActiveThread[ ON_UPDATE ].IsActive() )
				pMachine->Signal( gmVariable( MSG_PATH_FAILED ), mActiveThread[ ON_UPDATE ].ThreadId(), GM_INVALID_THREAD );
			// TODO: send it to forked threads also?
		}
		else
		{
			pMachine->Signal( gmVariable( MSG_PATH_FAILED ), GetSourceThread(), GM_INVALID_THREAD );
		}
	}

	// AimerUser functions
	bool ScriptGoal::GetAimPosition( Vector3f &_aimpos )
	{
		if ( mAimWeaponId )
		{
			const MemoryRecord *pRecord = GetClient()->GetTargetingSystem()->GetCurrentTargetRecord();
			WeaponPtr w = GetClient()->GetWeaponSystem()->GetWeapon( mAimWeaponId );
			if ( pRecord != NULL && w )
			{
				_aimpos = w->GetAimPoint( Primary, pRecord->GetEntity(), pRecord->mTargetInfo );
			}
			else
				return false;
		}

		_aimpos = mAimVector;
		switch ( mScriptAimType )
		{
			case Aimer::WorldFacing:
				_aimpos = GetClient()->GetEyePosition() + mAimVector;
				break;
			case Aimer::WorldPosition:
			case Aimer::MoveDirection:
			case Aimer::UserCallback:
			default:
				break;
		}
		return true;
	}

	void ScriptGoal::OnTarget()
	{
		mOnTargetTime = IGame::GetTime();
	}

	bool ScriptGoal::OnInit( gmMachine *_machine )
	{
		//rmt_ScopedCPUSample( ScriptGoalOnInit );

		if ( mCallbacks[ ScriptGoal::ON_INIT ] )
		{
			gmVariable varThis = gmVariable( GetScriptObject( _machine ) );

			gmCall call;
			if ( call.BeginFunction( _machine, mCallbacks[ ScriptGoal::ON_INIT ], varThis ) )
			{
				call.End();

				int iReturnVal = 0;
				if ( call.GetReturnedInt( iReturnVal ) && iReturnVal == 0 )
					return false;
			}
		}
		return true;
	}

	void ScriptGoal::OnSpawn()
	{
		//rmt_ScopedCPUSample( ScriptGoalOnSpawn );

		mNextGetPriorityUpdate = 0;
		SetScriptPriority( 0.0f );
		SetLastPriority( 0.0f );

		KillAllGoalThreads();

		if ( mCallbacks[ ON_SPAWN ] )
		{
			if ( mActiveThread[ ON_SPAWN ] )
				mActiveThread[ ON_SPAWN ].Kill();

			// don't call it unless we can
			if ( CanBeSelected() != NoSelectReasonNone )
				return;

			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
			gmCall call;
			if ( call.BeginFunction( pMachine, mCallbacks[ ON_SPAWN ], gmVariable( GetScriptObject( pMachine ) ) ) )
			{
				if ( call.End() == gmThread::EXCEPTION )
				{
					SetEnable( false, va( "Error in OnSpawn Callback in Goal: %s", GetName().c_str() ).c_str() );
					return;
				}

				mActiveThread[ ON_SPAWN ] = call.GetThreadId();
				if ( call.DidReturnVariable() )
				{
					mActiveThread[ ON_SPAWN ].Reset();
				}
			}
		}
	}

	void ScriptGoal::ProcessEvent( const Message & message, CallbackParameters &_cb )
	{
		Signal( gmVariable( message.Id() ) );
	}

	void ScriptGoal::Signal( const gmVariable &_var )
	{
		InternalSignal( _var );
	}

	bool ScriptGoal::OnPathThrough( const std::string &_s )
	{
		//rmt_ScopedCPUSample( ScriptGoalOnPathThru );

		if ( mCallbacks[ ON_PATH_THROUGH ] )
		{
			//if(!mActiveThread[ON_PATH_THROUGH].IsActive())
			{
				gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
				gmCall call;
				if ( call.BeginFunction( pMachine, mCallbacks[ ON_PATH_THROUGH ], gmVariable( GetScriptObject( pMachine ) ) ) )
				{
					call.AddParamString( _s.c_str() );
					if ( call.End() == gmThread::EXCEPTION )
					{
						SetEnable( false, va( "Error in OnPathThrough Callback in Goal: %s", GetName().c_str() ).c_str() );
						return false;
					}

					int iRetVal = 0;
					if ( call.DidReturnVariable() && call.GetReturnedInt( iRetVal ) && iRetVal )
					{
						SetScriptPriority( 1.f );
						SetLastPriority( 1.f );
						return true;
					}
				}
			}
		}
		return false;
	}

	void ScriptGoal::EndPathThrough()
	{
		SetScriptPriority( 0.f );
		SetLastPriority( 0.f );
	}

	float ScriptGoal::GetPriority()
	{
		//rmt_ScopedCPUSample( ScriptGoalGetPriority );

		if ( mSkipGetPriorityWhenActive && IsActive() )
		{
			// don't call getpriority while active
		}
		else
		{
			if ( mNextGetPriorityUpdate <= IGame::GetTime() )
			{
				DelayGetPriority( mNextGetPriorityDelay );

				if ( mCallbacks[ ON_GETPRIORITY ] )
				{
					if ( !mActiveThread[ ON_GETPRIORITY ].IsActive() )
					{
						gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
						gmCall call;
						if ( call.BeginFunction( pMachine, mCallbacks[ ON_GETPRIORITY ], gmVariable( GetScriptObject( pMachine ) ) ) )
						{
							if ( call.End() == gmThread::EXCEPTION )
							{
								SetEnable( false, va( "Error in GetPriority Callback in Goal: %s", GetName().c_str() ).c_str() );
								return 0.f;
							}

							mActiveThread[ ON_GETPRIORITY ] = call.GetThreadId();
							if ( call.DidReturnVariable() )
								mActiveThread[ ON_GETPRIORITY ].Reset();
						}
					}
				}
			}
		}
		UpdateEntityInRadius();
		return mScriptPriority;
	}

	void ScriptGoal::Enter()
	{
		//Fiber * fiber = new Fiber<ScriptGoal, ScriptGoal::Enter>( this, &ScriptGoal::Enter );

		//rmt_ScopedCPUSample( ScriptGoalGetEnter );

		mActiveThread[ ON_GETPRIORITY ].Kill();

		if ( mCallbacks[ ON_ENTER ] )
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
			gmCall call;
			if ( call.BeginFunction( pMachine, mCallbacks[ ON_ENTER ], gmVariable( GetScriptObject( pMachine ) ) ) )
			{
				if ( call.End() == gmThread::EXCEPTION )
				{
					SetEnable( false, va( "Error in Enter Callback in Goal: %s", GetName().c_str() ).c_str() );
					return;
				}
				//KillAllGoalThreads();
			}
		}
		mFinished = false;
	}

	void ScriptGoal::Exit()
	{
		//rmt_ScopedCPUSample( ScriptGoalGetExit );

		SetScriptPriority( 0.0f );

		if ( mCallbacks[ ON_EXIT ] )
		{
			gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
			gmCall call;
			if ( call.BeginFunction( pMachine, mCallbacks[ ON_EXIT ], gmVariable( GetScriptObject( pMachine ) ) ) )
			{
				if ( call.End() == gmThread::EXCEPTION )
				{
					SetEnable( false, va( "Error in Exit Callback in Goal: %s", GetName().c_str() ).c_str() );
					return;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Automatically release requests on exit.
		if ( AutoReleaseAim() )
		{
			using namespace AiState;
			FINDSTATE( aim, Aimer, GetClient()->GetStateRoot() );
			if ( aim )
				aim->ReleaseAimRequest( GetNameHash() );
		}
		if ( AutoReleaseWeapon() )
		{
			using namespace AiState;
			FINDSTATE( weapsys, WeaponSystem, GetClient()->GetStateRoot() );
			if ( weapsys )
				weapsys->ReleaseWeaponRequest( GetNameHash() );
		}
		if ( AutoReleaseTracker() )
		{
			mTracker.Reset();
		}
		//////////////////////////////////////////////////////////////////////////
		ClearFinishCriteria();
		KillAllGoalThreads();

		if ( GetParent() && GetParent()->GetNameHash() == 0xd9c27485 /* HighLevel */ )
		{
			FINDSTATEIF( Navigator, GetRootState(), Stop( true ) );
		}
	}

	State::StateStatus ScriptGoal::Update( float fDt )
	{
		//rmt_ScopedCPUSample( ScriptGoalUpdate );

		if ( !mFinished )
		{
			// check the finish criteria before running the update
			for ( int i = 0; i < MaxCriteria; ++i )
			{
				if ( mFinishCriteria[ i ].mCriteria != Criteria::NONE )
				{
					if ( mFinishCriteria[ i ].Check( GetClient() ) )
						mFinished = true;
				}
			}

			// and mapgoal too
			if ( mMapGoal )
			{
				if ( mAutoFinishOnUnavailable )
				{
					if ( !mMapGoal->IsAvailable( GetClient()->GetTeam() ) )
						return State_Finished;
				}

				if ( mAutoFinishOnNoProgressSlots )
				{
					if ( !mTracker.InProgress || mTracker.InProgress != mMapGoal )
						if ( mMapGoal->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) == 0 )
							return State_Finished;
				}

				if ( mAutoFinishOnNoUseSlots )
				{
					if ( !mTracker.InUse || mTracker.InUse != mMapGoal )
						if ( mMapGoal->GetSlotsOpen( MapGoal::TRACK_INUSE ) == 0 )
							return State_Finished;
				}
			}

			UpdateMapGoalsInRadius();

			if ( !mFinished && mCallbacks[ ON_UPDATE ] )
			{
				if ( !mActiveThread[ ON_UPDATE ].IsActive() )
				{
					gmMachine *pMachine = ScriptManager::GetInstance()->GetMachine();
					gmCall call;
					if ( call.BeginFunction( pMachine, mCallbacks[ ON_UPDATE ], gmVariable( GetScriptObject( pMachine ) ) ) )
					{
						//GetClient()->AddSignalThreadId(call.GetThreadId());
						if ( call.End() == gmThread::EXCEPTION )
						{
							SetEnable( false, va( "Error in Update Callback in Goal: %s", GetName().c_str() ).c_str() );
							return State_Finished;
						}

						mActiveThread[ ON_UPDATE ] = call.GetThreadId();
						if ( call.DidReturnVariable() )
							mActiveThread[ ON_UPDATE ] = 0;
					}
				}
			}
		}
		return mFinished ? State_Finished : State_Busy;
	}

	bool ScriptGoal::MarkInProgress( MapGoalPtr _p )
	{
		mTracker.InProgress.Reset();
		if ( !_p || _p->GetSlotsOpen( MapGoal::TRACK_INPROGRESS ) > 0 )
		{
			mTracker.InProgress = _p;
			return true;
		}
		return false;
	}

	bool ScriptGoal::MarkInUse( MapGoalPtr _p )
	{
		mTracker.InUse.Reset();
		if ( !_p || _p->GetSlotsOpen( MapGoal::TRACK_INUSE ) > 0 )
		{
			mTracker.InUse = _p;
			return true;
		}
		return false;
	}

	void ScriptGoal::SetEnable( bool _enable, const char *_error /*= 0*/ )
	{
		if ( !_enable && IsActive() )
			InternalExit();
		KillAllGoalThreads();
		return State::SetEnable( _enable, _error );
	}

	void ScriptGoal::SetSelectable( bool _selectable )
	{
		if ( _selectable == IsSelectable() )
			return;

		if ( !_selectable && IsActive() )
			InternalExit();
		KillAllGoalThreads();
		State::SetSelectable( _selectable );
	}

	bool ScriptGoal::AddScriptAimRequest( Priority::ePriority _prio, Aimer::AimType _type, Vector3f _v )
	{
		mScriptAimType = _type;
		mAimVector = _v;
		FINDSTATE( aim, Aimer, GetRootState() );
		if ( aim )
		{
			mOnTargetTime = 0;

			if ( _type == Aimer::MoveDirection )
				return aim->AddAimMoveDirRequest( _prio, GetNameHash() );
			else
				return aim->AddAimRequest( _prio, this, GetNameHash() );
		}
		return false;
	}

	bool ScriptGoal::AddFinishCriteria( const CheckCriteria &_crit )
	{
		for ( int i = 0; i < MaxCriteria; ++i )
		{
			if ( mFinishCriteria[ i ].mCriteria == Criteria::NONE )
			{
				mFinishCriteria[ i ] = _crit;
				return true;
			}
		}
		return false;
	}

	void ScriptGoal::ClearFinishCriteria( bool _clearpersistent )
	{
		for ( int i = 0; i < MaxCriteria; ++i )
		{
			if ( !mFinishCriteria[ i ].mPersistent || _clearpersistent )
				mFinishCriteria[ i ] = CheckCriteria();
		}
	}

	void ScriptGoal::WatchForEntityCategory( float radius, const BitFlag32 &category, int customTrace )
	{
		mWatchEntities.mRadius = radius;
		mWatchEntities.mCategory = category;
		mWatchEntities.mCustomTrace = customTrace;

		for ( int i = 0; i < WatchEntity::MaxEntities; ++i )
		{
			mWatchEntities.mEntry[ i ].Reset();
		}
	}

	void ScriptGoal::UpdateEntityInRadius()
	{
		if ( mWatchEntities.mCategory.AnyFlagSet() && mWatchEntities.mRadius > 0.f
			&& ( AlwaysRecieveEvents() || IsActive() ) )
		{
			SensoryMemory *sensory = GetClient()->GetSensoryMemory();

			RecordHandle records[ SensoryMemory::MaxRecords ];
			const int numEnts = sensory->FindEntityByCategoryInRadius(
				mWatchEntities.mRadius,
				mWatchEntities.mCategory,
				records,
				SensoryMemory::MaxRecords );

			// figure out which ones just entered, and which ones left so we can send the appropriate events
			for ( int e = 0; e < numEnts; ++e )
			{
				const MemoryRecord *mr = sensory->GetMemoryRecord( records[ e ] );

				// does it already exist?
				bool found = false;
				int emptySlot = -1;

				bool hasLOS = true;
				if ( mWatchEntities.mCustomTrace )
				{
					hasLOS = sensory->HasLineOfSightTo( *mr, mWatchEntities.mCustomTrace );
				}

				if ( !hasLOS )
					continue;

				for ( int i = 0; i < WatchEntity::MaxEntities; ++i )
				{
					if ( mWatchEntities.mEntry[ i ].mEnt == mr->GetEntity() )
					{
						mWatchEntities.mEntry[ i ].mTimeStamp = IGame::GetTime();
						found = true;
						break;
					}
					else if ( !mWatchEntities.mEntry[ i ].mEnt.IsValid() && emptySlot == -1 )
					{
						emptySlot = i;
					}
				}

				if ( !found )
				{
					// new entity adding to list
					mWatchEntities.mEntry[ emptySlot ].mEnt = mr->GetEntity();
					mWatchEntities.mEntry[ emptySlot ].mTimeStamp = IGame::GetTime();

					EvEntEnterRadius::Msg event;
					event.mData.mEntity = mr->GetEntity();
					GetClient()->SendEvent( event );
				}
			}

			// check for anyone that needs exiting
			for ( int i = 0; i < WatchEntity::MaxEntities; ++i )
			{
				if ( mWatchEntities.mEntry[ i ].mEnt.IsValid() &&
					mWatchEntities.mEntry[ i ].mTimeStamp != IGame::GetTime() )
				{
					EvEntLeaveRadius::Msg event;
					event.mData.mEntity = mWatchEntities.mEntry[ i ].mEnt;
					GetClient()->SendEvent( event );

					mWatchEntities.mEntry[ i ].Reset();
				}
			}
		}
	}

	GameEntity ScriptGoal::IterateWatchEntity( GameEntity lastEntity )
	{
		for ( int i = 0; i < WatchEntity::MaxEntities; ++i )
		{
			if ( mWatchEntities.mEntry[ i ].mEnt.IsValid() )
			{
				if ( !lastEntity.IsValid() )
					return mWatchEntities.mEntry[ i ].mEnt;
				else if ( mWatchEntities.mEntry[ i ].mEnt == lastEntity )
					lastEntity.Reset(); // so next iteration will return result
			}
		}
		return GameEntity();
	}

	void ScriptGoal::WatchForMapGoalsInRadius( const GoalManager::Query &qry, const GameEntity & ent, float radius )
	{
		mMapGoalInRadius.mQuery = qry;
		mMapGoalInRadius.mPositionEnt = ent;
		mMapGoalInRadius.mRadius = radius;
		mMapGoalInRadius.mInRadius.clear();
	}

	void ScriptGoal::ClearWatchForMapGoalsInRadius()
	{
		mMapGoalInRadius.mPositionEnt.Reset();
		mMapGoalInRadius.mRadius = 0.f;
		mMapGoalInRadius.mQuery = GoalManager::Query();
		mMapGoalInRadius.mInRadius.clear();
	}

	void ScriptGoal::UpdateMapGoalsInRadius()
	{
		if ( mMapGoalInRadius.mPositionEnt.IsValid() )
		{
			Vector3f entPos;
			if ( !EngineFuncs::EntityPosition( mMapGoalInRadius.mPositionEnt, entPos ) )
			{
				mMapGoalInRadius.mPositionEnt.Reset();
				return;
			}

			gmMachine * pMachine = ScriptManager::GetInstance()->GetMachine();

			mMapGoalInRadius.mQuery.CheckInRadius( entPos, mMapGoalInRadius.mRadius );
			System::mInstance->mGoalManager->GetGoals( mMapGoalInRadius.mQuery );

			MgSet newInRadius;
			MapGoalList::iterator it = mMapGoalInRadius.mQuery.mList.begin(),
				itEnt = mMapGoalInRadius.mQuery.mList.end();
			for ( ; it != itEnt; ++it )
			{
				newInRadius.insert( *it );
			}

			// look for all mapgoals that have left the radius or are still in
			for ( MgSet::iterator setIt = mMapGoalInRadius.mInRadius.begin();
				setIt != mMapGoalInRadius.mInRadius.end();
				)
			{
				if ( newInRadius.find( *setIt ) == newInRadius.end() )
				{
					EvEntLeaveRadius::Msg event;
					CallbackParameters cb( event.Id(), pMachine );
					cb.AddUserObj( "MapGoal", ( *setIt )->GetScriptObject( pMachine ) );
					InternalProcessEvent( event, cb );

					MapGoalPtr mg = *setIt;
					++setIt;
					mMapGoalInRadius.mInRadius.erase( mg );
				}
				else
				{
					// so we can trim it down to only new ones
					newInRadius.erase( *setIt );

					// still in radius
					++setIt;
				}
			}

			// only ones left are new ones!
			for ( MgSet::iterator newIt = newInRadius.begin();
				newIt != newInRadius.end();
				++newIt )
			{
				mMapGoalInRadius.mInRadius.insert( *newIt );
				
				EvEntEnterRadius::Msg event;
				CallbackParameters cb( event.Id(), pMachine );
				cb.AddUserObj( "MapGoal", ( *newIt )->GetScriptObject( pMachine ) );
				InternalProcessEvent( event, cb );
			}
		}
	}
};