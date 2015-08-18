////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "gmTargetInfo.h"

// script: TargetInfo
//		Script bindings for <TargetInfo>

GMBIND_INIT_TYPE( gmTargetInfo, "TargetInfo" );

GMBIND_FUNCTION_MAP_BEGIN( gmTargetInfo )
	GMBIND_FUNCTION( "IsA", gmIsA )
GMBIND_FUNCTION_MAP_END();

// property: Distance
//		The distance this target is from the bot.
// property: Position
//		The <Vector3> position of the target.
// property: Velocity
//		The <Vector3> velocity of the target.

GMBIND_PROPERTY_MAP_BEGIN( gmTargetInfo )
	GMBIND_PROPERTY( "Distance", getDistanceTo, NULL )
	GMBIND_PROPERTY( "Position", getPosition, NULL )
	GMBIND_PROPERTY( "Facing", getFacing, NULL )
	GMBIND_PROPERTY( "Velocity", getVelocity, NULL )
	GMBIND_PROPERTY( "Group", getGroup, NULL )
	GMBIND_PROPERTY( "Class", getClass, NULL )
	GMBIND_PROPERTY( "Energy", getEnergy, NULL )
	GMBIND_PROPERTY( "EnergyMax", getEnergyMax, NULL )
GMBIND_PROPERTY_MAP_END();

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

TargetInfo *gmTargetInfo::Constructor(gmThread *a_thread)
{
	return NULL;
}

void gmTargetInfo::Destructor(TargetInfo *_native)
{
}

//////////////////////////////////////////////////////////////////////////
// Functions

int gmTargetInfo::gmIsA(gmThread *a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(checkclass, 0);
	TargetInfo *pNative = gmTargetInfo::GetThisObject( a_thread );

	if ( pNative == NULL ) 
	{
		return GM_EXCEPTION;
	}
	a_thread->PushInt( pNative->mEntInfo.mClassId == checkclass ? 1 : 0 );
	return GM_OK;
}

//////////////////////////////////////////////////////////////////////////
// Property Accessors/Modifiers

bool gmTargetInfo::getDistanceTo( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[0].SetFloat(a_native->mDistanceTo);
	return true;
}

bool gmTargetInfo::getPosition( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[0].SetVector(
		a_native->mLastPosition.X(), 
		a_native->mLastPosition.Y(), 
		a_native->mLastPosition.Z());
	return true;
}

bool gmTargetInfo::getFacing( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[0].SetVector(
		a_native->mLastFacing.X(), 
		a_native->mLastFacing.Y(), 
		a_native->mLastFacing.Z());
	return true;
}

bool gmTargetInfo::getVelocity( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[0].SetVector(
		a_native->mLastVelocity.X(), 
		a_native->mLastVelocity.Y(), 
		a_native->mLastVelocity.Z());
	return true;
}

bool gmTargetInfo::getGroup( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetInt( a_native->mEntInfo.mGroup );
	return true;
}

bool gmTargetInfo::getClass( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetInt( a_native->mEntInfo.mClassId );
	return true;
}

bool gmTargetInfo::getEnergy( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetInt( a_native->mEntInfo.mEnergy.mNum );
	return true;
}

bool gmTargetInfo::getEnergyMax( TargetInfo *a_native, gmThread *a_thread, gmVariable *a_operands )
{
	a_operands[ 0 ].SetInt( a_native->mEntInfo.mEnergy.mMax );
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Operators
