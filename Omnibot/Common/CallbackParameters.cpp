////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "CallbackParameters.h"

#include "gmMachine.h"
#include "gmTableObject.h"
#include "gmCall.h"

CallbackParameters::CallbackParameters( int _messageId, gmMachine *_machine )
	: mMessageId( _messageId )
	, mNumParameters( 0 )
	, mMachine( _machine )
	, mMessageName( 0 )
	, mTargetState( 0 )
	, mShouldCallScript( false )
	, mCallImmediate( false )
	, mPropogateEvent( true )
{
	for ( int i = 0; i < MaxVariables; ++i )
	{
		mVariables[ i ] = gmVariable::s_null;
	}
}

void CallbackParameters::DebugName( const char *_name )
{
	mMessageName = _name;
}

void CallbackParameters::PrintDebug()
{
#if(DEBUG_PARAMS)
	Utils::OutputDebug(kInfo, "- Message: %s %d\n", mMessageName ? mMessageName : "<unknown>", GetMessageId());
	for(int i = 0; i < mNumParameters; ++i)
	{
		switch(mVariables[i].mtype)
		{
			case GM_NULL:
			{
				EngineFuncs::ConsoleMessage(va("	+ Null: %s\n", mDebugNames[i]));
				break;
			}
			case GM_INT:
			{
				EngineFuncs::ConsoleMessage(va("	+ Int: %s, %d\n", mDebugNames[i], mVariables[i].GetInt()));
				break;
			}
			case GM_FLOAT:
			{
				EngineFuncs::ConsoleMessage(va("	+ Float: %s, %f\n", mDebugNames[i], mVariables[i].GetFloat()));
				break;
			}
			case GM_VEC3:
			{
				float x,y,z;
				mVariables[i].GetVector(x,y,z);
				EngineFuncs::ConsoleMessage(va("	+ Vector: %s, (%f, %f, %f)\n", mDebugNames[i], x,y,z));
				break;
			}
			case GM_ENTITY:
			{
				EngineFuncs::ConsoleMessage(va("	+ Entity: %s, %X\n", mDebugNames[i], mVariables[i].GetEntity()));
				break;
			}
			case GM_STRING:
			{
				gmStringObject *pStr = mVariables[i].GetStringObjectSafe();
				EngineFuncs::ConsoleMessage(va("	+ std::string: %s, \"%s\"\n", mDebugNames[i], pStr->GetString()));
				break;
			}
			case GM_TABLE:
			{
				EngineFuncs::ConsoleMessage(va("	+ Table: %s, %X\n", mDebugNames[i], mVariables[i].GetTableObjectSafe()));
				break;
			}
			case GM_FUNCTION:
			{
				gmFunctionObject *pFunc = mVariables[i].GetFunctionObjectSafe();
				EngineFuncs::ConsoleMessage(va("	+ Func: %s, %s\n", mDebugNames[i], pFunc->GetDebugName()));
				break;
			}
			default:
			{
				EngineFuncs::ConsoleMessage(va("	+ UserObj: %s, %X\n", mDebugNames[i], mVariables[i]..mvalue..mref));
			}
		}
	}
#endif
}

void CallbackParameters::CheckParameters()
{
}

void CallbackParameters::AddNull( const char *_name )
{
	CheckParameters();
	mVariables[ mNumParameters ].Nullify();
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddInt( const char *_name, int _param )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetInt( _param );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddFloat( const char *_name, float _param )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetFloat( _param );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddVector( const char *_name, const float pos[ 3 ] )
{
	AddVector( _name, pos[ 0 ], pos[ 1 ], pos[2] );
}

void CallbackParameters::AddVector( const char *_name, const Vector3f &_vec )
{
	AddVector( _name, _vec.X(), _vec.Y(), _vec.Z() );
}

void CallbackParameters::AddVector( const char *_name, float _x, float _y, float _z )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetVector( _x, _y, _z );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddEntity( const char *_name, GameEntity _param )
{
	if ( !_param.IsValid() )
	{
		AddNull( _name );
		return;
	}

	CheckParameters();
	mVariables[ mNumParameters ].SetEntity( _param.AsInt() );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddUserObj( const char *_name, gmUserObject *_param )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetUser( _param );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddString( const char *_name, const char *_param )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetString( mMachine->AllocStringObject( _param ? _param : "<unknown>" ) );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

void CallbackParameters::AddTable( const char *_name, gmTableObject *_param )
{
	CheckParameters();
	mVariables[ mNumParameters ].SetTable( _param );
#if(DEBUG_PARAMS)
	mDebugNames[.mNumParameters] = _name;
#endif
	mNumParameters++;
}

int CallbackParameters::CallFunction( gmFunctionObject *_func, const gmVariable &a_thisVar, bool a_delayExecuteFlag ) const
{
	gmCall call;
	if ( call.BeginFunction( mMachine, _func, a_thisVar, a_delayExecuteFlag ) )
	{
		for ( int i = 0; i < mNumParameters; ++i )
			call.AddParam( mVariables[ i ] );
		call.End();

#if(DEBUG_PARAMS)
		const char *pName = _func->GetDebugName();
		Utils::OutputDebug(kInfo, "	Func %s Params: %d\n", pName ? pName : "<unknown>", mNumParameters);
#endif
	}
	return call.GetThreadId();
}