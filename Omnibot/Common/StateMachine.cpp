////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Omni-Bot_BitFlags.h"
#include "StateMachine.h"
#include "BotTargetingSystem.h"
#include "BotWeaponSystem.h"
#include "BlackBoardItems.h"
#include "gmCall.h"

State::State( const char * _name, const UpdateDelay &_ur )
	: mParent( 0 )
	, mRoot( 0 )
	, mClient( 0 )
	, mNextUpdate( 0 )
	, mLastUpdateTime( 0 )
	, mStateTime( 0.f )
	, mStateTimeUser( 0.f )
	, mLastPriority( 0.f )
	, mLastPriorityTime( 0 )
	, mUpdateRate( _ur )
	, mNameHash( 0 )
	, mDebugIcon( Ico_Default )
	, mSyncCrc( 0 )
	, mNumThreads( 0 )
{
	SetName( _name );
	DebugExpand( true );

	for ( int i = 0; i < MaxThreads; ++i )
		mThreadList[ i ] = GM_INVALID_THREAD;
}

State::~State()
{
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		delete mChildren[ i ];
		mChildren[ i ] = null;
	}
	mChildren.resize( 0 );
}

State* State::GetParent()
{
	return mParent;
}

State* State::GetRootState()
{
	return mRoot;
}

State* State::GetParent() const
{
	return mParent;
}

State* State::GetRootState() const
{
	return mRoot;
}

void State::SetName( const char *_name )
{
	mNameHash = Utils::MakeHash32( _name );
#ifdef _DEBUG
	mDebugName = _name ? _name : "";
#endif
}

std::string State::GetName() const
{
	return Utils::HashToString( GetNameHash() );
}

State * State::AppendState( State *_state )
{
	_state->mParent = this;

	mChildren.push_back( _state );

	return _state;
}

bool State::AppendTo( const char * _name, State *_insertstate )
{
	return AppendTo( Utils::Hash32( _name ), _insertstate );
}

bool State::AppendTo( uint32_t _name, State *_insertstate )
{
	if ( !_name )
		return false;

	State *pFoundState = FindState( _name );
	if ( pFoundState )
	{
		pFoundState->AppendState( _insertstate );
		_insertstate->mRoot = pFoundState->mRoot;
		return true;
	}
	return false;
}

void State::PrependState( State *_state )
{
	_state->mParent = this;
	mChildren.insert( mChildren.begin(), _state );
}

bool State::PrependTo( const char * _name, State *_insertstate )
{
	return PrependTo( Utils::Hash32( _name ), _insertstate );
}

bool State::PrependTo( uint32_t _name, State *_insertstate )
{
	if ( !_name )
		return false;

	State *pFoundState = FindState( _name );
	if ( pFoundState )
	{
		pFoundState->PrependState( _insertstate );
		_insertstate->mRoot = pFoundState->mRoot;
		return true;
	}
	return false;
}

State *State::ReplaceState( const char * _name, State *_insertstate )
{
	State *pReplaceState = FindState( _name );
	if ( pReplaceState )
	{
		for ( size_t i = 0; i < pReplaceState->mParent->mChildren.size(); ++i )
		{
			State* state = pReplaceState->mParent->mChildren[ i ];

			if ( state == pReplaceState )
			{
				_insertstate->mRoot = state->mRoot;
				_insertstate->mParent = state->mParent;

				pReplaceState->mParent->mChildren[ i ] = _insertstate;

				return state;
			}
		}
	}
	return _insertstate;
}

bool State::InsertAfter( const char * _name, State *_insertstate )
{
	return InsertAfter( Utils::Hash32( _name ), _insertstate );
}

bool State::InsertAfter( uint32_t _name, State *_insertstate )
{
	if ( !_name )
		return false;

	State *pFoundState = FindState( _name );
	if ( pFoundState )
	{
		_insertstate->mRoot = pFoundState->mRoot;
		_insertstate->mParent = pFoundState->mParent;

		for ( size_t i = 0; i < _insertstate->mParent->mChildren.size(); ++i )
		{
			State* state = _insertstate->mParent->mChildren[ i ];
			if ( state == pFoundState )
			{
				_insertstate->mParent->mChildren.insert( _insertstate->mParent->mChildren.begin() + i + 1, _insertstate );
				return true;
			}
		}
		return true;
	}
	return false;
}

bool State::InsertBefore( const char * _name, State *_insertstate )
{
	return InsertBefore( Utils::Hash32( _name ), _insertstate );
}

