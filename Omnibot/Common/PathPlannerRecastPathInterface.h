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

#include "PathPlannerRecast.h"
#include "platformspecifics.h"

#include <DetourPathCorridor.h>

class RecastPathInterface;
typedef std::vector<RecastPathInterface*> RecastPathInterfaces;

class RecastPathInterface : public PathInterface
{
public:
	RecastPathInterface( Client * client, PathPlannerRecast * nav );
	~RecastPathInterface();

	virtual PathStatus GetPathStatus() const;
	virtual void UpdateNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags );
	virtual void UpdateSourcePosition( const Vector3f & srcPos );
	virtual void UpdateGoalPosition( const Vector3f & goal, float radius );
	virtual void UpdateGoalPositions( const DestinationVector & goals );
	virtual bool UpdateGoalPositionRandom();
	virtual void UpdatePath();
	virtual void Cancel();	

	virtual NavArea GetCurrentArea() const;
	virtual NavAreaFlags GetCurrentAreaFlags() const;
	virtual size_t GetPathCorners( PathCorner * corners, size_t maxEdges );

	virtual bool GetNavLink( uint64_t id, OffMeshConnection& conn ) const;
	virtual bool CompleteNavLink( uint64_t id );
	virtual void Render();

	void ReInit();

	static RecastPathInterfaces sInterfaces;
private:
	Client *				mClient;
	PathPlannerRecast *		mNav;
	PathStatus				mStatus;

	dtQueryFilter			mFilter;
	class dtNavMeshQuery *	mQuery;
	dtPathCorridor			mCorridor;
	
	NavArea					mCurrentPolyArea;
	NavAreaFlags			mCurrentPolyFlags;

	Vector3f				mSrc;
	DestinationVector		mGoals;
	
	Vector3f				mMoveDirection;

	int						mFoundGoalIndex;
};

#endif
