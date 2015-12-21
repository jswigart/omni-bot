////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include "IGame.h"

class Client;
class Message;
class CallbackParameters;
class MapGoal;

class UpdateDelay
{
public:
	float GetDelay() const
	{
		return mUpdateDelay;
	}
	int GetDelayMsec() const
	{
		return Utils::SecondsToMilliseconds( mUpdateDelay );
	}
	float GetRate() const
	{
		return mUpdateDelay / 1000.f;
	}

	UpdateDelay( float _delay = 0 ) : mUpdateDelay( _delay )
	{
	}
private:
	float mUpdateDelay;
};

// class: State
//		Hierarchial FSM, modelled after the Halo 2 system, detailed here
//		http://www.gamasutra.com/gdc2005/features/20050311/isla_01.shtml
class State
{
public:
	friend class gmScriptGoal;

	enum DebugIcon
	{
		Ico_Default,
		Ico_Warning,
		Ico_Error
	};

	enum StateStatus
	{
		State_Busy,
		State_Finished,
		//State_Failed,
	};

	enum StateFlags
	{
		State_InActive,				// Sate currently not running.
		State_Active,				// State currently running.
		State_WantsActive,			// State wants to run.
		State_DeleteOnFinished,		// State should be deleted when it exits.
		State_UserDisabled,			// State has been disabled by the user.
		State_UnSelectable,			// State not available for activation(class filter, etc).

		State_DebugDraw,
		State_DebugDontRender,		// Don't show this state on the debug render window.
		State_DebugExpanded,		// State is expanded on debug menu.
		State_DebugExpandOnActive,	// State should auto expand on debug menu when active.

		State_AlwaysRecieveEvents, // State should recieve event callbacks even when not currently active
		State_AlwaysRecieveSignals, // State should recieve event callbacks even when not currently active

		State_DontAutoAdd,
		State_ScriptGoal,			// This state is a script goal.

		State_StartUser = 24,
	};

	State * AppendState( CHECK_PARAM_VALID State *_state );
	bool AppendTo( const char * _name, State *_insertstate );
	bool AppendTo( uint32_t _name, State *_insertstate );
	void PrependState( State *_state );
	bool PrependTo( const char * _name, State *_insertstate );
	bool PrependTo( uint32_t _name, State *_insertstate );
	State *ReplaceState( const char * _name, State *_insertstate );
	bool InsertAfter( const char * _name, State *_insertstate );
	bool InsertAfter( uint32_t _name, State *_insertstate );
	bool InsertBefore( const char * _name, State *_insertstate );
	bool InsertBefore( uint32_t _name, State *_insertstate );
	State *RemoveState( const char * _name );

	void SetClient( Client *_client );
	void SetName( const char *_name );

	State *FindState( const char *_name );
	State *FindState( uint32_t _namehash );

	virtual float GetPriority()
	{
		return ( float )1.0;
	}
	virtual bool OnPathThrough( const std::string &_s )
	{
		return false;
	}
	virtual void EndPathThrough()
	{
	}
	inline float GetLastPriority() const
	{
		return mLastPriority;
	}
	inline void SetLastPriority( float _p )
	{
		mLastPriority = _p;
	}

	State *GetParent() const;
	State *GetRootState() const;
	State *GetFirstChild() const;
	State *GetSibling() const;

	virtual State *GetActiveState() const
	{
		return NULL;
	}

	void RootUpdate();

	virtual void Initialize()
	{
	}
	virtual void Enter()
	{
	}
	virtual void Exit()
	{
	}
	virtual StateStatus Update( float fDt )
	{
		return State_Busy;
	};
	virtual void ProcessEvent( const Message &_message, CallbackParameters &_cb )
	{
	}

	void ExitAll();

	enum noSelectReason_t
	{
		NoSelectReasonNone,

		NoSelectReason_OnlyClass,
		NoSelectReason_OnlyTeam,
		NoSelectReason_OnlyPowerup,
		NoSelectReason_OnlyNoPowerup,
		NoSelectReason_OnlyEntFlag,
		NoSelectReason_OnlyNoEntFlag,
		NoSelectReason_OnlyRole,
		NoSelectReason_OnlyWeapon,
		NoSelectReason_OnlyWeaponNoAmmo,
		NoSelectReason_OnlyTarget,
		NoSelectReason_OnlyTargetClass,
		NoSelectReason_OnlyTargetTeam,
		NoSelectReason_OnlyTargetWeapon,
		NoSelectReason_OnlyTargetPowerup,
		NoSelectReason_OnlyTargetNoPowerup,
		NoSelectReason_OnlyTargetEntFlag,
		NoSelectReason_OnlyTargetNoEntFlag,
		NoSelectReason_LimitCallback,
	};
	noSelectReason_t CanBeSelected();