bool State::InsertBefore( uint32_t _name, State *_insertstate )
{
	if ( !_name )
		return false;

	State *pFoundState = FindState( _name );
	if ( pFoundState )
	{
		_insertstate->mRoot = pFoundState->mRoot;
		_insertstate->mParent = pFoundState->mParent;

		for ( size_t i = 0; i < _insertstate->mParent->mChildren.size(); ++i )
		{
			State* state = _insertstate->mParent->mChildren[ i ];
			if ( state == pFoundState )
			{
				_insertstate->mParent->mChildren.insert( _insertstate->mParent->mChildren.begin() + i, _insertstate );
				return true;
			}
		}
	}
	return false;
}

State *State::RemoveState( const char * _name )
{
	State *pDeleteState = FindState( _name );
	if ( pDeleteState )
	{
		pDeleteState->InternalExit();

		pDeleteState->mParent->mChildren.erase( std::remove( pDeleteState->mParent->mChildren.begin(), pDeleteState->mParent->mChildren.end(), pDeleteState ) );
		pDeleteState->mParent = null;
	}
	return pDeleteState;
}

void State::InitializeStates()
{
	Initialize();
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->InitializeStates();
}

void State::FixRoot()
{
	// Find the root state
	mRoot = GetParent();
	while ( mRoot != NULL && mRoot->mParent )
		mRoot = mRoot->mParent;

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->FixRoot();
}

void State::SetClient( Client *_client )
{
	mClient = _client;

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->SetClient( _client );
}

State *State::FindState( const char *_name )
{
	return FindState( Utils::Hash32( _name ) );
}

State *State::FindStateRecurse( uint32_t _namehash )
{
	if ( mNameHash == _namehash )
		return this;

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		State* foundState = ( *it )->FindStateRecurse( _namehash );
		if ( foundState != NULL )
			return foundState;
	}
	return NULL;
}

State *State::FindState( uint32_t _namehash )
{
	return FindStateRecurse( _namehash );
}

void State::RootUpdate()
{
	rmt_ScopedCPUSample( RootUpdate );
	if ( !IsActive() )
		InternalEnter();
	InternalUpdateState();
}

float State::InternalGetPriority()
{
	if ( mLastPriorityTime < IGame::GetTime() )
	{
		const noSelectReason_t rsn = CanBeSelected();
		SetSelectable( rsn == NoSelectReasonNone );
		mLastPriority =
			!IsDisabled() //&&
			/*IsSelectable() &&
			!IsUserDisabled() &&*/
			//(GetCurrentPriority()!=0.f)
			? GetPriority() : 0.f;
		mLastPriorityTime = IGame::GetTime();
	}
	return mLastPriority;
}

void State::InternalEnter()
{
	//Utils::OutputDebug(kInfo,"%s: State: %s Enter (%d)\n", GetClient()->GetName(), GetName().c_str(),IGame::GetTime());

	if ( mLimitCallback.mOnlyWhenActive ) mLimitCallback.mResult = true;

	mStateTime = mStateTimeUser = IGame::GetTimeSecs();
	mStateFlags.SetFlag( State_Active, true );
	Enter();

	if ( mStateFlags.CheckFlag( State_DebugExpandOnActive ) )
		DebugExpand( true );
}

void State::InternalExit()
{
	if ( IsActive() )
	{
		//Utils::OutputDebug(kInfo,"%s: State: %s Exit (%d)\n", GetClient()->GetName(), GetName().c_str(),IGame::GetTime());

		// exit all child states
		for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
			( *it )->InternalExit();

		mStateTime = mStateTimeUser = 0.f;
		SetLastPriority( 0.0f );
		mStateFlags.SetFlag( State_Active, false );

		InternalParentExit();
		Exit();

		if ( mStateFlags.CheckFlag( State_DebugExpandOnActive ) )
			DebugExpand( false );
	}
}

void State::InternalParentExit()
{
}

State::StateStatus State::InternalUpdateState()
{
	if ( DebugDrawingEnabled() )
		RenderDebug();

	if ( mNextUpdate <= IGame::GetTime() )
	{
		const int iMsPassed = IGame::GetTime() - mLastUpdateTime;
		float fDt = (float)iMsPassed / 1000.f;

		mNextUpdate = IGame::GetTime() + mUpdateRate.GetDelayMsec();
		mLastUpdateTime = IGame::GetTime();

		return UpdateState( fDt );
	}
	return State_Busy;
}

void State::ExitAll()
{
	InternalExit();
}

