////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __REGULATOR_H__
#define __REGULATOR_H__

#include "common.h"

// class: Regulator
//		Simple Regulator class that simply provides a true/false return
//		type through IsReady() for simple checking if time interval has elapsed
//		It is also self resetting
class Regulator
{
public:

	bool IsReady();
	void SetMsInterval( int _msInterval )
	{
		mUpdateInterval = _msInterval;
	}
	void SetTickRate( int _hz )
	{
		mUpdateInterval = 1000 / _hz;
	}
	int GetInterval() const
	{
		return mUpdateInterval;
	}

	Regulator( int _msInterval = 0 )
		: mUpdateInterval( _msInterval )
		, mNextUpdateTime( 0 )
	{
	}
protected:
	int	 mUpdateInterval;
	int	 mNextUpdateTime;
};

// typedef: RegulatorPtr
//		Smart pointer to a regulator
typedef std::shared_ptr<Regulator> RegulatorPtr;

#endif
