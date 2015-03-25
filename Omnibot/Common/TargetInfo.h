////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TARGETINFO_H__
#define __TARGETINFO_H__

#include "common.h"
#include "gmGCRoot.h"

class gmUserObject;

// class: TargetInfo
//		Holds the last known information regarding this target.
class TargetInfo
{
public:
	// float: .mDistanceTo
	//		Distance to this target
	float				mDistanceTo;

	// int: .mEntityClass
	//		The Classification for this entity
	EntityInfo		mEntInfo;

	// int: .mCurrentWeapon
	//		The currently equipped weapon
	int					mCurrentWeapon;
	
	// var: .mLastPosition
	//		The last position observed.
	Vector3f			mLastPosition;

	// var: .mLastVelocity
	//		The last velocity observed.
	Vector3f			mLastVelocity;

	// var: .mLastFacing
	//		The last facing direction observed.
	Vector3f			mLastFacing;

	bool Update();

	gmUserObject *GetScriptObject( gmMachine *_machine ) const;

	TargetInfo();
	~TargetInfo();
private:

	// var: .mScriptObject
	//		This objects script instance, so that the object can clear its script
	//		references when deleted.
	mutable gmGCRoot<gmUserObject> mScriptObject;
};

#endif
