////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERFLOODFILL_PATHINTERFACE_H__
#define __PATHPLANNERFLOODFILL_PATHINTERFACE_H__

#include "PathPlannerFloodFill.h"
#include "platformspecifics.h"

class FloodFillPathInterface : public PathInterface
{
public:
	FloodFillPathInterface( Client * client, PathPlannerFloodFill * nav );
	virtual PathStatus GetPathStatus() const;
	virtual void UpdateSourcePosition( const Vector3f & srcPos );
	virtual void UpdateGoalPosition( const Vector3f & goal, float radius );
	virtual void UpdateGoalPositions( const DestinationVector & goals );
	virtual bool UpdateGoalPositionRandom();
	virtual void UpdatePath();
	virtual void Cancel();	
	virtual size_t GetPathCorners( PathCorner * corners, size_t maxEdges );
	virtual bool GetPointAlongPath( float lookAheadDist, Vector3f & ptOut );
	virtual void Render();
private:
	Client *				mClient;
	PathPlannerFloodFill *	mNav;
	PathStatus				mStatus;

	Vector3f				mSrc;
	DestinationVector		mGoals;

	typedef PathPlannerFloodFill::SpanMap::InfluenceMap Influence;

	Influence *				mInfluence;

	Vector3f				mMoveDirection;

	int						mFoundGoalIndex;
};

#endif