void State::SignalThreads( const gmVariable &_signal )
{
	if ( !IsRoot() && !IsActive() && !mStateFlags.CheckFlag( State_AlwaysRecieveSignals ) )
		return;

	rmt_ScopedCPUSample( SignalThreads );

	InternalSignal( _signal );
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->SignalThreads( _signal );
}

void State::CheckForCallbacks( const Message & message, CallbackParameters &_cb )
{
	if ( IsRoot() || IsActive() || AlwaysRecieveEvents() )
		InternalProcessEvent( message, _cb );

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->CheckForCallbacks( message, _cb );
}

void State::InternalSignal( const gmVariable &_signal )
{
}

void State::InternalProcessEvent( const Message & message, CallbackParameters &_cb )
{
	SignalThreads( gmVariable( message.Id() ) );

	/*switch(message.GetMessageId())
	{
	HANDLER(SYSTEM_THREAD_CREATED)
	{
	const Event_SystemThreadCreated *m = message.Get<Event_SystemThreadCreated>();
	break;
	}
	HANDLER(SYSTEM_THREAD_DESTROYED)
	{
	const Event_SystemThreadDestroyed *m = message.Get<Event_SystemThreadDestroyed>();
	RemoveThreadReference(m->mThreadId);
	break;
	}
	}*/

	// Attempt script callbacks.
	const bool eventRelevent = _cb.GetTargetState() == 0 || _cb.GetTargetState() == GetNameHash();
	if ( mEventTable && eventRelevent )
	{
		gmTableIterator tIt;
		gmTableNode *pNode = mEventTable->GetFirst( tIt );
		while ( pNode )
		{
			const size_t bufferSize = 1024;
			char keyBuffer[ bufferSize ] = {};
			char valBuffer[ bufferSize ] = {};
			const char* keyInfo = pNode->m_key.AsStringWithType( ScriptManager::GetInstance()->GetMachine(), keyBuffer, bufferSize );
			const char* valInfo = pNode->m_value.AsStringWithType( ScriptManager::GetInstance()->GetMachine(), valBuffer, bufferSize );

			LOG( "Event: " << keyInfo << " : " << valInfo );

			pNode = mEventTable->GetNext( tIt );
		}

		gmVariable callback = mEventTable->Get( _cb.GetMessageId() );
		if ( gmFunctionObject *pFunc = callback.GetFunctionObjectSafe() )
		{
			gmVariable varThis = gmVariable( GetScriptObject( _cb.GetMachine() ) );
			int ThreadId = _cb.CallFunction( pFunc, varThis, !_cb.CallImmediate() );

			// add it to the tracking list for management of its lifetime.
			// don't add thread if AlwaysRecieveEvents=true, because events REVIVED, CHANGETEAM, DEATH would not be executed
			if ( ThreadId != GM_INVALID_THREAD && !AlwaysRecieveEvents() )
				AddForkThreadId( ThreadId );
		}
	}
	ProcessEvent( message, _cb );
}

void State::AddForkThreadId( int _threadId )
{
	int freeIndex = -1;
	for ( int i = 0; i < mNumThreads; ++i )
	{
		if ( mThreadList[ i ] == GM_INVALID_THREAD )
		{
			if ( freeIndex == -1 )
				freeIndex = i;
			continue;
		}
		if ( mThreadList[ i ] == _threadId )
			return;
	}
	if ( freeIndex < 0 && mNumThreads < MaxThreads )
	{
		freeIndex = mNumThreads++;
	}
	if ( freeIndex != -1 )
	{
		mThreadList[ freeIndex ] = _threadId;
	}
}

void State::ClearThreadReference( int index )
{
	mThreadList[ index ] = GM_INVALID_THREAD;
	if ( index == mNumThreads - 1 )
	{
		do
		{
			mNumThreads--;
		}
		while ( mNumThreads > 0 && mThreadList[ mNumThreads - 1 ] == GM_INVALID_THREAD );
	}
}

bool State::DeleteForkThread( int _threadId )
{
	gmMachine * pM = ScriptManager::GetInstance()->GetMachine();
	for ( int i = 0; i < mNumThreads; ++i )
	{
		if ( mThreadList[ i ] == _threadId )
		{
			pM->KillThread( _threadId );
			ClearThreadReference( i );
			return true;
		}
	}
	return false;
}

bool State::RemoveThreadReference( const int * _threadId, int _numThreadIds )
{
	bool b = false;
	for ( int t = 0; t < _numThreadIds; ++t )
	{
		int id = _threadId[ t ];
		for ( int i = 0; i < mNumThreads; ++i )
		{
			if ( mThreadList[ i ] == id )
			{
				ClearThreadReference( i );
				b = true;
				break;
			}
		}
	}
	return b;
}

