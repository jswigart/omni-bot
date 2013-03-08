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

#include "Omni-Bot_BasicTypes.h"

// class: obColor
//		Helper class for defining color values.
class obColor
{
public:
	obColor()
	{
		// initialize to white
		cdata.m_RGBA[0] = 255;
		cdata.m_RGBA[1] = 255;
		cdata.m_RGBA[2] = 255;
		cdata.m_RGBA[3] = 255; // 255 is opaque, 0 is transparent
	}
	obColor(obint32 _color)
	{
		cdata.m_RGBAi = _color;
	}
	obColor(obuint8 _r, obuint8 _g, obuint8 _b, obuint8 _a = 255)
	{
		cdata.m_RGBA[0] = _r;
		cdata.m_RGBA[1] = _g;
		cdata.m_RGBA[2] = _b;
		cdata.m_RGBA[3] = _a; // 255 is opaque, 0 is transparent
	}

	operator int() const
	{
		return cdata.m_RGBAi;
	}

	inline obuint8 r() const	{ return cdata.m_RGBA[0]; }
	inline obuint8 g() const	{ return cdata.m_RGBA[1]; }
	inline obuint8 b() const	{ return cdata.m_RGBA[2]; }
	inline obuint8 a() const	{ return cdata.m_RGBA[3]; }

	inline float rF() const	{ return (float)cdata.m_RGBA[0] / 255.0f; }
	inline float gF() const	{ return (float)cdata.m_RGBA[1] / 255.0f; }
	inline float bF() const	{ return (float)cdata.m_RGBA[2] / 255.0f; }
	inline float aF() const	{ return (float)cdata.m_RGBA[3] / 255.0f; }

	inline obColor fade(obuint8 _a) const { obColor c(cdata.m_RGBAi); c.cdata.m_RGBA[3]=_a; return c; }

	static obColor FromFloat(float _r, float _g, float _b, float _a = 1)
	{
		obColor c;
		c.cdata.m_RGBA[0] = (obuint8)(_r * 255.f);
		c.cdata.m_RGBA[1] = (obuint8)(_g * 255.f);
		c.cdata.m_RGBA[2] = (obuint8)(_b * 255.f);
		c.cdata.m_RGBA[3] = (obuint8)(_a * 255.f);// 255 is opaque, 0 is transparent
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

	inline obint32 rgba() const { return cdata.m_RGBAi; }
	inline obint32 argb() const { return obColor( a(), r(), g(), b() ); }
private:
	union cdatatype
	{
		obuint8		m_RGBA[4];
		obint32		m_RGBAi;
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
