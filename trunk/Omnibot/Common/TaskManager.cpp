#include "TaskManager.h"

void TaskManager::TaskThreadFunc( TaskManager * taskMan )
{
	while ( true )
	{
		TaskManager::TaskPtr nextTask;
		if ( !taskMan->mTaskQueue.TryAndPop( nextTask ) )
			taskMan->mTaskQueue.WaitAndPop( nextTask );

		// A Null task signals abort
		if ( nextTask == NULL )
			break;

		nextTask->RunTask();
		nextTask->mTaskFinished = true;
	}
}

TaskManager::TaskManager()
{
	unsigned int numHardwareThreads = boost::thread::hardware_concurrency();

	for ( unsigned int i = 0; i < numHardwareThreads; ++i )
	{
		mThreadGroup.add_thread( new boost::thread( TaskManager::TaskThreadFunc, this ) );
	}
}

TaskManager::~TaskManager()
{
	unsigned int numHardwareThreads = boost::thread::hardware_concurrency();

	for ( unsigned int i = 0; i < numHardwareThreads; ++i )
		mTaskQueue.Push( TaskPtr() ); // null task means abort
	mThreadGroup.interrupt_all();
	mThreadGroup.join_all();
}

void TaskManager::AddTask( TaskPtr task )
{
	task->mTaskFinished = false;
	mTaskQueue.Push( task );
}