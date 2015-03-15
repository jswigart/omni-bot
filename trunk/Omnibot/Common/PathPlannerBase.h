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
struct Event_EntityConnection;

#include "EventReciever.h"
#include "CommandReciever.h"
#include "PathQuery.h"

//enum NavigationAreas
//{
//	NAV_AREA_GROUND,
//	NAV_AREA_WATER,
//	NAV_AREA_CROUCH,
//	NAV_AREA_HARMFUL,
//	NAV_AREA_AVOID,
//	NAV_AREA_LADDER,
//};
//
//enum NavigationFlags
//{
//	NAV_FLAG_TEAM1 = ( 1 << 0 ),
//	NAV_FLAG_TEAM2 = ( 1 << 1 ),
//	NAV_FLAG_TEAM3 = ( 1 << 2 ),
//	NAV_FLAG_TEAM4 = ( 1 << 3 ),
//	NAV_FLAG_WALK = ( 1 << 4 ),
//	NAV_FLAG_JUMP = ( 1 << 5 ),
//	NAV_FLAG_LADDER = ( 1 << 6 ),
//
//	NAV_FLAG_MOD = ( 1 << 10 ),
//	//NAV_FLAG_TELEPORT	= (1<<2),
//	//NAV_FLAG_ALLTEAMS	= NAV_FLAG_TEAM1|NAV_FLAG_TEAM2|NAV_FLAG_TEAM3|NAV_FLAG_TEAM4
//};

enum NavArea
{
	NAVAREA_GROUND = 0,
	NAVAREA_WATER,
	NAVAREA_MOVER,
	NAVAREA_REGION,
	NAVAREA_JUMP,
	NAVAREA_LADDER,
	NAVAREA_TELEPORT,
	NAVAREA_DOOR,
	NAVAREA_ROCKETJUMP,
	NAVAREA_PUSHABLE,
	NAVAREA_ANY = 255,
};

enum NavAreaFlags
{
	NAVFLAGS_NONE = 0,
	NAVFLAGS_WALK = ( 1 << 0 ),
	NAVFLAGS_TEAM1_ONLY = ( 1 << 1 ),		// Team Specific
	NAVFLAGS_TEAM2_ONLY = ( 1 << 2 ),		// Team Specific
	NAVFLAGS_TEAM3_ONLY = ( 1 << 3 ),		// Team Specific
	NAVFLAGS_TEAM4_ONLY = ( 1 << 4 ),		// Team Specific
	NAVFLAGS_SWIM = ( 1 << 5 ),		// Ability to swim (water).

	//NAVFLAGS_DOOR = ( 1 << 2 ),		// Ability to move through doors.
	//NAVFLAGS_JUMP = ( 1 << 3 ),		// Ability to jump.

	//NAVFLAGS_LADDER = ( 1 << 5 ),		// Ladder
	//NAVFLAGS_TELEPORT = ( 1 << 6 ),		// Teleport	

	NAVFLAGS_DISABLED = ( 1 << 15 ),		// Disabled polygon

	NAVFLAGS_PATH_START = ( 1 << 29 ),
	NAVFLAGS_PATH_GOAL = ( 1 << 30 ),
	NAVFLAGS_PATH_LINK = ( 1 << 31 ),
};

static const NavFlags sTeamMask = ( NAVFLAGS_TEAM1_ONLY | NAVFLAGS_TEAM2_ONLY | NAVFLAGS_TEAM3_ONLY | NAVFLAGS_TEAM4_ONLY );

namespace NavigationAssertions
{
	BOOST_STATIC_ASSERT(sizeof(NavFlags) == 8); // 8 bytes = 64 bits
	//BOOST_STATIC_ASSERT(sizeof(obUserData) == 16); // cs: FIXME 64 bit struct size is different. do we really need this?
}

struct EntityInstance;

class PathInterface
{
public:
	PathInterface() {}
	virtual ~PathInterface() {}

	enum PathStatus
	{
		PATH_NONE,
		PATH_VALID,
		PATH_SEARCHING,
		PATH_NOPATHTOGOAL,
	};

	struct PathCorner
	{
		Vector3f		mPos;

		NavArea			mArea;
		NavAreaFlags	mFlags;
		
		PathCorner() 
			: mArea( NAVAREA_GROUND )
			, mFlags( NAVFLAGS_NONE )
		{
		}
	};

	virtual PathStatus GetPathStatus() const = 0;

	virtual void UpdateSourcePosition( const Vector3f & srcPos ) = 0;
	virtual void UpdateGoalPosition( const Vector3f & goal, float radius ) = 0;
	virtual void UpdateGoalPositions( const DestinationVector & goals ) = 0;
	virtual bool UpdateGoalPositionRandom() = 0;

