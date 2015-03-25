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
class obColor
{
public:
	obColor()
	{
		// initialize to white
		cdata.mRGBA[0] = 255;
		cdata.mRGBA[1] = 255;
		cdata.mRGBA[2] = 255;
		cdata.mRGBA[3] = 255; // 255 is opaque, 0 is transparent
	}
	explicit obColor(int32_t _color)
	{
		cdata.mRGBAi = _color;
	}
	obColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
	{
		cdata.mRGBA[0] = _r;
		cdata.mRGBA[1] = _g;
		cdata.mRGBA[2] = _b;
		cdata.mRGBA[3] = _a; // 255 is opaque, 0 is transparent
	}
	obColor( const float * flt )
	{
		*this = FromFloat( flt[ 0 ], flt[ 1 ], flt[ 2 ], flt[ 3 ] );
	}

	operator int() const
	{
		return cdata.mRGBAi;
	}

	inline uint8_t r() const	{ return cdata.mRGBA[0]; }
	inline uint8_t g() const	{ return cdata.mRGBA[1]; }
	inline uint8_t b() const	{ return cdata.mRGBA[2]; }
	inline uint8_t a() const	{ return cdata.mRGBA[3]; }

	inline float rF() const	{ return (float)cdata.mRGBA[0] / 255.0f; }
	inline float gF() const	{ return (float)cdata.mRGBA[1] / 255.0f; }
	inline float bF() const	{ return (float)cdata.mRGBA[2] / 255.0f; }
	inline float aF() const	{ return (float)cdata.mRGBA[3] / 255.0f; }

	inline obColor fade(uint8_t _a) const { obColor c(cdata.mRGBAi); c.cdata.mRGBA[3]=_a; return c; }

	static obColor FromFloat(float _r, float _g, float _b, float _a = 1)
	{
		obColor c;
		c.cdata.mRGBA[0] = (uint8_t)(_r * 255.f);
		c.cdata.mRGBA[1] = (uint8_t)(_g * 255.f);
		c.cdata.mRGBA[2] = (uint8_t)(_b * 255.f);
		c.cdata.mRGBA[3] = (uint8_t)(_a * 255.f);// 255 is opaque, 0 is transparent
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

	inline int32_t rgba() const { return cdata.mRGBAi; }
	inline int32_t argb() const { return obColor( a(), r(), g(), b() ); }
private:
	union cdatatype
	{
		uint8_t	 mRGBA[4];
		int32_t	 mRGBAi;
	} cdata;
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
