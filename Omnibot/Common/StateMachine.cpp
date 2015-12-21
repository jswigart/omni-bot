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
	: mSibling( 0 )
	, mParent( 0 )
	, mFirstChild( 0 )
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
	while ( mFirstChild )
	{
		State *pSt = mFirstChild;
		mFirstChild = pSt->mSibling;
		delete pSt;
	}
}

State* State::GetSibling()
{
	return mSibling;
}

State* State::GetParent()
{
	return mParent;
}

State* State::GetFirstChild()
{
	return mFirstChild;
}

State* State::GetRootState()
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
	if ( mFirstChild )
	{
		State *pLastState = mFirstChild;
		while ( pLastState && pLastState->mSibling )
			pLastState = pLastState->mSibling;

		pLastState->mSibling = _state;
	}
	else
	{
		mFirstChild = _state;
	}

	_state->mSibling = NULL;
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
	_state->mSibling = mFirstChild;
	mFirstChild = _state;

	_state->mParent = this;
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
		State *pLastState = NULL;
		for ( State *pState = pReplaceState->mParent->mFirstChild;
			pState;
			pState = pState->mSibling )
		{
			if ( pState == pReplaceState )
			{
				if ( pState->mParent && pState->mParent->mFirstChild == pState )
					pState->mParent->mFirstChild = _insertstate;

				// splice it out
				if ( pLastState )
					pLastState->mSibling = _insertstate;
				_insertstate->mSibling = pState->mSibling;
				_insertstate->mParent = pState->mParent;
				_insertstate->mRoot = pState->mRoot;

				// fix the old one and return it
				pState->mParent = 0;
				pState->mSibling = 0;

				return pState;
			}
			pLastState = pState;
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
		// splice it in
		_insertstate->mSibling = pFoundState->mSibling;
		_insertstate->mParent = pFoundState->mParent;
		_insertstate->mRoot = pFoundState->mRoot;
		pFoundState->mSibling = _insertstate;
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

	bool bGood = false;
	State *pFoundState = FindState( _name );
	if ( pFoundState )
	{
		_insertstate->mParent = pFoundState->mParent;
		_insertstate->mRoot = pFoundState->mRoot;

		if ( pFoundState->mParent->mFirstChild == pFoundState )
		{
			_insertstate->mSibling = pFoundState;
			pFoundState->mParent->mFirstChild = _insertstate;
			bGood = true;
		}
		else
		{
			bGood = false;
			for ( State *pS = pFoundState->mParent->mFirstChild;
				pS;
				pS = pS->mSibling )
			{
				if ( pS->mSibling == pFoundState )
				{
					pS->mSibling = _insertstate;
					_insertstate->mSibling = pFoundState;
					bGood = true;
					break;
				}
			}
		}
	}
	return bGood;
}

State *State::RemoveState( const char * _name )
{
	State *pDeleteState = FindState( _name );
	if ( pDeleteState )
	{
		pDeleteState->InternalExit();

		State *pLastState = NULL;
		for ( State *pState = pDeleteState->mParent->mFirstChild;
			pState;
			pState = pState->mSibling )
		{
			if ( pState == pDeleteState )
			{
				if ( pState->mParent && pState->mParent->mFirstChild == pState )
					pState->mParent->mFirstChild = pState->mSibling;

				if ( pLastState )
					pLastState->mSibling = pState->mSibling;

				// fix the old one and return it
				pState->mParent = 0;
				pState->mSibling = 0;

				return pState;
			}
			pLastState = pState;
		}
	}
	return 0;
}

void State::InitializeStates()
{
	Initialize();
	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->InitializeStates();
}

void State::FixRoot()
{
	// Find the root state
	mRoot = GetParent();
	while ( mRoot != NULL && mRoot->mParent )
		mRoot = mRoot->mParent;

	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->FixRoot();
}

void State::SetClient( Client *_client )
{
	mClient = _client;

	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->SetClient( _client );
}

State *State::FindState( const char *_name )
{
	return FindState( Utils::Hash32( _name ) );
}

State *State::FindStateRecurse( uint32_t _namehash )
{
	if ( mNameHash == _namehash )
		return this;

	State *ptr = NULL;
	for ( State *pState = mFirstChild; pState && !ptr; pState = pState->mSibling )
		ptr = pState->FindStateRecurse( _namehash );
	return ptr;
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
		for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
			pState->InternalExit();

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
	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->SignalThreads( _signal );
}

void State::CheckForCallbacks( const Message & message, CallbackParameters &_cb )
{
	if ( IsRoot() || IsActive() || AlwaysRecieveEvents() )
		InternalProcessEvent( message, _cb );

	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->CheckForCallbacks( message, _cb );
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
	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		pState->PropogateDeletedThreads( _threadIds, _numThreads );

	RemoveThreadReference( _threadIds, _numThreads );
}

