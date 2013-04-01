////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERFLOODFILL_H__
#define __PATHPLANNERFLOODFILL_H__

#include "PathPlannerBase.h"
#include "InternalFsm.h"
#include "SpanHeightMap.h"

//////////////////////////////////////////////////////////////////////////

// class: PathPlannerFloodFill
//		Path planner interface for the navmesh system for hl2
class PathPlannerFloodFill : public PathPlannerBase
{
public:
	enum NavMeshFlags
	{
		NAVMESH_STEPPROCESS = NUM_BASE_NAVFLAGS,
		NAVMESH_TAKESTEP,
	};

	struct FloodFillOptions
	{
		float		m_CharacterHeight;
		float		m_CharacterCrouchHeight;
		float		m_CharacterStepHeight;
		float		m_CharacterJumpHeight;
		float		m_GridRadius;
	};

	struct CellData
	{
		enum
		{
			TEAM1		= ( 1<<0 ),
			TEAM2		= ( 1<<1 ),
			TEAM3		= ( 1<<2 ),
			TEAM4		= ( 1<<3 ),
			WATER		= ( 1<<4 ),
			OPEN		= ( 1<<31 ),
		};

		unsigned int	mCellFlags;
	};

	//////////////////////////////////////////////////////////////////////////

	bool Init( System & system );
	void Update( System & system );
	void Shutdown();
	bool IsReady() const;

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
	void GetPath(Path &_path);

	virtual void RegisterNavFlag(const std::string &_name, const NavFlags &_bits);
	virtual void RegisterScriptFunctions(gmMachine *a_machine);

	bool GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name);

	void AddEntityConnection(const Event_EntityConnection &_conn);
	void RemoveEntityConnection(GameEntity _ent);

	const char *GetPlannerName() const { return "Flood Fill Path Planner"; } ;
	int GetPlannerType() const { return NAVID_FLOODFILL; };

	int GetLatestFileVersion() const { return 1; }

	PathPlannerFloodFill();
	virtual ~PathPlannerFloodFill();
protected:

	void InitCommands();
	void cmdNavSave(const StringVector &_args);
	void cmdNavLoad(const StringVector &_args);
	void cmdNavView(const StringVector &_args);
	void cmdNavViewConnections(const StringVector &_args);
	void cmdNavStep(const StringVector &_args);
	void cmdNavEnableStep(const StringVector &_args);
	void cmdAddFloodStart(const StringVector &_args);
	void cmdClearFloodStarts(const StringVector &_args);
	void cmdSaveFloodStarts(const StringVector &_args);
	void cmdLoadFloodStarts(const StringVector &_args);
	void cmdNavMeshFloodFill(const StringVector &_args);
	void cmdAutoBuildFeatures(const StringVector &_args);

	void cmdNext(const StringVector &_args);

	// Process Functions
	int Process_FloodFill();

	//////////////////////////////////////////////////////////////////////////
	void AddFloodStart(const Vector3f &_vec);
	void ClearFloodStarts();
	void SaveFloodStarts();
	void LoadFloodStarts();
	void FloodFill(const FloodFillOptions &_options);

	//////////////////////////////////////////////////////////////////////////
	// Friend functions
	friend int GM_CDECL gmfFloodFillView(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillViewConnections(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillEnableStep(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillStep(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillAddFloodStart(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillClearFloodStarts(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillLoadFloodStarts(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillSaveFloodStarts(gmThread *a_thread);
	friend int GM_CDECL gmfFloodFillFloodFill(gmThread *a_thread);

	//////////////////////////////////////////////////////////////////////////
protected:
	//////////////////////////////////////////////////////////////////////////

	Vector3List			m_StartPositions;

	typedef SpanHeightMap< CellData > SpanMap;
	SpanMap				mSpanMap;

	//////////////////////////////////////////////////////////////////////////
	// Tool state machine
	//STATE_PROTOTYPE(NoOp);

	//////////////////////////////////////////////////////////////////////////
	// Current tool variables
	obColor				m_CursorColor;

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension() { return ".nav"; }
	virtual void _BenchmarkPathFinder(const StringVector &_args);
	virtual void _BenchmarkGetNavPoint(const StringVector &_args);

	void _Render();
};

#endif
