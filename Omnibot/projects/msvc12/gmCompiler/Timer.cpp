////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-03-03 17:41:14 -0600 (Sun, 03 Mar 2013) $
// $LastChangedRevision: 817 $
//
////////////////////////////////////////////////////////////////////////////////

#include "Timer.h"

#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif

bool initialized = false; // a bit lame but whatever

#ifdef WIN32
LARGE_INTEGER iCounterFrequency;
#endif

void Init()
{
#ifdef WIN32
	QueryPerformanceFrequency(&iCounterFrequency);
#endif
	initialized = true;
}

Timer::Timer()
{
	if(!initialized)
		Init();

#ifdef WIN32
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	mLastTimer = li.QuadPart;
#else
	mLastTimer = clock();
#endif
}

void Timer::Reset()
{
#ifdef WIN32
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	mLastTimer = li.QuadPart;
#else
	mLastTimer = clock();
#endif
}

/*
Function: GetElapsedSeconds
Description:
Returns:
Returns the elapsed seconds since the timer was last reset.
*/
double Timer::GetElapsedSeconds()
{
#ifdef WIN32
	// Get the current count
	LARGE_INTEGER iCurrent, iLastCount;
	iLastCount.QuadPart = mLastTimer;
	QueryPerformanceCounter(&iCurrent);

	return double((iCurrent.QuadPart - iLastCount.QuadPart) /
		double(iCounterFrequency.QuadPart));
#else
	return static_cast<double>(clock() - mLastTimer) / CLOCKS_PER_SEC;
#endif
}