bool State::StateCommand( const StringVector & args )
{
	bool handled = false;
	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		handled |= pState->StateCommand( args );

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

	for ( State *pState = mFirstChild; pState; pState = pState->mSibling )
		if ( !pState->IsUserDisabled() )
			pState->OnSpawn();
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

#ifdef ENABLE_DEBUG_WINDOW
void State::RenderDebugWindow(gcn::DrawInfo drawinfo)
{
	if(DontDrawDebugWindow())
		return;

	// Draw the title.
	if(GetParent())
	{
		const char *prefix = "  ";
		if(.mFirstChild)
			prefix = IsDebugExpanded() ? "- ": "+ ";

		gcn::Color renderColor = IsActive()||.mLastUpdateTime==IGame::GetTime() ? gcn::Color(0,160,0) : gcn::Color(0,0,0);
		if(.mStateFlags.CheckFlag(State_UnSelectable))
			renderColor = gcn::Color(255,255,255);
		if(IsDisabled())
			renderColor = gcn::Color(255,0,0);

		const int fontHeight = drawinfo.mWidget->getFont()->getHeight();
		int iX = drawinfo.mIndent * 12;
		int iY = drawinfo.mLine * fontHeight;

		gcn::Rectangle l(0, iY, drawinfo.mWidget->getWidth(), fontHeight);

		if(l.isPointInRect(drawinfo.Mouse.X, drawinfo.Mouse.Y))
		{
			renderColor.a = 128;

			if(drawinfo.Mouse.MouseClicked[gcn::MouseCache::Right])
				DebugExpand(!IsDebugExpanded());
			if(drawinfo.Mouse.MouseClicked[gcn::MouseCache::Left])
				ShowStateWindow(GetNameHash());
			if(drawinfo.Mouse.MouseClicked[gcn::MouseCache::Mid])
				DebugDraw(!DebugDrawingEnabled());
		}

		drawinfo.mGraphics->setColor(renderColor);
		drawinfo.mGraphics->fillRectangle(l);
		drawinfo.mGraphics->setColor(drawinfo.mWidget->getForegroundColor());

		std::stringstream s;
		GetDebugString(s);

		drawinfo.mGraphics->drawText((std::string)va("%s %s (%.2f) - %s:",
			prefix,GetName().c_str(),
			GetLastPriority(),
			s.str().c_str()),iX,iY);

		++drawinfo.mLine;

		if(IsDebugExpanded())
		{
			for(State *pState = mFirstChild; pState; pState = pState->mSibling)
				pState->RenderDebugWindow(drawinfo.indent());
		}
		return;
	}

	if(IsDebugExpanded() || !GetParent())
	{
		for(State *pState = mFirstChild; pState; pState = pState->mSibling)
			pState->RenderDebugWindow(drawinfo.indent());
	}
}
#endif

#ifdef ENABLE_REMOTE_DEBUGGING
void State::Sync( RemoteLib::DebugConnection * connection, Remote::Behavior & cached, Remote::Behavior & update ) {
	//std::string pth = va( "%s/%s", statePath, GetName().c_str() );

	std::stringstream s;
	GetDebugString(s);
	obColor renderColor = IsActive()||.mLastUpdateTime==IGame::GetTime() ? obColor( 0, 160, 0) : obColor(0,0,0);
	if(.mStateFlags.CheckFlag(State_UnSelectable))
		renderColor = obColor(255,255,255);
	if(IsDisabled())
		renderColor = obColor(255,0,0);

	SET_IF_DIFF( cached, update, GetName().c_str(), name );
	SET_IF_DIFF( cached, update, (int)renderColor, color );
	SET_IF_DIFF( cached, update, s.str().c_str(), info );

	int childIndex = 0;
	for(State *pState = mFirstChild; pState; pState = pState->mSibling) {
		if ( cached.children().Capacity() <= childIndex ) {
			cached.mutable_children()->Reserve( childIndex + 1 );
		}

		while ( cached.children_size() <= childIndex ) {
			cached.add_children();
		}

		if ( update.children().Capacity() <= childIndex ) {
			update.mutable_children()->Reserve( childIndex + 1 );
		}

		while ( update.children_size() <= childIndex ) {
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
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pState->IsUserDisabled() )
			continue;

		float fPriority = pState->InternalGetPriority();
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
	State *pLastState = NULL;
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		bool bWantsActive = !pState->IsDisabled() ? pState->InternalGetPriority() > 0.0 : false;

		// Handle exits
		if ( pState->IsActive() && ( !bWantsActive || pState->IsDisabled() ) )
		{
			pState->InternalExit(); // call internal version
			if ( !bWantsActive && pState->CheckFlag( State_DeleteOnFinished ) )
			{
				if ( pLastState )
					pLastState = pState->GetSibling();
				delete pState;
			}
			continue;
		}

		if ( !pState->IsActive() && bWantsActive )
			pState->InternalEnter(); // call internal version

		if ( pState->IsActive() )
		{
			if ( pState->InternalUpdateState() == State_Finished )
				pState->InternalExit();
		}
		pLastState = pState;
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
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pState->IsUserDisabled() )
			continue;

		float fPriority = pState->InternalGetPriority();
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
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pState->IsUserDisabled() )
			continue;

		float fPriority = pState->InternalGetPriority();
		if ( fPriority > 0.f )
		{
			pBestState = pState;
			break;
		}
	}

	// Exit active states that are not our best
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pBestState != pState && pState->IsActive() )
		{
			pState->InternalExit();

			if ( mCurrentState == pState )
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
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pState->IsUserDisabled() )
			continue;

		float fPriority = pState->InternalGetPriority();
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

	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pState->IsUserDisabled() )
			continue;

		float fPriority = pState->InternalGetPriority();

#ifdef _DEBUG
		STATES_PRIO[ NumPriorities ] = fPriority;
		STATES_P[ NumPriorities ] = pState;
		NumPriorities++;
#endif

		if ( fPriority >= fBestPriority )
		{
			if ( fPriority > fBestPriority )
			{
				fBestPriority = fPriority;
				pBestState = pState;
				iBestRand = 0;
			}
			else if ( fPriority > 0 )
			{
				int iRand = rand();
				if ( iBestRand == 0 ) iBestRand = rand();
				if ( iRand > iBestRand )
				{
					iBestRand = iRand;
					pBestState = pState;
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
	for ( State *pState = GetFirstChild(); pState; pState = pState->GetSibling() )
	{
		if ( pBestState != pState && pState->IsActive() )
		{
#ifdef _DEBUG
			STATES_X[ NumExited++ ] = pState;
#endif
			pState->InternalExit();
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