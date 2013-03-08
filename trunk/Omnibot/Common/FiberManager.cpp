////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: ken.nickel $
// $LastChangedDate: 2011-08-15 23:47:44 -0500 (Mon, 15 Aug 2011) $
// $LastChangedRevision: 351 $
//
////////////////////////////////////////////////////////////////////////////////

#include "FiberManager.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>

void Fiber::FiberRun( void * data )
{
	Fiber * fiber = static_cast<Fiber*>( data );
	fiber->Run();
	fiber->fiberState = FIBER_FINISHED;
	while( true ) {
		fiber->WaitFrame();
	}
}
Fiber::Fiber() :
	fiber( NULL ),
	lastFiber( NULL ),
	fiberState( FIBER_RUNNING )
{
	const int STACK_SIZE = 1024 * 1024;
	fiber = CreateFiber( STACK_SIZE, (LPFIBER_START_ROUTINE)Fiber::FiberRun, this );
}
Fiber::~Fiber()
{
	DeleteFiber( fiber );
}
Fiber::FiberState Fiber::Execute()
{
	lastFiber = GetCurrentFiber();
	SwitchToFiber( lastFiber );
	return fiberState;
}
void Fiber::WaitFrame()
{
	SwitchToFiber( lastFiber );
}
void Fiber::Wait( float timeSeconds )
{
	timeSeconds;
	// todo: set delay until run again
	SwitchToFiber( lastFiber );
}

//////////////////////////////////////////////////////////////////////////

FiberManager::FiberManager() {
}

void FiberManager::Init() {
	ConvertThreadToFiber( NULL );
}

void FiberManager::Shutdown() {
	ConvertFiberToThread();
}

void FiberManager::RunFibers() {
	for ( size_t i = 0; i < fibers.size(); ++i ) {
		fibers[ i ]->Execute();
	}
}

//////////////////////////////////////////////////////////////////////////