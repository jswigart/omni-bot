////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNER_H__
#define __PATHPLANNER_H__

class System;
class Goal;
class GoalQueue;
class Client;
class Path;
class gmMachine;
struct EvEntityConnection;

#include "EventReciever.h"
#include "CommandReciever.h"
#include "PathQuery.h"

typedef EnumerationValues<NavAreaFlags> NavAreaFlagsEnum;

struct EntityInstance;

struct OffMeshConnection
{
	Vector3f				mEntry;
	Vector3f				mExit;

	std::vector<Vector3f>	mVertices;
	float					mRadius;
	NavAreaFlags			mAreaFlags;
	bool					mBiDirectional : 1;

	// runtime use
	uint64_t				mPolyId;

	void Render();

	OffMeshConnection();
};

struct MeshEdge
{
	Vector3f				mEdge[ 2 ];
	Vector3f				mNormal;
	uint64_t				mPolyRef;
};

class PathInterface
{
public:
	PathInterface();
	virtual ~PathInterface();
	
	enum PathStatus
	{
		PATH_NONE,
		PATH_VALID,
		PATH_SEARCHING,
		PATH_NOPATHTOGOAL,
	};

	struct PathCorner
	{
		static const int MAX_ENTITIES = 8;

		Vector3f		mPos;
		NavAreaFlags	mAreaMask;
		uint64_t		mPolyId;
		GameEntity		mEntities[ MAX_ENTITIES ];
		bool			mIsLink;
		
		PathCorner()
			: mAreaMask( NAVFLAGS_NONE )
			, mPolyId( 0 )
			, mIsLink( false )
		{
		}
	};
	
	virtual PathStatus GetPathStatus() const = 0;

	struct NavTraceResult
	{
		float		t;
		Vector3f	pos;
		Vector3f	normal;
		float		pathCost;

		NavTraceResult()
			: t( FLT_MAX )
			, pos( Vector3f::ZERO )
			, normal( Vector3f::ZERO )
			, pathCost( -1.0f )
		{
		}
	};
	
	virtual void UpdateNavFlags( NavFlags includeFlags, NavFlags excludeFlags ) = 0;
	virtual void UpdateSourcePosition( const Vector3f & srcPos ) = 0;
	virtual void UpdateGoalPosition( const Vector3f & goal, float radius ) = 0;
	virtual void UpdateGoalPositions( const DestinationVector & goals ) = 0;
	virtual bool UpdateGoalPositionRandom() = 0;
	virtual void UpdatePath( bool forceRecalculate ) = 0;
	virtual void Cancel() = 0;

	virtual bool GetNearestWall( Vector3f& wallPos, Vector3f& wallNormal, float& wallDist ) = 0;

	virtual bool NavTrace( NavTraceResult& result, const Vector3f& start, const Vector3f& end ) { return false; }

	virtual size_t FindAreaEntitiesInRadius( const Vector3f& pos, float radius, GameEntity entities[], size_t maxEntities ) { return 0; }

	virtual NavAreaFlags GetCurrentAreaFlags() const = 0;
	virtual size_t GetPathCorners( PathCorner * corners, size_t maxEdges ) = 0;

	virtual bool GetNavLink( uint64_t id, OffMeshConnection& conn ) const = 0;
	virtual bool CompleteNavLink( uint64_t id ) = 0;
	virtual void Render() = 0;
};

template< typename T >
class EditTool
{
public:
	EditTool( const char * toolname ) : mToolName( toolname )
	{
	}
	virtual ~EditTool()
	{
	}

	virtual bool Enter( T * system ) = 0;
	virtual bool ReEnter( T * system )
	{
		return true;
	}
	virtual bool Update( T * system ) = 0;
	virtual void Commit( T * system ) = 0;
	virtual void Undo( T * system )
	{
		EngineFuncs::ConsoleError( va( "No Undo functionality for tool %s", mToolName ).c_str() );
	}
protected:
	std::string		mToolError;
	const char *	mToolName;
};

// class: PathPlannerBase
//		Abstract Base Class for Path Planning
//		Provides the interface functions for running a pathfinding query.
//		Shouldn't care about the underlying implementation of the pathing system.
class PathPlannerBase : public CommandReciever
{
public:
	virtual bool Init( System & system ) = 0;
	virtual void Update( System & system ) = 0;
	virtual void Shutdown() = 0;
	virtual bool IsReady() const = 0;
			
	virtual bool Load( bool _dl = true );
	virtual bool Load( const std::string &_mapname, bool _dl = true ) = 0;
	virtual bool Save( const std::string &_mapname ) = 0;
	virtual void Unload() = 0;
	
	virtual const char *GetPlannerName() const = 0;
	virtual int GetPlannerType() const = 0;

	virtual void RegisterScriptFunctions( gmMachine *a_machine ) = 0;
		
	virtual bool GetNavInfo( const Vector3f &pos, int32_t &_id, std::string &_name ) = 0;

	virtual void AddEntityConnection( const EvEntityConnection &_conn ) = 0;
	virtual void RemoveEntityConnection( GameEntity _ent ) = 0;
	virtual void EntityCreated( const EntityInstance &ei );
	virtual void EntityDeleted( const EntityInstance &ei );

	virtual void QueueBatchQuery( QueryRef& qry, NavFlags inc, NavFlags exc, const Vector3f& src, const std::vector<Vector3f>& goals ) = 0;
	
	virtual size_t FindBorderEdges( NavFlags inc, NavFlags exc, NavFlags border, float minlen, const Vector3f& src, MeshEdge* edges, size_t maxEdges ) = 0;

	virtual PathInterface * AllocPathInterface();

	const AxisAlignedBox3f & GetNavigationBounds() const;
	
	PathPlannerBase();
	virtual ~PathPlannerBase();
protected:
	void InitCommands();
	void cmdBenchmarkPathFind( const StringVector & args );
	void cmdBenchmarkGetNavPoint( const StringVector & args );
	void cmdResaveNav( const StringVector & args );
	
	AxisAlignedBox3f	mNavigationBounds;

	//////////////////////////////////////////////////////////////////////////
	// Required subclass functions
	virtual std::string _GetNavFileExtension() = 0;
	virtual void _BenchmarkPathFinder( const StringVector & args );
	virtual void _BenchmarkGetNavPoint( const StringVector & args );
};

#endif
