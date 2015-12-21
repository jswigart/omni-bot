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
	struct FloodFillOptions
	{
		float	 mCharacterHeight;
		float	 mCharacterCrouchHeight;
		float	 mCharacterStepHeight;
		float	 mCharacterJumpHeight;
		float	 mGridRadius;

		FloodFillOptions();
	};

	struct CellData
	{
		enum
		{
			TEAM1 = ( 1 << 0 ),
			TEAM2 = ( 1 << 1 ),
			TEAM3 = ( 1 << 2 ),
			TEAM4 = ( 1 << 3 ),
			WATER = ( 1 << 4 ),
			OPEN = ( 1 << 31 ),
		};

		unsigned int	mCellFlags;
	};

	//////////////////////////////////////////////////////////////////////////

	bool Init( System & system );
	void Update( System & system );
	void Shutdown();
	bool IsReady() const;
		
	bool Load( const std::string &_mapname, bool _dl = true );
	bool Save( const std::string &_mapname );
	void Unload();
	bool SetFileComments( const std::string &_text );

	void GetPath( Path &_path );

	virtual void RegisterScriptFunctions( gmMachine *a_machine );

	bool GetNavInfo( const Vector3f &pos, int32_t &_id, std::string &_name );

	void AddEntityConnection( const EvEntityConnection &_conn );
	void RemoveEntityConnection( GameEntity _ent );

	const char *GetPlannerName() const
	{
		return "Flood Fill Path Planner";
	};
	int GetPlannerType() const
	{
		return NAVID_FLOODFILL;
	};

	typedef SpanHeightMap<uint8_t> SpanMap;

	SpanMap::InfluenceMap * AllocInfluenceMap();
	PathInterface * AllocPathInterface();

	void EntityCreated( const EntityInstance &ei );
	void EntityDeleted( const EntityInstance &ei );

	PathPlannerFloodFill();
	virtual ~PathPlannerFloodFill();
protected:

	void InitCommands();
	void cmdNavSave( const StringVector & args );
	void cmdNavLoad( const StringVector & args );
	void cmdNavView( const StringVector & args );
	void cmdNavViewConnections( const StringVector & args );
	void cmdNavStep( const StringVector & args );
	void cmdNavEnableStep( const StringVector & args );
	void cmdAddFloodStart( const StringVector & args );
	void cmdClearFloodStarts( const StringVector & args );
	void cmdSaveFloodStarts( const StringVector & args );
	void cmdLoadFloodStarts( const StringVector & args );
	void cmdNavMeshFloodFill( const StringVector & args );
	void cmdAutoBuildFeatures( const StringVector & args );

	void cmdInfluenceMapCreate( const StringVector & args );
	void cmdInfluenceMapSeed( const StringVector & args );
	void cmdInfluenceMapMem( const StringVector & args );
	void cmdInfluenceMapSave( const StringVector & args );
	void cmdInfluenceMapLoad( const StringVector & args );
	void cmdInfluenceMapFlood( const StringVector & args );

	void cmdNext( const StringVector & args );

	// Process Functions
	int Process_FloodFill();
	void UpdateFloodFill();

	//////////////////////////////////////////////////////////////////////////
	void AddFloodStart( const Vector3f &_vec );
	void ClearFloodStarts();
	void SaveFloodStarts();
	void LoadFloodStarts();
	void FloodFill( const FloodFillOptions &_options );

	//////////////////////////////////////////////////////////////////////////
	// Friend functions
	friend int GM_CDECL gmfFloodFillView( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillViewConnections( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillEnableStep( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillStep( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillAddFloodStart( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillClearFloodStarts( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillLoadFloodStarts( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillSaveFloodStarts( gmThread *a_thread );
	friend int GM_CDECL gmfFloodFillFloodFill( gmThread *a_thread );

	//////////////////////////////////////////////////////////////////////////
protected:
	//////////////////////////////////////////////////////////////////////////

	Vector3List		 mStartPositions;

	typedef std::queue<Vector3f> VectorQueue;

	SpanMap 				mSpanMap;
	SpanMap::InfluenceMap *	mInfluence;
	VectorQueue				mSpanFrontier;

	uint32_t				mInfluenceBufferId;
	bool					mUpdateInfluenceBuffer;

	FloodFillOptions		mFillOptions;

	typedef std::vector<SpanMap::InfluenceMap*> InfluenceMaps;
	InfluenceMaps			mActiveInfluences;

	struct Obstacle
	{
		GameEntity				mEntity;

		Box3f					mObb;

		int32_t					mExpireTime;

		bool					mActive;

		bool Expired() const;
		bool IsActive() const;

		Obstacle();
		~Obstacle();
	};
	typedef std::vector<Obstacle> ObstacleList;

	ObstacleList				mObstacles;

	struct Flags
	{
		uint32_t					mViewMode : 1;
		uint32_t					mViewConnections : 1;
		uint32_t					mStepProcess : 1;

		Flags()
			: mViewMode( 0 )
			, mViewConnections( 0 )
			, mStepProcess( 0 )
		{
		}
	}								mFlags;

	//////////////////////////////////////////////////////////////////////////
	// Current tool variables
	obColor			 mCursorColor;

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension()
	{
		return ".nav";
	}
	virtual void _BenchmarkPathFinder( const StringVector & args );
	virtual void _BenchmarkGetNavPoint( const StringVector & args );

	void UpdateObstacles();
};

#endif