void State::PropogateDeletedThreads( const int *_threadIds, int _numThreads )
{
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		( *it )->PropogateDeletedThreads( _threadIds, _numThreads );

	RemoveThreadReference( _threadIds, _numThreads );
}

bool State::StateCommand( const StringVector & args )
{
	bool handled = false;
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
		handled |= ( *it )->StateCommand( args );

	if ( mCommandTable )
	{
		gmMachine * pMachine = ScriptManager::GetInstance()->GetMachine();
		gmVariable varThis = gmVariable::s_null;
		gmUserObject * pScriptObject = GetScriptObject( pMachine );
		if ( pScriptObject )
			varThis.SetUser( pScriptObject );

		ScriptCommandExecutor cmdExec( pMachine, mCommandTable );
		if ( cmdExec.Exec( args, varThis ) )
			handled |= true;
	}
	return handled;
}

void State::OnSpawn()
{
	SetLastPriority( 0.0f );

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( !( *it )->IsUserDisabled() )
			( *it )->OnSpawn();
	}
}

void State::SetSelectable( bool _selectable )
{
	mStateFlags.SetFlag( State_UnSelectable, !_selectable );
}

void State::SetEnable( bool _enable, const char *_error )
{
	if ( _error )
	{
		LOGERR( _error );
	}
	mStateFlags.SetFlag( State_UserDisabled, !_enable );
}

