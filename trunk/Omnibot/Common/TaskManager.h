#pragma once

#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_

#include <string>
#include <boost/thread.hpp>

#include "ConcurrentQueue.h"

class Task
{
public:
	friend class TaskManager;

	Task( const std::string & taskname = "Worker" )
		: mTaskName( taskname )
		, mTaskFinished( false )
	{
	}
	virtual ~Task() {}

	const char * GetTaskName() const { return mTaskName.c_str(); }
	virtual bool IsFinished() const { return mTaskFinished; }

	virtual void RunTask() = 0;
protected:
	std::string		mTaskName;
	bool			mTaskFinished;
};

class TaskManager
{
public:
	typedef boost::shared_ptr<Task> TaskPtr;
	TaskManager();
	~TaskManager();

	void AddTask( TaskPtr task );
protected:
private:
	static void TaskThreadFunc( TaskManager * taskMan );

	ConcurrentQueue< TaskPtr >	mTaskQueue;
	boost::thread_group			mThreadGroup;
};

#endif
