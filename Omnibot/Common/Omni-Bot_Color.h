////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __OMNICOLOR_H__
#define __OMNICOLOR_H__

#include <stdint.h>

// class: obColor
//		Helper class for defining color values.
union obColor
{
public:
	obColor()
	{
		// initialize to white
		mElements.mR = 255;
		mElements.mG = 255;
		mElements.mB = 255;
		mElements.mA = 255; // 255 is opaque, 0 is transparent
	}
	explicit obColor(uint32_t _color)
	{
		mRGBA = _color;
	}
	obColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
	{
		mElements.mR = _r;
		mElements.mG = _g;
		mElements.mB = _b;
		mElements.mA = _a; // 255 is opaque, 0 is transparent
	}
	obColor( const float * flt )
	{
		*this = FromFloat( flt[ 0 ], flt[ 1 ], flt[ 2 ], flt[ 3 ] );
	}

	operator int() const
	{
		return mRGBA;
	}

	inline uint8_t r() const	{ return mElements.mR; }
	inline uint8_t g() const	{ return mElements.mG; }
	inline uint8_t b() const	{ return mElements.mB; }
	inline uint8_t a() const	{ return mElements.mA; }

	inline float rF() const	{ return (float)mElements.mR / 255.0f; }
	inline float gF() const	{ return (float)mElements.mG / 255.0f; }
	inline float bF() const	{ return (float)mElements.mB / 255.0f; }
	inline float aF() const	{ return (float)mElements.mA / 255.0f; }

	inline void set_r( uint8_t v )	{ mElements.mR = v; }
	inline void set_g( uint8_t v )	{ mElements.mG = v; }
	inline void set_b( uint8_t v )	{ mElements.mB = v; }
	inline void set_a( uint8_t v )	{ mElements.mA = v; }
	
	inline obColor fade(uint8_t _a) const { obColor c(mRGBA); c.mElements.mA=_a; return c; }

	static obColor FromFloat(float _r, float _g, float _b, float _a = 1)
	{
		obColor c;
		c.mElements.mR = (uint8_t)( _r * 255.f );
		c.mElements.mG = (uint8_t)( _g * 255.f );
		c.mElements.mB = (uint8_t)( _b * 255.f );
		c.mElements.mA = (uint8_t)( _a * 255.f );// 255 is opaque, 0 is transparent
		return c;
	}

	static obColor lerp( const obColor & from, const obColor & to, float t )
	{
		return FromFloat(
			from.rF() + (to.rF() - from.rF()) * t,
			from.gF() + (to.gF() - from.gF()) * t,
			from.bF() + (to.bF() - from.bF()) * t,
			from.aF() + (to.aF() - from.aF()) * t );
	}

	inline uint32_t rgba() const { return mRGBA; }
	inline uint32_t argb() const { return obColor( a(), r(), g(), b() ); }
	inline uint32_t abgr() const { return obColor( a(), b(), g(), r() ); }

	void MultRGB( float s )
	{
		mElements.mR = (uint8_t)( (float)mElements.mR * s );
		mElements.mG = (uint8_t)( (float)mElements.mG * s );
		mElements.mB = (uint8_t)( (float)mElements.mB * s );
	}
private:
	struct
	{
		uint8_t		mR;
		uint8_t		mG;
		uint8_t		mB;
		uint8_t		mA;
	}				mElements;

	uint32_t		mRGBA;
};

namespace COLOR
{
	const obColor RED			= obColor(255, 0, 0);
	const obColor GREEN			= obColor(0, 255, 0);
	const obColor BLUE			= obColor(0, 0, 255);
	const obColor BLACK			= obColor(0, 0, 0);
	const obColor WHITE			= obColor(255, 255, 255);
	const obColor MAGENTA		= obColor(255, 0, 255);
	const obColor GREY			= obColor(127, 127, 127);
	const obColor LIGHT_GREY	= obColor(211, 211, 211);
	const obColor ORANGE		= obColor(255, 127, 0);
	const obColor YELLOW		= obColor(255, 255, 0);
	const obColor CYAN			= obColor(0, 255, 255);
	const obColor PINK			= obColor(255, 20, 147);
	const obColor BROWN			= obColor(165, 42, 42);
	const obColor AQUAMARINE	= obColor(127, 255, 212);
	const obColor LAVENDER		= obColor(230, 230, 250);
};

#endif
