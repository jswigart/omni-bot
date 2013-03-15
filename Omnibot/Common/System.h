#pragma once

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class IGame;
class ScriptManager;
class PathPlannerBase;
class GoalManager;
class TaskManager;
class TriggerManager;
class NameManager;

class System
{
public:
	IGame *					mGame;
	ScriptManager *			mScript;
	PathPlannerBase *       mNavigation;
	GoalManager	*			mGoalManager;
	TaskManager *			mTaskManager;
	TriggerManager *		mTriggerManager;
	NameManager *			mNameManager;

	static System * mInstance;

	System();
private:
};

#endif