////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __CALLBACKPARAMETERS_H__
#define __CALLBACKPARAMETERS_H__

#include "common.h"

class Client;
class gmMachine;
class gmUserObject;
class gmTableObject;
class gmFunctionObject;
#include "gmVariable.h"

#define DEBUG_PARAMS 0

class CallbackParameters
{
public:
	enum
	{
		MaxVariables = 8
	};

	void CallScript( bool _immediate = false )
	{
		mShouldCallScript = true; 
		mCallImmediate = _immediate;
	}
	bool CallImmediate() const
	{
		return mCallImmediate;
	}
	void DontPropogateEvent()
	{
		mPropogateEvent = false;
	}
	bool ShouldCallScript() const
	{
		return mShouldCallScript;
	}
	bool ShouldPropogateEvent() const
	{
		return mPropogateEvent;
	}
	void DebugName( const char *_name );
	void PrintDebug();
	void SetTargetState( uint32_t _ts )
	{
		mTargetState = _ts;
	}
	uint32_t GetTargetState() const
	{
		return mTargetState;
	}
	int GetMessageId() const
	{
		return mMessageId;
	}
	gmMachine *GetMachine() const
	{
		return mMachine;
	}

	void ResetParams()
	{
		mNumParameters = 0;
	}

	void AddNull( const char *_name );
	void AddInt( const char *_name, int _param );
	void AddFloat( const char *_name, float _param );
	void AddVector( const char *_name, const Vector3f &_vec );
	void AddVector( const char *_name, const float pos[3] );
	void AddVector( const char *_name, float _x, float _y, float _z );
	void AddEntity( const char *_name, GameEntity _param );
	void AddUserObj( const char *_name, gmUserObject *_param );
	void AddString( const char *_name, const char *_param );
	void AddTable( const char *_name, gmTableObject *_param );

	int CallFunction( gmFunctionObject *_func,
		const gmVariable &a_thisVar = gmVariable::s_null,
		bool a_delayExecuteFlag = false ) const;

	CallbackParameters( int _messageId, gmMachine *_machine );
private:
	int		 mMessageId;
	int		 mNumParameters;
	gmMachine	* mMachine;
	const char	* mMessageName;
	uint32_t mTargetState;

	gmVariable	 mVariables[ MaxVariables ];

#if(DEBUG_PARAMS)
	const char*	 mDebugNames[MaxVariables];
#endif

	bool	 mShouldCallScript;
	bool	 mCallImmediate;
	bool	 mPropogateEvent;

	void CheckParameters();

	CallbackParameters();
};

#endif
