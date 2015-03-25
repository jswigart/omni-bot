////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __GOALMANAGER_H__
#define __GOALMANAGER_H__

#include <set>

#include "CommandReciever.h"

class System;
class Client;

struct EntityInstance;

// Title: GoalManager

// typedef: MapGoalPtr
class MapGoal;
typedef boost::shared_ptr<MapGoal> MapGoalPtr;
typedef boost::weak_ptr<MapGoal> MapGoalWPtr;

// typedef: MapGoalList
//		A list of goal entities.
typedef std::vector<MapGoalPtr> MapGoalList;
typedef std::set<MapGoalPtr> MapGoalSet;

// class: GoalManager
//		The goal manager is responsible for keeping track of various goals,
//		from flags to capture points. Bots can request goals from the goal
//		manager and the goal manager can assign goals to the bot based on
//		the needs of the game, and optionally the bot properties
class GoalManager : public CommandReciever
{
public:
	//////////////////////////////////////////////////////////////////////////
	class Query
	{
	public:
		friend class GoalManager;

		enum SortType
		{
			SORT_BIAS,
			SORT_NONE,
			SORT_RANDOM_FULL
		};

		enum QueryError
		{
			QueryOk,
			QueryBadNameExpression,
			QueryBadGroupExpression,
		};

		Query &AddType(uint32_t _type);
		Query &Team(int _team);
		Query &Bot(Client *_client);
		Query &TagName(const char *_tagname);
		Query &Sort(SortType _sort);
		Query &Expression(const char *_exp);
		Query &Group(const char *_exp);
		Query &RoleMask(uint32_t _i);
		Query &Ent(GameEntity _ent);
		Query &CheckInRadius(const Vector3f & pos, float radius);
		Query &CheckRangeProperty(bool checkRange);

		Query &NoFilters();
		Query &SkipDelayed(bool _skip);
		Query &SkipNoInProgress(bool _skip);
		Query &SkipNoInUse(bool _skip);
		Query &SkipInUse(bool _skip);

		QueryError GetError() const { return mError; }

		bool CheckForMatch(MapGoalPtr & mg);

		virtual void OnQueryStart();
		virtual void OnQueryFinish();
		virtual void OnMatch(MapGoalPtr & mg);

		bool GetBest(MapGoalPtr &_mg);

		Query(uint32_t _type = 0, Client *_client = 0);
		virtual ~Query() {}

		void DefaultGlobalQuery();
		void FromTable(gmMachine *a_machine, gmTableObject *a_table);

		const char *QueryErrorString();

		MapGoalList	 mList;

		enum { MaxGoalTypes = 8 };
	private:
		int			 mNumTypes;
		uint32_t	 mGoalTypeList[MaxGoalTypes];
		int			 mTeam;
		BitFlag32 	 mRoleMask;
		Client *	 mClient;
		const char * mTagName;
		SortType	 mSortType;
		GameEntity	 mEntity;

		Vector3f	 mPosition;
		float		 mRadius;

		std::string		 mNameExp;
		std::string		 mGroupExp;

		QueryError	 mError;

		// filters
		bool		 mSkipNoInProgressSlots;
		bool		 mSkipNoInUseSlots;
		bool		 mSkipDelayed;
		bool		 mSkipInUse;
		bool		 mCheckInRadius;
		bool		 mCheckRangeProperty;
	};
	//////////////////////////////////////////////////////////////////////////
	void InitGameGoals();

	void Init( System & system );
	void Update( System & system );
	void Shutdown();
	void Reset();

	bool Load(const std::string &_map, ErrorObj &_err);
	bool Save(const std::string &_map, ErrorObj &_err);

	MapGoalPtr AddGoal(const MapGoalDef &goaldef);

	void RemoveGoalsByType(const char *_goaltype);
	void RemoveGoalByEntity(GameEntity _ent);

	void GetGoals(Query &_qry);

	MapGoalPtr GetGoal(const std::string &_goalname);
	MapGoalPtr GetGoal(int _serialNum);
	
	const MapGoalList &GetGoalList() const { return mMapGoalList; }

	static GoalManager *GetInstance();
	static void DeleteInstance();
	void UpdateGoalEntity( GameEntity oldent, GameEntity newent );
	void RemoveGoalByName( const char *_goalname );
	
	void EntityCreated( const EntityInstance &ei );
	void EntityDeleted( const EntityInstance &ei );

	GoalManager();
	virtual ~GoalManager();
protected:

	// Commands
	void InitCommands();
	void cmdGoalShow(const StringVector &_args);
	void cmdGoalShowRoutes(const StringVector &_args);
	void cmdGoalDraw(const StringVector &_args);
	void cmdGoalDrawRoutes(const StringVector &_args);
	void cmdGoalLoad(const StringVector &_args);
	void cmdGoalSave(const StringVector &_args);
	void cmdGoalCreate(const StringVector &_args);
	void cmdGoalDelete(const StringVector &_args);
	void cmdGoalEdit(const StringVector &_args);
	void cmdGoalEditx(const StringVector &_args);
	void cmdGoalHelp(const StringVector &_args);
	void cmdGoalFinish(const StringVector &_args);
	void cmdGoalSetProperty(const StringVector &_args);
	void cmdGoalRemoveAll(const StringVector &_args);
	void cmdGoalMove(const StringVector &_args);

	enum EditMode
	{
		EditNone,
		EditMove,
	};

	void AddGoal(MapGoalPtr newGoal);

	MapGoalPtr _GetGoalInRange(const Vector3f &_pos, float _radius, bool _onlydrawenabled);
	void _SetActiveGoal(MapGoalPtr _mg);
	void _UpdateEditModes();

	static GoalManager	* mInstance;
private:
	MapGoalList	 mMapGoalList;

	MapGoalPtr	 mActiveGoal;

	EditMode	 mEditMode;

	MapGoalPtr	 mHighlightedGoal;

	gmGCRoot<gmTableObject> mLoadedMapGoals;

	void OnGoalDelete(const MapGoalPtr &_goal);

	void CheckEntityForGoal( const EntityInstance &ei );
};

#endif
