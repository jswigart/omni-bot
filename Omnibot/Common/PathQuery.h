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

#include "MapGoal.h"

// class: PathQuery
//		Represents a simple or complex path query for the path planner to search for any number of
//		up to <MaxGoals> target goals.
class PathQuery
{
public:
	PathQuery &Starts( const DestinationVector *_starts );
	PathQuery &Goals( const DestinationVector *_goals );

	PathQuery &AddTeam( int _team );
	PathQuery &AddClass( int _class );

	PathQuery &SetMovementCaps( const BitFlag32 &_caps );

	PathQuery &MaxPathDistance( float _dist );

	PathQuery( Client *_client = 0 );
private:
	Client				*  mClient;

	// specific goal destinations
	const DestinationVector		*  mStarts;
	const DestinationVector		*  mGoals;
	BitFlag32				 mTeam;
	BitFlag32				 mClass;
	BitFlag32				 mMovementCaps;
	float					 mMaxPathDistance;

	bool					 mReturnPartial;
};

#endif