	virtual StateStatus UpdateState( float fDt ) = 0;

	inline bool IsActive() const
	{
		return mStateFlags.CheckFlag( State_Active );
	}
	inline bool IsUserDisabled() const
	{
		return mStateFlags.CheckFlag( State_UserDisabled );
	}
	inline bool IsSelectable() const
	{
		return !mStateFlags.CheckFlag( State_UnSelectable );
	}
	inline bool IsDisabled() const
	{
		return IsUserDisabled() || !IsSelectable();
	}
	inline bool IsScriptGoal() const
	{
		return mStateFlags.CheckFlag( State_ScriptGoal );
	}
	inline bool IsAutoAdd() const
	{
		return !mStateFlags.CheckFlag( State_DontAutoAdd );
	}

	inline void SetScriptGoal( bool _b )
	{
		mStateFlags.SetFlag( State_ScriptGoal, _b );
	}
	inline void SetAutoAdd( bool _b )
	{
		mStateFlags.SetFlag( State_DontAutoAdd, !_b );
	}
	inline void SetUserDisabled( bool _b )
	{
		mStateFlags.SetFlag( State_UserDisabled, _b );
	}

	inline bool CheckFlag( int32_t _flag )
	{
		return mStateFlags.CheckFlag( _flag );
	}
	inline void SetFlag( int32_t _flag )
	{
		mStateFlags.SetFlag( _flag );
	}

	virtual void SetSelectable( bool _selectable );
	virtual void SetEnable( bool _enable, const char *_error = 0 );

	DebugIcon GetDebugIcon() const
	{
		return mDebugIcon;
	}

	void InitializeStates();

	inline bool DebugDrawingEnabled() const
	{
		return mStateFlags.CheckFlag( State_DebugDraw );
	}
	inline void DebugDraw( bool _draw )
	{
		mStateFlags.SetFlag( State_DebugDraw, _draw );
	}

	inline bool IsDebugExpanded()
	{
		return mStateFlags.CheckFlag( State_DebugExpanded );
	}
	inline void DebugExpand( bool b )
	{
		mStateFlags.SetFlag( State_DebugExpanded, b );
	}

	inline void ToggleDebugDraw()
	{
		DebugDraw( !DebugDrawingEnabled() );
	}

	inline bool DontDrawDebugWindow() const
	{
		return mStateFlags.CheckFlag( State_DebugDontRender );
	}

	inline float GetStateTime() const
	{
		return mStateTime != 0.f ? IGame::GetTimeSecs() - mStateTime : 0.f;
	}
	inline float GetUserStateTime() const
	{
		return mStateTimeUser != 0.f ? IGame::GetTimeSecs() - mStateTimeUser : 0.f;
	}
	inline void ResetStateTimeUser()
	{
		mStateTimeUser = IGame::GetTimeSecs();
	}

	void SetAlwaysRecieveEvents( bool _b )
	{
		mStateFlags.SetFlag( State::State_AlwaysRecieveEvents, _b );
	}
	bool AlwaysRecieveEvents() const
	{
		return mStateFlags.CheckFlag( State::State_AlwaysRecieveEvents );
	}

	bool IsRoot()
	{
		return !mParent && !mRoot;
	}
	void FixRoot();

	std::string GetName() const;
	inline uint32_t GetNameHash() const
	{
		return mNameHash;
	}

	Client *GetClient() const
	{
		return mClient;
	}

	State* GetSibling();
	State* GetParent();
	State* GetFirstChild();
	State* GetRootState();

	// Filters
	BitFlag32 &LimitToClass()
	{
		return mOnlyClass;
	}
	BitFlag32 &LimitToTeam()
	{
		return mOnlyTeam;
	}
	BitFlag32 &LimitToRole()
	{
		return mOnlyRole;
	}
	BitFlag32 &LimitToPowerup()
	{
		return mOnlyPowerUp;
	}
	BitFlag64 &LimitToEntFlag()
	{
		return mOnlyEntFlag;
	}

	BitFlag64 &LimitToNoEntFlag()
	{
		return mOnlyNoEntFlag;
	}
	BitFlag32 &LimitToNoPowerup()
	{
		return mOnlyNoPowerUp;
	}

	BitFlag128 &LimitToWeapon()
	{
		return mOnlyWeapon;
	}