State::noSelectReason_t State::CanBeSelected()
{
	if ( mOnlyClass.AnyFlagSet() && !mOnlyClass.CheckFlag( GetClient()->GetClass() ) )
		return NoSelectReason_OnlyClass;
	if ( mOnlyTeam.AnyFlagSet() && !mOnlyTeam.CheckFlag( GetClient()->GetTeam() ) )
		return NoSelectReason_OnlyTeam;
	if ( mOnlyPowerUp.AnyFlagSet() && !( mOnlyPowerUp & GetClient()->GetPowerUpFlags() ).AnyFlagSet() )
		return NoSelectReason_OnlyPowerup;
	if ( mOnlyNoPowerUp.AnyFlagSet() && ( mOnlyNoPowerUp & GetClient()->GetPowerUpFlags() ).AnyFlagSet() )
		return NoSelectReason_OnlyNoPowerup;
	if ( mOnlyEntFlag.AnyFlagSet() && !( mOnlyEntFlag & GetClient()->GetEntityFlags() ).AnyFlagSet() )
		return NoSelectReason_OnlyEntFlag;
	if ( mOnlyNoEntFlag.AnyFlagSet() && ( mOnlyNoEntFlag & GetClient()->GetEntityFlags() ).AnyFlagSet() )
		return NoSelectReason_OnlyNoEntFlag;
	if ( mOnlyRole.AnyFlagSet() && !( mOnlyRole & GetClient()->GetRoleMask() ).AnyFlagSet() )
		return NoSelectReason_OnlyRole;

	if ( mOnlyWeapon.AnyFlagSet() )
	{
		AiState::WeaponSystem *ws = GetClient()->GetWeaponSystem();

		BitFlag128 hasWeapons = ( mOnlyWeapon & ws->GetWeaponMask() );
		if ( !hasWeapons.AnyFlagSet() )
			return NoSelectReason_OnlyWeapon;

		bool bOutOfAmmo = true;
		for ( int i = 0; i < 128; ++i )
		{
			if ( hasWeapons.CheckFlag( i ) )
			{
				WeaponPtr w = ws->GetWeapon( i );
				if ( w )
				{
					w->UpdateAmmo();

					if ( w->OutOfAmmo() == InvalidFireMode )
					{
						bOutOfAmmo = false;
						break;
					}
				}
			}
		}
		if ( bOutOfAmmo )
			return NoSelectReason_OnlyWeaponNoAmmo;
	}

	if ( mOnlyTargetClass.AnyFlagSet() ||
		mOnlyTargetTeam.AnyFlagSet() ||
		mOnlyTargetPowerUp.AnyFlagSet() ||
		mOnlyTargetEntFlag.AnyFlagSet() ||
		mOnlyTargetNoPowerUp.AnyFlagSet() ||
		mOnlyTargetNoEntFlag.AnyFlagSet() ||
		mOnlyTargetWeapon.AnyFlagSet() )
	{
		AiState::TargetingSystem *ts = GetClient()->GetTargetingSystem();
		if ( ts )
		{
			const MemoryRecord *target = ts->GetCurrentTargetRecord();
			const bool NoTargetAllowed = mOnlyTargetClass.CheckFlag( 0 );

			if ( !target && NoTargetAllowed )
			{
				// noop
			}
			else if ( !target )
				return NoSelectReason_OnlyTarget;
			else
			{
				if ( mOnlyTargetClass.AnyFlagSet() )
				{
					if ( mOnlyTargetClass.CheckFlag( FilterSensory::ANYPLAYERCLASS ) &&
						target->mTargetInfo.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
					{
						// success
					}
					else if ( !mOnlyTargetClass.CheckFlag( target->mTargetInfo.mEntInfo.mClassId ) )
						return NoSelectReason_OnlyTargetClass;
				}

				if ( mOnlyTargetTeam.AnyFlagSet() )
				{
					if ( !mOnlyTargetTeam.CheckFlag( gEngineFuncs->GetEntityTeam( target->GetEntity() ) ) )
						return NoSelectReason_OnlyTargetTeam;
				}

				// only if target has powerup
				if ( mOnlyTargetPowerUp.AnyFlagSet() )
				{
					if ( ( target->mTargetInfo.mEntInfo.mPowerUps & mOnlyTargetPowerUp ) != mOnlyTargetPowerUp )
					{
						return NoSelectReason_OnlyTargetPowerup;
					}
				}

				// only if target doesn't have powerup
				if ( mOnlyTargetNoPowerUp.AnyFlagSet() )
				{
					if ( ( target->mTargetInfo.mEntInfo.mPowerUps & mOnlyTargetNoPowerUp ).AnyFlagSet() )
					{
						return NoSelectReason_OnlyTargetNoPowerup;
					}
				}

				// only if target has ent flag
				if ( mOnlyTargetEntFlag.AnyFlagSet() )
				{
					if ( ( target->mTargetInfo.mEntInfo.mFlags & mOnlyTargetEntFlag ) != mOnlyTargetEntFlag )
					{
						return NoSelectReason_OnlyTargetEntFlag;
					}
				}

				// only if target doesn't have ent flag
				if ( mOnlyTargetNoEntFlag.AnyFlagSet() )
				{
					if ( ( target->mTargetInfo.mEntInfo.mFlags & mOnlyTargetNoEntFlag ).AnyFlagSet() )
					{
						return NoSelectReason_OnlyTargetNoEntFlag;
					}
				}

				// only if target has one of these weapons
				if ( mOnlyTargetWeapon.AnyFlagSet() )
				{
					if ( !mOnlyTargetWeapon.CheckFlag( target->mTargetInfo.mCurrentWeapon ) )
						return NoSelectReason_OnlyTargetWeapon;
				}
			}
		}
	}
	if ( mLimitCallback.mLimitTo )
	{
		if ( !mLimitCallback.mOnlyWhenActive || IsActive() )
		{
			if ( mLimitCallback.mNextCallback <= IGame::GetTime() )
			{
				mLimitCallback.mNextCallback = IGame::GetTime() + mLimitCallback.mDelay;

				gmMachine *pM = ScriptManager::GetInstance()->GetMachine();

				gmCall call;
				if ( call.BeginFunction( pM, mLimitCallback.mLimitTo, mLimitCallback.mThis ) )
				{
					call.End();

					int iCancel = 0;
					if ( call.GetReturnedInt( iCancel ) )
						mLimitCallback.mResult = iCancel != 0;
				}
			}

			if ( !mLimitCallback.mResult )
				return NoSelectReason_LimitCallback;
		}
	}

	return NoSelectReasonNone;
}

void State::LimitTo( const gmVariable &varThis, gmGCRoot<gmFunctionObject> &_fn, int _delay, bool _onlywhenactive )
{
	mLimitCallback.mThis = varThis;
	mLimitCallback.mLimitTo = _fn;
	mLimitCallback.mDelay = _delay;
	mLimitCallback.mOnlyWhenActive = _onlywhenactive;
	mLimitCallback.mNextCallback = IGame::GetTime() + mLimitCallback.mDelay;
}

void State::ClearLimitTo()
{
	mLimitCallback = LimitToCallback();
}

void State::BlackboardDelay( float _delayseconds, int _targetId )
{
	enum
	{
		NumDelays = 4
	};
	BBRecordPtr delays[ NumDelays ];
	int n = GetClient()->GetBB().GetBBRecords( bbk_DelayGoal, delays, NumDelays );
	for ( int i = 0; i < n; ++i )
	{
		if ( delays[ i ]->mTarget == _targetId &&
			delays[ i ]->mOwner == GetClient()->GetGameID() )
		{
			delays[ i ]->mExpireTime = IGame::GetTime() + Utils::SecondsToMilliseconds( _delayseconds );
			return;
		}
	}

	BBRecordPtr bbp( new bbDelayGoal );
	bbp->mOwner = GetClient()->GetGameID();
	bbp->mTarget = _targetId;
	bbp->mExpireTime = IGame::GetTime() + Utils::SecondsToMilliseconds( _delayseconds );
	bbp->mDeleteOnExpire = true;
	GetClient()->GetBB().PostBBRecord( bbp );
}

bool State::BlackboardIsDelayed( int _targetId )
{
	return GetClient()->GetBB().GetNumBBRecords( bbk_DelayGoal, _targetId ) > 0;
}

//////////////////////////////////////////////////////////////////////////
// Debug

void State::RecursiveRenderDebug()
{
	if ( DebugDrawingEnabled() )
		RenderDebug();

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[i]->RecursiveRenderDebug();
	}
}


