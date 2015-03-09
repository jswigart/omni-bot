////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERNAVMESH_PATHINTERFACE_H__
#define __PATHPLANNERNAVMESH_PATHINTERFACE_H__

#include "PathPlannerNavMesh.h"
#include "platformspecifics.h"

class PathFindNavMesh
{
public:
	struct PlanNode
	{
		PathPlannerNavMesh::RuntimeNavSector *	Sector;
		Vector3f								Position;
		PlanNode *								Parent;
		const PathPlannerNavMesh::NavPortal *	Portal;
		
		float									CostHeuristic;
		float									CostGiven;
		float									CostFinal;

		obint32 Hash() const;

		void Reset();
	};
	typedef std::vector<PlanNode*> NodeList;

	NodeList &GetSolution();

	bool IsFinished() const;
	bool FoundGoal() const;
	int GetGoalIndex() const;
	void StartNewSearch();
	bool AddStart(const Vector3f &_pos);
	bool AddGoal(const Vector3f &_pos);
	void Iterate(int _numsteps = 1);
	void Render();
	PathFindNavMesh( PathPlannerNavMesh * navmesh );
private:
	PathPlannerNavMesh *			mNavMesh;
	struct GoalLocation
	{
		PathPlannerNavMesh::RuntimeNavSector	*Sector;
		Vector3f								Position;
	};
	typedef std::vector<GoalLocation> GoalList;
	GoalList	m_CurrentGoals;

	enum		{ MaxNodes=2048 };
	PlanNode	m_Nodes[MaxNodes];
	int			m_UsedNodes;

#ifdef WIN32
	typedef stdext::hash_compare<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare/*, HashMapAllocator*/ > NavHashMap;
#else
	typedef stdext::hash<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare, stdext::equal_to<int>/*, HashMapAllocator*/ > NavHashMap;
#endif
	NodeList			m_OpenList;
	NodeList			m_Solution;

	typedef NavHashMap NodeClosedList;
	NavHashMap			m_ClosedList;

	int					FoundGoalIndex;

	PlanNode *_GetFreeNode();
	PlanNode *_GetNextOpenNode();
	void _MarkClosed(PlanNode *_node);
	NodeClosedList::iterator IsOnClosedList(PlanNode *_node);
	NodeList::iterator IsOnOpenList(PlanNode *_node);
	
	void HeapInsert(NodeList &_wpl, PlanNode *_node);
	void _ExpandNode(PlanNode *_node, GoalLocation *_goal);
	GoalLocation *_IsNodeGoalInSector(PlanNode *_node);
};

class NavMeshPathInterface : public PathInterface
{
public:
	NavMeshPathInterface( Client * client, PathPlannerNavMesh * nav );
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
	struct NavMeshNode
	{
		Vector3f								mPos;
		const PathPlannerNavMesh::NavPortal *	mPortal;
		NavFlags								mFlags;

		PathPlannerNavMesh::RuntimeNavSector *	mSector;
		obuint32								mSectorVersion;
	};

	typedef std::vector<NavMeshNode> NavMeshPath;

	NavMeshPath			mPath;
	size_t				mPathNodeIndex;

	Client *			mClient;
	PathPlannerNavMesh *mNavMesh;
	PathStatus			mStatus;

	Vector3f			mSrc;
	DestinationVector	mGoals;

	Vector3f			mMoveDirection;

	int					mFoundGoalIndex;

	PathFindNavMesh		mPathFind;

	PathPlannerNavMesh::RuntimeNavSector *	mCurrentSector;
	CollisionModel::CacheId					mCurrentSectorCache; 
	
	bool				mSrcChanged;
	
	void UpdateCurrentSector();
};


#endif
