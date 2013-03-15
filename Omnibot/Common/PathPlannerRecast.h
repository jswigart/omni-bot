////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERRECAST_H__
#define __PATHPLANNERRECAST_H__

#include "PathPlannerBase.h"
#include "InternalFsm.h"

//////////////////////////////////////////////////////////////////////////

// class: PathPlannerRecast
//		Path planner interface for the navmesh system for hl2
class PathPlannerRecast : public PathPlannerBase
{
public:
	//////////////////////////////////////////////////////////////////////////

	bool Init( System & system );
	void Update( System & system );
	void Shutdown();
	bool IsReady() const;

	int GetLatestFileVersion() const;

	Vector3f GetRandomDestination(Client *_client, const Vector3f &_start, const NavFlags _team);

	void PlanPathToGoal(Client *_client, const Vector3f &_start, const Vector3f &_goal, const NavFlags _team);
	int PlanPathToNearest(Client *_client, const Vector3f &_start, const Vector3List &_goals, const NavFlags &_team);
	int PlanPathToNearest(Client *_client, const Vector3f &_start, const DestinationVector &_goals, const NavFlags &_team);

	bool GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const;

	Vector3f GetDisplayPosition(const Vector3f &_pos);

	bool IsDone() const;
	bool FoundGoal() const;
	bool Load(const std::string &_mapname, bool _dl = true);
	bool Save(const std::string &_mapname);
	void Unload();
	bool SetFileComments(const std::string &_text);

	void RegisterGameGoals();
	void GetPath(Path &_path, int _smoothiterations);

	virtual void RegisterNavFlag(const std::string &_name, const NavFlags &_bits);
	virtual void RegisterScriptFunctions(gmMachine *a_machine);

	bool GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name);

	void AddEntityConnection(const Event_EntityConnection &_conn);
	void RemoveEntityConnection(GameEntity _ent);
	void EntityCreated(const EntityInstance &ei);
	void EntityDeleted(const EntityInstance &ei);

	void FloodFill();
	void BuildNav();

	void AddFloodSeed(const Vector3f &_vec);
	void AddFloodEntityBounds(const AABB &_bnds);

	const char *GetPlannerName() const { return "Recast Path Planner"; } ;
	int GetPlannerType() const { return NAVID_RECAST; };

	PathPlannerRecast();
	virtual ~PathPlannerRecast();
protected:

	void InitCommands();
	void cmdNavSave(const StringVector &_args);
	void cmdNavLoad(const StringVector &_args);
	void cmdNavView(const StringVector &_args);
	void cmdNavViewConnections(const StringVector &_args);

	void cmdAddFloodSeed(const StringVector &_args);
	void cmdFloodFill(const StringVector &_args);
	void cmdBuildNav(const StringVector &_args);
	//void cmdAutoBuildFeatures(const StringVector &_args);
	void cmdCreateLadder(const StringVector &_args);

	// Process Functions
	int Process_FloodFill();

	//////////////////////////////////////////////////////////////////////////
	// Friend functions
	friend int GM_CDECL gmfRecastView(gmThread *a_thread);
	friend int GM_CDECL gmfRecastViewConnections(gmThread *a_thread);
	friend int GM_CDECL gmfRecastEnableStep(gmThread *a_thread);
	friend int GM_CDECL gmfRecastStep(gmThread *a_thread);
	friend int GM_CDECL gmfRecastAddFloodSeed(gmThread *a_thread);
	friend int GM_CDECL gmfRecastBuildNavMesh(gmThread *a_thread);
	friend int GM_CDECL gmfRecastFloodFill(gmThread *a_thread);
	friend int GM_CDECL gmfRecastTrimSectors(gmThread *a_thread);

	//////////////////////////////////////////////////////////////////////////

	friend class PathFindFloodFill;
protected:

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension() { return ".nav"; }
	virtual void _BenchmarkPathFinder(const StringVector &_args);
	virtual void _BenchmarkGetNavPoint(const StringVector &_args);

private:
	struct ladder_t {
		Vector3f	top;
		Vector3f	bottom;
		Vector3f	normal;
		float		width;

		bool OverLaps(const ladder_t & other) const;
		//void Render(RenderOverlay *overlay) const;
	};
	typedef std::vector<ladder_t> LadderList;
	LadderList	ladders;
};

#endif