#ifdef ENABLE_DEBUG_WINDOW
void State::RenderDebugWindow( gcn::DrawInfo drawinfo )
{
	if ( DontDrawDebugWindow() )
		return;

	// Draw the title.
	if ( GetParent() )
	{
		const char *prefix = "  ";
		if ( .mFirstChild )
			prefix = IsDebugExpanded() ? "- " : "+ ";

		gcn::Color renderColor = IsActive() || .mLastUpdateTime == IGame::GetTime() ? gcn::Color( 0, 160, 0 ) : gcn::Color( 0, 0, 0 );
		if ( .mStateFlags.CheckFlag( State_UnSelectable ) )
			renderColor = gcn::Color( 255, 255, 255 );
		if ( IsDisabled() )
			renderColor = gcn::Color( 255, 0, 0 );

		const int fontHeight = drawinfo.mWidget->getFont()->getHeight();
		int iX = drawinfo.mIndent * 12;
		int iY = drawinfo.mLine * fontHeight;

		gcn::Rectangle l( 0, iY, drawinfo.mWidget->getWidth(), fontHeight );

		if ( l.isPointInRect( drawinfo.Mouse.X, drawinfo.Mouse.Y ) )
		{
			renderColor.a = 128;

			if ( drawinfo.Mouse.MouseClicked[ gcn::MouseCache::Right ] )
				DebugExpand( !IsDebugExpanded() );
			if ( drawinfo.Mouse.MouseClicked[ gcn::MouseCache::Left ] )
				ShowStateWindow( GetNameHash() );
			if ( drawinfo.Mouse.MouseClicked[ gcn::MouseCache::Mid ] )
				DebugDraw( !DebugDrawingEnabled() );
		}

		drawinfo.mGraphics->setColor( renderColor );
		drawinfo.mGraphics->fillRectangle( l );
		drawinfo.mGraphics->setColor( drawinfo.mWidget->getForegroundColor() );

		std::stringstream s;
		GetDebugString( s );

		drawinfo.mGraphics->drawText( ( std::string )va( "%s %s (%.2f) - %s:",
			prefix, GetName().c_str(),
			GetLastPriority(),
			s.str().c_str() ), iX, iY );

		++drawinfo.mLine;

		if ( IsDebugExpanded() )
		{
			for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
				pState->RenderDebugWindow( drawinfo.indent() );
		}
		return;
	}

	if ( IsDebugExpanded() || !GetParent() )
	{
		for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
			pState->RenderDebugWindow( drawinfo.indent() );
	}
}
#endif