	BitFlag32 &LimitToTargetClass()
	{
		return mOnlyTargetClass;
	}
	BitFlag32 &LimitToTargetTeam()
	{
		return mOnlyTargetTeam;
	}
	BitFlag32 &LimitToTargetPowerup()
	{
		return mOnlyTargetPowerUp;
	}
	BitFlag32 &LimitToTargetNoPowerup()
	{
		return mOnlyTargetNoPowerUp;
	}
	BitFlag64 &LimitToTargetEntFlag()
	{
		return mOnlyTargetEntFlag;
	}
	BitFlag64 &LimitToTargetNoEntFlag()
	{
		return mOnlyTargetNoEntFlag;
	}
	BitFlag32 &LimitToTargetWeapon()
	{
		return mOnlyTargetWeapon;
	}

	void LimitTo( const gmVariable &varThis, gmGCRoot<gmFunctionObject> &_fn, int _delay, bool _onlywhenactive );
	void ClearLimitTo();

	void BlackboardDelay( float _delayseconds, int _targetId );
	bool BlackboardIsDelayed( int _targetId );

	void CheckForCallbacks( const Message &_message, CallbackParameters &_cb );
	void SignalThreads( const gmVariable &_signal );

	void AddForkThreadId( int _threadId );
	bool DeleteForkThread( int _threadId );
	bool RemoveThreadReference( const int * _threadId, int _numThreadIds );

	void PropogateDeletedThreads( const int *_threadIds, int _numThreads );

	bool StateCommand( const StringVector & args );

	virtual gmUserObject *GetScriptObject( gmMachine *_machine )
	{
		return NULL;
	}

	// Special case callbacks.
	virtual void OnSpawn();

	// Debug
	virtual void RenderDebug()
	{
	}
	virtual void GetDebugString( std::stringstream &out )
	{
	}
	virtual MapGoal *GetMapGoalPtr()
	{
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	struct LimitToCallback
	{
		gmGCRoot<gmFunctionObject>	mLimitTo;
		int							mNextCallback;
		int							mDelay;
		gmVariable					mThis;
		bool						mOnlyWhenActive;
		bool						mResult;

		LimitToCallback()
			: mLimitTo( 0 )
			, mNextCallback( 0 )
			, mDelay( 0 )
			, mThis( gmVariable::s_null )
			, mOnlyWhenActive( false )
			, mResult( false )
		{
		}
	};
	int gmfLimitToClass( gmThread *a_thread );
	int gmfLimitToTeam( gmThread *a_thread );
	int gmfLimitToPowerUp( gmThread *a_thread );
	int gmfLimitToEntityFlag( gmThread *a_thread );
	int gmfLimitToWeapon( gmThread *a_thread );

	int gmfLimitToNoTarget( gmThread *a_thread );
	int gmfLimitToTargetClass( gmThread *a_thread );
	int gmfLimitToTargetTeam( gmThread *a_thread );
	int gmfLimitToTargetPowerUp( gmThread *a_thread );
	int gmfLimitToTargetEntityFlag( gmThread *a_thread );
	//////////////////////////////////////////////////////////////////////////

	float InternalGetPriority();
	void InternalEnter();
	virtual void InternalExit();
	StateStatus InternalUpdateState();

	State( const char * _name, const UpdateDelay &_ur = UpdateDelay( 0 ) );
	virtual ~State();
protected:

	virtual void InternalParentExit();
	
	void InternalProcessEvent( const Message &_message, CallbackParameters &_cb );
	virtual void InternalSignal( const gmVariable &_signal );

	State *FindStateRecurse( uint32_t _namehash );

	BitFlag32	 mStateFlags;

	// limitations
	BitFlag32	 mOnlyClass;
	BitFlag32	 mOnlyTeam;
	BitFlag32	 mOnlyRole;
	BitFlag32	 mOnlyPowerUp;
	BitFlag32	 mOnlyNoPowerUp;
	BitFlag64	 mOnlyEntFlag;
	BitFlag64	 mOnlyNoEntFlag;
	BitFlag128	 mOnlyWeapon;

	BitFlag32	 mOnlyTargetClass;
	BitFlag32	 mOnlyTargetTeam;
	BitFlag32	 mOnlyTargetWeapon;
	BitFlag32	 mOnlyTargetPowerUp;
	BitFlag32	 mOnlyTargetNoPowerUp;
	BitFlag64	 mOnlyTargetEntFlag;
	BitFlag64	 mOnlyTargetNoEntFlag;

	LimitToCallback mLimitCallback;

	State*		 mSibling;
	State*		 mParent;
	State*		 mFirstChild;
	State*		 mRoot;

	static const int MaxThreads = 128;
	int			 mNumThreads;
	int			 mThreadList[ MaxThreads ];

	gmGCRoot<gmTableObject>	 mEventTable;
	gmGCRoot<gmTableObject>	 mCommandTable;
	gmGCRoot<gmUserObject>	 mScriptObject;
private:
	void ClearThreadReference( int index );

	Client			* mClient;

	int32_t		 mNextUpdate;
	int32_t		 mLastUpdateTime;
	float		 mStateTime;
	float		 mStateTimeUser;
	float		 mLastPriority;
	int32_t		 mLastPriorityTime;
	UpdateDelay	 mUpdateRate;

	uint32_t	 mNameHash;

	DebugIcon	 mDebugIcon;

	uint32_t	 mSyncCrc;

#ifdef _DEBUG
	std::string		 mDebugName;
#endif

	State();
};

//////////////////////////////////////////////////////////////////////////

// class: StateSimultaneous
//		Runs all non zero priority states at the same time.
class StateSimultaneous : public State
{
public:

	float GetPriority();
	StateStatus UpdateState( float fDt );

	StateSimultaneous( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

// class: StateFirstAvailable
//		List of states. First one that can run does, but it can
//		be interrupted by higher priority states.
class StateFirstAvailable : public State
{
public:
	void GetDebugString( std::stringstream &out );

	float GetPriority();
	void InternalParentExit();
	StateStatus UpdateState( float fDt );

	StateFirstAvailable( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
	State	* mCurrentState;
};

//////////////////////////////////////////////////////////////////////////

// class: StatePrioritized
//		Prioritized List of states. Highest priority runs, but it can
//		be interrupted by higher priority states.
class StatePrioritized : public State
{
public:
	void GetDebugString( std::stringstream &out );

	float GetPriority();
	void InternalParentExit();
	StateStatus UpdateState( float fDt );

	virtual State *GetActiveState() const
	{
		return mCurrentState;
	}

	StatePrioritized( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
	State	* mCurrentState;
};

//////////////////////////////////////////////////////////////////////////

// class: StateSequential
//		Run each child in order, skipping any that have priotity 0.
//		When all children finish running, the parent state is finished.
//		Fail if any children fail.
class StateSequential : public State
{
public:
	void GetDebugString( std::stringstream &out );

	void Exit();
	StateStatus UpdateState( float fDt );

	virtual State *GetActiveState() const
	{
		return mCurrentState;
	}

	StateSequential( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
	State	* mCurrentState;
};

//////////////////////////////////////////////////////////////////////////

// class: StateSequentialLooping
//		Run each child in order, skipping any that have priotity 0.
//		When all children finish running, it starts over.
class StateSequentialLooping : public State
{
public:
	StateStatus UpdateState( float fDt );

	//virtual State *GetActiveState() const { return mCurrentState; }

	StateSequentialLooping( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

// class: StateProbabilistic
//		Choose a random child state to run.
class StateProbabilistic : public State
{
public:
	StateStatus UpdateState( float fDt );

	//virtual State *GetActiveState() const { return mCurrentState; }

	StateProbabilistic( const char * _name, const UpdateDelay &_ur = UpdateDelay() );
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

// class: StateOneOff
//		Pick a random child state to run, but never repeat the same one twice.
class StateOneOff : public State
{
public:
	StateStatus UpdateState( float fDt );

	//virtual State *GetActiveState() const { return mCurrentState; }

	StateOneOff( const char * _name, const UpdateDelay &_ur = UpdateDelay( 0 ) );
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

// class: StateChild
//		Pick a random child state to run, but never repeat the same one twice.
class StateChild : public State
{
public:
	StateStatus UpdateState( float fDt );

	//virtual State *GetActiveState() const { return mCurrentState; }

	StateChild( const char * _name, const UpdateDelay &_ur = UpdateDelay( 0 ) );
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

#define FINDSTATE(var, statename, parent) \
	statename *var = static_cast<statename*>(parent->FindState(#statename));

#define FINDSTATEIF(statename, parent, exp) { \
	statename *st = static_cast<statename*>(parent->FindState(#statename));	if(st) st->exp; }

#define FINDSTATE_OPT(var, statename, parent) \
	statename *var = static_cast<statename*>(parent->FindState(#statename));

#define FINDSTATEIF_OPT(statename, parent, exp) { \
	statename *st = static_cast<statename*>(parent->FindState(#statename));	if(st) st->exp; }

#endif
