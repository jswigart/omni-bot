////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "TargetInfo.h"
#include "ScriptManager.h"
#include "gmTargetInfo.h"

TargetInfo::TargetInfo() 
	: mDistanceTo( 0 )
	, mCurrentWeapon( 0 )
	, mLastPosition( Vector3f::ZERO )
	, mLastVelocity( Vector3f::ZERO )
	, mLastFacing( Vector3f::ZERO )
{
}

TargetInfo::~TargetInfo()
{
	if ( mScriptObject )
	{
		gmTargetInfo::NullifyObject( mScriptObject );
		mScriptObject = 0; //NULL;
	}
}

gmUserObject *TargetInfo::GetScriptObject( gmMachine *_machine ) const
{
	if ( !mScriptObject )
	{
		mScriptObject.Set( gmTargetInfo::WrapObject( _machine, const_cast<TargetInfo*>( this ) ), _machine );
	}
	return mScriptObject;
}