#ifdef ENABLE_REMOTE_DEBUGGING
void State::Sync( RemoteLib::DebugConnection * connection, Remote::Behavior & cached, Remote::Behavior & update )
{
	//std::string pth = va( "%s/%s", statePath, GetName().c_str() );

	std::stringstream s;
	GetDebugString( s );
	obColor renderColor = IsActive() || .mLastUpdateTime == IGame::GetTime() ? obColor( 0, 160, 0 ) : obColor( 0, 0, 0 );
	if ( .mStateFlags.CheckFlag( State_UnSelectable ) )
		renderColor = obColor( 255, 255, 255 );
	if ( IsDisabled() )
		renderColor = obColor( 255, 0, 0 );

	SET_IF_DIFF( cached, update, GetName().c_str(), name );
	SET_IF_DIFF( cached, update, (int)renderColor, color );
	SET_IF_DIFF( cached, update, s.str().c_str(), info );

	int childIndex = 0;
	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
	{
		if ( cached.children().Capacity() <= childIndex )
		{
			cached.mutable_children()->Reserve( childIndex + 1 );
		}

		while ( cached.children_size() <= childIndex )
		{
			cached.add_children();
		}

		if ( update.children().Capacity() <= childIndex )
		{
			update.mutable_children()->Reserve( childIndex + 1 );
		}

		while ( update.children_size() <= childIndex )
		{
			update.add_children();
		}

		Remote::Behavior * cachedChild = cached.mutable_children( childIndex );
		Remote::Behavior * updateChild = update.mutable_children( childIndex );
		pState->Sync( connection, *cachedChild, *updateChild );
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

StateSimultaneous::StateSimultaneous( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
{
}

float StateSimultaneous::GetPriority()
{
	//State *pBestState = NULL;
	float fBestPriority = 0.f;
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		State* state = mChildren[ i ];
		if ( state->IsUserDisabled() )
			continue;

		float fPriority = state->InternalGetPriority();
		if ( fPriority > fBestPriority )
		{
			fBestPriority = fPriority;
			//pBestState = pState;
		}
	}
	return fBestPriority;
}

State::StateStatus StateSimultaneous::UpdateState( float fDt )
{
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		State* state = mChildren[ i ];

		bool bWantsActive = !state->IsDisabled() ? state->InternalGetPriority() > 0.0 : false;

		// Handle exits
		if ( state->IsActive() && ( !bWantsActive || state->IsDisabled() ) )
		{
			state->InternalExit(); // call internal version
			if ( !bWantsActive && state->CheckFlag( State_DeleteOnFinished ) )
			{
				delete mChildren[ i ];
				mChildren.erase( mChildren.begin() + i );
			}
			continue;
		}

		if ( !state->IsActive() && bWantsActive )
			state->InternalEnter(); // call internal version

		if ( state->IsActive() )
		{
			if ( state->InternalUpdateState() == State_Finished )
				state->InternalExit();
		}
	}
	Update( fDt );
	return State_Busy;
}

//////////////////////////////////////////////////////////////////////////

StateFirstAvailable::StateFirstAvailable( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
	, mCurrentState( 0 )
{
}

void StateFirstAvailable::GetDebugString( std::stringstream &out )
{
	if ( mCurrentState )
		mCurrentState->GetDebugString( out );
}

float StateFirstAvailable::GetPriority()
{
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( ( *it )->IsUserDisabled() )
			continue;

		float fPriority = ( *it )->InternalGetPriority();
		if ( fPriority > 0.f )
			return fPriority;
	}
	return 0.f;
}

void StateFirstAvailable::InternalParentExit()
{
	if ( mCurrentState && mCurrentState->IsActive() )
		mCurrentState->InternalExit();
	mCurrentState = 0;
}

State::StateStatus StateFirstAvailable::UpdateState( float fDt )
{
	State *pBestState = NULL;
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( ( *it )->IsUserDisabled() )
			continue;

		float fPriority = ( *it )->InternalGetPriority();
		if ( fPriority > 0.f )
		{
			pBestState = ( *it );
			break;
		}
	}

	// Exit active states that are not our best
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( pBestState != ( *it ) && ( *it )->IsActive() )
		{
			( *it )->InternalExit();

			if ( mCurrentState == ( *it ) )
				mCurrentState = 0;
		}
	}

	if ( pBestState && mCurrentState != pBestState )
	{
		mCurrentState = pBestState;
		mCurrentState->InternalEnter();
	}

	if ( mCurrentState )
	{
		if ( mCurrentState->InternalUpdateState() == State_Finished )
		{
			mCurrentState->InternalExit();
			mCurrentState = 0;
		}
	}

	Update( fDt );

	return mCurrentState || InternalGetPriority() > 0.f ? State_Busy : State_Finished;
}

//////////////////////////////////////////////////////////////////////////

StatePrioritized::StatePrioritized( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
	, mCurrentState( 0 )
{
}

void StatePrioritized::GetDebugString( std::stringstream &out )
{
	if ( mCurrentState )
		mCurrentState->GetDebugString( out );
}

float StatePrioritized::GetPriority()
{
	//State *pBestState = NULL;
	float fBestPriority = 0.f;
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( ( *it )->IsUserDisabled() )
			continue;

		float fPriority = ( *it )->InternalGetPriority();
		if ( fPriority > fBestPriority )
		{
			fBestPriority = fPriority;
			//pBestState = pState;
		}
	}
	return fBestPriority;
}

