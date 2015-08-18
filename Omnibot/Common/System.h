#pragma once

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class IGame;
class ScriptManager;
class PathPlannerBase;
class GoalManager;
class TacticalManager;
class TriggerManager;
class NameManager;
class GameAnalytics;

class System
{
public:
	IGame *					mGame;
	ScriptManager *			mScript;
	PathPlannerBase *       mNavigation;
	GoalManager	*			mGoalManager;
	TacticalManager*		mTacticalManager;
	TriggerManager *		mTriggerManager;
	NameManager *			mNameManager;
	GameAnalytics *			mAnalytics;

	static System * mInstance;

	System();
private:
};

#endif