////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PIDCONTROLLER_H__
#define __PIDCONTROLLER_H__

class PIDController
{
public:
	PIDController( float kp, float ki, float kd )
		: mP( kp ), mI( ki ), mD( kd )
	{
		mE0 = 0.f;
		mE1 = 0.f;
		mE2 = 0.f;
		mControlValue = 0.f;
	}

	void Update( float _target, float _current, float _dt )
	{
		if ( _dt <= 0.f )
			return;

		mE2 = mE1;
		mE1 = mE0;
		mE0 = _target - _current;
		float const    e01 = mE0 - mE1;
		float const    e12 = mE1 - mE2;
		float const    p = mP * e01;
		float const    i = mI * mE0 * _dt;
		float const    d = mD * ( e01 - e12 ) / _dt;
		mControlValue += p + i + d;
	}

	float GetP() const
	{
		return mP;
	}
	float GetI() const
	{
		return mI;
	}
	float GetD() const
	{
		return mD;
	}

	void SetP( float _p )
	{
		mP = _p;
	}
	void SetI( float _i )
	{
		mI = _i;
	}
	void SetD( float _d )
	{
		mD = _d;
	}

	void Reset()
	{
		mE0 = 0.f;
		mE1 = 0.f;
		mE2 = 0.f;
		mControlValue = 0.f;
	}

	float GetControlValue() const
	{
		return mControlValue;
	}
private:
	float    mControlValue;
	float    mP, mI, mD;
	float    mE0, mE1, mE2; // errors
};

#endif