void StatePrioritized::InternalParentExit()
{
	if ( mCurrentState && mCurrentState->IsActive() )
		mCurrentState->InternalExit();
	mCurrentState = 0;
}

State::StateStatus StatePrioritized::UpdateState( float fDt )
{
	State *pBestState = NULL;
	float fBestPriority = 0.f;
	int iBestRand = 0;

#ifdef _DEBUG
	const int N = 64; // cs: was 32
	float STATES_PRIO[ N ] = {};
	State *STATES_P[ N ] = {}; int NumPriorities = 0;
	State *STATES_X[ N ] = {}; int NumExited = 0;
	State *OLD_BEST = mCurrentState; OLD_BEST;
	const std::string& BOT_NAME = GetClient() ? GetClient()->GetName() : ""; BOT_NAME;
#endif

	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( ( *it )->IsUserDisabled() )
			continue;

		float fPriority = ( *it )->InternalGetPriority();

#ifdef _DEBUG
		STATES_PRIO[ NumPriorities ] = fPriority;
		STATES_P[ NumPriorities ] = ( *it );
		NumPriorities++;
#endif

		if ( fPriority >= fBestPriority )
		{
			if ( fPriority > fBestPriority )
			{
				fBestPriority = fPriority;
				pBestState = ( *it );
				iBestRand = 0;
			}
			else if ( fPriority > 0 )
			{
				int iRand = rand();
				if ( iBestRand == 0 ) iBestRand = rand();
				if ( iRand > iBestRand )
				{
					iBestRand = iRand;
					pBestState = ( *it );
				}
			}
		}
	}

	// if the current state has an equal priority to the 'best', the current
	// state has the edge, to prevent order dependency causing goals to override
	// on equal priorities
	if ( mCurrentState )
	{
		if ( mCurrentState->GetLastPriority() >= fBestPriority )
		{
			pBestState = mCurrentState;
		}
	}

	// Exit active states that are not our best
	for ( StateList::iterator it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		if ( pBestState != ( *it ) && ( *it )->IsActive() )
		{
#ifdef _DEBUG
			STATES_X[ NumExited++ ] = ( *it );
#endif
			( *it )->InternalExit();
		}
	}

	if ( pBestState && mCurrentState != pBestState )
	{
		mCurrentState = pBestState;
		mCurrentState->InternalEnter();
	}

	if ( mCurrentState )
	{
		if ( mCurrentState->InternalUpdateState() == State_Finished )
		{
			mCurrentState->InternalExit();
			mCurrentState = 0;
		}
	}

	Update( fDt );

	return mCurrentState || InternalGetPriority() > 0.f ? State_Busy : State_Finished;
}

//////////////////////////////////////////////////////////////////////////

StateSequential::StateSequential( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
	, mCurrentState( 0 )
{
}

void StateSequential::GetDebugString( std::stringstream &out )
{
	if ( mCurrentState )
		mCurrentState->GetDebugString( out );
}

void StateSequential::Exit()
{
	if ( mCurrentState && mCurrentState->IsActive() )
		mCurrentState->InternalExit();
	mCurrentState = 0;
}

State::StateStatus StateSequential::UpdateState( float fDt )
{
	/*if(.mCurrentState)
	{
	if(.mStateList.front()->IsActive())
	return mStateList.front()->InternalUpdateState();
	}*/
	return State_Busy;
}

//////////////////////////////////////////////////////////////////////////

StateSequentialLooping::StateSequentialLooping( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
{
}

State::StateStatus StateSequentialLooping::UpdateState( float fDt )
{
	return State_Busy;
}

//////////////////////////////////////////////////////////////////////////

StateProbabilistic::StateProbabilistic( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
{
}

State::StateStatus StateProbabilistic::UpdateState( float fDt )
{
	return State_Busy;
}

//////////////////////////////////////////////////////////////////////////

StateOneOff::StateOneOff( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
{
}

State::StateStatus StateOneOff::UpdateState( float fDt )
{
	return State_Busy;
}

//////////////////////////////////////////////////////////////////////////

StateChild::StateChild( const char * _name, const UpdateDelay &_ur )
	: State( _name, _ur )
{
}

State::StateStatus StateChild::UpdateState( float fDt )
{
	return Update( fDt );
}

//////////////////////////////////////////////////////////////////////////