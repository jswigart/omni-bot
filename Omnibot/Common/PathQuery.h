////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHQUERY_H__
#define __PATHQUERY_H__

#include "common.h"

struct Destination
{
	Vector3f	mPosition;
	float		mRadius;

	bool operator==( const Destination& other ) const;

	Destination();
	Destination( const Vector3f &_pos, float _radius );
};

typedef std::vector<Destination> DestinationVector;

struct DeferredQuery
{
	struct Goal
	{
		Vector3f		mDest;
		float			mNavCost;
		float			mThreatCost;

		Goal();
	};

	NavFlags				mInclude;
	NavFlags				mExclude;
	Vector3f				mSrc;
	std::vector<Goal>		mGoals;
	bool					mExecuting;
	bool					mFinished;

	DeferredQuery();
	virtual ~DeferredQuery();
};
typedef std::shared_ptr<DeferredQuery> QueryRef;

#endif