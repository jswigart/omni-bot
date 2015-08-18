////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TRIGGERMANAGER_H__
#define __TRIGGERMANAGER_H__

#include "CommandReciever.h"
#include "GoalManager.h"

class System;
class TriggerShape;
class gmFunctionObject;

typedef std::shared_ptr<TriggerShape> ShapePtr;
typedef std::vector<ShapePtr> ShapeList;

bool operator<( const GameEntity& _1, const GameEntity& _2 );

//////////////////////////////////////////////////////////////////////////

// class: TriggerManager
//		Handles triggers recieved from the game. Calls necessary script
//		or function callbacks for any trigger that has callbacks registered
class TriggerManager : public CommandReciever
{
public:
	// typedef: TriggerSignal
	typedef std::map<GameEntity, MapGoalWPtr> TriggerMap;

	// typedef: ScriptCallback
	typedef std::multimap<std::string, gmGCRoot<gmFunctionObject> > ScriptCallback;

	void SetScriptCallback( const std::string &_name, gmGCRoot<gmFunctionObject> _func );

	void HandleTrigger( const TriggerInfo &_triggerInfo );

	void Update( System & system );

	//////////////////////////////////////////////////////////////////////////
	int AddTrigger( const Vector3f &_pos, float _radius, gmMachine *_m, gmTableObject *tbl );
	int AddTrigger( const Box3f & obb, gmMachine *_m, gmTableObject *tbl );
	void DeleteTrigger( int _serial );
	void DeleteTrigger( const std::string &_name );
	//////////////////////////////////////////////////////////////////////////

	static TriggerManager *GetInstance();
	static void DeleteInstance();
protected:
	ScriptCallback	 mScriptCallbacks;

	ShapeList		 mTriggerShapes;

	// Commands
	virtual void InitCommands();
	void cmdDebugTriggers( const StringVector & args );
	void cmdDrawTriggers( const StringVector & args );

	std::string  mDebugTriggersExpr;

	bool mDebugTriggers;
	bool mDrawTriggers;

	static TriggerManager	* mInstance;

	TriggerManager();
	~TriggerManager()
	{
	}
	TriggerManager &operator=( const TriggerManager& );
};

#endif