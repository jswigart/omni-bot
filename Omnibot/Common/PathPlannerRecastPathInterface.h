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
	RecastPathInterface( PathPlannerRecast * nav );
	~RecastPathInterface();
	
	virtual PathStatus GetPathStatus() const;
	virtual void UpdateNavFlags( NavFlags includeFlags, NavFlags excludeFlags );
	virtual void UpdateSourcePosition( const Vector3f & srcPos );
	virtual void UpdateGoalPosition( const Vector3f & goal, float radius );
	virtual void UpdateGoalPositions( const DestinationVector & goals );
	virtual bool UpdateGoalPositionRandom();
	virtual void UpdatePath( bool forceRecalculate );
	virtual void Cancel();
	
	virtual bool GetNearestWall( Vector3f& wallPos, Vector3f& wallNormal, float& wallDist );

	virtual bool NavTrace( NavTraceResult& result, const Vector3f& start, const Vector3f& end );

	virtual size_t FindAreaEntitiesInRadius( const Vector3f& pos, float radius, GameEntity entities [], size_t maxEntities );

	virtual NavAreaFlags GetCurrentAreaFlags() const;
	virtual size_t GetPathCorners( PathCorner * corners, size_t maxEdges );

	virtual bool GetNavLink( uint64_t id, OffMeshConnection& conn ) const;
	virtual bool CompleteNavLink( uint64_t id );
	virtual void Render();

	void ReInit();

	static RecastPathInterfaces sInterfaces;
private:
	PathPlannerRecast *		mNav;
	PathStatus				mStatus;

	dtQueryFilter			mFilter;
	class dtNavMeshQuery *	mQuery;
	dtPathCorridor			mCorridor;
	
	NavAreaFlags			mCurrentAreaFlags;

	Vector3f				mSrc;
	DestinationVector		mGoals;
	
	Vector3f				mMoveDirection;

	int						mFoundGoalIndex;

	Vector3f				mHitPos;
	Vector3f				mHitNormal;
	float					mHitDist;

	size_t GatherPolyEntities( dtPolyRef ref, GameEntity entities [], size_t maxEntities );
};

#endif
