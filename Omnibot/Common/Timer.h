////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-09-24 22:34:55 -0700 (Wed, 24 Sep 2008) $
// $LastChangedRevision: 3482 $
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
	obint64		mLastTimer;
#else
	clock_t		mLastTimer;
#endif	
};

class GameTimer
{
public:

	void Delay(float _seconds);
	void DelayRandom(float _min, float _max);

	void Delay(int _ms);
	void DelayRandom(int _min, int _max);

	bool IsExpired() const;

	GameTimer();
private:
	int		m_TriggerTime;
};

#endif
