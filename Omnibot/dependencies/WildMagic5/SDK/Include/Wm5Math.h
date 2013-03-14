// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/10/01)

#ifndef WM5MATH_H
#define WM5MATH_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Assert.h"
#include <cfloat>
#include <cmath>
#include <cstdlib>

// TODO.  Provide SIMD versions of the implementations for whatever platforms
// we support.

namespace Wm5
{
	template <typename Real>
	class Math
	{
	public:
		// Wrappers to hide implementations of functions.  The ACos and ASin
		// functions clamp the input argument to [-1,1] to avoid NaN issues
		// when the input is slightly larger than 1 or slightly smaller than -1.
		// Other functions have the potential for using a fast and approximate
		// algorithm rather than calling the standard math library functions.
		static Real ACos (Real value);
		static Real ASin (Real value);
		static Real ATan (Real value);
		static Real ATan2 (Real y, Real x);
		static Real Ceil (Real value);
		static Real Cos (Real value);
		static Real Exp (Real value);
		static Real FAbs (Real value);
		static Real Floor (Real value);
		static Real FMod (Real x, Real y);
		static Real InvSqrt (Real value);
		static Real Log (Real value);
		static Real Log2 (Real value);
		static Real Log10 (Real value);
		static Real Pow (Real base, Real exponent);
		static Real Sin (Real value);
		static Real Sqr (Real value);
		static Real Sqrt (Real value);
		static Real Tan (Real value);

		// Return -1 if the input is negative, 0 if the input is zero, and +1
		// if the input is positive.
		static int Sign (int value);
		static Real Sign (Real value);

		// Generate a random number in [0,1].  The random number generator may
		// be seeded by a first call to UnitRandom with a positive seed.
		static Real UnitRandom (unsigned int seed = 0);

		// Generate a random number in [-1,1].  The random number generator may
		// be seeded by a first call to SymmetricRandom with a positive seed.
		static Real SymmetricRandom (unsigned int seed = 0);

		// Generate a random number in [min,max].  The random number generator may
		// be seeded by a first call to IntervalRandom with a positive seed.
		static Real IntervalRandom (Real min, Real max, unsigned int seed = 0);

		// Clamp the input to the specified interval [min,max].
		static Real Clamp (Real value, Real minValue, Real maxValue);

		// Clamp the input to [0,1].
		static Real Saturate (Real value);

		// Fast evaluation of trigonometric and inverse trigonometric functions
		// using polynomial approximations.  The speed ups vary with processor.

		// The input must be in [0,pi/2].
		static Real FastSin0 (Real angle);
		static Real FastSin1 (Real angle);

		// The input must be in [0,pi/2]
		static Real FastCos0 (Real angle);
		static Real FastCos1 (Real angle);

		// The input must be in [0,pi/4].
		static Real FastTan0 (Real angle);
		static Real FastTan1 (Real angle);

		// The input must be in [0,1].
		static Real FastInvSin0 (Real value);
		static Real FastInvSin1 (Real value);

		// The input must be in [0,1].
		static Real FastInvCos0 (Real value);
		static Real FastInvCos1 (Real value);

		// The input must be in [-1,1].
		static Real FastInvTan0 (Real value);
		static Real FastInvTan1 (Real value);

		// Fast approximations to exp(-x).  The input x must be in [0,infinity).
		static Real FastNegExp0 (Real value);
		static Real FastNegExp1 (Real value);
		static Real FastNegExp2 (Real value);
		static Real FastNegExp3 (Real value);

		// Common constants.
		WM5_MATHEMATICS_ITEM static const Real EPSILON;
		WM5_MATHEMATICS_ITEM static const Real ZERO_TOLERANCE;
		WM5_MATHEMATICS_ITEM static const Real MAX_REAL;
		WM5_MATHEMATICS_ITEM static const Real PI;
		WM5_MATHEMATICS_ITEM static const Real TWO_PI;
		WM5_MATHEMATICS_ITEM static const Real HALF_PI;
		WM5_MATHEMATICS_ITEM static const Real INV_PI;
		WM5_MATHEMATICS_ITEM static const Real INV_TWO_PI;
		WM5_MATHEMATICS_ITEM static const Real DEG_TO_RAD;
		WM5_MATHEMATICS_ITEM static const Real RAD_TO_DEG;
		WM5_MATHEMATICS_ITEM static const Real LN_2;
		WM5_MATHEMATICS_ITEM static const Real LN_10;
		WM5_MATHEMATICS_ITEM static const Real INV_LN_2;
		WM5_MATHEMATICS_ITEM static const Real INV_LN_10;
		WM5_MATHEMATICS_ITEM static const Real SQRT_2;
		WM5_MATHEMATICS_ITEM static const Real INV_SQRT_2;
		WM5_MATHEMATICS_ITEM static const Real SQRT_3;
		WM5_MATHEMATICS_ITEM static const Real INV_SQRT_3;

		///////////////////////////////////////////////////////////////////////////
		// Custom functions
		static int Round (Real fValue);
		static Real DegToRad(Real _a);
		static Real RadToDeg(Real _a);
		static int IntervalRandomInt (int iMin, int iMax, unsigned int uiSeed = 0);
		static Real UnitCircleNormalize( Real fRadians );
		static void SinCos(Real a, Real &s, Real &c);
		///////////////////////////////////////////////////////////////////////////
	};

#include "Wm5Math.inl"

	typedef Math<float> Mathf;
	typedef Math<double> Mathd;
}

#endif
