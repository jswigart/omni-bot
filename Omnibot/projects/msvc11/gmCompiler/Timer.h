////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TIMER_H__
#define __TIMER_H__

// class: Timer
//		High performance timer class for timing functions, with
//		implementations for windows and linux.
class Timer
{
public:
	Timer();

	/*
	Function: Reset
	Description: Resets the timer to 0	
	*/
	void Reset();

	/*
	Function: GetElapsedSeconds
	Description:		
	Returns: 
		Returns the elapsed seconds since the timer was last reset.
	*/
	double GetElapsedSeconds();

protected:
#ifdef WIN32
	long long int mLastTimer;
#else
	clock_t			mLastTimer;
#endif	
};

#endif