	virtual void UpdatePath() = 0;

	virtual void Cancel() = 0;

	virtual size_t GetPathCorners( PathCorner * corners, size_t maxEdges ) = 0;
	virtual bool GetPointAlongPath( float lookAheadDist, Vector3f & ptOut ) = 0;

	virtual void Render() = 0;
};

template< typename T >
class EditTool
{
public:
	EditTool( const char * toolname ) : mToolName( toolname ) {}
	virtual ~EditTool() {}

	virtual bool Enter( T * system ) = 0;
	virtual bool ReEnter( T * system ) { return true; }
	virtual bool Update( T * system ) = 0;
	virtual void Commit( T * system ) = 0;
	virtual void Undo( T * system ) { EngineFuncs::ConsoleError( va( "No Undo functionality for tool %s",mToolName ) ); }
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

	enum BasePlannerFlags
	{
		NAV_VIEW,
		NAV_VIEWCONNECTIONS,
		NAV_FOUNDGOAL,
		NAV_SAVEFAILEDPATHS,
		NAV_AUTODETECTFLAGS,
		NAV_VIEWFLAGS,

		NUM_BASE_NAVFLAGS
	};

	virtual bool Init( System & system ) = 0;
	virtual void Update( System & system ) = 0;
	virtual void Shutdown() = 0;
	virtual bool IsReady() const = 0;

	virtual bool GetRandomDestination( Vector3f & dstOut, Client *_client ) { return false; }

	virtual void RunPathQuery(const PathQuery &_qry) {};
	
	virtual bool GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const = 0;

	virtual Vector3f GetDisplayPosition(const Vector3f &_pos) = 0; // deprecated

	bool IsViewOn() const { return m_PlannerFlags.CheckFlag(NAV_VIEW); }
	bool IsViewConnectionOn() const { return m_PlannerFlags.CheckFlag(NAV_VIEWCONNECTIONS); }
	bool IsAutoDetectFlagsOn() const { return m_PlannerFlags.CheckFlag(NAV_AUTODETECTFLAGS); }

	virtual bool Load(bool _dl = true);
	virtual bool Load(const std::string &_mapname,bool _dl = true) = 0;
	virtual bool Save(const std::string &_mapname) = 0;
	virtual void Unload() = 0;

	virtual int GetLatestFileVersion() const = 0;

	virtual void RegisterGameGoals() = 0;

	virtual const char *GetPlannerName() const = 0;
	virtual int GetPlannerType() const = 0;

	virtual void RegisterScriptFunctions(gmMachine *a_machine) = 0;

	struct AvoidData
	{
		int		m_Team;
		float	m_Radius;
		float	m_AvoidWeight;
	};

	virtual void MarkAvoid(const Vector3f &_pos, const AvoidData&_data) {}

	virtual bool GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name) = 0;

	virtual void AddEntityConnection(const Event_EntityConnection &_conn) = 0;
	virtual void RemoveEntityConnection(GameEntity _ent) = 0;
	virtual void EntityCreated(const EntityInstance &ei) {}
	virtual void EntityDeleted(const EntityInstance &ei) {}

#ifdef ENABLE_REMOTE_DEBUGGING
	virtual void Sync( RemoteLib::DataBuffer & db, bool fullSync ) { }
#endif

	virtual PathInterface * AllocPathInterface( Client * client ) { return NULL; }

	const AxisAlignedBox3f & GetNavigationBounds() const { return mNavigationBounds; }

	PathPlannerBase();
	virtual ~PathPlannerBase();
protected:
	struct FailedPath
	{
		Vector3f	m_Start;
		Vector3f	m_End;
		bool		m_Render;
	};
	typedef std::list<FailedPath> FailedPathList;

	FailedPathList	m_FailedPathList;

	void AddFailedPath(const Vector3f &_start, const Vector3f &_end);

	void RenderFailedPaths();

	void InitCommands();
	void cmdLogFailedPaths(const StringVector &_args);
	void cmdShowFailedPaths(const StringVector &_args);
	void cmdBenchmarkPathFind(const StringVector &_args);
	void cmdBenchmarkGetNavPoint(const StringVector &_args);
	void cmdResaveNav(const StringVector &_args);

	BitFlag32			m_PlannerFlags;

	AxisAlignedBox3f	mNavigationBounds;

	//////////////////////////////////////////////////////////////////////////
	// Required subclass functions
	virtual std::string _GetNavFileExtension() = 0;
	virtual void _BenchmarkPathFinder(const StringVector &_args);
	virtual void _BenchmarkGetNavPoint(const StringVector &_args);
};

#endif
