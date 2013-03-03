///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains axes definition.
 *	\file		IceAxes.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEAXES_H__
#define __ICEAXES_H__

	enum PointComponent
	{
		_X					= 0,
		_Y					= 1,
		_Z					= 2,
		_W					= 3,

		_FORCE_DWORD		= 0x7fffffff
	};

	enum AxisOrder
	{
		AXES_XYZ			= (_X)|(_Y<<2)|(_Z<<4),
		AXES_XZY			= (_X)|(_Z<<2)|(_Y<<4),
		AXES_YXZ			= (_Y)|(_X<<2)|(_Z<<4),
		AXES_YZX			= (_Y)|(_Z<<2)|(_X<<4),
		AXES_ZXY			= (_Z)|(_X<<2)|(_Y<<4),
		AXES_ZYX			= (_Z)|(_Y<<2)|(_X<<4),

		AXES_FORCE_DWORD	= 0x7fffffff
	};

	class ICEMATHS_API Axes
	{
		public:

		inline_			Axes(AxisOrder order)
						{
							mAxis0 = (order   ) & 3;
							mAxis1 = (order>>2) & 3;
							mAxis2 = (order>>4) & 3;
						}
		inline_			~Axes()		{}

				udword	mAxis0;
				udword	mAxis1;
				udword	mAxis2;
	};

#endif // __ICEAXES_H__
